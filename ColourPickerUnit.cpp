
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#include <vcl.h>

#pragma hdrstop

#include "ColourPickerUnit.h"
#include "Settings.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

// *******************************

const int ARROW_WIDTH            = 10;		// slider arrow
const int ARROW_HEIGHT           = 17;		// slider arrow

const int SLIDER_OFFSET          = 3;
const int SLIDER_WIDTH           = 10;
const int ARROW_DELTA            = 5;		// delta for arrow keys
const int CROSSHAIR_SIZE         = 19;		// spectrum crosshair:
														//    height = width

// for each byte in this array:
//     0 = skip
//     1 = COLOR_WINDOWTEXT
//     2 = COLOR_WINDOWTEXT (COLOR_BTNSHADOW if bHasFocus == FALSE)
const uint8_t arrow[ARROW_HEIGHT][ARROW_WIDTH] =
{
	0,0,0,0,0,0,0,0,1,0,	// 1
	0,0,0,0,0,0,0,1,1,0,	// 2
	0,0,0,0,0,0,1,2,1,0,	// 3
	0,0,0,0,0,1,2,2,1,0,	// 4
	0,0,0,0,1,2,2,2,1,0,	// 5
	0,0,0,1,2,2,2,2,1,0,	// 6
	0,0,1,2,2,2,2,2,1,0,	// 7
	0,1,2,2,2,2,2,2,1,0,	// 8
	1,2,2,2,2,2,2,2,1,0,	// 9
	0,1,2,2,2,2,2,2,1,0,	// 10
	0,0,1,2,2,2,2,2,1,0,	// 11
	0,0,0,1,2,2,2,2,1,0,	// 12
	0,0,0,0,1,2,2,2,1,0,	// 13
	0,0,0,0,0,1,2,2,1,0,	// 14
	0,0,0,0,0,0,1,2,1,0,	// 15
	0,0,0,0,0,0,0,1,1,0,	// 16
	0,0,0,0,0,0,0,0,1,0 	// 17
};

// for each byte in this array:
//     0 = skip
//     1 = COLOR_WINDOWTEXT (COLOR_WINDOW if bHasFocus == FALSE)
const uint8_t crossHair[CROSSHAIR_SIZE][CROSSHAIR_SIZE] =
{
	0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 1
	0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 2
	0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 3
	0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 4
	0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 5
	0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,	// 6
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 7
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 8
	1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,	// 9
	1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,	// 10
	1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,	// 11
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 12
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 13
	0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,	// 14
	0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 15
	0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 16
	0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 17
	0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 18
	0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0	// 19
};

// *******************************

TColourPickerForm *ColourPickerForm = NULL;

float __fastcall HueToRGB(float m1, float m2, float h)
{
	if (h < 0) h += 1;
	if (h > 1) h -= 1;
	if ((6 * h) < 1)
		return (m1 + (m2 - m1) * h * 6.0);
	if ((2 * h) < 1)
		return m2;
	if ((3 * h) < 2)
		return (m1 + (m2 - m1) * ((2.0 / 3) - h) * 6);
	return m1;
}

void __fastcall RGBToHSL(uint32_t cr, float *H, float *S, float *L)
{
	const float r = (float)((cr >>  0) & 0xff) / 255;
	const float g = (float)((cr >>  8) & 0xff) / 255;
	const float b = (float)((cr >> 16) & 0xff) / 255;

	const float cmax = MAX(r, MAX(g, b));
	const float cmin = MIN(r, MIN(g, b));

	*L = (cmax + cmin) / 2;

	const float range = cmax - cmin;

	if (range <= 0)
	{
		*S = 0;
		*H = 0; // it's really undefined
	}
	else
	{
		if (*L < 0.5)
			*S = range / (cmax + cmin);
		else
			*S = range / (2.0 - cmax - cmin);

		const float delta = range;

		if (r == cmax)
			*H = (g - b) / delta;
		else
		if (g == cmax)
			*H = 2.0 + (b - r) / delta;
		else
			*H = 4.0 + (r - g) / delta;

		*H /= 6;

		if (*H < 0)
			*H += 1;
	}

	*H *= 240;
	*S *= 240;
	*L *= 240;
}

void __fastcall RGBToHSL(uint32_t cr, uint8_t *h, uint8_t *s, uint8_t *l)
{
	float H, S, L;

	RGBToHSL(cr, &H, &S, &L);

	*h = (uint8_t)IROUND(H);
	*s = (uint8_t)IROUND(S);
	*l = (uint8_t)IROUND(L);

	if (*h > 239)
		*h = 239;
	if (*s > 240)
		*s = 240;
	if (*l > 240)
		*l = 240;
}

uint32_t __fastcall HSLToRGB(float H, float S, float L, uint8_t *red = NULL, uint8_t *grn = NULL, uint8_t *blu = NULL)
{
	float r;
	float g;
	float b;

	H /= 240;
	S /= 240;
	L /= 240;

	if (S == 0)
	{
		r = L;
		g = L;
		b = L;
	}
	else
	{
		float m1;
		float m2;

		if (L <= 0.5)
			m2 = L * (1 + S);
		else
			m2 = L + S - (L * S);

		m1 = (2 * L) - m2;
		r = HueToRGB(m1, m2, H + (1.0 / 3));
		g = HueToRGB(m1, m2, H);
		b = HueToRGB(m1, m2, H - (1.0 / 3));
	}

	const int _r = IROUND(r * 255);
	const int _g = IROUND(g * 255);
	const int _b = IROUND(b * 255);

	if (red) *red = _r;
	if (grn) *grn = _g;
	if (blu) *blu = _b;

	return RGB(_r, _g, _b);
}

__fastcall TColourPickerForm::TColourPickerForm(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall TColourPickerForm::FormCreate(TObject *Sender)
{
	AnsiString s;
	TNotifyEvent ne;

	Caption = Application->Title + " " + this->Caption;

	// stop flicker
	this->ControlStyle                    = this->ControlStyle << csOpaque;
	SpectrumPanel->ControlStyle           = SpectrumPanel->ControlStyle << csOpaque;
	SpectrumPaintBox->ControlStyle        = SpectrumPaintBox->ControlStyle << csOpaque;
	LuminosityPanel->ControlStyle         = LuminosityPanel->ControlStyle << csOpaque;
	LuminosityPaintBox->ControlStyle      = LuminosityPaintBox->ControlStyle << csOpaque;
	LuminosityArrowPaintBox->ControlStyle = LuminosityArrowPaintBox->ControlStyle << csOpaque;
	ColourPanel->ControlStyle             = ColourPanel->ControlStyle << csOpaque;
	ColourPaintBox->ControlStyle          = ColourPaintBox->ControlStyle << csOpaque;

	ColourNameLabel->Caption = "";

	m_sender_form = NULL;
	m_on_colour_updated = NULL;
	m_on_close = NULL;

	m_mouse_x = -1;
	m_mouse_y = -1;

	m_hue = 0;
	m_sat = 0;
	m_lum = 0;

	m_spectrum_base_bm = new Graphics::TBitmap();
	if (m_spectrum_base_bm)
	{
		m_spectrum_base_bm->Monochrome   = false;
		m_spectrum_base_bm->Transparent  = false;
		m_spectrum_base_bm->PixelFormat  = pf32bit;
	}

	m_spectrum_bm = new Graphics::TBitmap();
	if (m_spectrum_bm)
	{
		m_spectrum_bm->Monochrome   = false;
		m_spectrum_bm->Transparent  = false;
		m_spectrum_bm->PixelFormat  = pf32bit;
	}

	m_luminosity_bm = new Graphics::TBitmap();
	if (m_luminosity_bm)
	{
		m_luminosity_bm->Monochrome   = false;
		m_luminosity_bm->Transparent  = false;
		m_luminosity_bm->PixelFormat  = pf32bit;
	}

	m_luminosity_arrow_bm = new Graphics::TBitmap();
	if (m_luminosity_arrow_bm)
	{
		m_luminosity_arrow_bm->Monochrome   = false;
		m_luminosity_arrow_bm->Transparent  = false;
		m_luminosity_arrow_bm->PixelFormat  = pf32bit;
	}

	// move to the saved position
//	this->Top    = settings.settingsWindowPos.top;
//	this->Left   = settings.settingsWindowPos.left;
//	this->Width  = settings.settingsWindowPos.width;
//	this->Height = settings.settingsWindowPos.height;
}

void __fastcall TColourPickerForm::FormDestroy(TObject *Sender)
{
	m_sender_form = NULL;
	m_on_colour_updated = NULL;
	m_on_close = NULL;

	if (m_luminosity_arrow_bm)
		delete m_luminosity_arrow_bm;
	m_luminosity_arrow_bm = NULL;

	if (m_luminosity_bm)
		delete m_luminosity_bm;
	m_luminosity_bm = NULL;

	if (m_spectrum_bm)
		delete m_spectrum_bm;
	m_spectrum_bm = NULL;

	if (m_spectrum_base_bm)
		delete m_spectrum_base_bm;
	m_spectrum_base_bm = NULL;
}

void __fastcall TColourPickerForm::WMWindowPosChanging(TWMWindowPosChanging &msg)
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

void __fastcall TColourPickerForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	ColourNameLabel->Caption = "";

	if (m_on_close)
		m_on_close(this, COLOUR_PICKER_NO_BUT, m_colour);
}

void __fastcall TColourPickerForm::FormHide(TObject *Sender)
{
	ColourNameLabel->Caption = "";

	if (m_on_close)
		m_on_close(this, COLOUR_PICKER_NO_BUT, m_colour);
}

void __fastcall TColourPickerForm::show(TObject *Sender, int x, int y, TColor colour, String colour_name)
{
	m_sender_form = dynamic_cast<TForm *>(Sender);

	RGBToHSL(Graphics::ColorToRGB(colour), &m_hue, &m_sat, &m_lum);

	ColourNameLabel->Caption = colour_name;

	m_colour = colour;

	if (m_sender_form == NULL)
	{
		Hide();
		return;
	}

	showColour();

	if (x >= 0 && y >= 0)
	{
		this->Left = x;
		this->Top  = y;
	}

	this->Show();
	this->BringToFront();
//	this->Update();
}

void __fastcall TColourPickerForm::showColour()
{
	uint8_t red;
	uint8_t grn;
	uint8_t blu;
	const uint32_t colour = HSLToRGB(m_hue, m_sat, m_lum, &red, &grn, &blu);

	const TNotifyEvent ne   = HexEdit->OnChange;
	const TNotifyEvent ne_r = RedSpinEdit->OnChange;
	const TNotifyEvent ne_g = GreenSpinEdit->OnChange;
	const TNotifyEvent ne_b = BlueSpinEdit->OnChange;

	HexEdit->OnChange       = NULL;
	RedSpinEdit->OnChange   = NULL;
	GreenSpinEdit->OnChange = NULL;
	BlueSpinEdit->OnChange  = NULL;

	{	// only set the hex edit text if it needs it
		String s = HexEdit->Text.Trim();
		while (!s.IsEmpty())
		{
			if (s[1] != '#')
				break;
			s = s.SubString(2, s.Length() - 1).Trim();
		}
		if (s.LowerCase().Pos("0x") <= 0)
			s = "0x" + s;
		uint32_t hex;
		if (!TryStrToUInt(s, hex) || hex != colour)
			HexEdit->Text = IntToHex((int)colour, 6);
	}

	RedSpinEdit->Value      = red;
	GreenSpinEdit->Value    = grn;
	BlueSpinEdit->Value     = blu;

	HexEdit->OnChange       = ne;
	RedSpinEdit->OnChange   = ne_r;
	GreenSpinEdit->OnChange = ne_g;
	BlueSpinEdit->OnChange  = ne_b;

	HexEdit->Refresh();
	RedSpinEdit->Refresh();
	GreenSpinEdit->Refresh();
	BlueSpinEdit->Refresh();

	SpectrumPaintBox->Refresh();
	LuminosityPaintBox->Refresh();
	LuminosityArrowPaintBox->Refresh();
	ColourPaintBox->Refresh();
}

void __fastcall TColourPickerForm::SpectrumPaintBoxPaint(TObject *Sender)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	TColor back_colour = TStyleManager::ActiveStyle->GetStyleColor(scWindow);

	if (m_spectrum_base_bm == NULL || m_spectrum_bm == NULL)
	{
		pb->Canvas->Brush->Color =  back_colour;
		pb->Canvas->Brush->Style = bsSolid;
		pb->Canvas->FillRect(pb->Canvas->ClipRect);
		return;
	}

	bool redraw = false;

	if (m_spectrum_base_bm->Width != pb->Width || m_spectrum_base_bm->Height != pb->Height || m_spectrum_bm->Width != pb->Width || m_spectrum_bm->Height != pb->Height)
	{
		m_spectrum_base_bm->Width  = pb->Width;
		m_spectrum_base_bm->Height = pb->Height;
		m_spectrum_bm->Width  = pb->Width;
		m_spectrum_bm->Height = pb->Height;
		redraw = true;
	}

	if (m_spectrum_base_bm->Width != pb->Width || m_spectrum_base_bm->Height != pb->Height || m_spectrum_bm->Width != pb->Width || m_spectrum_bm->Height != pb->Height)
	{
		pb->Canvas->Brush->Color =  back_colour;
		pb->Canvas->Brush->Style = bsSolid;
		pb->Canvas->FillRect(pb->Canvas->ClipRect);
		return;
	}

	if (redraw)
	{
		m_spectrum_base_bm->Canvas->Font = (settings.graphFont != NULL) ? settings.graphFont : pb->Canvas->Font;
		m_spectrum_bm->Canvas->Font = (settings.graphFont != NULL) ? settings.graphFont : pb->Canvas->Font;

		// background colour
		m_spectrum_base_bm->Canvas->Brush->Color =  back_colour;
		m_spectrum_base_bm->Canvas->Brush->Style = bsSolid;
		m_spectrum_base_bm->Canvas->FillRect(m_spectrum_base_bm->Canvas->ClipRect);

		const int width  = m_spectrum_base_bm->Width;
		const int height = m_spectrum_base_bm->Height;

		// saturation and luminosity values must be 0 to 240
		// hue value must be 0 to 239.

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				const float hue = (float)(x * 240) / (width - 1);	// 0 to 240
				const float sat = (float)(y * 240) / (height - 1);  // 0 to 240

				uint8_t red;
				uint8_t grn;
				uint8_t blu;
				HSLToRGB(hue, sat, 120, &red, &grn, &blu);

				uint8_t *p = (uint8_t *)m_spectrum_base_bm->ScanLine[height - y - 1] + (x * 4);
				p[0] = blu;
				p[1] = grn;
				p[2] = red;
				p[3] = 255;
//				m_dcSpectrum.SetPixelV(x, cy - y - 1 + VERTICAL_MARGIN, cr);
			}
		}
	}

//	pb->Canvas->CopyMode = cmSrcCopy;
	m_spectrum_bm->Canvas->Draw(0, 0, m_spectrum_base_bm);

	{	// draw the cross hair
		int ch_x = IROUND((m_hue * m_spectrum_base_bm->Width) / 239);
		if (ch_x < 0)
			ch_x = 0;
		else
		if (ch_x > (m_spectrum_base_bm->Width - 1))
			ch_x = m_spectrum_base_bm->Width - 1;

		int ch_y = m_spectrum_base_bm->Height - 1 - IROUND((m_sat * m_spectrum_base_bm->Height) / 240);
		if (ch_y < 0)
			ch_y = 0;
		else
		if (ch_y > (m_spectrum_base_bm->Height - 1))
			ch_y = m_spectrum_base_bm->Height - 1;

		if (ch_x >= 0 && ch_x < m_spectrum_bm->Width && ch_y >= 0 && ch_y < m_spectrum_bm->Height)
		{
			const uint8_t red = 0;
			const uint8_t grn = 0;
			const uint8_t blu = 0;
			for (int row = 0; row < CROSSHAIR_SIZE; row++)
			{
				for (int col = 0; col < CROSSHAIR_SIZE; col++)
				{
					if (crossHair[row][col] == 0)
						continue;
					const int x = ch_x - (CROSSHAIR_SIZE / 2) + col;
					const int y = ch_y - (CROSSHAIR_SIZE / 2) + row;
					if (x >= 0 && x < m_spectrum_bm->Width && y >= 0 && y < m_spectrum_bm->Height)
					{
						uint8_t *p = (uint8_t *)m_spectrum_bm->ScanLine[y] + (x * 4);
						p[0] = blu;
						p[1] = grn;
						p[2] = red;
						p[3] = 255;
					}
				}
			}
		}
	}

	//	pb->Canvas->CopyMode = cmSrcCopy;
	pb->Canvas->Draw(0, 0, m_spectrum_bm);
}

void __fastcall TColourPickerForm::LuminosityPaintBoxPaint(TObject *Sender)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	TColor back_colour = TStyleManager::ActiveStyle->GetStyleColor(scWindow);

	if (m_luminosity_bm == NULL)
	{
		pb->Canvas->Brush->Color = back_colour;
		pb->Canvas->Brush->Style = bsSolid;
		pb->Canvas->FillRect(pb->Canvas->ClipRect);
		return;
	}

	if (m_luminosity_bm->Width != pb->Width || m_luminosity_bm->Height != pb->Height)
	{
		m_luminosity_bm->Width  = pb->Width;
		m_luminosity_bm->Height = pb->Height;
	}

	if (m_luminosity_bm->Width <= 0 || m_luminosity_bm->Height <= 0)
	{
		pb->Canvas->Brush->Color = back_colour;
		pb->Canvas->Brush->Style = bsSolid;
		pb->Canvas->FillRect(pb->Canvas->ClipRect);
		return;
	}

	m_luminosity_bm->Canvas->Font = (settings.graphFont != NULL) ? settings.graphFont : pb->Canvas->Font;

	// background colour
	m_luminosity_bm->Canvas->Brush->Color = back_colour;
	m_luminosity_bm->Canvas->Brush->Style = bsSolid;
	m_luminosity_bm->Canvas->FillRect(m_luminosity_bm->Canvas->ClipRect);

	m_luminosity_bm->Canvas->Pen->Width = 1;
	m_luminosity_bm->Canvas->Pen->Style = psSolid;

	const int cx = m_luminosity_bm->Width;
	const int cy = m_luminosity_bm->Height;

	for (int y = 0; y < cy; y++)
	{
		const float lum = (float)(y * 240) / (cy - 1);	// 0 to 240
		const uint32_t colour = HSLToRGB(m_hue, m_sat, lum);
		m_luminosity_bm->Canvas->Pen->Color = TColor(colour);
		m_luminosity_bm->Canvas->MoveTo( 0, m_luminosity_bm->Height - 1 - y);
		m_luminosity_bm->Canvas->LineTo(cx, m_luminosity_bm->Height - 1 - y);
	}

//	pb->Canvas->CopyMode = cmSrcCopy;
	pb->Canvas->Draw(0, 0, m_luminosity_bm);

	LuminosityArrowPaintBox->Invalidate();
}

void __fastcall TColourPickerForm::ColourPaintBoxPaint(TObject *Sender)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

//	TColor back_colour = TStyleManager::ActiveStyle->GetStyleColor(scWindow);

	TRect rect1 = pb->Canvas->ClipRect;
	TRect rect2 = pb->Canvas->ClipRect;
	rect1.bottom = (rect1.Height() / 2);
	rect2.top    = rect1.bottom;

	// new colour
	pb->Canvas->Brush->Color = TColor(HSLToRGB(m_hue, m_sat, m_lum));
	pb->Canvas->Brush->Style = bsSolid;
	pb->Canvas->FillRect(rect1);

	// old colour
	pb->Canvas->Brush->Color = m_colour;
	pb->Canvas->Brush->Style = bsSolid;
	pb->Canvas->FillRect(rect2);
}

void __fastcall TColourPickerForm::EditChange(TObject *Sender)
{
	TSpinEdit *se = dynamic_cast<TSpinEdit *>(Sender);
	TEdit *edit = dynamic_cast<TEdit *>(Sender);

	if (m_sender_form == NULL)
		return;

	uint8_t red;
	uint8_t grn;
	uint8_t blu;
	HSLToRGB(m_hue, m_sat, m_lum, &red, &grn, &blu);

	if (se == RedSpinEdit)   red = se->Value;
	else
	if (se == GreenSpinEdit) grn = se->Value;
	else
	if (se == BlueSpinEdit)  blu = se->Value;
	else
	if (edit == HexEdit)
	{
		String s = edit->Text.Trim();
		while (!s.IsEmpty())
		{
			if (s[1] != '#')
				break;
			s = s.SubString(2, s.Length() - 1).Trim();
		}
		if (s.LowerCase().Pos("0x") <= 0)
			s = "0x" + s;
		uint32_t hex;
		if (!TryStrToUInt(s, hex))
			return;
		red = (hex >>  0) & 0xff;
		grn = (hex >>  8) & 0xff;
		blu = (hex >> 16) & 0xff;
	}
	else
		return;

	const uint32_t colour = RGB(red, grn, blu);

	RGBToHSL(colour, &m_hue, &m_sat, &m_lum);

	showColour();

	if (m_on_colour_updated)
		m_on_colour_updated(this, TColor(colour));
}

void __fastcall TColourPickerForm::SpectrumPaintBoxMouseMove(TObject *Sender,
			 TShiftState Shift, int X, int Y)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	if (Shift.Contains(ssLeft))
	{
		if (m_spectrum_base_bm)
		{
			if (m_spectrum_base_bm->Width > 0 && m_spectrum_base_bm->Height > 0)
			{
				if (X < 0) X = 0;
				else
				if (X > (m_spectrum_base_bm->Width - 1)) X = m_spectrum_base_bm->Width - 1;

				if (Y < 0) Y = 0;
				else
				if (Y > (m_spectrum_base_bm->Height - 1)) Y = m_spectrum_base_bm->Height - 1;

				m_hue = (float)(X * 239) / (pb->Width - 1);
				m_sat = (float)((pb->Height - Y - 1) * 240) / (pb->Height - 1);
				if (m_hue > 239)
					m_hue = 239;
				if (m_sat > 240)
					m_sat = 240;

				const uint32_t colour = HSLToRGB(m_hue, m_sat, m_lum);

				showColour();

				if (m_on_colour_updated)
					m_on_colour_updated(this, TColor(colour));
			}
		}
	}
}

void __fastcall TColourPickerForm::LuminosityArrowPaintBoxPaint(TObject *Sender)

{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	TColor back_colour = TStyleManager::ActiveStyle->GetStyleColor(scWindow);

	if (m_luminosity_arrow_bm == NULL)
	{
		pb->Canvas->Brush->Color = back_colour;
		pb->Canvas->Brush->Style = bsSolid;
		pb->Canvas->FillRect(pb->Canvas->ClipRect);
		return;
	}

	if (m_luminosity_arrow_bm->Width != pb->Width || m_luminosity_arrow_bm->Height != pb->Height)
	{
		m_luminosity_arrow_bm->Width  = pb->Width;
		m_luminosity_arrow_bm->Height = pb->Height;
	}

	m_luminosity_arrow_bm->Canvas->Font = (settings.graphFont != NULL) ? settings.graphFont : pb->Canvas->Font;

	// background colour
	m_luminosity_arrow_bm->Canvas->Brush->Color = back_colour;
	m_luminosity_arrow_bm->Canvas->Brush->Style = bsSolid;
	m_luminosity_arrow_bm->Canvas->FillRect(m_luminosity_arrow_bm->Canvas->ClipRect);

	const int y1 = LuminosityPanel->Top + LuminosityPaintBox->ClientToParent(TPoint(LuminosityPaintBox->Left, LuminosityPaintBox->Top)).Y - pb->Top - 1;
	const int y2 = y1 + LuminosityPaintBox->Height - 1;

	#if 0
		m_luminosity_arrow_bm->Canvas->Pen->Color = clWhite;
		m_luminosity_arrow_bm->Canvas->MoveTo(0, y1);
		m_luminosity_arrow_bm->Canvas->LineTo(m_luminosity_arrow_bm->Width, y1);
		m_luminosity_arrow_bm->Canvas->MoveTo(0, y2);
		m_luminosity_arrow_bm->Canvas->LineTo(m_luminosity_arrow_bm->Width, y2);
	#endif

	const int x = SLIDER_OFFSET;
	//int y = m_luminosity_arrow_bm->Height - IROUND((float)(m_luminosity_arrow_bm->Height * m_lum) / 240);
	int y = y2 - IROUND((float)(m_luminosity_bm->Height * m_lum) / 240);
	if (y < 0) y = 0;
	else
	if (y > (m_luminosity_arrow_bm->Height - 1)) y = m_luminosity_arrow_bm->Height - 1;

	for (int row = 0; row < ARROW_HEIGHT; row++)
	{
		for (int col = 0; col < ARROW_WIDTH; col++)
		{
			const int xx = x + col;
			const int yy = y + row - (ARROW_HEIGHT / 2);

			if (xx >= 0 & xx < m_luminosity_arrow_bm->Width && yy >= 0 & yy < m_luminosity_arrow_bm->Height)
			{
				if (arrow[row][col] == 0)
					continue;

				TColor colour = clRed;
				if (arrow[row][col] == 1)
					colour = clWhite;
				else
				if (arrow[row][col] == 2)
					colour = clBlack;

				const uint32_t c = Graphics::ColorToRGB(colour);
				const uint8_t red = (c >>  0) & 0xff;
				const uint8_t grn = (c >>  8) & 0xff;
				const uint8_t blu = (c >> 16) & 0xff;

				// set a pixel
				uint8_t *p  = (uint8_t *)m_luminosity_arrow_bm->ScanLine[yy] + (xx * 4);
				p[0] = blu;
				p[1] = grn;
				p[2] = red;
				p[3] = 255;
			}
		}
	}

//	pb->Canvas->CopyMode = cmSrcCopy;
	pb->Canvas->Draw(0, 0, m_luminosity_arrow_bm);
}

void __fastcall TColourPickerForm::SpectrumPaintBoxMouseDown(TObject *Sender,
			 TMouseButton Button, TShiftState Shift, int X, int Y)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	if (Shift.Contains(ssLeft))
	{
		if (m_spectrum_base_bm)
		{
			if (m_spectrum_base_bm->Width > 0 && m_spectrum_base_bm->Height > 0)
			{
				if (X < 0) X = 0;
				else
				if (X > (m_spectrum_base_bm->Width - 1)) X = m_spectrum_base_bm->Width - 1;

				if (Y < 0) Y = 0;
				else
				if (Y > (m_spectrum_base_bm->Height - 1)) Y = m_spectrum_base_bm->Height - 1;

				m_hue = (float)(X * 239) / (pb->Width - 1);
				m_sat = (float)((pb->Height - Y - 1) * 240) / (pb->Height - 1);
				if (m_hue > 239)
					m_hue = 239;
				if (m_sat > 240)
					m_sat = 240;

				const uint32_t colour = HSLToRGB(m_hue, m_sat, m_lum);

				showColour();

				if (m_on_colour_updated)
					m_on_colour_updated(this, TColor(colour));
			}
		}
	}
}

void __fastcall TColourPickerForm::LuminosityPaintBoxMouseDown(TObject *Sender,
			 TMouseButton Button, TShiftState Shift, int X, int Y)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	if (Shift.Contains(ssLeft))
	{
		if (pb->Width > 0 && pb->Height > 0)
		{
			if (X < 0) X = 0;
			else
			if (X > (pb->Width  - 1)) X = pb->Width - 1;

			if (Y < 0) Y = 0;
			else
			if (Y > (pb->Height - 1)) Y = pb->Height - 1;

			if (pb == LuminosityArrowPaintBox)
			{
				const int y1 = LuminosityPanel->Top + LuminosityPaintBox->ClientToParent(TPoint(LuminosityPaintBox->Left, LuminosityPaintBox->Top)).Y - pb->Top - 1;
				const int y2 = y1 + LuminosityPanel->Height - 1;
				m_lum = (float)((y2 - 1 - Y) * 240) / (y2 - y1);
			}
			else
				m_lum = (float)((pb->Height - 1 - Y) * 240) / pb->Height;

			if (m_lum < 0) m_lum = 0;
			else
			if (m_lum > 240) m_lum = 240;

			TColor colour = TColor(HSLToRGB(m_hue, m_sat, m_lum));

			showColour();

			if (m_on_colour_updated)
				m_on_colour_updated(this, colour);
		}
	}
}

void __fastcall TColourPickerForm::LuminosityPaintBoxMouseMove(TObject *Sender,
			 TShiftState Shift, int X, int Y)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	if (Shift.Contains(ssLeft))
	{
		if (pb->Width > 0 && pb->Height > 0)
		{
			if (X < 0) X = 0;
			else
			if (X > (pb->Width  - 1)) X = pb->Width - 1;

			if (Y < 0) Y = 0;
			else
			if (Y > (pb->Height - 1)) Y = pb->Height - 1;

			if (pb == LuminosityArrowPaintBox)
			{
				const int y1 = LuminosityPanel->Top + LuminosityPaintBox->ClientToParent(TPoint(LuminosityPaintBox->Left, LuminosityPaintBox->Top)).Y - pb->Top - 1;
				const int y2 = y1 + LuminosityPanel->Height - 1;
				m_lum = (float)((y2 - 1 - Y) * 240) / (y2 - y1);
			}
			else
				m_lum = (float)((pb->Height - 1 - Y) * 240) / pb->Height;

			if (m_lum < 0) m_lum = 0;
			else
			if (m_lum > 240) m_lum = 240;

			TColor colour = TColor(HSLToRGB(m_hue, m_sat, m_lum));

			showColour();

			if (m_on_colour_updated)
				m_on_colour_updated(this, colour);
		}
	}
}

void __fastcall TColourPickerForm::CancelButtonClick(TObject *Sender)
{
	if (m_on_colour_updated)
		m_on_colour_updated(this, m_colour);

	Hide();

	if (m_on_close)
		m_on_close(this, COLOUR_PICKER_CANCEL_BUT, m_colour);
}

void __fastcall TColourPickerForm::OKButtonClick(TObject *Sender)
{
	if (m_on_close)
		m_on_close(this, COLOUR_PICKER_OK_BUT, TColor(HSLToRGB(m_hue, m_sat, m_lum)));

	Hide();
}

void __fastcall TColourPickerForm::RestoreButtonClick(TObject *Sender)
{
	RGBToHSL(Graphics::ColorToRGB(m_colour), &m_hue, &m_sat, &m_lum);

	showColour();

	if (m_on_colour_updated)
		m_on_colour_updated(this, m_colour);
}

