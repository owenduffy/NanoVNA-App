
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef st_dfuH
#define st_dfuH

#include <stdint.h>
#include <vector>

//#include <windows.h>
#include "USB100.H"

// *************************************************
// STTubeDevice

#define STDEVICE_ERROR_OFFSET				0x12340000

#define STDEVICE_NOERROR					STDEVICE_ERROR_OFFSET
#define STDEVICE_MEMORY						(STDEVICE_ERROR_OFFSET+1)
#define STDEVICE_BADPARAMETER				(STDEVICE_ERROR_OFFSET+2)

#define STDEVICE_NOTIMPLEMENTED				(STDEVICE_ERROR_OFFSET+3)
#define STDEVICE_ENUMFINISHED				(STDEVICE_ERROR_OFFSET+4)
#define STDEVICE_OPENDRIVERERROR			(STDEVICE_ERROR_OFFSET+5)

#define STDEVICE_ERRORDESCRIPTORBUILDING	(STDEVICE_ERROR_OFFSET+6)
#define STDEVICE_PIPECREATIONERROR			(STDEVICE_ERROR_OFFSET+7)
#define STDEVICE_PIPERESETERROR				(STDEVICE_ERROR_OFFSET+8)
#define STDEVICE_PIPEABORTERROR				(STDEVICE_ERROR_OFFSET+9)
#define STDEVICE_STRINGDESCRIPTORERROR		(STDEVICE_ERROR_OFFSET+0xA)

#define STDEVICE_DRIVERISCLOSED				(STDEVICE_ERROR_OFFSET+0xB)
#define STDEVICE_VENDOR_RQ_PB				(STDEVICE_ERROR_OFFSET+0xC)
#define STDEVICE_ERRORWHILEREADING			(STDEVICE_ERROR_OFFSET+0xD)
#define STDEVICE_ERRORBEFOREREADING			(STDEVICE_ERROR_OFFSET+0xE)
#define STDEVICE_ERRORWHILEWRITING			(STDEVICE_ERROR_OFFSET+0xF)
#define STDEVICE_ERRORBEFOREWRITING			(STDEVICE_ERROR_OFFSET+0x10)
#define STDEVICE_DEVICERESETERROR			(STDEVICE_ERROR_OFFSET+0x11)
#define STDEVICE_CANTUSEUNPLUGEVENT			(STDEVICE_ERROR_OFFSET+0x12)
#define STDEVICE_INCORRECTBUFFERSIZE		(STDEVICE_ERROR_OFFSET+0x13)
#define STDEVICE_DESCRIPTORNOTFOUND			(STDEVICE_ERROR_OFFSET+0x14)
#define STDEVICE_PIPESARECLOSED				(STDEVICE_ERROR_OFFSET+0x15)
#define STDEVICE_PIPESAREOPEN				(STDEVICE_ERROR_OFFSET+0x16)

#define DESCRIPTOR_CONFIGURATION_LEVEL				0
#define DESCRIPTOR_INTERFACEALTSET_LEVEL			1
#define DESCRIPTOR_ENDPOINT_LEVEL					2

#define URB_FUNCTION_VENDOR_DEVICE                   0x0017
#define URB_FUNCTION_VENDOR_INTERFACE                0x0018
#define URB_FUNCTION_VENDOR_ENDPOINT                 0x0019
#define URB_FUNCTION_VENDOR_OTHER                    0x0020

#define URB_FUNCTION_CLASS_DEVICE                    0x001A
#define URB_FUNCTION_CLASS_INTERFACE                 0x001B
#define URB_FUNCTION_CLASS_ENDPOINT                  0x001C
#define URB_FUNCTION_CLASS_OTHER                     0x001F

#define PIPE_RESET						1
#define ABORT_TRANSFER					0

#define VENDOR_DIRECTION_IN				1
#define VENDOR_DIRECTION_OUT			0

typedef struct
{
	USHORT Function;
	ULONG Direction;
	char Request;
	USHORT Value;
	USHORT Index;
	ULONG Length;
} CNTRPIPE_RQ;

typedef struct
{
	BYTE config[512];
} FULL_CONFIG_INFO;

// **********************************************************************

class CSTDevice
{
private:
	int m_CurrentConfig;
	int m_CurrentInterf;
	int m_CurrentAltSet;

	char m_SymbolicName[512];
	HANDLE m_DeviceHandle;
	BOOL m_bDeviceIsOpen;
	DWORD m_nDefaultTimeOut;

	USB_DEVICE_DESCRIPTOR m_DeviceDescriptor;
	std::vector <FULL_CONFIG_INFO> m_pConfigs;
	HANDLE *m_pPipeHandles;
	UINT m_nbEndPoints;

	DWORD OpenDevice(PHANDLE phUnPlugEvent);
	DWORD CloseDevice();
	DWORD AllocDescriptors();
	DWORD ReleaseDescriptors();

public:
	CSTDevice(LPSTR sSymbolicName);
	~CSTDevice();

	// Connection state
	DWORD Open(PHANDLE);
	DWORD Close();
	DWORD OpenPipes();
	DWORD ClosePipes();
	DWORD SelectCurrentConfiguration(UINT ConfigIdx, UINT InterfaceIdx, UINT AltSetIdx);
	DWORD ControlPipeRequest(CNTRPIPE_RQ *pRequest, void *pData);
	DWORD EndPointControl(UINT nEndPointIdx, UINT nOperation);
	DWORD Reset();
	DWORD Read(UINT nEndPointIdx, PBYTE pBuffer, PUINT pSize, DWORD nTimeOut);
	DWORD Write(UINT nEndPointIdx, PBYTE pBuffer, PUINT pSize, DWORD nTimeOut);

	// Accessors
	// - Default Time out
	DWORD SetDefaultTimeOut(DWORD nTimeOut)
	{
		m_nDefaultTimeOut = nTimeOut;
		return STDEVICE_NOERROR;
	}

	// - Number of interrupt IN frames to bufferize
	DWORD SetMaxNumInterruptInputBuffer(WORD nMaxNumInputBuffer);
	DWORD GetMaxNumInterruptInputBuffer(PWORD pMaxNumInputBuffer);

	// - String descriptors
	DWORD GetStringDescriptor(UINT Index, LPSTR Desc);
	// - Device descriptors
	DWORD GetDeviceDescriptor(PUSB_DEVICE_DESCRIPTOR Desc);

	// - Config numbers
	DWORD GetNbOfConfigurations(PUINT);
	// - Config descriptors
	DWORD GetConfigurationDescriptor(UINT ConfigIdx, PUSB_CONFIGURATION_DESCRIPTOR Desc);

	// - Interface numbers
	DWORD GetNbOfInterfaces(UINT ConfigIdx, PUINT);
	// - Alternate Settings numbers
	DWORD GetNbOfAlternates(UINT ConfigIdx, UINT InterfIdx, PUINT);
	// - Interface descriptors
	DWORD GetInterfaceDescriptor(UINT ConfigIdx, UINT InterfIdx, UINT AltIdx, PUSB_INTERFACE_DESCRIPTOR Desc);

	// - Endpoint numbers
	DWORD GetNbOfEndPoints(UINT ConfigIdx, UINT InterfIdx, UINT AltIdx, PUINT);
	// - Endpoint descriptors
	DWORD GetEndPointDescriptor(UINT ConfigIdx, UINT InterfIdx, UINT AltIdx, UINT EndPointIdx, PUSB_ENDPOINT_DESCRIPTOR Desc);
	// - Other descriptors (Class f.i)
	//   * Nb of descriptors
	DWORD GetNbOfDescriptors(BYTE nLevel, BYTE nType, UINT ConfigIdx, UINT InterfIdx, UINT AltIdx, UINT EndPointIdx, PUINT pNbOfDescriptors);
	//   * Descriptor itself
	DWORD GetDescriptor(BYTE nLevel, BYTE nType, UINT ConfigIdx, UINT InterfIdx, UINT AltIdx, UINT EndPointIdx, UINT nIdx, PBYTE pDesc, UINT nDescSize);

	// - Get symbolic name of the driver
	LPSTR GetSymbolicName()
	{
		return (LPSTR)m_SymbolicName;
	}

	// - Set current driver behaviour regarding Suspend mode
	DWORD SetSuspendModeBehaviour(BOOL Allow);
};

// *************************************************

#define STDFU_ERROR_OFFSET				0x12340000

#define STDFU_NOERROR					STDFU_ERROR_OFFSET
#define STDFU_MEMORY					(STDFU_ERROR_OFFSET+1)
#define STDFU_BADPARAMETER				(STDFU_ERROR_OFFSET+2)

#define STDFU_NOTIMPLEMENTED			(STDFU_ERROR_OFFSET+3)
#define STDFU_ENUMFINISHED				(STDFU_ERROR_OFFSET+4)
#define STDFU_OPENDRIVERERROR			(STDFU_ERROR_OFFSET+5)

#define STDFU_ERRORDESCRIPTORBUILDING	(STDFU_ERROR_OFFSET+6)
#define STDFU_PIPECREATIONERROR			(STDFU_ERROR_OFFSET+7)
#define STDFU_PIPERESETERROR			(STDFU_ERROR_OFFSET+8)
#define STDFU_PIPEABORTERROR			(STDFU_ERROR_OFFSET+9)
#define STDFU_STRINGDESCRIPTORERROR		(STDFU_ERROR_OFFSET+0xA)

#define STDFU_DRIVERISCLOSED			(STDFU_ERROR_OFFSET+0xB)
#define STDFU_VENDOR_RQ_PB				(STDFU_ERROR_OFFSET+0xC)
#define STDFU_ERRORWHILEREADING			(STDFU_ERROR_OFFSET+0xD)
#define STDFU_ERRORBEFOREREADING		(STDFU_ERROR_OFFSET+0xE)
#define STDFU_ERRORWHILEWRITING			(STDFU_ERROR_OFFSET+0xF)
#define STDFU_ERRORBEFOREWRITING		(STDFU_ERROR_OFFSET+0x10)
#define STDFU_DEVICERESETERROR			(STDFU_ERROR_OFFSET+0x11)
#define STDFU_CANTUSEUNPLUGEVENT		(STDFU_ERROR_OFFSET+0x12)
#define STDFU_INCORRECTBUFFERSIZE		(STDFU_ERROR_OFFSET+0x13)
#define STDFU_DESCRIPTORNOTFOUND		(STDFU_ERROR_OFFSET+0x14)
#define STDFU_PIPESARECLOSED			(STDFU_ERROR_OFFSET+0x15)
#define STDFU_PIPESAREOPEN				(STDFU_ERROR_OFFSET+0x16)

#define STDFU_TIMEOUTWAITINGFORRESET	(STDFU_ERROR_OFFSET+0x17)

#define STDFU_RQ_GET_DEVICE_DESCRIPTOR			0x02000000
#define STDFU_RQ_GET_DFU_DESCRIPTOR				0x03000000
#define STDFU_RQ_GET_STRING_DESCRIPTOR			0x04000000
#define STDFU_RQ_GET_NB_OF_CONFIGURATIONS		0x05000000
#define STDFU_RQ_GET_CONFIGURATION_DESCRIPTOR	0x06000000
#define STDFU_RQ_GET_NB_OF_INTERFACES			0x07000000
#define STDFU_RQ_GET_NB_OF_ALTERNATES			0x08000000
#define STDFU_RQ_GET_INTERFACE_DESCRIPTOR		0x09000000
#define STDFU_RQ_OPEN							0x0A000000
#define STDFU_RQ_CLOSE							0x0B000000
#define STDFU_RQ_DETACH							0x0C000000
#define STDFU_RQ_DOWNLOAD						0x0D000000
#define STDFU_RQ_UPLOAD							0x0E000000
#define STDFU_RQ_GET_STATUS						0x0F000000
#define STDFU_RQ_CLR_STATUS						0x10000000
#define STDFU_RQ_GET_STATE						0x11000000
#define STDFU_RQ_ABORT							0x12000000
#define STDFU_RQ_SELECT_ALTERNATE				0x13000000
#define STDFU_RQ_AWAITINGPNPUNPLUGEVENT			0x14000000
#define STDFU_RQ_AWAITINGPNPPLUGEVENT			0x15000000
#define STDFU_RQ_IDENTIFYINGDEVICE				0x16000000

// *************************************************
// DFU States

#define STATE_IDLE							0x00
#define STATE_DETACH						0x01
#define STATE_DFU_IDLE						0x02
#define STATE_DFU_DOWNLOAD_SYNC				0x03
#define STATE_DFU_DOWNLOAD_BUSY				0x04
#define STATE_DFU_DOWNLOAD_IDLE				0x05
#define STATE_DFU_MANIFEST_SYNC				0x06
#define STATE_DFU_MANIFEST					0x07
#define STATE_DFU_MANIFEST_WAIT_RESET		0x08
#define STATE_DFU_UPLOAD_IDLE				0x09
#define STATE_DFU_ERROR						0x0A

#define STATE_DFU_UPLOAD_SYNC				0x91
#define STATE_DFU_UPLOAD_BUSY				0x92

// *************************************************
// DFU Status

#define STATUS_OK							0x00
#define STATUS_errTARGET					0x01
#define STATUS_errFILE						0x02
#define STATUS_errWRITE						0x03
#define STATUS_errERASE						0x04
#define STATUS_errCHECK_ERASE				0x05
#define STATUS_errPROG						0x06
#define STATUS_errVERIFY					0x07
#define STATUS_errADDRESS					0x08
#define STATUS_errNOTDONE					0x09
#define STATUS_errFIRMWARE					0x0A
#define STATUS_errVENDOR					0x0B
#define STATUS_errUSBR						0x0C
#define STATUS_errPOR						0x0D
#define STATUS_errUNKNOWN					0x0E
#define STATUS_errSTALLEDPKT				0x0F

#define ATTR_DNLOAD_CAPABLE					0x01
#define ATTR_UPLOAD_CAPABLE					0x02
#define ATTR_MANIFESTATION_TOLERANT			0x04
#define ATTR_WILL_DETACH					0x08
#define ATTR_ST_CAN_ACCELERATE				0x80

// *************************************************

#pragma pack(push, 1)

#define TYPE_DFUSTATUS
typedef struct
{
	char bStatus;
	char bwPollTimeout[3];
	char bState;
	char iString;
} DFUSTATUS;

typedef struct _DFU_FUNCTIONAL_DESCRIPTOR
{
	char bLength;
	char bDescriptorType; // Should be 0x21
	char bmAttributes;
	USHORT wDetachTimeOut;
	USHORT wTransfertSize;
	USHORT bcdDFUVersion;
} DFU_FUNCTIONAL_DESCRIPTOR;

#pragma pack(pop)

// *************************************************

class TSTDFU
{
private:
	CSTDevice *pDevice;

public:
	TSTDFU();
	~TSTDFU();

	bool isOpen() { return (pDevice) ? true : false; }

	//******************************************************************************
	// STDFU_GetDeviceDescriptor : Gets the device descriptor
	// Parameters:
	//   Num: Number of the selected DFU Device (Given by STDFU_EnumGetNbDevices)
	//   pDesc: buffer the descriptor will be copied to.
	// Returned: STDFU_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD getDeviceDescriptor(PUSB_DEVICE_DESCRIPTOR pDesc);

	//******************************************************************************
	// STDFU_GetDFUDescriptor :  Gets the DFU descriptor
	// Parameters:
	//   Num: Number of the selected DFU Device (Given by STDFU_EnumGetNbDevices)
	//   pDesc: buffer the DFU descriptor will be copied to.
	// Returned: STDFU_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD getDFUDescriptor(PUINT pDFUInterfaceNum, PUINT pNbOfAlternates, DFU_FUNCTIONAL_DESCRIPTOR *pDesc);

	//******************************************************************************
	// STDFU_GetStringDescriptor: Gets the string descriptor
	// Parameters:
	//   Num: Number of the selected DFU Device (Given by STDFU_EnumGetNbDevices)
	//   nIndex: desired string descriptor Index. If this index is too high, this
	//           function will return an error.
	//   szString: buffer the string descriptor will be copied to
	//   nStringLength: bufffer size
	// Returned: STDFU_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD getStringDescriptor(DWORD Index, LPSTR szString, UINT nStringLength);

	//******************************************************************************
	// STDFU_GetNbOfConfigurations: Gets Configurations number
	// Parameters:
	//   Num: Number of the selected DFU Device (Given by STDFU_EnumGetNbDevices)
	//   pNbOfConfigs: pointer to the configuration's number
	// Returned: STDFU_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD getNbOfConfigurations(PUINT pNbOfConfigs);

	//******************************************************************************
	// STDFU_GetConfigurationDescriptor: Gets the congiguration descriptor
	// Parameters:
	//   Num: Number of the selected DFU Device (Given by STDFU_EnumGetNbDevices)
	//   nConfigIdx : Number of the selected Configuration
	//   pDesc: buffer the descriptor will be copied to.
	// Returned: STDFU_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD getConfigurationDescriptor(UINT nConfigIdx, PUSB_CONFIGURATION_DESCRIPTOR pDesc);

	//******************************************************************************
	// STDFU_GetNbOfInterfaces: Gets Interfaces Number
	// Parameters:
	//   Num: Number of the selected DFU Device (Given by STDFU_EnumGetNbDevices)
	//   nConfigIdx : Number of the selected Configuration
	//   pNbOfInterfaces: pointer to Interfaces Number
	// Returned: STDFU_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD getNbOfInterfaces(UINT nConfigIdx, PUINT pNbOfInterfaces);

	//******************************************************************************
	// STDFU_GetNbOfAlternates: Gets Alternate Setting's Number
	// Parameters:
	//   Num: Number of the selected DFU Device (Given by STDFU_EnumGetNbDevices)
	//   nConfigIdx : Number of the selected Configuration
	//   nInterfaceIdx : Number of the selected Interface
	//   pNbOfAltSets: pointer to Alternate Setting's Number
	// Returned: STDFU_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD getNbOfAlternates(UINT nConfigIdx, UINT nInterfaceIdx, PUINT pNbOfAltSets);

	//******************************************************************************
	// STDFU_GetInterfaceDescriptor: Gets the interface descriptor
	// Parameters:
	//   Num: Number of the selected DFU Device (Given by STDFU_EnumGetNbDevices)
	//   nConfigIdx : Number of the selected Configuration
	//   nInterfaceIdx : Number of the selected Interface
	//   nAltSetIdx : Number of the selected Alternate Setting
	//   pDesc: buffer the descriptor will be copied to.
	// Returned: STDFU_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD getInterfaceDescriptor(UINT nConfigIdx, UINT nInterfaceIdx, UINT nAltSetIdx, PUSB_INTERFACE_DESCRIPTOR pDesc);

	//******************************************************************************
	// STDFU_Open: Opens the DFU driver, giving access to its descriptor
	// Parameters:
	//	Num: Number of the selected DFU Device (Given by STDFU_EnumGetNbDevices)
	//
	//  phDevice: handle returned by the function while the driver is successfully
	//             open
	// Returned: STDFU_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD open(LPSTR szDevicePath);

	//******************************************************************************
	// STDFU_Close: Closes the DFU driver
	// Parameters:
	//   phDevice: pointer to handle returned by the function STDevice_Open
	// Returned: STDFU_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD close();

	//******************************************************************************
	// STDFU_Detach: issues a Detach request on the Control Pipe (Endpoint0)
	//
	// Parameters:
	//   phDevice: pointer to handle returned by the function STDevice_Open
	// Returned: STDEVICE_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD detach(char DFUInterfaceNumber );

	//******************************************************************************
	// STDFU_SelectCurrentConfiguration: selects the currently active mode for
	//              a device, giving the configuration, the interface and the
	//              alternate setting. The pipes must be in closed state for this
	//              function to success
	// Parameters:
	//   hDevice: handle returned by the function STDevice_Open
	//   nConfigIdx: number of the desired configuration
	//   nInterfaceIdx: number of the desired interface
	//   nAltSetIdx: number of the desired alternate setting
	// Returned: STDEVICE_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD selectCurrentConfiguration(UINT nConfigIdx, UINT nInterfaceIdx, UINT nAltSetIdx);

	//******************************************************************************
	// STDFU_Dnload: issues a Download request on the Control Pipe (Endpoint0)
	//
	// Parameters:
	//   phDevice: pointer to handle returned by the function STDevice_Open
	//   Buffer: Buffer of Data
	//   nBytes: Number of data to be downloaded
	//   nBlock: Number of data block to be downloaded
	//   nAlternate: Number of Alternate Settings
	// Returned: STDEVICE_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD download(void *pBuffer, ULONG nBytes, USHORT nBlock);

	//******************************************************************************
	// STDFU_Upload: issues an Upload request on the Control Pipe (Endpoint0)
	//
	// Parameters:
	//   phDevice: pointer to handle returned by the function STDevice_Open
	//   Buffer: Buffer of Data
	// 	 nBytes: Number of data to be uploaded
	//   nBlock: Number of data block to be uploaded
	// Returned: STDEVICE_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD upload(void *pBuffer, ULONG nBytes, USHORT nBlock);

	//******************************************************************************
	// STDFU_Getstatus: issues a GetStatus request on the Control Pipe (Endpoint0)
	//
	// Parameters:
	//   phDevice: pointer to handle returned by the function STDevice_Open
	//   DfuStatus: structure containing DFU Status structure
	// Returned: STDEVICE_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD getStatus(DFUSTATUS *DfuStatus);

	//******************************************************************************
	// STDFU_Clrstatus: issues a ClearStatus request on the Control Pipe (Endpoint0)
	//
	// Parameters:
	//   phDevice: pointer to handle returned by the function STDevice_Open
	// Returned: STDEVICE_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD clrStatus();

	//******************************************************************************
	// STDFU_Getstate: issues a GetState request on the Control Pipe (Endpoint0)
	//
	// Parameters:
	//   phDevice: pointer to handle returned by the function STDevice_Open
	//	 pState  : pointer to a DFU State
	// Returned: STDEVICE_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD getState(char *pState);

	//******************************************************************************
	// STDFU_Abort: issues an Abort request on the Control Pipe (Endpoint0)
	//
	// Parameters:
	//   phDevice: pointer to handle returned by the function STDevice_Open
	// Returned: STDEVICE_NOERROR = SUCCESS, Error otherwise (Error Code)
	//******************************************************************************
	DWORD abort();
};

#endif

