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

#pragma hdrstop

#include "ScreenCapture.h"
#include "Unit1.h"
#include "DataUnit.h"
#include "common.h"
#include "Settings.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TVNAScreenCaptureForm *VNAScreenCaptureForm = NULL;

__fastcall TVNAScreenCaptureForm::TVNAScreenCaptureForm(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall TVNAScreenCaptureForm::FormCreate(TObject *Sender)
{
	m_initialised = false;
	m_mouse.down = false;
	m_title = Application->Title + " " + this->Caption;
	Caption = m_title;
	ToggleSwitch1->State = tssOff;
	ToggleSwitch1->Enabled = false;

	this->DoubleBuffered = true;

	//Panel1->ControlStyle  = Panel1->ControlStyle << csOpaque;
	ActualSizeImage->ControlStyle = ActualSizeImage->ControlStyle << csOpaque;
	ResizeImage->ControlStyle = ResizeImage->ControlStyle << csOpaque;

	SaveDialog1->InitialDir = ExtractFilePath(Application->ExeName);

	m_image_dw = this->Width  - Panel1->Width;
	m_image_dh = this->Height - Panel1->Height;

	ActualSizeImage->Visible = (ActualSizeToggleSwitch->State == tssOn ) ? true : false;
	ResizeImage->Visible     = (ActualSizeToggleSwitch->State == tssOff) ? true : false;

	// move to the saved position
	this->Top    = settings.screenCaptureWindowPos.top;
	this->Left   = settings.screenCaptureWindowPos.left;
//	this->Width  = settings.screenCaptureWindowPos.width;
//	this->Height = settings.screenCaptureWindowPos.height;
}

void __fastcall TVNAScreenCaptureForm::FormDestroy(TObject *Sender)
{
	//
}

void __fastcall TVNAScreenCaptureForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	//
    ToggleSwitch1->State = tssOff;
}

#define ABS(x)  (((x) >= 0) ? (x) : -(x))

void __fastcall TVNAScreenCaptureForm::WMWindowPosChanging(TWMWindowPosChanging &msg)
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

void __fastcall TVNAScreenCaptureForm::show()
{
	this->Show();
	this->BringToFront();
//	this->Update();
}

void __fastcall TVNAScreenCaptureForm::addImage(Graphics::TBitmap *bitmap)
{
	if (bitmap == NULL)
		return;
 	m_bitmap = bitmap;
	ActualSizeImage->Picture->Assign(m_bitmap);
	ResizeImage->Picture->Assign(m_bitmap);

	ActualSizeImage->Width = m_bitmap->Width + 6;
	ActualSizeImage->Height = m_bitmap->Height + 6;

	m_image_w = bitmap->Width;
	m_image_h = bitmap->Height;

	updateImage();

	String s;
	s.printf(L"   %d * %d", m_bitmap->Width, m_bitmap->Height);
	Caption = m_title + s;

	show();
}

void __fastcall TVNAScreenCaptureForm::updateImage()
{
	ActualSizeImage->Visible = (ActualSizeToggleSwitch->State == tssOn ) ? true : false;
	ResizeImage->Visible     = (ActualSizeToggleSwitch->State == tssOff) ? true : false;

	if (ActualSizeToggleSwitch->State == tssOn)
	{
		this->Width   = ActualSizeImage->Width  + m_image_dw;
		this->Height  = ActualSizeImage->Height + m_image_dh;
		this->BorderStyle = bsToolWindow;
	}
	else
	{

		this->BorderStyle = bsSizeToolWin;
	}
}

void __fastcall TVNAScreenCaptureForm::saveBitmap(Graphics::TBitmap *bm, String dialog_title)
{
	if (bm == NULL)
		return;

	dialog_title = dialog_title.Trim();

	String filename = data_unit.m_vna_data.name.LowerCase() + "_screen_" + FormatDateTime("yyyy-mm-dd_hh-nn-ss", Now()) + ".png";

	filename = common.cleanFilename(filename, false, true);

	SaveDialog1->Title    = dialog_title.IsEmpty() ? String("Save the image too ..") : dialog_title;
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

	AnsiString ext = ExtractFileExt(filename).LowerCase();

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
	Application->MessageBox(L"Only PNG, JPG, JPEG and BMP extensions are supported", L"Error", MB_ICONERROR | MB_OK);
	Application->RestoreTopMosts();
}

void __fastcall TVNAScreenCaptureForm::ActualSizeToggleSwitchClick(TObject *Sender)
{
	updateImage();
}

void __fastcall TVNAScreenCaptureForm::FormShow(TObject *Sender)
{
	if (!m_initialised)
	{
		// move to the saved position
		this->Top    = settings.screenCaptureWindowPos.top;
		this->Left   = settings.screenCaptureWindowPos.left;
//		this->Width  = settings.screenCaptureWindowPos.width;
//		this->Height = settings.screenCaptureWindowPos.height;

		m_initialised = true;
	}
}

void __fastcall TVNAScreenCaptureForm::CloseBitBtnClick(TObject *Sender)
{
	Hide();
}

void __fastcall TVNAScreenCaptureForm::FormKeyDown(TObject *Sender, WORD &Key,
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
//---------------------------------------------------------------------------

void __fastcall TVNAScreenCaptureForm::FormMouseEnter(TObject *Sender)
{
//	this->SetFocus();
}

void __fastcall TVNAScreenCaptureForm::SaveImageBitBtnClick(TObject *Sender)
{
	saveBitmap(m_bitmap, "Save the image too ..");
}

void __fastcall TVNAScreenCaptureForm::CopyImageBitBtnClick(TObject *Sender)
{
	TClipboard *cb;

	if (m_bitmap == NULL)
		return;

	try
	{
		cb = Clipboard();

		const DWORD tick = GetTickCount();
		do
		{
			try
			{
				cb->Clear();
				cb->Assign(m_bitmap);		// copy the image to the clipboard
				Sleep(10);
				if (cb->HasFormat(CF_BITMAP))
					break;
			}
			catch (Exception &exception)
			{
			}
		} while ((GetTickCount() - 500) < tick);
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
		//String s = exception.ToString();
	}
}

void __fastcall TVNAScreenCaptureForm::RefreshImageBitBtnClick(TObject *Sender)
{
	if (Form1)
		Form1->getVNAScreenCapture();
}

void __fastcall TVNAScreenCaptureForm::ResizeImageMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	m_mouse.x = X *  m_image_w / ResizeImage->Width;
	m_mouse.y = Y *  m_image_h / ResizeImage->Height;
	m_mouse.down = true;
	if (Form1)
		Form1->requestMouseDown(m_mouse.x,m_mouse.y);
}
//---------------------------------------------------------------------------

void __fastcall TVNAScreenCaptureForm::ResizeImageMouseUp(TObject *Sender, TMouseButton Button,
					TShiftState Shift, int X, int Y)
{
	m_mouse.x = X;
	m_mouse.y = Y;
	m_mouse.down = false;
	if (Form1)
		Form1->requestMouseUp();
}
//---------------------------------------------------------------------------

void __fastcall TVNAScreenCaptureForm::ActualSizeImageMouseDown(TObject *Sender, TMouseButton Button,
					TShiftState Shift, int X, int Y)
{
	m_mouse.x = X;
	m_mouse.y = Y;
	m_mouse.down = true;
	if (Form1)
		Form1->requestMouseDown(X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TVNAScreenCaptureForm::ActualSizeImageMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
  	m_mouse.x = X;
	m_mouse.y = Y;
	m_mouse.down = false;
	if (Form1)
		Form1->requestMouseUp();
}
//---------------------------------------------------------------------------

void __fastcall TVNAScreenCaptureForm::ResizeImageMouseMove(TObject *Sender, TShiftState Shift,
					int X, int Y)
{
	m_mouse.x = X *  m_image_w / ResizeImage->Width;
	m_mouse.y = Y *  m_image_h / ResizeImage->Height;
	if (Form1 && m_mouse.down) {
		Form1->requestMouseDown(m_mouse.x,m_mouse.y);
	}
}
//---------------------------------------------------------------------------

void __fastcall TVNAScreenCaptureForm::ActualSizeImageMouseMove(TObject *Sender, TShiftState Shift,
					int X, int Y)
{
	m_mouse.x = X;
	m_mouse.y = Y;
	if (Form1 && m_mouse.down)
		Form1->requestMouseDown(X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TVNAScreenCaptureForm::ActualSizeImageMouseLeave(TObject *Sender)

{
	if (Form1 && m_mouse.down) {
		m_mouse.down = false;
		Form1->requestMouseUp();
	}
}
//---------------------------------------------------------------------------

void __fastcall TVNAScreenCaptureForm::ResizeImageMouseLeave(TObject *Sender)
{
	if (Form1 && m_mouse.down) {
		m_mouse.down = false;
		Form1->requestMouseUp();
	}
}
//---------------------------------------------------------------------------

void __fastcall TVNAScreenCaptureForm::ToggleSwitch1Click(TObject *Sender)
{
	if (ToggleSwitch1->State == tssOn)
		Form1->autoRefresh(true);
	else
		Form1->autoRefresh(false);
}
//---------------------------------------------------------------------------

