
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#include <vcl.h>
#include <stdio.h>
//#include <intrin.h>

#pragma hdrstop

#include "UploadFirmwareUnit.h"
#include "DataUnit.h"
#include "Unit1.h"
#include "common.h"
#include "Settings.h"

//#include <initguid.h>
#include "Dbt.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

#define DEFAULT_FIRMWARE_BASE_ADDR		0x08000000

// **************
// NanoVNA-H

#define NANOVNA_H_SAVEAREA_MAX 5
#define NANOVNA_H_SAVE_CONFIG_SIZE        0x00000800
#define NANOVNA_H_SAVE_PROP_CONFIG_SIZE   0x00001800
#define NANOVNA_H_SAVE_CONFIG_ADDR        0x08018000
#define NANOVNA_H_SAVE_PROP_CONFIG_ADDR   (NANOVNA_H_SAVE_CONFIG_ADDR + NANOVNA_H_SAVE_CONFIG_SIZE)
#define NANOVNA_H_SAVE_FULL_AREA_SIZE     (NANOVNA_H_SAVE_CONFIG_SIZE + NANOVNA_H_SAVEAREA_MAX * NANOVNA_H_SAVE_PROP_CONFIG_SIZE)

// **************
// NanoVNA-H4

#define NANOVNA_H4_SAVEAREA_MAX 5
#define NANOVNA_H4_SAVE_CONFIG_SIZE        0x00000800
#define NANOVNA_H4_SAVE_PROP_CONFIG_SIZE   0x00004000
#define NANOVNA_H4_SAVE_CONFIG_ADDR        0x0802B800
#define NANOVNA_H4_SAVE_PROP_CONFIG_ADDR   (NANOVNA_H4_SAVE_CONFIG_ADDR + NANOVNA_H4_SAVE_CONFIG_SIZE)
#define NANOVNA_H4_SAVE_FULL_AREA_SIZE     (NANOVNA_H4_SAVE_CONFIG_SIZE + NANOVNA_H4_SAVEAREA_MAX * NANOVNA_H4_SAVE_PROP_CONFIG_SIZE)

// **************

const GUID GUID_DEVINTERFACE_COMPORT = {0x86e0d1e0, 0x8089, 0x11d0, {0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73}};

const GUID GUID_DEVINTERFACE_USB_DEVICE = {0xA5DCBF10, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};
//const GUID GUID_DFU              = {0x3FE809AB, 0xFB91, 0x4CB5, {0xA6, 0x43, 0x69, 0x67, 0x0D, 0x52, 0x36, 0x6E}};
//const GUID GUID_APP              = {0xcb979912, 0x5029, 0x420a, {0xae, 0xb1, 0x34, 0xfc, 0x0a, 0x7d, 0x57, 0x26}};

// VID/PID for STM32F bootloader
#define MD_VID					0x0483
#define MD_PID					0xDF11

// VID's/PID's for GD32 NanoVNA V2's
#define V2a_VID				0x0483	// this when no bootloader and no firmware present ?
#define V2a_PID				0x5740	//
#define V2b_VID				0x04b4	// this when a NanoVNA V2 is present - either DFU bootloader mode or normal mode
#define V2b_PID				0x0008	//   "     "

#define CONTROL_BLOCK			0
#define SPI_BLOCK					1
#define FLASH_BLOCK				2

#pragma pack(push, 1)

typedef struct
{
	char    sz_signature[5];		// "DfuSe"
	uint8_t b_version;				// 0x01
	uint32_t dfu_image_size;		//
	uint8_t b_targets;				//
} t_dfuse_prefix;

typedef struct
{
	char    sz_signature[6];		// "Target"
	uint8_t b_alternate_setting;	//
	uint32_t b_target_named;		//
	char sz_target_name[255];		//
	uint32_t target_size;			//
	uint32_t nb_elements;			//
} t_dfuse_target_prefix;

typedef struct
{
	uint32_t address;
	uint32_t size;
} t_dfuse_element;

typedef struct
{
	uint8_t bcd_device_lo;
	uint8_t bcd_device_hi;

	uint8_t id_product_lo;
	uint8_t id_product_hi;

	uint8_t id_vendor_lo;
	uint8_t id_vendor_hi;

	uint8_t bcd_dfu_lo;				// 0x1a
	uint8_t bcd_dfu_hi;				// 0x01

	uint8_t uc_dfu_signature[3];	// "UFD"

	uint8_t b_length;				// 16

	uint32_t dw_crc;				// zlib crc32 over entire file bar this crc
} t_dfuse_suffix;

#pragma pack(pop)

TUploadFirmwareForm *UploadFirmwareForm = NULL;

__fastcall TUploadFirmwareForm::TUploadFirmwareForm(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall TUploadFirmwareForm::FormCreate(TObject *Sender)
{
	AnsiString s;

	m_initialised = false;

	Caption = Application->Title + " " + this->Caption;

	// stop flicker
	Memo1->ControlStyle = Memo1->ControlStyle << csOpaque;

	Memo1->Clear();

	Memo1->Lines->Add("");
	Memo1->Lines->Add("You need to ensure your NanoVNA V1 is in DFU mode before uploading new firmware.");
	Memo1->Lines->Add("");
	Memo1->Lines->Add("NanoVNA-H ... go into the CONFIG/DFU menu and enable DFU mode.");
	Memo1->Lines->Add("NanoVNA-H4 .. turn VNA off, press & hold down the joy button whilst turning the unit back on.");
	Memo1->Lines->Add("");
	Memo1->Lines->Add("Then press the \"open folder\" button above this memo to find the correct firmware file.");
	Memo1->Lines->Add("Or select one of the built-in firmwares shown in the above row of buttons.");
	Memo1->Lines->Add("");
	Memo1->Lines->Add("If you uploaded the wrong firmware or the upload fails for whatever reason, don't worry, you");
	Memo1->Lines->Add("can easily recover the unit by shorting the 'BOOT0' and 'VDD' pin's/pads together on the VNA's");
	Memo1->Lines->Add("PCB whilst turning the unit back on if need be.");

	OpenDialog1->InitialDir = ExtractFilePath(Application->ExeName);
	SaveDialog1->InitialDir = ExtractFilePath(Application->ExeName);

	ProgressBar1->DoubleBuffered = true;
	ProgressBar1->Position = 0;

	m_flash_addr = DEFAULT_FIRMWARE_BASE_ADDR;
	m_flash_size = 0;

	m_firmware.reserve(1024 * 1024 * 4);	// 4MB

	memset(&m_DeviceDesc, 0, sizeof(USB_DEVICE_DESCRIPTOR));

	memset(&m_DfuDesc, 0, sizeof(DFU_FUNCTIONAL_DESCRIPTOR));
	m_DfuInterfaceIdx = 0;
	m_NbOfAlternates = 0;

	#if 0
		{	// USB serial comport
			DEV_BROADCAST_DEVICEINTERFACE dbch;
			memset(&dbch, 0, sizeof(dbch));
			dbch.dbcc_size       = sizeof(dbch);
			dbch.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
			dbch.dbcc_classguid  = GUID_DEVINTERFACE_COMPORT;
			HDEVNOTIFY handle    = ::RegisterDeviceNotification(this->Handle, &dbch, DEVICE_NOTIFY_WINDOW_HANDLE);
			if (handle != NULL)
				m_notification_handle.push_back(handle);
		}
	#endif

	#if 1
		{	// DFU device
			DEV_BROADCAST_DEVICEINTERFACE dbch;
			memset(&dbch, 0, sizeof(dbch));
			dbch.dbcc_size       = sizeof(dbch);
			dbch.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
			dbch.dbcc_classguid  = GUID_DEVINTERFACE_USB_DEVICE;
			HDEVNOTIFY handle    = ::RegisterDeviceNotification(this->Handle, &dbch, DEVICE_NOTIFY_WINDOW_HANDLE);
			if (handle != NULL)
				m_notification_handle.push_back(handle);
		}
	#endif

//	LeaveDFUButton->Enabled = false;

	// move to the saved position
	this->Top    = settings.firmwareWindowPos.top;
	this->Left   = settings.firmwareWindowPos.left;
	this->Width  = settings.firmwareWindowPos.width;
	this->Height = settings.firmwareWindowPos.height;
}

void __fastcall TUploadFirmwareForm::FormDestroy(TObject *Sender)
{
	for (int i = (int)m_notification_handle.size() - 1; i >= 0; i--)
	{
		if (m_notification_handle[i])
			::UnregisterDeviceNotification(m_notification_handle[i]);
		m_notification_handle[i] = NULL;
	}
	m_notification_handle.resize(0);

	closeDFUDevice();
}

void __fastcall TUploadFirmwareForm::FormClose(TObject *Sender, TCloseAction &Action)
{
//	rebootUnit();
}

void __fastcall TUploadFirmwareForm::WMWindowPosChanging(TWMWindowPosChanging &msg)
{
	const int thresh = WINDOW_SNAP;

	if (msg.WindowPos->flags & SWP_STATECHANGED)
	{
		if (msg.WindowPos->flags & SWP_FRAMECHANGED)
		{
			if (msg.WindowPos->x < 0 && msg.WindowPos->y < 0)
			{	// Window state is about to change to MAXIMIZED
				if ((msg.WindowPos->flags & (SWP_SHOWWINDOW | SWP_NOACTIVATE)) == (SWP_SHOWWINDOW | SWP_NOACTIVATE))
				{	// about to minimize
					return;
				}
				else
				{	// about to maximize
					return;
				}
			}
		}
	}

	if (msg.WindowPos->hwnd != this->Handle || Screen == NULL)
		return;

	const int dtLeft   = Screen->DesktopRect.left;
	//const int dtRight  = Screen->DesktopRect.right;
	const int dtTop    = Screen->DesktopRect.top;
	const int dtBottom = Screen->DesktopRect.bottom;
	const int dtWidth  = Screen->DesktopRect.Width();
	const int dtHeight = Screen->DesktopRect.Height();

	//const int waLeft   = Screen->WorkAreaRect.left;
	//const int waRight  = Screen->WorkAreaRect.right;
	//const int waTop    = Screen->WorkAreaRect.top;
	//const int waBottom = Screen->WorkAreaRect.bottom;
	//const int waWidth  = Screen->WorkAreaRect.Width();
	//const int waHeight = Screen->WorkAreaRect.Height();

	int x = msg.WindowPos->x;
	int y = msg.WindowPos->y;
	int w = msg.WindowPos->cx;
	int h = msg.WindowPos->cy;

	for (int i = 0; i < Screen->MonitorCount; i++)
	{	// sticky screen edges
		const int mLeft   = Screen->Monitors[i]->WorkareaRect.left;
		const int mRight  = Screen->Monitors[i]->WorkareaRect.right;
		const int mTop    = Screen->Monitors[i]->WorkareaRect.top;
		const int mBottom = Screen->Monitors[i]->WorkareaRect.bottom;
		const int mWidth  = Screen->Monitors[i]->WorkareaRect.Width();
		const int mHeight = Screen->Monitors[i]->WorkareaRect.Height();

		if (ABS(x - mLeft) < thresh)
				  x = mLeft;			// stick left to left side
		else
		if (ABS((x + w) - mRight) < thresh)
					x = mRight - w;	// stick right to right side

		if (ABS(y - mTop) < thresh)
				  y = mTop;				// stick top to top side
		else
		if (ABS((y + h) - mBottom) < thresh)
					y = mBottom - h;	// stick bottom to bottm side

		// stick the right side to the right side of the screen if the left side is stuck to the left side of the screen
		if (x == mLeft)
			if ((w >= (mWidth - thresh)) && (w <= (mWidth + thresh)))
				w = mWidth;

		// stick the bottom to the bottom of the screen if the top is stuck to the top of the screen
		if (y == mTop)
			if ((h >= (mHeight - thresh)) && (h <= (mHeight + thresh)))
				h = mHeight;
	}
/*
	{	// sticky screen edges
		if (ABS(x - waLeft) < thresh)
			x = waLeft;			// stick left to left side
		else
		if (ABS((x + w) - waRight) < thresh)
			x = waRight - w;	// stick right to right side

		if (ABS(y - waTop) < thresh)
			y = waTop;			// stick top to top side
		else
		if (ABS((y + h) - waBottom) < thresh)
			y = waBottom - h;	// stick bottom to bottm side

		// stick the right side to the right side of the screen if the left side is stuck to the left side of the screen
		if (x == waLeft)
			if ((w >= (waWidth - thresh)) && (w <= (waWidth + thresh)))
				w = waWidth;

		// stick the bottom to the bottom of the screen if the top is stuck to the top of the screen
		if (y == waTop)
			if ((h >= (waHeight - thresh)) && (h <= (waHeight + thresh)))
				h = waHeight;
	}
*/
	// limit minimum size
	if (w < Constraints->MinWidth)
		 w = Constraints->MinWidth;
	if (h < Constraints->MinHeight)
		 h = Constraints->MinHeight;

	// limit maximum size
	if (w > Constraints->MaxWidth && Constraints->MaxWidth > Constraints->MinWidth)
		 w = Constraints->MaxWidth;
	if (h > Constraints->MaxHeight && Constraints->MaxHeight > Constraints->MinHeight)
		 h = Constraints->MaxHeight;

	// limit maximum size
	if (w > dtWidth)
		 w = dtWidth;
	if (h > dtHeight)
		 h = dtHeight;
/*
	if (Application->MainForm && this != Application->MainForm)
	{	// stick to our main form sides
		const TRect rect = Application->MainForm->BoundsRect;

		if (ABS(x - rect.left) < thresh)
			x = rect.left;			// stick to left to left side
		else
		if (ABS((x + w) - rect.left) < thresh)
			x = rect.left - w;	// stick right to left side
		else
		if (ABS(x - rect.right) < thresh)
			x = rect.right;		// stick to left to right side
		else
		if (ABS((x + w) - rect.right) < thresh)
			x = rect.right - w;	// stick to right to right side

		if (ABS(y - rect.top) < thresh)
			y = rect.top;			// stick top to top side
		else
		if (ABS((y + h) - rect.top) < thresh)
			y = rect.top - h;		// stick bottom to top side
		else
		if (ABS(y - rect.bottom) < thresh)
			y = rect.bottom;		// stick top to bottom side
		else
		if (ABS((y + h) - rect.bottom) < thresh)
			y = rect.bottom - h;	// stick bottom to bottom side
	}
*/
	// stop it completely leaving the desktop area
	if (x < (dtLeft - Width + (dtWidth / 15)))
		 x =  dtLeft - Width + (dtWidth / 15);
	if (x > (dtWidth - (Screen->Width / 15)))
		 x =  dtWidth - (Screen->Width / 15);
	if (y < dtTop)
		 y = dtTop;
	if (y > (dtBottom - (dtHeight / 10)))
		 y =  dtBottom - (dtHeight / 10);

	msg.WindowPos->x  = x;
	msg.WindowPos->y  = y;
	msg.WindowPos->cx = w;
	msg.WindowPos->cy = h;
}

void __fastcall TUploadFirmwareForm::OnDeviceChange(TMessage &msg)
{
	const DWORD wparam = msg.WParam;
	const DWORD lparam = msg.LParam;

	const int event_type = wparam;

	String s;
	String Msg = "OnDeviceChange .. ";
	String name_str;

	Msg += " event type: ";
	switch (event_type)
	{
		case DBT_APPYBEGIN:
			Msg += "DBT_APPYBEGIN";
			break;
		case DBT_APPYEND:
			Msg += "DBT_CONFIGCHANGECANCELED";
			break;
		case DBT_MONITORCHANGE:
			Msg += "DBT_MONITORCHANGE";
			break;
		case DBT_SHELLLOGGEDON:
			Msg += "DBT_SHELLLOGGEDON";
			break;
		case DBT_CONFIGMGAPI32:
			Msg += "DBT_CONFIGMGAPI32";
			break;
		case DBT_VXDINITCOMPLETE:
			Msg += "DBT_VXDINITCOMPLETE";
			break;
		case DBT_VOLLOCKQUERYLOCK:
			Msg += "VOLLOCKQUERYLOCK";
			break;
		case DBT_VOLLOCKLOCKTAKEN:
			Msg += "DBT_VOLLOCKLOCKTAKEN";
			break;
		case DBT_VOLLOCKLOCKFAILED:
			Msg += "DBT_VOLLOCKLOCKFAILED";
			break;
		case DBT_VOLLOCKQUERYUNLOCK:
			Msg += "DBT_VOLLOCKQUERYUNLOCK";
			break;
		case DBT_VOLLOCKLOCKRELEASED:
			Msg += "DBT_VOLLOCKLOCKRELEASED";
			break;
		case DBT_VOLLOCKUNLOCKFAILED:
			Msg += "DBT_VOLLOCKUNLOCKFAILED";
			break;
		case DBT_CONFIGCHANGED:
			Msg += "DBT_CONFIGCHANGED";
			break;
		case DBT_CUSTOMEVENT:
			Msg += "DBT_CUSTOMEVENT";
			break;
		case DBT_DEVICEARRIVAL:
			Msg += "DBT_DEVICEARRIVAL";
			break;
		case DBT_DEVICEQUERYREMOVE:
			Msg += "DBT_DEVICEQUERYREMOVE";
			break;
		case DBT_DEVICEQUERYREMOVEFAILED:
			Msg += "DBT_DEVICEQUERYREMOVEFAILED";
			break;
		case DBT_DEVICEREMOVEPENDING:
			Msg += "DBT_DEVICEREMOVEPENDING";
			break;
		case DBT_DEVICEREMOVECOMPLETE:
			Msg += "DBT_DEVICEREMOVECOMPLETE";
			break;
		case DBT_DEVICETYPESPECIFIC:
			Msg += "DBT_DEVICETYPESPECIFIC";
			break;
		case DBT_QUERYCHANGECONFIG:
			Msg += "DBT_QUERYCHANGECONFIG";
			break;
		case DBT_DEVNODES_CHANGED:
			Msg += "DBT_DEVNODES_CHANGED";
			break;
		case DBT_USERDEFINED:
			Msg += "DBT_USERDEFINED";
			break;
		default:
			s.printf(L"unknown [%d]", wparam);
			Msg += s;
			break;
	}

	PDEV_BROADCAST_DEVICEINTERFACE pdbch = (PDEV_BROADCAST_DEVICEINTERFACE)lparam;
	if (pdbch == NULL)
	{
		Msg += ", device type: none";
//		Memo1->Lines->Add(Msg);
		return;
	}

	const DWORD device_type = pdbch->dbcc_devicetype;

	const GUID guid = pdbch->dbcc_classguid;
	s.printf(L", guid %08X %04X %04X %02X %02X %02X %02X %02X %02X %02X %02X",
				guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	Msg += s;

	if (pdbch->dbcc_classguid == GUID_DEVINTERFACE_USB_DEVICE)
	{
	}

	if (pdbch->dbcc_classguid == GUID_DEVINTERFACE_COMPORT)
	{
	}

	Msg += ", device type: ";

	switch (device_type)
	{
		case DBT_DEVTYP_OEM: 				// oem-defined device type
			Msg += "DBT_DEVTYP_OEM";
			break;

		case DBT_DEVTYP_DEVNODE:			// devnode number
			Msg += "DBT_DEVTYP_DEVNODE";
			break;

		case DBT_DEVTYP_VOLUME:				// logical volume
			Msg += "DBT_DEVTYP_VOLUME";
			break;

		case DBT_DEVTYP_PORT:				// serial, parallel
			Msg += "DBT_DEVTYP_PORT";

			if (event_type == DBT_DEVICEARRIVAL)
			{	// added
			}
			else
			if (event_type == DBT_DEVICEREMOVECOMPLETE)
			{	// removed
			}

			break;

		case DBT_DEVTYP_NET:					// network resource
			Msg += "DBT_DEVTYP_NET";
			break;

		case DBT_DEVTYP_DEVICEINTERFACE:	// device interface class
			Msg += "DBT_DEVTYP_DEVICEINTERFACE";

			#if defined(__BORLANDC__)
				name_str = String((const wchar_t *)pdbch->dbcc_name);
				if (!name_str.IsEmpty())
					Msg += ", name: " + name_str + " ";
			#endif

			// NanoVNA DFU
			// name: \\?\USB#VID_0483&PID_DF11#FFFFFFFEFFFF#{a5dcbf10-6530-11d2-901f-00c04fb951ed}
			if (name_str.UpperCase().Pos("USB#VID_0483&PID_DF11") > 0)
			{
				if (event_type == DBT_DEVICEARRIVAL)
				{	// added
					//m_dfu_name = name_str;
					Memo1->Lines->Add("");
					Memo1->Lines->Add(Msg);
					if (this->Visible)
						Beep(440, 80);
				}
				else
				if (event_type == DBT_DEVICEREMOVECOMPLETE)
				{	// removed
					if (m_dfu_name.UpperCase() == name_str.UpperCase())
					{
						m_dfu_name = "";
						Memo1->Lines->Add(Msg);
						if (this->Visible)
							Beep(349.23, 80);
					}
				}

				usbProcess();
			}

			break;

		case DBT_DEVTYP_HANDLE:				// file system handle
			Msg += "DBT_DEVTYP_HANDLE";
			break;

//		#if(WINVER >= _WIN32_WINNT_WIN7)
//			case DBT_DEVTYP_DEVINST:			// device instance
//				Msg += "DBT_DEVTYP_DEVINST";
//				break;
//		#endif

		default:
			s.printf(L"unknown device type [%u]", device_type);
			Msg += s;

			#if defined(__BORLANDC__)
				Msg += ", name: " + String((const wchar_t *)pdbch->dbcc_name) + " ";
			#endif

			break;
	}

//	Memo1->Lines->Add(Msg);
}

void __fastcall TUploadFirmwareForm::show()
{
	this->Show();
	this->BringToFront();
	this->Update();

	usbProcess();
}

void __fastcall TUploadFirmwareForm::saveData(std::vector <uint8_t> &data, String ID)
{
	if (data.empty())
		return;

	String filename;

	ID = ID.Trim();
	if (!ID.IsEmpty())
		filename += ID;

	if (!filename.IsEmpty())
		filename += "_";

	filename += FormatDateTime("yyyy-mm-dd_hh-nn-ss", Now());

	// remove/replace any invalid filename characters
	filename = common.cleanFilename(filename, false, true);

	SaveDialog1->Title    = "Save the flash image to ..";
	SaveDialog1->FileName = filename;

	Application->NormalizeTopMosts();
	const bool ok = SaveDialog1->Execute();
	Application->RestoreTopMosts();

	if (!ok)
		return;

	filename = SaveDialog1->FileName;

	AnsiString ext = ExtractFileExt(filename).LowerCase();

	if (ext == ".bin")
	{
		common.saveFile(filename, data);
		return;
	}

	if (ext == ".hex")
	{
		// https://en.wikipedia.org/wiki/Intel_HEX
		//
		// rec_type
		// 00 - data record
		// 01 - end-of-file record
		// 02 - extended segment address record
		// 03 - start segment address
		// 04 - extended linear address record
		// 05 - start linear address record (MDK-ARM only)
		//
		// :02 0000 04 0800 F2
		//
		// :10 0000 00 00020020010100086134000851D40008 FA
		// :10 0010 00 E1620008E1620008E1620008E1620008 B4
		//        ""                ""
		// :10 FFE0 00 083D0020000000000000000000000000 AC
		// :10 FFF0 00 70B50225054C0134290020006419FFF7 73
		//
		// :02 0000 04 0801 F1
		//
		// :10 0000 00 27FE2578002DF6D170BDC046A0630108 FB
		// :10 0010 00 084A0300D16910B50020994206D8581A 41
		//        ""                ""
		// :10 77B0 00 71727466000000003863ED3EDA0F493F D5
		// :10 77C0 00 5E987B3FDA0FC93F6937AC3168212233 BD
		//
		// :08 77D0 00 B40F14336821A23349
		// :08 77D8 00 488FFE7F0100000054
		//
		// :10 77E0 00 0096000000000800206B4D475450455A 99
		// :10 77F0 00 590000006D1D6E7066617A7900010000 0D
		//        ""                ""
		// :10 7940 00 00000000000000000000000000000000 37
		// :10 7950 00 00000000000000000000000000000000 27
		//
		// :04 0000 05 08000101 ED
		// :00 0000 01 FF

		std::vector <String> lines;

		int rec_type = 4;	// start with extended linear address record

		const uint32_t base_addr = 0x08000000;

		uint32_t addr = 0;

		while (true)
		{
			String s = ":";
			String s2;

			uint8_t chksum = 0;

			int size = 0;

			switch (rec_type)
			{
				case 0:	// data record
					size = 16;
					break;
				case 1:	// end-of-file record
					size = 0;
					break;
				case 2:	// extended segment address record
					size = 2;
					break;
				case 3:	// start segment address
					size = 4;
					break;
				case 4:	// extended linear address record
					size = 2;
					break;
				case 5:	// start linear address record (MDK-ARM only)
					size = 4;
					break;
			}

			// size of data to follow (8-bit)
			s2.printf(L"%02X", size);
			s += s2;
			chksum += (uint8_t)size;

			{	// address
				uint16_t adr = 0x0000;
				if (rec_type == 0)	// data record
					adr = (uint16_t)(addr & 0xffff);
/*				else
				if (rec_type == 1)	// end-of-file record
					adr = 0x0000;
				else
				if (rec_type == 2)	// extended segment address record
					adr = 0x0000;
				else
				if (rec_type == 3)	// start segment address
					adr = 0x0000;
				else
				if (rec_type == 4)	// extended linear address record
					adr = 0x0000;
				else
				if (rec_type == 5)	// start linear address record (MDK-ARM only)
					adr = 0x0000;
*/
				s2.printf(L"%04X", adr);
				s += s2;

				chksum += (uint8_t)(adr >> 8);
				chksum += (uint8_t)(adr >> 0);
			}

			// record type (8-bit)
			s2.printf(L"%02X", rec_type);
			s += s2;
			chksum += rec_type;

			// data
			if (rec_type == 0)
			{	// data record
				for (int i = 0; i < size; i++)
				{
					const uint8_t b = data[addr++];
					s2.printf(L"%02X", b);
					s += s2;
					chksum += b;
				}
			}
			else
			if (rec_type == 1)
			{	// end-of-file record
			}
			else
			if (rec_type == 2)
			{	// extended segment address record
			}
			else
			if (rec_type == 3)
			{	// start segment address
			}
			else
			if (rec_type == 4)
			{	// extended linear address record
				const uint16_t value = (uint16_t)((base_addr + addr) >> 16);

				s2.printf(L"%04X", value);
				s += s2;

				chksum += (uint8_t)(value >> 8);
				chksum += (uint8_t)(value >> 0);
			}
			else
			if (rec_type == 5)
			{	// start linear address record (MDK-ARM only)
				const uint32_t value = base_addr + addr;

				s2.printf(L"%08X", value);
				s += s2;

				chksum += (uint8_t)(value >> 24);
				chksum += (uint8_t)(value >> 16);
				chksum += (uint8_t)(value >>  8);
				chksum += (uint8_t)(value >>  0);
			}

			// checksum (8-bit)
			s2.printf(L"%02X", (uint8_t)(0 - (int)chksum));
			s += s2;

			// line is complete
			lines.push_back(s);

			// decide on the next stage
			if (rec_type == 0)	// data record
			{
				if (addr >= data.size())
					rec_type = 5;
				else
				if ((addr & 0xffff) == 0)
					rec_type = 4;
			}
			else
			if (rec_type == 1)	// end-of-file record
			{
				break;
			}
			else
			if (rec_type == 2)	// extended segment address record
			{
			}
			else
			if (rec_type == 3)	// start segment address
			{
			}
			else
			if (rec_type == 4)	// extended linear address record
			{
				rec_type = 0;
			}
			else
			if (rec_type == 5)	// start linear address record (MDK-ARM only)
			{
				rec_type = 1;
			}
			else
			{	// error
				break;
			}
		}

		common.saveFile(filename, lines);

		return;
	}

	Application->NormalizeTopMosts();
	Application->MessageBox(L"No saved - unknown file extension", L"Error", MB_ICONERROR | MB_OK);
	Application->RestoreTopMosts();
}

void __fastcall TUploadFirmwareForm::ClearMemoButtonClick(TObject *Sender)
{
	Memo1->Clear();
}

bool __fastcall TUploadFirmwareForm::openDFUDevice(String device_path)
{
	AnsiString s;
	DFUSTATUS status;

	if (m_stdfu.isOpen())
		return false;

	m_flash_addr = 0;
	m_flash_size = 0;

	m_desc_string.resize(0);

	if (device_path.IsEmpty())
		return false;
/*
	int i = USBDeviceComboBox->ItemIndex;
	if (i >= 0)
		i = (int)USBDeviceComboBox->Items->Objects[i];
	if (i < 0)
		return false;
	if (i < 0 || i >= (int)m_usb_devices.size())
		return false;
	device_path = m_usb_devices[i].path;
*/
	Memo1->Lines->Add("");
	Memo1->Lines->Add("Opening " + device_path + " ");

	if (m_stdfu.open(AnsiString(device_path).c_str()) != STDFU_NOERROR)
	{
		Memo1->Lines->Add(" open failed ");
		return false;
	}

	// **********************************

	memset(&m_DfuDesc, 0, sizeof(DFU_FUNCTIONAL_DESCRIPTOR));
	m_DfuInterfaceIdx = 0;
	m_NbOfAlternates = 0;

	if (m_stdfu.getDFUDescriptor(&m_DfuInterfaceIdx, &m_NbOfAlternates, &m_DfuDesc) != STDFU_NOERROR)
	{
		Memo1->Lines->Add(" get dfu descriptor failed ");
		return false;
	}

	if (m_DfuDesc.wTransfertSize <= 0)
	{
		Memo1->Lines->Add(" invalid block size ");
		m_DfuDesc.wTransfertSize = 1024;
//		return false;
	}

	m_block_buf.resize(m_DfuDesc.wTransfertSize);

	Memo1->Lines->Add("");
	s.printf("block size: %d ", m_DfuDesc.wTransfertSize);
	Memo1->Lines->Add(s);

	// **********************************

	memset(&m_DeviceDesc, 0, sizeof(USB_DEVICE_DESCRIPTOR));

	if (m_stdfu.getDeviceDescriptor(&m_DeviceDesc) == STDFU_NOERROR)
	{
		AnsiString s;

		Memo1->Lines->Add("");

		if ((m_DfuDesc.bcdDFUVersion < 0x011A) || (m_DfuDesc.bcdDFUVersion >= 0x0120))
		{
			if (m_DfuDesc.bcdDFUVersion != 0)
				Memo1->Lines->Add("Bad DFU protocol version. Should be 1.1A ");
		}
		else
		{
			s.printf("vid: 0x%04X ", m_DeviceDesc.idVendor);
			Memo1->Lines->Add(s);

			s.printf("pid: 0x%04X ", m_DeviceDesc.idProduct);
			Memo1->Lines->Add(s);

			s.printf("ver: %d.%02X ", m_DeviceDesc.bcdDevice >> 8, m_DeviceDesc.bcdDevice & 0xff);
			Memo1->Lines->Add(s);
		}
	}

	// **********************************


	// STM32F072
	//
	// vid: 0x0483
	// pid: 0xDF11
	// ver: 34.00
	//
	// desc 1:STMicroelectronics
	// desc 2:STM32  BOOTLOADER
	// desc 3:FFFFFFFEFFFF
	// desc 4:@Internal Flash  /0x08000000/064*0002Kg
	// desc 5:@Option Bytes  /0x1FFFF800/01*016 e
	//
	// Flash addr: 0x08000000
	// Flash size: 131072


	// STM32F303
	//
	// vid: 0x0483
	// pid: 0xDF11
	// ver: 34.00
	//
	// desc 1:STMicroelectronics
	// desc 2:STM32  BOOTLOADER
	// desc 3:205C366B2039
	// desc 4:@Internal Flash  /0x08000000/128*0002Kg
	// desc 5:@Option Bytes  /0x1FFFF800/01*016 e
	//
	// Flash addr: 0x08000000
	// Flash size: 262144


	if (!selectCurrentConfig(0, 0, 0))
	{
		Memo1->Lines->Add(" error: select current config 1");
		return false;
	}

	Memo1->Lines->Add("");
	for (int i = 1; i < 8; i++)
	{
		AnsiString s1;
		AnsiString s3;
		char str[256];

		s1.printf("desc %d:", i);

		if (!getStringDiscrip(i, str, sizeof(str)))
		{
			Memo1->Lines->Add(" error: get string discriptor");
			return false;
		}

		AnsiString s2(str);

		// replace tabs with spaces
		for (int k = 1; k <= s2.Length(); k++)
			if (s2[k] == '\t')
				s2[k] = ' ';

		s1 += s2;
		Memo1->Lines->Add(s1);

		int pos;
		s2 = s2.Trim();

		// @Internal Flash  /0x08000000/064*0002Kg

		s3 = "@internal flash";
		pos = s2.LowerCase().Pos(s3);
		if (pos <= 0)
		{
			if (s2.LowerCase().Pos("@option bytes") > 0 && m_flash_addr > 0 && m_flash_size > 0)
				break;	// we now have what we need
			continue;
		}
		s2 = s2.SubString(pos + s3.Length(), s2.Length()).Trim();

		int scanned = 0;
		unsigned int address = 0;
		int sectors = 0;
		int size = 0;
		char multiplier = 'B';
		char memtype = '\0';
		int res;

		res = sscanf(s2.c_str(), "/0x%x/%n", &address, &scanned);
		if (res <= 0)
			continue;

		if (scanned > 0)
			s2 = s2.SubString(1 + scanned, s2.Length()).Trim();

		res = sscanf(s2.c_str(), "%d*%d%c%c%n", &sectors, &size, &multiplier, &memtype, &scanned);
		if (res < 4)
			continue;

		if (scanned > 0)
			s2 = s2.SubString(1 + scanned, s2.Length()).Trim();

		switch (multiplier)
		{
			case 'B':
				break;
			case 'K':
				size *= 1024;
				break;
			case 'M':
				size *= 1024 * 1024;
				break;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
				break;
			default:
				continue;
		}

		if (!memtype)
			continue;

		m_flash_addr = address;
		m_flash_size = sectors * size;

		m_desc_string.push_back(s);

//		if (m_flash_addr > 0 && m_flash_size > 0)
//			break;	// we now have what we need
	}

	Memo1->Lines->Add("");

	s.printf("Flash addr: 0x%08x", m_flash_addr);
	Memo1->Lines->Add(s);
	s.printf("Flash size: %u", m_flash_size);
	Memo1->Lines->Add(s);

	// **********************************

	if (m_flash_addr == 0 || m_flash_size == 0)
		return false;

	Memo1->Lines->Add("");
	Memo1->Lines->Add("DFU device opened.");

	return true;
}

void __fastcall TUploadFirmwareForm::closeDFUDevice(bool lock_critical)
{
	CCriticalSection cs(readWrite_cs, lock_critical);

	m_stdfu.abort();
	m_stdfu.close();

	m_block_buf.resize(0);

	m_desc_string.resize(0);

	Memo1->Lines->Add("");
	Memo1->Lines->Add("DFU device closed.");
}

bool __fastcall TUploadFirmwareForm::waitUntilState(int state, float timeout_secs)
{
	DFUSTATUS status;

	// reset timer
	m_tick.mark();

	if (!m_stdfu.isOpen()) return false;
	if (m_stdfu.getStatus(&status) != STDFU_NOERROR) return false;
	while (true)
	{
		if (!m_stdfu.isOpen()) return false;
		if (m_stdfu.clrStatus() != STDFU_NOERROR) return false;
		if (m_stdfu.getStatus(&status) != STDFU_NOERROR)
			if (m_stdfu.abort() != STDFU_NOERROR)	// Reset State machine
				return false;
		if (status.bState == state) break;
		if (m_tick.secs(false) >= timeout_secs) return false;
	}

	return true;
}

bool __fastcall TUploadFirmwareForm::waitUntilIdle(float timeout_secs)
{
	DFUSTATUS status;

	// reset timer
	m_tick.mark();

	if (!m_stdfu.isOpen()) return false;
	if (m_stdfu.getStatus(&status) != STDFU_NOERROR) return false;	// cathy
	while (true)
	{
		if (!m_stdfu.isOpen()) return false;
		if (m_stdfu.clrStatus() != STDFU_NOERROR) return false;	// cathy

		m_stdfu.getStatus(&status);

//		if (m_stdfu.getStatus(&status) != STDFU_NOERROR)
//			if (m_stdfu.abort() != STDFU_NOERROR)	// Reset State machine
//				return false;

		if (status.bState == STATE_DFU_IDLE) break;
		if (status.bState == STATE_DFU_DOWNLOAD_IDLE) break;
		if (status.bState == STATE_DFU_UPLOAD_IDLE) break;
		if (m_tick.secs(false) >= timeout_secs) return false;
	}

	return true;
}

bool __fastcall TUploadFirmwareForm::download(void *buffer, int size, int block)
{
	if (!m_stdfu.isOpen())
		return false;
	if (m_stdfu.download(buffer, size, block) != STDFU_NOERROR)
		return false;
	if (!waitUntilIdle(500))
		return false;

	return true;
}

bool __fastcall TUploadFirmwareForm::upload(void *buffer, int size, int block)
{
	if (!m_stdfu.isOpen())
		return false;
	if (m_stdfu.upload(buffer, size, block) != STDFU_NOERROR)
		return false;

	if (!waitUntilIdle(500))
		return false;

	return true;
}

bool __fastcall TUploadFirmwareForm::getStringDiscrip(int index, void *buffer, int size)
{
	if (!m_stdfu.isOpen())
		return false;
	if (m_stdfu.getStringDescriptor(index, (LPSTR)buffer, size) != STDFU_NOERROR)
		return false;

	if (!waitUntilIdle(100))
		return false;

	return true;
}

bool __fastcall TUploadFirmwareForm::selectCurrentConfig(int config_index, int interface_index, int alt_set_index)
{
	if (!m_stdfu.isOpen())
		return false;
	if (!m_stdfu.selectCurrentConfiguration(config_index, interface_index, alt_set_index) == STDFU_NOERROR)
		return false;

	if (!waitUntilIdle(100))
		return false;

	return true;
}

bool __fastcall TUploadFirmwareForm::sendCommand(uint8_t a, uint8_t b)
{
	uint8_t cmd[2] = {a, b};

	if (!download(cmd, sizeof(cmd), CONTROL_BLOCK))
		return false;

	return true;
}

bool __fastcall TUploadFirmwareForm::eraseBlock(uint32_t address)
{
	uint8_t cmd[5] = {0x41, (uint8_t)(address >> 0), (uint8_t)(address >> 8), (uint8_t)(address >> 16), (uint8_t)(address >> 24)};

	if (!download(cmd, sizeof(cmd), CONTROL_BLOCK))
		return false;

	return true;
}

bool __fastcall TUploadFirmwareForm::setAddressPointer(uint32_t address)
{
	uint8_t cmd[5] = {0x21, (uint8_t)(address >> 0), (uint8_t)(address >> 8), (uint8_t)(address >> 16), (uint8_t)(address >> 24)};

	if (!download(cmd, sizeof(cmd), CONTROL_BLOCK))
		return false;
/*
	Sleep(100);

	if (!waitUntilIdle(100))
		return false;
*/

	return true;
}

bool __fastcall TUploadFirmwareForm::getVidPid(AnsiString s, int &vid, int &pid)
{
	vid = -1;
	pid = -1;

	if (s.IsEmpty())
		return false;

	s = s.LowerCase();

	// *****************
	// VID

	int i = s.Pos("vid_");
	if (i > 0)
	{
		AnsiString str = s.SubString(i + 4, 5);
		i = str.Pos('&');
		if (i > 0) str = str.SubString(1, i - 1);
		if (!TryStrToInt("0x" + str, vid))
			vid = -1;
	}

	// *****************
	// PID

	i = s.Pos("pid_");
	if (i > 0)
	{
		AnsiString str = s.SubString(i + 4, 5);
		i = str.Pos('#');
		if (i > 0) str = str.SubString(1, i - 1);
		if (!TryStrToInt("0x" + str, pid))
			pid = -1;
	}

	// *****************

	return true;
}

void __fastcall TUploadFirmwareForm::usbProcess()
{
	DWORD last_error;
	char last_error_str[512];

//	CCriticalSection cs(&readWriteCriticalSection, false);
	CCriticalSection cs(readWrite_cs, false);
	if (!cs.tryEnter())
		return;

	if (m_stdfu.isOpen())
		return;

	// **************************************************

	int devIndex = -1;

	m_usb_devices.resize(0);

//	const GUID guid = GUID_DFU;
	const GUID guid = GUID_DEVINTERFACE_USB_DEVICE;

//	HDEVINFO dev_info = ::SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
	HDEVINFO dev_info = ::SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (dev_info == INVALID_HANDLE_VALUE)
	{
		//last_error = ::GetLastError();
		//common.getLastErrorStr(last_error, last_error_str, sizeof(last_error_str));
		return;
	}

	std::vector <uint8_t> buffer;

	while (true)
	{
		BOOL res;
		char str[512];

		t_usb_device usb_device;
		usb_device.index = ++devIndex;
		usb_device.vid   = -1;
		usb_device.pid   = -1;

		SP_INTERFACE_DEVICE_DATA ifData;
		memset(&ifData, 0, sizeof(SP_INTERFACE_DEVICE_DATA));
		ifData.cbSize = sizeof(ifData);

		res = ::SetupDiEnumDeviceInterfaces(dev_info, NULL, &guid, devIndex, &ifData);
		if (!res)
		{	// not found
			//last_error = ::GetLastError();
			//common.getLastErrorStr(last_error, last_error_str, sizeof(last_error_str));
			::SetupDiDestroyDeviceInfoList(dev_info);
//			dev_info = NULL;
			break;
		}

		SP_DEVINFO_DATA did;
		memset(&did, 0, sizeof(SP_DEVINFO_DATA));
		did.cbSize = sizeof(SP_DEVINFO_DATA);

		DWORD needed = 0;
		::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, NULL, 0, &needed, &did);
//		res = ::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, NULL, 0, &needed, NULL);
		if (needed <= 0)
			continue;

		buffer.resize(needed);
		memset(&buffer[0], 0, needed);

		SP_INTERFACE_DEVICE_DETAIL_DATA *detail = (SP_INTERFACE_DEVICE_DETAIL_DATA *)&buffer[0];
		detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

//		SP_DEVINFO_DATA did;
//		memset(&did, 0, sizeof(SP_DEVINFO_DATA));
//		did.cbSize = sizeof(SP_DEVINFO_DATA);

//		res = ::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, detail, needed, NULL, &did);
		res = ::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, detail, needed, NULL, NULL);
		if (!res)
			continue;

		usb_device.path = AnsiString(detail->DevicePath);

		memset(str, 0, sizeof(str));
		res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_DEVICEDESC, NULL, (PBYTE)str, sizeof(str), NULL);
		if (res)
			usb_device.description = AnsiString(str);

		memset(str, 0, sizeof(str));
		res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_ENUMERATOR_NAME, NULL, (PBYTE)str, sizeof(str), NULL);
		if (res)
			usb_device.enumerator_name = AnsiString(str);

		memset(str, 0, sizeof(str));
		res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_FRIENDLYNAME, NULL, (PBYTE)str, sizeof(str), NULL);
		if (res)
			usb_device.friendly_name = AnsiString(str);

		memset(str, 0, sizeof(str));
		res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_LOCATION_INFORMATION, NULL, (PBYTE)str, sizeof(str), NULL);
		if (res)
			usb_device.location_information = AnsiString(str);

		memset(str, 0, sizeof(str));
		res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_MFG, NULL, (PBYTE)str, sizeof(str), NULL);
		if (res)
			usb_device.mfg = AnsiString(str);

		memset(str, 0, sizeof(str));
		res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_SERVICE, NULL, (PBYTE)str, sizeof(str), NULL);
		if (res)
			usb_device.service = AnsiString(str);

		if (!getVidPid(usb_device.path, usb_device.vid, usb_device.pid))
			continue;
		if (usb_device.vid != MD_VID || usb_device.pid != MD_PID)
			continue;

		m_usb_devices.push_back(usb_device);

		// fetch some details about the CPU
		if (openDFUDevice(usb_device.path))
			closeDFUDevice();
	}

	// **************************************************

//	if (m_usb_devices.size() <= 0 && USBDeviceComboBox->Items->Count <= 0)
//		return;	// no change

	if (!updateUSBDeviceComboBox())
		return;	// error

	//setControlState();

	// **************************************************
}

bool __fastcall TUploadFirmwareForm::updateUSBDeviceComboBox()
{
	TNotifyEvent ne = USBDeviceComboBox->OnChange;
	USBDeviceComboBox->OnChange = NULL;

	USBDeviceComboBox->Items->BeginUpdate();

	if (m_stdfu.isOpen())
	{
		USBDeviceComboBox->Enabled = false;
		USBDeviceComboBox->Items->EndUpdate();
		USBDeviceComboBox->OnChange = ne;
		return false;
	}

	if (USBDeviceComboBox->DroppedDown)
	{
		USBDeviceComboBox->Items->EndUpdate();
		USBDeviceComboBox->OnChange = ne;
		return false;
	}

	AnsiString prev_selection = USBDeviceComboBox->Text.Trim().UpperCase();
	int new_itemIndex = 0;

	USBDeviceComboBox->Clear();
	for (int i = 0; i < (int)m_usb_devices.size(); i++)
	{
		AnsiString s = m_usb_devices[i].location_information.Trim() + " " + m_usb_devices[i].path.Trim();

		if (s.UpperCase() == prev_selection)
			new_itemIndex = i;

		USBDeviceComboBox->AddItem(s + " ", (TObject *)i);
	}

	// if only one device, select it
	if (USBDeviceComboBox->Items->Count == 1 && new_itemIndex == 0)
		new_itemIndex = 0;

	if (USBDeviceComboBox->Items->Count > 0 && new_itemIndex >= 0)
		USBDeviceComboBox->ItemIndex = new_itemIndex;

//	USBDeviceComboBox->Enabled = (USBDeviceComboBox->Items->Count > 1) ? true : false;

	common.comboBoxAutoWidth(USBDeviceComboBox);

	USBDeviceComboBox->Update();

	USBDeviceComboBox->Items->EndUpdate();

	int i = USBDeviceComboBox->ItemIndex;
	if (i >= 0)
		i = (int)USBDeviceComboBox->Items->Objects[i];
	if (i >= 0)
		m_dfu_name = m_usb_devices[USBDeviceComboBox->ItemIndex].path;
	else
		m_dfu_name = "";

	USBDeviceComboBox->OnChange = ne;

	return !m_dfu_name.IsEmpty();
}

void __fastcall TUploadFirmwareForm::USBDeviceComboBoxDropDown(TObject *Sender)
{
	usbProcess();
}

void __fastcall TUploadFirmwareForm::USBDeviceComboBoxSelect(TObject *Sender)
{
	int i = USBDeviceComboBox->ItemIndex;
	if (i >= 0)
		i = (int)USBDeviceComboBox->Items->Objects[i];
	if (i < 0)
	{
		m_dfu_name = "";
		return;
	}
	m_dfu_name = m_usb_devices[USBDeviceComboBox->ItemIndex].path;

	if (openDFUDevice(m_dfu_name))
		closeDFUDevice();
}

void __fastcall TUploadFirmwareForm::USBDeviceComboBoxChange(TObject *Sender)
{
//	USBDeviceComboBoxSelect(USBDeviceComboBox);
}

void __fastcall TUploadFirmwareForm::FormKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	switch (Key)
	{
		case VK_ESCAPE:
			Key = 0;
//			rebootUnit();
			this->Hide();
			break;
	}
}

void __fastcall TUploadFirmwareForm::uploadFirmwareFile(std::vector <uint8_t> &buffer, AnsiString file_ext, int unit_type)
{
	AnsiString s;
	DFUSTATUS status;

	Memo1->Lines->Add("");

	if (unit_type == UNIT_TYPE_NONE)
	{
		String s = L"Invalid unit type";
		Memo1->Lines->Add(s);

		Application->NormalizeTopMosts();
		Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	if (buffer.size() <= 128)
	{
		String s = L"File is too small to be valid";
		Memo1->Lines->Add(s);

		Application->NormalizeTopMosts();
		Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	ProgressBar1->Position = 0;
	ProgressBar1->Update();

	// ******************************

	m_firmware.resize(0);

	if (file_ext != ".hex" && file_ext != ".dfu" && file_ext != ".bin")
	{
		String s = "Unknown file type, must be .bin, .dfu or .hex";
		Memo1->Lines->Add(s);

		Application->NormalizeTopMosts();
		Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	if (file_ext == ".dfu")
	{
		// **********

		const t_dfuse_prefix *prefix = (t_dfuse_prefix *)&buffer[0];

		AnsiString prefix_sz_signature;
		for (unsigned int i = 0; i < sizeof(prefix->sz_signature); i++)
			prefix_sz_signature += prefix->sz_signature[i];

		if (prefix_sz_signature != "DfuSe")
		{
			String s;
			s.printf(L"Invalid DFU prefix signature");
			Memo1->Lines->Add(s);

			Application->NormalizeTopMosts();
			Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		if (prefix->b_version != 1)
		{
			String s;
			s.printf(L"Only DFU file format v1 currently supported [%u]", prefix->b_version);
			Memo1->Lines->Add(s);

			Application->NormalizeTopMosts();
			Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		if ((prefix->dfu_image_size + sizeof(t_dfuse_suffix)) > buffer.size())
		{
			String s;
			s.printf(L"Invalid DFU image size [dfu_image_size: %u   actual dfu_image_size: %u]",
				prefix->dfu_image_size,
				buffer.size() - sizeof(t_dfuse_suffix));
			Memo1->Lines->Add(s);

			Application->NormalizeTopMosts();
			Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		if (prefix->b_targets != 1)
		{
			String s;
			s.printf(L"Invalid DFU number of targets [b_targets: %u]", prefix->b_targets);
			Memo1->Lines->Add(s);

			Application->NormalizeTopMosts();
			Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		// **********

		const t_dfuse_target_prefix *target_prefix = (t_dfuse_target_prefix *)&buffer[sizeof(t_dfuse_prefix)];

		AnsiString target_sz_signature;
		for (unsigned int i = 0; i < sizeof(target_prefix->sz_signature); i++)
			target_sz_signature += target_prefix->sz_signature[i];

		if (target_sz_signature != "Target")
		{
			String s;
			s.printf(L"Invalid DFU target prefix [target sz_signature: %s]", target_sz_signature.c_str());
			Memo1->Lines->Add(s);

			Application->NormalizeTopMosts();
			Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		// **********

		const t_dfuse_suffix *suffix = (t_dfuse_suffix *)&buffer[prefix->dfu_image_size];

		const uint32_t crc = common.updateCRC32(0xffffffff, &buffer[0], prefix->dfu_image_size + sizeof(t_dfuse_suffix) - sizeof(suffix->dw_crc));
		if (crc != suffix->dw_crc)
		{
			String s;
			s.printf(L"Invalid DFU crc [%08x  actual: %08x]", suffix->dw_crc, crc);
			Memo1->Lines->Add(s);

			Application->NormalizeTopMosts();
			Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		if (suffix->b_length != sizeof(t_dfuse_suffix))
		{
			String s;
			s.printf(L"Invalid DFU suffix length [%u  %u]", suffix->b_length, sizeof(t_dfuse_suffix));
			Memo1->Lines->Add(s);

			Application->NormalizeTopMosts();
			Application->MessageBox(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		// **********

		t_dfuse_element *element = (t_dfuse_element *)&buffer[sizeof(t_dfuse_prefix) + sizeof(t_dfuse_target_prefix)];
		uint32_t element_data    = sizeof(t_dfuse_prefix) + sizeof(t_dfuse_target_prefix) + sizeof(t_dfuse_element);

		//m_flash_addr = element->address;

//		Memo1->Lines->Add("");

		for (unsigned int i = 0; i < target_prefix->nb_elements; i++)
		{
			{
				String s;
				s.printf(L"DFU element address [%u]: 0x%08x", i, element->address);
				Memo1->Lines->Add(s);
				s.printf(L"DFU element size [%u]:    %u", i, element->size);
				Memo1->Lines->Add(s);
			}

			if (element->size <= 128 || element->size >= (1024 * 1024 * 4))
			{
				String s;
				s.printf(L"DFU firmware size too small");
				Memo1->Lines->Add(s);

				Application->NormalizeTopMosts();
				Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
				return;
			}

			if (element->address < DEFAULT_FIRMWARE_BASE_ADDR || element->address >= (DEFAULT_FIRMWARE_BASE_ADDR + (1024 * 1024)))
			{
				String s;
				s.printf(L"DFU firmware address out of range");
				Memo1->Lines->Add(s);

				Application->NormalizeTopMosts();
				Application->MessageBox(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
				return;
			}

			const uint32_t addr = element->address - DEFAULT_FIRMWARE_BASE_ADDR;

			const uint32_t old_size = m_firmware.size();
			const uint32_t new_size = addr + element->size;
			if (old_size < new_size)
			{
				m_firmware.resize(new_size);
				memset(&m_firmware[old_size], 0xff, new_size - old_size);
			}

			memcpy(&m_firmware[addr], &buffer[element_data], element->size);

			element_data += element->size + sizeof(t_dfuse_element);
			element       = (t_dfuse_element *)((uint8_t *)element + sizeof(t_dfuse_element) + element->size);
		}
	}






/*

	// TEST ONLY
	{	// save the firmware
		const int file_handle = FileCreate("saved.bin");
		if (file_handle > 0)
		{
			FileWrite(file_handle, &m_firmware[0], m_firmware.size());
			FileClose(file_handle);
		}
		return;
	}

*/



	if (file_ext == ".hex")
	{
		char tmp[5];
		uint8_t  chksum;
		uint8_t  rec_length;
		uint8_t  rec_type;
		uint8_t  rec_chksum;
		uint16_t rec_addr;
		uint32_t size = 0;
		uint32_t flash_addr = 0;
		bool end_of_file_record = false;
		std::vector <uint8_t> data(64);

		m_firmware.resize(1024 * 1024 * 4);	// allow for up to 4MB
		memset(&m_firmware[0], 0xff, m_firmware.size());

		Memo1->Lines->BeginUpdate();

		//Memo1->Lines->Add("");

		int line_num = 0;

		unsigned int in_file_pos = 0;

		while (in_file_pos < buffer.size())
		{
			AnsiString line;

			line_num++;

			data.resize(0);

			// remove any invalid characters
			unsigned int i = 0;
			while (in_file_pos < buffer.size())
			{
				const char c = (char)buffer[in_file_pos];
				if (i == 0)
				{
					if (c == ':')
						i = 1;	// found start of a line
				}
				else
				{
					if (c == ':')
						break;	// start of next line
					if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
						line += c;
				}
				in_file_pos++;
			}

			if (line.IsEmpty())
				continue;

			//Memo1->Lines->Add(line);

			if (line.Length() < (2 + 4 + 2))
			{
				Memo1->Lines->EndUpdate();

				m_firmware.resize(0);

				String s;
				s.printf(L"Invalid HEX file on line %d", line_num);
				Memo1->Lines->Add(s);

				Application->NormalizeTopMosts();
				Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
				return;
			}

			int j = 1;
			int k;

			chksum = 0;

			k = 0;
			while (k < 2)
				tmp[k++] = line[j++];
			tmp[k] = '\0';
			rec_length = (uint8_t)strtol(tmp, NULL, 16);
			chksum += rec_length;

			k = 0;
			while (k < 4)
				tmp[k++] = line[j++];
			tmp[k] = '\0';
			rec_addr = (uint16_t)strtol(tmp, NULL, 16);
			chksum += (rec_addr >> 8) & 0xff;
			chksum += (rec_addr >> 0) & 0xff;

			k = 0;
			while (k < 2)
				tmp[k++] = line[j++];
			tmp[k] = '\0';
			rec_type = (uint8_t)strtol(tmp, NULL, 16);
			chksum += rec_type;

			if (rec_length != ((line.Length() - j - 1) / 2))
			{
				Memo1->Lines->EndUpdate();

				m_firmware.resize(0);

				String s;
				s.printf(L"Invalid HEX file on line %d", line_num);
				Memo1->Lines->Add(s);

				Application->NormalizeTopMosts();
				Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
				return;
			}

			for (i = 0; i < (unsigned int)rec_length; i++)
			{
				int k = 0;
				while (k < 2)
					tmp[k++] = line[j++];
				tmp[k] = '\0';

				const uint8_t b = (uint8_t)strtol(tmp, NULL, 16);
				chksum += b;

				data.push_back(b);
			}

			k = 0;
			while (k < 2)
				tmp[k++] = line[j++];
			tmp[k] = '\0';
			chksum += (uint8_t)strtol(tmp, NULL, 16);

			if (chksum != 0)
			{
				Memo1->Lines->EndUpdate();

				m_firmware.resize(0);

				String s;
				s.printf(L"Bad checksum in HEX file on line %d", line_num);
				Memo1->Lines->Add(s);

				Application->NormalizeTopMosts();
				Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
				return;
			}

			// rec_type
			// 00 - data record
			// 01 - end-of-file record
			// 02 - extended segment address record
			// 04 - extended linear address record
			// 05 - start linear address record (MDK-ARM only)

			if (rec_type == 0)
			{	// data record
				uint32_t addr = flash_addr + rec_addr;

				if (addr < DEFAULT_FIRMWARE_BASE_ADDR)
				{
					String s;
					s.printf(L"Address out of range in HEX file on line %d", line_num);
					Memo1->Lines->Add(s);

					Application->NormalizeTopMosts();
					Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
					return;
				}

				addr -= DEFAULT_FIRMWARE_BASE_ADDR;

				if (size < (addr + data.size()))
					 size =  addr + data.size();

				if (size > m_firmware.capacity())
				{
					String s;
					s.printf(L"Address out of range in HEX file on line %d", line_num);
					Memo1->Lines->Add(s);

					Application->NormalizeTopMosts();
					Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
					return;
				}

				memcpy(&m_firmware[addr], &data[0], data.size());
			}

			if (rec_type == 1)
			{	// end of file record
				end_of_file_record = true;
				break;
			}

			if (rec_type == 2)
			{	// extended segment address record
				uint32_t addr = 0;
				for (unsigned int k = 0; k < data.size(); k++)
					addr = (addr << 8) | data[k];

				flash_addr = addr << 4;
			}

			if (rec_type == 3)
			{	// unknown
			}

			if (rec_type == 4)
			{	// 04 - extended linear address record
				uint32_t addr = 0;
				for (unsigned int k = 0; k < data.size(); k++)
					addr = (addr << 8) | data[k];

				flash_addr = addr << 16;
			}

			if (rec_type == 5)
			{	// 05 - start linear address record (MDK-ARM only)
				uint32_t addr = 0;
				for (unsigned int k = 0; k < data.size(); k++)
					addr = (addr << 8) | data[k];

				flash_addr = addr;
			}
		}

		Memo1->Lines->EndUpdate();

		if (!end_of_file_record)
		{
/*			String s;
			s.printf(L"End of file record not found");
			Memo1->Lines->Add(s);

			Application->NormalizeTopMosts();
			Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();

			m_firmware.resize(0);
			return;
*/		}

		m_firmware.resize(size);
	}

	if (file_ext == ".bin")
	{
		m_firmware.resize(buffer.size());
		memcpy(&m_firmware[0], &buffer[0], m_firmware.size());
	}

	Memo1->Lines->Add("");
	s.printf("Firmware size: %u bytes", m_firmware.size());
	Memo1->Lines->Add(s);

	// ******************************

	if (Form1)
	{
		Form1->stop();

		if (Form1->connected() && data_unit.m_vna_data.cmd_reset)
		{	// reboot the unit into DFU mode .. only works if we have an open and working serial link to the unit

			Memo1->Lines->Add("");
			Memo1->Lines->Add("Putting unit into DFU mode ..");

			Form1->addSerialTxCommand("reset dfu");
			Form1->waitTxCommandEmpty(1000);
			Form1->disconnect();

			if (Form1->serialCommandsSize() <= 0)
			{	// all seems OK
				Sleep(1000);
				usbProcess();
				Memo1->Lines->Add("done");
			}
			else
				Memo1->Lines->Add("error");
		}
	}

	if (!openDFUDevice(m_dfu_name))
	{
		Memo1->Lines->Add("");
		Memo1->Lines->Add("No valid DFU device found");
		Memo1->Lines->Add("");
		Memo1->Lines->Add("Have you put your NanoVNA into DFU mode ?");
		return;
	}

	if (m_firmware.size() > m_flash_size)
	{
		closeDFUDevice();

		String s;
		s.printf(L"Firmware is too large for the MCU [%u  %u]", m_firmware.size(), m_flash_size);
		Memo1->Lines->Add(s);

		Application->NormalizeTopMosts();
		Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();

		m_firmware.resize(0);
		return;
	}

	if (m_DfuDesc.wTransfertSize <= 0)
	{
		closeDFUDevice();
		Memo1->Lines->Add(" error: no block size 1");
		return;
	}

//	const uint32_t page_size = 0x0800;
	const uint32_t page_size = m_DfuDesc.wTransfertSize;

	m_block_buf.resize(page_size);

	switch (unit_type)
	{
		default:
		case UNIT_TYPE_NONE:
			closeDFUDevice();
			Memo1->Lines->Add(" error: invalid unit type");
			return;
		case UNIT_TYPE_UNKNOWN:
			break;
		case UNIT_TYPE_NANOVNA_H:
			if (m_flash_size != 131072)
			{
				closeDFUDevice();
				Memo1->Lines->Add("");
				Memo1->Lines->Add("error: the unit is not a NanoVNA-H");
				return;
			}
			break;
		case UNIT_TYPE_NANOVNA_H4:
			if (m_flash_size != 262144)
			{
				closeDFUDevice();
				Memo1->Lines->Add("");
				Memo1->Lines->Add("error: the unit is not a NanoVNA-H4");
				return;
			}
			break;
		case UNIT_TYPE_NANOVNA_H7:
			if (m_flash_size != 262144)
			{
				closeDFUDevice();
				Memo1->Lines->Add("");
				Memo1->Lines->Add("error: the unit is not a NanoVNA-H7");
				return;
			}
			break;
		case UNIT_TYPE_NANOVNA_V2:
			closeDFUDevice();
			Memo1->Lines->Add("");
			Memo1->Lines->Add("error: NanoVNA-V2 not supported here");
			return;
		case UNIT_TYPE_JANVNA_V2:
			closeDFUDevice();
			Memo1->Lines->Add("");
			Memo1->Lines->Add("error: JanVNA-V2 not supported here");
			return;
	}

	// ***************************************************
	// first read the entire flash area

	#if 0
		Memo1->Lines->Add("");
		Memo1->Lines->Add("reading entire flash .. ");

		ProgressBar1->Position = 0;
		ProgressBar1->Update();

		if (!selectCurrentConfig(0, 0, 0))
		{
			closeDFUDevice();
			Memo1->Lines->Add(" error: config 1");
			return;
		}

		std::vector <uint8_t> flash_data;

		if (!setAddressPointer(m_flash_addr))
		{
			closeDFUDevice();
			Memo1->Lines->Add(" error: address pointer ");
			return;
		}

		for (uint32_t addr = m_flash_addr, block = 2; addr < m_flash_addr + m_firmware.size(); addr += page_size)
		{
			memset(&m_block_buf[0], 0xff, m_block_buf.size());

			uint32_t block_size = m_block_buf.size();
			if (block_size > (m_firmware.size() - addr))
				 block_size =  m_firmware.size() - addr;

			if (!upload(&m_block_buf[0], m_block_buf.size(), block++))
			{
				closeDFUDevice();
				Memo1->Lines->Add(" error: reading page ");
				return;
			}

			// save the flash data
			const unsigned int index = flash_data.size();
			flash_data.resize(index + block_size);
			memcpy(&flash_data[index], &m_block_buf[0], block_size);

			ProgressBar1->Position = ((uint64_t)index * ProgressBar1->Max) / m_firmware.size();
			ProgressBar1->Update();
		}

		Memo1->Lines->Add(" done ");

		// save the flash to a file
		common.saveFile("saved_flash.bin", flash_data);
	#endif

	// ***************************************************
	// erase the old firmware

	Memo1->Lines->Add("");
	Memo1->Lines->Add("erasing old firmware .. ");

	ProgressBar1->Position = 0;
	ProgressBar1->Update();

	if (!selectCurrentConfig(0, 0, 0))
	{
		closeDFUDevice();
		Memo1->Lines->Add(" error: config 2");
		return;
	}

	for (uint32_t addr = m_flash_addr; addr < m_flash_addr + m_firmware.size(); addr += page_size)
	{
		if (!eraseBlock(addr))
		{
			closeDFUDevice();
			Memo1->Lines->Add(" error: erasing block 0x" + IntToHex((int64_t)addr, 8) + " ");
			return;
		}
		ProgressBar1->Position = ((uint64_t)((addr + page_size) - m_flash_addr) * ProgressBar1->Max) / m_firmware.size();
		ProgressBar1->Update();
	}

	Memo1->Lines->Add(" done ");

	// **********************************************
	// write the new firmware

	ProgressBar1->Position = 0;
	ProgressBar1->Update();

	Memo1->Lines->Add("");
	Memo1->Lines->Add("writing new firmware .. ");

	if (!selectCurrentConfig(0, 0, 0))
	{
		closeDFUDevice();
		Memo1->Lines->Add(" error: config 3");
		return;
	}

	for (uint32_t addr = 0; addr < m_firmware.size(); addr += m_block_buf.size())
	{
		if (!setAddressPointer(m_flash_addr + addr))
		{
			closeDFUDevice();
			Memo1->Lines->Add(" error: address pointer ");
			return;
		}

		memset(&m_block_buf[0], 0xff, m_block_buf.size());

		uint32_t block_size = m_block_buf.size();
		if (block_size > (m_firmware.size() - addr))
			 block_size =  m_firmware.size() - addr;

		memmove(&m_block_buf[0], &m_firmware[addr], block_size);

		uint32_t i = 0;
		for (i = 0; i < m_block_buf.size(); i++)
			if (m_block_buf[i] != 0xff)
				break;	// we have data to send

		if (i < m_block_buf.size())
		{
			if (!download(&m_block_buf[0], m_block_buf.size(), 2))
			{
				closeDFUDevice();
				Memo1->Lines->Add(" error: writing page ");
				return;
			}
		}

		ProgressBar1->Position = ((uint64_t)(addr + m_block_buf.size()) * ProgressBar1->Max) / m_firmware.size();
		ProgressBar1->Update();
	}

	selectCurrentConfig(0, 0, 1);

	// **********************************************

	Memo1->Lines->Add(" done ");

	// ******************************
	// Re-boot the unit

	LeaveDFUButton->Enabled = true;

//	rebootUnit();

	// ******************************

	closeDFUDevice();

	Memo1->Lines->Add("");
	Memo1->Lines->Add("The VNA now needs to be power cycled (turned off then on again).");
	Memo1->Lines->Add("");
}

void __fastcall TUploadFirmwareForm::rebootUnit()
{
//	return;  // don't do this - until the PC blue screen problem is solved (USB driver bug ?)

	if (!m_stdfu.isOpen())
		if (!openDFUDevice(m_dfu_name))
			return;

	if (m_stdfu.selectCurrentConfiguration(0, 0, 1) == STDFU_NOERROR)
	{
		DFUSTATUS status;

		Memo1->Lines->Add("");
		Memo1->Lines->Add("Removing read protection .. ");

		m_tick.mark();
		m_stdfu.getStatus(&status);
		while (status.bState != STATE_DFU_IDLE && m_tick.secs(false) < 1)
		{
			m_stdfu.clrStatus();
			m_stdfu.getStatus(&status);
		}

		uint8_t cmd[1] = {0x92};	// Remove Protection
		m_stdfu.download(cmd, 1, CONTROL_BLOCK);
		//m_stdfu.download(cmd, 0, CONTROL_BLOCK);

		m_stdfu.getStatus(&status);
	}

	closeDFUDevice();

//	LeaveDFUButton->Enabled = false;
}

void __fastcall TUploadFirmwareForm::OneOfElevenFirmwareButtonClick(TObject *Sender)
{
	/*
	std::vector <uint8_t> firmware;

	if (common.fetchResource("FIRMWARE_H_1OF11_LSI_DFU", firmware) <= 0)
	{
		String s = L"Firmware data not found";
		Memo1->Lines->Add(s);

		Application->NormalizeTopMosts();
		Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();

		return;
	}

	uploadFirmwareFile(firmware, ".dfu", UNIT_TYPE_NANOVNA_H);
    */
}

void __fastcall TUploadFirmwareForm::Button1Click(TObject *Sender)
{
	std::vector <uint8_t> firmware;

	if (common.fetchResource("FIRMWARE_H4_DISLORD_DFU", firmware) <= 0)
	{
		String s = L"Firmware data not found";
		Memo1->Lines->Add(s);

		Application->NormalizeTopMosts();
		Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();

		return;
	}

	uploadFirmwareFile(firmware, ".dfu", UNIT_TYPE_NANOVNA_H4);
}

void __fastcall TUploadFirmwareForm::Button2Click(TObject *Sender)
{
	std::vector <uint8_t> firmware;

	if (common.fetchResource("FIRMWARE_H_DISLORD_DFU", firmware) <= 0)
	{
		String s = L"Firmware data not found";
		Memo1->Lines->Add(s);

		Application->NormalizeTopMosts();
		Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();

		return;
	}

	uploadFirmwareFile(firmware, ".dfu", UNIT_TYPE_NANOVNA_H);
}

void __fastcall TUploadFirmwareForm::OpenFileBitBtnClick(TObject *Sender)
{
	CCriticalSection cs(readWrite_cs, false);
	if (!cs.tryEnter())
		return;

	Application->NormalizeTopMosts();
	const bool ok = OpenDialog1->Execute();
	Application->RestoreTopMosts();

	if (!ok)
		return;

	Memo1->Lines->Clear();
	Memo1->Lines->Add("");

	this->Update();

	String name = OpenDialog1->FileName;
	String ext  = ExtractFileExt(name).LowerCase();

	std::vector <uint8_t> buffer;

	{
		String s;

		s = "Loading \"" + name + "\" ..";
		Memo1->Lines->Add(s);

		const int size = common.loadFile(name, buffer);
		if (size <= 0)
		{
			switch (size)
			{
				case -1:
					s = "could not open the file";
					Memo1->Lines->Add(s);
					Application->NormalizeTopMosts();
					Application->MessageBoxA(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
					return;

				case -2:
					s = "file is too small";
					Memo1->Lines->Add(s);
					Application->NormalizeTopMosts();
					Application->MessageBoxA(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
					return;

				case -3:
					s = "error reading the file";
					Memo1->Lines->Add(s);
					Application->NormalizeTopMosts();
					Application->MessageBoxA(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
					return;

				default:
					return;
			}
		}

		Memo1->Lines->Add("");
		s.printf(L"%u bytes loaded OK from %s", buffer.size(), name.c_str());
		Memo1->Lines->Add(s);
	}

	cs.leave();

	uploadFirmwareFile(buffer, ext, UNIT_TYPE_UNKNOWN);
}

void __fastcall TUploadFirmwareForm::FormHide(TObject *Sender)
{
//	rebootUnit();
}

void __fastcall TUploadFirmwareForm::LeaveDFUButtonClick(TObject *Sender)
{
	rebootUnit();
}

void __fastcall TUploadFirmwareForm::FormShow(TObject *Sender)
{
	if (!m_initialised)
	{
		// move to the saved position
		this->Top    = settings.firmwareWindowPos.top;
		this->Left   = settings.firmwareWindowPos.left;
		this->Width  = settings.firmwareWindowPos.width;
		this->Height = settings.firmwareWindowPos.height;

		m_initialised = true;
	}
}

void __fastcall TUploadFirmwareForm::CloseBitBtnClick(TObject *Sender)
{
//	rebootUnit();
	this->Hide();
}


void __fastcall TUploadFirmwareForm::FormMouseEnter(TObject *Sender)
{
//	this->SetFocus();
}

void __fastcall TUploadFirmwareForm::SaveFlashBitBtnClick(TObject *Sender)
{
	saveFlashToFile();
}

void __fastcall TUploadFirmwareForm::saveFlashToFile()
{
	// ******************************

	if (Form1)
	{
		Form1->stop();

		if (Form1->connected() && data_unit.m_vna_data.cmd_reset)
		{	// reboot the unit into DFU mode .. only works if we have an open and working serial link to the unit

			Memo1->Lines->Add("");
			Memo1->Lines->Add("Putting unit into DFU mode ..");

			Form1->addSerialTxCommand("reset dfu");
			Form1->waitTxCommandEmpty(1000);
			Form1->disconnect();

			if (Form1->serialCommandsSize() <= 0)
			{	// all seems OK
				Sleep(1000);
				usbProcess();
				Memo1->Lines->Add("done");
			}
			else
				Memo1->Lines->Add("error");
		}
	}

	if (!openDFUDevice(m_dfu_name))
	{
		Memo1->Lines->Add("");
		Memo1->Lines->Add("No valid DFU device found");
		Memo1->Lines->Add("");
		Memo1->Lines->Add("Have you put your NanoVNA into DFU mode ?");
		return;
	}

	if (m_flash_size <= 0)
	{
		closeDFUDevice();
		Memo1->Lines->Add(" error: flash size");
		return;
	}

	if (m_DfuDesc.wTransfertSize <= 0)
	{
		closeDFUDevice();
		Memo1->Lines->Add(" error: no block size 1");
		return;
	}

//	const uint32_t page_size = 0x0800;
	const uint32_t page_size = m_DfuDesc.wTransfertSize;

	m_block_buf.resize(page_size);

	// ***************************************************
	// read the entire flash area

	Memo1->Lines->Add("");
	Memo1->Lines->Add("reading entire flash (" + common.valueToStr(m_flash_size, true, true) + "B) .. ");

	ProgressBar1->Position = 0;
	ProgressBar1->Update();

	if (!selectCurrentConfig(0, 0, 0))
	{
		closeDFUDevice();
		Memo1->Lines->Add(" error: config 1");
		return;
	}

	if (!setAddressPointer(m_flash_addr))
	{
		closeDFUDevice();
		Memo1->Lines->Add(" error: address pointer ");
		return;
	}

	std::vector <uint8_t> flash_data(m_flash_size);

	for (uint32_t addr = m_flash_addr, index = 0, block = 2; addr < m_flash_addr + m_flash_size; addr += page_size)
	{
		memset(&m_block_buf[0], 0xff, m_block_buf.size());

		uint32_t block_size = m_block_buf.size();
		if (block_size > (m_firmware.size() - addr))
			 block_size =  m_firmware.size() - addr;

		if (!upload(&m_block_buf[0], m_block_buf.size(), block++))
		{
			closeDFUDevice();
			Memo1->Lines->Add(" error: reading page ");
			return;
		}

		// save the flash data
		memcpy(&flash_data[index], &m_block_buf[0], block_size);
		index += block_size;

		ProgressBar1->Position = ((uint64_t)index * ProgressBar1->Max) / m_flash_size;
		ProgressBar1->Update();
	}

	Memo1->Lines->Add("read " + common.valueToStr(flash_data.size(), true, true) + "bytes of flash");
	Memo1->Lines->Add(" done ");

	closeDFUDevice();

	// ***************************************************
	// now save the flash to a file

	saveData(flash_data, "");

	// ***************************************************
}

