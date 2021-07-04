
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef UploadFirmwareV2UnitH
#define UploadFirmwareV2UnitH

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
#include "cgauges.h"

#include <vector>

#include "types.h"
#include "CriticalSection.h"
#include "HighResolutionTick.h"

class TUploadFirmwareV2Form : public TForm
{
__published:	// IDE-managed Components
	TOpenDialog *OpenDialog1;
	TMemo *Memo1;
	TButton *ClearMemoButton;
	TLabel *Label3;
	TBitBtn *OpenFileBitBtn;
	TButton *LeaveDFUButton;
	TBitBtn *CloseBitBtn;
	TCGauge *CGauge1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ClearMemoButtonClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall OpenFileBitBtnClick(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall LeaveDFUButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall CloseBitBtnClick(TObject *Sender);
	void __fastcall FormMouseEnter(TObject *Sender);

private:	// User declarations
	bool m_initialised;

	std::vector <t_usb_device> m_usb_devices;

	std::vector <uint8_t> m_firmware;
	std::vector <uint8_t> m_block_buf;

	uint32_t m_flash_addr;
	uint32_t m_flash_size;

	CHighResolutionTick m_tick;

//	CRITICAL_SECTION readWriteCriticalSection;
	CCriticalSectionObj readWrite_cs;

	void __fastcall WMWindowPosChanging(TWMWindowPosChanging &msg);

	void __fastcall uploadFirmwareFile(std::vector <uint8_t> &buffer, AnsiString file_ext);

	void __fastcall rebootUnit();

protected:
	#pragma option push -vi-
		BEGIN_MESSAGE_MAP
			VCL_MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosMsg, WMWindowPosChanging);
		END_MESSAGE_MAP(TForm)
	#pragma option pop

public:		// User declarations
	__fastcall TUploadFirmwareV2Form(TComponent* Owner);

	void __fastcall show();
};

extern PACKAGE TUploadFirmwareV2Form *UploadFirmwareV2Form;

#endif

