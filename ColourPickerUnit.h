
#ifndef ColourPickerUnitH
#define ColourPickerUnitH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Samples.Spin.hpp>

typedef void __fastcall (__closure *t_colour_picker_on_colour_updated)(TObject *Sender, TColor colour);
typedef void __fastcall (__closure *t_colour_picker_on_close)(TObject *Sender, const int type, TColor colour);

#define COLOUR_PICKER_OK_BUT       2
#define COLOUR_PICKER_CANCEL_BUT   1
#define COLOUR_PICKER_NO_BUT       0

class TColourPickerForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *SpectrumPanel;
	TPaintBox *SpectrumPaintBox;
	TPanel *LuminosityPanel;
	TPaintBox *LuminosityPaintBox;
	TPanel *ColourPanel;
	TPaintBox *ColourPaintBox;
	TSpinEdit *RedSpinEdit;
	TLabel *Label1;
	TLabel *Label2;
	TSpinEdit *GreenSpinEdit;
	TLabel *Label3;
	TSpinEdit *BlueSpinEdit;
	TEdit *HexEdit;
	TLabel *Label4;
	TLabel *ColourNameLabel;
	TButton *CancelButton;
	TButton *OKButton;
	TButton *RestoreButton;
	TPaintBox *LuminosityArrowPaintBox;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall SpectrumPaintBoxPaint(TObject *Sender);
	void __fastcall LuminosityPaintBoxPaint(TObject *Sender);
	void __fastcall ColourPaintBoxPaint(TObject *Sender);
	void __fastcall EditChange(TObject *Sender);
	void __fastcall SpectrumPaintBoxMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall LuminosityArrowPaintBoxPaint(TObject *Sender);
	void __fastcall SpectrumPaintBoxMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall LuminosityPaintBoxMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall LuminosityPaintBoxMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall RestoreButtonClick(TObject *Sender);

private:	// User declarations

	Graphics::TBitmap *m_spectrum_base_bm;
	Graphics::TBitmap *m_spectrum_bm;

	Graphics::TBitmap *m_luminosity_bm;
	Graphics::TBitmap *m_luminosity_arrow_bm;

	int m_mouse_x;
	int m_mouse_y;

	TForm *m_sender_form;

	TColor m_colour;

	float m_hue;
	float m_sat;
	float m_lum;

	t_colour_picker_on_colour_updated m_on_colour_updated;
	t_colour_picker_on_close m_on_close;

	void __fastcall WMWindowPosChanging(TWMWindowPosChanging &msg);

	void __fastcall showColour();

protected:
	#pragma option push -vi-
		BEGIN_MESSAGE_MAP
			VCL_MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosMsg, WMWindowPosChanging);
		END_MESSAGE_MAP(TForm)
	#pragma option pop

public:		// User declarations
	__fastcall TColourPickerForm(TComponent* Owner);

	void __fastcall show(TObject *Sender, int x, int y, TColor colour, String colour_name);

	__property t_colour_picker_on_colour_updated onColourUpdated = {read = m_on_colour_updated, write = m_on_colour_updated};
	__property t_colour_picker_on_close onClose                  = {read = m_on_close,          write = m_on_close};
};

extern PACKAGE TColourPickerForm *ColourPickerForm;

#endif
