
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#include <vcl.h>
#include <stdio.h>

#pragma hdrstop

#include "UploadFirmwareV2Unit.h"
#include "DataUnit.h"
#include "Unit1.h"
#include "common.h"
#include "Settings.h"

#pragma package(smart_init)
#pragma link "cgauges"
#pragma resource "*.dfm"

// *********************************************

#define DEFAULT_FIRMWARE_BASE_ADDR			0x08000000
#define DEFAULT_FIRMWARE_V2_BASE_ADDR		(DEFAULT_FIRMWARE_BASE_ADDR + 0x00004000)

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

// *********************************************

TUploadFirmwareV2Form *UploadFirmwareV2Form = NULL;

__fastcall TUploadFirmwareV2Form::TUploadFirmwareV2Form(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall TUploadFirmwareV2Form::FormCreate(TObject *Sender)
{
	AnsiString s;

	m_initialised = false;

	Caption = Application->Title + " " + this->Caption;

	// stop flicker
	Memo1->ControlStyle = Memo1->ControlStyle << csOpaque;

	Memo1->Clear();

	Memo1->Lines->Add("");
	Memo1->Lines->Add("You will need to ensure your NanoVNA V2 is in firmware upload mode before uploading new firmware.");
	Memo1->Lines->Add("");
	Memo1->Lines->Add("Ensure you're disconnected from the VNA (disconnect button top/left main window).");
	Memo1->Lines->Add("Turn the VNA off.");
	Memo1->Lines->Add("Ensure the VNA is plugged into your PC using the USB cable.");
	Memo1->Lines->Add("Press and hold down the left user button on the VNA whilst turning the VNA back on.");
	Memo1->Lines->Add("The VNA's screen should then be all white/blank (firmware upload mode).");
	Memo1->Lines->Add("Reconnect to the VNA (connect button top/left main window).");
	Memo1->Lines->Add("");
	Memo1->Lines->Add("Press the \"open folder\" button above this memo to find the correct V2 firmware file.");
	#ifdef _DEBUG
		Memo1->Lines->Add("Or select one of the built-in firmwares to upload to your VNA (the above top row of buttons).");
	#endif
	Memo1->Lines->Add("");
	Memo1->Lines->Add("If you uploaded the wrong firmware or the upload fails for whatever reason, don't");
	Memo1->Lines->Add("worry, just repeat the above process with the correct firmware file.");

	OpenDialog1->InitialDir = ExtractFilePath(Application->ExeName);

//	CGauge1->DoubleBuffered = true;
	CGauge1->Progress = 0;

	m_flash_addr = DEFAULT_FIRMWARE_V2_BASE_ADDR;
	m_flash_size = 0;

	m_firmware.reserve(1024 * 1024 * 4);	// 4MB

//	LeaveDFUButton->Enabled = false;

//	#ifdef _DEBUG
//		GridPanel1->Visible = true;
//		GridPanel1->Enabled = true;
//	#else
//		GridPanel1->Visible = false;
//		GridPanel1->Enabled = false;
//	#endif

	// move to the saved position
	this->Top    = settings.firmwareV2WindowPos.top;
	this->Left   = settings.firmwareV2WindowPos.left;
	this->Width  = settings.firmwareV2WindowPos.width;
	this->Height = settings.firmwareV2WindowPos.height;
}

void __fastcall TUploadFirmwareV2Form::FormDestroy(TObject *Sender)
{
	//
}

void __fastcall TUploadFirmwareV2Form::FormClose(TObject *Sender, TCloseAction &Action)
{
//	rebootUnit();
}

void __fastcall TUploadFirmwareV2Form::WMWindowPosChanging(TWMWindowPosChanging &msg)
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

void __fastcall TUploadFirmwareV2Form::show()
{
	this->Show();
	this->BringToFront();
	this->Update();
}

void __fastcall TUploadFirmwareV2Form::ClearMemoButtonClick(TObject *Sender)
{
	Memo1->Clear();
}

void __fastcall TUploadFirmwareV2Form::FormKeyDown(TObject *Sender, WORD &Key,
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

void __fastcall TUploadFirmwareV2Form::uploadFirmwareFile(std::vector <uint8_t> &buffer, AnsiString file_ext)
{
	String s;

	if (!Form1)
		return;

	if (!Form1->connected() || !nanovna2_comms.inDFUMode())
		return;

	Memo1->Lines->Add("");
/*
	if (unit_type == UNIT_TYPE_NONE)
	{
		String s = L"Invalid unit type";
		Memo1->Lines->Add(s);

		Application->NormalizeTopMosts();
		Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}
*/
	if (buffer.size() <= 128)
	{
		s = L"File is too small to be valid";
		Memo1->Lines->Add(s);

		Application->NormalizeTopMosts();
		Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	CGauge1->Progress = 0;
	CGauge1->Update();

	// ******************************

	m_firmware.resize(0);

	if (file_ext != ".hex" && file_ext != ".dfu" && file_ext != ".bin")
	{
		s = "Unknown file type, must be .bin, .dfu or .hex";
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
			s.printf(L"Invalid DFU prefix signature");
			Memo1->Lines->Add(s);

			Application->NormalizeTopMosts();
			Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		if (prefix->b_version != 1)
		{
			s.printf(L"Only DFU file format v1 currently supported [%u]", prefix->b_version);
			Memo1->Lines->Add(s);

			Application->NormalizeTopMosts();
			Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		if ((prefix->dfu_image_size + sizeof(t_dfuse_suffix)) > buffer.size())
		{
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
			s.printf(L"Invalid DFU crc [%08x  actual: %08x]", suffix->dw_crc, crc);
			Memo1->Lines->Add(s);

			Application->NormalizeTopMosts();
			Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		if (suffix->b_length != sizeof(t_dfuse_suffix))
		{
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
				s.printf(L"DFU element address [%u]: 0x%08x", i, element->address);
				Memo1->Lines->Add(s);
				s.printf(L"DFU element size [%u]:    %u", i, element->size);
				Memo1->Lines->Add(s);
			}

			if (element->size <= 128 || element->size >= (1024 * 1024 * 4))
			{
				s.printf(L"DFU firmware size too small");
				Memo1->Lines->Add(s);

				Application->NormalizeTopMosts();
				Application->MessageBox(s.w_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
				return;
			}

			if (element->address < DEFAULT_FIRMWARE_BASE_ADDR || element->address >= (DEFAULT_FIRMWARE_BASE_ADDR + (1024 * 1024)))
			{
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
			String line;

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
/*			s.printf(L"End of file record not found");
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
	s.printf(L"Firmware size: %u bytes", m_firmware.size());
	Memo1->Lines->Add(s);

	// ******************************

	if (data_unit.m_vna_data.type != UNIT_TYPE_NANOVNA_V2 || !nanovna2_comms.inDFUMode())
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Your unit does not appear to be in DFU mode", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();

		m_firmware.resize(0);
		return;
	}

	// **********************************************
	// write the new firmware

	nanovna2_comms.mode = MODE_DFU_MODE;

	CGauge1->Progress = 0;
	CGauge1->Update();

	Memo1->Lines->Add("");
	Memo1->Lines->Add("writing new firmware .. ");

	nanovna2_comms.addTxNulls(true, 64);
	nanovna2_comms.addTxFlashWriteAddr(DEFAULT_FIRMWARE_V2_BASE_ADDR);
	nanovna2_comms.sendData();

	int64_t addr_error = -1;

	int block_size = 128;
	m_block_buf.resize(block_size);

	for (uint32_t addr = 0; addr < m_firmware.size() && Form1->connected(); addr += block_size)
	{
		uint32_t block_size = m_block_buf.size();
		if (block_size > (m_firmware.size() - addr))
			block_size = m_firmware.size() - addr;

		memset(&m_block_buf[0], 0xff, m_block_buf.size());
		memcpy(&m_block_buf[0], &m_firmware[addr], block_size);

		nanovna2_comms.addTxWriteFlashData(true, m_block_buf, true);
		nanovna2_comms.sendData();

		uint8_t ack_byte = ~CMD_V2_INDICATE_ACK;

		// wait for the VNA to finish writing the data to flash - wait for up to 2 seconds for the reply/ACK
		Form1->m_comms.rx_timer.mark();
		while (Form1->connected() && Form1->m_comms.rx_timer.millisecs(false) < 2000 && ack_byte != CMD_V2_INDICATE_ACK)
		{
			Sleep(1);
			Application->ProcessMessages();

			const int bytes_read = Form1->fetchCommsData(true);
			if (bytes_read < 0)
				break;		// error

			if (bytes_read <= 0)
				continue;	// no data received from the V2

			Form1->m_comms.rx_timer.mark();

			// display all the received bytes (as hex)
			s = "";
			for (int k = 0, i = 0; k < bytes_read; k++)
			{
				String s2;
				const uint8_t b = (int)Form1->m_comms.rx.buffer[k];		// fetch an RX byte
				s2.printf(L" %02x", b);
				s += s2;
				if (++i >= 16)
				{
					i = 0;
					Form1->pushCommMessage("rx:" + s);
					s = "";
				}
			}
			if (!s.IsEmpty())
			{
				Form1->pushCommMessage("rx:" + s);
				s = "";
			}

			// look for the ACK byte
			for (int k = 0; k < bytes_read && ack_byte != CMD_V2_INDICATE_ACK; k++)
				ack_byte = (int)Form1->m_comms.rx.buffer[k];
		}

		if (ack_byte != CMD_V2_INDICATE_ACK)
		{	// flash write ACK not received
			addr_error = (int64_t)addr;
			break;
		}

		CGauge1->Progress = ((uint64_t)(addr + block_size) * CGauge1->MaxValue) / m_firmware.size();
		CGauge1->Update();
	}

	Form1->m_comms.rx.buffer_wr = 0;

	if (addr_error < 0)
	{
		Memo1->Lines->Add(" done ");
	}
	else
	{	// error
		s.printf(L"error writing to flash address 0x%08X", (uint32_t)(DEFAULT_FIRMWARE_V2_BASE_ADDR + addr_error));
		Memo1->Lines->Add(s);
	}

	// Re-boot the unit
	if (addr_error < 0)
	{
		LeaveDFUButton->Enabled = true;

		rebootUnit();

		Hide();
/*
		Memo1->Lines->Add("");
		Memo1->Lines->Add("The VNA now needs to be power cycled (turned off then on again).");
		Memo1->Lines->Add("");
*/
	}

	if (Form1->connected())
		nanovna2_comms.mode = MODE_IDLE;
}

void __fastcall TUploadFirmwareV2Form::rebootUnit()
{
	nanovna2_comms.softReboot();

	if (Form1)
		Form1->disconnect();

//	LeaveDFUButton->Enabled = false;
}

void __fastcall TUploadFirmwareV2Form::OpenFileBitBtnClick(TObject *Sender)
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

	uploadFirmwareFile(buffer, ext);
}

void __fastcall TUploadFirmwareV2Form::FormHide(TObject *Sender)
{
//	rebootUnit();
}

void __fastcall TUploadFirmwareV2Form::LeaveDFUButtonClick(TObject *Sender)
{
	rebootUnit();
}

void __fastcall TUploadFirmwareV2Form::FormShow(TObject *Sender)
{
	if (!m_initialised)
	{
		// move to the saved position
		this->Top    = settings.firmwareV2WindowPos.top;
		this->Left   = settings.firmwareV2WindowPos.left;
		this->Width  = settings.firmwareV2WindowPos.width;
		this->Height = settings.firmwareV2WindowPos.height;

		m_initialised = true;
	}
}

void __fastcall TUploadFirmwareV2Form::CloseBitBtnClick(TObject *Sender)
{
//	rebootUnit();
	this->Hide();
}

void __fastcall TUploadFirmwareV2Form::FormMouseEnter(TObject *Sender)
{
//	this->SetFocus();
}

