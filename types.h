
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef typesH
#define typesH

#include <System.hpp>
#include <Vcl.Graphics.hpp>

#include <stdint.h>
#include <vector>

#include "complex.h"

#define WM_INIT_GUI						(WM_USER + 100)
#define WM_CONNECT						(WM_USER + 101)
#define WM_DISCONNECT					(WM_USER + 102)
#define WM_CAPTURE_SCAN					(WM_USER + 103)
#define WM_CAPTURE_STOP					(WM_USER + 104)
#define WM_UPDATE_GRAPH					(WM_USER + 105)
#define WM_UPDATE_INFO_PANEL			(WM_USER + 106)
#define WM_SCREEN_CAPTURE				(WM_USER + 107)
#define WM_INCOMING_POINTS				(WM_USER + 108)
#define WM_GRAPH_FORM_CLOSED			(WM_USER + 109)
#define WM_GRAPH_FORM_DESTROYED		(WM_USER + 110)
#define WM_NEW_DATA						(WM_USER + 111)
#define WM_NEW_UNIT_TYPE            (WM_USER + 112)
#define WM_UPDATE_BATTERY_VOLTAGE   (WM_USER + 113)
#define WM_UPDATE_OUTPUT_POWER	   (WM_USER + 114)
#define WM_UPDATE_POINT_BANDWIDTH	(WM_USER + 115)
#define WM_UPDATE_THRESHOLD_HZ		(WM_USER + 116)
#define WM_UPDATE_EDELAY				(WM_USER + 117)
#define WM_COM_DEVICE_CHANGED			(WM_USER + 118)
#define WM_REBUILD_FONTS				(WM_USER + 119)

#define SWP_STATECHANGED            0x8000
#define WINDOW_SNAP                 5

#define METERS_TO_FEET              3.28084

#define MAX_CHANNELS                4	// S11 S21 S12 S22
#define MAX_CAL_TRACES					5	// open short load isolation through

#define MAX_HISTORY_POWER				8	// must be >= 0
#define MAX_HISTORY						(1 << MAX_HISTORY_POWER)

#define MAX_GRAPHS						4

#define MAX_MEMORIES						(1 + 4)

#define MAX_FREQ_BANDS              64

#define GRID_ALPHA                  128

#ifdef __BORLANDC__
	#if (__BORLANDC__ < 0x0600)
		#define cosf(x)                 (float)cos(x)
		#define sinf(x)                 (float)sin(x)
		#define sqrtf(x)                (float)sqrt(x)
		#define fabsf(x)                (float)fabs(x)
		#define ceilf(x)                (float)ceil(x)
		#define floorf(x)               (float)floor(x)
		#define log10f(x)               (float)log10(x)
		#define atan2f(y, x)            (float)atan2(y, x)
	#else
		#define ceilf(x)                (float)ceil(x)
		#define floorf(x)               (float)floor(x)
		#define fabsf(x)                (float)fabs(x)
		#define log10f(x)               (float)log10(x)
	#endif
#endif

// **************

#define ARRAY_SIZE(array)       (sizeof(array) / sizeof(array[0]))
#define SQR(x)                  ((x) * (x))
#define IROUND(x)               ((int)floor((x) + 0.5))
#define I64ROUND(x)             ((int64_t)floor((x) + 0.5))
#define MIN(a, b)               (((a) < (b)) ? (a) : (b))
#define MAX(a, b)               (((a) > (b)) ? (a) : (b))
#define ABS(x)                  (((x) >= 0) ? (x) : -(x))

// **************

#define SPEED_OF_LIGHT                 299792458 // meters per sec

#define MIN_WAV_SIZE                   44 // bytes

// this MIN/MAX is just a general frequency limit to use when loading the settings
// actual frequency limits aren't known until we are connected to a unit (actual actual limits shown further down below)
#define MAX_FREQ                       10000000000LL  // 10GHz
#define MIN_FREQ                       0LL            // 0Hz

#define MAX_VNA_V1_FREQ_HZ             0xffffffffLL   // max unsigned 32-bit value
#define MIN_VNA_V1_FREQ_HZ             800LL

#define MAX_VNA_V2_FREQ_HZ             6500000000LL
#define MIN_VNA_V2_FREQ_HZ             1500LL

#define MAX_VNA_JANVNAV2_FREQ_HZ       6000000000LL
#define MIN_VNA_JANVNAV2_FREQ_HZ       1LL

#define MAX_TINYSA_FREQ_HZ             950000000LL
#define MIN_TINYSA_FREQ_HZ             1000LL

#define MAX_FREQ_STEPS                 10000

#define DEFAULT_JANVNAV2_MAX_POINTS    4501
#define DEFAULT_MAX_POINTS             101

#define DEFAULT_JANVNAV2_MIN_IF_BW     6
#define DEFAULT_JANVNAV2_MAX_IF_BW     50000
#define DEFAULT_JANVNAV2_MAX_POINTS    4501
#define DEFAULT_JANVNAV2_MIN_CDBM      -40
#define DEFAULT_JANVNAV2_MAX_CDBM      0
#define DEFAULT_JANVNAV2_MIN_RBW       13
#define DEFAULT_JANVNAV2_MAX_RBW       111500

#define MAX_S_PARAM_VALUE            100.0f

//#define FFT_SIZE                     1024
#define FFT_SIZE                     2048

#define VSWR_GAMMA_DEFAULT           2.5

// **************
// graph scale limits

#define LOG_MAG_MAX						 200
#define LOG_MAG_MIN						-200

#define LIN_MAG_MAX						1000
#define LIN_MAG_MIN						0

#define PHASE_DEG_MAX                20000
#define PHASE_DEG_MIN               -20000

#define GROUP_DELAY_MAX              10000
#define GROUP_DELAY_MIN             -10000

#define RI_MAX                       1000
#define RI_MIN                      -1000

#define IMPEDANCE_MAX               200000
#define IMPEDANCE_MIN               0

#define QUALITY_FACTOR_MAX          200000
#define QUALITY_FACTOR_MIN				0

#define RJX_MAX							 200000
#define RJX_MIN                     -200000

#define VSWR_MAX							100000
#define VSWR_MIN							1

#define GAMMA_MAX							10.0
#define GAMMA_MIN							1.0

#define CAP_MAX							 1.0f
#define CAP_MIN							-1.0f

#define IND_MAX							 1.0f
#define IND_MIN							-1.0f

// **************

#define MARKER_SIZE						3

// t_marker
#define MARKER_TYPE_NORMAL				0
#define MARKER_TYPE_DELTA				1

// **************
// serial link timeout values

#define DEFAULT_POLL_V1_MS					6000
#define DEFAULT_STATE_TIME_OUT_V1_MS	3000
#define DEFAULT_LINK_TIME_OUT_V1_MS		15000

#define DEFAULT_POLL_V2_MS					6000
#define DEFAULT_STATE_TIME_OUT_V2_MS	6000

#define DEFAULT_POLL_JANVNAV2_MS			6000

// **************

//#define FORM_ALPHA_BLEND_VALUE      190	// 0 to 255 .. 0 = fully transparent, 255 = fully opaque
//#define FORM_ALPHA_BLEND_VALUE      240	// 0 to 255 .. 0 = fully transparent, 255 = fully opaque
#define FORM_ALPHA_BLEND_VALUE      255	// 0 to 255 .. 0 = fully transparent, 255 = fully opaque

// **************

#define VNA_MAX_USER_COMMANDS  							4
#define VNA_COMMS_COMMAND_HISTORY_FILENAME			"vna_comms_command_history.txt";
#define VNA_USART_COMMS_COMMAND_HISTORY_FILENAME	"vna_usart_comms_command_history.txt";

// **************
// DiSlords firmware colour list

//'color idx RGB888'
//There - idx is color index:
#define LCD_BG_COLOR             0
#define LCD_FG_COLOR             1
#define LCD_GRID_COLOR           2
#define LCD_MENU_COLOR           3
#define LCD_MENU_TEXT_COLOR      4
#define LCD_MENU_ACTIVE_COLOR    5
#define LCD_TRACE_1_COLOR        6
#define LCD_TRACE_2_COLOR        7
#define LCD_TRACE_3_COLOR        8
#define LCD_TRACE_4_COLOR        9
#define LCD_NORMAL_BAT_COLOR    10
#define LCD_LOW_BAT_COLOR       11
#define LCD_SPEC_INPUT_COLOR    12
#define LCD_RISE_EDGE_COLOR     13
#define LCD_FALLEN_EDGE_COLOR   14
#define LCD_SWEEP_LINE_COLOR    15
#define LCD_BW_TEXT_COLOR       16  // BW text at bottom
#define LCD_INPUT_TEXT_COLOR    17  // input text color (on keyboard screen)
#define LCD_INPUT_BG_COLOR      18  // input background color
#define LCD_LC_MATCH_COLOR      19

// **************
// TDR FFT types

#define TD_NONE   			0
#define TD_BANDPASS			1
#define TD_LOWPASS_IMPULSE	2
#define TD_LOWPASS_STEP		3

// **************
// graph arrangements

enum t_graph_arrange : int
{
	GRAPH_ARRANGE_1 = 0,		// 1 graph
	GRAPH_ARRANGE_1L_1R,		// 1 left   1 right
	GRAPH_ARRANGE_1T_1B,		// 1 top    1 bottom
	GRAPH_ARRANGE_1T_2B,		// 1 top    2 bottom
	GRAPH_ARRANGE_2L_1R,		// 2 left   1 right
	GRAPH_ARRANGE_2T_1B,		// 2 top    1 bottom
	GRAPH_ARRANGE_1L_2R,		// 1 left   2 right
	GRAPH_ARRANGE_3D,			// 3 down
	GRAPH_ARRANGE_3A,			// 3 across
	GRAPH_ARRANGE_4S,			// 4 graphs in a square
	GRAPH_ARRANGE_4A,			// 4 across
	GRAPH_ARRANGE_4D,			// 4 down
	GRAPH_ARRANGE_3T_1B,		// 3 top    1 bottom
	GRAPH_ARRANGE_3L_1R,		// 3 left   1 right
	GRAPH_ARRANGE_1T_3B,		// 1 top    3 bottom
	GRAPH_ARRANGE_1L_3R,		// 1 left   3 right
	GRAPH_ARRANGE_1T_1M_2B,	// 1 top    1 middle   2 bottom
	GRAPH_ARRANGE_1T_2M_1B,	// 1 top    2 middle   1 bottom
	GRAPH_ARRANGE_2T_1M_1B, // 2 top    1 middle   1 bottom
	GRAPH_ARRANGE_1L_1M_2R,	// 1 left   1 middle   2 right
	GRAPH_ARRANGE_1L_2M_1R,	// 1 left   2 middle   1 right
	GRAPH_ARRANGE_2L_1M_1R	// 2 left   1 middle   1 right
};

// **************
// graph types

#define GRAPH_TYPE_LOGMAG_S11             0
#define GRAPH_TYPE_LOGMAG_S21             1
#define GRAPH_TYPE_LOGMAG_S11S21          2
#define GRAPH_TYPE_LINMAG_S11             3
#define GRAPH_TYPE_LINMAG_S21             4
#define GRAPH_TYPE_LINMAG_S11S21          5
#define GRAPH_TYPE_PHASE_S11              6
#define GRAPH_TYPE_PHASE_S21              7
#define GRAPH_TYPE_PHASE_S11S21           8
#define GRAPH_TYPE_PHASE_UNWRAP_S11       9
#define GRAPH_TYPE_PHASE_UNWRAP_S21       10
#define GRAPH_TYPE_PHASE_UNWRAP_S11S21    11
#define GRAPH_TYPE_VSWR_S11               12
#define GRAPH_TYPE_IMPEDANCE_S11          13
#define GRAPH_TYPE_SERIES_RJX_S11         14
#define GRAPH_TYPE_PARALLEL_RJX_S11       15
#define GRAPH_TYPE_QUALITY_FACTOR_S11     16
#define GRAPH_TYPE_SMITH_S11              17
#define GRAPH_TYPE_SMITH_S21              18
#define GRAPH_TYPE_ADMITTANCE_S11         19
#define GRAPH_TYPE_ADMITTANCE_S21         20
#define GRAPH_TYPE_POLAR_S11              21
#define GRAPH_TYPE_POLAR_S21              22
#define GRAPH_TYPE_GROUP_DELAY_S11        23
#define GRAPH_TYPE_GROUP_DELAY_S21        24
#define GRAPH_TYPE_GROUP_DELAY_S11S21     25
#define GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11 26
#define GRAPH_TYPE_TDR_LIN_BP_S11         27
#define GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11 28
#define GRAPH_TYPE_TDR_LOG_BP_S11         29
#define GRAPH_TYPE_TDR_IMPEDANCE_S11      30
#define GRAPH_TYPE_REAL_IMAG_S11          31
#define GRAPH_TYPE_REAL_IMAG_S21          32
#define GRAPH_TYPE_SERIES_CAPACITANCE_S11 33
#define GRAPH_TYPE_SERIES_INDUCTANCE_S11  34
#define GRAPH_TYPE_SERIES_RESISTANCE_S11  35
#define GRAPH_TYPE_SERIES_REACTANCE_S11   36
#define GRAPH_TYPE_COAX_LOSS_S11          37
#define GRAPH_TYPE_CAL_LOGMAG   				38
#define GRAPH_TYPE_PHASE_VECTOR_S11			39
#define GRAPH_TYPE_PHASE_VECTOR_S21			40
#define GRAPH_TYPE_GJB_S11                41
#define GRAPH_TYPE_RETLOSS_S11            42
#define GRAPH_TYPE_SIZE                   43	// set this one to the number of graph types their are

// **************

enum t_calibration_selection : int
{
	CAL_SELECT_NONE = 0,
	CAL_SELECT_VNA,
	CAL_SELECT_APP
};

// remote unit type
enum t_unit_type : int
{
	UNIT_TYPE_NONE = 0,
	UNIT_TYPE_UNKNOWN,
	UNIT_TYPE_NANOVNA_H,
	UNIT_TYPE_NANOVNA_H4,
	UNIT_TYPE_NANOVNA_H7,
	UNIT_TYPE_NANOVNA_V2,
	UNIT_TYPE_JANVNA_V2,
	UNIT_TYPE_TINYSA
};

// serial connection mode/state
enum t_mode : int
{
	MODE_NONE = 0,		// unconnected
	MODE_INIT1,			// connection init
	MODE_INIT2,	 		// connection init
	MODE_INIT3,	 		// connection init
	MODE_INIT4,	 		// connection init
	MODE_IDLE,	 		// doing nothing .. this must NOT be before the previous INIT modes
	MODE_POLL,	 		// this must NOT be before IDLE
	MODE_SINGLE_SCAN,	// a single scan .. this must NOT be before the IDLE mode
	MODE_SCAN,	 		// continuous scanning .. this must NOT be before the IDLE mode
	MODE_GENERATOR,	// CW output mode
	MODE_CAPTURE,		// capturing the V2 screen
	MODE_DFU_MODE		// uploading flash data to the V2
};

// serial command state
enum t_serial_state : int
{
	SERIAL_STATE_IDLE = 0,
	SERIAL_STATE_HELP,
	SERIAL_STATE_VERSION,
	SERIAL_STATE_INFO,
	SERIAL_STATE_MARKER,
	SERIAL_STATE_SWEEP,
	SERIAL_STATE_STAT,
	SERIAL_STATE_TIME,
	SERIAL_STATE_VBAT,
	SERIAL_STATE_VBAT_OFFSET,
	SERIAL_STATE_INTEGRATOR,
	SERIAL_STATE_BANDWIDTH,
	SERIAL_STATE_THRESHOLD,
	SERIAL_STATE_EDELAY,
	SERIAL_STATE_DEVICE_ID,
	SERIAL_STATE_CAL,
	SERIAL_STATE_POWER,
	SERIAL_STATE_USART,
	SERIAL_STATE_USART_CFG,
	SERIAL_STATE_CLEARCONFIG,
	SERIAL_STATE_SAVECONFIG,
	SERIAL_STATE_SCAN,
	SERIAL_STATE_SCAN_BIN,
	SERIAL_STATE_SCANRAW,
	SERIAL_STATE_PAUSE,
	SERIAL_STATE_RESUME,
	SERIAL_STATE_RESET,
	SERIAL_STATE_DATA0,
	SERIAL_STATE_DATA1,
	SERIAL_STATE_DATA2,
	SERIAL_STATE_DATA3,
	SERIAL_STATE_DATA4,
	SERIAL_STATE_DATA5,
	SERIAL_STATE_DATA6,
	SERIAL_STATE_DATA7,
	SERIAL_STATE_FREQDATA,
	SERIAL_STATE_FREQDATA_BIN,
	SERIAL_STATE_FREQDATA_RAW,
	SERIAL_STATE_SCREEN_CAPTURE,
	SERIAL_STATE_SCREEN_FILL,
	SERIAL_STATE_SCREEN_BULK,
	SERIAL_STATE_SD_LIST,
	SERIAL_STATE_SD_READFILE,
	SERIAL_STATE_MODE
};

// *****************************************************************

struct TVersion
{
	uint16_t MajorVer;
	uint16_t MinorVer;
	uint16_t ReleaseVer;
	uint16_t BuildVer;

	TVersion() :
		MajorVer(0),
		MinorVer(0),
		ReleaseVer(0),
		BuildVer(0)
	{
	}
};

struct t_marker_freq
{
	int64_t Hz;			// frequency
	int     type;		// normal/delta
	int     graph;		// graph  number ( <0 = all graphs)
	int     mem;		// memory number ( <0 = all memories)
	int     trace;		// trace  number ( <0 = all traces)
};

struct t_marker_time
{
	double  secs;	// seconds
	int     type;	// normal/delta
	int     graph;	// graph number ( <0 = all graphs)
	int     mem;	// memory number ( <0 = all memories)
	int     trace;	// trace number ( <0 = all traces)
};

struct t_window_pos
 {
	int  left;
	int  top;
	int  width;
	int  height;
	bool showing;
 };

struct t_graph_setting
{
	bool   auto_max;
	bool   auto_min;
	double max;
	double min;
	double gamma;
	bool   show_min_max;
	bool   show_max_marker;
	bool   show_min_marker;
	bool   show_markers;
	int    auto_max_hold_count;
	int    auto_min_hold_count;
};

struct t_serial_buffer
{
	std::vector <uint8_t> buffer;
	uint32_t              buffer_wr;
};

struct t_usb_device
{
	int    index;
	String path;
	String description;
	String enumerator_name;
	String friendly_name;
	String location_information;
	String mfg;
	String service;
	int    vid;
	int    pid;
};

struct t_colour_scheme
{
	TColor background;
	TColor border;
	TColor grid;
	TColor mouse_line;
	TColor marker_line;
	TColor marker;
	TColor marker_selected;
	TColor font;
	TColor vswr2_line;
	TColor mouse_marker;
	TColor point;
//	TColor line[MAX_MEMORIES][MAX_CHANNELS];
	TColor line[MAX_MEMORIES][MAX_CAL_TRACES];
};

struct t_freq_band_c
{
	char    name[256];
	int64_t low_Hz;
	int64_t high_Hz;
	bool    enabled;
};

struct t_freq_band
{
	String  name;
	int64_t low_Hz;
	int64_t high_Hz;
	bool    enabled;
};

struct t_pointf
{
	float x;
	float y;
};

struct t_data_point
{
	union
	{
		int64_t Hz;
		double  time;
	};

	complexf sParam[MAX_CHANNELS];

	t_data_point()
	{
		Hz = 0;
		sParam[0] = complexf(0, 0);
		sParam[1] = complexf(0, 0);
		sParam[2] = complexf(0, 0);
		sParam[3] = complexf(0, 0);
	}

	__property complexf s11 = {read = sParam[0], write = sParam[0]};
	__property complexf s21 = {read = sParam[1], write = sParam[1]};
	__property complexf s12 = {read = sParam[2], write = sParam[2]};
	__property complexf s22 = {read = sParam[3], write = sParam[3]};
};

struct t_data_point_hist
{
	union
	{
		int64_t Hz;
		double  time;
	};

	t_data_point hist[MAX_HISTORY];
/*
	struct
	{
		complexf sParam[MAX_CHANNELS];
		__property complexf s11 = {read = sParam[0], write = sParam[0]};
		__property complexf s21 = {read = sParam[1], write = sParam[1]};
		__property complexf s12 = {read = sParam[2], write = sParam[2]};
		__property complexf s22 = {read = sParam[3], write = sParam[3]};
	} hist[MAX_HISTORY];
*/
	t_data_point_hist() : Hz(0) {}
};

struct t_calibration_point
{
	int64_t  m_Hz;

	complexf m_open;
	complexf m_short;
	complexf m_load;
	complexf m_isolation;
	complexf m_through;

	// S11
	complexf m_e00;		// directivity
	complexf m_e11;		// port match
	complexf m_delta_e;	// tracking

	// S21
	complexf m_e30;		// port match
	complexf m_e10e32;	// transmission

	t_calibration_point()
	{
		m_Hz = 0;
	}

	complexf __fastcall getSParam(const int chan)
	{
		switch (chan)
		{
			case 0: return m_open;
			case 1: return m_short;
			case 2: return m_load;
			case 3: return m_isolation;
			case 4: return m_through;
			default: return complexf(0);
		}
	}

	void __fastcall setSParam(const int chan, const complexf sparam)
	{
		switch (chan)
		{
			case 0: m_open      = sparam; break;
			case 1: m_short     = sparam; break;
			case 2: m_load      = sparam; break;
			case 3: m_isolation = sparam; break;
			case 4: m_through   = sparam; break;
		}
	}

	complexf __fastcall getErrorTerm(const int index)
	{
		switch (index)
		{
			case 0: return m_e00;
			case 1: return m_e11;
			case 2: return m_delta_e;
			case 3: return m_e30;
			case 4: return m_e10e32;
			default: return complexf(0);
		}
	}

	void __fastcall setErrorTerm(const int index, const complexf et)
	{
		switch (index)
		{
			case 0: m_e00     = et; break;
			case 1: m_e11     = et; break;
			case 2: m_delta_e = et; break;
			case 3: m_e30     = et; break;
			case 4: m_e10e32  = et; break;
		}
	}

	__property int64_t HzCal = {read = m_Hz, write = m_Hz};

	// sampled s-params
	__property complexf openCal                = {read = m_open,      write = m_open};
	__property complexf shortCal               = {read = m_short,     write = m_short};
	__property complexf loadCal                = {read = m_load,      write = m_load};
	__property complexf isolationCal           = {read = m_isolation, write = m_isolation};
	__property complexf throughCal             = {read = m_through,   write = m_through};
	__property complexf sParam[const int chan] = {read = getSParam,   write = setSParam};

	// error terms
	__property complexf e00                        = {read = m_e00,        write = m_e00};
	__property complexf e11                        = {read = m_e11,        write = m_e11};
	__property complexf delta_e                    = {read = m_delta_e,    write = m_delta_e};
	__property complexf e30                        = {read = m_e30,        write = m_e30};
	__property complexf e10e32                     = {read = m_e10e32,     write = m_e10e32};
	__property complexf errorTerm[const int index] = {read = getErrorTerm, write = setErrorTerm};
};

struct t_calibration
{
	String filename;
	String name;
	std::vector <t_calibration_point> point;
};

#endif

