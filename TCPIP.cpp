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

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#ifdef _MSC_VER
	#include "StdAfx.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdint.h>

//#include <netinet/tcp.h>

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

#include "TCPIP.h"

#ifdef __BORLANDC__
	#pragma package(smart_init)
#endif

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

CTCPIP::CTCPIP()
{
	m_thread = NULL;

	m_tcpip_socket = INVALID_SOCKET;

	m_errors.mutex = ::CreateMutex(NULL, TRUE, NULL);

	m_last_error = ERROR_SUCCESS;
	memset(m_last_error_str, 0, sizeof(m_last_error_str));

	m_buffer.resize(65536);

	m_rx.buffer.resize(65536 * 4);
	m_rx.buffer_rd = 0;
	m_rx.buffer_wr = 0;

	m_tx.buffer.resize(65536 * 4);
	m_tx.buffer_rd = 0;
	m_tx.buffer_wr = 0;

	m_winsock_ok = true;
	const int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (result != 0)
	{
		m_winsock_ok = false;
		ZeroMemory(&m_wsaData, sizeof(m_wsaData));
	}
	else
	if (LOBYTE(m_wsaData.wVersion) < 2 || (LOBYTE(m_wsaData.wVersion) == 2 && HIBYTE(m_wsaData.wVersion) < 2))
	{
		m_winsock_ok = false;
		WSACleanup();
		ZeroMemory(&m_wsaData, sizeof(m_wsaData));
	}

	if (m_winsock_ok)
		m_thread = new CTCPIPThread(&threadProcess);
}

CTCPIP::~CTCPIP()
{
	if (m_thread != NULL)
	{
		m_thread->Terminate();
		m_thread->WaitFor();
		delete m_thread;
		m_thread = NULL;
	}

	disconnectSocket();

	if (m_winsock_ok)
	{
		m_winsock_ok = false;
		WSACleanup();
	}

	if (m_errors.mutex != NULL)
	{
		::CloseHandle(m_errors.mutex);
		m_errors.mutex = NULL;
	}
}

void __fastcall CTCPIP::disconnectSocket()
{
	int result;

	if (m_tcpip_socket == INVALID_SOCKET)
		return;

	SOCKET tcpip_socket = m_tcpip_socket;
	m_tcpip_socket = INVALID_SOCKET;

	result = shutdown(tcpip_socket, SD_BOTH);
	if (result == SOCKET_ERROR)
	{
		const int err = WSAGetLastError();
		pushError(err, "disconnect_shutdown");
	}

	result = closesocket(tcpip_socket);
	if (result == SOCKET_ERROR)
	{
		const int err = WSAGetLastError();
		pushError(err, "disconnect_closesocket");
	}
}

void __fastcall CTCPIP::clearErrors()
{
	const DWORD res = ::WaitForSingleObject(m_errors.mutex, 100);
	m_errors.list.resize(0);
	if (res == WAIT_OBJECT_0)
		::ReleaseMutex(m_errors.mutex);
}

void __fastcall CTCPIP::pushError(const DWORD error, STRING leading_text)
{
	const DWORD res = ::WaitForSingleObject(m_errors.mutex, 10);

	m_last_error = error;
	GetLastErrorStr(error, m_last_error_str, sizeof(m_last_error_str));

	t_tcpip_error err = {error, leading_text + STRING(m_last_error_str)};
	m_errors.list.push_back(err);

	if (res == WAIT_OBJECT_0)
		::ReleaseMutex(m_errors.mutex);
}

unsigned int __fastcall CTCPIP::errorCount()
{
	const DWORD res = ::WaitForSingleObject(m_errors.mutex, 10);
	const unsigned int size = m_errors.list.size();
	if (res == WAIT_OBJECT_0)
		::ReleaseMutex(m_errors.mutex);
	return size;
}

t_tcpip_error __fastcall CTCPIP::pullError()
{
	t_tcpip_error err = {ERROR_SUCCESS, ""};

	const DWORD res = ::WaitForSingleObject(m_errors.mutex, 100);

	if (!m_errors.list.empty())
	{
		err = m_errors.list.[0];
		m_errors.list..erase(m_errors.list..begin() + 0);
	}

	if (res == WAIT_OBJECT_0)
		::ReleaseMutex(m_errors.mutex);

	return err;
}

bool __fastcall CTCPIP::Connected()
{
	return (m_tcpip_socket != INVALID_SOCKET) ? true : false;
}

bool __fastcall CTCPIP::Connect(AnsiString address, int port)
{
	int result;

	disconnectSocket();

	address = address.Trim();
	if (address.IsEmpty() || port < 0 || port > 65535)
		return false;

	const wchar_t *waddr_str = String(address).w_str();
	AnsiString port_str = IntToStr(port);

	ADDRINFOA *address_info = NULL;
	sockaddr  *socket_addr;
	size_t    socket_addr_len;

	SOCKET tcpip_socket;

	// ***************
	// Create a SOCKET for connecting to server

	result = getaddrinfo(address.c_str(), port_str.c_str(), NULL, &address_info);
	if (result != 0 || address_info == NULL)
	{
		const int err = WSAGetLastError();
		pushError(err, "connect_getaddrinfo");
		return false;
	}

	socket_addr     = address_info->ai_addr;
	socket_addr_len = address_info->ai_addrlen;

	tcpip_socket = WSASocket(socket_addr->sa_family, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
//	tcpip_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tcpip_socket == INVALID_SOCKET)
	{
		const int err = WSAGetLastError();
		pushError(err, "connect_WSASocket");
		freeaddrinfo(address_info);
		return false;
	}

	// ***************
	// Turn on security for the socket
	SOCKET_SECURITY_SETTINGS security_settings;
	security_settings.SecurityProtocol = SOCKET_SECURITY_PROTOCOL_DEFAULT;
//	security_settings.SecurityFlags = SOCKET_SETTINGS_GUARANTEE_ENCRYPTION;
	security_settings.SecurityFlags = SOCKET_SETTINGS_ALLOW_INSECURE;
	result = WSASetSocketSecurity(tcpip_socket, &security_settings, sizeof(security_settings), NULL, NULL);
	if (result == SOCKET_ERROR)
	{
		const int err = WSAGetLastError();
		pushError(err, "connect_WSASetSocketSecurity");
		freeaddrinfo(address_info);
		return false;
	}

	// ***************
	// Specify the server SPN

	{
		DWORD serverSpnStringLen = (DWORD)wcslen(waddr_str);
		DWORD peerTargetNameLen = sizeof(SOCKET_PEER_TARGET_NAME) + (serverSpnStringLen * sizeof(wchar_t));

		SOCKET_PEER_TARGET_NAME *peerTargetName = (SOCKET_PEER_TARGET_NAME *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, peerTargetNameLen);
		if (peerTargetName == NULL)
		{
			//result = ERROR_NOT_ENOUGH_MEMORY;
			const int err = WSAGetLastError();
			pushError(err, "connect_HeapAlloc");
			freeaddrinfo(address_info);
			return false;
		}

		// Use the security protocol as specified by the settings
		peerTargetName->SecurityProtocol = security_settings.SecurityProtocol;

		// Specify the server SPN
		peerTargetName->PeerTargetNameStringLen = serverSpnStringLen;
		RtlCopyMemory((BYTE *)peerTargetName->AllStrings, (BYTE *)waddr_str, serverSpnStringLen * sizeof(wchar_t));

		result = WSASetSocketPeerTargetName(tcpip_socket, peerTargetName, peerTargetNameLen, NULL, NULL);
		if (result == SOCKET_ERROR)
		{
			const int err = WSAGetLastError();
			pushError(err, "connect_WSASetSocketPeerTargetName");
			freeaddrinfo(address_info);
			HeapFree(GetProcessHeap(), 0, peerTargetName);
			return false;
		}

		HeapFree(GetProcessHeap(), 0, peerTargetName);
	}

	// setsockopt .. https://docs.microsoft.com/en-us/windows/win32/winsock/sol-socket-socket-options

	{	// enable Nagle's algorithmm
		BOOL opt = FALSE;
		result = setsockopt(tcpip_socket, SOL_SOCKET, TCP_NODELAY, (char *)&opt, sizeof(opt));
		if (result == SOCKET_ERROR)
		{
			const int err = WSAGetLastError();
			pushError(err, "connect_setsockopt_nodelay");
			freeaddrinfo(address_info);
			return false;
		}
	}

	{	// lots of RX buffer bytes so as not to miss any data
		DWORD buf_size = 16777216;
		result = setsockopt(tcpip_socket, SOL_SOCKET, SO_RCVBUF, (char *)&buf_size, sizeof(buf_size));
		if (result == SOCKET_ERROR)
		{
			const int err = WSAGetLastError();
			pushError(err, "connect_setsockopt_rcvbuf");
			freeaddrinfo(address_info);
			return false;
		}
	}

	{
//		tcp_keepalive keepalive;
//		keepalive.onoff = 1;
//		keepalive.keepalivetime = 30000;	// 30 seconds
//		keepalive.keepaliveinterval = 2000;	// 2 seconds

		BOOL opt = TRUE;
		result = setsockopt(tcpip_socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&opt, sizeof(opt));
		if (result == SOCKET_ERROR)
		{
			const int err = WSAGetLastError();
			pushError(err, "connect_setsockopt_keepalive");
			freeaddrinfo(address_info);
			return false;
		}
/*
		DWORD idle = 30;
		result = setsockopt(tcpip_socket, IPPROTO_TCP, TCP_KEEPIDLE, (char *)&idle, sizeof(idle));
		if (result == SOCKET_ERROR)
		{
			const int err = WSAGetLastError();
			pushError(err, "connect_setsockopt_keepidle");
			freeaddrinfo(address_info);
			return false;
		}

		DWORD interval = 2;
		result = setsockopt(tcpip_socket, IPPROTO_TCP, TCP_KEEPINTVL, (char *)&interval, sizeof(interval));
		if (result == SOCKET_ERROR)
		{
			const int err = WSAGetLastError();
			pushError(err, "connect_setsockopt_keepintvl");
			freeaddrinfo(address_info);
			return false;
		}

		DWORD maxpkt = 5;
		result = setsockopt(tcpip_socket, IPPROTO_TCP, TCP_KEEPCNT, (char *)&maxpkt, sizeof(maxpkt));
		if (result == SOCKET_ERROR)
		{
			const int err = WSAGetLastError();
			pushError(err, "connect_setsockopt_keepcnt");
			freeaddrinfo(address_info);
			return false;
		}
*/
	}

	// ***************
	// allow buffered data to be sent for up to 200ms when closing the socket

	{
		LINGER linger;
		linger.l_onoff = 1;
		linger.l_linger = 200;
		result = setsockopt(tcpip_socket, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger));
		if (result == SOCKET_ERROR)
		{
			const int err = WSAGetLastError();
			pushError(err, "connect_setsockopt_linger");
			freeaddrinfo(address_info);
			return false;
		}
	}

	// ***************
	// put socket in non-blocking mode

	{
		u_long block = 1;
		result = ioctlsocket(tcpip_socket, FIONBIO, &block);
		if (result == SOCKET_ERROR)
		{
			const int err = WSAGetLastError();
			pushError(err, "connect_ioctlsocket");
			freeaddrinfo(address_info);
			return false;
		}
	}

	// ***************
	// connect to server

	result = WSAConnect(tcpip_socket, socket_addr, socket_addr_len, NULL, NULL, NULL, NULL);

//	struct sockaddr_in client_service = {0};
//	client_service.sin_family         = AF_INET;
//	client_service.sin_port           = htons(port);
//	client_service.sin_addr.s_addr    = inet_addr(addr_str.c_str());
//	result = connect(tcpip_socket, (SOCKADDR *)&client_service, sizeof(client_service));

	if (result == SOCKET_ERROR)
	{
		const int err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK)
		{
			pushError(err, "connect_WSAConnect");
			freeaddrinfo(address_info);
			return false;
		}
	}

	// connection pending

	fd_set setW;
	FD_ZERO(&setW);
	FD_SET(tcpip_socket, &setW);

	fd_set setE;
	FD_ZERO(&setE);
	FD_SET(tcpip_socket, &setE);

	timeval time_out = {0};
	time_out.tv_sec = 3;		// 3 second timeout
	time_out.tv_usec = 0;

	result = select(0, NULL, &setW, &setE, &time_out);
	if (result <= 0)
	{	// select() failed or connection timed out
		if (result == SOCKET_ERROR)
		{
			const int err = WSAGetLastError();
			pushError(err, "connect_select");
		}
		else
			pushError(SOCKET_ERROR, "connect_select");
		freeaddrinfo(address_info);
		return false;
	}

	result = FD_ISSET(tcpip_socket, &setE);
	if (result != 0)
	{	// connection failed
		int err = 0;
		int err_len = sizeof(err);
		getsockopt(tcpip_socket, SOL_SOCKET, SO_ERROR, (char*)&err, &err_len);
		pushError(err, "connect_getsockopt");
		freeaddrinfo(address_info);
		return false;
	}

	// ***************
	// put socket back into blocking mode

//	block = 0;
//	result = ioctlsocket(tcpip_socket, FIONBIO, &block);
//	if (result == SOCKET_ERROR)
//	{
//		const int err = WSAGetLastError();
//		pushError(err, "connect_ioctlsocket");
//		freeaddrinfo(address_info);
//		return false;
//	}

	// ***************
	// connected !

	freeaddrinfo(address_info);

	m_tcpip_socket = tcpip_socket;

	return true;
}

void __fastcall CTCPIP::Disconnect()
{
	disconnectSocket();
}

int __fastcall CTCPIP::RxBytesAvailable()
{	// return number of available received bytes
	if (m_tcpip_socket == INVALID_SOCKET)
		return -1;

	const int wr = m_rx.buffer_wr;
	const int rd = m_rx.buffer_rd;
	return (wr >= rd) ? wr - rd : (m_rx.buffer.size() - rd) + wr;
}

int __fastcall CTCPIP::RxByte()
{
	if (m_tcpip_socket == INVALID_SOCKET)
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

int __fastcall CTCPIP::RxBytePeek()
{	// fetch a byte without removing it from the rx buffer
	if (m_tcpip_socket == INVALID_SOCKET)
		return -1;

	const int wr = m_rx.buffer_wr;

	int rd = m_rx.buffer_rd;
	if (rd == wr)
		return -2;

	return (int)m_rx.buffer[rd];
}

int __fastcall CTCPIP::RxBytes(void *buf, int size)
{
	if (m_tcpip_socket == INVALID_SOCKET)
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

int __fastcall CTCPIP::RxBytesPeek(void *buf, int size)
{	// fetch bytes without removing them from the rx buffer
	if (m_tcpip_socket == INVALID_SOCKET)
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

bool __fastcall CTCPIP::TxEmpty()
{
	return (m_tcpip_socket == INVALID_SOCKET || m_tx.buffer_rd == m_tx.buffer_wr) ? true : false;
}

int __fastcall CTCPIP::TxBytesWaiting()
{	// return number of queued transmit bytes
	if (m_tcpip_socket == INVALID_SOCKET)
		return -1;

	const int rd = m_tx.buffer_rd;
	const int wr = m_tx.buffer_wr;
	const int bytes_available = (wr >= rd) ? wr - rd : (m_tx.buffer.size() - rd) + wr;

	return bytes_available;
}

int __fastcall CTCPIP::TxByte(const int b)
{
	if (m_tcpip_socket == INVALID_SOCKET)
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

int __fastcall CTCPIP::TxBytes(const void *buf, int bytes)
{
	if (m_tcpip_socket == INVALID_SOCKET)
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

int __fastcall CTCPIP::TxBytes(const char *s)
{
	if (m_tcpip_socket == INVALID_SOCKET)
		return -1;

	if (s == NULL)
		return 0;

	const int len = (int)strlen(s);
	if (len <= 0)
		return 0;

	return TxBytes(s, len);
}

int __fastcall CTCPIP::TxBytes(const STRING s)
{
	if (m_tcpip_socket == INVALID_SOCKET)
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

void __fastcall CTCPIP::processTx()
{
	if (m_tcpip_socket == INVALID_SOCKET)
		return;

	SOCKET tcpip_socket = m_tcpip_socket;

	int result;

	int bytes_written = 0;

	const int wr = m_tx.buffer_wr;
	int rd       = m_tx.buffer_rd;
	int len      = (wr >= rd) ? wr - rd : m_tx.buffer.size() - rd;

	if (len <= 0)
		return;

//	DWORD bytesSent = 0;
//	WSABUF wsaBuf = {0};
//	wsaBuf.len = packet_size;
//	wsaBuf.buf = (char *)&packet;
//	result = WSASend(tcpip_socket, &wsaBuf, 1, &bytesSent, 0, NULL, NULL);

	result = send(tcpip_socket, (char *)&m_tx.buffer[rd], len, 0);

	if (result == SOCKET_ERROR)
	{
		const int err = WSAGetLastError();
		pushError(::GetLastError(), "tx send");
		disconnectSocket();
		return;
	}

	if (result <= 0)
		return;

	bytes_written = result;	// number of bytes sent

	if (bytes_written > 0)
	{
		rd += bytes_written;
		if (rd >= (int)m_tx.buffer.size())
			rd -= m_tx.buffer.size();
		m_tx.buffer_rd = rd;
	}
}

void __fastcall CTCPIP::processRx()
{
	if (m_tcpip_socket == INVALID_SOCKET)
		return;

	SOCKET tcpip_socket = m_tcpip_socket;

	int result;

	DWORD bytes_available = 0;

//	result = WSAIoctl(tcpip_socket, FIONREAD, NULL, 0, NULL, 0, &bytes_available, NULL, NULL);
	result = ioctlsocket(tcpip_socket, FIONREAD, &bytes_available);

	if (result == SOCKET_ERROR)
	{
		const int err = WSAGetLastError();
		pushError(::GetLastError(), "rx ioctlsocket");
		disconnectSocket();
		return;
	}

	if (bytes_available == 0)
		return;

	// make sure the temp buffer is big enough for the number of rx bytes ready
	if (m_buffer.size() < bytes_available)
		m_buffer.resize(bytes_available * 4);

	// **********************
	// fetch the received bytes

	//	DWORD flags = 0;
	//	DWORD bytesRecvd = 0;
	//	WSABUF wsaBuf = {0};
	//	wsaBuf.len = i;
	//	wsaBuf.buf = &m_buffer[0];
	//	result = WSARecv(tcpip_socket, &wsaBuf, 1, &bytesRecvd, &flags, NULL, NULL);

	result = recv(tcpip_socket, &m_buffer[0], bytes_available, 0);

	if (result == 0)
	{	// socket has closed
		const int err = WSAGetLastError();
		pushError(err, "rx recv");
		disconnectSocket();
		return;
	}

	if (result == SOCKET_ERROR)
	{
		const int err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK)
		{
			const int err = WSAGetLastError();
			pushError(err, "rx recv");
			disconnectSocket();
			return;
		}
	}

	const int bytes_read = result;	// number of bytes received
	if (bytes_read <= 0)
		return;

	// **********************
	// copy the received bytes into our rx fifo buffer

	int loops = 0;
	int rd = 0;
	while (loops++ < 100 && rd < (int)bytes_read)
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
			Sleep(1);
		}
	}

	// **********************
}

void __fastcall CTCPIP::threadProcess()
{
	if (m_tcpip_socket == INVALID_SOCKET || m_thread == NULL)
		return;

	processTx();
	processRx();
}

/*

//	DWORD bytesSent = 0;
//	WSABUF wsaBuf = {0};
//	wsaBuf.len = packet_size;
//	wsaBuf.buf = (char *)&packet;
//	int result = WSASend(tcpip_socket, &wsaBuf, 1, &bytesSent, 0, NULL, NULL);

	result = send(tcpip_socket, (char *)packet, packet_size, 0);

	if (result == SOCKET_ERROR)
	{
		err = WSAGetLastError();

		closesocket(tcpip_socket);
		tcpip_socket = INVALID_SOCKET;

		AnsiString s;
		s.printf("tx: connection closed, error %d ", err);
		Memo1->Lines->Add(s);

		::PostMessage(this->Handle, WM_CAPTURE_STOP, 0, 0);

		return -1;
	}
*/
