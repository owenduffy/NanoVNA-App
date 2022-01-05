
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#include <vcl.h>
#include <Vcl.Themes.hpp>
#include <Vcl.FileCtrl.hpp>
#include <mmsystem.h>		// PlaySound()

#pragma hdrstop

#include "SettingsUnit.h"
#include "DataUnit.H"
#include "Graphs.h"
#include "Unit1.h"
#include "ColourPickerUnit.h"
#include "common.h"
#include "Settings.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TSettingsForm *SettingsForm = NULL;

__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall TSettingsForm::FormCreate(TObject *Sender)
{
	AnsiString s;
	TNotifyEvent ne;

	m_initialised = false;

	Caption = Application->Title + " " + this->Caption;

	// stop flicker
//	this->ControlStyle                = this->ControlStyle << csOpaque;
	Memo1->ControlStyle               = Memo1->ControlStyle << csOpaque;
	ColourBevel->ControlStyle         = ColourBevel->ControlStyle << csOpaque;
//	ColoursGroupBox->ControlStyle     = ColoursGroupBox->ControlStyle << csOpaque;
//	FreqBandGroupBox->ControlStyle    = FreqBandGroupBox->ControlStyle << csOpaque;
	GraphColourPaintBox->ControlStyle = GraphColourPaintBox->ControlStyle << csOpaque;

	m_mouse_x = -1;
	m_mouse_y = -1;
	m_colour_picker_index = -1;

	Memo1->Clear();

	FreqBandNameEdit->Text    = "";
	FreqBandLowMHzEdit->Text  = "";
	FreqBandHighMHzEdit->Text = "";

	ThresholdEdit->Text  = "";

	ne = RecordPathEdit->OnChange;
	RecordPathEdit->OnChange = NULL;
	RecordPathEdit->Text = settings.recordFolder;
	RecordPathEdit->OnChange = ne;

	FreqBandDeleteButton->Enabled = (FreqBandListBox->SelCount > 0) ? true : false;
	SetScanButton->Enabled        = (FreqBandListBox->SelCount > 0) ? true : false;

	m_colour_picker_colour = NULL;

//	ThemeServices->OnThemeChange = onThemeChange;

	{
		TComboBox *cb = GUIStyleComboBox;

		cb->Items->BeginUpdate();

		ne = cb->OnChange;
		cb->OnChange = NULL;

		for (int i = 0; i < TStyleManager::StyleNames.Length; i++)
			cb->Items->Add(TStyleManager::StyleNames[i]);

		cb->ItemIndex = cb->Items->IndexOf(TStyleManager::ActiveStyle->Name);

		cb->OnChange = ne;

		cb->Items->EndUpdate();

		common.comboBoxAutoWidth(cb);
	}

	{
		TComboBox *cb = FreqBandPadComboBox;

		cb->Items->BeginUpdate();

		ne = cb->OnChange;
		cb->OnChange = NULL;

		cb->Clear();
		cb->AddItem(  "0 %", (TObject *)0);
		cb->AddItem( "10 %", (TObject *)10);
		cb->AddItem( "25 %", (TObject *)25);
		cb->AddItem( "50 %", (TObject *)50);
		cb->AddItem("100 %", (TObject *)100);
		cb->AddItem("200 %", (TObject *)200);

		const int i = cb->Items->IndexOfObject((TObject *)settings.freqBandPaddingPercent);
		cb->ItemIndex = (i >= 0) ? i : 0;

		cb->OnChange = ne;

		cb->Items->EndUpdate();

		common.comboBoxAutoWidth(cb);
	}

//	t_freq_band fb;
//	FreqBandAddButton->Enabled = fetchFreqBandInput(fb);
	FreqBandAddButton->Enabled = true;

	{
		const TNotifyEvent ne = LineAlphaTrackBar->OnChange;
		LineAlphaTrackBar->OnChange = NULL;
		LineAlphaTrackBar->Position = settings.lineAlpha;
		LineAlphaTrackBar->OnChange = ne;

		String s;
		s.printf(L" Line alpha (line transparency) %d ", settings.lineAlpha);
		LineAlphaTrackBar->Hint = s;
	}

	LineWidthTrackBar->Position = settings.lineWidth;
	LineWidthTrackBarChange(LineWidthTrackBar);

	BorderWidthTrackBar->Position = settings.borderWidth;
	BorderWidthTrackBarChange(BorderWidthTrackBar);

	m_colour_bm = new Graphics::TBitmap();
	if (m_colour_bm)
	{
		m_colour_bm->Monochrome   = false;
		m_colour_bm->Transparent  = false;
		m_colour_bm->PixelFormat  = pf32bit;
	}

//	Application->OnMessage  = OnMessage;

	// move to the saved position
	this->Top    = settings.settingsWindowPos.top;
	this->Left   = settings.settingsWindowPos.left;
	this->Width  = settings.settingsWindowPos.width;
	this->Height = settings.settingsWindowPos.height;
}

void __fastcall TSettingsForm::FormDestroy(TObject *Sender)
{
	m_colour_picker_colour = NULL;

	if (m_colour_bm)
		delete m_colour_bm;
	m_colour_bm = NULL;
}

void __fastcall TSettingsForm::OnMessage(tagMSG &msg, bool &handled)
{
	switch (msg.message)
	{
//		case WM_MAP_SHOW:
//			MapSpeedButton->Down = true;
//			handled = true;
//			break;

//		case WM_MAP_HIDE:
//			MapSpeedButton->Down = false;
//			handled = true;
//			break;

		case WM_ERASEBKGND:
			handled = true;
			break;
	}
}

void __fastcall TSettingsForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	//
}

void __fastcall TSettingsForm::WMWindowPosChanging(TWMWindowPosChanging &msg)
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

void __fastcall TSettingsForm::updateInfo()
{
	Memo1->Lines->BeginUpdate();
	Memo1->Clear();
	if (Form1)
	{
		for (unsigned int i = 0; i < data_unit.m_vna_data.info.size(); i++)
			Memo1->Lines->Add(data_unit.m_vna_data.info[i]);
	}
	Memo1->Lines->EndUpdate();
}

void __fastcall TSettingsForm::updateThresholdHz()
{
	TNotifyEvent ne;

	ne = ThresholdEdit->OnChange;
	ThresholdEdit->OnChange = NULL;
	if (Form1)
		ThresholdEdit->Text = common.freqToStrMHz(data_unit.m_vna_data.freq_threshold_Hz);
	else
		ThresholdEdit->Text = "";
	ThresholdEdit->OnChange = ne;
}

void __fastcall TSettingsForm::updateVBatOffset()
{
	AnsiString s;
	TNotifyEvent ne;

	ne = VBatOffsetSpinEdit->OnChange;
	VBatOffsetSpinEdit->OnChange = NULL;
	if (Form1)
		VBatOffsetSpinEdit->Value = data_unit.m_vna_data.vbat_offset_mv;
	else
		VBatOffsetSpinEdit->Value = 0;
	VBatOffsetSpinEdit->OnChange = ne;
}

void __fastcall TSettingsForm::update()
{
	String s;
	TNotifyEvent ne;

	updateInfo();
	updateThresholdHz();
	updateVBatOffset();
	updateFreqBandListBox();

	ne = RecordPathEdit->OnChange;
	RecordPathEdit->OnChange = NULL;
	RecordPathEdit->Text = settings.recordFolder;
	RecordPathEdit->OnChange = ne;

	ne = MarkerFillToggleSwitch->OnClick;
	MarkerFillToggleSwitch->OnClick = NULL;
	MarkerFillToggleSwitch->State   = settings.markerFill ? tssOn : tssOff;
	MarkerFillToggleSwitch->OnClick = ne;

	ne = AutoScalePeakHoldToggleSwitch->OnClick;
	AutoScalePeakHoldToggleSwitch->OnClick = NULL;
	AutoScalePeakHoldToggleSwitch->State   = settings.autoScalePeakHold ? tssOn : tssOff;
	AutoScalePeakHoldToggleSwitch->OnClick = ne;

	ne = SmithBothScalesToggleSwitch->OnClick;
	SmithBothScalesToggleSwitch->OnClick = NULL;
	SmithBothScalesToggleSwitch->State   = settings.smithBothScales ? tssOn : tssOff;
	SmithBothScalesToggleSwitch->OnClick = ne;

	GUIStyleComboBox->ItemIndex = GUIStyleComboBox->Items->IndexOf(TStyleManager::ActiveStyle->Name);

	ne = LineAlphaTrackBar->OnChange;
	LineAlphaTrackBar->OnChange = NULL;
	LineAlphaTrackBar->Position = settings.lineAlpha;
	LineAlphaTrackBar->OnChange = ne;
	s.printf(L" Line alpha (line transparency) %d ", settings.lineAlpha);
	LineAlphaTrackBar->Hint = s;
}

void __fastcall TSettingsForm::show()
{
	update();

	this->Show();
	this->BringToFront();
//	this->Update();
}

void __fastcall TSettingsForm::SetTimeButtonClick(TObject *Sender)
{
	if (Form1)
		Form1->sendTimeCommand();
}

void __fastcall TSettingsForm::ClearConfigButtonClick(TObject *Sender)
{
	if (Form1)
	{
		Application->NormalizeTopMosts();
		int res = Application->MessageBox(L"Are you really sure you want to factory reset your NanoVNA ?", L"Factory reset your NanoVNA ?", MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2);
		Application->RestoreTopMosts();
		if (res == IDYES)
		{
			Application->NormalizeTopMosts();
			res = Application->MessageBox(L"Are you really really sure you want to factory reset your NanoVNA ?", L"Factory reset your NanoVNA ?", MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2);
			Application->RestoreTopMosts();
			if (res == IDYES)
			{
				Form1->stop();
				Form1->addSerialTxCommand("clearconfig 1234");
			}
		}
	}
}

void __fastcall TSettingsForm::SaveConfigButtonClick(TObject *Sender)
{
	if (Form1)
	{
		sendThresholdFreq();
		sendVBATOffset();
		Form1->addSerialTxCommand("saveconfig");
	}
}

void __fastcall TSettingsForm::FormKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	switch (Key)
	{
		case VK_ESCAPE:
			Key = 0;
			this->Hide();
			break;
	}
}

bool __fastcall TSettingsForm::fetchFreqBandInput(t_freq_band &fb)
{
	AnsiString name;
	double low_MHz;
	double high_MHz;

	fb.name    = "";
	fb.low_Hz  = 0;
	fb.high_Hz = 0;
	fb.enabled = true;

	name = FreqBandNameEdit->Text.Trim();
	if (name.IsEmpty())
		return false;

	if (!TryStrToFloat(common.localiseDecimalPoint(FreqBandLowMHzEdit->Text), low_MHz))
		return false;

	if (!TryStrToFloat(common.localiseDecimalPoint(FreqBandHighMHzEdit->Text), high_MHz))
		return false;

	fb.name    = name;
	fb.low_Hz  = I64ROUND(low_MHz  * 1e6);
	fb.high_Hz = I64ROUND(high_MHz * 1e6);
	fb.enabled = (FreqBandEnableToggleSwitch->State == tssOn) ? true : false;

	if (fb.low_Hz == fb.high_Hz)
		return false;

	if (fb.low_Hz > fb.high_Hz)
	{	// swap
		const int64_t Hz = fb.low_Hz;
		fb.low_Hz  = fb.high_Hz;
		fb.high_Hz = Hz;
	}

	return true;
}

void __fastcall TSettingsForm::updateFreqBandListBox()
{
	TListBox *lb = FreqBandListBox;

	t_freq_band fb;
	const bool select = fetchFreqBandInput(fb);

	const int top_index = lb->TopIndex;

	lb->Items->BeginUpdate();
	lb->Clear();

	for (unsigned int i = 0; i < settings.m_freq_band.size(); i++)
	{
		const t_freq_band fb = settings.m_freq_band[i];
		AnsiString s;

		s = fb.name.Trim();
		while (s.Length() < 12)
			s = " " + s;

		s  = " " + s;
		s += "  " + common.freqToStr2(fb.low_Hz, 10) + "Hz";
		s += "  " + common.freqToStr2(fb.high_Hz, 10) + "Hz";
		s += fb.enabled ? "          " : "  hide";

		lb->Items->AddObject(s, (TObject *)i);
	}

	if (top_index >= 0 && !settings.m_freq_band.empty())
		lb->TopIndex = top_index;

	if (select)
	{
		for (int i = lb->Items->Count - 1; i >= 0; i--)
		{
			const int index = (int)lb->Items->Objects[i];
			const t_freq_band fb2 = settings.m_freq_band[index];
			if (fb2.low_Hz == fb.low_Hz && fb2.high_Hz == fb.high_Hz)
			{
				lb->Selected[i] = true;
				break;
			}
		}
	}

	lb->Items->EndUpdate();

	{
		bool selected = false;
		for (int i = lb->Items->Count - 1; i >= 0; i--)
		{
			if (lb->Selected[i])
			{
				selected = true;
				break;
			}
		}
		FreqBandDeleteButton->Enabled = selected;
		SetScanButton->Enabled = selected;
	}
}

void __fastcall TSettingsForm::deleteSelectedFreqBand()
{
	TListBox *lb = FreqBandListBox;

	for (int i = lb->Items->Count - 1; i >= 0; i--)
	{
		if (lb->Selected[i])
		{
			const int index = (int)lb->Items->Objects[i];
			settings.m_freq_band.erase(settings.m_freq_band.begin() + index);	// delete a band
		}
	}

	settings.sortFreqBand();

	updateFreqBandListBox();

	if (Form1)
		::PostMessage(Form1->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TSettingsForm::applyFreqBandSetting(t_freq_band fb)
{
	// check to see if the band already exists
	int index = -1;
	for (int i = 0; i < (int)settings.m_freq_band.size(); i++)
	{
		if (settings.m_freq_band[i].low_Hz == fb.low_Hz && settings.m_freq_band[i].high_Hz == fb.high_Hz)
			index = i;
	}

	if (index >= 0)
	{	// replace the current one
		settings.m_freq_band[index] = fb;

		updateFreqBandListBox();

		if (Form1)
			::PostMessage(Form1->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TSettingsForm::FreqBandListBoxKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	TListBox *lb = dynamic_cast<TListBox *>(Sender);
	if (lb == NULL)
		return;

	if (Key == VK_DELETE)
	{
		Key = 0;
		deleteSelectedFreqBand();
		return;
	}
}

void __fastcall TSettingsForm::FreqBandAddButtonClick(TObject *Sender)
{
	t_freq_band fb;

	if (!fetchFreqBandInput(fb))
		return;

	if (fb.low_Hz == fb.high_Hz)
		return;

	if (fb.low_Hz > fb.high_Hz)
	{	// swap
		const int64_t Hz = fb.low_Hz;
		fb.low_Hz  = fb.high_Hz;
		fb.high_Hz = Hz;
	}

	// check to see if the band already exists
	int index = -1;
	for (int i = 0; i < (int)settings.m_freq_band.size(); i++)
	{
		if (settings.m_freq_band[i].low_Hz == fb.low_Hz && settings.m_freq_band[i].high_Hz == fb.high_Hz)
			index = i;
	}

	if (index >= 0)
	{	// replace the current one
		settings.m_freq_band[index] = fb;
	}
	else
	{	// add a new one
		settings.m_freq_band.push_back(fb);
	}

	settings.sortFreqBand();

	updateFreqBandListBox();

	if (Form1)
		::PostMessage(Form1->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TSettingsForm::FreqBandDeleteButtonClick(TObject *Sender)
{
	deleteSelectedFreqBand();
}

void __fastcall TSettingsForm::FreqBandListBoxClick(TObject *Sender)
{
	TListBox *lb = dynamic_cast<TListBox *>(Sender);
	if (lb == NULL)
		return;

	int sel_count = 0;
	for (int i = lb->Items->Count - 1; i >= 0; i--)
	{
		if (lb->Selected[i])
		{
			if (sel_count++ == 0)
			{	// 1st one
				AnsiString s;
				TNotifyEvent ne;

				const int index = (int)lb->Items->Objects[i];
				const t_freq_band fb = settings.m_freq_band[index];

				ne = FreqBandNameEdit->OnChange;
				FreqBandNameEdit->OnChange = NULL;
				FreqBandNameEdit->Text    = fb.name.Trim();
				FreqBandNameEdit->OnChange = ne;

				ne = FreqBandLowMHzEdit->OnChange;
				FreqBandLowMHzEdit->OnChange = NULL;
				FreqBandLowMHzEdit->Text = common.freqToStrMHz(fb.low_Hz);
				FreqBandLowMHzEdit->OnChange = ne;

				ne = FreqBandHighMHzEdit->OnChange;
				FreqBandHighMHzEdit->OnChange = NULL;
				FreqBandHighMHzEdit->Text = common.freqToStrMHz(fb.high_Hz);
				FreqBandHighMHzEdit->OnChange = ne;

				ne = FreqBandEnableToggleSwitch->OnClick;
				FreqBandEnableToggleSwitch->OnClick = NULL;
				FreqBandEnableToggleSwitch->State = fb.enabled ? tssOn : tssOff;
				FreqBandEnableToggleSwitch->OnClick = ne;
			}
		}
	}

	{
		bool selected = false;
		for (int i = lb->Items->Count - 1; i >= 0; i--)
		{
			if (lb->Selected[i])
			{
				selected = true;
				break;
			}
		}
		FreqBandDeleteButton->Enabled = selected;
		SetScanButton->Enabled = selected;
	}
}

void __fastcall TSettingsForm::FreqBandListBoxDblClick(TObject *Sender)
{
	setScanRange();
}

void __fastcall TSettingsForm::setScanRange()
{
	TListBox *lb = FreqBandListBox;
	if (lb == NULL)
		return;

	for (int i = lb->Items->Count - 1; i >= 0; i--)
	{
		if (!lb->Selected[i])
			continue;

		AnsiString s;

		const int index = (int)lb->Items->Objects[i];

		t_freq_band fb = settings.m_freq_band[index];

		FreqBandNameEdit->Text    = fb.name.Trim();
		FreqBandLowMHzEdit->Text  = common.freqToStrMHz(fb.low_Hz);
		FreqBandHighMHzEdit->Text = common.freqToStrMHz(fb.high_Hz);
		FreqBandEnableToggleSwitch->State = fb.enabled ? tssOn : tssOff;

		const int pad_percent = settings.freqBandPaddingPercent;

		if (Form1)
		{	// set the scan range to the clicked frequency band
			int64_t span_Hz = fb.high_Hz - fb.low_Hz;
			if (span_Hz < 0)
			{
				span_Hz = -span_Hz;
				const int64_t Hz = fb.low_Hz;
				fb.low_Hz = fb.high_Hz;
				fb.high_Hz = Hz;
			}

			const int64_t center_Hz = fb.low_Hz + (span_Hz / 2);

			span_Hz += (span_Hz * pad_percent) / 100;

			Form1->setStartStopHz(center_Hz - (span_Hz / 2), center_Hz + (span_Hz / 2), true);

			return;
		}
	}
}

void __fastcall TSettingsForm::SetScanButtonClick(TObject *Sender)
{
	setScanRange();
}

void __fastcall TSettingsForm::FreqBandDefaultButtonClick(TObject *Sender)
{
	Application->NormalizeTopMosts();
	int res = Application->MessageBox(L"Are you sure you want to restore default frequency bands ?", L"Restore default frequency bands", MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2);
	Application->RestoreTopMosts();
	if (res == IDYES)
	{
		settings.defaultFreqBands();

		updateFreqBandListBox();

		if (Form1)
			::PostMessage(Form1->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TSettingsForm::FreqBandEnableToggleSwitchClick(TObject *Sender)
{
	t_freq_band fb;
	fetchFreqBandInput(fb);
	//FreqBandAddButton->Enabled = fetchFreqBandInput(fb);
	applyFreqBandSetting(fb);
}

String __fastcall TSettingsForm::colourName(const int index)
{
	String name = "";

	switch (index)
	{
		case  0: name = "border";          break;
		case  1: name = "background";      break;
		case  2: name = "grid";            break;
		case  3: name = "mouse line";      break;
		case  4: name = "marker line";     break;
		case  5: name = "marker";          break;
		case  6: name = "marker selected"; break;
		case  7: name = "font";            break;
		case  8: name = "vswr2 line";      break;
		case  9: name = "mouse marker";    break;
		case 10: name = "sweep point";     break;

		case 11: name = "live line 1";     break;
		case 12: name = "live line 2";     break;
		case 13: name = "live line 3";     break;
		case 14: name = "live line 4";     break;

		case 15: name = "mem 1 line 1";    break;
		case 16: name = "mem 1 line 2";    break;
		case 17: name = "mem 1 line 3";    break;
		case 18: name = "mem 1 line 4";    break;

		case 19: name = "mem 2 line 1";    break;
		case 20: name = "mem 2 line 2";    break;
		case 21: name = "mem 2 line 3";    break;
		case 22: name = "mem 2 line 4";    break;

		case 23: name = "mem 3 line 1";    break;
		case 24: name = "mem 3 line 2";    break;
		case 25: name = "mem 3 line 3";    break;
		case 26: name = "mem 3 line 4";    break;

		case 27: name = "mem 4 line 1";    break;
		case 28: name = "mem 4 line 2";    break;
		case 29: name = "mem 4 line 3";    break;
		case 30: name = "mem 4 line 4";    break;
	}

	return name;
}

void __fastcall TSettingsForm::GraphColourPaintBoxPaint(TObject *Sender)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	if (m_colour_bm == NULL)
	{
		pb->Canvas->Brush->Color = pb->Color;
		pb->Canvas->Brush->Style = bsSolid;
		pb->Canvas->FillRect(pb->Canvas->ClipRect);
		return;
	}

	if (m_colour_bm->Width != pb->Width || m_colour_bm->Height != pb->Height)
	{
		m_colour_bm->Width  = pb->Width;
		m_colour_bm->Height = pb->Height;
	}

	m_colour_bm->Canvas->Font = (settings.graphFont != NULL) ? settings.graphFont : pb->Canvas->Font;

	//m_colour_bm->Canvas->Font->Style = m_colour_bm->Canvas->Font->Style << fsBold;
	m_colour_bm->Canvas->Font->Size = m_colour_bm->Canvas->Font->Size + 1;

	const int border_size = 10;

	// background colour
	m_colour_bm->Canvas->Pen->Width   = 1;
	m_colour_bm->Canvas->Pen->Style   = psSolid;
	m_colour_bm->Canvas->Pen->Color   = settings.m_colours.border;
	m_colour_bm->Canvas->Brush->Color = settings.m_colours.background;
	m_colour_bm->Canvas->Brush->Style = bsSolid;
	m_colour_bm->Canvas->Rectangle(m_colour_bm->Canvas->ClipRect);
	//m_colour_bm->Canvas->FillRect(m_colour_bm->Canvas->ClipRect);

//	const int th = m_colour_bm->Canvas->TextHeight("Hq");

	const int x = border_size + 10;
	int       y = border_size + 5;

	const int v_spacing = (m_colour_bm->Height - (y * 2)) / ((11 + (MAX_MEMORIES * MAX_CHANNELS)) - 1);

	m_colour_bm->Canvas->Pen->Style   = psSolid;
	m_colour_bm->Canvas->Brush->Color = settings.m_colours.background;
	m_colour_bm->Canvas->Brush->Style = bsSolid;

	{	// try to ensure that the font is always visible no matter what the background colour is
		const uint32_t colour = Graphics::ColorToRGB(settings.m_colours.background);
		const uint8_t red     = (colour >>  0) & 0xff;
		const uint8_t grn     = (colour >>  8) & 0xff;
		const uint8_t blu     = (colour >> 16) & 0xff;
		uint8_t level         = ((uint16_t)red + (uint16_t)grn + (uint16_t)blu) / 3;
		level = (level < 128) ? 255 : 0;
		m_colour_bm->Canvas->Font->Color = TColor(RGB(level, level, level));
	}

	m_colour_picker_rect.resize(0);

	showColour(m_colour_bm, x, y, colourName(0), settings.m_colours.border);
	y += v_spacing;

	showColour(m_colour_bm, x, y, colourName(1), settings.m_colours.background);
	y += v_spacing;

	showColour(m_colour_bm, x, y, colourName(2), settings.m_colours.grid);
	y += v_spacing;

	showColour(m_colour_bm, x, y, colourName(3), settings.m_colours.mouse_line);
	y += v_spacing;

	showColour(m_colour_bm, x, y, colourName(4), settings.m_colours.marker_line);
	y += v_spacing;

	showColour(m_colour_bm, x, y, colourName(5), settings.m_colours.marker);
	y += v_spacing;

	showColour(m_colour_bm, x, y, colourName(6), settings.m_colours.marker_selected);
	y += v_spacing;

	showColour(m_colour_bm, x, y, colourName(7), settings.m_colours.font);
	y += v_spacing;

	showColour(m_colour_bm, x, y, colourName(8), settings.m_colours.vswr2_line);
	y += v_spacing;

	showColour(m_colour_bm, x, y, colourName(9), settings.m_colours.mouse_marker);
	y += v_spacing;

	showColour(m_colour_bm, x, y, colourName(10), settings.m_colours.point);
	y += v_spacing;

	for (unsigned int m = 0; m < MAX_MEMORIES; m++)
	{
		for (unsigned int c = 0; c < MAX_CHANNELS; c++)
		{
			String colour_name = colourName(11 + (m * MAX_CHANNELS) + c);
			showColour(m_colour_bm, x, y, colour_name, settings.m_colours.line[m][c]);
			y += v_spacing;
		}
	}

//	pb->Canvas->CopyMode = cmSrcCopy;
	pb->Canvas->Draw(0, 0, m_colour_bm);
}

void __fastcall TSettingsForm::showColour(Graphics::TBitmap *bm, int x, int y, String name, TColor colour)
{
	String s;
	s.printf(L"%06X ", Graphics::ColorToRGB(colour));
	name = s + name;

	const int tw = m_colour_bm->Canvas->TextWidth(name);
	const int th = m_colour_bm->Canvas->TextWidth("Hq");

	m_colour_picker_rect.push_back(TRect(x - 9, y - 5, x + 50 + 20 + tw + 9, y + 5));

	TRect line_rect = TRect(x, y - 3, x + 50, y + 3);

	const int cp = m_colour_picker_rect.size() - 1;
	if (m_colour_picker_index == cp)
	{
		m_colour_bm->Canvas->Pen->Width   = 1;
		m_colour_bm->Canvas->Pen->Color   = settings.m_dark_mode ? TColor(RGB(80, 80, 80)) : TColor(RGB(200, 200, 200));
		m_colour_bm->Canvas->Brush->Style = bsClear;
		m_colour_bm->Canvas->Rectangle(m_colour_picker_rect[cp]);
		line_rect.Inflate(0, 2);
	}

	m_colour_bm->Canvas->Pen->Width   = 1;
	m_colour_bm->Canvas->Pen->Color   = colour;
	m_colour_bm->Canvas->Brush->Color = colour;
	m_colour_bm->Canvas->Rectangle(line_rect);

	m_colour_bm->Canvas->Brush->Style = bsClear;
	m_colour_bm->Canvas->TextOut(x + 50 + 15, y - (th / 2), name);
}

void __fastcall TSettingsForm::onThemeChange(TObject *Sender)
{
	GraphColourPaintBox->Invalidate();
}

void __fastcall TSettingsForm::GUIStyleComboBoxChange(TObject *Sender)
{
	settings.setColourStyle(GUIStyleComboBox->Text);

	GraphColourPaintBox->Invalidate();

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);

	// TODO: fix get back focus after setting a new GUI style

	this->BringToFront();

	if (this->CanFocus())
		this->SetFocus();

	if (GUIStyleComboBox->CanFocus())
		GUIStyleComboBox->SetFocus();
}

void __fastcall TSettingsForm::GraphColourPaintBoxMouseMove(TObject *Sender, TShiftState Shift,
			 int X, int Y)
{
	m_mouse_x = X;
	m_mouse_y = Y;

	int colour_index = -1;
	for (int i = 0; i < (int)m_colour_picker_rect.size(); i++)
	{
		const TRect rect = m_colour_picker_rect[i];
		if (X >= rect.Left && Y >= rect.Top && X < rect.Right && Y < rect.Bottom)
		{
			colour_index = i;
			break;
		}
	}

	if (m_colour_picker_index != colour_index)
	{
		m_colour_picker_index = colour_index;
		GraphColourPaintBox->Invalidate();
	}
}

void __fastcall TSettingsForm::GraphColourPaintBoxMouseLeave(TObject *Sender)
{
	m_mouse_x = -1;
	m_mouse_y = -1;
	m_colour_picker_index = -1;

	GraphColourPaintBox->Invalidate();
}

void __fastcall TSettingsForm::colourPickerColourUpdated(TObject *Sender, TColor colour)
{
	if (m_colour_picker_colour)
	{
		*m_colour_picker_colour = colour;

		GraphColourPaintBox->Refresh();

		if (Application->MainForm)
			::SendMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TSettingsForm::colourPickerOnClose(TObject *Sender, const int type, TColor colour)
{
	if (m_colour_picker_colour)
	{
		*m_colour_picker_colour = colour;
		switch (type)
		{
			case COLOUR_PICKER_NO_BUT:
				settings.saveColourScheme();
				break;
			case COLOUR_PICKER_CANCEL_BUT:
				break;
			case COLOUR_PICKER_OK_BUT:
				settings.saveColourScheme();
				break;
		}
		GraphColourPaintBox->Refresh();

		if (Application->MainForm)
			::SendMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
	m_colour_picker_colour = NULL;
}

void __fastcall TSettingsForm::GraphColourPaintBoxMouseDown(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	if (m_colour_picker_index >= 0 && Shift.Contains(ssLeft))
	{
		TColor *colour = NULL;
		switch (m_colour_picker_index)
		{
			case  0: colour = &settings.m_colours.border;          break;
			case  1: colour = &settings.m_colours.background;      break;
			case  2: colour = &settings.m_colours.grid;            break;
			case  3: colour = &settings.m_colours.mouse_line;      break;
			case  4: colour = &settings.m_colours.marker_line;     break;
			case  5: colour = &settings.m_colours.marker;          break;
			case  6: colour = &settings.m_colours.marker_selected; break;
			case  7: colour = &settings.m_colours.font;            break;
			case  8: colour = &settings.m_colours.vswr2_line;      break;
			case  9: colour = &settings.m_colours.mouse_marker;    break;
			case 10: colour = &settings.m_colours.point;           break;

			case 11: colour = &settings.m_colours.line[0][0];      break;
			case 12: colour = &settings.m_colours.line[0][1];      break;
			case 13: colour = &settings.m_colours.line[0][2];      break;
			case 14: colour = &settings.m_colours.line[0][3];      break;

			case 15: colour = &settings.m_colours.line[1][0];      break;
			case 16: colour = &settings.m_colours.line[1][1];      break;
			case 17: colour = &settings.m_colours.line[1][2];      break;
			case 18: colour = &settings.m_colours.line[1][3];      break;

			case 19: colour = &settings.m_colours.line[2][0];      break;
			case 20: colour = &settings.m_colours.line[2][1];      break;
			case 21: colour = &settings.m_colours.line[2][2];      break;
			case 22: colour = &settings.m_colours.line[2][3];      break;

			case 23: colour = &settings.m_colours.line[3][0];      break;
			case 24: colour = &settings.m_colours.line[3][1];      break;
			case 25: colour = &settings.m_colours.line[3][2];      break;
			case 26: colour = &settings.m_colours.line[3][3];      break;

			case 27: colour = &settings.m_colours.line[4][0];      break;
			case 28: colour = &settings.m_colours.line[4][1];      break;
			case 29: colour = &settings.m_colours.line[4][2];      break;
			case 30: colour = &settings.m_colours.line[4][3];      break;

			default: break;
		}

		String colour_name = colourName(m_colour_picker_index);

		if (colour != NULL)
		{
			#if 1
				if (ColourPickerForm)
				{
					TPoint point = pb->ClientToScreen(TPoint(X + 50, Y - (ColourPickerForm->Height / 2)));
					if (point.x < 0) point.x = 0;
					else
					if (point.x > (Screen->Width - ColourPickerForm->Width)) point.x = Screen->Width - ColourPickerForm->Width;
					if (point.y < 0) point.y = 0;
					else
					if (point.y > (Screen->Height - ColourPickerForm->Height)) point.y = Screen->Height - ColourPickerForm->Height;

					m_colour_picker_colour = colour;
//					ColourPickerForm->show(this, point.x, point.y, *colour, colour_name);
					ColourPickerForm->show(this, -1, -1, *colour, colour_name);
					ColourPickerForm->onColourUpdated = colourPickerColourUpdated;
					ColourPickerForm->onClose = colourPickerOnClose;
				}
			#else
				ColorDialog1->Color = *colour;
				if (ColorDialog1->Execute(this->Handle))
				{
					*colour = ColorDialog1->Color;
					settings.saveColourScheme();
					GraphColourPaintBox->Invalidate();

					if (Application->MainForm)
						::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
				}
			#endif
		}

		return;
	}
}

void __fastcall TSettingsForm::DefaultDarkColoursButtonClick(TObject *Sender)
{
	TButton *but = dynamic_cast<TButton *>(Sender);
	if (but == NULL)
		return;

	if (ColourPickerForm)
		ColourPickerForm->Hide();

	const bool dark = (but == DefaultDarkColoursButton) ? true : false;

	settings.setDefaultColourScheme(dark);

	LineAlphaTrackBar->Position = 255;
	//LineWidthTrackBar->Position = 1;

	//BorderWidthTrackBar->Position = 3;

	GraphColourPaintBox->Invalidate();

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TSettingsForm::sendThresholdFreq()
{
	double value;
	if (common.strToMHz(ThresholdEdit->Text, value) && value > 0)
	{
		int64_t threshold = I64ROUND(value * 1e6);
		if (Form1)
		{
			if (data_unit.m_vna_data.cmd_threshold)
			{
				String s;
				s.printf(L"threshold %lld", threshold);
				Form1->addSerialTxCommand(s);

				Form1->addSerialTxCommand("threshold");
			}
		}
	}
}

void __fastcall TSettingsForm::sendVBATOffset()
{
	const int vbat_offset_mv = VBatOffsetSpinEdit->Value;

	if (Form1)
	{
		if (data_unit.m_vna_data.cmd_vbat_offset)
		{
			String s;
			s.printf(L"vbat_offset %d", vbat_offset_mv);
			Form1->addSerialTxCommand(s);

			Form1->addSerialTxCommand("vbat_offset");
		}
	}
}

void __fastcall TSettingsForm::ThresholdEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if (Key == VK_RETURN)
	{
		Key = 0;
		sendThresholdFreq();
	}
}

void __fastcall TSettingsForm::LineAlphaTrackBarChange(TObject *Sender)
{
	settings.lineAlpha = LineAlphaTrackBar->Position;

	String s;
	s.printf(L" Line alpha (transparency) %d ", settings.lineAlpha);
	LineAlphaTrackBar->Hint = s;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TSettingsForm::LineWidthTrackBarChange(TObject *Sender)
{
	settings.lineWidth = LineWidthTrackBar->Position;

	String s;
	s.printf(L"Line width %d ", settings.lineWidth);
	LineWidthTrackBar->Hint = s;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TSettingsForm::FontButtonClick(TObject *Sender)
{
	FontDialog1->Font = (settings.graphFont != NULL) ? settings.graphFont : GraphColourPaintBox->Font;

	if (!FontDialog1->Execute(this->Handle))
		return;

	settings.graphFont = FontDialog1->Font;

	GraphColourPaintBox->Invalidate();

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_REBUILD_FONTS, 0, 0);
}

void __fastcall TSettingsForm::FontDialog1Apply(TObject *Sender, HWND Wnd)
{
	settings.graphFont = FontDialog1->Font;

	GraphColourPaintBox->Invalidate();

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TSettingsForm::VBatOffsetSpinEditChange(TObject *Sender)
{
	sendVBATOffset();
}

void __fastcall TSettingsForm::FormShow(TObject *Sender)
{
	if (!m_initialised)
	{
		// move to the saved position
		this->Top    = settings.settingsWindowPos.top;
		this->Left   = settings.settingsWindowPos.left;
		this->Width  = settings.settingsWindowPos.width;
		this->Height = settings.settingsWindowPos.height;

		m_initialised = true;
	}
/*
	// make all other forms that are showing transparent
	for (int i = 0; i < Application->ComponentCount; i++)
	{
		TComponent *comp = Application->Components[i];
		if (comp)
		{
			TForm *form = dynamic_cast<TForm *>(comp);
			if (form)
			{
				if (form != Application->MainForm && form != this)
				{
					if (form->Showing)
					{
						form->AlphaBlendValue = FORM_ALPHA_BLEND_VALUE;
						form->AlphaBlend      = true;
					}
				}
			}
		}
	}
*/
}

void __fastcall TSettingsForm::RecordPathEditChange(TObject *Sender)
{
	if (settings.recordFolder != RecordPathEdit->Text.Trim())
		settings.recordFolder = RecordPathEdit->Text.Trim();
}

void __fastcall TSettingsForm::SelectRecordPathBitBtnClick(TObject *Sender)
{
	String path = IncludeTrailingPathDelimiter(settings.recordFolder);

	const bool path_exists = DirectoryExists(path);

	// if the current path doesn't currently exist the OS's directory dialog box takes well away from this directory :(
	// so create the directory to stop it doing so - even if only temporary
	if (!path_exists)
		common.createPath(AnsiString(path).c_str());

	Application->NormalizeTopMosts();
	bool ok;
	try
	{
		//ok = SelectDirectory("Select the recording folder ..", "", path, TSelectDirExtOpts() << sdNewUI << sdNewFolder << sdShowEdit << sdShowShares << sdShowFiles <<sdValidateDir, this);
		ok = SelectDirectory("Select the recording folder ..", "", path, TSelectDirExtOpts() << sdNewUI << sdNewFolder << sdShowEdit << sdShowShares << sdValidateDir, this);
	}
	catch (...)
	{
		Application->RestoreTopMosts();
		if (!path_exists)
			RemoveDir(path);
		return;
	}
	Application->RestoreTopMosts();

	if (!path_exists)
		RemoveDir(path);

	if (!ok)
		return;

	settings.recordFolder = path;

	RecordPathEdit->Text = settings.recordFolder;
}

void __fastcall TSettingsForm::CloseBitBtnClick(TObject *Sender)
{
	#ifdef _INC_MMSYSTEM
//		if (data_unit.stereophonic_wav.size() > MIN_WAV_SIZE)
//			PlaySound(&data_unit.stereophonic_wav[0], NULL, SND_MEMORY | SND_NODEFAULT | SND_NOWAIT | SND_ASYNC);
	#endif

	if (ColourPickerForm)
		ColourPickerForm->Hide();

	this->Hide();
}

void __fastcall TSettingsForm::MarkerFillToggleSwitchClick(TObject *Sender)
{
	settings.markerFill = (MarkerFillToggleSwitch->State == tssOn) ? true : false;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TSettingsForm::FormMouseEnter(TObject *Sender)
{
//	this->SetFocus();
}

void __fastcall TSettingsForm::FreqBandPadComboBoxChange(TObject *Sender)
{
	const int i = FreqBandPadComboBox->ItemIndex;
	if (i >= 0)
		settings.freqBandPaddingPercent = (int)FreqBandPadComboBox->Items->Objects[i];
}

void __fastcall TSettingsForm::BorderWidthTrackBarChange(TObject *Sender)
{
	settings.borderWidth = BorderWidthTrackBar->Position;

	String s;
	s.printf(L"Border width %d ", settings.borderWidth);
	BorderWidthTrackBar->Hint = s;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TSettingsForm::Button1Click(TObject *Sender)
{
	if (ColourPickerForm)
		ColourPickerForm->Hide();

	settings.m_colours.background = TStyleManager::ActiveStyle->GetStyleColor(scWindow);
	settings.saveColourScheme();

	GraphColourPaintBox->Invalidate();

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TSettingsForm::DefaultGraphScalesButtonClick(TObject *Sender)

{
	settings.setDefaultGraphScales();

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TSettingsForm::AutoScalePeakHoldToggleSwitchClick(TObject *Sender)

{
	settings.autoScalePeakHold = (AutoScalePeakHoldToggleSwitch->State == tssOn) ? true : false;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TSettingsForm::FormHide(TObject *Sender)
{
	if (ColourPickerForm)
		ColourPickerForm->Hide();
}

void __fastcall TSettingsForm::SmithBothScalesToggleSwitchClick(TObject *Sender)

{
	settings.smithBothScales = (SmithBothScalesToggleSwitch->State == tssOn) ? true : false;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

