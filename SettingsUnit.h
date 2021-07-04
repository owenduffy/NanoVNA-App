
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef SettingsUnitH
#define SettingsUnitH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.WinXCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Samples.Spin.hpp>
#include <Vcl.Buttons.hpp>

#include <vector>

#include "types.h"

class TSettingsForm : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TLabel *Label7;
	TGroupBox *FreqBandGroupBox;
	TLabel *Label10;
	TEdit *FreqBandLowMHzEdit;
	TEdit *FreqBandHighMHzEdit;
	TLabel *Label11;
	TListBox *FreqBandListBox;
	TButton *FreqBandDeleteButton;
	TButton *FreqBandAddButton;
	TEdit *FreqBandNameEdit;
	TLabel *Label12;
	TButton *SetScanButton;
	TComboBox *FreqBandPadComboBox;
	TLabel *Label8;
	TButton *FreqBandDefaultButton;
	TToggleSwitch *FreqBandEnableToggleSwitch;
	TGroupBox *VNAGroupBox;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TEdit *ThresholdEdit;
	TButton *ClearConfigButton;
	TButton *SetTimeButton;
	TButton *SaveConfigButton;
	TLabel *Label13;
	TColorDialog *ColorDialog1;
	TFontDialog *FontDialog1;
	TSpinEdit *VBatOffsetSpinEdit;
	TEdit *RecordPathEdit;
	TLabel *Label6;
	TBitBtn *SelectRecordPathBitBtn;
	TBitBtn *CloseBitBtn;
	TBevel *ColourBevel;
	TLabel *Label9;
	TComboBox *GUIStyleComboBox;
	TPaintBox *GraphColourPaintBox;
	TButton *DefaultLightColoursButton;
	TButton *DefaultDarkColoursButton;
	TButton *FontButton;
	TLabel *Label14;
	TTrackBar *LineAlphaTrackBar;
	TLabel *Label5;
	TTrackBar *LineWidthTrackBar;
	TLabel *Label15;
	TToggleSwitch *MarkerFillToggleSwitch;
	TTrackBar *BorderWidthTrackBar;
	TLabel *Label16;
	TButton *Button1;
	TButton *DefaultGraphScalesButton;
	TLabel *Label17;
	TToggleSwitch *AutoScalePeakHoldToggleSwitch;
	TLabel *Label18;
	TToggleSwitch *SmithBothScalesToggleSwitch;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall SetTimeButtonClick(TObject *Sender);
	void __fastcall ClearConfigButtonClick(TObject *Sender);
	void __fastcall SaveConfigButtonClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall FreqBandListBoxKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall FreqBandAddButtonClick(TObject *Sender);
	void __fastcall FreqBandDeleteButtonClick(TObject *Sender);
	void __fastcall FreqBandListBoxClick(TObject *Sender);
	void __fastcall FreqBandListBoxDblClick(TObject *Sender);
	void __fastcall SetScanButtonClick(TObject *Sender);
	void __fastcall FreqBandDefaultButtonClick(TObject *Sender);
	void __fastcall FreqBandEnableToggleSwitchClick(TObject *Sender);
	void __fastcall GraphColourPaintBoxPaint(TObject *Sender);
	void __fastcall GUIStyleComboBoxChange(TObject *Sender);
	void __fastcall GraphColourPaintBoxMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall GraphColourPaintBoxMouseLeave(TObject *Sender);
	void __fastcall GraphColourPaintBoxMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall DefaultDarkColoursButtonClick(TObject *Sender);
	void __fastcall ThresholdEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall LineAlphaTrackBarChange(TObject *Sender);
	void __fastcall LineWidthTrackBarChange(TObject *Sender);
	void __fastcall FontButtonClick(TObject *Sender);
	void __fastcall FontDialog1Apply(TObject *Sender, HWND Wnd);
	void __fastcall VBatOffsetSpinEditChange(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall RecordPathEditChange(TObject *Sender);
	void __fastcall SelectRecordPathBitBtnClick(TObject *Sender);
	void __fastcall CloseBitBtnClick(TObject *Sender);
	void __fastcall MarkerFillToggleSwitchClick(TObject *Sender);
	void __fastcall FormMouseEnter(TObject *Sender);
	void __fastcall FreqBandPadComboBoxChange(TObject *Sender);
	void __fastcall BorderWidthTrackBarChange(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall DefaultGraphScalesButtonClick(TObject *Sender);
	void __fastcall AutoScalePeakHoldToggleSwitchClick(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall SmithBothScalesToggleSwitchClick(TObject *Sender);



private:	// User declarations

	bool m_initialised;

	Graphics::TBitmap *m_colour_bm;

	int m_mouse_x;
	int m_mouse_y;
	int m_colour_picker_index;
	std::vector <TRect> m_colour_picker_rect;

	TColor *m_colour_picker_colour;

	void __fastcall WMWindowPosChanging(TWMWindowPosChanging &msg);
	void __fastcall OnMessage(tagMSG &msg, bool &handled);
	void __fastcall onThemeChange(TObject *Sender);

	String __fastcall colourName(const int index);

	void __fastcall colourPickerColourUpdated(TObject *Sender, TColor colour);
	void __fastcall colourPickerOnClose(TObject *Sender, const int type, TColor colour);

	void __fastcall showColour(Graphics::TBitmap *bm, int x, int y, String name, TColor colour);

	bool __fastcall fetchFreqBandInput(t_freq_band &fb);
	void __fastcall updateFreqBandListBox();
	void __fastcall deleteSelectedFreqBand();
	void __fastcall applyFreqBandSetting(t_freq_band fb);
	void __fastcall setScanRange();

	void __fastcall sendThresholdFreq();
	void __fastcall sendVBATOffset();

protected:
	#pragma option push -vi-
		BEGIN_MESSAGE_MAP
			VCL_MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosMsg, WMWindowPosChanging);
		END_MESSAGE_MAP(TForm)
	#pragma option pop

public:		// User declarations
	__fastcall TSettingsForm(TComponent* Owner);

	void __fastcall updateInfo();
	void __fastcall updateThresholdHz();
	void __fastcall updateVBatOffset();
	void __fastcall update();
	void __fastcall show();
};

extern PACKAGE TSettingsForm *SettingsForm;

#endif

