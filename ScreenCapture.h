
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef ScreenCaptureH
#define ScreenCaptureH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.WinXCtrls.hpp>

class TVNAScreenCaptureForm : public TForm
{
__published:	// IDE-managed Components
	TSaveDialog *SaveDialog1;
	TToggleSwitch *ActualSizeToggleSwitch;
	TBitBtn *CloseBitBtn;
	TBitBtn *SaveImageBitBtn;
	TBitBtn *CopyImageBitBtn;
	TBitBtn *RefreshImageBitBtn;
	TToggleSwitch *ToggleSwitch1;
	TLabel *ActualSize;
	TLabel *Remote;
	TPanel *Panel1;
	TImage *ResizeImage;
	TImage *ActualSizeImage;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ActualSizeToggleSwitchClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall CloseBitBtnClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormMouseEnter(TObject *Sender);
	void __fastcall SaveImageBitBtnClick(TObject *Sender);
	void __fastcall CopyImageBitBtnClick(TObject *Sender);
	void __fastcall RefreshImageBitBtnClick(TObject *Sender);
	void __fastcall ToggleSwitch1Click(TObject *Sender);

	void __fastcall ResizeImageMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ActualSizeImageMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,  int X, int Y);

	void __fastcall ResizeImageMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ActualSizeImageMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);

	void __fastcall ResizeImageMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall ActualSizeImageMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);

	void __fastcall ActualSizeImageMouseLeave(TObject *Sender);
	void __fastcall ResizeImageMouseLeave(TObject *Sender);
private:	// User declarations
	bool m_initialised;

	int m_image_dw;
	int m_image_dh;
	int m_image_w;
	int m_image_h;
	struct {
		int    x;
		int    y;
		int down;
	} m_mouse;

	String m_title;

	Graphics::TBitmap *m_bitmap;

	void __fastcall WMWindowPosChanging(TWMWindowPosChanging &msg);

	void __fastcall saveBitmap(Graphics::TBitmap *bm, String dialog_title);

	void __fastcall updateImage();

protected:
	#pragma option push -vi-
	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosMsg, WMWindowPosChanging);
	END_MESSAGE_MAP(TForm)
	#pragma option pop

public:		// User declarations
	__fastcall TVNAScreenCaptureForm(TComponent* Owner);

	void __fastcall show();
	void __fastcall addImage(Graphics::TBitmap *bitmap);
};

extern PACKAGE TVNAScreenCaptureForm *VNAScreenCaptureForm;

#endif
