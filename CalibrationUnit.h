
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef CalibrationUnitH
#define CalibrationUnitH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <System.ImageList.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Graphics.hpp>

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
#include "median_filter.h"
#include "smoothing_filter.h"

typedef enum : int
{
	CAL_TYPE_NONE = 0,
	CAL_TYPE_SHORT,
	CAL_TYPE_OPEN,
	CAL_TYPE_LOAD_ISOLN,
	CAL_TYPE_THROUGH
} t_cal_type;

class TCalibrationForm : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *SaveAsFileBitBtn;
	TBitBtn *LoadFileBitBtn;
	TGridPanel *GridPanel1;
	TButton *ClearCalibrationButton;
	TLabel *Label6;
	TEdit *CalibrationPathEdit;
	TBitBtn *SelectFilePathBitBtn;
	TPopupMenu *ListPopupMenu;
	TMenuItem *LoadCalibrationFile1;
	TMenuItem *DeleteCalibrationFile1;
	TMenuItem *N1;
	TMenuItem *Cancel1;
	TBitBtn *LoadBitBtn;
	TImage *CheckImage1;
	TImage *CheckImage2;
	TMenuItem *RenameCalibrationFile1;
	TBitBtn *ExportFilesBitBtn;
	TBitBtn *SaveFileBitBtn;
	TBitBtn *ShortBitBtn;
	TBitBtn *OpenBitBtn;
	TMenuItem *CopyCalibrationName1;
	TLabel *Label2;
	TTrackBar *AverageTrackBar;
	TLabel *AverageLabel;
	TBitBtn *ThroughBitBtn;
	TTrackBar *SmoothingTrackBar;
	TLabel *SmoothingLabel;
	TLabel *Label40;
	TTrackBar *MedianTrackBar;
	TLabel *MedianLabel;
	TLabel *Label4;
	TPopupMenu *CalButtonPopupMenu;
	TMenuItem *ClearCal1;
	TMenuItem *N2;
	TMenuItem *Cancel2;
	TMenuItem *Stop1;
	TButton *SetMainSweepButton;
	TLabel *CalibrationNameLabel;
	TListView *CalibrationFilesListView;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormMouseEnter(TObject *Sender);
	void __fastcall LoadFileBitBtnClick(TObject *Sender);
	void __fastcall ClearCalibrationButtonClick(TObject *Sender);
	void __fastcall CalibrationFilesListViewData(TObject *Sender, TListItem *Item);
	void __fastcall CalibrationFilesListViewKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall SelectFilePathBitBtnClick(TObject *Sender);
	void __fastcall CalibrationPathEditChange(TObject *Sender);
	void __fastcall CalibrationFilesListViewDblClick(TObject *Sender);
	void __fastcall ListPopupMenuPopup(TObject *Sender);
	void __fastcall LoadCalibrationFile1Click(TObject *Sender);
	void __fastcall DeleteCalibrationFile1Click(TObject *Sender);
	void __fastcall CalibrationFilesListViewMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall CalTypeBitBtnClick(TObject *Sender);
	void __fastcall RenameCalibrationFile1Click(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall ExportFilesBitBtnClick(TObject *Sender);
	void __fastcall SaveFileBitBtnClick(TObject *Sender);
	void __fastcall SaveAsFileBitBtnClick(TObject *Sender);
	void __fastcall CalibrationFilesListViewMouseLeave(TObject *Sender);
	void __fastcall CalibrationFilesListViewSelectItem(TObject *Sender, TListItem *Item,
			 bool Selected);
	void __fastcall CopyCalibrationName1Click(TObject *Sender);
	void __fastcall AverageTrackBarChange(TObject *Sender);
	void __fastcall SmoothingTrackBarChange(TObject *Sender);
	void __fastcall MedianTrackBarChange(TObject *Sender);
	void __fastcall CalButtonBitBtnMouseDown(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y);
	void __fastcall CalButtonPopupMenuPopup(TObject *Sender);
	void __fastcall ClearCal1Click(TObject *Sender);
	void __fastcall SetMainSweepButtonClick(TObject *Sender);
	void __fastcall CalibrationFilesListViewChange(TObject *Sender, TListItem *Item,
          TItemChange Change);

private:	// User declarations
	bool m_initialised;

	Gdiplus::GdiplusStartupInput m_gdi_plus_startup_input;
	ULONG_PTR                    m_gdi_plus_token;
	Gdiplus::Graphics            *m_gdi_plus;

	Graphics::TBitmap *m_graph_bm;

	std::vector <Gdiplus::PointF> m_gdi_points;

	int m_cal_button_popup_menu_index;
	int m_list_popup_menu_index;

	std::vector < complexf > m_cal_samples[MAX_CHANNELS];

	typedef struct
	{
		String  filename;
		String  name;
		int64_t start_Hz;
		int64_t stop_Hz;
		int     points;
	} t_calibration_file_list;

	std::vector <t_calibration_file_list> m_cal_file_list;

	t_cal_type m_measure_type;
	int m_measure_count;

	int m_average_counts;

	CMedianFilter    m_median_filter;
	CSmoothingFilter m_smoothing_filter;

	void __fastcall WMWindowPosChanging(TWMWindowPosChanging &msg);

	void __fastcall applyCalibration();

	void __fastcall updateCalibrationFileList(String directory);

	void __fastcall filterValues(std::vector < complexf > &values, const int median_size, const int smooth_level);

	bool __fastcall areCalibrationsEqual(const t_calibration *cal1, const t_calibration *cal2);

	void __fastcall setButtonGlyph(TBitBtn *but, const int image_index);
	void __fastcall setButtonStates();

	void __fastcall updateLabels();

protected:
	#pragma option push -vi-
	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosMsg, WMWindowPosChanging);
	END_MESSAGE_MAP(TForm)
	#pragma option pop

public:		// User declarations
	__fastcall TCalibrationForm(TComponent* Owner);

	t_calibration m_calibration;

	void __fastcall loadCalibration(String filename);

	void __fastcall show();
	void __fastcall scanComplete(std::vector <t_data_point> &points);
	void __fastcall stop();

	__property t_cal_type calType = {read = m_measure_type};
};

extern PACKAGE TCalibrationForm *CalibrationForm;

#endif

