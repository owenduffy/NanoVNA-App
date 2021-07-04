
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

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

#include "BatteryVoltageUnit.h"
#include "common.h"
#include "Settings.h"
#include "DataUnit.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TBatteryVoltageForm *BatteryVoltageForm = NULL;

__fastcall TBatteryVoltageForm::TBatteryVoltageForm(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall TBatteryVoltageForm::FormCreate(TObject *Sender)
{
	AnsiString s;

	m_initialised = false;

	Caption = Application->Title + " " + this->Caption;

	this->DoubleBuffered = true;

	//Panel1->ControlStyle  = Panel1->ControlStyle << csOpaque;
	PaintBox1->ControlStyle = PaintBox1->ControlStyle << csOpaque;

	SaveDialog1->InitialDir = ExtractFilePath(Application->ExeName);

	m_graph_mouse_x     = -1;
	m_graph_mouse_y     = -1;
	m_graph_mouse_index = -1;

	m_graph_bm = NULL;

	m_gdiplusToken = 0;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

   m_battery_voltage.reserve(10000);

	VBATOffsetLabel->Caption = "";

	s.printf("%u", m_battery_voltage.size());
	StatusBar1->Panels->Items[0]->Text = s;

	// move to the saved position
	this->Top    = settings.batteryWindowPos.top;
	this->Left   = settings.batteryWindowPos.left;
	this->Width  = settings.batteryWindowPos.width;
	this->Height = settings.batteryWindowPos.height;
}

void __fastcall TBatteryVoltageForm::FormDestroy(TObject *Sender)
{
	if (m_graph_bm != NULL)
	{
		delete m_graph_bm;
		m_graph_bm = NULL;
	}

	if (m_gdiplusToken != 0)
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	m_gdiplusToken = 0;
}

void __fastcall TBatteryVoltageForm::WMWindowPosChanging(TWMWindowPosChanging &msg)
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

void __fastcall TBatteryVoltageForm::show()
{
//	update();

	this->Show();
	this->BringToFront();
//	this->Update();
}

void __fastcall TBatteryVoltageForm::clear()
{
	m_battery_voltage.resize(0);

	AnsiString s;
	s.printf("%u", m_battery_voltage.size());
	StatusBar1->Panels->Items[0]->Text = s;

	if (this->Showing)
		PaintBox1->Invalidate();
}

void __fastcall TBatteryVoltageForm::addBatteryVoltage(const uint16_t vbat_mv, const uint16_t vbat_offset_mv)
{
	AnsiString s;

	if (vbat_mv > 0)
	{
		t_battery_voltage battery_voltage;
		battery_voltage.datetime = Now();
		battery_voltage.mv       = vbat_mv;
		m_battery_voltage.push_back(battery_voltage);

		s.printf("%u samples", m_battery_voltage.size());
		StatusBar1->Panels->Items[0]->Text = s;

		s.printf("%0.3fV", (float)vbat_mv / 1000);
		StatusBar1->Panels->Items[1]->Text = s;
	}

	s.printf("%u", vbat_offset_mv);
	VBATOffsetLabel->Caption = s;

	if (this->Showing)
		PaintBox1->Invalidate();
}

void __fastcall TBatteryVoltageForm::FormShow(TObject *Sender)
{
	if (!m_initialised)
	{
		// move to the saved position
		this->Top    = settings.batteryWindowPos.top;
		this->Left   = settings.batteryWindowPos.left;
		this->Width  = settings.batteryWindowPos.width;
		this->Height = settings.batteryWindowPos.height;

		m_initialised = true;
	}
}

void __fastcall TBatteryVoltageForm::PaintBox1Paint(TObject *Sender)
{
	TPaintBox *pb = dynamic_cast<TPaintBox *>(Sender);
	if (pb == NULL)
		return;

	m_graph_mouse_index = -1;

	if (m_graph_bm == NULL)
	{
		m_graph_bm = new Graphics::TBitmap();
		if (m_graph_bm == NULL)
		{
			pb->Canvas->Brush->Color = pb->Color;
			pb->Canvas->Brush->Style = bsSolid;
			pb->Canvas->FillRect(pb->Canvas->ClipRect);
			return;
		}

		m_graph_bm->Monochrome   = false;
		m_graph_bm->Transparent  = false;
		m_graph_bm->PixelFormat  = pf32bit;
	}

	m_graph_bm->Canvas->Font = pb->Canvas->Font;
	m_graph_bm->Width        = pb->Width;
	m_graph_bm->Height       = pb->Height;

	// background colour
	m_graph_bm->Canvas->Pen->Width   = 1;
	m_graph_bm->Canvas->Pen->Style   = psSolid;
	m_graph_bm->Canvas->Pen->Color   = clGray;
//	m_graph_bm->Canvas->Pen->Color   = pb->Color;
	m_graph_bm->Canvas->Brush->Color = pb->Color;
	m_graph_bm->Canvas->Brush->Style = bsSolid;
	m_graph_bm->Canvas->Rectangle(m_graph_bm->Canvas->ClipRect);

	const TColor back_colour = pb->Color;
	const TColor grid_colour = TColor(RGB(190, 190, 190));

	LOGFONT lf;
	::GetObject(m_graph_bm->Canvas->Font->Handle, sizeof(LOGFONT), &lf);
	lf.lfEscapement  = -900;	// 90 degrees rotation
	lf.lfOrientation = lf.lfEscapement;	//    "          "
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
	const HFONT lf_handle = CreateFontIndirect(&lf);
//	const HFONT old_handle = ::SelectObject(bm->Canvas->Handle, lf_handle);
	::SetGraphicsMode(m_graph_bm->Canvas->Handle, GM_ADVANCED);

	const int line_width = 1;

	m_graph_x      = 55;
	m_graph_y      = 30;
	m_graph_width  = m_graph_bm->Width  - m_graph_x - 50;
	m_graph_height = m_graph_bm->Height - m_graph_y - 30;

	// find the maximum and minimum values so far
	m_max_mv = m_battery_voltage.empty() ? 4300 : m_battery_voltage[0].mv;
	m_min_mv = m_battery_voltage.empty() ? 3000 : m_battery_voltage[0].mv;
	for (unsigned int i = 0; i < m_battery_voltage.size(); i++)
	{
		const uint16_t mv = m_battery_voltage[i].mv;
		if (m_max_mv < mv)
			 m_max_mv = mv;
		if (m_min_mv > mv)
			 m_min_mv = mv;
	}
	m_max_mv    = ((m_max_mv + 100) / 100) * 100;		// 100mV steps
	if (m_min_mv >= 100)
		m_min_mv = ((m_min_mv -   0) / 100) * 100;			// 100mV steps
	else
		m_min_mv = 0;
	if (m_max_mv < (m_min_mv + 100))
		 m_max_mv =  m_min_mv + 100;

	const double start_dt = m_battery_voltage.empty()      ? (TDateTime)0.0 : m_battery_voltage[0].datetime;
	const double end_dt   = (m_battery_voltage.size() < 1) ? (TDateTime)0.0 : m_battery_voltage[m_battery_voltage.size() - 1].datetime;
	const double span_dt  = end_dt - start_dt;

	const int text_height = m_graph_bm->Canvas->TextHeight("Hq");

	const float x_scale = (span_dt > 0) ? (float)m_graph_width / span_dt : 0;
	const float y_scale = (float)m_graph_height / (m_max_mv - m_min_mv);

	m_gdi_points.resize(0);

	{	// draw vertical lines
		m_graph_bm->Canvas->Pen->Style   = psDot;
		m_graph_bm->Canvas->Brush->Style = bsSolid;
		m_graph_bm->Canvas->Brush->Color = back_colour;
		m_graph_bm->Canvas->Pen->Color   = grid_colour;

		const int num_lines = 19;

		const int min_text_spacing = 15;

		const int y1 = m_graph_y;
		const int y2 = m_graph_y + m_graph_height;

		if (m_battery_voltage.size() < 2)
		{
			const float x_scale = (float)m_graph_width / (num_lines - 1);
			for (int i = 0; i < num_lines; i++)
			{
				// vertical line
				const int x = m_graph_x + IROUND(i * x_scale);
				m_graph_bm->Canvas->MoveTo(x, y1);
				m_graph_bm->Canvas->LineTo(x, y2);
			}
		}
		else
		{
			for (int i = 0, x2 = -min_text_spacing; i < num_lines; i++)
			{
				const double delta_time = (span_dt * i) / (num_lines - 1);
				const double date_time  = start_dt + delta_time;

				const int x = m_graph_x + IROUND(delta_time * x_scale);

				// vertical line
				m_graph_bm->Canvas->MoveTo(x, y1);
				m_graph_bm->Canvas->LineTo(x, y2);

				// upper text
				AnsiString s1 = FormatDateTime("hh:mm:ss", delta_time);
				const int tw1 = m_graph_bm->Canvas->TextWidth(s1);
				const int tx1 = x - (tw1 / 2);
				const int ty1 = y1 - 10 - text_height;

				// lower text
				AnsiString s2 = FormatDateTime("hh:mm:ss", date_time);
				const int tw2 = m_graph_bm->Canvas->TextWidth(s2);
				const int tx2 = x - (tw2 / 2);
				const int ty2 = y2 + 10;

				// draw the text
				if (tx1 >= (x2 + min_text_spacing) && tx2 >= (x2 + min_text_spacing))
				{
					m_graph_bm->Canvas->TextOut(tx1, ty1, s1);
					m_graph_bm->Canvas->TextOut(tx2, ty2, s2);
					x2 = (tx1 >= tx2) ? tx1 + tw1 : tx2 + tw2;
				}
			}
		}

		m_graph_bm->Canvas->Pen->Style   = psSolid;
	}

	{	// draw horizontal lines
		AnsiString s;

		m_graph_bm->Canvas->Pen->Style   = psDot;
		m_graph_bm->Canvas->Brush->Style = bsSolid;
		m_graph_bm->Canvas->Brush->Color = back_colour;
		m_graph_bm->Canvas->Pen->Color   = grid_colour;

		const int x1 = m_graph_x;
		const int x2 = m_graph_x + m_graph_width;

		const int tx = x1 - 10 - m_graph_bm->Canvas->TextWidth("0.0");

		{
			s = "Battery Voltage";
			const int ty = m_graph_y + ((m_graph_height - m_graph_bm->Canvas->TextWidth(s)) / 2);
			const HFONT old_handle = (HFONT)::SelectObject(m_graph_bm->Canvas->Handle, lf_handle);
			m_graph_bm->Canvas->TextOut(tx - 5, ty, s);
			::SelectObject(m_graph_bm->Canvas->Handle, old_handle);
		}

		for (int mv = m_min_mv; mv <= m_max_mv; mv += 100)
		{
			const int y = m_graph_y + m_graph_height - IROUND((mv - m_min_mv) * y_scale);

			// horizontal line
			m_graph_bm->Canvas->MoveTo(x1, y);
			m_graph_bm->Canvas->LineTo(x2, y);

			s.printf("%0.1f", (float)mv / 1000);
			const int ty = y - (text_height / 2);

			// draw the text
			m_graph_bm->Canvas->TextOut(tx, ty, s);
		}

		m_graph_bm->Canvas->Pen->Style   = psSolid;
	}

	for (unsigned int i = 0; i < m_battery_voltage.size(); i++)
	{
		const uint16_t mv         = m_battery_voltage[i].mv;
		const TDateTime date_time = m_battery_voltage[i].datetime;

		Gdiplus::PointF point;
		point.X = m_graph_x + (((double)date_time - start_dt) * x_scale);
		point.Y = m_graph_y + m_graph_height - ((mv - m_min_mv) * y_scale);
		m_gdi_points.push_back(point);
	}

	// draw the curve/line
	if (m_gdi_points.size() >= 2)
	{
		Gdiplus::Graphics g(m_graph_bm->Canvas->Handle);
//		g.SetSmoothingMode(Gdiplus::SmoothingModeNone);
//		g.SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);		// fast
		g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);		// high quality
//		g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
//		g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeNone);
		g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

		const int alpha      = 255;
		const int red        = 0;
		const int grn        = 0;
		const int blu        = 0;

		Gdiplus::Pen pen(Gdiplus::Color(alpha, red, grn, blu), line_width);
		pen.SetAlignment(Gdiplus::PenAlignmentCenter);
		pen.SetDashStyle(Gdiplus::DashStyleSolid);
//		pen.SetDashStyle(Gdiplus::DashStyleDot);
		g.DrawLines(&pen, &m_gdi_points[0], m_gdi_points.size());
	}

	if (!m_gdi_points.empty())
	{
		AnsiString s;

		{	// plot the graph points
			const int point_size = 1 + line_width;
			if (fabs(m_gdi_points[1].X - m_gdi_points[0].X) >= (point_size * 4))
			{
				m_graph_bm->Canvas->Pen->Color   = TColor(RGB(0, 0, 0));
				m_graph_bm->Canvas->Pen->Width   = 1;
				m_graph_bm->Canvas->Brush->Color = (point_size <= 2) ? m_graph_bm->Canvas->Pen->Color : back_colour;
				for (unsigned int i = 0; i < m_gdi_points.size(); i++)
				{
					const int x = IROUND(m_gdi_points[i].X);
					const int y = IROUND(m_gdi_points[i].Y);
					m_graph_bm->Canvas->Ellipse(IROUND(x - point_size), IROUND(y - point_size), IROUND(x + point_size), IROUND(y + point_size));
				}
			}
		}

		{	// draw the voltage at the last point
			const int index   = m_battery_voltage.size() - 1;
			const uint16_t mv = m_battery_voltage[index].mv;
			s.printf("%0.3f", (float)mv / 1000);
			const int tx      = m_graph_x + m_graph_width + 10;
			const int  y      = m_graph_y + m_graph_height - ((mv - m_min_mv) * y_scale);
			m_graph_bm->Canvas->Brush->Color = back_colour;
			m_graph_bm->Canvas->TextOut(tx,  y - (text_height / 2), s);
		}

		if (!m_battery_voltage.empty() &&
			 m_graph_mouse_x >= m_graph_x &&
			 m_graph_mouse_y >= m_graph_y &&
			 m_graph_mouse_x <= m_graph_x + m_graph_width &&
			 m_graph_mouse_y <= m_graph_y + m_graph_height)
		{	// draw the mouse

			int index = -1;
			{	// find the closest point
				float min_dist = -1.0f;
				for (unsigned int i = 0; i < m_gdi_points.size(); i++)
				{
					const Gdiplus::PointF point = m_gdi_points[i];
					const float delta_x = (float)m_graph_mouse_x - point.X;
					const float delta_y = (float)m_graph_mouse_y - point.Y;
					const float dist = (delta_x * delta_x) + (delta_y * delta_y);
					if (min_dist < 0.0f || min_dist > dist)
					{
						min_dist = dist;
						index    = i;
					}
				}
			}

			if (index >= 0 && index < (int)m_battery_voltage.size())
			{
				const int mv              = m_battery_voltage[index].mv;
				const TDateTime date_time = m_battery_voltage[index].datetime;

				const int x = m_graph_x + IROUND(((double)date_time - start_dt) * x_scale);
				const int y = m_graph_y + m_graph_height - IROUND((mv - m_min_mv) * y_scale);

//				if (std::abs(m_graph_mouse_x - x) <= 20 && std::abs(m_graph_mouse_y - y) <= 30)
				{
					m_graph_mouse_index = index;

					const int marker_size = 6;

					{	// marker
						TPoint points[4];
						points[0].x = x; points[0].y = y - 2;
						points[1].x = x - marker_size; points[1].y = y - marker_size * 2;
						points[2].x = x + marker_size; points[2].y = y - marker_size * 2;
						points[3].x = x; points[3].y = y - 2;
						m_graph_bm->Canvas->Pen->Style = psSolid;
						m_graph_bm->Canvas->Pen->Color = TColor(RGB(0, 0, 0));
						m_graph_bm->Canvas->Pen->Width = 1;
						//	m_graph_bm->Canvas->Brush->Style = bsClear;
						m_graph_bm->Canvas->Brush->Color = back_colour;
						m_graph_bm->Canvas->Polygon(&points[0], 3);
					}

					{	// index text
						s.printf("%d", index);
						TSize ts = m_graph_bm->Canvas->TextExtent(s);
						const int tx = x - (ts.cx / 2);
						const int ty = y - marker_size - 10 - (text_height * 3);
						m_graph_bm->Canvas->Brush->Color = back_colour;
						m_graph_bm->Canvas->TextOut(tx, ty, s);
					}

					{	// time text
						s = FormatDateTime("hh:mm:ss", date_time);
						TSize ts = m_graph_bm->Canvas->TextExtent(s);
						const int tx = x - (ts.cx / 2);
						const int ty = y - marker_size - 10 - (text_height * 2);
						m_graph_bm->Canvas->Brush->Color = back_colour;
						m_graph_bm->Canvas->TextOut(tx, ty, s);
					}

					{	// voltage text
						s.printf("%0.3fV", (float)mv / 1e3);
						const int tx = x - (m_graph_bm->Canvas->TextWidth(s) / 2);
						const int ty = y - marker_size - 10 - (text_height * 1);
						m_graph_bm->Canvas->Brush->Color = back_colour;
						m_graph_bm->Canvas->TextOut(tx, ty, s);
					}
				}
			}
		}
	}

	::DeleteObject(lf_handle);

	pb->Canvas->Draw(0, 0, m_graph_bm);
}

void __fastcall TBatteryVoltageForm::saveImage(const bool to_clipboard)
{
	if (m_graph_bm == NULL)
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"No image to save", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	if (to_clipboard)
	{
		try
		{
			Clipboard()->Assign(m_graph_bm);		// copy the image to the clipboard
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
			//String s = exception.ToString();
		}
		return;
	}

	String filename = data_unit.m_vna_data.name.LowerCase() + "_bat_volt_" + FormatDateTime("yyyy-mm-dd_hh-nn-ss", Now()) + ".png";

	filename = common.cleanFilename(filename, false, true);

	SaveDialog1->FileName = filename;

	Application->NormalizeTopMosts();
	const bool ok = SaveDialog1->Execute();
	Application->RestoreTopMosts();

	if (!ok)
		return;

	filename = SaveDialog1->FileName;

	if (filename.IsEmpty())
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Invalid filename characters", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	String ext = ExtractFileExt(filename).LowerCase();

	if (ext == ".bmp")
	{
		try
		{
			m_graph_bm->SaveToFile(filename);
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
			jpg->Assign(m_graph_bm);
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
			png->Assign(m_graph_bm);
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
	Application->MessageBox(L"Only PNG, JPG, JPEG and BMP extensions are supported", L"Error", MB_ICONERROR | MB_OK);
	Application->RestoreTopMosts();
}

void __fastcall TBatteryVoltageForm::PaintBox1MouseMove(TObject *Sender,
		TShiftState Shift, int X, int Y)
{
	m_graph_mouse_x = X;
	m_graph_mouse_y = Y;

	PaintBox1->Invalidate();
}

void __fastcall TBatteryVoltageForm::FormKeyDown(TObject *Sender, WORD &Key,
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

void __fastcall TBatteryVoltageForm::PaintBox1MouseLeave(TObject *Sender)
{
	m_graph_mouse_x     = -1;
	m_graph_mouse_y     = -1;
	m_graph_mouse_index = -1;
}

void __fastcall TBatteryVoltageForm::CloseBitBtnClick(TObject *Sender)
{
	Hide();
}

void __fastcall TBatteryVoltageForm::FormMouseEnter(TObject *Sender)
{
//	this->SetFocus();
}

void __fastcall TBatteryVoltageForm::SaveBitBtnClick(TObject *Sender)
{
	saveImage(false);
}

void __fastcall TBatteryVoltageForm::CopyBitBtnClick(TObject *Sender)
{
	saveImage(true);
}


void __fastcall TBatteryVoltageForm::ClearBitBtnClick(TObject *Sender)
{
	clear();
}

