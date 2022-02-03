
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef UploadFirmwareUnitH
#define UploadFirmwareUnitH

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>

#include <setupapi.h>		// You must Manually add the setupapi.lib to the project library link tab!
#include <winioctl.h>
#include <initguid.h>

#include <vector>

#include "types.h"
#include "CriticalSection.h"
#include "HighResolutionTick.h"
#include "st_dfu.h"
#include "libusb.h"

// STM32F in bootloader mode (not DFU mode)
// shows up in libusb device list as ..
//
// list_devices: index            0
// list_devices: vid/pid          0483 DF11
// list_devices: manufacturer     STMicroelectronics
// list_devices: product          STM32  BOOTLOADER
// list_devices: serial number    FFFFFFFEFFFF
//
// DfuUtil uses that mode


class TUploadFirmwareForm : public TForm
{
__published:	// IDE-managed Components
	TOpenDialog *OpenDialog1;
	TMemo *Memo1;
	TButton *ClearMemoButton;
	TProgressBar *ProgressBar1;
	TComboBox *USBDeviceComboBox;
	TLabel *Label1;
	TLabel *Label3;
	TBitBtn *OpenFileBitBtn;
	TButton *LeaveDFUButton;
	TGridPanel *GridPanel1;
	TBitBtn *CloseBitBtn;
	TBitBtn *SaveFlashBitBtn;
	TSaveDialog *SaveDialog1;
	TLabel *Label2;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ClearMemoButtonClick(TObject *Sender);
	void __fastcall USBDeviceComboBoxDropDown(TObject *Sender);
	void __fastcall USBDeviceComboBoxSelect(TObject *Sender);
	void __fastcall USBDeviceComboBoxChange(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall OpenFileBitBtnClick(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall LeaveDFUButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall CloseBitBtnClick(TObject *Sender);
	void __fastcall FormMouseEnter(TObject *Sender);
	void __fastcall SaveFlashBitBtnClick(TObject *Sender);

private:	// User declarations
	bool m_initialised;

	std::vector <HDEVNOTIFY> m_notification_handle;

	AnsiString m_dfu_name;

	std::vector <t_usb_device> m_usb_devices;

	std::vector <uint8_t> m_firmware;
	std::vector <uint8_t> m_block_buf;

	std::vector <AnsiString> m_desc_string;

	uint32_t m_flash_addr;
	uint32_t m_flash_size;

	CHighResolutionTick m_tick;

	TSTDFU m_stdfu;

	DFU_FUNCTIONAL_DESCRIPTOR m_DfuDesc;
	UINT m_DfuInterfaceIdx;
	UINT m_NbOfAlternates;

	USB_DEVICE_DESCRIPTOR m_DeviceDesc;

//	CRITICAL_SECTION readWriteCriticalSection;
	CCriticalSectionObj readWrite_cs;

	void __fastcall OnDeviceChange(TMessage &msg);

	void __fastcall WMWindowPosChanging(TWMWindowPosChanging &msg);

	void __fastcall saveData(std::vector <uint8_t> &data, String ID);
	void __fastcall saveFlashToFile();

	void __fastcall uploadFirmwareFile(std::vector <uint8_t> &buffer, AnsiString file_ext, int unit_type = UNIT_TYPE_NONE);

	bool __fastcall updateUSBDeviceComboBox();

	void __fastcall closeDFUDevice(bool lock_critical = true);
	bool __fastcall openDFUDevice(String device_path);

	bool __fastcall waitUntilState(int state, float timeout_secs);
	bool __fastcall waitUntilIdle(float timeout_secs);
	bool __fastcall download(void *buffer, int size, int block);
	bool __fastcall upload(void *buffer, int size, int block);
	bool __fastcall getStringDiscrip(int index, void *buffer, int size);
	bool __fastcall selectCurrentConfig(int config_index, int interface_index, int alt_set_index);
	bool __fastcall sendCommand(uint8_t a, uint8_t b);
	bool __fastcall eraseBlock(uint32_t address);
	bool __fastcall setAddressPointer(uint32_t address);
	bool __fastcall getVidPid(AnsiString s, int &vid, int &pid);

	void __fastcall rebootUnit();

	void __fastcall usbProcess();

protected:
	#pragma option push -vi-
		BEGIN_MESSAGE_MAP
			VCL_MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosMsg, WMWindowPosChanging);
			VCL_MESSAGE_HANDLER(WM_DEVICECHANGE, TMessage, OnDeviceChange);
		END_MESSAGE_MAP(TForm)
	#pragma option pop

public:		// User declarations
	__fastcall TUploadFirmwareForm(TComponent* Owner);

	void __fastcall show();
};

extern PACKAGE TUploadFirmwareForm *UploadFirmwareForm;

#endif

