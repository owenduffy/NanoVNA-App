
#ifndef GraphUnitH
#define GraphUnitH

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>

// GDI+ system
//#include <Unknwn.h>
//#include <windows.h>
#include <algorithm>
//using std::min;
//using std::max;
#pragma option push
	#pragma warn -8022
	#include <gdiplus.h>
//	#pragma link "gdiplus.lib"
//	#pragma comment (lib, "gdiplus.lib")
//	using namespace Gdiplus;
#pragma option pop

#include <vector>

#include "types.h"
#include "settings.h"
#include "common.h"
#include "Graphs.h"
#include "tfft.h"

class TGraphForm : public TForm
{
__published:	// IDE-managed Components
	TSaveDialog *SaveDialog1;
	TPopupMenu *GraphPopupMenu;
	TMenuItem *Enablespline1;
	TMenuItem *Showpoints1;
	TMenuItem *Cliptraces1;
	TMenuItem *Snaptonearestpoint1;
	TMenuItem *N8;
	TMenuItem *Graphtype1;
	TMenuItem *N4;
	TMenuItem *Addmarker1;
	TMenuItem *Deletemarker1;
	TMenuItem *Deselectmarker1;
	TMenuItem *Deltamarker2;
	TMenuItem *Markertrace1;
	TMenuItem *Alltraces1;
	TMenuItem *Trace11;
	TMenuItem *Trace21;
	TMenuItem *N5;
	TMenuItem *Showmarkers1;
	TMenuItem *Showminmaxtext1;
	TMenuItem *Showmaxmarker1;
	TMenuItem *Showminmarker1;
	TMenuItem *N1;
	TMenuItem *Setasstartfrequency1;
	TMenuItem *Setascenterfrequency1;
	TMenuItem *Setasstopfrequency1;
	TMenuItem *N6;
	TMenuItem *Automaxscale1;
	TMenuItem *Autominscale1;
	TMenuItem *Scaletominmaxvalues1;
	TMenuItem *Scaletomaxvalue1;
	TMenuItem *Scaletominvalue1;
	TMenuItem *Setmaxscale1;
	TMenuItem *Setminscale1;
	TMenuItem *N3;
	TMenuItem *Saveimage1;
	TMenuItem *N2;
	TMenuItem *Cancel1;
	TPopupMenu *GraphTypePopupMenu;
	TPaintBox *GraphPaintBox;
	TImageList *ImageList1;
	TImage *SaveImageImage;
	TImage *CloseImage;
	TMenuItem *Copyimage1;
	TImage *CopyImageImage;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall GraphPaintBoxMouseDown(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y);
	void __fastcall GraphPaintBoxMouseUp(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y);
	void __fastcall GraphPaintBoxMouseMove(TObject *Sender, TShiftState Shift,
			 int X, int Y);
	void __fastcall GraphPaintBoxMouseEnter(TObject *Sender);
	void __fastcall GraphPaintBoxMouseLeave(TObject *Sender);
	void __fastcall GraphPaintBoxPaint(TObject *Sender);
	void __fastcall GraphPaintBoxDblClick(TObject *Sender);
	void __fastcall CloseBitBtnClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormMouseEnter(TObject *Sender);
	void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta,
          TPoint &MousePos, bool &Handled);
	void __fastcall GraphPopupMenuPopup(TObject *Sender);
	void __fastcall Enablespline1Click(TObject *Sender);
	void __fastcall Showpoints1Click(TObject *Sender);
	void __fastcall Cliptraces1Click(TObject *Sender);
	void __fastcall Snaptonearestpoint1Click(TObject *Sender);
	void __fastcall Showmarkers1Click(TObject *Sender);
	void __fastcall Showminmaxtext1Click(TObject *Sender);
	void __fastcall Showmaxmarker1Click(TObject *Sender);
	void __fastcall Showminmarker1Click(TObject *Sender);
	void __fastcall Setasstartfrequency1Click(TObject *Sender);
	void __fastcall Setascenterfrequency1Click(TObject *Sender);
	void __fastcall Setasstopfrequency1Click(TObject *Sender);
	void __fastcall Automaxscale1Click(TObject *Sender);
	void __fastcall Autominscale1Click(TObject *Sender);
	void __fastcall Scaletominmaxvalues1Click(TObject *Sender);
	void __fastcall Scaletomaxvalue1Click(TObject *Sender);
	void __fastcall Scaletominvalue1Click(TObject *Sender);
	void __fastcall Setmaxscale1Click(TObject *Sender);
	void __fastcall Setminscale1Click(TObject *Sender);
	void __fastcall Addmarker1Click(TObject *Sender);
	void __fastcall Deletemarker1Click(TObject *Sender);
	void __fastcall Deselectmarker1Click(TObject *Sender);
	void __fastcall Deltamarker2Click(TObject *Sender);
	void __fastcall Alltraces1Click(TObject *Sender);
	void __fastcall Trace11Click(TObject *Sender);
	void __fastcall Trace21Click(TObject *Sender);
	void __fastcall GraphTypePopupMenuPopup(TObject *Sender);
	void __fastcall SaveImageImageClick(TObject *Sender);
	void __fastcall CloseImageClick(TObject *Sender);
	void __fastcall Saveimage1Click(TObject *Sender);
	void __fastcall Copyimage1Click(TObject *Sender);
	void __fastcall CopyImageImageClick(TObject *Sender);


private:	// User declarations
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR                    gdiplusToken;

	TWinControl *m_parent;

	int m_screen_width;
	int m_screen_height;

	Graphics::TBitmap *m_graph_bm;

	int m_popup_menu_graph_type_graph;

	int     m_popup_menu_mouse_graph;
	int64_t m_popup_menu_mouse_Hz;
	int     m_popup_menu_mouse_marker;

	int     m_popup_menu_marker_index;

	//   int m_graph;
	int m_graph_type;

	std::vector <t_pointf> m_line_points[MAX_GRAPHS][MAX_MEMORIES][MAX_CHANNELS];

	CGraphs graphs;

	void __fastcall OnMessage(tagMSG &msg, bool &handled);
	void __fastcall WMWindowPosChanging(TWMWindowPosChanging &msg);

	void __fastcall saveBitmap(Graphics::TBitmap *bm, String ID, String dialog_title);
	void __fastcall saveImage(const bool to_clipboard);

	void __fastcall createGraphTypeMenus();

	void __fastcall PopupMeasureItem(System::TObject *Sender, Vcl::Graphics::TCanvas *ACanvas, int &Width, int &Height);
	void __fastcall MenuItemGraphTypeClick(TObject *Sender);
	void __fastcall MenuItemChangeGraphTypeClick(TObject *Sender);

protected:

	#pragma option push -vi-
	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosMsg, WMWindowPosChanging);
	END_MESSAGE_MAP(TForm)
	#pragma option pop

public:		// User declarations
	__fastcall TGraphForm(TComponent* Owner, TWinControl *parent = NULL);
};

extern PACKAGE TGraphForm *GraphForm;

#endif
