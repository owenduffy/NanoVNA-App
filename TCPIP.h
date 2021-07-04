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

#ifndef TCPIPH
#define TCPIPH

#pragma once

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <Classes.hpp>
//#include <stdint.h>

#ifdef __BORLANDC__
	#include <system.hpp>	// for AnsiString
#endif

#include <windows.h>
#include <winsock2.h>
#include <mstcpip.h>
#include <ws2tcpip.h>
//#include <rpc.h>
//#include <ntdsapi.h>
//#include <tchar.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")
#pragma comment(lib, "fwpuclnt.lib")

#include <vector>

#if defined(__BORLANDC__)
	#define STRING  AnsiString
#elif defined(_MSC_VER)
	#define STRING  CString
#else
	#error "FIX ME"
#endif

typedef struct
{
	DWORD code;
	String str;
} t_tcpip_error;

class CTCPIPThread : public TThread
{
	typedef void __fastcall (__closure *tcpip_threadProcess)();
	private:
		tcpip_threadProcess m_process;
	protected:
		void __fastcall Execute()
		{
			while (!Terminated)
			{
				Sleep(1);
				if (m_process)
					m_process();
			}
			ReturnValue = 0;
		}
	public:
		__fastcall CTCPIPThread(tcpip_threadProcess process) : TThread(false)
		{
			Priority        = tpNormal;
			FreeOnTerminate = false;
			m_process       = process;
		}
		__fastcall virtual ~CTCPIPThread()
		{
			m_process = NULL;
		}
};

class CTCPIP
{
private:
	DWORD m_last_error;
	char  m_last_error_str[256];

	STRING m_ip_address;
	int    m_ip_port;

	std::vector <uint8_t> m_buffer;

	struct
	{
		HANDLE mutex;
		std::vector <t_tcpip_error> list;
	} m_errors;

	WSADATA m_wsaData;
	bool    m_winsock_ok;
	SOCKET  m_tcpip_socket;

	struct
	{
		std::vector <uint8_t> buffer;
		volatile int buffer_rd;
		volatile int buffer_wr;
	} m_rx;

	struct
	{
		std::vector <uint8_t> buffer;
		volatile int buffer_rd;
		volatile int buffer_wr;
	} m_tx;

	CTCPIPThread *m_thread;

	void __fastcall pushError(const DWORD error, STRING leading_text);

	STRING __fastcall GetLastErrorString()
	{
		return AnsiString(m_last_error_str);
	}

	void __fastcall disconnectSocket();

	bool __fastcall Connected();

	int __fastcall RxBytesAvailable();
	int __fastcall RxByte();
	int __fastcall RxBytePeek();

	bool __fastcall TxEmpty();
	int __fastcall TxBytesWaiting();
	int __fastcall TxByte(const int b);

	void __fastcall processTx();
	void __fastcall processRx();
	void __fastcall threadProcess();

public:
	CTCPIP();
	~CTCPIP();

	unsigned int __fastcall errorCount();
	void __fastcall clearErrors();
	t_tcpip_error __fastcall pullError();

	bool __fastcall Connect(AnsiString address, int port);
	void __fastcall Disconnect();

	int __fastcall RxBytes(void *buf, int size);
	int __fastcall RxBytesPeek(void *buf, int size);

	int __fastcall TxBytes(const void *buf, int bytes);
	int __fastcall TxBytes(const char *s);
	int __fastcall TxBytes(const STRING s);

	__property bool connected = {read = Connected};

	__property int rxBytesAvailable = {read = RxBytesAvailable};
	__property int rxByte           = {read = RxByte};
	__property int rxBytePeek       = {read = RxBytePeek};

	__property bool txEmpty         = {read = TxEmpty};
	__property int txBytesWaiting   = {read = TxBytesWaiting};
	__property int txByte           = {write = TxByte};
};

#endif
