
#include <vcl.h>
#include <Clipbrd.hpp>

#ifdef __BORLANDC__
	#if (__BORLANDC__ < 0x0600)
		#include <pngimage.hpp>
		#include <Jpeg.hpp>
		#include <GIFImg.hpp>
	#else
		#include <Vcl.Imaging.pngimage.hpp>
		#include <Vcl.Imaging.jpeg.hpp>
		#include <Vcl.Imaging.GIFImg.hpp>
	#endif
#endif

#include <math.h>

#pragma hdrstop

#include "GraphUnit.h"
#include "Unit1.h"
#include "DataUnit.h"
#include "LCMatch.h"
#include "spline.h"
#include "Graphs.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

#define POPUP_MENU_LINE_PADDING  5

TGraphForm *GraphForm = NULL;

__fastcall TGraphForm::TGraphForm(TComponent* Owner, TWinControl *parent)
	: TForm(Owner)
{
	m_parent = this->Parent;
	if (parent)
	{
		this->SetParent(parent);
		this->BorderStyle = bsNone;
		this->FormStyle   = fsNormal;
//		this->Align       = alClient;
	}
}

void __fastcall TGraphForm::FormCreate(TObject *Sender)
{
	gdiplusToken = 0;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Caption = Application->Title + " " + this->Caption;

	{	// get the phsyical screen size
		m_screen_width  = 0;
		m_screen_height = 0;

		SYSTEM_INFO m_system_info;
		HDC hDC = GetDC(0);
		if (hDC != NULL)
		{
			//ScreenBitsPerPixel = ::GetDeviceCaps(hDC, BITSPIXEL);
			m_screen_width  = ::GetDeviceCaps(hDC, HORZRES);
			m_screen_height = ::GetDeviceCaps(hDC, VERTRES);
			ReleaseDC(0, hDC);
		}
	}

	// stop flicker
	this->ControlStyle           = this->ControlStyle           << csOpaque;
	GraphPaintBox->ControlStyle  = GraphPaintBox->ControlStyle  << csOpaque;
	SaveImageImage->ControlStyle = SaveImageImage->ControlStyle << csOpaque;
	CopyImageImage->ControlStyle = CopyImageImage->ControlStyle << csOpaque;
	CloseImage->ControlStyle     = CloseImage->ControlStyle     << csOpaque;

	m_graph_bm = NULL;
/*
	m_graph_bm = new Graphics::TBitmap();
	if (m_graph_bm)
	{
		m_graph_bm->Monochrome   = false;
		m_graph_bm->Transparent  = false;
		m_graph_bm->PixelFormat  = pf32bit;
	}
*/

	m_popup_menu_mouse_graph  = -1;
	m_popup_menu_mouse_Hz     = -1;
	m_popup_menu_mouse_marker = -1;

	m_popup_menu_marker_index = -1;

	m_popup_menu_graph_type_graph = -1;

	m_popup_menu_mouse_graph = -1;

//	m_graph_type = GRAPH_TYPE_LOGMAG_S11S21;
	m_graph_type = GRAPH_TYPE_SMITH_S11;
//	m_graph_type = GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11;
//	m_graph_type = GRAPH_TYPE_TDR_LIN_BP_S11;
//	m_graph_type = GRAPH_TYPE_TDR_IMPEDANCE_S11;

	createGraphTypeMenus();

//	this->OnMessage = OnMessage;
}

void __fastcall TGraphForm::FormDestroy(TObject *Sender)
{
	if (m_parent)
	{
		this->SetParent(m_parent);
		m_parent = NULL;
	}
	else
		this->SetParent(NULL);

	if (m_graph_bm != NULL)
		delete m_graph_bm;
	m_graph_bm = NULL;

	if (gdiplusToken != 0)
		Gdiplus::GdiplusShutdown(gdiplusToken);
	gdiplusToken = 0;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_GRAPH_FORM_DESTROYED, (WPARAM)this, 0);
}

void __fastcall TGraphForm::OnMessage(tagMSG &msg, bool &handled)
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

void __fastcall TGraphForm::WMWindowPosChanging(TWMWindowPosChanging &msg)
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
//	const int dtRight  = Screen->DesktopRect.right;
	const int dtTop    = Screen->DesktopRect.top;
	const int dtBottom = Screen->DesktopRect.bottom;
	const int dtWidth  = Screen->DesktopRect.Width();
	const int dtHeight = Screen->DesktopRect.Height();

//	const int waLeft   = Screen->WorkAreaRect.left;
//	const int waRight  = Screen->WorkAreaRect.right;
//	const int waTop    = Screen->WorkAreaRect.top;
//	const int waBottom = Screen->WorkAreaRect.bottom;
//	const int waWidth  = Screen->WorkAreaRect.Width();
//	const int waHeight = Screen->WorkAreaRect.Height();

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

void __fastcall TGraphForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	this->SetParent(m_parent);
	m_parent = NULL;

	Action = caFree;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_GRAPH_FORM_CLOSED, (WPARAM)this, 0);
}

void __fastcall TGraphForm::FormShow(TObject *Sender)
{
	//
}

void __fastcall TGraphForm::FormHide(TObject *Sender)
{
	Close();
}

void __fastcall TGraphForm::createGraphTypeMenus()
{
	// reduce the item vertical spacing
	//GraphTypePopupMenu->OwnerDraw = true;
	//GraphPopupMenu->OwnerDraw     = true;

	Graphtype1->Clear();
//	for (int i = Graphtype1->Count - 1; i >= 0; i--)
//		Graphtype1->Delete(i);

	GraphTypePopupMenu->Items->Clear();
//	for (int i = GraphTypePopupMenu->Items->Count - 1; i >= 0; i--)
//		GraphTypePopupMenu->Items->Delete(i);

	if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
	{
		for (int i = 0; i < GRAPH_TYPE_SIZE; i++)
		{
			int graph_type = -1;
			String s = "None";

			switch (i)
			{
				case  0: graph_type = GRAPH_TYPE_LOGMAG_S11;             s = "Log mag S11";              break;
				case  1: graph_type = GRAPH_TYPE_LOGMAG_S21;             s = "Log mag S21";              break;
				case  2: graph_type = GRAPH_TYPE_LOGMAG_S11S21;          s = "Log mag S11 and S21";      break;
				case  3: graph_type = GRAPH_TYPE_LINMAG_S11;             s = "Lin mag S11";              break;
				case  4: graph_type = GRAPH_TYPE_LINMAG_S21;             s = "Lin mag S21";              break;
				case  5: graph_type = GRAPH_TYPE_LINMAG_S11S21;          s = "Lin mag S11 and S21";      break;
				case  6: graph_type = GRAPH_TYPE_PHASE_S11;              s = "Phase S11";                break;
				case  7: graph_type = GRAPH_TYPE_PHASE_S21;              s = "Phase S21";                break;
				case  8: graph_type = GRAPH_TYPE_PHASE_S11S21;           s = "Phase S11 and S21";        break;
				case  9: graph_type = GRAPH_TYPE_PHASE_UNWRAP_S11;       s = "Phase unwrap S11";         break;
				case 10: graph_type = GRAPH_TYPE_PHASE_UNWRAP_S21;       s = "Phase unwrap S21";         break;
				case 11: graph_type = GRAPH_TYPE_PHASE_UNWRAP_S11S21;    s = "Phase unwrap S11 and S21"; break;
				case 12: graph_type = GRAPH_TYPE_GROUP_DELAY_S11;        s = "Group delay S11";          break;
				case 13: graph_type = GRAPH_TYPE_GROUP_DELAY_S21;        s = "Group delay S21";          break;
				case 14: graph_type = GRAPH_TYPE_GROUP_DELAY_S11S21;     s = "Group delay S11 and S21";  break;
				case 15: graph_type = GRAPH_TYPE_REAL_IMAG_S11;          s = "S-Parameters S11";         break;
				case 16: graph_type = GRAPH_TYPE_REAL_IMAG_S21;          s = "S-Parameters S21";         break;
				case 17: graph_type = GRAPH_TYPE_VSWR_S11;               s = "VSWR S11";                 break;
				case 18: graph_type = GRAPH_TYPE_IMPEDANCE_S11;          s = "Impedance S11";            break;
				case 19: graph_type = GRAPH_TYPE_SERIES_RJX_S11;         s = "Series R+jX S11";          break;
				case 20: graph_type = GRAPH_TYPE_PARALLEL_RJX_S11;       s = "Parallel R||jX S11";        break;
				case 21: graph_type = GRAPH_TYPE_SERIES_RESISTANCE_S11;  s = "Series resistance S11";    break;
				case 22: graph_type = GRAPH_TYPE_SERIES_REACTANCE_S11;   s = "Series reactance S11";     break;
				case 23: graph_type = GRAPH_TYPE_QUALITY_FACTOR_S11;     s = "Quality factor S11";       break;
				case 24: graph_type = GRAPH_TYPE_SERIES_CAPACITANCE_S11; s = "Series capacitance S11";   break;
				case 25: graph_type = GRAPH_TYPE_SERIES_INDUCTANCE_S11;  s = "Series inductance S11";    break;
				case 26: graph_type = GRAPH_TYPE_COAX_LOSS_S11;          s = "Coax loss S11";            break;
				case 27: graph_type = GRAPH_TYPE_CAL_LOGMAG;             s = "Calibrations log mag";     break;
				case 28: graph_type = GRAPH_TYPE_SMITH_S11;              s = "Smith S11";                break;
				case 29: graph_type = GRAPH_TYPE_SMITH_S21;              s = "Smith S21";                break;
				case 30: graph_type = GRAPH_TYPE_ADMITTANCE_S11;         s = "Admittance S11";           break;
				case 31: graph_type = GRAPH_TYPE_ADMITTANCE_S21;         s = "Admittance S21";           break;
				case 32: graph_type = GRAPH_TYPE_POLAR_S11;              s = "Polar S11";                       break;
				case 33: graph_type = GRAPH_TYPE_POLAR_S21;              s = "Polar S21";                       break;
//				case 34: graph_type = GRAPH_TYPE_PHASE_VECTOR_S11;       s = "Phase vector S11";                break;
//				case 35: graph_type = GRAPH_TYPE_PHASE_VECTOR_S21;       s = "Phase vector S21";                break;
				case 36: graph_type = GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11; s = "TDR linear low pass impulse S11"; break;
				case 37: graph_type = GRAPH_TYPE_TDR_LIN_BP_S11;         s = "TDR linear band pass S11";        break;
				case 38: graph_type = GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11; s = "TDR log low pass impulse S11";    break;
				case 39: graph_type = GRAPH_TYPE_TDR_LOG_BP_S11;         s = "TDR log band pass S11";           break;
				case 40: graph_type = GRAPH_TYPE_TDR_IMPEDANCE_S11;      s = "TDR impedance S11";               break;
				case 41: graph_type = GRAPH_TYPE_GJB_S11;                s = "Admittance G,jB S11";      break;
			}

			if (graph_type < 0)
				continue;

			TMenuItem *menu_item;

			const bool column_break = (i == ((3 + GRAPH_TYPE_SIZE) / 2)) ? true : false;
			const bool line_break  = (graph_type == GRAPH_TYPE_SMITH_S11 || graph_type == GRAPH_TYPE_PHASE_VECTOR_S11 || graph_type == GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11 || graph_type == GRAPH_TYPE_GJB_S11) ? true : false;

			// add it to the graph popup menu
			if (column_break || line_break)
			{
				menu_item = new TMenuItem(GraphPopupMenu);
				menu_item->Caption = "-";
				menu_item->Tag     = -1;
				menu_item->Break   = column_break ? mbBreak : mbNone;
				menu_item->Enabled = false;
				menu_item->AutoLineReduction = maAutomatic;	//maManual
				Graphtype1->Add(menu_item);
			}
			menu_item = new TMenuItem(GraphPopupMenu);
			if (menu_item != NULL)
			{
				menu_item->OnClick       = MenuItemGraphTypeClick;
				menu_item->Caption       = s;
				menu_item->Tag           = graph_type;
				menu_item->RadioItem     = true;
				Graphtype1->Add(menu_item);
			}

			// add it to the right click popup menu
			if (column_break || line_break)
			{
				menu_item = new TMenuItem(GraphTypePopupMenu);
				menu_item->Caption = "-";
				menu_item->Tag     = -1;
				menu_item->Break   = column_break ? mbBreak : mbNone;
				menu_item->Enabled = false;
				menu_item->AutoLineReduction = maAutomatic;	//maManual
				GraphTypePopupMenu->Items->Add(menu_item);
			}
			menu_item = new TMenuItem(GraphTypePopupMenu);
			if (menu_item != NULL)
			{
				menu_item->OnClick       = MenuItemChangeGraphTypeClick;
				menu_item->Caption       = s;
				menu_item->Tag           = graph_type;
				menu_item->RadioItem     = true;
				GraphTypePopupMenu->Items->Add(menu_item);
			}
		}
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			int graph_type = -1;
			String s = "None";

			switch (i)
			{
				case  0: graph_type = GRAPH_TYPE_LOGMAG_S11; s = "Log mag"; break;
				case  1: graph_type = GRAPH_TYPE_LINMAG_S11; s = "Lin mag"; break;
			}

			TMenuItem *menu_item;

			// add it to the graph popup menu
			menu_item = new TMenuItem(GraphPopupMenu);
			if (menu_item != NULL)
			{
				menu_item->OnClick       = MenuItemGraphTypeClick;
				menu_item->Caption       = s;
				menu_item->Tag           = graph_type;
				menu_item->RadioItem     = true;
				Graphtype1->Add(menu_item);
			}

			menu_item = new TMenuItem(GraphTypePopupMenu);
			if (menu_item != NULL)
			{
				menu_item->OnClick       = MenuItemChangeGraphTypeClick;
				menu_item->Caption       = s;
				menu_item->Tag           = graph_type;
				menu_item->RadioItem     = true;
				GraphTypePopupMenu->Items->Add(menu_item);
			}
		}

		settings.graphArrangement = GRAPH_ARRANGE_1;

		settings.graphType[0] = GRAPH_TYPE_LOGMAG_S11;
		settings.graphType[1] = GRAPH_TYPE_LINMAG_S11;
	}
}

void __fastcall TGraphForm::saveBitmap(Graphics::TBitmap *bm, String ID, String dialog_title)
{
	if (bm == NULL)
		return;

	String filename;

	ID = ID.Trim();
	dialog_title = dialog_title.Trim();

	if (!ID.IsEmpty())
		filename += ID;

	if (!filename.IsEmpty())
		filename += "_";

	filename += FormatDateTime("yyyy-mm-dd_hh-nn-ss", Now()) + ".png";

	filename = common.cleanFilename(filename, false, true);

	SaveDialog1->Title    = dialog_title.IsEmpty() ? String("Save the image too ..") : dialog_title;
	SaveDialog1->FileName = filename;

	Application->NormalizeTopMosts();
	const bool ok = SaveDialog1->Execute();
	Application->RestoreTopMosts();

	if (!ok)
		return;

	filename = SaveDialog1->FileName;

	String ext = ExtractFileExt(filename).LowerCase();

	if (ext == ".bmp")
	{
		try
		{
			bm->SaveToFile(filename);
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
		return;
	}

	if (ext == ".jpg" || ext == ".jpeg")
	{
		TJPEGImage *jpg;
		try
		{
			jpg = new TJPEGImage;
			if (jpg == NULL)
				return;
			jpg->Assign(bm);
			jpg->CompressionQuality = 100;   // 0 to 100
			jpg->SaveToFile(filename);
		}
		__finally
		{
			if (jpg != NULL)
				delete jpg;
		}
		return;
	}

	if (ext == ".png")
	{
		TPngImage *png;
		try
		{
			png = new TPngImage();
			if (png == NULL)
				return;
			png->Assign(bm);
			png->CompressionLevel = 9;	// 0 to 9
			png->SaveToFile(filename);
		}
		__finally
		{
			if (png != NULL)
				delete png;
		}
		return;
	}

	Application->NormalizeTopMosts();
	Application->MessageBox(L"Only PNG, JPG, JPEG and BMP formats are supported", L"Error", MB_ICONERROR | MB_OK);
	Application->RestoreTopMosts();
}

void __fastcall TGraphForm::saveImage(const bool to_clipboard)
{
	if (graphs.m_graph_bm == NULL)
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"No image to save", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	String filename = data_unit.m_vna_data.name.LowerCase();

//	String sweep_name = SweepNameEdit->Text.Trim();

//	if (!sweep_name.IsEmpty())
//		filename += "_" + sweep_name;

	#if 0
		// add the graph type into the filename
		switch (m_graph_type)
		{
			case GRAPH_TYPE_LOGMAG_S11:              filename += "_LOGMAG_S11";             break;
			case GRAPH_TYPE_LOGMAG_S21:              filename += "_LOGMAG_S21";             break;
			case GRAPH_TYPE_LOGMAG_S11S21:           filename += "_LOGMAG_S11S21";          break;
			case GRAPH_TYPE_LINMAG_S11:              filename += "_LINMAG_S11";             break;
			case GRAPH_TYPE_LINMAG_S21:              filename += "_LINMAG_S21";             break;
			case GRAPH_TYPE_LINMAG_S11S21:           filename += "_LINMAG_S11S21";          break;
			case GRAPH_TYPE_PHASE_S11:               filename += "_PHASE_S11";              break;
			case GRAPH_TYPE_PHASE_S21:               filename += "_PHASE_S21";              break;
			case GRAPH_TYPE_PHASE_S11S21:            filename += "_PHASE_S11S21";           break;
			case GRAPH_TYPE_PHASE_UNWRAP_S11:        filename += "_PHASE_UNWRAP_S11";       break;
			case GRAPH_TYPE_PHASE_UNWRAP_S21:        filename += "_PHASE_UNWRAP_S21";       break;
			case GRAPH_TYPE_PHASE_UNWRAP_S11S21:     filename += "_PHASE_UNWRAP_S11S21";    break;
			case GRAPH_TYPE_GROUP_DELAY_S11:         filename += "_GROUP_DELAY_S11";        break;
			case GRAPH_TYPE_GROUP_DELAY_S21:         filename += "_GROUP_DELAY_S21";        break;
			case GRAPH_TYPE_GROUP_DELAY_S11S21:      filename += "_GROUP_DELAY_S11S21";     break;
			case GRAPH_TYPE_VSWR_S11:                filename += "_VSWR_S11";               break;
			case GRAPH_TYPE_IMPEDANCE_S11:           filename += "_IMPEDANCE_S11";          break;
			case GRAPH_TYPE_SERIES_RJX_S11:          filename += "_SERIES_RJX_S11";         break;
			case GRAPH_TYPE_PARALLEL_RJX_S11:        filename += "_PARALLEL_RJX_S11";       break;
			case GRAPH_TYPE_SERIES_RESISTANCE_S11:   filename += "_SERIES_RESISTANCE_S11";  break;
			case GRAPH_TYPE_SERIES_REACTANCE_S11:    filename += "_SERIES_REACTANCE_S11";   break;
			case GRAPH_TYPE_QUALITY_FACTOR_S11:      filename += "_QUALITY_FACTOR_S11";     break;
			case GRAPH_TYPE_SMITH_S11:               filename += "_SMITH_S11";              break;
			case GRAPH_TYPE_SMITH_S21:               filename += "_SMITH_S21";              break;
			case GRAPH_TYPE_ADMITTANCE_S11:          filename += "_ADMITTANCE_S11";         break;
			case GRAPH_TYPE_ADMITTANCE_S21:          filename += "_ADMITTANCE_S21";         break;
			case GRAPH_TYPE_POLAR_S11:               filename += "_POLAR_S11";              break;
			case GRAPH_TYPE_POLAR_S21:               filename += "_POLAR_S21";              break;
			case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:  filename += "_TDR_LIN_LP_IMPULSE_S11"; break;
			case GRAPH_TYPE_TDR_LIN_BP_S11:          filename += "_TDR_LIN_BP_S11";         break;
			case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11:  filename += "_TDR_LOG_LP_IMPULSE_S11"; break;
			case GRAPH_TYPE_TDR_LOG_BP_S11:          filename += "_TDR_LOG_BP_S11";         break;
			case GRAPH_TYPE_TDR_IMPEDANCE_S11:       filename += "_TDR_IMPEDANCE_S11";      break;
			case GRAPH_TYPE_REAL_IMAG_S11:           filename += "_REAL_IMAG_S11";          break;
			case GRAPH_TYPE_REAL_IMAG_S21:           filename += "_REAL_IMAG_S21";          break;
			case GRAPH_TYPE_SERIES_CAPACITANCE_S11:  filename += "_SERIES_CAPACITANCE_S11"; break;
			case GRAPH_TYPE_SERIES_INDUCTANCE_S11:   filename += "_SERIES_INDUCTANCE_S11";  break;
			case GRAPH_TYPE_COAX_LOSS_S11:           filename += "_COAX_LOSS_S11";          break;
			case GRAPH_TYPE_CAL_LOGMAG:              filename += "_CALIBRATIONS";           break;
			case GRAPH_TYPE_PHASE_VECTOR_S11:        filename += "_PHASE_VECTOR_S11";       break;
			case GRAPH_TYPE_PHASE_VECTOR_S21:        filename += "_PHASE_VECTOR_S21";       break;
			case GRAPH_TYPE_GJB_S11:                 filename += "_GJB_S11";               break;
		}
	#endif

	// add the date & time onto the image

	Graphics::TBitmap *bm1 = graphs.m_graph_bm;
	Graphics::TBitmap *bm2 = bm2 = new Graphics::TBitmap();
	if (bm2)
	{  // make a copy of the image but with extra height so as to add a title and date/time

		// ****************
		// date time string

		String datetime_s = FormatDateTime("ddd dd mmm yyyy hh:nn:ss", Now());

		// ***************

		bm2->Monochrome           = false;
		bm2->Transparent          = false;
		bm2->PixelFormat          = pf32bit;

		//bm2->Canvas->Font         = SweepNameFontLabel->Font;
//		bm2->Canvas->Font->Style  = bm2->Canvas->Font->Style << fsBold;
		//const TSize sz1           = bm2->Canvas->TextExtent(sweep_name);
//		bm2->Canvas->Font->Style  = bm2->Canvas->Font->Style >> fsBold;

		bm2->Canvas->Font         = bm1->Canvas->Font;
//		bm2->Canvas->Font->Style  = bm2->Canvas->Font->Style << fsBold;
		const TSize sz2           = bm2->Canvas->TextExtent(datetime_s);
//		bm2->Canvas->Font->Style  = bm2->Canvas->Font->Style >> fsBold;

		int y1 = 3;
		int y2 = y1;

//		if (!sweep_name.IsEmpty())
//			y2 += sz1.cy + 3;
		y2 += sz2.cy + 3;

		bm2->Width                = bm1->Width;
		bm2->Height               = bm1->Height + y2;

		// background colour
		bm2->Canvas->Brush->Style = bsSolid;
		bm2->Canvas->Brush->Color = settings.m_colours.background;
		bm2->Canvas->FillRect(bm2->Canvas->ClipRect);

		// draw the current graph image onto the new image
		bm2->Canvas->CopyMode     = cmSrcCopy;
		bm2->Canvas->Draw(0, y2, bm1);

		// remove the border gradient
		bm2->Canvas->Brush->Style = bsClear;
		bm2->Canvas->Pen->Color   = settings.m_colours.background;
		bm2->Canvas->Pen->Style   = psSolid;
		bm2->Canvas->Pen->Width   = 1;
		for (int i = 0; i < settings.borderWidth; i++)
			bm2->Canvas->Rectangle(i, y2 + i, bm1->Width - i, y2 + bm1->Height - i);

		// add the sweep name text onto the new image
		//if (!sweep_name.IsEmpty())
		//{
		//	bm2->Canvas->Font         = SweepNameFontLabel->Font;
		//	bm2->Canvas->Font->Color  = settings.m_colours.font;
		//	bm2->Canvas->Brush->Style = bsClear;
		//	bm2->Canvas->TextOutA((bm2->Width - sz1.cx) / 2, y1, sweep_name);
		//	y1 += sz1.cy + 3;
		//}

		// add the date//time text onto the new image
		bm2->Canvas->Font         = bm1->Canvas->Font;
		bm2->Canvas->Font->Color  = settings.m_colours.font;
		bm2->Canvas->Brush->Style = bsClear;
		bm2->Canvas->TextOutA((bm2->Width - sz2.cx) / 2, y1, datetime_s);
		//y1 += sz2.cy + 3;

		bm1 = bm2;
	}

	if (to_clipboard)
	{
		try
		{
			Clipboard()->Assign(bm1);		// copy the image to the clipboard
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
			//String s = exception.ToString();
		}
	}
	else
		saveBitmap(bm1, filename, "Save the graph image too ..");		// save the image to a file

	if (bm2 != NULL)
		delete bm2;
}

void __fastcall TGraphForm::GraphPaintBoxMouseDown(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	graphs.m_mouse.down_x = X;
	graphs.m_mouse.down_y = Y;

	graphs.m_mouse.x = X;
	graphs.m_mouse.y = Y;

	graphs.m_mouse.down_graph = graphs.m_mouse.graph;

	graphs.m_mouse.down_marker = -1;

	if (graphs.m_mouse.graph_v_scale >= 0)
	{	// start dragging the v-scale
		const int graph      = graphs.m_mouse.graph_v_scale;
		const int graph_type = m_graph_type;

		if (Shift.Contains(ssLeft))
		{
			graphs.m_mouse.graph_scale_pos_drag = graphs.m_mouse.graph_scale_pos;
			graphs.m_mouse.graph_v_scale_drag   = graph;

			if (graphs.isFrequencyGraph(graph_type) || graphs.isTDRGraph(graph_type))
			{
				if (graphs.m_mouse.down_y >= 0)
				{
					t_graph_setting *gs = &settings.m_graph_setting[graph_type];

					bool center_gamma = false;
					switch (graph_type)
					{
						case GRAPH_TYPE_LINMAG_S11:
						case GRAPH_TYPE_LINMAG_S21:
						case GRAPH_TYPE_LINMAG_S11S21:
						case GRAPH_TYPE_IMPEDANCE_S11:
						case GRAPH_TYPE_QUALITY_FACTOR_S11:
						case GRAPH_TYPE_VSWR_S11:
						case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
						case GRAPH_TYPE_TDR_LIN_BP_S11:
						case GRAPH_TYPE_TDR_IMPEDANCE_S11:
							center_gamma = true;
							break;
						default:
							break;
					}

					// disable auto mode - maybe
					bool disable_auto_mode = true;
					if (center_gamma && graphs.m_mouse.graph_scale_pos == 1)
						disable_auto_mode = false;
					if (disable_auto_mode)
					{
						if (gs->auto_max && graphs.m_mouse.graph_scale_pos_drag <= 1)
						{
							if (graphs.m_max[graph].index >= 0 && graphs.m_max[graph].value != graphs.m_min[graph].value)
								gs->max = graphs.m_max[graph].value;
							gs->auto_max = false;
						}
						if (gs->auto_min && graphs.m_mouse.graph_scale_pos_drag >= 1)
						{
							if (graphs.m_min[graph].index >= 0 && graphs.m_max[graph].value != graphs.m_min[graph].value)
								gs->min = graphs.m_min[graph].value;
							gs->auto_min = false;
						}
					}

					graphs.m_mouse.down_max = gs->max;
					graphs.m_mouse.down_min = gs->min;
					graphs.m_mouse.down_val = gs->gamma;
				}

				TCursor cursor = crHandPoint;
				if (pb->Cursor != cursor)
				{
					pb->Cursor = cursor;
					// force Windows to change the cursor
					pb->Parent->Perform(WM_SETCURSOR, (unsigned int)pb->Parent->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
				}
			}
		}
	}
	else
	if (graphs.m_mouse.graph_h_scale >= 0)
	{	// start dragging the h-scale
		const int graph      = graphs.m_mouse.graph_h_scale;
		const int graph_type = m_graph_type;

		if (Shift.Contains(ssLeft))
		{
			graphs.m_mouse.graph_scale_pos_drag = graphs.m_mouse.graph_scale_pos;
			graphs.m_mouse.graph_h_scale_drag   = graph;

			if (graphs.isFrequencyGraph(graph_type))
			{
				if (graphs.m_mouse.down_x >= 0)
				{
					graphs.m_mouse.down_Hz  = data_unit.m_freq_center_Hz;
					graphs.m_mouse.down_max = data_unit.m_freq_stop_Hz;
					graphs.m_mouse.down_min = data_unit.m_freq_start_Hz;
				}
			}
			else
			if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
			{
			}
			else
			if (graphs.isTDRGraph(graph_type))
			{
			}

			TCursor cursor = crHandPoint;
			if (pb->Cursor != cursor)
			{
				pb->Cursor = cursor;
				// force Windows to change the cursor
				pb->Parent->Perform(WM_SETCURSOR, (unsigned int)pb->Parent->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
			}
		}
	}
	else
	if (graphs.m_mouse.graph_type_select >= 0)
	{	// selecting a graph type
		const TPoint point = pb->ClientToScreen(TPoint(X, Y));
		GraphTypePopupMenu->Popup(point.X, point.Y);
	}
	else
	if (graphs.m_mouse.graph >= 0)
	{
		graphs.m_mouse.graph_scale_pos_drag = -1;
		graphs.m_mouse.graph_v_scale_drag   = -1;
		graphs.m_mouse.graph_h_scale_drag   = -1;
		graphs.m_mouse.graph_drag           = -1;

		const int graph_type = m_graph_type;

		graphs.m_mouse.down_marker = settings.m_graph_setting[graph_type].show_markers ? graphs.m_mouse.marker_index : -1;

		if (graphs.m_mouse.down_marker >= 0)
		{	// the user has clicked on a marker
/*
			if (Shift.Contains(ssLeft))
			{	// toggle it's selected state it if left mouse button clicked
				const int i = MarkerListBox->Items->IndexOfObject((TObject *)graphs.m_mouse.down_marker);
				if (i >= 0)
				{
					MarkerListBox->Selected[i] = !MarkerListBox->Selected[i];
					updateInfoPanel();
				}
			}
*/
			TCursor cursor = crHandPoint;
			if (pb->Cursor != cursor)
			{
				pb->Cursor = cursor;
				// force Windows to change the cursor
				pb->Parent->Perform(WM_SETCURSOR, (unsigned int)pb->Parent->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
			}

		}
		else
		{	// user has not clicked on a marker .. deselect all markers
//			for (int k = 0; k < MarkerListBox->Items->Count; k++)
//				MarkerListBox->Selected[k] = false;
//			updateInfoPanel();
		}

		if (graphs.isFrequencyGraph(graph_type))
		{
			if (graphs.m_mouse.down_x >= 0)
			{
				//m_graph_mouse_down_Hz = xyToFreq(m_mouse_down_graph, graph_type, m_graph_mouse_down_x, graph_y[m_mouse_down_graph]);
				graphs.m_mouse.down_Hz = data_unit.m_freq_center_Hz;

				//Label33->Caption = common.freqToStrMhz(m_graph_mouse_down_Hz);
			}
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
			if (graphs.m_mouse.down_marker >= 0)
				graphs.m_mouse.down_Hz = settings.m_markers_freq[graphs.m_mouse.down_marker].Hz;
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{

		}
	}

	pb->Invalidate();

	GraphPaintBox->Invalidate();
//	if (Application->MainForm)
//		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TGraphForm::GraphPaintBoxMouseUp(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	if (graphs.m_mouse.down_marker >= 0)
	{	// dragging a marker
		const int graph_type = m_graph_type;

		const int mi = graphs.m_mouse.down_marker;

		{	// delete the marker if it's been dragged off the graph
			const int64_t Hz = settings.m_markers_freq[mi].Hz;
			if (Hz < data_unit.m_freq_start_Hz || Hz > data_unit.m_freq_stop_Hz)
			{
				settings.m_markers_freq.erase(settings.m_markers_freq.begin() + mi);

				// deselect all markers
//				for (int k = 0; k < MarkerListBox->Items->Count; k++)
//					MarkerListBox->Selected[k] = false;
			}
		}

//		buildMarkerListBox();
//		ClearMarkersButton->Enabled = !settings.m_markers_freq.empty();

		if (graphs.isFrequencyGraph(graph_type))
		{
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}

	}

	graphs.m_mouse.graph_scale_pos_drag = -1;
	graphs.m_mouse.graph_v_scale_drag   = -1;
	graphs.m_mouse.graph_h_scale_drag   = -1;
	graphs.m_mouse.graph_drag           = -1;

	graphs.m_mouse.down_graph  = -1;
	graphs.m_mouse.down_x      = -1;
	graphs.m_mouse.down_y      = -1;
	graphs.m_mouse.down_marker = -1;

	pb->Cursor = crArrow;

	pb->Invalidate();

	GraphPaintBox->Invalidate();
//	if (Application->MainForm)
//		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TGraphForm::GraphPaintBoxMouseMove(TObject *Sender, TShiftState Shift,
			 int X, int Y)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	graphs.m_mouse.x = X;
	graphs.m_mouse.y = Y;

	TCursor cursor = crArrow;

	graphs.m_mouse.graph = -1;

	if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
	{
		graphs.m_mouse.graph_v_scale = -1;
		graphs.m_mouse.graph_h_scale = -1;
	}

	graphs.m_mouse.graph_type_select = -1;

	graphs.m_mouse.Hz           = -1;
	graphs.m_mouse.point_mem    = -1;
	graphs.m_mouse.point_index  = -1;
	graphs.m_mouse.time_mem     = -1;
	graphs.m_mouse.time_index   = -1;
	graphs.m_mouse.marker_mem   = -1;
	graphs.m_mouse.marker_index = -1;
	graphs.m_mouse.cpx          = 0;
	graphs.m_mouse.mag          = -1;

	const int smith_pixels = (m_screen_width > 0) ? IROUND(m_screen_width * 0.02f) : 20;
	const int pixels       = (m_screen_width > 0) ? IROUND(m_screen_width * 0.01f) : 10;

	{
		const int graph = 0;  // only one graph in this window

		const int gt = m_graph_type;
		const int gx = graphs.m_graph_pos[graph].gx;
		const int gy = graphs.m_graph_pos[graph].gy;
		const int gw = graphs.m_graph_pos[graph].gw;
		const int gh = graphs.m_graph_pos[graph].gh;
		const int cx = graphs.m_graph_pos[graph].cx;
		const int cy = graphs.m_graph_pos[graph].cy;
		const int gr = graphs.m_graph_pos[graph].cr;

		const int mx = graphs.m_mouse.x;
		const int my = graphs.m_mouse.y;

		if (mx >= (gx - 50) && mx <= (gx + 50) && my >= (gy - 30) && my <= gy)
		{	// graph type
			graphs.m_mouse.graph_type_select = graph;
		}
		else
		if (graphs.isFrequencyGraph(m_graph_type) || graphs.isTDRGraph(gt))
		{
			if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
			{
				if (mx >= (gx - 50) && mx <= (gx - 5) && my >= gy && my <= (gy + gh))
				{	// graph v-scale
					graphs.m_mouse.graph_v_scale   = graph;
					graphs.m_mouse.graph_scale_pos = ((my - gy) * 3) / gh;	// 0 to 2 (top/mid/bot)
				}
				else
				if (mx >= gx && mx <= (gx + gw) && my >= (gy + gh + 5) && my <= (gy + gh + 30))
				{	// graph h-scale
					graphs.m_mouse.graph_h_scale   = graph;
					graphs.m_mouse.graph_scale_pos = ((mx - gx) * 3) / gw;	// 0 to 2 (left/mid/right)
				}
				else
				if (mx >= gx && mx <= (gx + gw) && my >= (gy - 10) && my <= (gy + gh + 10))
				{	// graph
					graphs.m_mouse.graph = graph;
				}
			}
			else
			if (mx >= gx && mx <= (gx + gw) && my >= (gy - 10) && my <= (gy + gh + 10))
			{	// graph
				graphs.m_mouse.graph = graph;
			}
		}
		else
		if (graphs.isSmithGraph(gt) || graphs.isAdmittanceGraph(gt) || graphs.isPolarGraph(gt))
		{
			const int dx = (mx - cx);
			const int dy = (cy - my);
			const int dc = IROUND(sqrtf((dx * dx) + (dy * dy)));
			if (dc <= (gr + smith_pixels))   // more than 'smith_pixels' outside the smith chart ?
			{
				graphs.m_mouse.graph = graph;
			}
		}
	}

	// ************************************
	// compute the mouse position details - sweep point index, frequency and marker index

	if (graphs.m_mouse.graph >= 0)
	{
		const int graph      = graphs.m_mouse.graph;
		const int graph_type = m_graph_type;

		const int gx = graphs.m_graph_pos[graph].gx;
		//const int gy = graphs.m_graph_pos[graph].gy;
		const int gw = graphs.m_graph_pos[graph].gw;
		//const int gh = graphs.m_graph_pos[graph].gh;
		const int cx = graphs.m_graph_pos[graph].cx;
		const int cy = graphs.m_graph_pos[graph].cy;
		const int gr = graphs.m_graph_pos[graph].cr;

		const int mx = graphs.m_mouse.x;
		const int my = graphs.m_mouse.y;

		const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

		int     m_graph   = -1;
		int     m_mem     = -1;
		int     m_channel = -1;
		int     m_index   = -1;
		int64_t m_Hz      = -1;
		double  m_secs    = -1;

		if (graphs.isFrequencyGraph(graph_type))
		{
			if (my >= graphs.m_graph_pos[graph].gy && my < (graphs.m_graph_pos[graph].gy + graphs.m_graph_pos[graph].gh))
			{
				const bool near_point = settings.snapToNearestPoint ? graphs.nearestPoint(graph, graph_type, mx, my, pixels, m_graph, m_mem, m_channel, m_index, m_Hz, m_secs) : false;

				if (!near_point)
				{	// use the mouse X position and the first valid memory
					m_Hz = graphs.xyToFreq(graph, graph_type, mx, my);

					int mem   = -1;
					int index = -1;

					if (graph_type == GRAPH_TYPE_CAL_LOGMAG)
					{	// calibration graph
						mem = -2;
						index = data_unit.indexFreq(m_Hz, mem);
					}
					else
					{
						while (mem < MAX_MEMORIES && index < 0)
						{
							mem = data_unit.firstUsedMem(true, ++mem);
							if (mem < 0)
								break;
							index = data_unit.indexFreq(m_Hz, mem);
						}

						if (mem >= 0 && mem < MAX_MEMORIES && index >= 0 && index < (int)data_unit.m_point_filt[mem].size())
						{
							m_index = index;
							m_Hz    = data_unit.m_point_filt[mem][m_index].Hz;	// snap to the sweep point frequency
						}
					}
				}

				graphs.m_mouse.Hz          = m_Hz;
				graphs.m_mouse.point_mem   = m_mem;
				graphs.m_mouse.point_index = m_index;

				if (graph_type != GRAPH_TYPE_CAL_LOGMAG)
				{	// memories

					if (gs && gs->show_markers)
					{
						if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
						{	// find the nearest marker

							int m_mem   = -1;
							int m_index = -1;
							int m_dist  = -1;
							for (int i = 0; i < (int)settings.m_markers_freq.size(); i++)
							{
								const t_marker_freq marker = settings.m_markers_freq[i];
								for (int m = 0; m < MAX_MEMORIES; m++)
								{
									if (settings.memoryEnable[m])
									{
										const int mi     = data_unit.indexFreq(marker.Hz, m);	// snap the nearest sweep point
										const int64_t Hz = data_unit.getFrequency(m, mi);
										const int kx     = graphs.freqToX(graph, graph_type, Hz);
										const int dx     = ABS(kx - mx);
										if (m_index < 0 || m_dist > dx)
										{	// found a closer marker
											m_mem   = m;
											m_index = i;
											m_dist  = dx;
										}
									}
								}
							}
							if (m_index >= 0 && m_dist >= 0)
							{
								if (m_dist <= pixels)	// within 'pixels' of the nearest marker ?
								{
									graphs.m_mouse.marker_mem   = m_mem;
									graphs.m_mouse.marker_index = m_index;
								}
                     }
						}
					}
				}
			}

		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
			const int dx = mx - cx;
			const int dy = cy - my;
			const int cd = IROUND(sqrtf((dx * dx) + (dy * dy)));	// distance (in pixels) from center of chart

			if (cd <= gr)
			{
				graphs.m_mouse.cpx = complexf ((float)dx / gr, (float)dy / gr);	// -1 to +1
				graphs.m_mouse.mag = sqrtf((graphs.m_mouse.cpx.real() * graphs.m_mouse.cpx.real()) + (graphs.m_mouse.cpx.imag() * graphs.m_mouse.cpx.imag()));
			}

			if (cd <= (gr + pixels))
			{
				const bool near_point = settings.snapToNearestPoint ? graphs.nearestPoint(graph, graph_type, mx, my, pixels, m_graph, m_mem, m_channel, m_index, m_Hz, m_secs) : false;

				if (near_point)
				{
					graphs.m_mouse.point_mem   = m_mem;
					graphs.m_mouse.point_index = m_index;
					graphs.m_mouse.Hz          = m_Hz;
				}

				if (gs && gs->show_markers)
				{
					if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
					{	// find the nearest marker
						const int g = graph;
						const int c = 0;
						int m_mem   = -1;
						int m_index = -1;
						int m_dist  = -1;
						for (int i = 0; i < (int)settings.m_markers_freq.size(); i++)
						{
							const t_marker_freq marker = settings.m_markers_freq[i];
							for (int m = 0; m < MAX_MEMORIES; m++)
							{
								if (settings.memoryEnable[m])
								{
									const int p_index = data_unit.indexFreq(marker.Hz, m);
									if (p_index >= 0 && p_index < (int)graphs.m_line_points[g][m][c].size())
									{
										const int dx = graphs.m_line_points[g][m][c][p_index].x - mx;
										const int dy = graphs.m_line_points[g][m][c][p_index].y - my;
										const int dp = (dx * dx) + (dy * dy);
										if (m_index < 0 || m_dist > dp)
										{	// found a closer line point
											//m_graph = g;
											m_mem   = m;
											//m_chan  = c;
											m_index = i;
											m_dist  = dp;
										}
									}
								}
							}
						}
						if (m_mem >= 0 && m_index >= 0 && m_dist >= 0)
						{
							m_dist = IROUND(sqrtf(m_dist));	// now in pixels
							if (m_dist <= pixels)	// within 'pixels' of the nearest marker ?
							{
								graphs.m_mouse.marker_mem   = m_mem;
								graphs.m_mouse.marker_index = m_index;
							}
						}
					}
				}
			}

		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
			int mem = data_unit.firstUsedMem(true, 0);
			if (mem < 0)
				mem = 0;

			const float mf = (float)(mx - gx) / gw;	// 0.0 to 1.0

			const double freq_step = data_unit.freq_step(-1);

			if (mf >= 0.0f && mf <= 1.0f && freq_step > 0)
			{
				const double max_time = data_unit.max_time(freq_step);
				const double mt = max_time * mf;

//				const double mem_freq_step = data_unit.freq_step(mem);
//				const double mem_max_time  = data_unit.max_time(mem_freq_step);

				const int size = graphs.m_fft[graph][mem].size() / 2;

				int x = (size > 1) ? IROUND((size * mt) / max_time) : -1;

				const bool near_point = settings.snapToNearestPoint ? graphs.nearestPoint(graph, graph_type, mx, my, pixels, m_graph, m_mem, m_channel, m_index, m_Hz, m_secs) : false;

				if (!near_point)
				{	// use the mouse X position and the first valid memory
//					if (mem < MAX_MEMORIES && size > 0 && x >= 0 && x < size)
					if (mem < MAX_MEMORIES)
					{
						m_graph   = graph;
						m_index   = x;
						m_secs    = max_time * mf;
					}
				}

				graphs.m_mouse.point_mem   = m_mem;
				graphs.m_mouse.point_index = m_index;
				graphs.m_mouse.Hz          = 0;
				graphs.m_mouse.secs        = m_secs;
			}
		}
	}

	// ************************************
	// dragging a marker

	if (graphs.m_mouse.down_marker >= 0)
	{
		//const int graph      = graphs.m_mouse.down_graph;
		const int graph_type = m_graph_type;

		cursor = crHandPoint;

		if (graphs.isFrequencyGraph(graph_type))
		{
			settings.m_markers_freq[graphs.m_mouse.down_marker].Hz = graphs.m_mouse.Hz;
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
			settings.m_markers_freq[graphs.m_mouse.down_marker].Hz = graphs.m_mouse.Hz;
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}
	}

	// ************************************
	// dragging v-scale

	if (graphs.m_mouse.graph_v_scale_drag >= 0)
	{
		cursor = crHandPoint;

		const int pos        = graphs.m_mouse.graph_scale_pos_drag;
		const int graph      = graphs.m_mouse.graph_v_scale_drag;
		const int graph_type = m_graph_type;

		if (graphs.isFrequencyGraph(graph_type) || graphs.isTDRGraph(graph_type))
		{
			t_graph_setting *gs = &settings.m_graph_setting[graph_type];

			double range = graphs.m_mouse.down_max - graphs.m_mouse.down_min;
         range *= 2.0;
			if (range < 1e-14)
				range = 1e-14;

			const double delta = ((double)(graphs.m_mouse.y - graphs.m_mouse.down_y) * range) / graphs.m_graph_pos[graph].gh;

			switch (graph_type)
			{
				case GRAPH_TYPE_LOGMAG_S11:
				case GRAPH_TYPE_LOGMAG_S21:
				case GRAPH_TYPE_LOGMAG_S11S21:
				case GRAPH_TYPE_PHASE_S11:
				case GRAPH_TYPE_PHASE_S21:
				case GRAPH_TYPE_PHASE_S11S21:
				case GRAPH_TYPE_PHASE_UNWRAP_S11:
				case GRAPH_TYPE_PHASE_UNWRAP_S21:
				case GRAPH_TYPE_PHASE_UNWRAP_S11S21:
				case GRAPH_TYPE_SERIES_RJX_S11:
				case GRAPH_TYPE_PARALLEL_RJX_S11:
				case GRAPH_TYPE_GJB_S11:
				case GRAPH_TYPE_SERIES_REACTANCE_S11:
				case GRAPH_TYPE_GROUP_DELAY_S11:
				case GRAPH_TYPE_GROUP_DELAY_S21:
				case GRAPH_TYPE_GROUP_DELAY_S11S21:
				case GRAPH_TYPE_REAL_IMAG_S11:
				case GRAPH_TYPE_REAL_IMAG_S21:
				case GRAPH_TYPE_SERIES_CAPACITANCE_S11:
				case GRAPH_TYPE_SERIES_INDUCTANCE_S11:
				case GRAPH_TYPE_COAX_LOSS_S11:
				case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11:
				case GRAPH_TYPE_TDR_LOG_BP_S11:
				case GRAPH_TYPE_SERIES_RESISTANCE_S11:
            case GRAPH_TYPE_CAL_LOGMAG:
					switch (pos)
					{
						case 0:	// dragging top section
							gs->max = graphs.m_mouse.down_max + delta;
							break;
						case 1:	// dragging mid section
							gs->max = graphs.m_mouse.down_max + delta;
							gs->min = graphs.m_mouse.down_min + delta;
							break;
						case 2:	// dragging bot section
							gs->min = graphs.m_mouse.down_min + delta;
							break;
					}
					break;

				case GRAPH_TYPE_LINMAG_S11:
				case GRAPH_TYPE_LINMAG_S21:
				case GRAPH_TYPE_LINMAG_S11S21:
				case GRAPH_TYPE_IMPEDANCE_S11:
				case GRAPH_TYPE_QUALITY_FACTOR_S11:
				case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
				case GRAPH_TYPE_TDR_LIN_BP_S11:
				case GRAPH_TYPE_TDR_IMPEDANCE_S11:
					switch (pos)
					{
						case 0:	// dragging top section
							gs->max = graphs.m_mouse.down_max + delta;
							break;
						case 1:	// dragging mid section
							#if 0
								gs->max = graphs.m_mouse.down_max + delta;
								gs->min = graphs.m_mouse.down_min + delta;
							#else
								{	// modify the gamma (graph non-linear scale)
									const double diff = delta / range;
									double gamma = graphs.m_mouse.down_val - (diff * 10);
									if (gamma < GAMMA_MIN) gamma = GAMMA_MIN;
									else
									if (gamma > GAMMA_MAX) gamma = GAMMA_MAX;
									gs->gamma = gamma;
								}
							#endif
							break;
						case 2:	// dragging bot section
							gs->min = graphs.m_mouse.down_min + delta;
							break;
					}
					break;

				case GRAPH_TYPE_VSWR_S11:
					switch (pos)
					{
						case 0:	// dragging top section
							gs->max = graphs.m_mouse.down_max + delta;
							break;
						case 1:	// dragging mid section .. alter the gamma rather than the min/max
						//case 2:	// dragging bot section
							{
								const double diff = delta / range;
								double gamma = graphs.m_mouse.down_val - (diff * 10);
								if (gamma < GAMMA_MIN) gamma = GAMMA_MIN;
								else
								if (gamma > GAMMA_MAX) gamma = GAMMA_MAX;
								gs->gamma = gamma;
							}
							break;
						case 2:	// dragging bot section
							gs->min = graphs.m_mouse.down_min + delta;
							break;
					}
					//gs->min = VSWR_MIN;
					break;

				case GRAPH_TYPE_SMITH_S11:
				case GRAPH_TYPE_SMITH_S21:
				case GRAPH_TYPE_ADMITTANCE_S11:
				case GRAPH_TYPE_ADMITTANCE_S21:
				case GRAPH_TYPE_POLAR_S11:
				case GRAPH_TYPE_POLAR_S21:
					break;

				case GRAPH_TYPE_PHASE_VECTOR_S11:
				case GRAPH_TYPE_PHASE_VECTOR_S21:
					break;

				default:
					break;
			}

			// sanity check
			settings.clipGraphMinMax(graph_type, gs->max, gs->min);
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
		}
	}

	// ************************************
	// dragging h-scale

	if (graphs.m_mouse.graph_h_scale_drag >= 0)
	{
		cursor = crHandPoint;

		const int pos        = graphs.m_mouse.graph_scale_pos_drag;
		const int graph      = graphs.m_mouse.graph_h_scale_drag;
		const int graph_type = m_graph_type;

		if (graphs.isFrequencyGraph(graph_type))
		{
			const double range_Hz = graphs.m_mouse.down_max - graphs.m_mouse.down_min;
			const double delta_Hz = ((double)(graphs.m_mouse.x - graphs.m_mouse.down_x) * range_Hz) / graphs.m_graph_pos[graph].gw;

			double start_Hz = graphs.m_mouse.down_Hz - (range_Hz / 2);
			double stop_Hz  = graphs.m_mouse.down_Hz + (range_Hz / 2);

			switch (pos)
			{
				case 0:	// dragging left section
					start_Hz -= delta_Hz;
					if (start_Hz < min_Hz)
						 start_Hz = min_Hz;
					else
					if (start_Hz > (stop_Hz - 1000))
						 start_Hz =  stop_Hz - 1000;
					break;

				case 1:	// dragging mid section
					start_Hz -= delta_Hz;
					if (start_Hz < min_Hz)
						 start_Hz = min_Hz;
					else
					if (start_Hz > (max_Hz - range_Hz))
						 start_Hz =  max_Hz - range_Hz;

					stop_Hz -= delta_Hz;
					if (stop_Hz < (min_Hz + range_Hz))
						 stop_Hz =  min_Hz + range_Hz;
					else
					if (stop_Hz > max_Hz)
						 stop_Hz = max_Hz;
					break;

				case 2:	// dragging right section
					stop_Hz -= delta_Hz;
					if (stop_Hz < (start_Hz + 1000))
						 stop_Hz =  start_Hz + 1000;
					else
					if (stop_Hz > max_Hz)
						 stop_Hz = max_Hz;
					break;
			}

			if (Form1)
				Form1->setStartStopHz(I64ROUND(start_Hz), I64ROUND(stop_Hz), false);
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}
	}

	// ************************************
	// choose the mouse cursor

	if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
	{
		if (graphs.m_mouse.graph_v_scale >= 0)
		{
			switch (graphs.m_mouse.graph_scale_pos)
			{
				case 0:	// dragging top section
					cursor = crHandPoint;
					//cursor = crSizeNS;
					break;
				case 1:	// dragging mid section
					cursor = crHandPoint;
					//cursor = crVSplit;
					break;
				case 2:	// dragging bot section
					cursor = crHandPoint;
					//cursor = crSizeNS;
					break;
			}
		}
		else
		if (graphs.m_mouse.graph_h_scale >= 0)
		{
			switch (graphs.m_mouse.graph_scale_pos)
			{
				case 0:	// dragging left section
					cursor = crHandPoint;
					//cursor = crSizeWE;
					break;
				case 1:	// dragging mid section
					cursor = crHandPoint;
					//cursor = crHSplit;
					break;
				case 2:	// dragging right section
					cursor = crHandPoint;
					//cursor = crSizeWE;
					break;
			}
		}
		else
		if (graphs.m_mouse.graph_type_select >= 0)
		{
			cursor = crHandPoint;
		}
		else
		if (graphs.m_mouse.graph >= 0)
		{
//			const int graph      = graphs.m_mouse.graph;
			const int graph_type = m_graph_type;
/*
			const int gx = graphs.m_graph_pos[graph].gx;
			const int gy = graphs.m_graph_pos[graph].gy;
			const int gw = graphs.m_graph_pos[graph].gw;
			const int gh = graphs.m_graph_pos[graph].gh;
			const int cx = graphs.m_graph_pos[graph].cx;
			const int cy = graphs.m_graph_pos[graph].cy;
			const int gr = graphs.m_graph_pos[graph].cr;

			const int mx = graphs.m_mouse.x;
			const int my = graphs.m_mouse.y;
*/
			if (graphs.m_mouse.marker_index >= 0)
			{
				cursor = crHandPoint;
			}
			else
			{
				if (graphs.isFrequencyGraph(graph_type))
				{
//					if (mx >= gx && mx < (gx + gw) && my >= gy && my < (gy + gh))
//						if (graphs.m_mouse.point_index < 0)
//							cursor = crCross;
				}
				else
				if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
				{
//					const int dx = mx - cx;
//					const int dy = cy - my;
//					const int dc = IROUND(sqrt((dx * dx) + (dy * dy)));	// distance from center
//					if (dc <= gr)
//						if (graphs.m_mouse.point_index < 0)
//							cursor = crCross;
				}
				else
				if (graphs.isTDRGraph(graph_type))
				{
//					if (mx >= gx && mx < (gx + gw) && my >= gy && my < (gy + gh))
//					//if (graphs.m_mouse.time_index < 0)
//						if (graphs.m_mouse.point_index < 0)
//							cursor = crCross;
				}
			}
		}
	}

	// ************************************

	if (pb->Cursor != cursor)
	{
		pb->Cursor = cursor;
		// force Windows to change the cursor
		pb->Parent->Perform(WM_SETCURSOR, (unsigned int)pb->Parent->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
	}

//	updateInfoPanel();

	pb->Refresh();

	GraphPaintBox->Invalidate();
//	if (Application->MainForm)
//		::SendMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TGraphForm::GraphPaintBoxMouseEnter(TObject *Sender)
{
	GraphPaintBox->Invalidate();
}

void __fastcall TGraphForm::GraphPaintBoxMouseLeave(TObject *Sender)
{
//	if (graphs.m_mouse.down_marker >= 0)
	{
		graphs.m_mouse.marker_index = -1;
		graphs.m_mouse.graph        = -1;
		graphs.m_mouse.Hz           = -1;
		graphs.m_mouse.point_mem    = -1;
		graphs.m_mouse.point_index  = -1;
		graphs.m_mouse.time_mem     = -1;
		graphs.m_mouse.time_index   = -1;
		graphs.m_mouse.x            = -1;
		graphs.m_mouse.y            = -1;
		graphs.m_mouse.cpx          = 0;
		graphs.m_mouse.mag          = -1;
	}

	if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0)
	{
		graphs.m_mouse.graph_scale_pos = -1;
		graphs.m_mouse.graph_v_scale   = -1;
		graphs.m_mouse.graph_h_scale   = -1;
		graphs.m_mouse.graph_drag      = -1;
	}

	GraphPaintBox->Invalidate();
	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TGraphForm::GraphPaintBoxPaint(TObject *Sender)
{
	#ifndef USE_OPENGL
		graphs.onPaint(Sender,
							false,
							m_graph_type,
							GRAPH_TYPE_LOGMAG_S11S21,
							GRAPH_TYPE_LOGMAG_S11S21,
							GRAPH_TYPE_LOGMAG_S11S21,
							1,
							0);
   #endif
}

void __fastcall TGraphForm::GraphPaintBoxDblClick(TObject *Sender)
{
	if (Form1)
	{
		if (graphs.m_mouse.marker_index >= 0)
		{	// delete the marker
			Form1->deleteMarker(graphs.m_mouse.marker_index);
			graphs.m_mouse.marker_index = -1;
		}
		else
		if (graphs.m_mouse.Hz > 0)
		{	// add a marker
			Form1->addMarker(graphs.m_mouse.Hz, MARKER_TYPE_NORMAL, -1, -1);
		}
	}
}

void __fastcall TGraphForm::CloseBitBtnClick(TObject *Sender)
{
	Close();
}

void __fastcall TGraphForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	switch (Key)
	{
		case VK_ESCAPE:
			Key = 0;
			this->Hide();
			break;
	}
}

void __fastcall TGraphForm::FormMouseEnter(TObject *Sender)
{
//	this->SetFocus();
}

void __fastcall TGraphForm::FormMouseWheel(TObject *Sender, TShiftState Shift,
          int WheelDelta, TPoint &MousePos, bool &Handled)
{
	TPoint pos = ScreenToClient(MousePos);

	TControl *control = ControlAtPos(pos, false, true, true);
	if (control == NULL)
		return;

//	if (control != GraphPaintBox)
//		return;

	Handled = true;

	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	if (graphs.m_mouse.graph_type_select >= 0)
	{	// graph type

		Handled = true;

		if (WheelDelta > 0)
		{
			settings.graphType[graphs.m_mouse.graph_type_select] = settings.graphType[graphs.m_mouse.graph_type_select] - 1;
			GraphPaintBox->Invalidate();
		}
		else
		if (WheelDelta < 0)
		{
			settings.graphType[graphs.m_mouse.graph_type_select] = settings.graphType[graphs.m_mouse.graph_type_select] + 1;
			GraphPaintBox->Invalidate();
		}
	}
	else
	if (graphs.m_mouse.graph_h_scale >= 0)
	{	// h-scale zoom
		//const int graph      = graphs.m_mouse.graph_h_scale;
		const int graph_type = m_graph_type;

		if (graphs.isFrequencyGraph(graph_type))
		{
			if (WheelDelta != 0)
			{	// zoom frequency in/out

				const double scale = (WheelDelta < 0) ? 1.2 : (WheelDelta > 0) ? 1.0 / 1.2 : 0;

				int64_t center_Hz = data_unit.m_freq_center_Hz;
				int64_t span_Hz   = I64ROUND(data_unit.m_freq_span_Hz * scale);

				int64_t start_Hz = center_Hz - (span_Hz / 2);
				int64_t stop_Hz  = center_Hz + (span_Hz / 2);

				if (start_Hz < min_Hz)
					span_Hz = (center_Hz - min_Hz) * 2;
				if (stop_Hz > max_Hz)
					span_Hz = (max_Hz - center_Hz) * 2;

				if (span_Hz < 10000)
					span_Hz = 10000;

				span_Hz = I64ROUND((double)span_Hz / 1000) * 1000;	// 1kHz rounding

				start_Hz = center_Hz - (span_Hz / 2);
				stop_Hz  = center_Hz + (span_Hz / 2);

				if (Form1)
					Form1->setStartStopHz(start_Hz, stop_Hz, false);

				GraphPaintBox->Refresh();
			}
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}
	}
	else
	if (graphs.m_mouse.graph_v_scale >= 0)
	{	// v-scale zoom
		const int graph      = graphs.m_mouse.graph_v_scale;
		const int graph_type = settings.graphType[graph];

		if (graphs.isFrequencyGraph(graph_type) || graphs.isTDRGraph(graph_type))
		{
			if (WheelDelta != 0)
			{	// zoom level in/out

				Handled = true;

				const double scale = (WheelDelta < 0) ? 1.1 : (WheelDelta > 0) ? 1.0 / 1.1 : 0;

				t_graph_setting *gs = &settings.m_graph_setting[graph_type];

				// disable auto mode
				if (gs->auto_max)
				{
					if (graphs.m_max[graph].index >= 0 && graphs.m_max[graph].value != graphs.m_min[graph].value)
						gs->max = graphs.m_max[graph].value;
					gs->auto_max = false;
				}
				if (gs->auto_min)
				{
					if (graphs.m_min[graph].index >= 0 && graphs.m_max[graph].value != graphs.m_min[graph].value)
						gs->min = graphs.m_min[graph].value;
					gs->auto_min = false;
				}

				const double center = (gs->min + gs->max) / 2;

				double range = (gs->max - gs->min) * scale;
				if (range < 1e-13)
					 range = 1e-13;

				switch (graph_type)
				{
					case GRAPH_TYPE_VSWR_S11:
						if (range > (VSWR_MAX - VSWR_MIN)) range = VSWR_MAX - VSWR_MIN;
						else
						if (range < 1.0) range = 1.0;
						gs->max = center + (range / 2);
						gs->min = center - (range / 2);
						//gs->min = VSWR_MIN;
						break;
					default:
						gs->max = center + (range / 2);
						gs->min = center - (range / 2);
						break;
				}

				settings.clipGraphMinMax(graph_type, gs->max, gs->min);

				if (Application->MainForm)
					::SendMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
			}
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
		}
	}
}

void __fastcall TGraphForm::PopupMeasureItem(System::TObject *Sender, Vcl::Graphics::TCanvas *ACanvas, int &Width, int &Height)
{
	if (ACanvas)
		Height = ACanvas->TextHeight("Hq") + POPUP_MENU_LINE_PADDING;
}

void __fastcall TGraphForm::MenuItemGraphTypeClick(TObject *Sender)
{
	TMenuItem *menu_item = dynamic_cast<TMenuItem *>(Sender);
	if (menu_item == NULL)
		return;

	const int graph_type = menu_item->Tag;
	if (graphs.m_mouse.graph >= 0 && graph_type >= 0 && graph_type < GRAPH_TYPE_SIZE)
		m_graph_type = graph_type;

	m_popup_menu_mouse_graph = -1;

	GraphPaintBox->Invalidate();
}

void __fastcall TGraphForm::MenuItemChangeGraphTypeClick(TObject *Sender)
{
	TMenuItem *menu_item = dynamic_cast<TMenuItem *>(Sender);
	if (menu_item == NULL)
		return;

	const int graph_type = menu_item->Tag;
	if (m_popup_menu_graph_type_graph >= 0 && graph_type >= 0 && graph_type < GRAPH_TYPE_SIZE)
		m_graph_type = graph_type;

	m_popup_menu_graph_type_graph = -1;

	GraphPaintBox->Invalidate();
}

void __fastcall TGraphForm::GraphPopupMenuPopup(TObject *Sender)
{
	m_popup_menu_mouse_Hz     = graphs.m_mouse.Hz;
	m_popup_menu_mouse_marker = graphs.m_mouse.marker_index;
	m_popup_menu_mouse_graph  = graphs.m_mouse.graph;

	if (m_popup_menu_mouse_graph < 0 && graphs.m_mouse.graph_v_scale >= 0)
		 m_popup_menu_mouse_graph = graphs.m_mouse.graph_v_scale;

	t_marker_freq marker_freq;
	t_marker_time marker_time;

	if (m_popup_menu_mouse_marker >= 0)
	{
		marker_freq = settings.m_markers_freq[m_popup_menu_mouse_marker];
	}
	else
	{
		marker_freq.Hz   = -1;
		marker_freq.type = MARKER_TYPE_NORMAL;
	}

//	const int graph           = m_popup_menu_mouse_graph;
	const int graph_type      = m_graph_type;
	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

//	const bool freq_mode  = graphs.isFrequencyGraph(graph_type);
//	const bool smith_mode = graphs.isSmithGraph(graph_type) || isAdmittanceGraph(graph_type) || isPolarGraph(graph_type);
	const bool tdr_mode   = graphs.isTDRGraph(graph_type);

	Enablespline1->Checked       = settings.splineEnable;
	Showpoints1->Checked         = settings.showPoints;
	Cliptraces1->Checked         = settings.clipTraces;
	Snaptonearestpoint1->Checked = settings.snapToNearestPoint;

	Graphtype1->Enabled = (m_popup_menu_mouse_graph >= 0) ? true : false;
	if (m_popup_menu_mouse_graph >= 0)
	{
		for (int i = 0; i < Graphtype1->Count; i++)
		{
			if (Graphtype1->Items[i]->Tag == graph_type)
			{
				Graphtype1->Items[i]->Checked = true;
				break;
			}
		}
	}

	Addmarker1->Enabled       = !tdr_mode;
	Deletemarker1->Enabled    = (m_popup_menu_mouse_marker >= 0) ? true : false;

	Deltamarker2->Enabled     = (m_popup_menu_mouse_marker >= 0) ? true : false;
	Deltamarker2->Checked     = (marker_freq.type == MARKER_TYPE_DELTA) ? true : false;
/*
	Markergraph1->Enabled     = false;
	//Markergraph1->Enabled     = (m_popup_menu_mouse_marker >= 0) ? true : false;
	if (m_popup_menu_mouse_marker >= 0)
	{
		const int mg = settings.m_markers_freq[m_popup_menu_mouse_marker].graph;
		Allgraphs1->Checked = (mg  < 0) ? true : false;
		Graph11->Checked    = (mg == 0) ? true : false;
		Graph21->Checked    = (mg == 1) ? true : false;
		Graph31->Checked    = (mg == 2) ? true : false;
		Graph41->Checked    = (mg == 3) ? true : false;
	}
*/
	Markertrace1->Enabled     = false;
	//Markertrace1->Enabled     = (m_popup_menu_mouse_marker >= 0) ? true : false;
	if (m_popup_menu_mouse_marker >= 0)
	{
		const int mt = settings.m_markers_freq[m_popup_menu_mouse_marker].trace;
		Alltraces1->Checked = (mt  < 0) ? true : false;
		Trace11->Checked    = (mt == 0) ? true : false;
		Trace21->Checked    = (mt == 1) ? true : false;
	}

	{
		Showmarkers1->Checked     = (gs != NULL && gs->show_markers) ? true : false;
		Showminmaxtext1->Checked  = (gs != NULL && gs->show_min_max) ? true : false;
		Showmaxmarker1->Checked   = (gs != NULL && gs->show_max_marker) ? true : false;
		Showminmarker1->Checked   = (gs != NULL && gs->show_min_marker) ? true : false;

		Showmarkers1->Enabled     = (gs != NULL) ? true : false;
		Showminmaxtext1->Enabled  = (gs != NULL) ? true : false;
		Showmaxmarker1->Enabled   = (gs != NULL) ? true : false;
		Showminmarker1->Enabled   = (gs != NULL) ? true : false;
	}

	if (m_popup_menu_mouse_Hz > 0)
	{
		String s = common.freqToStr1(m_popup_menu_mouse_Hz, true, true, 6, false);

		Setasstartfrequency1->Caption  = "Set as start frequency [" + s + "]";
		Setascenterfrequency1->Caption = "Set as center frequency [" + s + "]";
		Setasstopfrequency1->Caption   = "Set as stop frequency [" + s + "]";

		Setasstartfrequency1->Enabled  = true;
		Setasstopfrequency1->Enabled   = true;
		Setascenterfrequency1->Enabled = true;
	}
	else
	{
		Setasstartfrequency1->Caption  = "Set as start frequency";
		Setascenterfrequency1->Caption = "Set as center frequency";
		Setasstopfrequency1->Caption   = "Set as stop frequency";

		Setasstartfrequency1->Enabled  = false;
		Setasstopfrequency1->Enabled   = false;
		Setascenterfrequency1->Enabled = false;
	}

	if (gs == NULL)
	{
		Automaxscale1->Enabled = false;
		Autominscale1->Enabled = false;

		Automaxscale1->Checked = false;
		Autominscale1->Checked = false;

		Setmaxscale1->Enabled  = false;
		Setminscale1->Enabled  = false;

		Setmaxscale1->Caption = "Set max scale";
		Setminscale1->Caption = "Set min scale";
	}
	else
	{
		bool enable_max_option = true;
		bool enable_min_option = true;

		switch (graph_type)
		{
			case GRAPH_TYPE_SMITH_S11:
			case GRAPH_TYPE_SMITH_S21:
			case GRAPH_TYPE_ADMITTANCE_S11:
			case GRAPH_TYPE_ADMITTANCE_S21:
			case GRAPH_TYPE_POLAR_S11:
			case GRAPH_TYPE_POLAR_S21:
				enable_max_option = true;
				enable_min_option = false;
				break;
			case GRAPH_TYPE_PHASE_VECTOR_S11:
			case GRAPH_TYPE_PHASE_VECTOR_S21:
				enable_max_option = false;
				enable_min_option = false;
				break;
			case GRAPH_TYPE_VSWR_S11:
				//enable_min_option = false;
				break;
		}

		Automaxscale1->Enabled = enable_max_option;
		Autominscale1->Enabled = enable_min_option;

		Automaxscale1->Checked = enable_max_option ? gs->auto_max : false;
		Autominscale1->Checked = enable_min_option ? gs->auto_min : false;

		Scaletominmaxvalues1->Enabled = enable_max_option && enable_min_option;
		Scaletomaxvalue1->Enabled = enable_max_option;
		Scaletominvalue1->Enabled = enable_min_option;

		String s;

		if (enable_max_option)
		{
			s.printf(L"%0.5f", gs->max);
			// remove trailing zero's and unneeded decimal points
			if (s.Pos('.') > 0 || s.Pos(',') > 0)
			{
				while (!s.IsEmpty() && s[s.Length()] == '0')
					s = s.SubString(1, s.Length() - 1).Trim();
				if (!s.IsEmpty() && (s[s.Length()] == '.' || s[s.Length()] == ','))
					s = s.SubString(1, s.Length() - 1).Trim();
			}
			Setmaxscale1->Caption = "Set max scale [" + s + "]";
		}

		if (enable_min_option)
		{
			s.printf(L"%0.5f", gs->min);
			// remove trailing zero's and unneeded decimal points
			if (s.Pos('.') > 0 || s.Pos(',') > 0)
			{
				while (!s.IsEmpty() && s[s.Length()] == '0')
					s = s.SubString(1, s.Length() - 1).Trim();
				if (!s.IsEmpty() && (s[s.Length()] == '.' || s[s.Length()] == ','))
					s = s.SubString(1, s.Length() - 1).Trim();
			}
			Setminscale1->Caption = "Set min scale [" + s + "]";
		}

		Setmaxscale1->Enabled  = enable_max_option;
		Setminscale1->Enabled  = enable_min_option;
	}

//	Deselectmarker1->Enabled = (selectedMarker() >= 0) ? true : false;
}

void __fastcall TGraphForm::Enablespline1Click(TObject *Sender)
{
	settings.splineEnable = !settings.splineEnable;

	GraphPaintBox->Invalidate();
	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TGraphForm::Showpoints1Click(TObject *Sender)
{
	settings.showPoints = !settings.showPoints;

	GraphPaintBox->Invalidate();
	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TGraphForm::Cliptraces1Click(TObject *Sender)
{
	settings.clipTraces = !settings.clipTraces;

	GraphPaintBox->Invalidate();
	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TGraphForm::Snaptonearestpoint1Click(TObject *Sender)
{
	settings.snapToNearestPoint = !settings.snapToNearestPoint;
}

void __fastcall TGraphForm::Showmarkers1Click(TObject *Sender)
{
/*	if (m_popup_menu_mouse_graph >= 0)
	{
		const int graph      = m_popup_menu_mouse_graph;
		const int graph_type = (graph >= 0) ? settings.graphType[graph] : -1;
		t_graph_setting *gs  = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;
		if (gs)
			gs->show_markers = !gs->show_markers;
	}

	GraphPaintBox->Invalidate();
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Showminmaxtext1Click(TObject *Sender)
{
/*	if (m_popup_menu_mouse_graph >= 0)
	{
		const int graph      = m_popup_menu_mouse_graph;
		const int graph_type = (graph >= 0) ? settings.graphType[graph] : -1;
		t_graph_setting *gs  = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;
		if (gs)
			gs->show_min_max = !gs->show_min_max;
	}

	GraphPaintBox->Invalidate();
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Showmaxmarker1Click(TObject *Sender)
{
/*	if (m_popup_menu_mouse_graph >= 0)
	{
		const int graph      = m_popup_menu_mouse_graph;
		const int graph_type = (graph >= 0) ? settings.graphType[graph] : -1;
		t_graph_setting *gs  = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;
		if (gs)
			gs->show_max_marker = !gs->show_max_marker;
	}

	GraphPaintBox->Invalidate();
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Showminmarker1Click(TObject *Sender)
{
/*	if (m_popup_menu_mouse_graph >= 0)
	{
		const int graph      = m_popup_menu_mouse_graph;
		const int graph_type = (graph >= 0) ? settings.graphType[graph] : -1;
		t_graph_setting *gs  = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;
		if (gs)
			gs->show_min_marker = !gs->show_min_marker;
	}

	GraphPaintBox->Invalidate();
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Setasstartfrequency1Click(TObject *Sender)
{
//	int64_t rounding_Hz = data_unit.m_freq_span_Hz / 100;
//	rounding_Hz = I64ROUND((double)rounding_Hz / 500) * 500;	// 500Hz rounding
	const int64_t rounding_Hz = 500;	// 500Hz rounding

	const int64_t start_Hz = I64ROUND((double)m_popup_menu_mouse_Hz / rounding_Hz) * rounding_Hz;

	if (Form1)
	{
		//Form1->setStartStopHz(start_Hz, data_unit.m_freq_stop_Hz, (m_mode == MODE_SCAN) ? true : false);
		Form1->setStartStopHz(start_Hz, data_unit.m_freq_stop_Hz, false);
	}
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Setascenterfrequency1Click(TObject *Sender)
{
	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	const int64_t center_Hz = m_popup_menu_mouse_Hz;
	int64_t span_Hz = data_unit.m_freq_span_Hz;

	int64_t start_Hz = center_Hz - (span_Hz / 2);
	int64_t stop_Hz  = center_Hz + (span_Hz / 2);

	if (start_Hz < min_Hz)
		span_Hz = (center_Hz - min_Hz) * 2;
	if (stop_Hz > max_Hz)
		 span_Hz = (max_Hz - center_Hz) * 2;

	span_Hz = I64ROUND((double)span_Hz / 1000) * 1000;	// 1kHz rounding

	start_Hz = center_Hz - (span_Hz / 2);
	stop_Hz  = center_Hz + (span_Hz / 2);

	if (Form1)
	{
//		Form1->setStartStopHz(start_Hz, stop_Hz, (m_mode == MODE_SCAN) ? true : false);
		Form1->setStartStopHz(start_Hz, stop_Hz, false);
	}
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Setasstopfrequency1Click(TObject *Sender)
{
//	int64_t rounding_Hz = data_unit.m_freq_span_Hz / 100;
//	rounding_Hz = I64ROUND((double)rounding_Hz / 500) * 500;	// 500Hz rounding
	const int64_t rounding_Hz = 500;	// 500Hz rounding

	const int64_t stop_Hz = I64ROUND((double)m_popup_menu_mouse_Hz / rounding_Hz) * rounding_Hz;

	if (Form1)
	{
//		Form1->setStartStopHz(data_unit.m_freq_start_Hz, stop_Hz, (m_mode == MODE_SCAN) ? true : false);
		Form1->setStartStopHz(data_unit.m_freq_start_Hz, stop_Hz, false);
	}
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Automaxscale1Click(TObject *Sender)
{
/*	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];
		t_graph_setting *gs = &settings.m_graph_setting[graph_type];
		gs->auto_max = !gs->auto_max;
		gs->auto_max_hold_count = 0;
		GraphPaintBox->Invalidate();
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Autominscale1Click(TObject *Sender)
{
/*	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];
		t_graph_setting *gs = &settings.m_graph_setting[graph_type];
		gs->auto_min = !gs->auto_min;
		gs->auto_min_hold_count = 0;
		GraphPaintBox->Invalidate();
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Scaletominmaxvalues1Click(TObject *Sender)
{
/*	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];
		t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

		if (gs)
		{
			if (graphs.m_max[graph].index >= 0)
			{
				// disable auto max
				gs->auto_max = false;
				// set scale max
				gs->max = graphs.m_max[graph].value;
			}

			if (graphs.m_min[graph].index >= 0)
			{
				// disable auto min
				gs->auto_min = false;
				// set scale max
				gs->min = graphs.m_min[graph].value;
			}

			// sanity check
			settings.clipGraphMinMax(graph_type, gs->max, gs->min);

			GraphPaintBox->Invalidate();
		}
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Scaletomaxvalue1Click(TObject *Sender)
{
/*	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];

		if (graphs.m_max[graph].index >= 0)
		{
			t_graph_setting *gs = &settings.m_graph_setting[graph_type];

			// disable auto max
			gs->auto_max = false;

			// set scale max
			gs->max = graphs.m_max[graph].value;

			// sanity check
			settings.clipGraphMinMax(graph_type, gs->max, gs->min);

			GraphPaintBox->Invalidate();
		}
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Scaletominvalue1Click(TObject *Sender)
{
/*	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];

		if (graphs.m_min[graph].index >= 0)
		{
			t_graph_setting *gs = &settings.m_graph_setting[graph_type];

			// disable auto min
			gs->auto_min = false;

			// set scale max
			gs->min = graphs.m_min[graph].value;

			// sanity check
			settings.clipGraphMinMax(graph_type, gs->max, gs->min);

			GraphPaintBox->Invalidate();
		}
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Setmaxscale1Click(TObject *Sender)
{
/*	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];
		t_graph_setting *gs = &settings.m_graph_setting[graph_type];

		String s;
		s.printf(L"%0.5f", gs->max);

		// remove trailing zero's and unneeded decimal points
		if (s.Pos('.') > 0 || s.Pos(',') > 0)
		{
			while (!s.IsEmpty() && s[s.Length()] == '0')
				s = s.SubString(1, s.Length() - 1).Trim();
			if (!s.IsEmpty() && (s[s.Length()] == '.' || s[s.Length()] == ','))
				s = s.SubString(1, s.Length() - 1).Trim();
		}

		String units;
		switch (graph_type)
		{
			case GRAPH_TYPE_LOGMAG_S11:
			case GRAPH_TYPE_LOGMAG_S21:
			case GRAPH_TYPE_LOGMAG_S11S21:
				units = "Max log magnitude (dB)";
				break;
			case GRAPH_TYPE_PHASE_S11:
			case GRAPH_TYPE_PHASE_S21:
			case GRAPH_TYPE_PHASE_S11S21:
			case GRAPH_TYPE_PHASE_UNWRAP_S11:
			case GRAPH_TYPE_PHASE_UNWRAP_S21:
			case GRAPH_TYPE_PHASE_UNWRAP_S11S21:
				units = "Max degrees";
				break;
			case GRAPH_TYPE_SERIES_RJX_S11:
			case GRAPH_TYPE_PARALLEL_RJX_S11:
			case GRAPH_TYPE_SERIES_RESISTANCE_S11:
			case GRAPH_TYPE_SERIES_REACTANCE_S11:
				units = "Max ohms";
				break;
			case GRAPH_TYPE_GROUP_DELAY_S11:
			case GRAPH_TYPE_GROUP_DELAY_S21:
			case GRAPH_TYPE_GROUP_DELAY_S11S21:
				units = "Max nano seconds";
				break;
			case GRAPH_TYPE_REAL_IMAG_S11:
			case GRAPH_TYPE_REAL_IMAG_S21:
				units = "Max linear value";
				break;
			case GRAPH_TYPE_SERIES_CAPACITANCE_S11:
				units = "Max capacitance value";
				break;
			case GRAPH_TYPE_SERIES_INDUCTANCE_S11:
				units = "Max inductance value";
				break;
			case GRAPH_TYPE_LINMAG_S11:
			case GRAPH_TYPE_LINMAG_S21:
			case GRAPH_TYPE_LINMAG_S11S21:
				units = "Max linear magnitude";
				break;
			case GRAPH_TYPE_IMPEDANCE_S11:
				units = "Max ohms";
				break;
			case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
				units = "Max linear LP magnitude";
				break;
			case GRAPH_TYPE_TDR_LIN_BP_S11:
				units = "Max linear BP magnitude";
				break;
			case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11:
				units = "Max log LP magnitude (dB)";
				break;
			case GRAPH_TYPE_TDR_LOG_BP_S11:
				units = "Max log BP magnitude (dB)";
				break;
			case GRAPH_TYPE_TDR_IMPEDANCE_S11:
				units = "Max Ohms";
				break;
			case GRAPH_TYPE_QUALITY_FACTOR_S11:
				units = "Max quality factor";
				break;
			case GRAPH_TYPE_VSWR_S11:
				units = "Max VSWR";
				break;
			case GRAPH_TYPE_COAX_LOSS_S11:
				units = "Max coax loss (dB)";
				break;
			case GRAPH_TYPE_CAL_LOGMAG:
				units = "Max cal log magnitude (dB)";
				break;
			case GRAPH_TYPE_SMITH_S11:
			case GRAPH_TYPE_SMITH_S21:
			case GRAPH_TYPE_ADMITTANCE_S11:
			case GRAPH_TYPE_ADMITTANCE_S21:
			case GRAPH_TYPE_POLAR_S11:
			case GRAPH_TYPE_POLAR_S21:
				units = "Max mag";
				break;
			case GRAPH_TYPE_PHASE_VECTOR_S11:
			case GRAPH_TYPE_PHASE_VECTOR_S21:
				break;
			default:
				break;
		}

		if (InputQuery("Graph MAX scale", units, s))
		{
			double value;
			if (TryStrToFloat(s, value))
			{
				gs->auto_max = false;	// disable auto max
				gs->max = value;

				settings.clipGraphMinMax(graph_type, gs->max, gs->min);

				GraphPaintBox->Invalidate();
			}
		}
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Setminscale1Click(TObject *Sender)
{
/*	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];
		t_graph_setting *gs = &settings.m_graph_setting[graph_type];

		String s;
		s.printf(L"%0.5f", gs->min);

		// remove trailing zero's and unneeded decimal points
		if (s.Pos('.') > 0 || s.Pos(',') > 0)
		{
			while (!s.IsEmpty() && s[s.Length()] == '0')
				s = s.SubString(1, s.Length() - 1).Trim();
			if (!s.IsEmpty() && (s[s.Length()] == '.' || s[s.Length()] == ','))
				s = s.SubString(1, s.Length() - 1).Trim();
		}

		String units;
		switch (graph_type)
		{
			case GRAPH_TYPE_LOGMAG_S11:
			case GRAPH_TYPE_LOGMAG_S21:
			case GRAPH_TYPE_LOGMAG_S11S21:
				units = "Min log magnitude (dB)";
				break;
			case GRAPH_TYPE_PHASE_S11:
			case GRAPH_TYPE_PHASE_S21:
			case GRAPH_TYPE_PHASE_S11S21:
			case GRAPH_TYPE_PHASE_UNWRAP_S11:
			case GRAPH_TYPE_PHASE_UNWRAP_S21:
			case GRAPH_TYPE_PHASE_UNWRAP_S11S21:
				units = "Min degrees";
				break;
			case GRAPH_TYPE_SERIES_RJX_S11:
			case GRAPH_TYPE_PARALLEL_RJX_S11:
			case GRAPH_TYPE_SERIES_RESISTANCE_S11:
			case GRAPH_TYPE_SERIES_REACTANCE_S11:
				units = "Min Ohms";
				break;
			case GRAPH_TYPE_GROUP_DELAY_S11:
			case GRAPH_TYPE_GROUP_DELAY_S21:
			case GRAPH_TYPE_GROUP_DELAY_S11S21:
				units = "Min nano seconds";
				break;
			case GRAPH_TYPE_REAL_IMAG_S11:
			case GRAPH_TYPE_REAL_IMAG_S21:
				units = "Min linear value";
				break;
			case GRAPH_TYPE_SERIES_CAPACITANCE_S11:
				units = "Min capacitance value";
				break;
			case GRAPH_TYPE_SERIES_INDUCTANCE_S11:
				units = "Min inductance value";
				break;
			case GRAPH_TYPE_LINMAG_S11:
			case GRAPH_TYPE_LINMAG_S21:
			case GRAPH_TYPE_LINMAG_S11S21:
				units = "Min linear magnitude";
				break;
			case GRAPH_TYPE_IMPEDANCE_S11:
				units = "Min Ohms";
				break;
			case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
				units = "Min linear LP magnitude";
				break;
			case GRAPH_TYPE_TDR_LIN_BP_S11:
				units = "Min linear BP magnitude";
				break;
			case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11:
				units = "Min log LP magnitude (dB)";
				break;
			case GRAPH_TYPE_TDR_LOG_BP_S11:
				units = "Min log BP magnitude (dB)";
				break;
			case GRAPH_TYPE_TDR_IMPEDANCE_S11:
				units = "Min Ohms";
				break;
			case GRAPH_TYPE_QUALITY_FACTOR_S11:
				units = "Min quality factor";
				break;
			case GRAPH_TYPE_VSWR_S11:
				units = "Min VSWR";
				break;
			case GRAPH_TYPE_COAX_LOSS_S11:
				units = "Min coax loss (dB)";
				break;
			case GRAPH_TYPE_CAL_LOGMAG:
				units = "Min cal log magnitude (dB)";
				break;
			case GRAPH_TYPE_SMITH_S11:
			case GRAPH_TYPE_SMITH_S21:
			case GRAPH_TYPE_ADMITTANCE_S11:
			case GRAPH_TYPE_ADMITTANCE_S21:
			case GRAPH_TYPE_POLAR_S11:
			case GRAPH_TYPE_POLAR_S21:
				break;
			case GRAPH_TYPE_PHASE_VECTOR_S11:
			case GRAPH_TYPE_PHASE_VECTOR_S21:
				break;
			default:
				break;
		}

		if (InputQuery("Graph MIN scale", units, s))
		{
			double value;
			if (TryStrToFloat(s, value))
			{
				gs->auto_min = false;	// disable auto min
				gs->min = value;

				settings.clipGraphMinMax(graph_type, gs->max, gs->min);

				GraphPaintBox->Invalidate();
			}
		}
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Addmarker1Click(TObject *Sender)
{
//	addMarker(m_popup_menu_mouse_Hz);
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Deletemarker1Click(TObject *Sender)
{
/*	if (m_popup_menu_mouse_marker >= 0)
	{
		deleteMarker(m_popup_menu_mouse_marker);
		m_popup_menu_mouse_marker = -1;
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Deselectmarker1Click(TObject *Sender)
{
/*  // delect all markers
	for (int i = 0; i < MarkerListBox->Items->Count; i++)
		MarkerListBox->Selected[i] = false;
	GraphPaintBox->Invalidate();
	updateInfoPanel();
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Deltamarker2Click(TObject *Sender)
{
/*	if (m_popup_menu_mouse_marker >= 0)
	{
		if (settings.m_markers_freq[m_popup_menu_mouse_marker].type == MARKER_TYPE_DELTA)
		{
			settings.m_markers_freq[m_popup_menu_mouse_marker].type = MARKER_TYPE_NORMAL;
		}
		else
		{
			if (!settings.m_markers_freq.empty())
				settings.m_markers_freq[m_popup_menu_mouse_marker].type = MARKER_TYPE_DELTA;
		}

		buildMarkerListBox();

		GraphPaintBox->Invalidate();
		updateInfoPanel();
		ClearMarkersButton->Enabled = !settings.m_markers_freq.empty();

		m_popup_menu_mouse_marker = -1;
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Alltraces1Click(TObject *Sender)
{
/*	if (m_popup_menu_mouse_marker >= 0)
	{
		settings.m_markers_freq[m_popup_menu_mouse_marker].trace = -1;
		GraphPaintBox->Invalidate();
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Trace11Click(TObject *Sender)
{
/*	if (m_popup_menu_mouse_marker >= 0)
	{
		settings.m_markers_freq[m_popup_menu_mouse_marker].trace = 0;
		GraphPaintBox->Invalidate();
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::Trace21Click(TObject *Sender)
{
/*	if (m_popup_menu_mouse_marker >= 0)
	{
		settings.m_markers_freq[m_popup_menu_mouse_marker].trace = 1;
		GraphPaintBox->Invalidate();
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::GraphTypePopupMenuPopup(TObject *Sender)
{
	m_popup_menu_graph_type_graph = graphs.m_mouse.graph_type_select;

	if (m_graph_type >= 0)
	{
		const int graph_type = m_graph_type;
		for (int i = 0; i < GraphTypePopupMenu->Items->Count; i++)
		{
			if (GraphTypePopupMenu->Items->Items[i]->Tag == graph_type)
			{
				GraphTypePopupMenu->Items->Items[i]->Checked = true;
				break;
			}
		}
	}

	graphs.m_mouse.graph_type_select = -1;
}


void __fastcall TGraphForm::SaveImageImageClick(TObject *Sender)
{
	saveImage(false);
}

void __fastcall TGraphForm::CloseImageClick(TObject *Sender)
{
	Close();
}

void __fastcall TGraphForm::Saveimage1Click(TObject *Sender)
{
	saveImage(false);
}

void __fastcall TGraphForm::Copyimage1Click(TObject *Sender)
{
	saveImage(true);
}
//---------------------------------------------------------------------------

void __fastcall TGraphForm::CopyImageImageClick(TObject *Sender)
{
	saveImage(true);
}

