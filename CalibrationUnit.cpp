
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#include <vcl.h>
#include <Vcl.FileCtrl.hpp>
#include <System.IOUtils.hpp>	// TPath
#include <Clipbrd.hpp>

#pragma hdrstop

#include "CalibrationUnit.h"
#include "common.h"
#include "Settings.h"
#include "DataUnit.h"
#include "Unit1.h"
#include "Calibration.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TCalibrationForm *CalibrationForm = NULL;

__fastcall TCalibrationForm::TCalibrationForm(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall TCalibrationForm::FormCreate(TObject *Sender)
{
	TNotifyEvent ne;

	m_initialised = false;

	m_gdi_plus_token = 0;
	Gdiplus::GdiplusStartup(&m_gdi_plus_token, &m_gdi_plus_startup_input, NULL);
	m_gdi_plus = NULL;

	Caption = Application->Title + " " + this->Caption;

	this->ControlStyle                     = this->ControlStyle << csOpaque;
	CalibrationFilesListView->ControlStyle = CalibrationFilesListView->ControlStyle << csOpaque;

	m_graph_bm = new Graphics::TBitmap();
	if (m_graph_bm)
	{
		m_graph_bm->Monochrome   = false;
		m_graph_bm->Transparent  = false;
		m_graph_bm->PixelFormat  = pf32bit;
	}

	m_cal_button_popup_menu_index = -1;
	m_list_popup_menu_index = -1;

	m_measure_type   = CAL_TYPE_NONE;
	m_measure_count  = 0;

	updateLabels();

	ne = CalibrationPathEdit->OnChange;
	CalibrationPathEdit->OnChange = NULL;
	CalibrationPathEdit->Text = settings.calibrationFolder;
	CalibrationPathEdit->OnChange = ne;

	updateCalibrationFileList(settings.calibrationFolder);

	setButtonGlyph(OpenBitBtn, 0);
	setButtonGlyph(ShortBitBtn, 0);
	setButtonGlyph(LoadBitBtn, 0);
	setButtonGlyph(ThroughBitBtn, 0);

	ne = AverageTrackBar->OnChange;
	AverageTrackBar->OnChange = NULL;
	AverageTrackBar->Position = settings.averageCalibration;
	AverageTrackBar->OnChange = ne;
	AverageTrackBarChange(AverageTrackBar);

	ne = SmoothingTrackBar->OnChange;
	SmoothingTrackBar->OnChange = NULL;
	SmoothingTrackBar->Position = settings.smoothingCalibration;
	SmoothingTrackBar->OnChange = ne;
	SmoothingTrackBarChange(SmoothingTrackBar);

	ne = MedianTrackBar->OnChange;
	MedianTrackBar->OnChange = NULL;
	MedianTrackBar->Position = settings.medianCalibration;
	MedianTrackBar->OnChange = ne;
	MedianTrackBarChange(MedianTrackBar);

	// move to the saved position
	this->Top    = settings.calibrationWindowPos.top;
	this->Left   = settings.calibrationWindowPos.left;
	this->Width  = settings.calibrationWindowPos.width;
	this->Height = settings.calibrationWindowPos.height;
}

void __fastcall TCalibrationForm::FormDestroy(TObject *Sender)
{
	m_measure_type  = CAL_TYPE_NONE;
	m_measure_count = 0;

	if (m_graph_bm != NULL)
		delete m_graph_bm;
	m_graph_bm = NULL;

	if (m_gdi_plus)
		delete m_gdi_plus;
	m_gdi_plus = NULL;

	if (m_gdi_plus_token != 0)
		Gdiplus::GdiplusShutdown(m_gdi_plus_token);
	m_gdi_plus_token = 0;
}

void __fastcall TCalibrationForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	m_measure_type  = CAL_TYPE_NONE;
	m_measure_count = 0;
}

void __fastcall TCalibrationForm::WMWindowPosChanging(TWMWindowPosChanging &msg)
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

void __fastcall TCalibrationForm::show()
{
	updateLabels();
	setButtonStates();

	this->Show();
	this->BringToFront();
//	this->Update();
}

void __fastcall TCalibrationForm::FormShow(TObject *Sender)
{
	if (!m_initialised)
	{
		// move to the saved position
		this->Top    = settings.calibrationWindowPos.top;
		this->Left   = settings.calibrationWindowPos.left;
		this->Width  = settings.calibrationWindowPos.width;
		this->Height = settings.calibrationWindowPos.height;

		m_initialised = true;
	}
}
void __fastcall TCalibrationForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	switch (Key)
	{
		case VK_ESCAPE:
			Key = 0;
			this->Hide();
			break;
	}
}

void __fastcall TCalibrationForm::FormMouseEnter(TObject *Sender)
{
//	this->SetFocus();
}

void __fastcall TCalibrationForm::LoadFileBitBtnClick(TObject *Sender)
{
	stop();

	String path = IncludeTrailingPathDelimiter(settings.calibrationFolder);

	const bool path_exists = DirectoryExists(path);
	if (!path_exists)
		common.createPath(AnsiString(path).c_str());

	loadCalibration("");
}

void __fastcall TCalibrationForm::loadCalibration(String filename)
{
	std::vector <t_calibration_point> calibration_points;

	filename = common.loadCalibrationFile(filename, calibration_points);
	if (filename.IsEmpty() || calibration_points.empty())
	{
		return;
	}

	m_calibration.filename = filename;
	m_calibration.name     = TPath::GetFileNameWithoutExtension(filename);
	m_calibration.point    = calibration_points;

	updateLabels();
	setButtonStates();

	applyCalibration();
}
/*
void __fastcall TCalibrationForm::updateMemo()
{
	String s;
	String str;

//	Beep(3000, 1);

	Memo1->Lines->BeginUpdate();

	Memo1->Clear();

	if (!m_calibration.point.empty())
	{
		Memo1->Lines->Add("");

		Memo1->Lines->Add(" filename: " + m_calibration.filename);
		Memo1->Lines->Add("     name: " + m_calibration.name);
		Memo1->Lines->Add("   points: " + UIntToStr(m_calibration.point.size()));
		Memo1->Lines->Add("");

//		s += " " + common.padLeft("No.", 5);
		s += " " + common.padLeft("Hz", 11);
		s += " " + common.padLeft("Short Re", 18);
		s += " " + common.padLeft("Short Im", 18);
		s += " " + common.padLeft("Open Re", 18);
		s += " " + common.padLeft("Open Im", 18);
		s += " " + common.padLeft("Load Re", 18);
		s += " " + common.padLeft("Load Im", 18);
		s += " " + common.padLeft("Isolation Re", 18);
		s += " " + common.padLeft("Isolation Im", 18);
		s += " " + common.padLeft("Through Re", 18);
		s += " " + common.padLeft("Through Im", 18);
		Memo1->Lines->Add(s);

		for (unsigned int i = 0; i < m_calibration.point.size(); i++)
		{
			complexf cpx;

			const t_calibration_point *c = &m_calibration.point[i];

			s = "";

//			str.printf(L"%u", 1 + i);
//			s += " " + common.padLeft(str, 5);

			str.printf(L"%lld", c->HzCal);
			s += " " + common.padLeft(str, 11);

			cpx = c->shortCal;
			str.printf(L"%0.9e", cpx.real);
			s += " " + common.padLeft(str, 18);
			str.printf(L"%0.9e", cpx.imag);
			s += " " + common.padLeft(str, 18);

			cpx = c->openCal;
			str.printf(L"%0.9e", cpx.real);
			s += " " + common.padLeft(str, 18);
			str.printf(L"%0.9e", cpx.imag);
			s += " " + common.padLeft(str, 18);

			cpx = c->loadCal;
			str.printf(L"%0.9e", cpx.real);
			s += " " + common.padLeft(str, 18);
			str.printf(L"%0.9e", cpx.imag);
			s += " " + common.padLeft(str, 18);

			cpx = c->isolationCal;
			str.printf(L"%0.9e", cpx.real);
			s += " " + common.padLeft(str, 18);
			str.printf(L"%0.9e", cpx.imag);
			s += " " + common.padLeft(str, 18);

			cpx = c->throughCal;
			str.printf(L"%0.9e", cpx.real);
			s += " " + common.padLeft(str, 18);
			str.printf(L"%0.9e", cpx.imag);
			s += " " + common.padLeft(str, 18);

			Memo1->Lines->Add(s);
		}
	}

	Memo1->Lines->EndUpdate();

	// *****************************
}
*/
void __fastcall TCalibrationForm::stop()
{
	m_measure_type  = CAL_TYPE_NONE;
	m_measure_count = 0;

	setButtonStates();
}

void __fastcall TCalibrationForm::ClearCalibrationButtonClick(TObject *Sender)
{
	stop();

//	m_calibration.filename = "";
//	m_calibration.name     = "";
	m_calibration.point.resize(0);

	updateLabels();
	setButtonStates();

	applyCalibration();
}

void __fastcall TCalibrationForm::updateLabels()
{
	if (m_calibration.point.empty())
	{
		CalibrationNameLabel->Caption = "";
	}
	else
	{
		CalibrationNameLabel->Caption =
			m_calibration.name
			+ ", " + common.freqToStr1(m_calibration.point[0].HzCal, true, true, 6, false) + "Hz"
			+ " to " + common.freqToStr1(m_calibration.point[m_calibration.point.size() - 1].HzCal, true, true, 6, false) + "Hz"
			+ ", " + UIntToStr(m_calibration.point.size()) + " points";
	}
}

void __fastcall TCalibrationForm::setButtonStates()
{
//	const bool cals_are_equal = areCalibrationsEqual(&m_calibration, &calibration_module.m_calibration);

	SetMainSweepButton->Enabled = !m_calibration.point.empty();

	int open_data      = 0;
	int short_data     = 0;
	int load_data      = 0;
	int isolation_data = 0;
	int through_data   = 0;

	const int data_image_index = 2;

	for (unsigned int i = 0; i < m_calibration.point.size(); i++)
	{
		const t_calibration_point *c = &m_calibration.point[i];
		if (short_data <= 0)
			if (c->shortCal != calibration_module.idealShort)
				short_data = data_image_index;
		if (open_data <= 0)
			if (c->openCal != calibration_module.idealOpen)
				open_data = data_image_index;
		if (load_data <= 0)
			if (c->loadCal != calibration_module.idealLoad)
				load_data = data_image_index;
		if (isolation_data <= 0)
			if (c->isolationCal != calibration_module.idealIsolation)
				isolation_data = data_image_index;
		if (through_data <= 0)
			if (c->throughCal != calibration_module.idealThrough)
				through_data = data_image_index;
	}

	setButtonGlyph(ShortBitBtn,   (m_measure_type != CAL_TYPE_SHORT)      ? short_data   : 0);
	setButtonGlyph(OpenBitBtn,    (m_measure_type != CAL_TYPE_OPEN)       ? open_data    : 0);
	setButtonGlyph(LoadBitBtn,    (m_measure_type != CAL_TYPE_LOAD_ISOLN) ? load_data    : 0);
	setButtonGlyph(ThroughBitBtn, (m_measure_type != CAL_TYPE_THROUGH)    ? through_data : 0);

	ShortBitBtn->Caption   = (m_measure_type == CAL_TYPE_SHORT)      ? "Capturing\nShort"    : "Short";
	OpenBitBtn->Caption    = (m_measure_type == CAL_TYPE_OPEN)       ? "Capturing\nOpen"     : "Open";
	LoadBitBtn->Caption    = (m_measure_type == CAL_TYPE_LOAD_ISOLN) ? "Capturing\nLoad/Iso" : "Load/Isoln";
	ThroughBitBtn->Caption = (m_measure_type == CAL_TYPE_THROUGH)    ? "Capturing\nThrough"  : "Through";

	ShortBitBtn->Update();
	OpenBitBtn->Update();
	LoadBitBtn->Update();
	ThroughBitBtn->Update();

//	ClearButton->Enabled       = !m_calibration.point.empty() ? true : false;
	SaveAsFileBitBtn->Enabled  = !m_calibration.point.empty() ? true : false;
	SaveFileBitBtn->Enabled    = (!m_calibration.point.empty() && !m_calibration.name.IsEmpty()) ? true : false;
	ExportFilesBitBtn->Enabled = !m_calibration.point.empty() ? true : false;
}

bool __fastcall TCalibrationForm::areCalibrationsEqual(const t_calibration *cal1, const t_calibration *cal2)
{
	if (!cal1 || !cal2)
		return false;

	if (cal1->filename != cal2->filename)
		return false;

	if (cal1->name != cal2->name)
		return false;

	if (cal1->point.size() != cal2->point.size())
		return false;

	for (unsigned int i = 0; i < cal1->point.size(); i++)
	{
		const t_calibration_point *c1 = &cal1->point[i];
		const t_calibration_point *c2 = &cal2->point[i];
		if (c1->HzCal        != c2->HzCal       ) return false;
		if (c1->openCal      != c2->openCal     ) return false;
		if (c1->shortCal     != c2->shortCal    ) return false;
		if (c1->loadCal      != c2->loadCal     ) return false;
		if (c1->isolationCal != c2->isolationCal) return false;
		if (c1->throughCal   != c2->throughCal  ) return false;
	}

	return true;
}

void __fastcall TCalibrationForm::applyCalibration()
{
	calibration_module.m_calibration.filename = m_calibration.filename;
	calibration_module.m_calibration.name     = m_calibration.name;
	calibration_module.m_calibration.point    = m_calibration.point;

	calibration_module.m_inter_cal.resize(0);

	// compute the error terms for the correction function
//	calibration_module.computeErrorTerms(calibration_module.m_calibration);

	setButtonStates();

	CalibrationFilesListView->Refresh();

	// clear the history buffer
	data_unit.clearHistory();

	settings.calibrationFile = m_calibration.filename;

	if (Form1)
	{
//		Form1->TimeAverageLevelTrackBar->Position = 0; 	// set the average slider to zero

		if (calibration_module.m_calibration.point.empty())
			Form1->setCalibrationSelection(CAL_SELECT_NONE);
		else
			Form1->setCalibrationSelection(CAL_SELECT_APP);	// switch to our own calibration

//		Form1->updateHistoryFramesInfo(true);
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TCalibrationForm::CalibrationFilesListViewData(TObject *Sender, TListItem *Item)

{
	TListView *lv = dynamic_cast<TListView *>(Sender);
	if (!lv || !Item)
		return;

	if (!lv->OwnerData)
		return;

	const int index = Item->Index;
	if (index < 0 || index >= (int)m_cal_file_list.size())
		return;

	// ************
	// see if this line is the currently in use calibration

	bool cal_in_use = false;

	const t_calibration_file_list *cal = &m_cal_file_list[index];

	const t_calibration *cal1          = &calibration_module.m_calibration;

	if (cal->points > 0)
	{
		if (cal->filename == cal1->filename)
		{
			if (cal->name == cal1->name)
			{
				if (cal->points == (int)cal1->point.size())
				{
					if (cal->start_Hz == cal1->point[0].HzCal)
					{
						if (cal->stop_Hz == cal1->point[cal1->point.size() - 1].HzCal)
						{
							#if 1
								cal_in_use = true;	// this one matches the currently in use calibration
							#else
								// load the calibration in
								std::vector <t_calibration_point> calibration_points;
								String filename = common.loadCalibrationFile(cal->filename, calibration_points);
								if (!filename.IsEmpty() && !calibration_points.empty())
								{
									if (cal1->point.size() == calibration_points.size())
									{
										const t_calibration_point *cal2 = &calibration_points[0];	// point to calibration data just loaded in
										unsigned int i = 0;
										while (i < cal1->point.size())
										{
											const t_calibration_point *c1 = &cal1->point[i];
											const t_calibration_point *c2 = &cal2[i];
											if (c1->HzCal        != c2->HzCal       ) break;
											if (c1->openCal      != c2->openCal     ) break;
											if (c1->shortCal     != c2->shortCal    ) break;
											if (c1->loadCal      != c2->loadCal     ) break;
											if (c1->isolationCal != c2->isolationCal) break;
											if (c1->throughCal   != c2->throughCal  ) break;
											i++;
										}
										if (i >= cal1->point.size())
											cal_in_use = true;	// this one matches the currently in use calibration
									}
								}
							#endif
						}
					}
				}
			}
		}
	}

	// ************

	Item->Data = (void *)index;
	Item->Caption = "";	// we don't use the first column
	if (cal_in_use)
		Item->SubItems->Add(">>>  " + IntToStr(1 + index));
	else
		Item->SubItems->Add(IntToStr(1 + index));
	Item->SubItems->Add(cal->name);
	Item->SubItems->Add(common.freqToStr1(cal->start_Hz, true, true, 6, false) + "Hz");
	Item->SubItems->Add(common.freqToStr1(cal->stop_Hz,  true, true, 6, false) + "Hz");
	Item->SubItems->Add(IntToStr(cal->points));

	// ************
}

void __fastcall TCalibrationForm::CalibrationFilesListViewKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift)
{
	TListView *lv = dynamic_cast<TListView *>(Sender);
	if (lv == NULL)
		return;

	if (Key == VK_DELETE)
	{
		Key = 0;

		const TListItem *item = lv->Selected;
		if (item == NULL)
			return;

		const int index = (int)item->Data;

		if (index >= 0 && index < (int)m_cal_file_list.size())
		{
			String filename = m_cal_file_list[index].filename;

			String s = "Are you sure you want to delete calibration file .. \n\n" + ExtractFileName(filename) + " ?";

			Application->NormalizeTopMosts();
			const int res = Application->MessageBox(s.w_str(), Application->Title.w_str(), MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
			Application->RestoreTopMosts();

			if (res == IDYES)
				if (DeleteFile(filename))
					updateCalibrationFileList(settings.calibrationFolder);
		}

		return;
	}
}

void __fastcall TCalibrationForm::SelectFilePathBitBtnClick(TObject *Sender)

{
	String path = settings.calibrationFolder;

	const bool path_exists = DirectoryExists(path);

	// if the current path doesn't currently exist the OS's directory dialog box takes well away from this directory :(
	// so create the directory to stop it doing so - even if only temporary
	if (!path_exists)
		common.createPath(AnsiString(path).c_str());

	Application->NormalizeTopMosts();
	bool ok;
	try
	{
		//ok = SelectDirectory("Select the calibrations folder ..", "", path, TSelectDirExtOpts() << sdNewUI << sdNewFolder << sdShowEdit << sdShowShares << sdShowFiles <<sdValidateDir, this);
		ok = SelectDirectory("Select the calibrations folder ..", "", path, TSelectDirExtOpts() << sdNewUI << sdNewFolder << sdShowEdit << sdValidateDir, this);
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

	settings.calibrationFolder = path;

	CalibrationPathEdit->Text = settings.calibrationFolder;
}

void __fastcall TCalibrationForm::CalibrationPathEditChange(TObject *Sender)

{
	if (settings.calibrationFolder != CalibrationPathEdit->Text.Trim())
		settings.calibrationFolder = CalibrationPathEdit->Text.Trim();
}

void __fastcall TCalibrationForm::CalibrationFilesListViewDblClick(TObject *Sender)

{
	TListView *lv = dynamic_cast<TListView *>(Sender);
	if (lv == NULL)
		return;

	const TListItem *item = lv->Selected;
	if (item == NULL)
		return;

	const int index = (int)item->Data;
	if (index < 0 || index >= (int)m_cal_file_list.size())
		return;

	t_calibration_file_list cal = m_cal_file_list[index];

	loadCalibration(cal.filename);
}

void __fastcall TCalibrationForm::updateCalibrationFileList(String directory)
{
	TListView *lv = CalibrationFilesListView;
	//const TListItem *selected_item = lv->Selected;

	m_cal_file_list.resize(0);

	std::vector <t_calibration_point> calibration_points;

	t_calibration_file_list cal_file;

	String ext = ".cal";
	String search_name = directory + "*" + ext;
	TSearchRec sr;
	int num_files = 0;

	bool found = (FindFirst(search_name, faAnyFile, sr) == 0) ? true : false;

	while (found)
	{
		String s;

		if (++num_files >= 2)
		{
			found = (FindNext(sr) == 0) ? true : false;
			if (!found)
				break;
		}

		if ((sr.Attr & (faSysFile | faDirectory)) != 0)
			continue;	// it's not a file

		// it's a file

		String filename = directory + String(sr.Name);

		if (filename.SubString(1 + filename.Length() - ext.Length(), ext.Length()).LowerCase() != ext.LowerCase())
			continue;	// different extension

		// open the file
		filename = common.loadCalibrationFile(filename, calibration_points);
		if (filename.IsEmpty() || calibration_points.empty())
			continue;

		cal_file.filename = filename;
		cal_file.name     = TPath::GetFileNameWithoutExtension(filename);
		cal_file.start_Hz = calibration_points[0].HzCal;
		cal_file.stop_Hz  = calibration_points[calibration_points.size() - 1].HzCal;
		cal_file.points   = calibration_points.size();

		m_cal_file_list.push_back(cal_file);
	}

	FindClose(sr);

	lv->Items->BeginUpdate();
	lv->Items->Count = m_cal_file_list.size();
	lv->Refresh();
	lv->Items->EndUpdate();
}

void __fastcall TCalibrationForm::ListPopupMenuPopup(TObject *Sender)
{
	LoadCalibrationFile1->Enabled   = (m_list_popup_menu_index >= 0) ? true : false;
	DeleteCalibrationFile1->Enabled = (m_list_popup_menu_index >= 0) ? true : false;
	RenameCalibrationFile1->Enabled = (m_list_popup_menu_index >= 0) ? true : false;
	CopyCalibrationName1->Enabled   = (m_list_popup_menu_index >= 0) ? true : false;
}

void __fastcall TCalibrationForm::LoadCalibrationFile1Click(TObject *Sender)

{
	if (m_list_popup_menu_index >= 0 && m_list_popup_menu_index < (int)m_cal_file_list.size())
	{
		t_calibration_file_list cal = m_cal_file_list[m_list_popup_menu_index];
		loadCalibration(cal.filename);
	}
}

void __fastcall TCalibrationForm::DeleteCalibrationFile1Click(TObject *Sender)

{
	if (m_list_popup_menu_index >= 0 && m_list_popup_menu_index < (int)m_cal_file_list.size())
	{
		String filename = m_cal_file_list[m_list_popup_menu_index].filename;

		String s = "Are you sure you want to delete calibration file .. \n\n" + ExtractFileName(filename) + " ?";

		Application->NormalizeTopMosts();
		const int res = Application->MessageBox(s.w_str(), Application->Title.w_str(), MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
		Application->RestoreTopMosts();

		if (res == IDYES)
			if (DeleteFile(filename))
				updateCalibrationFileList(settings.calibrationFolder);
	}
}

void __fastcall TCalibrationForm::CalibrationFilesListViewMouseDown(TObject *Sender,
			 TMouseButton Button, TShiftState Shift, int X, int Y)
{
	TListView *lv = dynamic_cast<TListView *>(Sender);
	if (lv == NULL)
		return;

	m_list_popup_menu_index = -1;

	if (Shift.Contains(ssRight))
	{
		const TListItem *item = lv->Selected;
		if (item)
		{
			const int index = (int)item->Data;
			m_list_popup_menu_index = index;

			const TPoint point = lv->ClientToScreen(TPoint(X, Y));
			ListPopupMenu->Popup(point.X, point.Y);
		}
	}
}

void __fastcall TCalibrationForm::CalTypeBitBtnClick(TObject *Sender)
{
	TBitBtn *but = dynamic_cast<TBitBtn *>(Sender);
	if (but == NULL)
		return;

	if (m_measure_type != CAL_TYPE_NONE)
	{	// measuring
		if (	(but == ShortBitBtn   && m_measure_type == CAL_TYPE_SHORT)    ||
				(but == OpenBitBtn    && m_measure_type == CAL_TYPE_OPEN)     ||
				(but == LoadBitBtn    && m_measure_type == CAL_TYPE_LOAD_ISOLN) ||
				(but == ThroughBitBtn && m_measure_type == CAL_TYPE_THROUGH))
		stop();	// stop measuring
		return;
	}

	m_measure_type  = CAL_TYPE_NONE;
	m_measure_count = 0;

	if (!Form1)
		return;

	if (!Form1->connected())
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"You need to be connected too your VNA", Application->Title.w_str(), MB_ICONINFORMATION | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

//	Form1->setCalibrationSelection(CAL_SELECT_NONE);	// make sure the VNA calibration is not being applied
	Form1->setCalibrationSelection(CAL_SELECT_APP);		// make sure the VNA calibration is not being applied

	if (Form1->commsMode() != MODE_SCAN)
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Your VNA needs to be continuously scanning", Application->Title.w_str(), MB_ICONINFORMATION | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	if (but == ShortBitBtn)
	{
		setButtonGlyph(ShortBitBtn, 0);
		m_measure_count = 0;
		m_measure_type  = CAL_TYPE_SHORT;
		setButtonStates();
	}
	else
	if (but == OpenBitBtn)
	{
		setButtonGlyph(OpenBitBtn, 0);
		m_measure_count = 0;
		m_measure_type  = CAL_TYPE_OPEN;
		setButtonStates();
	}
	else
	if (but == LoadBitBtn)
	{
		setButtonGlyph(LoadBitBtn, 0);
		m_measure_count = 0;
		m_measure_type  = CAL_TYPE_LOAD_ISOLN;
		setButtonStates();
	}
	else
	if (but == ThroughBitBtn)
	{
		setButtonGlyph(ThroughBitBtn, 0);
		m_measure_count = 0;
		m_measure_type  = CAL_TYPE_THROUGH;
		setButtonStates();
	}
}

void __fastcall TCalibrationForm::setButtonGlyph(TBitBtn *but, const int image_index)
{
	if (!but)
		return;

	switch (image_index)
	{
		default:
			but->Glyph->Assign(NULL);
			but->Update();
			break;
		case 1:
			but->Glyph->Assign(CheckImage1->Picture->Bitmap);
			but->Update();
			break;
		case 2:
			but->Glyph->Assign(CheckImage2->Picture->Bitmap);
			but->Update();
			break;
	}
}

void __fastcall TCalibrationForm::RenameCalibrationFile1Click(TObject *Sender)

{
	if (m_list_popup_menu_index < 0 || m_list_popup_menu_index >= (int)m_cal_file_list.size())
		return;

	String old_name = m_cal_file_list[m_list_popup_menu_index].filename;
	String path     = IncludeTrailingPathDelimiter(ExtractFilePath(old_name));
	old_name        = TPath::GetFileNameWithoutExtension(old_name);
	String new_name = old_name;

	if (!InputQuery("Rename calibration file", "New name", new_name))
		return;

	new_name = common.cleanFilename(new_name, false, true);
	if (new_name.IsEmpty() || new_name == old_name)
		return;

	old_name = path + old_name + ".cal";
	new_name = path + new_name + ".cal";

	if (FileExists(new_name))
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"File already exists", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	if (!RenameFile(old_name, new_name))
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Could not rename the file", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	updateCalibrationFileList(settings.calibrationFolder);
}

void __fastcall TCalibrationForm::filterValues(std::vector < complexf > &values, const int median_size, const int smooth_level)
{
	// smoother
	if (smooth_level > 0)
		m_smoothing_filter.process(values, 1 << (smooth_level - 1), 1, true);     // boxcar

	// median filter to remove spikes
	if (median_size > 0)
		m_median_filter.process(values, median_size);
}

void __fastcall TCalibrationForm::scanComplete(std::vector <t_data_point> &points)
{
	String s;

	if (m_measure_type == CAL_TYPE_NONE)
		return;	// not capturing

	const int size = points.size();
	if (size <= 0)
	{	// nothing supplied
//		stop();	// stop capturing
		return;
	}

	// check to see if their has been a change in any frequency
	if ((int)m_calibration.point.size() == size)
	{
		bool equal = true;
		for (int i = 0; i < size; i++)
		{
			if (m_calibration.point[i].HzCal != points[i].Hz)
			{
				equal = false;
				break;
			}
		}
		if (!equal)
			m_calibration.point.resize(0);
	}

	// create a new array if different in size
	if ((int)m_calibration.point.size() != size)
	{
		m_calibration.point.resize(size);
		for (int i = 0; i < size; i++)
		{
			m_calibration.point[i].HzCal        = points[i].Hz;
			m_calibration.point[i].shortCal     = calibration_module.idealShort;
			m_calibration.point[i].openCal      = calibration_module.idealOpen;
			m_calibration.point[i].loadCal      = calibration_module.idealLoad;
			m_calibration.point[i].isolationCal = calibration_module.idealIsolation;
			m_calibration.point[i].throughCal   = calibration_module.idealThrough;
		}
	}

	if (m_measure_count == 0)
	{	// the first scan - we ignore it as it may have started part way through
		for (int c = 0; c < MAX_CHANNELS; c++)
		{
			m_cal_samples[c].resize(size);
			const complexf cpx;
			for (int i = 0; i < size; i++)
				m_cal_samples[c][i] = cpx; // zero
		}
	}
	else
	if (m_measure_count > 0)
	{
		for (int i = 0; i < size; i++)
			if (points[i].Hz <= 0)
				return;  // missed a point - forget this run

		for (int i = 0; i < size; i++)
		{
			m_cal_samples[0][i] += points[i].s11;
			m_cal_samples[1][i] += points[i].s21;
			m_cal_samples[2][i] += points[i].s12;
			m_cal_samples[3][i] += points[i].s22;
		}
	}

	switch (m_measure_type)
	{
		case CAL_TYPE_NONE:
			break;

		case CAL_TYPE_SHORT:
			s.printf(L"Capturing\nShort %d", m_average_counts - m_measure_count);
			ShortBitBtn->Caption = s;
			ShortBitBtn->Update();

			if (m_measure_count++ >= m_average_counts)
			{	// finished capturing

				filterValues(m_cal_samples[0], settings.medianCalibration, settings.smoothingCalibration);

				const float scale = 1.0f / (m_measure_count - 1);
				for (int i = 0; i < size; i++)
					m_calibration.point[i].shortCal = m_cal_samples[0][i] * scale;

				stop();

				Beep(1800, 10);
				applyCalibration();
			}
			break;

		case CAL_TYPE_OPEN:
			s.printf(L"Capturing\nOpen %d", m_average_counts - m_measure_count);
			OpenBitBtn->Caption = s;
			OpenBitBtn->Update();

			if (m_measure_count++ >= m_average_counts)
			{	// finished capturing

				filterValues(m_cal_samples[0], settings.medianCalibration, settings.smoothingCalibration);

				const float scale = 1.0f / (m_measure_count - 1);
				for (int i = 0; i < size; i++)
					m_calibration.point[i].openCal = m_cal_samples[0][i] * scale;

				stop();

				Beep(1800, 10);
				applyCalibration();
			}
			break;

		case CAL_TYPE_LOAD_ISOLN:
			s.printf(L"Capturing\nLoad/Isoln %d", m_average_counts - m_measure_count);
			LoadBitBtn->Caption = s;
			LoadBitBtn->Update();

			if (m_measure_count++ >= m_average_counts)
			{	// finished capturing

				filterValues(m_cal_samples[0], settings.medianCalibration, settings.smoothingCalibration);
				filterValues(m_cal_samples[1], settings.medianCalibration, settings.smoothingCalibration);

				const float scale = 1.0f / (m_measure_count - 1);
				for (int i = 0; i < size; i++)
				{
					m_calibration.point[i].loadCal      = m_cal_samples[0][i] * scale;
					m_calibration.point[i].isolationCal = m_cal_samples[1][i] * scale;
				}

				stop();

				Beep(1800, 10);
				applyCalibration();
			}
			break;

		case CAL_TYPE_THROUGH:	// through
			s.printf(L"Capturing\nThrough %d", m_average_counts - m_measure_count);
			ThroughBitBtn->Caption = s;
			ThroughBitBtn->Update();

			if (m_measure_count++ >= m_average_counts)
			{	// finished capturing

				filterValues(m_cal_samples[1], settings.medianCalibration, settings.smoothingCalibration);

				const float scale = 1.0f / (m_measure_count - 1);
				for (int i = 0; i < size; i++)
					m_calibration.point[i].throughCal = m_cal_samples[1][i] * scale;

				stop();

				Beep(1800, 10);
				applyCalibration();
			}
			break;

		default:	// error
			stop();
			break;
	}
}

void __fastcall TCalibrationForm::FormHide(TObject *Sender)
{
	stop();

/*	if (!areCalibrationsEqual(&m_calibration, &calibration_module.m_calibration))
	{
		Application->NormalizeTopMosts();
		const int res = Application->MessageBox(L"You have unapplied changes.\n\nClose without applying ?", Application->Title.w_str(), MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
		Application->RestoreTopMosts();

		if (res != IDYES)
			return;
	}
*/
}

void __fastcall TCalibrationForm::ExportFilesBitBtnClick(TObject *Sender)
{
// stop();

	// *************
	// choose the folder where to save the exported data

	String path = settings.calibrationFolder;

	const bool path_exists = DirectoryExists(path);

	// if the current path doesn't currently exist the OS's directory dialog box takes well away from this directory :(
	// so create the directory to stop it doing so - even if only temporary
	if (!path_exists)
		common.createPath(AnsiString(path).c_str());

	bool ok;

	Application->NormalizeTopMosts();
	try
	{
		//ok = SelectDirectory("Select where to save the exported files ..", "", path, TSelectDirExtOpts() << sdNewUI << sdNewFolder << sdShowEdit << sdShowShares << sdShowFiles <<sdValidateDir, this);
		ok = SelectDirectory("Select where to save the exported files ..", "", path, TSelectDirExtOpts() << sdNewUI << sdNewFolder << sdShowEdit << sdValidateDir, this);
	}
	catch (...)
	{
		Application->RestoreTopMosts();
		if (!path_exists)
			RemoveDir(path);
		return;
	}
	Application->RestoreTopMosts();

	if (!ok)
	{
		if (!path_exists)
			RemoveDir(path);
		return;
	}

	// *************

	path = IncludeTrailingPathDelimiter(path);

	if (!DirectoryExists(path))
		common.createPath(AnsiString(path).c_str());

	std::vector <t_data_point> points(m_calibration.point.size());
	for (unsigned int i = 0; i < m_calibration.point.size(); i++)
		points[i].Hz = m_calibration.point[i].HzCal;

	{  // open
		for (unsigned int i = 0; i < m_calibration.point.size(); i++)
			points[i].sParam[0] = m_calibration.point[i].openCal;

		String filename = path + m_calibration.name + " open.s1p";
		if (!common.saveSParams(points, 1, filename))
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(String("Open not saved to ..\n\n" + filename).w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
	}

	{  // short
		for (unsigned int i = 0; i < m_calibration.point.size(); i++)
			points[i].sParam[0] = m_calibration.point[i].shortCal;

		String filename = path + m_calibration.name + " short.s1p";
		if (!common.saveSParams(points, 1, filename))
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(String("Short not saved to ..\n\n" + filename).w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
	}

	{  // load
		for (unsigned int i = 0; i < m_calibration.point.size(); i++)
			points[i].sParam[0] = m_calibration.point[i].loadCal;

		String filename = path + m_calibration.name + " load.s1p";
		if (!common.saveSParams(points, 1, filename))
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(String("Load not saved to ..\n\n" + filename).w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
	}

	{  // isolation
		for (unsigned int i = 0; i < m_calibration.point.size(); i++)
			points[i].sParam[0] = m_calibration.point[i].isolationCal;

		String filename = path + m_calibration.name + " isolation.s1p";
		if (!common.saveSParams(points, 1, filename))
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(String("Isolation not saved to ..\n\n" + filename).w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
	}

	{  // through
		for (unsigned int i = 0; i < m_calibration.point.size(); i++)
			points[i].sParam[0] = m_calibration.point[i].throughCal;

		String filename = path + m_calibration.name + " through.s1p";
		if (!common.saveSParams(points, 1, filename))
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(String("Through not saved to ..\n\n" + filename).w_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
	}
}

void __fastcall TCalibrationForm::SaveFileBitBtnClick(TObject *Sender)
{
	if (m_calibration.point.empty())
	{	// no calibration data
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Calibration is empty", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	String name = m_calibration.name.Trim();
	if (name.IsEmpty())
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"You need to enter a name for the calibration in the 'Calibration name' edit box", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	String path = IncludeTrailingPathDelimiter(settings.calibrationFolder);

	const bool path_exists = DirectoryExists(path);
	if (!path_exists)
		common.createPath(AnsiString(path).c_str());

	String filename = path + name + ".cal";
	filename = common.saveCalibrationFile(filename, m_calibration.point, false);
	if (!filename.IsEmpty())
	{
		m_calibration.filename = filename;
		updateCalibrationFileList(settings.calibrationFolder);
	}
}

void __fastcall TCalibrationForm::SaveAsFileBitBtnClick(TObject *Sender)
{
	if (m_calibration.point.empty())
	{	// no calibration data
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Calibration is empty", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	String path = IncludeTrailingPathDelimiter(settings.calibrationFolder);

	const bool path_exists = DirectoryExists(path);
	if (!path_exists)
		common.createPath(AnsiString(path).c_str());

	String filename;
	if (!m_calibration.name.IsEmpty())
		filename = path + m_calibration.name + ".cal";

	filename = common.saveCalibrationFile(filename, m_calibration.point, true);
	if (!filename.IsEmpty())
	{
		m_calibration.name = TPath::GetFileNameWithoutExtension(filename);
		updateLabels();

		updateCalibrationFileList(settings.calibrationFolder);
	}
}

void __fastcall TCalibrationForm::CalibrationFilesListViewMouseLeave(TObject *Sender)

{
	TListView *lv = dynamic_cast<TListView *>(Sender);
	if (!lv)
		return;

	lv->Hint = "";
}

void __fastcall TCalibrationForm::CalibrationFilesListViewSelectItem(TObject *Sender,
			 TListItem *Item, bool Selected)
{
	TListView *lv = dynamic_cast<TListView *>(Sender);
	if (!lv)
		return;

	const TListItem *item = lv->Selected;
	if (item == NULL)
	{
		lv->Hint = "";
		return;
	}

	const int index = (int)item->Data;
	if (index < 0 || index >= (int)m_cal_file_list.size())
	{
		lv->Hint = "";
		return;
	}

	lv->Hint = m_cal_file_list[index].name;
}

void __fastcall TCalibrationForm::CopyCalibrationName1Click(TObject *Sender)

{
	if (m_list_popup_menu_index >= 0 && m_list_popup_menu_index < (int)m_cal_file_list.size())
	{
		String name = TPath::GetFileNameWithoutExtension(m_cal_file_list[m_list_popup_menu_index].filename);
		TClipboard *cb;
		try
		{
			cb = Clipboard();
			cb->Clear();
			cb->SetTextBuf(name.w_str());		// copy the calibration name to the clipboard
		}
		catch (Exception &exception)
		{
			//Application->ShowException(&exception);
			//String s = exception.ToString();
		}
	}
}

void __fastcall TCalibrationForm::AverageTrackBarChange(TObject *Sender)

{
	TTrackBar *tb = dynamic_cast<TTrackBar *>(Sender);
	if (!tb)
		return;

	settings.averageCalibration = tb->Position;

	m_average_counts = 1 << settings.averageCalibration;

	String s;
	if (m_average_counts <= 1)
		s = "off";
	else
		s.printf(L"%d", m_average_counts);
	AverageLabel->Caption = s;
	AverageLabel->Update();
}

void __fastcall TCalibrationForm::SmoothingTrackBarChange(TObject *Sender)

{
	TTrackBar *tb = dynamic_cast<TTrackBar *>(Sender);
	if (!tb)
		return;

	settings.smoothingCalibration = tb->Position;

	String s;
	if (settings.smoothingCalibration <= 0)
		s = "off";
	else
		s.printf(L"%d", settings.smoothingCalibration);

	SmoothingLabel->Caption = s;
//	SmoothingTrackBar->Hint = "Cal smoothing " + s;
}

void __fastcall TCalibrationForm::MedianTrackBarChange(TObject *Sender)
{
	TTrackBar *tb = dynamic_cast<TTrackBar *>(Sender);
	if (!tb)
		return;

	settings.medianCalibration = tb->Position;

	String s;
	if (settings.medianCalibration <= 0)
		s = "off";
	else
		s.printf(L"%d", settings.medianCalibration);

	MedianLabel->Caption = s;
//	MedianTrackBar->Hint = "Cal median " + s;
}

void __fastcall TCalibrationForm::CalButtonBitBtnMouseDown(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y)
{
	TBitBtn *but = dynamic_cast<TBitBtn *>(Sender);
	if (but == NULL)
		return;

	m_cal_button_popup_menu_index = -1;

	if (Shift.Contains(ssRight))
	{
		if (but == ShortBitBtn) m_cal_button_popup_menu_index = (int)CAL_TYPE_SHORT;
		else
		if (but == OpenBitBtn) m_cal_button_popup_menu_index = (int)CAL_TYPE_OPEN;
		else
		if (but == LoadBitBtn) m_cal_button_popup_menu_index = (int)CAL_TYPE_LOAD_ISOLN;
		else
		if (but == ThroughBitBtn) m_cal_button_popup_menu_index = (int)CAL_TYPE_THROUGH;

		if (m_cal_button_popup_menu_index >= 0)
		{
			const TPoint point = but->ClientToScreen(TPoint(X, Y));
			CalButtonPopupMenu->Popup(point.X - 50, point.Y + but->Height / 2);
		}
	}
}

void __fastcall TCalibrationForm::CalButtonPopupMenuPopup(TObject *Sender)
{
	ClearCal1->Enabled = (m_cal_button_popup_menu_index >= 0) ? true : false;

	if (m_cal_button_popup_menu_index == (int)CAL_TYPE_SHORT)
		ClearCal1->Caption = "Clear SHORT";
	else
	if (m_cal_button_popup_menu_index == (int)CAL_TYPE_OPEN)
		ClearCal1->Caption = "Clear OPEN";
	else
	if (m_cal_button_popup_menu_index == (int)CAL_TYPE_LOAD_ISOLN)
		ClearCal1->Caption = "Clear LOAD/ISOLN";
	else
	if (m_cal_button_popup_menu_index == (int)CAL_TYPE_THROUGH)
		ClearCal1->Caption = "Clear THROUGH";
	else
		ClearCal1->Caption = "Clear";
}

void __fastcall TCalibrationForm::ClearCal1Click(TObject *Sender)
{
	if (m_cal_button_popup_menu_index == (int)CAL_TYPE_SHORT)
	{
		if (m_measure_type == CAL_TYPE_SHORT)
			stop();

		if (!m_calibration.point.empty())
		{
			for (unsigned int i = 0; i < m_calibration.point.size(); i++)
				m_calibration.point[i].shortCal = calibration_module.idealShort;
			setButtonStates();
		}
	}
	else
	if (m_cal_button_popup_menu_index == (int)CAL_TYPE_OPEN)
	{
		if (m_measure_type == CAL_TYPE_OPEN)
			stop();

		if (!m_calibration.point.empty())
		{
			for (unsigned int i = 0; i < m_calibration.point.size(); i++)
				m_calibration.point[i].openCal = calibration_module.idealOpen;
			setButtonStates();
		}
	}
	else
	if (m_cal_button_popup_menu_index == (int)CAL_TYPE_LOAD_ISOLN)
	{
		if (m_measure_type == CAL_TYPE_LOAD_ISOLN)
			stop();

		if (!m_calibration.point.empty())
		{
			for (unsigned int i = 0; i < m_calibration.point.size(); i++)
			{
				m_calibration.point[i].loadCal      = calibration_module.idealLoad;
				m_calibration.point[i].isolationCal = calibration_module.idealIsolation;
			}
			setButtonStates();
		}
	}
	else
	if (m_cal_button_popup_menu_index == (int)CAL_TYPE_THROUGH)
	{
		if (m_measure_type == CAL_TYPE_THROUGH)
			stop();

		if (!m_calibration.point.empty())
		{
			for (unsigned int i = 0; i < m_calibration.point.size(); i++)
				m_calibration.point[i].throughCal = calibration_module.idealThrough;
			setButtonStates();
		}
	}
}

void __fastcall TCalibrationForm::SetMainSweepButtonClick(TObject *Sender)
{
	applyCalibration();

	if (Form1)
	{
		const int points = m_calibration.point.size();
		if (points > 0)
		{
			const int64_t start_Hz = m_calibration.point[0].HzCal;
			const int64_t stop_Hz  = m_calibration.point[m_calibration.point.size() - 1].HzCal;
			Form1->setStartStopHz(start_Hz, stop_Hz, false);

//			Form1->NumberOfPointsComboBox->Text = IntToStr(points);
			const int i = Form1->NumberOfPointsComboBox->Items->IndexOfObject((TObject *)points);
			if (i >= 0)
			{
				if (Form1->NumberOfPointsComboBox->ItemIndex != i)
				{
					Form1->stop();
					Form1->NumberOfPointsComboBox->ItemIndex = i;
				}
			}
		}
	}
}

void __fastcall TCalibrationForm::CalibrationFilesListViewChange(TObject *Sender,
			 TListItem *Item, TItemChange Change)
{
	TListView *lv = dynamic_cast<TListView *>(Sender);
	if (!lv || !Item)
		return;

}

