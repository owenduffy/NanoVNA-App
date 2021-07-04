
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

//#include <vcl.h>		// AnsiString
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>		// sprintf
#include <stdlib.h>     // wcstombs, wchar_t(C)
//#include <cstdio>
//#include <tchar.h>	// _T

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include "st_dfu.h"

#ifdef __BORLANDC__
	#pragma package(smart_init)
#endif

// *************************************************

#define PU_GET_NUMBEROFCONFIG			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_GET_CONFIG_DESCRIPTOR		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_GET_DEVICE_DESCRIPTOR		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_GET_STRING_DESCRIPTOR		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0803, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_CONFIG						CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0804, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_VENDOR_REQUEST				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0805, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_PIPE_CONTROL					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0806, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_SET_TIMEOUT					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0807, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_SET_EVENT_DISCONNECT			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0808, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_SET_EXCLUSIVE_OPEN			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0809, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_SET_EVENT_POWER_RESUME		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x080a, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_GET_ERROR					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x080b, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_NB_FRAMES_BUFFER				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x080c, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PU_SUSPEND_CONTROL				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x080d, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define URB_FUNCTION_VENDOR_DEVICE		0x0017
#define URB_FUNCTION_VENDOR_INTERFACE	0x0018
#define URB_FUNCTION_VENDOR_ENDPOINT	0x0019
#define URB_FUNCTION_VENDOR_OTHER		0x0020

#define URB_FUNCTION_CLASS_DEVICE		0x001A
#define URB_FUNCTION_CLASS_INTERFACE	0x001B
#define URB_FUNCTION_CLASS_ENDPOINT		0x001C
#define URB_FUNCTION_CLASS_OTHER		0x001F

#define STATUS_PIPE_OPEN				0xf0000001

// *************************************************

#define DEFAULT_TIMEOUT					2000

// *************************************************
// DFU Commands

#define DFU_DETACH						0x00
#define DFU_DNLOAD						0x01
#define DFU_UPLOAD						0x02
#define DFU_GETSTATUS					0x03
#define DFU_CLRSTATUS					0x04
#define DFU_GETSTATE					0x05
#define DFU_ABORT						0x06

// *************************************************

CSTDevice::CSTDevice(LPSTR SymbolicName)
{
	memset(m_SymbolicName, 0, sizeof(m_SymbolicName));

	if (SymbolicName)
	{
		int i = strlen(SymbolicName);
		if (i > (int)sizeof(m_SymbolicName) - 1)
			i = (int)sizeof(m_SymbolicName) - 1;
		memmove(m_SymbolicName, SymbolicName, i);
	}
	
	m_DeviceHandle = NULL;
	m_CurrentConfig = 0;
	m_CurrentInterf = 0;
	m_CurrentAltSet = 0;
	m_bDeviceIsOpen = FALSE;
	m_nDefaultTimeOut = DEFAULT_TIMEOUT;
	m_nbEndPoints = 0;
	m_pPipeHandles = NULL;
}

CSTDevice::~CSTDevice()
{
	Close();
}

DWORD CSTDevice::Open(PHANDLE phUnPlugEvent)
{
	DWORD nRet;

	if (m_bDeviceIsOpen)
		return STDEVICE_NOERROR;

	// Open the device
	nRet = OpenDevice(phUnPlugEvent);
	if (nRet != STDEVICE_NOERROR)
		return nRet;

	// Get the descriptors
	nRet = AllocDescriptors();
	if (nRet != STDEVICE_NOERROR)
	{
		CloseDevice();
		return nRet;
	}

	m_bDeviceIsOpen = TRUE;

	return nRet;
}

DWORD CSTDevice::Close()
{
	if (!m_bDeviceIsOpen)
		return STDEVICE_NOERROR;

	ClosePipes();
	ReleaseDescriptors();
	CloseDevice();

	m_bDeviceIsOpen = FALSE;

	return STDEVICE_NOERROR;
}

DWORD CSTDevice::SelectCurrentConfiguration(UINT ConfigIdx, UINT InterfaceIdx, UINT AltSetIdx)
{
	DWORD nRet;
	DWORD ByteCount;
	USB_INTERFACE_DESCRIPTOR Desc;
	USB_CONFIGURATION_DESCRIPTOR *pConfDesc;

	// Put the InterfaceIdx to -1 if it should not be a search criterion.
	// Put the AltSetIdx to -1 if it should not be a search criterion.

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	if (m_pPipeHandles)
		return STDEVICE_PIPESAREOPEN;

	pConfDesc = (USB_CONFIGURATION_DESCRIPTOR *)&m_pConfigs[ConfigIdx];
	
	nRet = GetInterfaceDescriptor(ConfigIdx, InterfaceIdx, AltSetIdx, &Desc);
	if (nRet != STDEVICE_NOERROR)
		return nRet;

	m_bDeviceIsOpen = FALSE;

	ReleaseDescriptors();

	m_CurrentConfig = ConfigIdx;
	m_CurrentInterf = InterfaceIdx;
	m_CurrentAltSet = AltSetIdx;

	nRet = AllocDescriptors();
	if (nRet == STDEVICE_NOERROR)
	{
		BYTE TheConfig[3];
		TheConfig[0] = pConfDesc->bConfigurationValue - 1;
		TheConfig[1] = Desc.bInterfaceNumber;
		TheConfig[2] = Desc.bAlternateSetting;

		if (::DeviceIoControl(m_DeviceHandle, PU_CONFIG, TheConfig, sizeof(TheConfig), NULL, 0, &ByteCount, NULL))
			nRet = STDEVICE_NOERROR;
		else
			nRet = STDEVICE_BADPARAMETER;
	}

	if (nRet == STDEVICE_NOERROR)
		m_bDeviceIsOpen = TRUE;
	else
	{
		ClosePipes();
		ReleaseDescriptors();
		CloseDevice();
		m_bDeviceIsOpen = FALSE;
	}

	return nRet;
}

DWORD  CSTDevice::OpenDevice(PHANDLE phUnPlugEvent)
{
	CloseDevice();

	m_DeviceHandle = ::CreateFileA((LPSTR)m_SymbolicName, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (!m_DeviceHandle)
		return STDEVICE_OPENDRIVERERROR;

	BOOL bFake = m_bDeviceIsOpen;
	DWORD nRet = STDEVICE_NOERROR;

	m_bDeviceIsOpen = TRUE;

	// BUG BUG: Do not issue a reset as Composite devices do not support this !
	//nRet = Reset();

	m_bDeviceIsOpen = bFake;

	if (nRet != STDEVICE_NOERROR || !phUnPlugEvent)
		return nRet;

	*phUnPlugEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);	// Disconnect event
	if (!*phUnPlugEvent)
		return nRet;

	DWORD ByteCount;

	if (!::DeviceIoControl(m_DeviceHandle, PU_SET_EVENT_DISCONNECT, phUnPlugEvent, sizeof(HANDLE), NULL, 0, &ByteCount, NULL))
		return STDEVICE_CANTUSEUNPLUGEVENT;

	return STDEVICE_NOERROR;
}

DWORD CSTDevice::CloseDevice()
{
	if (m_DeviceHandle)
		::CloseHandle(m_DeviceHandle);
	m_DeviceHandle = NULL;

	return STDEVICE_NOERROR;
}

DWORD CSTDevice::AllocDescriptors()
{
	BOOL Success;
	DWORD nRet = STDEVICE_ERRORDESCRIPTORBUILDING;

	ReleaseDescriptors();

	memset(&m_DeviceDescriptor, 0, sizeof(USB_DEVICE_DESCRIPTOR));

	DWORD ByteCount = 0;

	Success = ::DeviceIoControl(m_DeviceHandle, PU_GET_DEVICE_DESCRIPTOR, NULL, 0, &m_DeviceDescriptor, sizeof(USB_DEVICE_DESCRIPTOR), &ByteCount, NULL);
	if (!Success || ByteCount != sizeof(USB_DEVICE_DESCRIPTOR))
		return nRet;

	// Get the full configuration
	m_pConfigs.resize(m_DeviceDescriptor.bNumConfigurations);
	if ((int)m_pConfigs.size() != m_DeviceDescriptor.bNumConfigurations)
	{
		m_pConfigs.resize(0);
		return nRet;
	}

	for (int i = 0; i < m_DeviceDescriptor.bNumConfigurations; i++)
	{
		// cathy
		uint8_t j = i;
		Success = ::DeviceIoControl(m_DeviceHandle, PU_GET_CONFIG_DESCRIPTOR, &j, sizeof(j), &m_pConfigs[i], sizeof(m_pConfigs[i]), &ByteCount, NULL);
		if (!Success)
			break;
	}

	if (Success)
		return STDEVICE_NOERROR;

	m_pConfigs.resize(0);

	return nRet;
}

DWORD CSTDevice::ReleaseDescriptors()
{
	m_pConfigs.resize(0);
	return STDEVICE_NOERROR;
}

DWORD CSTDevice::OpenPipes()
{
	char PipeSymbName[512];

	int i;
	DWORD nRet;
	BOOL bTmp;
	UINT _uiNbOfInterfaces = 0;
	UINT _uiNbOfEndpointsTmp = 0;
//	UINT j = 0;
	UINT k = 0;

	ClosePipes();
	// Fake open
	bTmp = m_bDeviceIsOpen;
	m_bDeviceIsOpen = TRUE;

	// We must open all the endpoints available on all the configured interfaces
	nRet = GetNbOfInterfaces(m_CurrentConfig, &_uiNbOfInterfaces);
	if (nRet!=STDEVICE_NOERROR)
	{
		m_bDeviceIsOpen=bTmp;
		return nRet;
	}

	for (int j = 0; j < (int)_uiNbOfInterfaces; j++)
	{
		nRet = GetNbOfEndPoints(m_CurrentConfig, j, m_CurrentAltSet, &_uiNbOfEndpointsTmp);
		if (nRet != STDEVICE_NOERROR)
		{
			m_bDeviceIsOpen = bTmp;
			return nRet;
		}
		m_nbEndPoints += _uiNbOfEndpointsTmp;
	}

	m_pPipeHandles = new HANDLE[m_nbEndPoints];

	for (int j = 0; j < (int)_uiNbOfInterfaces; j++)
	{
		nRet = GetNbOfEndPoints(m_CurrentConfig, j, m_CurrentAltSet, &_uiNbOfEndpointsTmp);

		if (nRet!=STDEVICE_NOERROR)
		{
			m_bDeviceIsOpen=bTmp;
			return nRet;
		}

		for (i = 0; i < (int)_uiNbOfEndpointsTmp; i++)
		{
			// Get endpoint description to see if it's an input or output pipe, and get its address
			USB_ENDPOINT_DESCRIPTOR Desc;
			nRet = GetEndPointDescriptor(m_CurrentConfig, m_CurrentInterf, m_CurrentAltSet, k, &Desc);
			if (nRet==STDEVICE_NOERROR)
			{
				sprintf(PipeSymbName, "%s\\%02x", m_SymbolicName, Desc.bEndpointAddress);

				m_pPipeHandles[k] = ::CreateFileA(
												PipeSymbName,
												(USB_ENDPOINT_DIRECTION_IN(Desc.bEndpointAddress)) ? GENERIC_READ : GENERIC_WRITE,
												0,
												NULL,
												OPEN_EXISTING,
												FILE_FLAG_OVERLAPPED,
												NULL);

				if ( (!m_pPipeHandles[k]) || (m_pPipeHandles[k] == INVALID_HANDLE_VALUE) )
				{
					while (k > 0)
					{
						k--;
						::CloseHandle(m_pPipeHandles[k]);
						m_pPipeHandles[k]=INVALID_HANDLE_VALUE;
					}
					delete[] m_pPipeHandles;
					m_pPipeHandles=NULL;
					nRet=STDEVICE_PIPECREATIONERROR;
					break;
				}
				else
				{
					// reset the pipe
					BYTE InBuffer[2];
					DWORD ByteCount;
					InBuffer[0] = Desc.bEndpointAddress;
					InBuffer[1] = 1;  // Reset pipe

					if (!::DeviceIoControl(m_DeviceHandle, PU_PIPE_CONTROL, InBuffer, sizeof(InBuffer), NULL, 0, &ByteCount, NULL))
					{
						while (k>0)
						{
							k--;
							::CloseHandle(m_pPipeHandles[k]);
							m_pPipeHandles[k]=INVALID_HANDLE_VALUE;
						}
						delete[] m_pPipeHandles;
						m_pPipeHandles=NULL;
						nRet=STDEVICE_PIPERESETERROR;
						break;
					}
					else
					{
						// Abort any transfer
						BYTE InBuffer[2];
						DWORD ByteCount;
						InBuffer[0] = Desc.bEndpointAddress;
						InBuffer[1] = 0;  // Abort

						if (!::DeviceIoControl(m_DeviceHandle, PU_PIPE_CONTROL, InBuffer, sizeof(InBuffer), NULL, 0, &ByteCount, NULL))
						{
							while (k>0)
							{
								k--;
								::CloseHandle(m_pPipeHandles[k]);
								m_pPipeHandles[k]=INVALID_HANDLE_VALUE;
							}
							delete[] m_pPipeHandles;
							m_pPipeHandles=NULL;
							nRet=STDEVICE_PIPEABORTERROR;
							break;
						}
					}
				}
			}
			else
			{
				while (k>0)
				{
					k--;
					::CloseHandle(m_pPipeHandles[k]);
					m_pPipeHandles[k]=INVALID_HANDLE_VALUE;
				}
				delete[] m_pPipeHandles;
				m_pPipeHandles=NULL;
				nRet=STDEVICE_PIPECREATIONERROR;
				break;
			}

			k++;
		}
	}

	m_bDeviceIsOpen=bTmp;

	return nRet;
}

DWORD CSTDevice::ClosePipes()
{
	DWORD nRet = STDEVICE_NOERROR;

	if (m_pPipeHandles)
	{
		for (int i = 0; i < (int)m_nbEndPoints; i++)
		{
			if (m_pPipeHandles[i])
			{
				::CloseHandle(m_pPipeHandles[i]);
				m_pPipeHandles[i] = NULL;
			}
		}
		m_nbEndPoints = 0;
		delete[] m_pPipeHandles;
		m_pPipeHandles = NULL;
	}
	return nRet;
}


DWORD CSTDevice::GetStringDescriptor(UINT Index, LPSTR Desc)
{
	DWORD byteCount;
	uint8_t strID[3];
	char buffer[512];

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	if (Index <= 0)
		return FALSE;

	strID[0] = Index;

	// UK Language
	strID[1] = 0x08;
	strID[2] = 0x09;

	// US Language
//	strID[1] = 0x04;
//	strID[2] = 0x09;

	memset(buffer, 0, sizeof(buffer));

	BOOL success = ::DeviceIoControl(m_DeviceHandle, PU_GET_STRING_DESCRIPTOR, strID, sizeof(strID), buffer, sizeof(buffer) - 4, &byteCount, NULL);
	if (success && byteCount > 2 && buffer[1] == 3) // check string decsriptor type == 3
	{
		if (Desc)
		{
			#if defined(_MSC_VER)
				strcpy(Desc, buffer + 2);
			#elif defined(__BORLANDC__)
				// convert wchar to char
				wcstombs(buffer, (const wchar_t *)(buffer + 2), byteCount);
//				int ret = wcstombs(buffer, (const wchar_t *)(buffer + 2), sizeof(buffer) - 4);
//				if (byteCount >= 0)
//					buffer[byteCount / 2] = 0;
				strcpy(Desc, buffer);
			#endif
		}
		return STDEVICE_NOERROR;
	}

	return STDEVICE_STRINGDESCRIPTORERROR;
}

DWORD CSTDevice::GetDeviceDescriptor(PUSB_DEVICE_DESCRIPTOR pDevDescr)
{
	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	memmove(pDevDescr, &m_DeviceDescriptor, sizeof(m_DeviceDescriptor));

	return STDEVICE_NOERROR;
}

DWORD CSTDevice::GetNbOfConfigurations(PUINT pNbOfConfigs)
{
	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	*pNbOfConfigs = m_DeviceDescriptor.bNumConfigurations;

	return STDEVICE_NOERROR;
}

DWORD CSTDevice::GetConfigurationDescriptor(UINT ConfigIdx, PUSB_CONFIGURATION_DESCRIPTOR Desc)
{
	DWORD nRet;
	UINT NbOfConfigs;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	nRet = GetNbOfConfigurations(&NbOfConfigs);
	if (nRet != STDEVICE_NOERROR)
		return nRet;

	if (ConfigIdx >= NbOfConfigs)
		return STDEVICE_BADPARAMETER;

	if (m_pConfigs.size() <= 0)
		return STDEVICE_BADPARAMETER;

	memcpy(Desc, &m_pConfigs[ConfigIdx], sizeof(USB_CONFIGURATION_DESCRIPTOR));

	return nRet;
}

DWORD CSTDevice::GetNbOfInterfaces(UINT ConfigIdx, PUINT pNbOfInterfaces)
{
	PUSB_CONFIGURATION_DESCRIPTOR ConfDesc;
	DWORD nRet;
	UINT NbOfConfigs;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	nRet = GetNbOfConfigurations(&NbOfConfigs);
	if (nRet != STDEVICE_NOERROR)
		return nRet;

	if (ConfigIdx >= NbOfConfigs)
		return STDEVICE_BADPARAMETER;

	if (m_pConfigs.size() <= 0)
		return STDEVICE_BADPARAMETER;

	ConfDesc = (USB_CONFIGURATION_DESCRIPTOR *)&m_pConfigs[ConfigIdx];
	*pNbOfInterfaces = ConfDesc->bNumInterfaces;

	return nRet;
}

DWORD CSTDevice::GetNbOfAlternates(UINT ConfigIdx, UINT InterfIdx, PUINT pNbOfAltSets)
{
	int Res = 1;
	UINT Cnt = 0;
	PBYTE pTmp;
	int TotalLength;
	PUSB_CONFIGURATION_DESCRIPTOR ConfDesc;
	DWORD nRet;
	UINT NbOfInterfaces;
	USB_INTERFACE_DESCRIPTOR *pPreviousInterf = NULL;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	nRet = GetNbOfInterfaces(ConfigIdx, &NbOfInterfaces);
	if (nRet != STDEVICE_NOERROR)
		return nRet;

	if (InterfIdx >= NbOfInterfaces)
		return STDEVICE_BADPARAMETER;

	if (m_pConfigs.size() <= 0)
		return STDEVICE_BADPARAMETER;

	pTmp = (PBYTE)&m_pConfigs[ConfigIdx];
	ConfDesc = (PUSB_CONFIGURATION_DESCRIPTOR)&m_pConfigs[ConfigIdx];
	TotalLength = ConfDesc->wTotalLength;
	pTmp += pTmp[0];

	while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
	{
		if (pTmp[1] == USB_CONFIGURATION_DESCRIPTOR_TYPE)
			return STDEVICE_DESCRIPTORNOTFOUND;

		if (pTmp[1] == USB_INTERFACE_DESCRIPTOR_TYPE)
		{
			USB_INTERFACE_DESCRIPTOR *pInterf = (USB_INTERFACE_DESCRIPTOR *)pTmp;

			// increment our index to look for the InterfIdx
			if (pPreviousInterf)
			{
				// See if the current interface descriptor is a different interface or
				// an aleternate of the same one
				if (Cnt == InterfIdx)
				{
					if (pPreviousInterf->bInterfaceNumber == pInterf->bInterfaceNumber)
						Res++;
					else
					if (Res > 1)
						break;
				}
				else
					Cnt++;
			}
			pPreviousInterf = pInterf;
		}
		pTmp += pTmp[0]; // Switch to next descriptor
	}

	*pNbOfAltSets = Res;

	return STDEVICE_NOERROR;
}

DWORD CSTDevice::GetInterfaceDescriptor(UINT ConfigIdx, UINT InterfIdx, UINT AltIdx, PUSB_INTERFACE_DESCRIPTOR Desc)
{
	PUSB_CONFIGURATION_DESCRIPTOR ConfDesc;
	PBYTE pTmp;
	int TotalLength;
	DWORD nRet;
	UINT NbOfAltSets;
	UINT CntAlt = 1;
	UINT CntInterf = 0;
	PUSB_INTERFACE_DESCRIPTOR pPreviousInterf = NULL;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	nRet = GetNbOfAlternates(ConfigIdx, InterfIdx, &NbOfAltSets);
	if (nRet != STDEVICE_NOERROR)
		return nRet;

	if (AltIdx >= NbOfAltSets)
		return STDEVICE_BADPARAMETER;

	if (m_pConfigs.size() <= 0)
		return STDEVICE_BADPARAMETER;

	pTmp = (PBYTE)&m_pConfigs[ConfigIdx];
	ConfDesc = (USB_CONFIGURATION_DESCRIPTOR *)&m_pConfigs[ConfigIdx];
	TotalLength = ConfDesc->wTotalLength;
	pTmp += pTmp[0];

	while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
	{
		if (pTmp[1] == USB_CONFIGURATION_DESCRIPTOR_TYPE)
			return STDEVICE_DESCRIPTORNOTFOUND;

		if (pTmp[1] == USB_INTERFACE_DESCRIPTOR_TYPE)
		{
			PUSB_INTERFACE_DESCRIPTOR pInterf = (PUSB_INTERFACE_DESCRIPTOR)pTmp;

			// increment our index to look for the InterfIdx
			if (pPreviousInterf)
			{
				// See if the current interface descriptor is a different interface or
				// an aleternate of the same one
				if (CntInterf == InterfIdx)
				{
					if  (pPreviousInterf->bInterfaceNumber == pInterf->bInterfaceNumber)
					{
						if (CntAlt == AltIdx)
						{
							memcpy(Desc, pInterf, sizeof(USB_INTERFACE_DESCRIPTOR));
							break;
						}
						else
							CntAlt++;
					}
					else
						if (CntAlt > 0)
							return STDEVICE_DESCRIPTORNOTFOUND;
				}
				else
					CntInterf++;
			}
			else
			{
				if ( (InterfIdx == 0) && (AltIdx == 0) )
				{
					memcpy(Desc, pInterf, sizeof(USB_INTERFACE_DESCRIPTOR));
					break;
				}
			}
			pPreviousInterf = pInterf;
		}
		pTmp += pTmp[0]; // Switch to next descriptor
	}

	return STDEVICE_NOERROR;
}

DWORD CSTDevice::GetNbOfEndPoints(UINT ConfigIdx, UINT InterfIdx, UINT AltIdx, PUINT pNbOfEndPoints)
{
	PUSB_CONFIGURATION_DESCRIPTOR ConfDesc;
	PBYTE pTmp;
	int TotalLength;
	DWORD nRet;
	UINT NbOfAltSets;
	PUSB_INTERFACE_DESCRIPTOR pPreviousInterf = NULL;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	nRet=GetNbOfAlternates(ConfigIdx, InterfIdx, &NbOfAltSets);
	if (nRet!=STDEVICE_NOERROR)
		return nRet;

	if (AltIdx >= NbOfAltSets)
		return STDEVICE_BADPARAMETER;

	if (m_pConfigs.size() <= 0)
		return STDEVICE_BADPARAMETER;

	pTmp = (PBYTE)&m_pConfigs[ConfigIdx];
	ConfDesc = (PUSB_CONFIGURATION_DESCRIPTOR)&m_pConfigs[ConfigIdx];
	TotalLength = ConfDesc->wTotalLength;
	pTmp += pTmp[0];

	while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
	{
		if (pTmp[1]==USB_CONFIGURATION_DESCRIPTOR_TYPE)
			return STDEVICE_DESCRIPTORNOTFOUND;

		if (pTmp[1]==USB_INTERFACE_DESCRIPTOR_TYPE)
		{
			PUSB_INTERFACE_DESCRIPTOR pInterf=(PUSB_INTERFACE_DESCRIPTOR)pTmp;

			// increment our index to look for the InterfIdx
			if (pPreviousInterf)
			{
				// See if the current interface descriptor is a different interface or
				// an aleternate of the same one
				// We check that the founded interface is the same as the wanted
				// and has the good alternate setting
				//if (CntInterf==InterfIdx)
				{
					// if (pPreviousInterf->bInterfaceNumber==pInterf->bInterfaceNumber)
					if (pInterf->bInterfaceNumber == (int)InterfIdx)
					{
						// if (CntAlt==AltIdx)
						if ((int)AltIdx == pInterf->bAlternateSetting)
						{
							*pNbOfEndPoints=pInterf->bNumEndpoints;
							break;
						}
						//else
						//	CntAlt++;
					}
					//else
					//{
					//	if (CntAlt>0)
					//	{
					//		nRet=STDEVICE_DESCRIPTORNOTFOUND;
					//		break;
					//	}
					//}
				}
				//else
				//	CntInterf++;
			}
			else
			{
				if ( (InterfIdx==0) && (AltIdx==0) )
				{
					*pNbOfEndPoints=pInterf->bNumEndpoints;
					break;
				}
			}
			pPreviousInterf=pInterf;
		}

		pTmp+=pTmp[0]; // Switch to next descriptor
	}

	return nRet;
}

DWORD CSTDevice::GetEndPointDescriptor(	UINT ConfigIdx,
										UINT InterfIdx,
										UINT AltIdx,
										UINT EndPointIdx,
										PUSB_ENDPOINT_DESCRIPTOR Desc)
{
	PUSB_CONFIGURATION_DESCRIPTOR ConfDesc;
	PBYTE pTmp;
	int TotalLength;
	DWORD nRet = STDEVICE_NOERROR;
	UINT Idx = 0;
	BOOLEAN _bEndPointFound = FALSE;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	// We check that the index is not invalid corresponding to the total number of endpoints configured
	if (EndPointIdx >= m_nbEndPoints)
		return STDEVICE_BADPARAMETER;

	if (m_pConfigs.size() <= 0)
		return STDEVICE_BADPARAMETER;

	pTmp = (PBYTE)&m_pConfigs[ConfigIdx];
	ConfDesc = (PUSB_CONFIGURATION_DESCRIPTOR)&m_pConfigs[ConfigIdx];
	TotalLength = ConfDesc->wTotalLength;
	pTmp += pTmp[0];

	// We must loop on all the interfaces because the endpoint index is based
	// on all the endpoints configured
	while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength && !_bEndPointFound)
	{
		if (pTmp[1] == USB_CONFIGURATION_DESCRIPTOR_TYPE)
			return STDEVICE_DESCRIPTORNOTFOUND;

		if (pTmp[1] == USB_INTERFACE_DESCRIPTOR_TYPE)
		{
//			PUSB_INTERFACE_DESCRIPTOR pInterf = (PUSB_INTERFACE_DESCRIPTOR)pTmp;

			pTmp += pTmp[0];

			while(pTmp[1]==USB_ENDPOINT_DESCRIPTOR_TYPE)
			{
				if (Idx==EndPointIdx)
				{
					memcpy(Desc, pTmp, sizeof(USB_ENDPOINT_DESCRIPTOR));
					_bEndPointFound = TRUE;
					break;
				}
				else
				{
					Idx++;
					pTmp += pTmp[0];
				}
			}

		}
		else
			pTmp += pTmp[0];
	}

	if (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] >= TotalLength)
		nRet = STDEVICE_DESCRIPTORNOTFOUND;

	return nRet;
}

DWORD CSTDevice::GetNbOfDescriptors(BYTE nLevel, BYTE nType, UINT ConfigIdx, UINT InterfIdx, UINT AltIdx, UINT EndPointIdx, PUINT pNbOfDescriptors)
{
	DWORD nRet=STDEVICE_BADPARAMETER;
	UINT NbOf;
	PUSB_CONFIGURATION_DESCRIPTOR ConfDesc;
	PBYTE pTmp;
	int TotalLength;
	UINT Idx=0;
	UINT EndPIdx=(UINT)-1;
	UINT CntAlt=1;
	UINT CntInterf=0;
	PUSB_INTERFACE_DESCRIPTOR pPreviousInterf=NULL;
	BOOLEAN _bEndPointFound = FALSE;

	if (!pNbOfDescriptors)
		return STDEVICE_BADPARAMETER;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	if ( (nType==USB_CONFIGURATION_DESCRIPTOR_TYPE) ||
		 (nType==USB_INTERFACE_DESCRIPTOR_TYPE) ||
		 (nType==USB_ENDPOINT_DESCRIPTOR_TYPE) ||
		 (nType==USB_DEVICE_DESCRIPTOR_TYPE) )
		return STDEVICE_BADPARAMETER;

	switch (nLevel)
	{
	case DESCRIPTOR_CONFIGURATION_LEVEL:
		nRet=GetNbOfConfigurations(&NbOf);
		if (nRet==STDEVICE_NOERROR)
		{
			if (ConfigIdx>=NbOf)
				nRet=STDEVICE_BADPARAMETER;
			else
			{
				pTmp = (PBYTE)&m_pConfigs[ConfigIdx];
				ConfDesc = (PUSB_CONFIGURATION_DESCRIPTOR)&m_pConfigs[ConfigIdx];
				TotalLength = ConfDesc->wTotalLength;
				pTmp += pTmp[0];

				while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
				{
					if (pTmp[1]==USB_CONFIGURATION_DESCRIPTOR_TYPE)
						break;
					if (pTmp[1]==USB_INTERFACE_DESCRIPTOR_TYPE)
						break;
					if (pTmp[1]==USB_ENDPOINT_DESCRIPTOR_TYPE)
						break;
					if (pTmp[1]==nType)
						Idx++;
					pTmp+=pTmp[0];
				}
			}
		}
		break;
	case DESCRIPTOR_INTERFACEALTSET_LEVEL:
		nRet=GetNbOfAlternates(ConfigIdx, InterfIdx, &NbOf);
		if (nRet==STDEVICE_NOERROR)
		{
			if (AltIdx>=NbOf)
				nRet=STDEVICE_BADPARAMETER;
			else
			{
				pTmp = (PBYTE)&m_pConfigs[ConfigIdx];
				ConfDesc = (PUSB_CONFIGURATION_DESCRIPTOR)&m_pConfigs[ConfigIdx];
				TotalLength = ConfDesc->wTotalLength;
				pTmp += pTmp[0];

				while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
				{
					if (pTmp[1]==USB_CONFIGURATION_DESCRIPTOR_TYPE)
						break;

					if (pTmp[1]==USB_INTERFACE_DESCRIPTOR_TYPE)
					{
						PUSB_INTERFACE_DESCRIPTOR pInterf=(PUSB_INTERFACE_DESCRIPTOR)pTmp;

						// increment our index to look for the InterfIdx
						if (pPreviousInterf)
						{
							// See if the current interface descriptor is a different interface or
							// an alternate of the same one
							if (CntInterf==InterfIdx)
							{
								if  (pPreviousInterf->bInterfaceNumber==pInterf->bInterfaceNumber)
								{
									if (CntAlt==AltIdx)
									{
										// We found wanted interface and alt set !
										pTmp+=pTmp[0];
										while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
										{
											if (pTmp[1]==USB_CONFIGURATION_DESCRIPTOR_TYPE)
												break;
											if (pTmp[1]==USB_INTERFACE_DESCRIPTOR_TYPE)
												break;
											if (pTmp[1]==USB_ENDPOINT_DESCRIPTOR_TYPE)
												break;
											if (pTmp[1]==nType)
												Idx++;
											pTmp+=pTmp[0];
										}
										break;
									}
									else
										CntAlt++;
								}
								else
								{
									if (CntAlt>0)
									{
										nRet=STDEVICE_DESCRIPTORNOTFOUND;
										break;
									}
								}
							}
							else
								CntInterf++;
						}
						else
						{
							// Do we need to access interface 0 and altset 0 ?
							if ( (InterfIdx==0) && (AltIdx==0) )
							{
								// Yes! We are in the good place. Let's search the wanted descriptor
								pTmp+=pTmp[0];
								while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
								{
									if (pTmp[1]==USB_CONFIGURATION_DESCRIPTOR_TYPE)
										break;
									if (pTmp[1]==USB_INTERFACE_DESCRIPTOR_TYPE)
										break;
									if (pTmp[1]==USB_ENDPOINT_DESCRIPTOR_TYPE)
										break;
									if (pTmp[1]==nType)
										Idx++;
									pTmp+=pTmp[0];
								}
								break;
							}
						}
						pPreviousInterf=pInterf;
					}
					pTmp+=pTmp[0];
				}
			}
		}
		break;
	case DESCRIPTOR_ENDPOINT_LEVEL:
		nRet=GetNbOfEndPoints(ConfigIdx, InterfIdx, AltIdx, &NbOf);
		if (nRet==STDEVICE_NOERROR)
		{
			// We must loop on all the interfaces because the endpoint index is based
			// on all the endpoints configured
			if (EndPointIdx>=m_nbEndPoints)
				nRet=STDEVICE_BADPARAMETER;
			else
			{
				pTmp = (PBYTE)&m_pConfigs[ConfigIdx];
				ConfDesc = (PUSB_CONFIGURATION_DESCRIPTOR)&m_pConfigs[ConfigIdx];
				TotalLength = ConfDesc->wTotalLength;
				pTmp += pTmp[0];

				while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength && !_bEndPointFound)
				{
					if (pTmp[1]==USB_CONFIGURATION_DESCRIPTOR_TYPE)
						return STDEVICE_DESCRIPTORNOTFOUND;

					if (pTmp[1]==USB_INTERFACE_DESCRIPTOR_TYPE)
					{
//						PUSB_INTERFACE_DESCRIPTOR pInterf=(PUSB_INTERFACE_DESCRIPTOR)pTmp;

						pTmp+=pTmp[0];

						while(pTmp[1]==USB_ENDPOINT_DESCRIPTOR_TYPE)
						{
							if (EndPIdx==EndPointIdx)
							{
								_bEndPointFound = TRUE;
								EndPIdx = 0;
								break;
							}
							else
							{
								EndPIdx++;
								pTmp+=pTmp[0];
							}
						}
					}
					else
						pTmp+=pTmp[0];
				}

				if (_bEndPointFound)
				{
					pTmp+=pTmp[0];
					while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
					{
						if (pTmp[1]==USB_CONFIGURATION_DESCRIPTOR_TYPE)
							break;
						if (pTmp[1]==USB_INTERFACE_DESCRIPTOR_TYPE)
							break;
						if (pTmp[1]==USB_ENDPOINT_DESCRIPTOR_TYPE)
							break;
						return STDEVICE_DESCRIPTORNOTFOUND;
//						if (pTmp[1]==nType)
//							Idx++;
//						pTmp+=pTmp[0];
					}
				}
				else
					return STDEVICE_DESCRIPTORNOTFOUND;
			}
		}
		break;

	default:
		break;
	}

	if (nRet==STDEVICE_NOERROR)
		*pNbOfDescriptors=Idx;

	return nRet;
}

DWORD CSTDevice::GetDescriptor(BYTE nLevel, BYTE nType, UINT ConfigIdx, UINT InterfIdx, UINT AltIdx, UINT EndPointIdx, UINT Idx, PBYTE pDesc, UINT nDescSize)
{
	DWORD nRet=STDEVICE_BADPARAMETER;
	UINT NbOf;
	PUSB_CONFIGURATION_DESCRIPTOR ConfDesc;
	PBYTE pTmp;
	int TotalLength;
	UINT Cnt=(UINT)-1;
	UINT CntAlt=1;
	UINT CntInterf=0;
	PUSB_INTERFACE_DESCRIPTOR pPreviousInterf=NULL;
	BOOLEAN _bEndPointFound = FALSE;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	if (!pDesc)
		return STDEVICE_BADPARAMETER;

	if ( (nType==USB_CONFIGURATION_DESCRIPTOR_TYPE) ||
		 (nType==USB_INTERFACE_DESCRIPTOR_TYPE) ||
		 (nType==USB_ENDPOINT_DESCRIPTOR_TYPE) ||
		 (nType==USB_DEVICE_DESCRIPTOR_TYPE) )
		return STDEVICE_BADPARAMETER;

	switch (nLevel)
	{
		case DESCRIPTOR_CONFIGURATION_LEVEL:
			nRet = GetNbOfConfigurations(&NbOf);
			if (nRet != STDEVICE_NOERROR)
				break;

			if (ConfigIdx >= NbOf)
			{
				nRet = STDEVICE_BADPARAMETER;
				break;
			}

			pTmp = (PBYTE)&m_pConfigs[ConfigIdx];
			ConfDesc = (PUSB_CONFIGURATION_DESCRIPTOR)&m_pConfigs[ConfigIdx];
			TotalLength = ConfDesc->wTotalLength;
			pTmp += pTmp[0];

			while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
			{
				if (pTmp[1]==USB_CONFIGURATION_DESCRIPTOR_TYPE)
					return STDEVICE_DESCRIPTORNOTFOUND;

				if (pTmp[1]==USB_INTERFACE_DESCRIPTOR_TYPE)
					return STDEVICE_DESCRIPTORNOTFOUND;

				if (pTmp[1]==USB_ENDPOINT_DESCRIPTOR_TYPE)
					return STDEVICE_DESCRIPTORNOTFOUND;

				if (pTmp[1] == nType)
				{
					Cnt++;
					if (Cnt == Idx)
					{
						if ((int)nDescSize < pTmp[0])
							nRet = STDEVICE_INCORRECTBUFFERSIZE;
						else
						{
							nRet = STDEVICE_NOERROR;
							memcpy(pDesc, pTmp, pTmp[0]);
							break;
						}
					}
				}

				pTmp += pTmp[0];
			}

			break;

	case DESCRIPTOR_INTERFACEALTSET_LEVEL:
			nRet = GetNbOfAlternates(ConfigIdx, InterfIdx, &NbOf);
			if (nRet != STDEVICE_NOERROR)
				break;

			if (AltIdx >= NbOf)
			{
				nRet = STDEVICE_BADPARAMETER;
				break;
			}

			pTmp = (PBYTE)&m_pConfigs[ConfigIdx];
			ConfDesc = (PUSB_CONFIGURATION_DESCRIPTOR)&m_pConfigs[ConfigIdx];
			TotalLength = ConfDesc->wTotalLength;
			pTmp += pTmp[0];

			while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
			{
				if (pTmp[1]==USB_CONFIGURATION_DESCRIPTOR_TYPE)
					return STDEVICE_DESCRIPTORNOTFOUND;

				if (pTmp[1] == USB_INTERFACE_DESCRIPTOR_TYPE)
				{
					PUSB_INTERFACE_DESCRIPTOR pInterf=(PUSB_INTERFACE_DESCRIPTOR)pTmp;

					// increment our index to look for the InterfIdx
					if (pPreviousInterf)
					{
						// See if the current interface descriptor is a different interface or
						// an alternate of the same one
						if (CntInterf==InterfIdx)
						{
							if  (pPreviousInterf->bInterfaceNumber==pInterf->bInterfaceNumber)
							{
								if (CntAlt==AltIdx)
								{
									// We found wanted interface and alt set !
									pTmp+=pTmp[0];
									while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
									{
										if (pTmp[1]==USB_CONFIGURATION_DESCRIPTOR_TYPE)
											return STDEVICE_DESCRIPTORNOTFOUND;

										if (pTmp[1]==USB_INTERFACE_DESCRIPTOR_TYPE)
											return STDEVICE_DESCRIPTORNOTFOUND;

										if (pTmp[1]==USB_ENDPOINT_DESCRIPTOR_TYPE)
											return STDEVICE_DESCRIPTORNOTFOUND;

										if (pTmp[1]==nType)
										{
											Cnt++;
											if (Cnt==Idx)
											{
												if ((int)nDescSize < pTmp[0])
													nRet = STDEVICE_INCORRECTBUFFERSIZE;
												else
												{
													nRet = STDEVICE_NOERROR;
													memcpy(pDesc, pTmp, pTmp[0]);
													break;
												}
											}
										}
										pTmp += pTmp[0];
									}
									break;
								}
								else
									CntAlt++;
							}
							else
							{
								if (CntAlt > 0)
								{
									nRet = STDEVICE_DESCRIPTORNOTFOUND;
									break;
								}
							}
						}
						else
							CntInterf++;
					}
					else
					{
						// Do we need to access interface 0 and altset 0 ?
						if (InterfIdx == 0 && AltIdx == 0)
						{
							// Yes! We are in the good place. Let's search the wanted descriptor
							pTmp += pTmp[0];
							while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
							{
								if (pTmp[1] == USB_CONFIGURATION_DESCRIPTOR_TYPE)
									return STDEVICE_DESCRIPTORNOTFOUND;
								if (pTmp[1] == USB_INTERFACE_DESCRIPTOR_TYPE)
									return STDEVICE_DESCRIPTORNOTFOUND;
								if (pTmp[1] == USB_ENDPOINT_DESCRIPTOR_TYPE)
									return STDEVICE_DESCRIPTORNOTFOUND;
								if (pTmp[1] == nType)
								{
									if (++Cnt == Idx)
									{
										if ((int)nDescSize < pTmp[0])
											nRet = STDEVICE_INCORRECTBUFFERSIZE;
										else
										{
											nRet = STDEVICE_NOERROR;
											memcpy(pDesc, pTmp, pTmp[0]);
											break;
										}
									}
								}
								pTmp += pTmp[0];
							}
							break;
						}
					}
					pPreviousInterf = pInterf;
				}

				pTmp += pTmp[0];
			}

			break;

	case DESCRIPTOR_ENDPOINT_LEVEL:
			nRet = GetNbOfEndPoints(ConfigIdx, InterfIdx, AltIdx, &NbOf);
			if (nRet != STDEVICE_NOERROR)
				break;

			// We must loop on all the interfaces because the endpoint index is based on all the endpoints configured
			if (EndPointIdx >= m_nbEndPoints)
			{
				nRet = STDEVICE_BADPARAMETER;
				break;
			}

			pTmp = (PBYTE)&m_pConfigs[ConfigIdx];
			ConfDesc = (PUSB_CONFIGURATION_DESCRIPTOR)&m_pConfigs[ConfigIdx];
			TotalLength = ConfDesc->wTotalLength;
			pTmp += pTmp[0];

			while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength && !_bEndPointFound)
			{
				if (pTmp[1] == USB_CONFIGURATION_DESCRIPTOR_TYPE)
					return STDEVICE_DESCRIPTORNOTFOUND;

				if (pTmp[1] == USB_INTERFACE_DESCRIPTOR_TYPE)
				{
//					PUSB_INTERFACE_DESCRIPTOR pInterf=(PUSB_INTERFACE_DESCRIPTOR)pTmp;

					pTmp += pTmp[0];

					while (pTmp[1] == USB_ENDPOINT_DESCRIPTOR_TYPE)
					{
						if (Idx == EndPointIdx)
						{
							_bEndPointFound = TRUE;
							break;
						}
						else
						{
							Idx++;
							pTmp += pTmp[0];
						}
					}
				}
				else
					pTmp += pTmp[0];
			}

			if (!_bEndPointFound)
				return STDEVICE_DESCRIPTORNOTFOUND;

			pTmp += pTmp[0];
			while (pTmp - (PBYTE)&m_pConfigs[ConfigIdx] < TotalLength)
			{
				if (pTmp[1]==USB_CONFIGURATION_DESCRIPTOR_TYPE)
					return STDEVICE_DESCRIPTORNOTFOUND;
				if (pTmp[1]==USB_INTERFACE_DESCRIPTOR_TYPE)
					return STDEVICE_DESCRIPTORNOTFOUND;
				if (pTmp[1]==USB_ENDPOINT_DESCRIPTOR_TYPE)
					return STDEVICE_DESCRIPTORNOTFOUND;

				if (pTmp[1]==nType)
				{
					Cnt++;
					if (Cnt==Idx)
					{
						if ((int)nDescSize < pTmp[0])
							nRet = STDEVICE_INCORRECTBUFFERSIZE;
						else
						{
							nRet = STDEVICE_NOERROR;
							memcpy(pDesc, pTmp, pTmp[0]);
							break;
						}
					}
				}
				pTmp+=pTmp[0];
			}

			break;

		default:
			break;
	}

	return nRet;
}

DWORD CSTDevice::SetMaxNumInterruptInputBuffer(WORD nMaxNumInputBuffer)
{
	DWORD ByteCount = 0;
	BYTE InBuffer[3];
	BYTE OutBuffer[2];

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	if (m_pPipeHandles)
		return STDEVICE_PIPESAREOPEN;

	memset(OutBuffer, 0, sizeof(OutBuffer));

	InBuffer[0] = 1;
	InBuffer[1] = nMaxNumInputBuffer >> 0;
	InBuffer[2] = nMaxNumInputBuffer >> 8;

	BOOL Success = ::DeviceIoControl(m_DeviceHandle, PU_NB_FRAMES_BUFFER, InBuffer, sizeof(InBuffer), OutBuffer, sizeof(OutBuffer), &ByteCount, NULL);
	if (Success && ByteCount == sizeof(OutBuffer))
		return STDEVICE_NOERROR;

	return STDEVICE_MEMORY;
}

DWORD CSTDevice::GetMaxNumInterruptInputBuffer(PWORD pMaxNumInputBuffer)
{
	DWORD ByteCount = 0;
	BYTE InBuffer[1];

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	if (m_pPipeHandles)
		return STDEVICE_PIPESAREOPEN;

	if (!pMaxNumInputBuffer)
		return STDEVICE_BADPARAMETER;

	InBuffer[0] = 0;
	BOOL Success = ::DeviceIoControl(m_DeviceHandle, PU_NB_FRAMES_BUFFER, InBuffer, sizeof(InBuffer), pMaxNumInputBuffer, 2, &ByteCount, NULL);
	if (Success && ByteCount == 2)
		return STDEVICE_NOERROR;

	return STDEVICE_MEMORY;
}

DWORD CSTDevice::SetSuspendModeBehaviour(BOOL Allow)
{
	DWORD ByteCount = 0;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	BYTE InBuffer[1];
	InBuffer[0] = Allow ? 1 : 0;

	BOOL success = ::DeviceIoControl(m_DeviceHandle, PU_SUSPEND_CONTROL, InBuffer, sizeof(InBuffer), NULL, 0, &ByteCount, NULL);
	if (success)
		return STDEVICE_NOERROR;

	return STDEVICE_MEMORY;
}

DWORD CSTDevice::ControlPipeRequest(CNTRPIPE_RQ *pRequest, void *pData)
{
	void *pOutBuffer = NULL;
	DWORD nOutNbBytes = 0;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	if (pRequest == NULL)
		return STDEVICE_BADPARAMETER;

//	if (pRequest->Length < 0)
//		pRequest->Length = 0;
		
	int pDriverRq_size = sizeof(CNTRPIPE_RQ);
	uint8_t *pDriverRq = new uint8_t [pDriverRq_size + pRequest->Length];
	if (!pDriverRq)
		return STDEVICE_MEMORY;
	memmove(pDriverRq, pRequest, sizeof(CNTRPIPE_RQ));

	if (pData && pRequest->Length > 0)
	{
		if (pRequest->Direction == VENDOR_DIRECTION_IN)
		{	// receiving data
			nOutNbBytes = pRequest->Length;
			pOutBuffer = pData;
		}
		else
		{	// sending data
			memmove(pDriverRq + sizeof(CNTRPIPE_RQ), pData, pRequest->Length);
			pDriverRq_size += pRequest->Length;
		}
	}

	DWORD ByteCount = 0;
	BOOL res = ::DeviceIoControl(m_DeviceHandle, PU_VENDOR_REQUEST, pDriverRq, pDriverRq_size, pOutBuffer, nOutNbBytes, &ByteCount, NULL);

	delete [] pDriverRq;

	if (!res || ByteCount != nOutNbBytes)
		return STDEVICE_VENDOR_RQ_PB;

	return STDEVICE_NOERROR;
}

DWORD CSTDevice::EndPointControl(UINT nEndPointIdx, UINT nOperation)
{
	BYTE InBuffer[2];
	DWORD ByteCount = 0;
	UINT NbOfEndPoints;
	DWORD nRet;
	USB_ENDPOINT_DESCRIPTOR Desc;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	if (!m_pPipeHandles)
		return STDEVICE_PIPESARECLOSED;

	nRet = GetNbOfEndPoints(m_CurrentConfig, m_CurrentInterf, m_CurrentAltSet, &NbOfEndPoints);
	if (nRet != STDEVICE_NOERROR)
		return nRet;

	nRet = GetEndPointDescriptor(m_CurrentConfig, m_CurrentInterf, m_CurrentAltSet, nEndPointIdx, &Desc);
	if (nRet != STDEVICE_NOERROR)
		return nRet;

	InBuffer[0] = Desc.bEndpointAddress;
	InBuffer[1] = nOperation;

	if (::DeviceIoControl(m_DeviceHandle, PU_PIPE_CONTROL, InBuffer, sizeof(InBuffer), NULL, 0, &ByteCount, NULL))
		return STDEVICE_NOERROR;

	if (nOperation == PIPE_RESET)
		return STDEVICE_PIPERESETERROR;

	return STDEVICE_PIPEABORTERROR;
}

DWORD CSTDevice::Reset()
{
	BYTE InBuffer[2];
	DWORD ByteCount = 0;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	InBuffer[0] = 0;
	InBuffer[1] = 2; // RESET CODE

	if (::DeviceIoControl(m_DeviceHandle, PU_PIPE_CONTROL, InBuffer, sizeof(InBuffer), NULL, 0, &ByteCount, NULL))
		return STDEVICE_NOERROR;

	return STDEVICE_DEVICERESETERROR;
}

DWORD CSTDevice::Read(UINT nEndPointIdx, PBYTE pBuffer, PUINT pSize, DWORD nTimeOut)
{
	DWORD		ByteCount;
	HANDLE		hWait;
	OVERLAPPED	OverLapped;
	DWORD		nDelay;
	DWORD		nRet;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	if (!m_pPipeHandles)
		return STDEVICE_PIPESARECLOSED;

	if (!pBuffer || !pSize)
		return STDEVICE_BADPARAMETER;

	if (*pSize <= 0)
		return STDEVICE_BADPARAMETER;

	// Update to support the read from all the available endpoints on all the configured interfaces
	if (nEndPointIdx >= m_nbEndPoints)
		return STDEVICE_BADPARAMETER;

	if (nTimeOut)
		nDelay = nTimeOut;
	else
		nDelay = m_nDefaultTimeOut;

	hWait = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	OverLapped.Offset = 0;
	OverLapped.OffsetHigh = 0;
	OverLapped.hEvent = hWait;

	if (::ReadFile(m_pPipeHandles[nEndPointIdx], pBuffer, *pSize, &ByteCount, &OverLapped))
	{
		*pSize = ByteCount;
		return STDEVICE_NOERROR;
	}

	nRet = ::GetLastError();
	if (nRet != ERROR_IO_PENDING)
	{
		::CloseHandle(hWait);
		return STDEVICE_ERRORBEFOREREADING;
	}

	nRet = ::WaitForSingleObject(hWait, nDelay);
	::CloseHandle(hWait);
	if (nRet != WAIT_OBJECT_0)
		return STDEVICE_ERRORWHILEREADING;

	if (!::GetOverlappedResult(m_pPipeHandles[nEndPointIdx], &OverLapped, (DWORD*)pSize, FALSE))
	{
//		nRet = ::GetLastError();
		return STDEVICE_ERRORWHILEREADING;
	}

	return STDEVICE_NOERROR;
}

DWORD CSTDevice::Write(UINT nEndPointIdx, PBYTE pBuffer, PUINT pSize, DWORD nTimeOut)
{
	DWORD		ByteCount;
	HANDLE		hWait;
	OVERLAPPED	OverLapped;
	DWORD		nDelay;

	if (!m_bDeviceIsOpen)
		return STDEVICE_DRIVERISCLOSED;

	if (!m_pPipeHandles)
		return STDEVICE_PIPESARECLOSED;

	if (!pBuffer || !pSize)
		return STDEVICE_BADPARAMETER;

	if (*pSize <= 0)
		return STDEVICE_BADPARAMETER;

	// Update to support the write from all the available endpoints on all the configured interfaces
	if (nEndPointIdx >= m_nbEndPoints)
		return STDEVICE_BADPARAMETER;

	if (nTimeOut)
		nDelay = nTimeOut;
	else
		nDelay = m_nDefaultTimeOut;

	hWait = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	OverLapped.Offset = 0;
	OverLapped.OffsetHigh = 0;
	OverLapped.hEvent = hWait;

	if (::WriteFile(m_pPipeHandles[nEndPointIdx], pBuffer, *pSize, &ByteCount, &OverLapped))
	{
		*pSize = ByteCount;
		return STDEVICE_NOERROR;
	}

	DWORD res = ::GetLastError();
	if (res != ERROR_IO_PENDING)
	{
		::CloseHandle(hWait);
		return STDEVICE_ERRORBEFOREREADING;
	}

	res = ::WaitForSingleObject(hWait, nDelay);
	::CloseHandle(hWait);
	if (res != WAIT_OBJECT_0)
		return STDEVICE_ERRORWHILEREADING;

	if (!::GetOverlappedResult(m_pPipeHandles[nEndPointIdx], &OverLapped, (DWORD *)pSize, FALSE))
		return STDEVICE_ERRORWHILEREADING;

	return STDEVICE_NOERROR;
}

// *************************************************

TSTDFU::TSTDFU()
{
	pDevice = NULL;
}

TSTDFU::~TSTDFU()
{
	close();
}

//  Gets an handle on the device
DWORD TSTDFU::open(LPSTR szDevicePath)
{
	if (!szDevicePath)
		return STDEVICE_BADPARAMETER;
	if (szDevicePath[0] == '\0')
		return STDEVICE_BADPARAMETER;

	close();

	pDevice = new CSTDevice(szDevicePath);
	if (!pDevice)
		return STDFU_OPENDRIVERERROR;

//	DWORD res = pDevice->Open(phUnPlugEvent);
	DWORD res = pDevice->Open(NULL);
	if (res != STDEVICE_NOERROR)
		close();

	return res;
}

//  Releases an handle on the device
DWORD TSTDFU::close()
{
	if (pDevice)
		delete pDevice;
	pDevice = NULL;

	return STDFU_NOERROR;
}

//  Get the device descriptor
DWORD TSTDFU::getDeviceDescriptor(PUSB_DEVICE_DESCRIPTOR pDesc)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	if (!pDesc)
		return STDFU_BADPARAMETER;

	return pDevice->GetDeviceDescriptor(pDesc);
}

//  Get a string descriptor
DWORD TSTDFU::getStringDescriptor(DWORD Index, LPSTR szString, UINT nStringLength)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	if (!szString || nStringLength <= 0)
		return STDFU_BADPARAMETER;

	char sString[512];
	memset(sString, 0, sizeof(sString));

	DWORD res = pDevice->GetStringDescriptor(Index, (LPSTR)sString);
	if (res != STDEVICE_NOERROR)
		return res;

	memset(szString, 0, nStringLength);
	int i = strlen(sString);
	if (i > (int)nStringLength - 1)
		i = nStringLength - 1;
	memmove(szString, sString, i);

	return STDFU_NOERROR;
}

DWORD TSTDFU::getNbOfConfigurations(PUINT pNbOfConfigs)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	if (!pNbOfConfigs)
		return STDFU_BADPARAMETER;

	return pDevice->GetNbOfConfigurations(pNbOfConfigs);
}

DWORD TSTDFU::getConfigurationDescriptor(UINT nConfigIdx, PUSB_CONFIGURATION_DESCRIPTOR pDesc)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	if (!pDesc)
		return STDFU_BADPARAMETER;

	return pDevice->GetConfigurationDescriptor(nConfigIdx, pDesc);
}

DWORD TSTDFU::getNbOfInterfaces(UINT nConfigIdx, PUINT pNbOfInterfaces)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	if (!pNbOfInterfaces)
		return STDFU_BADPARAMETER;

	return pDevice->GetNbOfInterfaces(nConfigIdx, pNbOfInterfaces);
}

DWORD TSTDFU::getNbOfAlternates(UINT nConfigIdx, UINT nInterfaceIdx, PUINT pNbOfAltSets)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	if (!pNbOfAltSets)
		return STDFU_BADPARAMETER;

	return pDevice->GetNbOfAlternates(nConfigIdx, nInterfaceIdx, pNbOfAltSets);
}

DWORD TSTDFU::getInterfaceDescriptor(UINT nConfigIdx, UINT nInterfaceIdx, UINT nAltSetIdx, PUSB_INTERFACE_DESCRIPTOR pDesc)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	if (!pDesc)
		return STDFU_BADPARAMETER;

	return pDevice->GetInterfaceDescriptor(nConfigIdx, nInterfaceIdx, nAltSetIdx, pDesc);
}

//  Get the DFU descriptor
DWORD TSTDFU::getDFUDescriptor(PUINT pDFUInterfaceNum, PUINT pNbOfAlternates, DFU_FUNCTIONAL_DESCRIPTOR *pDesc)
{
	DWORD Ret;
	UINT NbOfAltSets;
	UINT NbOfInterfaces;
	USB_INTERFACE_DESCRIPTOR ItfDesc;

	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	if (!pDFUInterfaceNum || !pNbOfAlternates || !pDesc)
		return STDFU_BADPARAMETER;

	// DFU itnerface should alays be the latest one
	Ret = getNbOfInterfaces(0, &NbOfInterfaces);
	if (Ret != STDEVICE_NOERROR)
		return Ret;

	Ret = getNbOfAlternates(0, NbOfInterfaces - 1, &NbOfAltSets);
	if (Ret != STDEVICE_NOERROR)
		return Ret;

	Ret = pDevice->GetDescriptor(	DESCRIPTOR_INTERFACEALTSET_LEVEL,
									0x21,
									0,					// Configuration 0
									NbOfInterfaces - 1,
									NbOfAltSets - 1,	// Func desc should be on latest alt set
									0,					// Unused
									0,					// Index of DFU Interface: the first one
									(PBYTE)pDesc,
									sizeof(DFU_FUNCTIONAL_DESCRIPTOR));
	if (Ret != STDEVICE_NOERROR)
		return Ret;

	Ret = pDevice->GetInterfaceDescriptor(0, NbOfInterfaces-1, 0, &ItfDesc);
	if (Ret != STDEVICE_NOERROR)
		return Ret;

	*pNbOfAlternates = NbOfAltSets;
	*pDFUInterfaceNum = ItfDesc.bInterfaceNumber;

	return Ret;
}

// ******************************************************************************
// selects the currently active mode for a device, giving the configuration, the interface and the alternate setting.

DWORD TSTDFU::selectCurrentConfiguration(UINT nConfigIdx, UINT nInterfaceIdx, UINT nAltSetIdx)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	return pDevice->SelectCurrentConfiguration(nConfigIdx, nInterfaceIdx, nAltSetIdx);
}

//  DFU DETACH REQUEST
DWORD TSTDFU::detach(char DFUInterfaceNumber)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	CNTRPIPE_RQ Request;
	memset(&Request, 0, sizeof(CNTRPIPE_RQ));
	Request.Function = URB_FUNCTION_CLASS_INTERFACE;
	Request.Direction = VENDOR_DIRECTION_OUT;
	Request.Request = DFU_DETACH;
	Request.Value = 5000;
	Request.Index = DFUInterfaceNumber;
	Request.Length = 0;

	DWORD res = pDevice->ControlPipeRequest(&Request, NULL);
	if (res != STDEVICE_NOERROR)
		return res;

	close();

	return res;
}

//  DFU DOWNLOAD REQUEST
DWORD TSTDFU::download(void *pBuffer, ULONG nBytes, USHORT nBlock)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	CNTRPIPE_RQ Request;
	memset(&Request, 0, sizeof(CNTRPIPE_RQ));
	Request.Function = URB_FUNCTION_CLASS_INTERFACE;
	Request.Direction = VENDOR_DIRECTION_OUT;
	Request.Request = DFU_DNLOAD;
	Request.Value = nBlock;
	Request.Index = 0;
	Request.Length = nBytes;

	DWORD res = pDevice->ControlPipeRequest(&Request, pBuffer);
	if (res != STDFU_NOERROR)
		res = ::GetLastError();

	return res;
}

//  DFU UPLOAD REQUEST
DWORD TSTDFU::upload(void *pBuffer, ULONG nBytes, USHORT nBlock)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	CNTRPIPE_RQ Request;
	memset(&Request, 0, sizeof(CNTRPIPE_RQ));
	Request.Function = URB_FUNCTION_CLASS_INTERFACE;
	Request.Direction = VENDOR_DIRECTION_IN;
	Request.Request = DFU_UPLOAD;
	Request.Value = nBlock;
	Request.Index = 0;
	Request.Length = nBytes;

	DWORD res = pDevice->ControlPipeRequest(&Request, pBuffer);
	if (res != STDFU_NOERROR)
		res = ::GetLastError();

	return res;
}

//  DFU GETSTATUS REQUEST
DWORD TSTDFU::getStatus(DFUSTATUS *DfuStatus)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	if (!DfuStatus)
		return STDFU_BADPARAMETER;

	memset(DfuStatus, 0, sizeof(DFUSTATUS));

	CNTRPIPE_RQ Request;
	memset(&Request, 0, sizeof(CNTRPIPE_RQ));
	Request.Function = URB_FUNCTION_CLASS_INTERFACE;
	Request.Direction = VENDOR_DIRECTION_IN;
	Request.Request = DFU_GETSTATUS;
	Request.Value = 0;
	Request.Index = 0;
	Request.Length = sizeof(DFUSTATUS);

	DWORD res = pDevice->ControlPipeRequest(&Request, DfuStatus);
	if (res == STDEVICE_NOERROR && (DfuStatus->bState == STATE_DFU_MANIFEST_WAIT_RESET || DfuStatus->bState == STATE_DFU_MANIFEST))
		close();

	return res;
}

//  DFU CLRSTATUS REQUEST
DWORD TSTDFU::clrStatus()
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	CNTRPIPE_RQ Request;
	memset(&Request, 0, sizeof(CNTRPIPE_RQ));
	Request.Function = URB_FUNCTION_CLASS_INTERFACE;;
	Request.Direction = VENDOR_DIRECTION_OUT;
	Request.Request = DFU_CLRSTATUS;
	Request.Value = 0;
	Request.Index = 0;
	Request.Length = 0;

	DWORD res = pDevice->ControlPipeRequest(&Request, NULL);
	if (res != STDFU_NOERROR)
		res = ::GetLastError();

	return res;
}

//  DFU GETSTATE REQUEST
DWORD TSTDFU::getState(char *pState)
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	if (!pState)
		return STDFU_BADPARAMETER;

	*pState = 0;
	
	CNTRPIPE_RQ Request;
	memset(&Request, 0, sizeof(CNTRPIPE_RQ));
	Request.Function = URB_FUNCTION_CLASS_INTERFACE;;
	Request.Direction = VENDOR_DIRECTION_IN;
	Request.Request = DFU_GETSTATE;
	Request.Value = 0;
	Request.Index = 0;
	Request.Length = 1;

	DWORD res = pDevice->ControlPipeRequest(&Request, pState);
	if (res != STDFU_NOERROR)
		res = ::GetLastError();

	return res;
}

//  DFU ABORT REQUEST
DWORD TSTDFU::abort()
{
	if (!pDevice)
		return STDFU_DRIVERISCLOSED;

	CNTRPIPE_RQ Request;
	memset(&Request, 0, sizeof(CNTRPIPE_RQ));
	Request.Function = URB_FUNCTION_CLASS_INTERFACE;;
	Request.Direction = VENDOR_DIRECTION_OUT;
	Request.Request = DFU_ABORT;
	Request.Value = 0;
	Request.Index = 0;
	Request.Length = 0;

	DWORD res = pDevice->ControlPipeRequest(&Request, NULL);
	if (res != STDFU_NOERROR)
		res = ::GetLastError();

	return res;
}

// *************************************************

