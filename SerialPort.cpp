/*
 * Copyright 2020 C Moss G6AMU
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

// overlapped info .. https://msdn.microsoft.com/en-us/library/ff802693.aspx

#ifdef _MSC_VER
	#include "StdAfx.h"
#endif

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
//#include <Windows.hpp>
#include <initguid.h>
//#include "Dbt.h"
#include <setupapi.h>

#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifdef _DEBUG
	#ifdef _MSC_VER
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include "SerialPort.h"

#ifdef __BORLANDC__
	#pragma package(smart_init)
#endif

const GUID GUID_DEVINTERFACE_COMPORT = {0x86E0D1E0, 0x8089, 0x11D0, {0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73}};	// NanoVNA V1

// ***********************************************************

void __fastcall GetLastErrorStr(DWORD err, char *err_str, int max_size)
{
	if (!err_str || max_size <= 0)
		return;

	memset(err_str, 0, max_size);

	char *buf = NULL;

	WORD prevErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);	// ignore critical errors

	HMODULE wnet_handle = ::GetModuleHandle(_T("wininet.dll"));

	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM;
	if (wnet_handle) flags |= FORMAT_MESSAGE_FROM_HMODULE;		// retrieve message from specified DLL

	DWORD res = ::FormatMessageA(flags, wnet_handle, err, 0, (LPSTR)&buf, 0, NULL);

	if (wnet_handle)
		::FreeLibrary(wnet_handle);

	if (buf != NULL)
	{
		#if (__BORLANDC__ < 0x0600)
			if (res > 0)
				sprintf(err_str, "[%d] %s", err, buf);
		#else
			if (res > 0)
				sprintf_s(err_str, max_size, "[%d] %s", err, buf);
		#endif

		::LocalFree(buf);
	}

	::SetErrorMode(prevErrorMode);
}

// ********************************************

CSerialPort::CSerialPort()
{
	m_last_error = ERROR_SUCCESS;
	memset(m_last_error_str, 0, sizeof(m_last_error_str));

	memset(&m_rx.overlapped, 0, sizeof(OVERLAPPED));
	memset(&m_tx.overlapped, 0, sizeof(OVERLAPPED));
	m_rx.overlapped.hEvent  = INVALID_HANDLE_VALUE;
	m_tx.overlapped.hEvent  = INVALID_HANDLE_VALUE;
	m_rx.overlapped_waiting = false;
	m_tx.overlapped_waiting = false;

	m_device_handle = INVALID_HANDLE_VALUE;

	GetSerialPortList();

	memset(&m_dcb, 0, sizeof(m_dcb));
	m_dcb.DCBlength     = sizeof(m_dcb);
	m_dcb.BaudRate      = 115200;
	m_dcb.fBinary       = true;
	m_dcb.fParity       = false;
	m_dcb.fOutxCtsFlow  = false;
	m_dcb.fOutxDsrFlow  = false;
	m_dcb.fDtrControl   = DTR_CONTROL_ENABLE;	// DTR_CONTROL_DISABLE, DTR_CONTROL_ENABLE, DTR_CONTROL_HANDSHAKE
//	m_dcb.fDsrSensitivity:1;   // DSR sensitivity
//	m_dcb.fTXContinueOnXoff:1; // XOFF continues Tx
	m_dcb.fOutX         = false;
	m_dcb.fInX          = false;
//	m_dcb.fErrorChar: 1;       // enable error replacement
	m_dcb.fNull         = false;
	m_dcb.fRtsControl   = RTS_CONTROL_ENABLE;	// RTS_CONTROL_DISABLE, RTS_CONTROL_ENABLE, RTS_CONTROL_HANDSHAKE
	m_dcb.fAbortOnError = false;
//	m_dcb.fDummy2:17;          // reserved
//	m_dcb.XonLim;
//	m_dcb.XoffLim;
	m_dcb.ByteSize      = 8;	// 8 bits per byte - when was it ever any different ??????
	m_dcb.Parity        = NOPARITY;
	m_dcb.StopBits      = ONESTOPBIT;
//	m_dcb.XonChar;
//	m_dcb.XoffChar;
//	m_dcb.ErrorChar;
//	m_dcb.EofChar;
//	m_dcb.EvtChar;

	memset(&m_timeouts, 0, sizeof(m_timeouts));
	m_timeouts.ReadIntervalTimeout           = MAXDWORD;
	m_timeouts.ReadTotalTimeoutMultiplier    = 0;
	m_timeouts.ReadTotalTimeoutConstant      = 0;
	m_timeouts.WriteTotalTimeoutMultiplier   = 0;
	m_timeouts.WriteTotalTimeoutConstant     = 0;

	m_rx.queue_size = 65536;
	m_tx.queue_size = 65536;

	m_buffer.resize(m_rx.queue_size);

	m_rx.buffer.resize(1000000);
	m_rx.buffer_rd = 0;
	m_rx.buffer_wr = 0;

	m_tx.buffer.resize(1000000);
	m_tx.buffer_rd = 0;
	m_tx.buffer_wr = 0;

	m_rx.overlapped_waiting = false;
	m_tx.overlapped_waiting = false;

	m_thread = new CSerialPortThread(&threadProcess);
}

CSerialPort::~CSerialPort()
{
	if (m_thread != NULL)
	{
		m_thread->Terminate();
		m_thread->WaitFor();
		delete m_thread;
		m_thread = NULL;
	}

	Disconnect();
}

void __fastcall CSerialPort::clearErrors()
{
	CCriticalSection cs(m_errors.cs);
	m_errors.list.resize(0);
}

void __fastcall CSerialPort::pushError(const DWORD error, SERIALPORT_STRING leading_text)
{
	CCriticalSection cs(m_errors.cs);

	m_last_error = error;
	GetLastErrorStr(error, m_last_error_str, sizeof(m_last_error_str));

	t_serial_port_error err = {error, leading_text + SERIALPORT_STRING(m_last_error_str)};
	m_errors.list.push_back(err);
}

unsigned int __fastcall CSerialPort::errorCount()
{
//	CCriticalSection cs(m_errors.cs);
	return m_errors.list.size();
}

t_serial_port_error __fastcall CSerialPort::pullError()
{
	t_serial_port_error err = {ERROR_SUCCESS, ""};

	CCriticalSection cs(m_errors.cs);

	if (!m_errors.list.empty())
	{
		err = m_errors.list[0];
		m_errors.list.erase(m_errors.list.begin() + 0);
	}

	return err;
}

void __fastcall CSerialPort::getState()
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return;

	if (::GetCommModemStatus(m_device_handle, &m_modem_state_read) == FALSE)
		pushError(::GetLastError(), "GetCommModemStatus ");

	if (::GetCommState(m_device_handle, &m_dcb) == FALSE)
		pushError(::GetLastError(), "GetCommState ");

	if (::ClearCommError(m_device_handle, &m_errors_read, &m_stat_read) == FALSE)
		pushError(::GetLastError(), "ClearCommError ");
}

void __fastcall CSerialPort::GetSerialPortList()
{  // enumerate the com-ports - non-WMI
	#define MAX_KEY_LENGTH 255
	#define MAX_VALUE_NAME 16383

	DWORD err;

	m_serialPortList.resize(0);

	HKEY hKey = NULL;

	err = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &hKey);
	if (err != ERROR_SUCCESS || hKey == NULL)
	{
		pushError(err, "reg ");
		if (hKey != NULL)
			::RegCloseKey(hKey);
		return;
	}

//	TCHAR    achKey[MAX_KEY_LENGTH];		// buffer for subkey name
//	DWORD    cbName;							// size of name string
	char     achClass[MAX_PATH];			// buffer for class name
	DWORD    cchClassName = MAX_PATH;	// size of class string
	DWORD    cSubKeys = 0;					// number of subkeys
	DWORD    cbMaxSubKey;					// longest subkey size
	DWORD    cchMaxClass;					// longest class string
	DWORD    cValues;							// number of values for key
	DWORD    cchMaxValue;					// longest value name
	DWORD    cbMaxValueData;				// longest value data
	DWORD    cbSecurityDescriptor;		// size of security descriptor
	FILETIME ftLastWriteTime;				// last write time

	char achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;

	// get the class name and the value count
	err = ::RegQueryInfoKeyA(
				hKey,                    // key handle
				achClass,                // buffer for class name
				&cchClassName,           // size of class string
				NULL,                    // reserved
				&cSubKeys,               // number of subkeys
				&cbMaxSubKey,            // longest subkey size
				&cchMaxClass,            // longest class string
				&cValues,                // number of values for this key
				&cchMaxValue,            // longest value name
				&cbMaxValueData,         // longest value data
				&cbSecurityDescriptor,   // security descriptor
				&ftLastWriteTime);       // last write time
	if (err != ERROR_SUCCESS)
	{
		pushError(err, "reg ");
		::RegCloseKey(hKey);
		return;
	}

	if (cValues <= 0)
	{	// no serial ports
		::RegCloseKey(hKey);
		return;
	}

	for (int i = 0; i < (int)cValues; i++)
	{
		cchValue = MAX_VALUE_NAME;
		achValue[0] = '\0';

		err = ::RegEnumValueA(hKey, i, achValue, &cchValue, NULL, NULL, NULL, NULL);
		if (err != ERROR_SUCCESS)
		{
			pushError(err, "reg ");
			break;
//			continue;
		}

		DWORD type;
		char str[MAX_PATH];
		DWORD size = sizeof(str);

		err = ::RegQueryValueExA(hKey, achValue, NULL, &type, (BYTE *)str, &size);
		if (err != ERROR_SUCCESS)
		{
			pushError(err, "reg ");
			break;
//			continue;
		}

		// check to see if we already have it
		unsigned int k = 0;
		while (k < m_serialPortList.size())
		{
			if (SERIALPORT_STRING(m_serialPortList[k].name).LowerCase() == SERIALPORT_STRING(str).LowerCase())
				break;	// already in the list
			k++;
		}

		// save it into our list if it's not already there
		if (k >= m_serialPortList.size())
		{
			T_SerialPortInfo spi;
			spi.name     = SERIALPORT_STRING(str);
			spi.deviceID = SERIALPORT_STRING(achValue);
			spi.vid      = -1;
			spi.pid      = -1;
			m_serialPortList.push_back(spi);
		}
	}

	::RegCloseKey(hKey);

	// ****************************
	// fetch the VID/PID (if any)

	//const GUID guid = GUID_DEVINTERFACE_USB_DEVICE;
	const GUID guid = GUID_DEVINTERFACE_COMPORT;

	//HDEVINFO dev_info = ::SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
	HDEVINFO dev_info = ::SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);

	if (dev_info != INVALID_HANDLE_VALUE)
	{
		int devIndex = -1;
		std::vector <uint8_t> buffer;

		while (true)
		{
			BOOL res;
			char str[512];

			struct
			{
				int index;
				SERIALPORT_STRING path;
				SERIALPORT_STRING description;
				//SERIALPORT_STRING enumerator_name;
				SERIALPORT_STRING friendly_name;
				//SERIALPORT_STRING location_information;
				SERIALPORT_STRING mfg;
				SERIALPORT_STRING service;
				int vid;
				int pid;
			} usb_device;

			usb_device.index = ++devIndex;
			usb_device.vid   = -1;
			usb_device.pid   = -1;

			SP_INTERFACE_DEVICE_DATA ifData;
			memset(&ifData, 0, sizeof(SP_INTERFACE_DEVICE_DATA));
			ifData.cbSize = sizeof(ifData);

			res = ::SetupDiEnumDeviceInterfaces(dev_info, NULL, &guid, devIndex, &ifData);
			if (!res)
			{	// not found
				::SetupDiDestroyDeviceInfoList(dev_info);
				//dev_info = NULL;
				break;
			}

			SP_DEVINFO_DATA did;
			memset(&did, 0, sizeof(SP_DEVINFO_DATA));
			did.cbSize = sizeof(SP_DEVINFO_DATA);

			DWORD needed = 0;
			::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, NULL, 0, &needed, &did);
			//res = ::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, NULL, 0, &needed, NULL);
			if (needed <= 0)
				continue;

			buffer.resize(needed);
			memset(&buffer[0], 0, needed);

			SP_INTERFACE_DEVICE_DETAIL_DATA *detail = (SP_INTERFACE_DEVICE_DETAIL_DATA *)&buffer[0];
			detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

			//SP_DEVINFO_DATA did;
			//memset(&did, 0, sizeof(SP_DEVINFO_DATA));
			//did.cbSize = sizeof(SP_DEVINFO_DATA);

			//res = ::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, detail, needed, NULL, &did);
			res = ::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, detail, needed, NULL, NULL);
			if (!res)
				continue;

			usb_device.path = SERIALPORT_STRING(detail->DevicePath);

			memset(str, 0, sizeof(str));
			res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_DEVICEDESC, NULL, (PBYTE)str, sizeof(str), NULL);
			if (res)
				usb_device.description = SERIALPORT_STRING(str);

			//memset(str, 0, sizeof(str));
			//res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_ENUMERATOR_NAME, NULL, str, sizeof(str), NULL);
			//if (res)
			//	usb_device.enumerator_name = SERIALPORT_STRING(str);

			memset(str, 0, sizeof(str));
			res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_FRIENDLYNAME, NULL, (PBYTE)str, sizeof(str), NULL);
			if (res)
				usb_device.friendly_name = SERIALPORT_STRING(str);

			//memset(str, 0, sizeof(str));
			//res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_LOCATION_INFORMATION, NULL, str, sizeof(str), NULL);
			//if (res)
			//	usb_device.location_information = SERIALPORT_STRING(str);

			memset(str, 0, sizeof(str));
			res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_MFG, NULL, (PBYTE)str, sizeof(str), NULL);
			if (res)
				usb_device.mfg = SERIALPORT_STRING(str);

			memset(str, 0, sizeof(str));
			res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_SERVICE, NULL, (PBYTE)str, sizeof(str), NULL);
			if (res)
				usb_device.service = SERIALPORT_STRING(str);

			if (!usb_device.path.IsEmpty())
			{
				int i;
				SERIALPORT_STRING s = usb_device.path.LowerCase();

				i = s.Pos("vid_");
				if (i > 0)
				{
					SERIALPORT_STRING str = s.SubString(i + 4, 5);
					i = str.Pos('&');
					if (i > 0)
						str = str.SubString(1, i - 1);
					if (!TryStrToInt("0x" + str, usb_device.vid))
						usb_device.vid = -1;
				}

				i = s.Pos("pid_");
				if (i > 0)
				{
					SERIALPORT_STRING str = s.SubString(i + 4, 5);
					i = str.Pos('#');
					if (i > 0)
						str = str.SubString(1, i - 1);
					if (!TryStrToInt("0x" + str, usb_device.pid))
						usb_device.pid = -1;
				}
			}

			// device path: \\?\usb#vid_0483&pid_5740#400#{86e0d1e0-8089-11d0-9ce4-08003e301f73}
			// device name: STMicroelectronics Virtual COM Port (COM25)
			//
			// device path: \\?\ports#vid_04b4&pid_0008#demo#{86e0d1e0-8089-11d0-9ce4-08003e301f73}
			// device name: USB Serial Port (COM12)

			if (usb_device.vid >= 0 && usb_device.pid >= 0)
			{
				for (unsigned int i = 0; i < m_serialPortList.size(); i++)
				{
					T_SerialPortInfo *spi = &m_serialPortList[i];
					//SERIALPORT_STRING s = "(" + spi->name + ")";
					SERIALPORT_STRING s = spi->name;
					if (usb_device.friendly_name.LowerCase().Pos(s.LowerCase()) > 0)
					{	// found it
						spi->description   = usb_device.description;
						spi->path          = usb_device.path;
						spi->friendly_name = usb_device.friendly_name;
						spi->mfg           = usb_device.mfg;
						spi->service       = usb_device.service;
						spi->vid           = usb_device.vid;
						spi->pid           = usb_device.pid;
						break;
					}
				}
			}
			//pushCommMessage("    device path: " + usb_device.path);
			//pushCommMessage("    device desc: " + usb_device.description);
			//pushCommMessage("    device enum: " + usb_device.enumerator_name);
			//pushCommMessage("    device name: " + usb_device.friendly_name);
			//pushCommMessage("device loc info: " + usb_device.location_information);
			//pushCommMessage("     device mfg: " + usb_device.mfg);
			//pushCommMessage(" device service: " + usb_device.service);
		}
	}

	// ****************************
}

void __fastcall CSerialPort::GetSerialPortList(std::vector <T_SerialPortInfo> &serialPortList)
{
	GetSerialPortList();

	serialPortList.clear();

	for (int i = 0; i < (int)m_serialPortList.size(); i++)
		serialPortList.push_back(m_serialPortList[i]);
}

bool __fastcall CSerialPort::Connected()
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return false;

	if (::ClearCommError(m_device_handle, &m_errors_read, &m_stat_read) == FALSE)
	{	// close the comm port
		pushError(::GetLastError(), "connected ClearCommError ");

//		if (m_last_error == ERROR_ACCESS_DENIED)
//		{	// tends to mean they have unplugged the USB serial port, or we're no longer connected
//		}

		Disconnect();
	}

	return (m_device_handle != INVALID_HANDLE_VALUE) ? true : false;
}

int __fastcall CSerialPort::Connect(SERIALPORT_STRING device_name)
{
	HANDLE device_handle;

	if (m_device_handle != INVALID_HANDLE_VALUE)
		Disconnect();

	clearErrors();

	memset(&m_rx.overlapped, 0, sizeof(m_rx.overlapped));
	memset(&m_tx.overlapped, 0, sizeof(m_tx.overlapped));
	m_rx.overlapped.hEvent  = INVALID_HANDLE_VALUE;
	m_tx.overlapped.hEvent  = INVALID_HANDLE_VALUE;
	m_rx.overlapped_waiting = false;
	m_tx.overlapped_waiting = false;

	m_rx.buffer_rd = 0;
	m_rx.buffer_wr = 0;

	m_tx.buffer_rd = 0;
	m_tx.buffer_wr = 0;

	if (device_name.IsEmpty())
		return -1;

	m_device_name = device_name;

	SERIALPORT_STRING name = SERIALPORT_STRING("\\\\.\\") + device_name;
	#if defined(__BORLANDC__)
		#ifdef SERIAL_OVERLAPPED
			device_handle = ::CreateFileA(AnsiString(name).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);
		#else
			device_handle = ::CreateFileA(AnsiString(name).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		#endif
	#elif defined(_MSC_VER)
		#ifdef SERIAL_OVERLAPPED
			device_handle = ::CreateFileA(name.GetString(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);
		#else
			device_handle = ::CreateFileA(name.GetString(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		#endif
	#else
		#error "FIX ME"
	#endif

	if (device_handle == INVALID_HANDLE_VALUE)
	{
		pushError(::GetLastError(), "connect CreateFile ");
		return m_last_error;
	}

	if (::GetCommState(device_handle, &m_original_dcb) == FALSE)
		pushError(::GetLastError(), "connect GetCommState ");

	if (::GetCommTimeouts(device_handle, &m_original_timeouts) == FALSE)
		pushError(::GetLastError(), "connect GetCommTimeouts ");

	if (::GetCommModemStatus(device_handle, &m_modem_state_read) == FALSE)
		pushError(::GetLastError(), "connect GetCommModemStatus ");

	if (::SetCommState(device_handle, &m_dcb) == FALSE)
		pushError(::GetLastError(), "connect SetCommState ");

	if (::SetCommTimeouts(device_handle, &m_timeouts) == FALSE)
		pushError(::GetLastError(), "connect SetCommTimeouts ");

	if (::SetupComm(device_handle, m_rx.queue_size, m_tx.queue_size) == FALSE)
		pushError(::GetLastError(), "connect SetupComm ");

	#ifdef SERIAL_OVERLAPPED
		m_rx.overlapped.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if (m_rx.overlapped.hEvent == NULL || m_rx.overlapped.hEvent == INVALID_HANDLE_VALUE)
			pushError(::GetLastError(), "connect CreateEvent ");

		m_tx.overlapped.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if (m_tx.overlapped.hEvent == NULL || m_tx.overlapped.hEvent == INVALID_HANDLE_VALUE)
			pushError(::GetLastError(), "connect CreateEvent ");

		//if (::SetCommMask(m_device_handle, EV_TXEMPTY | EV_RXCHAR) == FALSE)
		//if (::SetCommMask(m_device_handle, EV_RXCHAR | EV_BREAK) == FALSE)
		//	pushError(::GetLastError(), "connect SetCommMask ");
	#endif

	// flush the Tx/Rx buffers
	::PurgeComm(device_handle, PURGE_RXABORT | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR);
	::ClearCommError(device_handle, &m_errors_read, &m_stat_read);

	m_device_handle = device_handle;

	return ERROR_SUCCESS;	// OK
}

void __fastcall CSerialPort::Disconnect()
{
	const HANDLE handle = m_device_handle;
	m_device_handle = INVALID_HANDLE_VALUE;	// stops the thread accessing the serial port

	m_rx.buffer_rd = 0;
	m_rx.buffer_wr = 0;

	m_tx.buffer_rd = 0;
	m_tx.buffer_wr = 0;

	if (m_rx.overlapped.hEvent != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_rx.overlapped.hEvent);
		memset(&m_rx.overlapped, 0, sizeof(m_rx.overlapped));
		m_rx.overlapped.hEvent = INVALID_HANDLE_VALUE;
	}

	if (m_tx.overlapped.hEvent != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_tx.overlapped.hEvent);
		memset(&m_tx.overlapped, 0, sizeof(m_tx.overlapped));
		m_tx.overlapped.hEvent = INVALID_HANDLE_VALUE;
	}

	if (handle != INVALID_HANDLE_VALUE)
	{
		::PurgeComm(handle, PURGE_RXABORT | PURGE_RXCLEAR);
		::ClearCommError(handle, NULL, NULL);
		::SetCommState(handle, &m_original_dcb);
		::SetCommTimeouts(handle, &m_original_timeouts);
		::ClearCommError(handle, NULL, NULL);
		::CloseHandle(handle);
	}
}

void __fastcall CSerialPort::processTx()
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return;

	DWORD bytes_written = 0;

	const int wr = m_tx.buffer_wr;
	int rd       = m_tx.buffer_rd;
	int len      = (wr >= rd) ? wr - rd : m_tx.buffer.size() - rd;

	if (m_tx.overlapped.hEvent == INVALID_HANDLE_VALUE)
	{	// non-overlapped

		if (len > 0)
		{
			if (::WriteFile(m_device_handle, &m_tx.buffer[rd], len, &bytes_written, NULL) == FALSE)
			{
				pushError(::GetLastError(), "tx WriteFile ");
				//::ClearCommBreak(m_device_handle);
				::ClearCommError(m_device_handle, &m_errors_read, &m_stat_read);
				return;
			}
		}
	}
	else
	{	// overlapped

		if (!m_tx.overlapped_waiting)
		{
			if (len > 0)
			{
				if (::WriteFile(m_device_handle, &m_tx.buffer[rd], len, &bytes_written, &m_tx.overlapped) == FALSE)
				{
					const DWORD error = ::GetLastError();
					if (error != ERROR_IO_PENDING)
					{	// error
						//::ClearCommBreak(m_device_handle);
						::ClearCommError(m_device_handle, &m_errors_read, &m_stat_read);
						pushError(error, "tx overlapped WriteFile ");
						return;
					}
					m_tx.overlapped_waiting = true;
				}
			}
		}

		if (m_tx.overlapped_waiting)
		{
			if (::GetOverlappedResult(m_device_handle, &m_tx.overlapped, &bytes_written, FALSE) == FALSE)
			{
				const DWORD error = ::GetLastError();
				::ClearCommError(m_device_handle, &m_errors_read, &m_stat_read);
				//if (error == ERROR_SEM_TIMEOUT)
				//	m_tx.overlapped_waiting = false;
				if (error != ERROR_IO_INCOMPLETE)
				{
					pushError(error, "tx GetOverlappedResult ");
					m_rx.overlapped_waiting = false;
				}
			}
			else
				m_tx.overlapped_waiting = false;
		}
	}

	if (bytes_written > 0)
	{
		rd += bytes_written;
		if (rd >= (int)m_tx.buffer.size())
			rd -= m_tx.buffer.size();
		m_tx.buffer_rd = rd;
	}
}

void __fastcall CSerialPort::processRx()
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return;

	DWORD bytes_available = 0;
	DWORD bytes_read      = 0;

	if (::ClearCommError(m_device_handle, &m_errors_read, &m_stat_read) == FALSE)
		pushError(::GetLastError(), "rx ClearCommError ");
	else
		bytes_available = m_stat_read.cbInQue;

	if (m_buffer.size() < bytes_available)
		m_buffer.resize(bytes_available * 4);

	if (m_rx.overlapped.hEvent == INVALID_HANDLE_VALUE)
	{	// non-overlapped
		if (bytes_available > 0)
		{
			if (!m_buffer.empty())
			{
				if (::ReadFile(m_device_handle, &m_buffer[0], m_buffer.size(), &bytes_read, NULL) == FALSE)
//				if (::ReadFile(m_device_handle, &m_buffer[0], bytes_available, &bytes_read, NULL) == FALSE)
				{
					const DWORD err = ::GetLastError();
//					if (err != ERROR_INVALID_PARAMETER)
						pushError(err, "rx ReadFile ");
					::ClearCommError(m_device_handle, &m_errors_read, &m_stat_read);
					return;
				}
			}
		}
	}
	else
	{	// overlapped

		if (!m_rx.overlapped_waiting)
		{
			if (bytes_available > 0)
			{
				if (!m_buffer.empty())
				{
					if (::ReadFile(m_device_handle, &m_buffer[0], m_buffer.size(), &bytes_read, &m_rx.overlapped) == FALSE)
					{
						const DWORD error = ::GetLastError();
						if (error != ERROR_IO_PENDING)
						{
							::ClearCommError(m_device_handle, &m_errors_read, &m_stat_read);
							pushError(error, "rx overlapped ReadFile ");
							return;
						}

						m_rx.overlapped_waiting = true;
					}
				}
			}
		}

		if (m_rx.overlapped_waiting)
		{
			if (::GetOverlappedResult(m_device_handle, &m_rx.overlapped, &bytes_read, FALSE) == FALSE)
			{
				const DWORD error = ::GetLastError();
				::ClearCommError(m_device_handle, &m_errors_read, &m_stat_read);
				//if (error == ERROR_SEM_TIMEOUT)
				//	m_rx.overlapped_waiting = false;
				if (error != ERROR_IO_INCOMPLETE)
				{
					pushError(error, "rx GetOverlappedResult ");
					m_rx.overlapped_waiting = false;
				}
			}
			else
				m_rx.overlapped_waiting = false;
		}
	}

	if (bytes_read > 0)
	{	// copy the received bytes into our rx buffer
		int loops = 0;
		int rd = 0;
		while (m_thread != NULL && loops++ < 100 && rd < (int)bytes_read)
		{
			int buf_rd = m_rx.buffer_rd;
			int buf_wr = m_rx.buffer_wr;
			int len = (buf_wr >= buf_rd) ? (int)m_rx.buffer.size() - buf_wr : buf_rd - buf_wr;
			if (len > ((int)bytes_read - rd))
				len = (int)bytes_read - rd;
			if (len > 0)
			{
				memcpy(&m_rx.buffer[buf_wr], &m_buffer[rd], len);
				rd += len;
				buf_wr += len;
				if (buf_wr >= (int)m_rx.buffer.size())
					buf_wr -= m_rx.buffer.size();
				m_rx.buffer_wr = buf_wr;
			}
			else
			{	// wait a bit for the exec to free up some buffer space
				m_thread->Sleep(1);
			}
		}
	}
}

void __fastcall CSerialPort::threadProcess()
{
	if (m_device_handle == INVALID_HANDLE_VALUE || m_thread == NULL)
		return;

	processTx();
	processRx();
}

int __fastcall CSerialPort::RxBytesAvailable()
{	// return number of available received bytes
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return -1;

	const int wr = m_rx.buffer_wr;
	const int rd = m_rx.buffer_rd;
	return (wr >= rd) ? wr - rd : (m_rx.buffer.size() - rd) + wr;
}

int __fastcall CSerialPort::RxByte()
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return -1;

	const int wr = m_rx.buffer_wr;

	int rd = m_rx.buffer_rd;
	if (rd == wr)
		return -2;

	const int i = m_rx.buffer[rd];
	if (++rd >= (int)m_rx.buffer.size())
		rd -= m_rx.buffer.size();

	m_rx.buffer_rd = rd;

	return i;
}

int __fastcall CSerialPort::RxBytePeek()
{	// fetch a byte without removing it from the rx buffer
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return -1;

	const int wr = m_rx.buffer_wr;

	int rd = m_rx.buffer_rd;
	if (rd == wr)
		return -2;

	return (int)m_rx.buffer[rd];
}

int __fastcall CSerialPort::RxBytes(void *buf, int size)
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return -1;

	uint8_t *buffer = (uint8_t *)buf;
	int bytes_written = 0;

	while (bytes_written < size)
	{
		int buf_wr = m_rx.buffer_wr;
		int buf_rd = m_rx.buffer_rd;
		int len = (buf_wr >= buf_rd) ? buf_wr - buf_rd : m_rx.buffer.size() - buf_rd;
		if (len > (size - bytes_written))
			len = size - bytes_written;
		if (len <= 0)
			break;
		memcpy(&buffer[bytes_written], &m_rx.buffer[buf_rd], len);
		bytes_written += len;
		buf_rd        += len;
		if (buf_rd >= (int)m_rx.buffer.size())
			buf_rd -= m_rx.buffer.size();
		m_rx.buffer_rd = buf_rd;
	}

	return bytes_written;
}

int __fastcall CSerialPort::RxBytesPeek(void *buf, int size)
{	// fetch bytes without removing them from the rx buffer
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return -1;

	uint8_t *buffer = (uint8_t *)buf;
	int bytes_written = 0;

	int buf_rd = m_rx.buffer_rd;

	while (bytes_written < size)
	{
		int buf_wr = m_rx.buffer_wr;
		int len = (buf_wr >= buf_rd) ? buf_wr - buf_rd : m_rx.buffer.size() - buf_rd;
		if (len > (size - bytes_written))
			len = size - bytes_written;
		if (len <= 0)
			break;
		memcpy(&buffer[bytes_written], &m_rx.buffer[buf_rd], len);
		bytes_written += len;
		buf_rd        += len;
		if (buf_rd >= (int)m_rx.buffer.size())
			buf_rd -= m_rx.buffer.size();
	}

	return bytes_written;
}

bool __fastcall CSerialPort::TxEmpty()
{
	return (m_device_handle == INVALID_HANDLE_VALUE || m_tx.buffer_rd == m_tx.buffer_wr) ? true : false;
}

int __fastcall CSerialPort::TxBytesWaiting()
{	// return number of queued transmit bytes
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return -1;

	const int rd = m_tx.buffer_rd;
	const int wr = m_tx.buffer_wr;
	const int bytes_available = (wr >= rd) ? wr - rd : (m_tx.buffer.size() - rd) + wr;

	return bytes_available;
}

int __fastcall CSerialPort::TxByte(const int b)
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return -1;

	const int buf_rd = m_tx.buffer_rd;
	int       buf_wr = m_tx.buffer_wr;
	int space_available = (buf_wr >= buf_rd) ? (int)m_tx.buffer.size() - (buf_wr - buf_rd) : buf_rd - buf_wr;
	if (space_available > 0)
		space_available--;
	if (space_available < 1)
		return 0;

	m_tx.buffer[buf_wr] = (uint8_t)b;
	if (++buf_wr >= (int)m_tx.buffer.size())
		buf_wr -= m_tx.buffer.size();

	m_tx.buffer_wr = buf_wr;

	return 1;
}

int __fastcall CSerialPort::TxByte(const uint8_t b)
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return -1;

	const int buf_rd = m_tx.buffer_rd;
	int       buf_wr = m_tx.buffer_wr;

	int space_available = (buf_wr >= buf_rd) ? (int)m_tx.buffer.size() - (buf_wr - buf_rd) - 1 : buf_rd - buf_wr - 1;
	if (space_available <= 0)
		return 0;

	m_tx.buffer[buf_wr] = b;
	if (++buf_wr >= (int)m_tx.buffer.size())
		buf_wr -= m_tx.buffer.size();

	m_tx.buffer_wr = buf_wr;

	return 1;
}

int __fastcall CSerialPort::TxBytes(const void *buf, int bytes)
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return -1;

	if (buf == NULL || bytes <= 0)
		return 0;

	uint8_t *buffer = (uint8_t *)buf;

	const int buf_rd = m_tx.buffer_rd;
	int       buf_wr = m_tx.buffer_wr;

	int space_available = (buf_wr >= buf_rd) ? (int)m_tx.buffer.size() - (buf_wr - buf_rd) - 1 : buf_rd - buf_wr - 1;
	if (space_available <= 0)
		return 0;

	if (bytes > space_available)
		bytes = space_available;

	int bytes_written = 0;

	while (bytes_written < bytes)
	{
		int len = (buf_wr >= buf_rd) ? (int)m_tx.buffer.size() - buf_wr : buf_rd - buf_wr;
		if (len > (bytes - bytes_written))
			len = bytes - bytes_written;
		memcpy(&m_tx.buffer[buf_wr], &buffer[bytes_written], len);
		bytes_written += len;
		buf_wr        += len;
		if (buf_wr >= (int)m_tx.buffer.size())
			buf_wr -= m_tx.buffer.size();
	}

	m_tx.buffer_wr = buf_wr;

	return bytes_written;
}

int __fastcall CSerialPort::TxBytes(const char *s)
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return -1;

	if (s == NULL)
		return 0;

	const int len = (int)strlen(s);
	if (len <= 0)
		return 0;

	return TxBytes(s, len);
}

int __fastcall CSerialPort::TxBytes(const SERIALPORT_STRING s)
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return -1;

	if (s.IsEmpty())
		return 0;

	#if defined(__BORLANDC__)
		return TxBytes(s.c_str(), s.Length());
	#elif defined(_MSC_VER)
		return TxBytes(s.GetString(), s.GetLength());
	#else
		#error "FIX ME"
	#endif
}

int __fastcall CSerialPort::GetBaudRate()
{
	getState();

	return m_dcb.BaudRate;
}

void __fastcall CSerialPort::SetBaudRate(int value)
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
	{	// not connected
		m_dcb.BaudRate = value;
		return;
	}

	if ((int)m_dcb.BaudRate == value)
		return;	// no change

	m_dcb.BaudRate = value;

	if (::SetCommState(m_device_handle, &m_dcb) == FALSE)
		pushError(::GetLastError(), "SetCommState ");

	getState();
}

void __fastcall CSerialPort::SetRTS(bool value)
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return;

	if (::EscapeCommFunction(m_device_handle, value ? SETRTS : CLRRTS) == FALSE)
		pushError(::GetLastError(), "EscapeCommFunction ");

	getState();
}

void __fastcall CSerialPort::SetDTR(bool value)
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return;

	if (::EscapeCommFunction(m_device_handle, value ? SETDTR : CLRDTR) == FALSE)
		pushError(::GetLastError(), "EscapeCommFunction ");

	getState();
}

bool __fastcall CSerialPort::GetCTS()
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return false;

	getState();

	return (m_modem_state_read & MS_CTS_ON) ? true : false;
}

bool __fastcall CSerialPort::GetDSR()
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return false;

	getState();

	return (m_modem_state_read & MS_DSR_ON) ? true : false;
}

bool __fastcall CSerialPort::GetRING()
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return false;

	getState();

	return (m_modem_state_read & MS_RING_ON) ? true : false;
}

bool __fastcall CSerialPort::GetRLSD()
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
		return false;

	getState();

	return (m_modem_state_read & MS_RLSD_ON) ? true : false;
}

int __fastcall CSerialPort::GetByteSize()
{
	getState();

	return m_dcb.ByteSize;
}

void __fastcall CSerialPort::SetByteSize(int value)
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
	{
		m_dcb.ByteSize = (BYTE)value;
		return;
	}

	if (m_dcb.ByteSize == (BYTE)value)
		return;

	m_dcb.ByteSize = (BYTE)value;
	if (::SetCommState(m_device_handle, &m_dcb) == FALSE)
		pushError(::GetLastError(), "SetCommState ");

	getState();
}

int __fastcall CSerialPort::GetParity()
{
	getState();

	return m_dcb.Parity;
}

void __fastcall CSerialPort::SetParity(int value)
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
	{
		m_dcb.Parity = (BYTE)value;
		return;
	}

	if (m_dcb.Parity == (BYTE)value)
		return;

	m_dcb.Parity = (BYTE)value;
	if (::SetCommState(m_device_handle, &m_dcb) == FALSE)
		pushError(::GetLastError(), "SetCommState ");

	getState();
}

int __fastcall CSerialPort::GetStopBits()
{
	getState();

	return m_dcb.StopBits;
}

void __fastcall CSerialPort::SetStopBits(int value)
{
	if (m_device_handle == INVALID_HANDLE_VALUE)
	{
		m_dcb.StopBits = (BYTE)value;
		return;
	}

	if (m_dcb.StopBits == (BYTE)value)
		return;

	m_dcb.StopBits = (BYTE)value;
	if (::SetCommState(m_device_handle, &m_dcb) == FALSE)
		pushError(::GetLastError(), "SetCommState ");

	getState();
}

// ********************************************

