
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef Unit1H
#define Unit1H

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <System.Classes.hpp>
#include <System.ImageList.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ExtDlgs.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.WinXCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ButtonGroup.hpp>

#include "SerialPort.h"
//#include "TCPIP.h"	// our own TCPIP client
#include "CriticalSection.h"
#include "HighResolutionTick.h"
#include "st_dfu.h"
#include "Graphs.h"
#include "GraphUnit.h"
#include "NanoVNA_v1_comms.h"
#include "NanoVNA_v2_comms.h"
#include "JanVNA2_comms.h"
#include "DataUnit.h"

#ifndef TCPIPH
	// use the Indy TCPIP client
	#include <IdBaseComponent.hpp>
	#include <IdComponent.hpp>
	#include <IdTCPClient.hpp>
	#include <IdTCPConnection.hpp>
#endif

// ******************************************************************************

typedef void __fastcall (__closure *mainForm_threadProcess)();

class CMainFormThread : public TThread
{
	private:
		//mainForm_threadProcess m_process;
		HANDLE m_event;
	protected:
		void __fastcall Execute()
		{
//			m_event = ::CreateEvent(NULL, TRUE, FALSE, NULL);

			while (!Terminated)
			{
				// give time to other system threads
				if (m_event != NULL)
				{
					::WaitForSingleObject(m_event, 1);
				}
				else
				{
					this->Sleep(1);
				}
				if (m_process)
				{
					#ifndef _DEBUG
						m_process();
					#else
						Synchronize(m_process);	// thread safe mode
					#endif
				}
			}

			if (m_event)
				CloseHandle(m_event);
			m_event = NULL;

			ReturnValue = 0;
		}

	public:
		__fastcall CMainFormThread(mainForm_threadProcess process, TThreadPriority priority, bool start) : TThread(!start)
		{
			m_event         = NULL;
			m_process       = process;
//			FreeOnTerminate = false;
			FreeOnTerminate = true;
			Priority        = priority;
		}
		virtual __fastcall ~CMainFormThread()
		{
			m_process = NULL;
		}
		mainForm_threadProcess m_process;
};

// ******************************************************************************

class TForm1 : public TForm
{
__published:
	TComboBox *SerialPortBaudrateComboBox;
	TComboBox *DeviceComboBox;
	TPopupMenu *GraphPopupMenu;
	TMenuItem *Enablespline1;
	TMenuItem *Cancel1;
	TMenuItem *N2;
	TMenuItem *Showpoints1;
	TStatusBar *StatusBar1;
	TTimer *Timer1;
	TComboBox *VelocityFactorComboBox;
	TTrackBar *TimeAverageLevelTrackBar;
	TLabel *Label12;
	TMenuItem *N3;
	TMenuItem *Saveimage1;
	TEdit *VelocityFactorEdit;
	TLabel *MovingAverageFilterLabel;
	TSaveDialog *SaveDialog1;
	TSpeedButton *MemorySpeedButton1;
	TSpeedButton *MemorySpeedButton2;
	TSpeedButton *MemorySpeedButton3;
	TTrackBar *TDRWindowTrackBar;
	TLabel *TDRWindowLabel2;
	TLabel *TDRWindowLabel1;
	TLabel *Label3;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label4;
	TLabel *Label13;
	TEdit *CWMHzEdit;
	TEdit *StopMHzEdit;
	TEdit *SpanMHzEdit;
	TEdit *CenterMHzEdit;
	TEdit *StartMHzEdit;
	TPanel *Panel1;
	TPanel *InfoPanel;
	TLabel *Label14;
	TLabel *Label16;
	TLabel *Label23;
	TLabel *Label19;
	TLabel *Label22;
	TLabel *Label25;
	TLabel *Label27;
	TLabel *Label29;
	TLabel *Label35;
	TLabel *Label31;
	TLabel *Label20;
	TLabel *Label53;
	TLabel *Label51;
	TLabel *MarkerS21GainLabel;
	TLabel *MarkerS21Label;
	TLabel *MarkerS21RealImagLabel;
	TLabel *MarkerS11ParallelCLabel;
	TLabel *MarkerS11ParallelLLabel;
	TLabel *MarkerS11ParallelXLabel;
	TLabel *MarkerS11ParallelRLabel;
	TLabel *MarkerS11SeriesCLabel;
	TLabel *MarkerS11SeriesLLabel;
	TLabel *MarkerS11SeriesXLabel;
	TLabel *MarkerS11SeriesRLabel;
	TLabel *MarkerS11RealImagLabel;
	TLabel *MarkerFrequencyLabel;
	TLabel *Label37;
	TLabel *Label39;
	TLabel *Label47;
	TLabel *Label41;
	TLabel *Label43;
	TLabel *Label46;
	TLabel *Label49;
	TLabel *Label34;
	TLabel *Label15;
	TLabel *MarkerS11AdmittanceLabel1;
	TLabel *Label59;
	TLabel *Label55;
	TLabel *Label58;
	TLabel *MarkerS21GroupDelayLabel;
	TLabel *MarkerS21PolarLabel;
	TLabel *MarkerS21PhaseLabel;
	TLabel *MarkerS11ImpedanceLabel;
	TLabel *MarkerS11AdmittanceLabel2;
	TLabel *MarkerS11GroupDelayLabel;
	TLabel *MarkerS11PolarLabel;
	TLabel *MarkerS11PhaseLabel;
	TLabel *MarkerS11ZLabel;
	TLabel *MarkerS11QualityFactorLabel;
	TLabel *MarkerS11Label;
	TLabel *MarkerS11ReturnLossLabel;
	TLabel *MarkerS11VSWRLabel;
	TLabel *PointBandwidthLabel;
	TLabel *PointsPerSegmentLabel;
	TLabel *S21OffsetLabel;
	TEdit *S21OffsetEdit;
	TToggleSwitch *FreqBandEnableToggleSwitch;
	TLabel *Label33;
	TEdit *SweepNameEdit;
	TLabel *SweepNameFontLabel;
	TComboBox *NumberOfPointsComboBox;
	TComboBox *PointBandwidthHzComboBox;
	TToggleSwitch *LCMatchingToggleSwitch;
	TLabel *LCMatchingLabel;
	TSpeedButton *ScanOnceSpeedButton;
	TLabel *Label40;
	TTrackBar *CurveSmoothingTrackBar;
	TLabel *CurveSmoothingLabel;
	TLabel *EDelayLabel;
	TEdit *EDelayEdit;
	TGroupBox *MarkersGroupBox;
	TListBox *MarkerListBox;
	TListView *MarkerListView;
	TToggleSwitch *ShowMarkersOnGraphToggleSwitch;
	TLabel *OutputPowerLabel2;
	TLabel *OutputPowerLabel1;
	TTrackBar *OutputPowerTrackBar;
	TBitBtn *SetScanRangeToVNAScanRangeBitBtn;
	TMenuItem *N4;
	TMenuItem *Deselectmarker1;
	TMenuItem *Deletemarker1;
	TMenuItem *Addmarker1;
	TSpeedButton *ConnectDisconnectSpeedButton;
	TSpeedButton *MemorySpeedButton0;
	TSpeedButton *RecordSpeedButton;
	TBitBtn *NewGraphBitBtn;
	TBitBtn *CloseBitBtn;
	TSpeedButton *ScanSpeedButton;
	TBitBtn *UploadFirmwareBitBtn;
	TBitBtn *SettingsBitBtn;
	TBitBtn *BatteryVoltageBitBtn;
	TBitBtn *CaptureVNAScreenBitBtn;
	TBitBtn *VNACommsBitBtn;
	TBitBtn *VNAUsartCommsBitBtn;
	TBitBtn *CalibrationBitBtn;
	TMenuItem *N1;
	TMenuItem *Setasstartfrequency1;
	TMenuItem *Setasstopfrequency1;
	TMenuItem *Setascenterfrequency1;
	TToggleSwitch *InfoPanelToggleSwitch;
	TLabel *InfoPanelLabel2;
	TButton *SetNormaliseButton;
	TMenuItem *Showmaxmarker1;
	TMenuItem *Showminmarker1;
	TMenuItem *N5;
	TMenuItem *N6;
	TMenuItem *Automaxscale1;
	TMenuItem *Autominscale1;
	TMenuItem *Setmaxscale1;
	TMenuItem *Setminscale1;
	TEdit *TCPIPAddressEdit;
	TEdit *TCPIPPortEdit;
	TPopupMenu *MarkersListBoxPopupMenu;
	TMenuItem *Deltamarker1;
	TMenuItem *N7;
	TMenuItem *Cancel2;
	TMenuItem *Deltamarker2;
	TMenuItem *Setmarkerfrequency1;
	TMenuItem *Markergraph1;
	TMenuItem *Allgraphs1;
	TMenuItem *Graph11;
	TMenuItem *Graph21;
	TMenuItem *Graph31;
	TMenuItem *Graph41;
	TMenuItem *Markertrace1;
	TMenuItem *Alltraces1;
	TMenuItem *Trace11;
	TMenuItem *Trace21;
	TPaintBox *GraphPaintBox;
	TToggleSwitch *ShowMarkerTextToggleSwitch;
	TMenuItem *Showminmaxtext1;
	TMenuItem *N8;
	TPopupMenu *GraphTypePopupMenu;
	TMenuItem *Graphtype1;
	TMenuItem *Deletemarker2;
	TMenuItem *Addmarker2;
	TMenuItem *N9;
	TMenuItem *Grapharrange1;
	TMenuItem *GraphArrange1a;
	TMenuItem *GraphArrange2a;
	TMenuItem *GraphArrange2b;
	TMenuItem *GraphArrange3a;
	TMenuItem *GraphArrange3b;
	TMenuItem *GraphArrange3c;
	TMenuItem *GraphArrange3d;
	TMenuItem *GraphArrange4a;
	TMenuItem *GraphArrange4b;
	TMenuItem *GraphArrange4c;
	TMenuItem *GraphArrange4d;
	TMenuItem *GraphArrange4e;
	TMenuItem *GraphArrange4h;
	TMenuItem *GraphArrange4i;
	TMenuItem *GraphArrange3e;
	TMenuItem *GraphArrange3f;
	TMenuItem *GraphArrange4j;
	TMenuItem *GraphArrange4k;
	TMenuItem *GraphArrange4l;
	TMenuItem *GraphArrange4m;
	TMenuItem *GraphArrange4f;
	TMenuItem *GraphArrange4g;
	TLabel *Label5;
	TTrackBar *MedianFilterTrackBar;
	TLabel *MedianFilterLabel;
	TMenuItem *Scaletomaxvalue1;
	TMenuItem *Scaletominvalue1;
	TMenuItem *Deleteallmarkers1;
	TBevel *Bevel1;
	TBevel *Bevel2;
	TBevel *Bevel3;
	TPopupMenu *MemoryPopupMenu;
	TMenuItem *SetScanRangeFromMemory1;
	TMenuItem *N10;
	TMenuItem *Cancel3;
	TMenuItem *LoadMemoryFromFile1;
	TMenuItem *SetMemory1;
	TMenuItem *ClearMemory1;
	TMenuItem *N11;
	TSpeedButton *MemorySpeedButton4;
	TTrackBar *TraceLPFTrackBar;
	TMenuItem *Cliptraces1;
	TMenuItem *Scaletominmaxvalues1;
	TMenuItem *Showmarkers1;
	TMenuItem *Snaptonearestpoint1;
	TMenuItem *Autoscalepeakhold1;
	TLabel *Label6;
	TLabel *MarkerWavelengthLabel1;
	TLabel *Label8;
	TLabel *MarkerWavelengthLabel2;
	TMenuItem *Copyimage1;
	TBitBtn *SaveGraphImageBitBtn;
	TBitBtn *SaveS1PFileBitBtn;
	TBitBtn *Save2PFileBitBtn;
	TBitBtn *SaveCSVFileBitBtn;
	TBitBtn *CopyGraphImageBitBtn;
	TBitBtn *StopScanBitBtn;
	TBitBtn *DeleteFrequencyMarkersBitBtn;
	TBitBtn *SpareBitBtn;
	TMenuItem *Memoryname1;
	TMenuItem *N12;
	TMenuItem *Clearmemoryname1;
	TStatusBar *StatusBar2;
	TSpeedButton *EnableNormaliseSpeedButton;
	TPanel *Panel2;
	TTrackBar *HistoryTrackBar;
	TBitBtn *BitBtn1;
	TLabel *Label18;
	TComboBox *CalibrationSelectComboBox;
	TSpeedButton *CWModeSpeedButton;
	TPanel *GLPanel;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall GraphPaintBoxPaint(TObject *Sender);
	void __fastcall GraphPopupMenuPopup(TObject *Sender);
	void __fastcall Enablespline1Click(TObject *Sender);
	void __fastcall Showpoints1Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall SerialPortBaudrateComboBoxChange(TObject *Sender);
	void __fastcall VelocityFactorComboBoxChange(TObject *Sender);
	void __fastcall TimeAverageLevelTrackBarChange(TObject *Sender);
	void __fastcall Saveimage1Click(TObject *Sender);
	void __fastcall VelocityFactorEditChange(TObject *Sender);
	void __fastcall TDRWindowTrackBarChange(TObject *Sender);
	void __fastcall DeviceComboBoxClick(TObject *Sender);
	void __fastcall MarkerListViewData(TObject *Sender, TListItem *Item);
	void __fastcall MarkerListViewKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall MarkerListViewChange(TObject *Sender, TListItem *Item,
			 TItemChange Change);
	void __fastcall MarkerListBoxKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall MarkerListBoxClick(TObject *Sender);
	void __fastcall DeviceComboBoxDropDown(TObject *Sender);
	void __fastcall FormDblClick(TObject *Sender);
	void __fastcall StartMHzEditKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall StopMHzEditKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall CenterMHzEditKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall SpanMHzEditKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall CWMHzEditKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall S21OffsetEditKeyDown(TObject *Sender,
          WORD &Key, TShiftState Shift);
	void __fastcall S21OffsetEditChange(TObject *Sender);
	void __fastcall GraphPaintBoxMouseDown(TObject *Sender,
			 TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall GraphPaintBoxMouseUp(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y);
	void __fastcall GraphPaintBoxMouseMove(TObject *Sender,
			 TShiftState Shift, int X, int Y);
	void __fastcall FreqBandEnableToggleSwitchClick(TObject *Sender);
	void __fastcall GraphPaintBoxMouseLeave(TObject *Sender);
	void __fastcall PointBandwidthHzComboBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall LCMatchingToggleSwitchClick(TObject *Sender);
	void __fastcall ScanOnceSpeedButtonClick(TObject *Sender);
	void __fastcall CurveSmoothingTrackBarChange(TObject *Sender);
	void __fastcall EDelayEditChange(TObject *Sender);
	void __fastcall EDelayEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ShowMarkersOnGraphToggleSwitchClick(TObject *Sender);
	void __fastcall OutputPowerTrackBarChange(TObject *Sender);
	void __fastcall MemorySpeedButtonClick(TObject *Sender);
	void __fastcall MarkerListBoxDblClick(TObject *Sender);
	void __fastcall GraphPaintBoxMouseEnter(TObject *Sender);
	void __fastcall SetScanRangeToVNAScanRangeBitBtnClick(TObject *Sender);
	void __fastcall MarkerListBoxMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall Deselectmarker1Click(TObject *Sender);
	void __fastcall Deletemarker1Click(TObject *Sender);
	void __fastcall GraphPaintBoxDblClick(TObject *Sender);
	void __fastcall Addmarker1Click(TObject *Sender);
	void __fastcall DeviceComboBoxChange(TObject *Sender);
	void __fastcall ConnectDisconnectSpeedButtonClick(TObject *Sender);
	void __fastcall SweepNameEditChange(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall RecordSpeedButtonClick(TObject *Sender);
	void __fastcall RecordSpeedButtonMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall FreqEditChange(TObject *Sender);
	void __fastcall NewGraphBitBtnClick(TObject *Sender);
	void __fastcall Panel2Resize(TObject *Sender);
	void __fastcall CloseBitBtnClick(TObject *Sender);
	void __fastcall ScanSpeedButtonClick(TObject *Sender);
	void __fastcall UploadFirmwareBitBtnClick(TObject *Sender);
	void __fastcall SettingsBitBtnClick(TObject *Sender);
	void __fastcall BatteryVoltageBitBtnClick(TObject *Sender);
	void __fastcall CaptureVNAScreenBitBtnClick(TObject *Sender);
	void __fastcall VNACommsBitBtnClick(TObject *Sender);
	void __fastcall VNAUsartCommsBitBtnClick(TObject *Sender);
	void __fastcall CalibrationBitBtnClick(TObject *Sender);
	void __fastcall Setasstartfrequency1Click(TObject *Sender);
	void __fastcall Setasstopfrequency1Click(TObject *Sender);
	void __fastcall Setascenterfrequency1Click(TObject *Sender);
	void __fastcall FormMouseEnter(TObject *Sender);
	void __fastcall InfoPanelToggleSwitchClick(TObject *Sender);
	void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta,
          TPoint &MousePos, bool &Handled);
	void __fastcall SetNormaliseButtonClick(TObject *Sender);
	void __fastcall Showmaxmarker1Click(TObject *Sender);
	void __fastcall Showminmarker1Click(TObject *Sender);
	void __fastcall Automaxscale1Click(TObject *Sender);
	void __fastcall Autominscale1Click(TObject *Sender);
	void __fastcall Setmaxscale1Click(TObject *Sender);
	void __fastcall Setminscale1Click(TObject *Sender);
	void __fastcall TCPIPAddressPortEditChange(TObject *Sender);
	void __fastcall MarkersListBoxPopupMenuPopup(TObject *Sender);
	void __fastcall Deltamarker1Click(TObject *Sender);
	void __fastcall Deltamarker2Click(TObject *Sender);
	void __fastcall Setmarkerfrequency1Click(TObject *Sender);
	void __fastcall Allgraphs1Click(TObject *Sender);
	void __fastcall Graph11Click(TObject *Sender);
	void __fastcall Graph21Click(TObject *Sender);
	void __fastcall Graph31Click(TObject *Sender);
	void __fastcall Graph41Click(TObject *Sender);
	void __fastcall Alltraces1Click(TObject *Sender);
	void __fastcall Trace11Click(TObject *Sender);
	void __fastcall Trace21Click(TObject *Sender);
	void __fastcall ShowMarkerTextToggleSwitchClick(TObject *Sender);
	void __fastcall Showminmaxtext1Click(TObject *Sender);
	void __fastcall GraphTypePopupMenuPopup(TObject *Sender);
	void __fastcall Deletemarker2Click(TObject *Sender);
	void __fastcall Addmarker2Click(TObject *Sender);
	void __fastcall GraphArrangeClick(TObject *Sender);
	void __fastcall OnGraphArrangeMeasureItem(TObject *Sender, TCanvas *ACanvas,
          int &Width, int &Height);
	void __fastcall OnMenuItemDraw(TObject *Sender, TCanvas *ACanvas, TRect &ARect,
			 bool Selected);
	void __fastcall MedianFilterTrackBarChange(TObject *Sender);
	void __fastcall Scaletomaxvalue1Click(TObject *Sender);
	void __fastcall Scaletominvalue1Click(TObject *Sender);
	void __fastcall Deleteallmarkers1Click(TObject *Sender);
	void __fastcall GraphPaintBoxGesture(TObject *Sender, const TGestureEventInfo &EventInfo,
			 bool &Handled);
	void __fastcall MemoryPopupMenuPopup(TObject *Sender);
	void __fastcall SetScanRangeFromMemory1Click(TObject *Sender);
	void __fastcall MemorySpeedButtonMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall LoadMemoryFromFile1Click(TObject *Sender);
	void __fastcall SetMemory1Click(TObject *Sender);
	void __fastcall ClearMemory1Click(TObject *Sender);
	void __fastcall TraceLPFTrackBarChange(TObject *Sender);
	void __fastcall Cliptraces1Click(TObject *Sender);
	void __fastcall Scaletominmaxvalues1Click(TObject *Sender);
	void __fastcall Showmarkers1Click(TObject *Sender);
	void __fastcall Snaptonearestpoint1Click(TObject *Sender);
	void __fastcall Autoscalepeakhold1Click(TObject *Sender);
	void __fastcall NumberOfPointsComboBoxChange(TObject *Sender);
	void __fastcall PointBandwidthHzComboBoxChange(TObject *Sender);
	void __fastcall StatusBarMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall Copyimage1Click(TObject *Sender);
	void __fastcall SaveGraphImageBitBtnClick(TObject *Sender);
	void __fastcall SaveS1PFileBitBtnClick(TObject *Sender);
	void __fastcall Save2PFileBitBtnClick(TObject *Sender);
	void __fastcall SaveCSVFileBitBtnClick(TObject *Sender);
	void __fastcall CopyGraphImageBitBtnClick(TObject *Sender);
	void __fastcall StopScanBitBtnClick(TObject *Sender);
	void __fastcall DeleteFrequencyMarkersBitBtnClick(TObject *Sender);
	void __fastcall SpareBitBtnClick(TObject *Sender);
	void __fastcall Memoryname1Click(TObject *Sender);
	void __fastcall Clearmemoryname1Click(TObject *Sender);
	void __fastcall EnableNormaliseSpeedButtonClick(TObject *Sender);
	void __fastcall HistoryTrackBarChange(TObject *Sender);
	void __fastcall MemorySpeedButtonMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall BitBtn1Click(TObject *Sender);
	void __fastcall CalibrationSelectComboBoxChange(TObject *Sender);
	void __fastcall CWModeSpeedButtonClick(TObject *Sender);
	void __fastcall GLPanelResize(TObject *Sender);
	void __fastcall GLPanelDblClick(TObject *Sender);
	void __fastcall GLPanelEndDrag(TObject *Sender, TObject *Target, int X, int Y);
	void __fastcall GLPanelGesture(TObject *Sender, const TGestureEventInfo &EventInfo,
          bool &Handled);
	void __fastcall GLPanelMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall GLPanelMouseLeave(TObject *Sender);
	void __fastcall GLPanelMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall GLPanelMouseEnter(TObject *Sender);
	void __fastcall GLPanelMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);





private:
	int         m_screen_width;
	int         m_screen_height;

//	TWindowState m_window_state;
	int          m_window_not_minimized_count;

	bool m_initialised;

	int m_memory_but_index;

	int m_popup_menu_memory_index;

	int m_popup_menu_graph_type_graph;

	int     m_popup_menu_mouse_graph;
	int64_t m_popup_menu_mouse_Hz;
	int     m_popup_menu_mouse_marker;

	int     m_popup_menu_marker_index;

	std::vector <TColor> m_info_panel_colours;

	CMainFormThread *m_thread;
	bool __fastcall startThread();
	void __fastcall stopThread();

	CGraphs graphs;

	#ifdef GraphUnitH
		std::vector <TGraphForm *> m_graph_form;
	#endif

	struct
	{
		CCriticalSectionObj cs;
		std::vector <String> list;
	} m_comm_messages;

	struct
	{
		bool enabled;
		String folder;
		String filename;
		int file_number;
//		std::vector <t_data_point> points;
	} m_record;

	std::vector <HDEVNOTIFY> m_notification_handle;

	CHighResolutionTick  m_button_timer;

	// **********
/*
	std::vector <t_usb_device> usb_devices;

	DFU_FUNCTIONAL_DESCRIPTOR m_DfuDesc;
	UINT                      m_DfuInterfaceIdx;
	UINT                      m_NbOfAlternates;

	USB_DEVICE_DESCRIPTOR m_DeviceDesc;
*/
	// **********

	void __fastcall myTerminate();

	void __fastcall onIdle(TObject *Sender, bool &done);
	void __fastcall onRestore(TObject *Sender);

	void __fastcall fetchResource(AnsiString name, std::vector <uint8_t> &buffer);

	void __fastcall sortMarkers();

	bool __fastcall updateInfoPanel2(const int graph);
	void __fastcall updateInfoPanel();

	void __fastcall poll();

	//void __fastcall updateComboDropDownWidth(TComboBox *cb, bool add_scrollbar_allowance);

	#if (__BORLANDC__ < 0x0600)
		void __fastcall OnShowHint(AnsiString &HintStr, bool &CanShow, THintInfo &HintInfo);
	#else
		void __fastcall OnShowHint(String &HintStr, bool &CanShow, THintInfo &HintInfo);
	#endif
	void __fastcall OnHint(TObject *Sender);
	void __fastcall WndProc(TMessage &msg);
	void __fastcall ActiveFormChanged(TObject *Sender);
	void __fastcall ActiveControlChanged(TObject *Sender);

	void __fastcall createGraphTypeMenus();

	void __fastcall saveSettings();

	void __fastcall updateDeviceComboBox();

	void __fastcall tcpipClientDisconnect();

	void __fastcall processEDelayEdit();
	void __fastcall processS21OffsetEdit();
	bool __fastcall processStartMHzEdit(const bool update_other_edits, const bool update_own_edit);
	bool __fastcall processStopMHzEdit(const bool update_other_edits, const bool update_own_edit);
	bool __fastcall processCenterMHzEdit(const bool update_other_edits, const bool update_own_edit);
	bool __fastcall processSpanMHzEdit(const bool update_other_edits, const bool update_own_edit);
	bool __fastcall processCWMHzEdit(const bool update_other_edits, const bool update_own_edit);
	bool __fastcall processPointBandwidthHzComboBox();
	bool __fastcall processNumberOfPointsComboBox();

	void __fastcall init();

	void __fastcall configGUI();
	void __fastcall updatePointBandwidthComboBox(const bool create = false);
	void __fastcall updateNumberOfPointsComboBox(const bool process = true);
	void __fastcall updateVelocityFactorComboBox();
	void __fastcall updateCalibrationSelectComboBox();
	void __fastcall updateOutputPowerTrackBar(const int value);
	void __fastcall updateEDelayEdit();
	void __fastcall updateS21OffsetEdit();
	void __fastcall updateStepDisplay();

	bool __fastcall scanUSB(const GUID guid, const uint16_t vid, const uint16_t pid);

	void __fastcall OnDeviceChange(TMessage &msg);

	void __fastcall WMWindowPosChanging(TWMWindowPosChanging &msg);
	void __fastcall WMInitGUI(TMessage &msg);
	void __fastcall WMConnect(TMessage &msg);
	void __fastcall WMDisconnect(TMessage &msg);
	void __fastcall WMCaptureScan(TMessage &msg);
	void __fastcall WMCaptureStop(TMessage &msg);
	void __fastcall WMScreenCapture(TMessage &msg);
	void __fastcall WMComDeviceChanged(TMessage &msg);
	void __fastcall WMUpdateEDelay(TMessage &msg);
	void __fastcall WMUpdateThresholdHz(TMessage &msg);
	void __fastcall WMUpdatePointBandwidth(TMessage &msg);
	void __fastcall WMUpdateOutputPower(TMessage &msg);
	void __fastcall WMUpdateBatteryVoltage(TMessage &msg);
	void __fastcall WMNewUnitType(TMessage &msg);
	void __fastcall WMNewData(TMessage &msg);
	void __fastcall WMIncomingPoints(TMessage &msg);
	void __fastcall WMUpdateGraph(TMessage &msg);
	void __fastcall WMRebuildFonts(TMessage &msg);
	void __fastcall WMUpdateInfoPanel(TMessage &msg);
	void __fastcall WMGraphFormClosed(TMessage &msg);
	void __fastcall WMGraphFormDestroyed(TMessage &msg);

	bool __fastcall processRxLine();

	void __fastcall fetchUserInput();

	bool __fastcall requestCapture();
	void __fastcall requestScan();

	int __fastcall processRx(t_serial_buffer &serial_buffer, bool force_print);
	void __fastcall processSerial();
	void __fastcall threadProcess();

	void __fastcall showWarnings();

	void __fastcall saveFormImage();

	void __fastcall saveImage(const bool to_clipboard);

//   int __fastcall getNumberOfPointsUserInput();

	void __fastcall updateDateTime();

	void __fastcall buildMarkerListBox();

	bool __fastcall recordDataStart();
	void __fastcall recordDataToFile(std::vector <t_data_point> &points);

	void __fastcall arrangeGraphs();

	//DWORD __fastcall saveDIBToFile(IN LPCSTR filename, IN const BITMAPINFO &bmi, IN const unsigned char *pBits);

	void __fastcall enterMarkerFrequency(const int index);

	void __fastcall PopupMeasureItem(System::TObject *Sender, Vcl::Graphics::TCanvas *ACanvas, int &Width, int &Height);
	void __fastcall MenuItemGraphTypeClick(TObject *Sender);
	void __fastcall MenuItemChangeGraphTypeClick(TObject *Sender);

	void __fastcall clearMemory(const int mem);
	void __fastcall loadMemoryFile(const int mem);
	void __fastcall setMemory(const int mem);

	void __fastcall updateNumberOfPointsIndicator(const int index = -1);

	void __fastcall cwPopupWindow(TObject *sender, const bool open, String title = "");
	void __fastcall cwPopupWindowupdatenTrackBar(TObject *Sender);
	void __fastcall cwPopupWindowOnTrackBarChange(TObject *Sender);
	void __fastcall cwPopupWindowOnClick(TObject *Sender);
	void __fastcall cwPopupWindowOnMouseLeave(TObject *Sender);

protected:
	#pragma option push -vi-
	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosMsg, WMWindowPosChanging);

		VCL_MESSAGE_HANDLER(WM_DEVICECHANGE, TMessage, OnDeviceChange);

		VCL_MESSAGE_HANDLER(WM_CONNECT, TMessage, WMConnect);
		VCL_MESSAGE_HANDLER(WM_DISCONNECT, TMessage, WMDisconnect);
		VCL_MESSAGE_HANDLER(WM_CAPTURE_SCAN, TMessage, WMCaptureScan);
		VCL_MESSAGE_HANDLER(WM_CAPTURE_STOP, TMessage, WMCaptureStop);

		VCL_MESSAGE_HANDLER(WM_COM_DEVICE_CHANGED, TMessage, WMComDeviceChanged);
		VCL_MESSAGE_HANDLER(WM_UPDATE_EDELAY, TMessage, WMUpdateEDelay);
		VCL_MESSAGE_HANDLER(WM_UPDATE_THRESHOLD_HZ, TMessage, WMUpdateThresholdHz);
		VCL_MESSAGE_HANDLER(WM_UPDATE_POINT_BANDWIDTH, TMessage, WMUpdatePointBandwidth);
		VCL_MESSAGE_HANDLER(WM_UPDATE_OUTPUT_POWER, TMessage, WMUpdateOutputPower);
		VCL_MESSAGE_HANDLER(WM_UPDATE_BATTERY_VOLTAGE, TMessage, WMUpdateBatteryVoltage);
		VCL_MESSAGE_HANDLER(WM_NEW_UNIT_TYPE, TMessage, WMNewUnitType);
		VCL_MESSAGE_HANDLER(WM_NEW_DATA, TMessage, WMNewData);
		VCL_MESSAGE_HANDLER(WM_SCREEN_CAPTURE, TMessage, WMScreenCapture);
		VCL_MESSAGE_HANDLER(WM_INCOMING_POINTS, TMessage, WMIncomingPoints);
		VCL_MESSAGE_HANDLER(WM_UPDATE_GRAPH, TMessage, WMUpdateGraph);
		VCL_MESSAGE_HANDLER(WM_REBUILD_FONTS, TMessage, WMRebuildFonts);
		VCL_MESSAGE_HANDLER(WM_UPDATE_INFO_PANEL, TMessage, WMUpdateInfoPanel);
		VCL_MESSAGE_HANDLER(WM_INIT_GUI, TMessage, WMInitGUI);

		VCL_MESSAGE_HANDLER(WM_GRAPH_FORM_CLOSED, TMessage, WMGraphFormClosed);
		VCL_MESSAGE_HANDLER(WM_GRAPH_FORM_DESTROYED, TMessage, WMGraphFormDestroyed);
	END_MESSAGE_MAP(TForm)
	#pragma option pop

public:
	__fastcall TForm1(TComponent* Owner);

	struct
	{
		int                  connect_timer_tick;

		CHighResolutionTick  points_speed_timer;
		int                  points_per_sec_buf_count;
		float                points_per_sec_buf[9];	// history buffer .. better to be even in size (median)
		float                points_per_sec;			// history buffer output

		t_serial_buffer      rx;
		CHighResolutionTick  rx_timer;

		CHighResolutionTick  link_timer;

		CSerialPort          serial;

		#ifdef TCPIPH
			CTCPIP            tcpip;
		#else
			TIdTCPClient      *tcpip;
			TMemoryStream     *tcpip_stream_rx;
		#endif

	} m_comms;

	std::vector <t_data_point> m_freq_data_list;

	void __fastcall clearCommMessages();
	unsigned int __fastcall commMessagesCount();
	void printfCommMessage(const char *fmt, ...);
	void __fastcall pushCommMessage(String s);
	String __fastcall pullCommMessage();

	void __fastcall resetFreqArray();

	bool __fastcall connected();
	bool __fastcall scanning();
	bool __fastcall generating();
	t_mode __fastcall commsMode();

	void __fastcall waitTxCommandEmpty(const DWORD max_ms)
	{
		if (connected())
		{	// wait for the tx command queue to empty
			const DWORD tick = GetTickCount();
			while ((GetTickCount() - tick) < max_ms && serialCommandsSize() > 0)
				Application->ProcessMessages();
		}
	}

	int __fastcall serialCommandsSize()
	{
		return (connected() && data_unit.m_vna_data.type != UNIT_TYPE_NANOVNA_V2 && data_unit.m_vna_data.type != UNIT_TYPE_JANVNA_V2) ?
			nanovna1_comms.serialCommandsSize() : 0;
	}

	void __fastcall insertSerialTxCommand(String s, const int command_ends = 0)
	{
		if (connected() && data_unit.m_vna_data.type != UNIT_TYPE_NANOVNA_V2 && data_unit.m_vna_data.type != UNIT_TYPE_JANVNA_V2)
			nanovna1_comms.insertSerialTxCommand(s, command_ends);
	}

	void __fastcall addSerialTxCommand(String s, const int command_ends = 0)
	{
		if (connected() && data_unit.m_vna_data.type != UNIT_TYPE_NANOVNA_V2 && data_unit.m_vna_data.type != UNIT_TYPE_JANVNA_V2)
			nanovna1_comms.addSerialTxCommand(s, command_ends);
	}

	void __fastcall addSerialTxCommand(std::vector <uint8_t> &data)
	{
		if (connected() && data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2 && data_unit.m_vna_data.type != UNIT_TYPE_JANVNA_V2 && nanovna2_comms.mode == MODE_IDLE)
			nanovna2_comms.sendData(&data[0], data.size());
	}

	void __fastcall sendOutputPowerCommand()
	{
		if (connected())
		{
			if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
			{
				const float level   = (float)(OutputPowerTrackBar->Position - OutputPowerTrackBar->Min) / (OutputPowerTrackBar->Max - OutputPowerTrackBar->Min);	// 0.0 to 1.0
				const float dBm_max = janvna2_comms.maxOutputPowerdBm();
				const float dBm_min = janvna2_comms.minOutputPowerdBm();
				const float dBm     = dBm_min + (ABS(dBm_max - dBm_min) * level); // dBm_min to dBm_max
				janvna2_comms.setOutputPower(dBm);
			}
			else
			if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
			{	// V2
				nanovna2_comms.setADF4350OutputPower(settings.outputPower);
			}
			else
			{  // V1
				nanovna1_comms.sendOutputPowerCommand(settings.outputPower);
			}
		}
	}

	void __fastcall sendBandwidthCommand()
	{
		if (!connected()) return;
		if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2) {
			nanovna2_comms.setAverageSetting(data_unit.m_bandwidth_Hz);
			return;
		}
		if (data_unit.m_vna_data.type != UNIT_TYPE_JANVNA_V2)
			nanovna1_comms.sendBandwidthCommand();
	}

	void __fastcall sendTimeCommand()
	{
		if (connected())
		{
			String s;
			TDateTime now = Now();
			unsigned short year;
			unsigned short month;
			unsigned short day;
			unsigned short hour;
			unsigned short min;
			unsigned short sec;
			unsigned short msec;
			now.DecodeDate(&year, &month, &day);
			now.DecodeTime(&hour, &min, &sec, &msec);
			if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
			{

			}
			else
			if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
			{	// V2
				nanovna2_comms.setTime(year, month, day, hour, min, sec);
			}
			else
			{  // V1
				// time b 0x200831 0x233315  .. set 2020/08/31 23:33:15
				s.printf(L"time b 0x%02u%02u%02u 0x%02u%02u%02u", year % 100, month, day,hour, min, sec);
				nanovna1_comms.addSerialTxCommand(s);
				/*
				s.printf(L"time y %u", (year >= 2000) ? year - 2000 : (year >= 1900) ? year - 1900 : year);
				nanovna1_comms.addSerialTxCommand(s);
				s.printf(L"time m %u", month);
				nanovna1_comms.addSerialTxCommand(s);
				s.printf(L"time d %u", day);
				nanovna1_comms.addSerialTxCommand(s);
				s.printf(L"time h %u", hour);
				nanovna1_comms.addSerialTxCommand(s);
				s.printf(L"time min %u", min);
				nanovna1_comms.addSerialTxCommand(s);
				s.printf(L"time sec %u", sec);
				nanovna1_comms.addSerialTxCommand(s);
				*/
				nanovna1_comms.addSerialTxCommand("time");
			}
		}
	}

	void __fastcall disconnect();
	void __fastcall connect();

	void __fastcall scan();
	void __fastcall stop();

	int __fastcall fetchCommsData(const bool clear_rx_buffer);

	void __fastcall setStartStopHz(int64_t low_Hz, int64_t high_Hz, bool start_scan);

	bool __fastcall requestMouseDown(int x, int y);
	bool __fastcall requestMouseUp(void);
	bool __fastcall autoRefresh(bool enable);

	void __fastcall getVNAScreenCapture();

	void __fastcall addNewRxData(std::vector <t_data_point> &new_points);

	void __fastcall pauseComms(bool pause);

	void __fastcall addMarker(const int64_t freq_Hz, const int type = MARKER_TYPE_NORMAL, const int graph = -1, const int mem = -1, const int trace = -1);
	void __fastcall deleteMarker(const int index);
	void __fastcall moveMarker(const int index, const int64_t freq_Hz);
	int __fastcall nonDeltaMarker(int start_index = 0);
	int __fastcall selectedMarker();

	void __fastcall setCalibrationSelection(t_calibration_selection cal_select);

	void __fastcall updateHistoryFramesInfo(const bool update = false);
};

extern PACKAGE TForm1 *Form1;

// ******************************************************************************

#endif

