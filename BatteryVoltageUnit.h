
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef BatteryVoltageUnitH
#define BatteryVoltageUnitH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Buttons.hpp>

// GDI+ system
//#include <Unknwn.h>
//#include <windows.h>
#include <algorithm>
#pragma option push
	#pragma warn -8022
	#include <gdiplus.h>
#pragma option pop

#include <vector>

//#include "types.h"

typedef struct
{
	TDateTime datetime;
	uint16_t mv;
} t_battery_voltage;

class TBatteryVoltageForm : public TForm
{
__published:	// IDE-managed Components
	TPaintBox *PaintBox1;
	TStatusBar *StatusBar1;
	TSaveDialog *SaveDialog1;
	TLabel *Label1;
	TLabel *VBATOffsetLabel;
	TBitBtn *CloseBitBtn;
	TBitBtn *CopyBitBtn;
	TBitBtn *SaveBitBtn;
	TBitBtn *ClearBitBtn;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall PaintBox1Paint(TObject *Sender);
	void __fastcall PaintBox1MouseMove(TObject *Sender, TShiftState Shift,
			 int X, int Y);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall PaintBox1MouseLeave(TObject *Sender);
	void __fastcall CloseBitBtnClick(TObject *Sender);
	void __fastcall FormMouseEnter(TObject *Sender);
	void __fastcall SaveBitBtnClick(TObject *Sender);
	void __fastcall CopyBitBtnClick(TObject *Sender);
	void __fastcall ClearBitBtnClick(TObject *Sender);

private:
	bool m_initialised;

	Graphics::TBitmap *m_graph_bm;

	std::vector <t_battery_voltage> m_battery_voltage;

	int m_graph_x;
	int m_graph_y;
	int m_graph_width;
	int m_graph_height;

	int m_graph_mouse_x;
	int m_graph_mouse_y;
	int m_graph_mouse_index;

	uint16_t m_max_mv;
	uint16_t m_min_mv;

	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR                    m_gdiplusToken;

	std::vector <Gdiplus::PointF> m_gdi_points;

	void __fastcall WMWindowPosChanging(TWMWindowPosChanging &msg);

	void __fastcall saveImage(const bool to_clipboard);

protected:
	#pragma option push -vi-
	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosMsg, WMWindowPosChanging);
	END_MESSAGE_MAP(TForm)
	#pragma option pop

public:		// User declarations
	__fastcall TBatteryVoltageForm(TComponent* Owner);

	void __fastcall show();
	void __fastcall clear();
	void __fastcall addBatteryVoltage(const uint16_t vbat_mv, const uint16_t vbat_offset_mv);
};

extern PACKAGE TBatteryVoltageForm *BatteryVoltageForm;

#endif
