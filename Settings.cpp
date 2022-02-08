
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <Vcl.Forms.hpp>
//#include <System.SysUtils.hpp>

//#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "Settings.h"
#include "Unit1.h"
#include "common.h"

//#pragma package(smart_init)

const t_freq_band_c default_freq_band[] =
{
	{"100k to 900M",      100000,  900000000, false},
	{"100k to 1.5G",      100000, 1500000000, false},
	{"2200m",             135700,     137800, true},
	{"630m",              472000,     479000, true},
	{"160m",             1800000,    2000000, true},
	{"80m",              3500000,    3800000, true},
	{"60m",              5250000,    5450000, true},
	{"40m",              7000000,    7200000, true},
	{"30m",             10100000,   10150000, true},
	{"20m",             14000000,   14350000, true},
	{"17m",             18068000,   18168000, true},
	{"15m",             21000000,   21450000, true},
	{"12m",             24890000,   24990000, true},
	{"CB CEPT",         26965000,   27405000, false},
	{"CB UK",           27601250,   27991250, false},
	{"10m",             28000000,   29700000, true},
	{"6m",              50000000,   52000000, true},
	{"4m",              70000000,   70500000, true},
	{"FM",              88000000,  108000000, false},
	{"Air",            118000000,  136000000, false},
	{"2m",             144000000,  146000000, true},
	{"70cm",           430000000,  440000000, true},
	{"PMR 446",        446006250,  446193750, false},
	{"23cm",          1240000000, 1300000000, true}
};

typedef struct
{
	const char *name;
	bool       dark_mode;
} t_style_names;

const t_style_names styles[] =
{
	{"Amakrits",            true},
	{"Amethyst Kamri",      false},
	{"Aqua Graphite",       true},
	{"Aqua Light Slate",    false},
	{"Auric",               true},
	{"Carbon",              true},
	{"Charcoal Dark Slate", true},
	{"Cobalt XEMedia",      true},
	{"Cyan Dusk",           false},
	{"Cyan Night",          false},
	{"Emerald Light Slate", false},
	{"Glossy",              true},
	{"Glow",                true},
	{"Golden Graphite",     true},
	{"Iceberg Classico",    false},
	{"Lavender Classico",   false},
	{"Light",               false},
	{"Luna",                false},
	{"Metropolis UI Black", true},
	{"Metropolis UI Blue",  false},
	{"Metropolis UI Dark",  true},
	{"Metropolis UI Green", false},
	{"Obsidian",            true},
	{"Onyx Blue",           true},
	{"Ruby Graphite",       true},
	{"Sapphire Kamri",      false},
	{"Silver",              false},
	{"Sky",                 false},
	{"Slate Classico",      false},
	{"Smokey Quartz Kamri", false},
	{"Tablet Light",        false},
	{"TabletDark",          true},
	{"Turquoise Gray",      false},
	{"Windows",             false},
	{"Windows10",           false},
	{"Windows10 Blue",      false},
	{"Windows10 Green",     false},
	{"Windows10 Dark",      true},
	{"Windows10 Purple",    false},
	{"windows10 SlateGray", true}
};

CSettings settings;

// ********************************************************

CSettings::CSettings()
{
	{
//		char username[64];
//		DWORD size = sizeof(username);
//		if (::GetUserNameA(username, &size) != FALSE && size > 1)
//			m_settings_filename = ChangeFileExt(Application->ExeName, "_" + String(username) + ".ini");
//		else
			m_settings_filename = ChangeFileExt(Application->ExeName, ".ini");
	}

	//m_field_sep   = ',';
	//m_decimal_point = '.';

	m_graph_font = new TFont();
	if (m_graph_font)
	{
		m_graph_font->Name    = "Consolas";							// string
		m_graph_font->Charset = ANSI_CHARSET;						// int
		m_graph_font->Color   = clBlack;       					// int
		m_graph_font->Size    = 8;             					// int
		m_graph_font->Style   = TFontStyles(); 					// byte array
//		m_graph_font->Pitch   = TFontPitch::fpFixed;				//
//		m_graph_font->Quality = TFontQuality::fqAntialiased;	//
	}

	setRecordFolder(ExtractFilePath(Application->ExeName) + "Recordings\\");

	setCalibrationFolder(ExtractFilePath(Application->ExeName) + "Calibrations\\");

	memset(&m_main_form,            0, sizeof(m_main_form));
	memset(&m_comms_form,           0, sizeof(m_comms_form));
	memset(&m_vna_usart_comms_form, 0, sizeof(m_vna_usart_comms_form));
	memset(&m_battery_form,         0, sizeof(m_battery_form));
	memset(&m_settings_form,        0, sizeof(m_settings_form));
	memset(&m_firmware_form,        0, sizeof(m_firmware_form));
	memset(&m_firmware_v2_form,     0, sizeof(m_firmware_v2_form));
	memset(&m_calibration_form,     0, sizeof(m_calibration_form));
	memset(&m_screen_capture_form,  0, sizeof(m_screen_capture_form));

	m_serial_port.name     = "";
	m_serial_port.baudrate = 115200;

	m_tcpip.address = "";
	m_tcpip.port    = 0;

	m_line_width = 1;
	m_line_alpha = 255;

	m_border_width = 3;

	m_spline_enable         = false;
	m_show_points           = false;
	m_clip_traces           = false;
	m_snap_to_nearest_point = true;
	m_auto_scale_peak_hold  = false;

	m_smith_both_scales = false;

	m_show_freq_bands = true;

	m_marker_fill           = false;

	m_show_markers_on_graph = true;
	m_show_marker_text      = false;

	m_lc_matching_enable = false;

	m_info_panel = false;

	m_start_Hz = 50000;
	m_stop_Hz  = 900000000;
	m_cw_Hz    = 145000000;

	m_point_bandwidth = 4000;

   m_num_points = 101;

	m_time_average_level = 0;

	m_velocity_factor.value = 0.66f;

	m_average_calibration   = 3;
	m_smoothing_calibration = 0;
	m_median_calibration    = 0;
	m_calibration_selection = CAL_SELECT_NONE;

	m_tdr_window = 6;

	m_output_power = -1; // auto

	m_median_filter_level = 0;

	m_curve_smoothing_level = 0;

	m_vna_usart_command_ends = 0;

	m_graph_arrangement = GRAPH_ARRANGE_1;

	m_norm_enabled = false;

	m_memory[0].enabled = true;
	for (int i = 1; i < MAX_MEMORIES; i++)
	{
		m_memory[i].enabled = false;
		m_memory[i].name = "";
	}

	for (int i = 0; i < MAX_GRAPHS; i++)
		m_graph_type[i] = GRAPH_TYPE_LOGMAG_S11S21;

	m_s21_offset_dB = 0.0f;

	m_edelay_secs = 0.0;

	setDefaultGraphScales();

	m_freq_band_sweep_padding_percent = 0;

	defaultFreqBands();

	m_colour_scheme.resize(ARRAY_SIZE(styles));
	for (unsigned int i = 0; i < m_colour_scheme.size(); i++)
//		setDefaultColourScheme(i, styles[i].dark_mode);
		setDefaultColourScheme(i, true);

	// fetch the current style
	setColourStyle(TStyleManager::ActiveStyle->Name);

	load();
}

CSettings::~CSettings()
{
	if (m_graph_font)
		delete m_graph_font;
	m_graph_font = NULL;
}

void __fastcall CSettings::load()
{	// load all the settings from the settings file

	TVersion version;

	std::vector <String> lines;	// holds all the text lines from the settings file
	std::vector <String> params;	// used with parsing each text line as we go

	// load the entire settiongs file
	if (common.loadFile(m_settings_filename, lines) <= 0)
		return;

	m_markers_freq.resize(0);
	m_markers_time.resize(0);

	m_freq_band.resize(0);

	// now process each text line one at a time
	for (unsigned int i = 0; i < lines.size(); i++)
	{
		String line = lines[i].Trim();	// fetch a text line

		if (line.Length() < 2)  			// the text line seems invalid - ignore it
			continue;

		if (line[1] == '#' || line[1] == ';')
			continue;							// the line is a comment line - ignore it

		// parse the text line up
		params.resize(0);
		common.parseString(line, " ", params);
		if (params.size() < 2)
			continue;                  	// text line contains too few parameters

		{	// remove any surrounding double quotes found on single params
			int k = 0;
			while (k < (int)params.size())
			{
				String s = params[k++];
				if (s.Length() >= 2)
				{
					if (s[1] == '\"' && s[s.Length()] == '\"')
					{
						s = s.SubString(2, s.Length() - 2).Trim();
						k--;
						params[k++] = s;
						if (s.IsEmpty() && k <= 0)
							params.resize(0); // a problem with the setting name value
					}
				}
			}

			if (params.size() < 2)
				continue;
		}

		{	// re-combine any parsed up params that were located within a pair of quotes
			int k = 0;
			while (k < (int)params.size())
			{
				const int m = k;
				String s1 = params[k++];
				if (s1.IsEmpty())
					continue;
				if (s1[1] == '\"' && s1[s1.Length()] == '\"' && s1.Length() >= 2)
				{	// simply remove the double quotes
					s1 = s1.SubString(2, s1.Length() - 2).Trim();
					params[k - 1] = s1;
				}
				else
				if (s1[1] == '\"')
				{
					while (k < (int)params.size())
					{
						String s2 = params[k++];
						s1 += " " + s2;
						if (s2[s2.Length()] == '\"')
							break;
					}

					if (s1[1] == '\"' && s1[s1.Length()] == '\"')
					{
						s1 = s1.SubString(2, s1.Length() - 2).Trim();
						while (--k > m)
							params.erase(params.begin() + k);
						params[k++] = s1;
					}
				}
			}
		}

		if (params[0][1] == '#' || params[0][1] == ';')
			continue;							// the line is a comment line - ignore it

		// ***********************
		// process the params found in the text line

		if (params[0] == "version")
		{
			int i;
			TVersion ver;
			if (params.size() < 5)
				continue;
			if (!TryStrToInt(params[1], i) || i < 0)
				continue;
			ver.MajorVer = i;
			if (!TryStrToInt(params[2], i) || i < 0)
				continue;
			ver.MinorVer = i;
			if (!TryStrToInt(params[3], i) || i < 0)
				continue;
			ver.ReleaseVer = i;
			if (!TryStrToInt(params[4], i) || i < 0)
				continue;
			ver.BuildVer = i;
			version = ver;
         continue;
		}

/*
			if (params[0] == "field_sep")
			{
				if (params.size() >= 2)
					m_field_sep = params[1][1];
				continue;
			}

			if (params[0] == "decimal_dot")
			{
				if (params.size() >= 2)
					m_decimal_point = params[1][1];
				continue;
			}
*/

		if (params[0] == "main_form")
		{
			t_window_pos window_pos;
			if (params.size() < 5)
				continue;
			for (unsigned int k = 1; k < params.size(); k++)
				params[k] = common.localiseDecimalPoint(params[k]);
			if (!TryStrToInt(params[1], window_pos.left))
				continue;
			if (!TryStrToInt(params[2], window_pos.top))
				continue;
			if (!TryStrToInt(params[3], window_pos.width))
				continue;
			if (!TryStrToInt(params[4], window_pos.height))
				continue;
			window_pos.showing = (params.size() >= 6) ? (params[5].LowerCase() == "true") ? true : false : false;
			m_main_form = window_pos;
			continue;
		}

		if (params[0] == "comms_form")
		{
			t_window_pos window_pos;
			if (params.size() < 5)
				continue;
			for (unsigned int k = 1; k < params.size(); k++)
				params[k] = common.localiseDecimalPoint(params[k]);
			if (!TryStrToInt(params[1], window_pos.left))
				continue;
			if (!TryStrToInt(params[2], window_pos.top))
				continue;
			if (!TryStrToInt(params[3], window_pos.width))
				continue;
			if (!TryStrToInt(params[4], window_pos.height))
				continue;
			window_pos.showing = (params.size() >= 6) ? (params[5].LowerCase() == "true") ? true : false : false;
			m_comms_form = window_pos;
			continue;
		}

		if (params[0] == "vna_usart_comms_form")
		{
			t_window_pos window_pos;
			if (params.size() < 5)
				continue;
			for (unsigned int k = 1; k < params.size(); k++)
				params[k] = common.localiseDecimalPoint(params[k]);
			if (!TryStrToInt(params[1], window_pos.left))
				continue;
			if (!TryStrToInt(params[2], window_pos.top))
				continue;
			if (!TryStrToInt(params[3], window_pos.width))
				continue;
			if (!TryStrToInt(params[4], window_pos.height))
				continue;
			window_pos.showing = (params.size() >= 6) ? (params[5].LowerCase() == "true") ? true : false : false;
			m_vna_usart_comms_form = window_pos;
			continue;
		}

		if (params[0] == "battery_form")
		{
			t_window_pos window_pos;
			for (unsigned int k = 1; k < params.size(); k++)
				params[k] = common.localiseDecimalPoint(params[k]);
			if (params.size() < 5)
				continue;
			if (!TryStrToInt(params[1], window_pos.left))
				continue;
			if (!TryStrToInt(params[2], window_pos.top))
				continue;
			if (!TryStrToInt(params[3], window_pos.width))
				continue;
			if (!TryStrToInt(params[4], window_pos.height))
				continue;
			window_pos.showing = (params.size() >= 6) ? (params[5].LowerCase() == "true") ? true : false : false;
			m_battery_form = window_pos;
			continue;
		}

		if (params[0] == "settings_form")
		{
			t_window_pos window_pos;
			if (params.size() < 5)
				continue;
			for (unsigned int k = 1; k < params.size(); k++)
				params[k] = common.localiseDecimalPoint(params[k]);
			if (!TryStrToInt(params[1], window_pos.left))
				continue;
			if (!TryStrToInt(params[2], window_pos.top))
				continue;
			if (!TryStrToInt(params[3], window_pos.width))
				continue;
			if (!TryStrToInt(params[4], window_pos.height))
				continue;
			window_pos.showing = (params.size() >= 6) ? (params[5].LowerCase() == "true") ? true : false : false;
			m_settings_form = window_pos;
			continue;
		}

		if (params[0] == "firmware_form")
		{
			t_window_pos window_pos;
			if (params.size() < 5)
				continue;
			for (unsigned int k = 1; k < params.size(); k++)
				params[k] = common.localiseDecimalPoint(params[k]);
			if (!TryStrToInt(params[1], window_pos.left))
				continue;
			if (!TryStrToInt(params[2], window_pos.top))
				continue;
			if (!TryStrToInt(params[3], window_pos.width))
				continue;
			if (!TryStrToInt(params[4], window_pos.height))
				continue;
			window_pos.showing = (params.size() >= 6) ? (params[5].LowerCase() == "true") ? true : false : false;
			m_firmware_form = window_pos;
			continue;
		}

		if (params[0] == "firmware_v2_form")
		{
			t_window_pos window_pos;
			if (params.size() < 5)
				continue;
			for (unsigned int k = 1; k < params.size(); k++)
				params[k] = common.localiseDecimalPoint(params[k]);
			if (!TryStrToInt(params[1], window_pos.left))
				continue;
			if (!TryStrToInt(params[2], window_pos.top))
				continue;
			if (!TryStrToInt(params[3], window_pos.width))
				continue;
			if (!TryStrToInt(params[4], window_pos.height))
				continue;
			window_pos.showing = (params.size() >= 6) ? (params[5].LowerCase() == "true") ? true : false : false;
			m_firmware_v2_form = window_pos;
			continue;
		}

		if (params[0] == "calibration_form")
		{
			t_window_pos window_pos;
			if (params.size() < 5)
				continue;
			for (unsigned int k = 1; k < params.size(); k++)
				params[k] = common.localiseDecimalPoint(params[k]);
			if (!TryStrToInt(params[1], window_pos.left))
				continue;
			if (!TryStrToInt(params[2], window_pos.top))
				continue;
			if (!TryStrToInt(params[3], window_pos.width))
				continue;
			if (!TryStrToInt(params[4], window_pos.height))
				continue;
			window_pos.showing = (params.size() >= 6) ? (params[5].LowerCase() == "true") ? true : false : false;
			m_calibration_form = window_pos;
			continue;
		}

		if (params[0] == "screen_capture_form")
		{
			t_window_pos window_pos;
			if (params.size() < 5)
				continue;
			for (unsigned int k = 1; k < params.size(); k++)
				params[k] = common.localiseDecimalPoint(params[k]);
			if (!TryStrToInt(params[1], window_pos.left))
				continue;
			if (!TryStrToInt(params[2], window_pos.top))
				continue;
			if (!TryStrToInt(params[3], window_pos.width))
				continue;
			if (!TryStrToInt(params[4], window_pos.height))
				continue;
			window_pos.showing = (params.size() >= 6) ? (params[5].LowerCase() == "true") ? true : false : false;
			m_screen_capture_form = window_pos;
			continue;
		}

		if (params[0] == "record_folder")
		{
			if (params.size() >= 2)
				setRecordFolder(params[1]);
			continue;
		}

		if (params[0] == "calibration_folder")
		{
			if (params.size() >= 2)
				setCalibrationFolder(params[1]);
			continue;
		}

		if (params[0] == "s21_offset_db")
		{
			if (params.size() >= 2)
			{
				float f = 0;
				if (TryStrToFloat(common.localiseDecimalPoint(params[1]), f))
					m_s21_offset_dB = f;
			}
			continue;
		}

		if (params[0] == "edelay_secs")
		{
			if (params.size() >= 2)
			{
				double d = 0;
				if (TryStrToFloat(common.localiseDecimalPoint(params[1]), d))
					m_edelay_secs = d;
			}
			continue;
		}

		if (params[0] == "spline_enable")
		{
			if (params.size() >= 2)
				m_spline_enable = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "show_points_enable")
		{
			if (params.size() >= 2)
				m_show_points = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "clip_traces_enable")
		{
			if (params.size() >= 2)
				m_clip_traces = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "snap_to_nearest_point_enable")
		{
//			if (params.size() >= 2)
//				m_snap_to_nearest_point = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "auto_scale_peak_hold_enable")
		{
			if (params.size() >= 2)
				m_auto_scale_peak_hold = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "smith_both_scales_enable")
		{
			if (params.size() >= 2)
				m_smith_both_scales = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "line_width")
		{
			if (params.size() >= 2)
			{
				int d = 1;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 1)
						d = 1;
					m_line_width = d;
				}
			}
			continue;
		}

		if (params[0] == "line_alpha")
		{
			if (params.size() >= 2)
			{
				int d = 180;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d <   1) d = 1;
					else
					if (d > 255) d = 255;
					m_line_alpha = d;
				}
			}
			continue;
		}

		if (params[0] == "border_width")
		{
			if (params.size() >= 2)
			{
				int d = 1;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d <  0) d = 0;
					else
					if (d > 10) d = 10;
					m_border_width = d;
				}
			}
			continue;
		}

		if (params[0] == "show_freq_bands_enable")
		{
			if (params.size() >= 2)
				m_show_freq_bands = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "lc_matching_enable")
		{
			if (params.size() >= 2)
				m_lc_matching_enable = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "marker_fill_enable")
		{
			if (params.size() >= 2)
				m_marker_fill = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "show_markers_on_graph_enable")
		{
			if (params.size() >= 2)
				m_show_markers_on_graph = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "show_marker_text")
		{
			if (params.size() >= 2)
				m_show_marker_text = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "gui_style_name")
		{
			if (params.size() >= 2)
				m_gui_style_name = params[1];
			continue;
		}

		if (params[0] == "info_panel_enable")
		{
			if (params.size() >= 2)
				m_info_panel = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "serial_port")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					m_serial_port.baudrate = d;
					if (params.size() >= 3)
						m_serial_port.name  = params[2];
				}
			}
			continue;
		}

		if (params[0] == "tcpip")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d >= 0 && d <= 65535)
						m_tcpip.port = d;
					if (params.size() >= 3)
						m_tcpip.address  = params[2];
				}
			}
			continue;
		}

		if (params[0] == "start_freq")
		{
			if (params.size() >= 2)
			{
				int64_t d;
				if (TryStrToInt64(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < MIN_FREQ) d = MIN_FREQ;
					else
					if (d > MAX_FREQ) d = MAX_FREQ;
					m_start_Hz = d;
				}
			}
			continue;
		}

		if (params[0] == "stop_freq")
		{
			if (params.size() >= 2)
			{
				int64_t d;
				if (TryStrToInt64(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < MIN_FREQ) d = MIN_FREQ;
					else
					if (d > MAX_FREQ) d = MAX_FREQ;
					m_stop_Hz = d;
				}
			}
			continue;
		}

		if (params[0] == "cw_freq")
		{
			if (params.size() >= 2)
			{
				int64_t d;
				if (TryStrToInt64(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < MIN_FREQ) d = MIN_FREQ;
					else
					if (d > MAX_FREQ) d = MAX_FREQ;
					m_cw_Hz = d;
				}
			}
			continue;
		}

		if (params[0] == "point_bandwidth")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 1)
						d = 1;
					m_point_bandwidth = d;
				}
			}
			continue;
		}

		if (params[0] == "num_of_points")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 11)
						d = 11;
					m_num_points = d;
				}
			}
			continue;
		}

		if (params[0] == "time_average_level")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 0)
						d = 0;
					m_time_average_level = d;
				}
			}
			continue;
		}

		if (params[0] == "velocity_factor")
		{
			if (params.size() >= 2)
			{
				float f;
				if (TryStrToFloat(common.localiseDecimalPoint(params[1]), f))
				{
					if (f < 0.0f) f = 0.0f;
					else
					if (f > 1.0f) f = 1.0f;
					m_velocity_factor.value = f;

					if (params.size() >= 3)
						m_velocity_factor.name = params[2];
				}
			}
			continue;
		}

		if (params[0] == "sweep_name")
		{
			if (params.size() >= 2)
				m_sweep_name = params[1];
			continue;
		}

		if (params[0] == "calibration_file")
		{
			if (params.size() >= 2)
				m_calibration_file = params[1];
			continue;
		}

		if (params[0] == "average_calibration")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 0)
						d = 0;
					m_average_calibration = d;
				}
			}
			continue;
		}

		if (params[0] == "smoothing_calibration")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 0)
						d = 0;
					m_smoothing_calibration = d;
				}
			}
			continue;
		}

		if (params[0] == "median_calibration")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 0)
						d = 0;
					m_median_calibration = d;
				}
			}
			continue;
		}

		if (params[0] == "calibration_selection")
		{
			if (params.size() >= 2)
			{
				int d = 0;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					m_calibration_selection = (t_calibration_selection)d;
				}
			}
			continue;
		}

		if (params[0] == "tdr_window")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 0)
						d = 0;
					m_tdr_window = d;
				}
			}
			continue;
		}

		if (params[0] == "output_power")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
					m_output_power = d;
			}
			continue;
		}

		if (params[0] == "median_filter_level")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 0)
						d = 0;
					//m_median_filter_level = d;
				}
			}
			continue;
		}

		if (params[0] == "curve_smoothing_level")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 0)
						d = 0;
					m_curve_smoothing_level = d;
				}
			}
			continue;
		}

		if (params[0] == "graph_arrangement")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 0)
						d = 0;
					m_graph_arrangement = (t_graph_arrange)d;
				}
			}
			continue;
		}

		if (params[0] == "vna_usart_command_ends")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 0)
						d = 0;
					m_vna_usart_command_ends = d;
				}
			}
			continue;
		}

		if (params[0] == "graph_font_name")
		{
			if (params.size() >= 2)
				if (m_graph_font)
					m_graph_font->Name = params[1];
			continue;
		}
		if (params[0] == "graph_font_charset")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d) && d >= 0)
					if (m_graph_font)
						m_graph_font->Charset = d;
			}
			continue;
		}
		if (params[0] == "graph_font_colour")
		{
			if (params.size() >= 2)
			{
				uint32_t u = 0;
				if (TryStrToUInt(common.localiseDecimalPoint(params[1]), u))
					if (m_graph_font)
						m_graph_font->Color = TColor(u);
			}
			continue;
		}
		if (params[0] == "graph_font_size")
		{
			if (params.size() >= 2)
			{
				int d = 0;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d) && d >= 7 && d <= 16)
					if (m_graph_font)
						m_graph_font->Size = d;
			}
			continue;
		}

		if (params[0] == "norm_enable")
		{
//			if (params.size() >= 2)
//				m_norm_enabled = (params[1].LowerCase() == "true") ? true : false;
			continue;
		}

		if (params[0] == "memory")
		{
			if (params.size() >= 3)
			{
				int d = -1;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d) && d >= 0 && d < MAX_MEMORIES)
				{
					m_memory[d].enabled = (params[2].LowerCase() == "true") ? true : false;
					if (params.size() >= 4)
						m_memory[d].name = params[3];
				}
			}
			continue;
		}

		if (params[0] == "marker_freq")
		{
			t_marker_freq marker;
			marker.Hz    = 0;
			marker.type  = MARKER_TYPE_NORMAL;
			marker.graph = -1;
			marker.trace = -1;

			if (params.size() < 2)
				continue;

			int64_t i64;
			if (!TryStrToInt64(common.localiseDecimalPoint(params[1]), i64))
				continue;
			if (i64 < MIN_FREQ || i64 > MAX_FREQ)
				continue;
			marker.Hz = i64;

			int i;

			if (params.size() >= 3)
				if (TryStrToInt(params[2], i))
					if (i >= MARKER_TYPE_NORMAL && i <= MARKER_TYPE_DELTA)
						marker.type = i;

			if (params.size() >= 4)
				if (TryStrToInt(params[3], i))
					marker.graph = i;

			if (params.size() >= 5)
				if (TryStrToInt(params[4], i))
					marker.trace = i;

			m_markers_freq.push_back(marker);
			continue;
		}

		if (params[0] == "marker_time")
		{
			t_marker_time marker;
			marker.secs  = 0;
			marker.type  = MARKER_TYPE_NORMAL;
			marker.graph = -1;
			marker.trace = -1;

			if (params.size() < 2)
				continue;

			double d;
			if (!TryStrToFloat(params[1], d))
				continue;
			if (d < 0)
				continue;
			marker.secs = d;

			int i;

			if (params.size() >= 3)
				if (TryStrToInt(params[2], i))
					if (i >= MARKER_TYPE_NORMAL && i <= MARKER_TYPE_DELTA)
						marker.type = i;

			if (params.size() >= 4)
				if (TryStrToInt(params[3], i))
					marker.graph = i;

			if (params.size() >= 5)
				if (TryStrToInt(params[4], i))
					marker.trace = i;

			m_markers_time.push_back(marker);
			continue;
		}

		if (params[0] == "graph_type")
		{
			if (params.size() >= 3)
			{
				int d = -1;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d) && d >= 0 && d < MAX_GRAPHS)
				{
					int t;
					if (TryStrToInt(common.localiseDecimalPoint(params[2]), t) && t >= 0 && t < GRAPH_TYPE_SIZE)
						m_graph_type[d] = t;
				}
			}
			continue;
		}

		if (params[0] == "graph_setting")
		{
			// graph_setting 0 false false n.nnnn -n.nnnnn n.nnnn

			int d;
			t_graph_setting gs;

			if (params.size() < 11)
				continue;

			if (!TryStrToInt(common.localiseDecimalPoint(params[1]), d) || d < 0 || d >= GRAPH_TYPE_SIZE)
				continue;

			if (params[2].LowerCase() == "true")
				gs.auto_max = true;
			else
			if (params[2].LowerCase() == "false")
				gs.auto_max = false;
			else
				continue;

			if (params[3].LowerCase() == "true")
				gs.auto_min = true;
			else
			if (params[3].LowerCase() == "false")
				gs.auto_min = false;
			else
				continue;

			if (!TryStrToFloat(common.localiseDecimalPoint(params[4]), gs.max))
				continue;

			if (!TryStrToFloat(common.localiseDecimalPoint(params[5]), gs.min))
				continue;

			if (!TryStrToFloat(common.localiseDecimalPoint(params[6]), gs.gamma))
				continue;

			if (params[7].LowerCase() == "true")
				gs.show_min_max = true;
			else
			if (params[7].LowerCase() == "false")
				gs.show_min_max = false;
			else
				continue;

			if (params[8].LowerCase() == "true")
				gs.show_max_marker = true;
			else
			if (params[8].LowerCase() == "false")
				gs.show_max_marker = false;
			else
				continue;

			if (params[9].LowerCase() == "true")
				gs.show_min_marker = true;
			else
			if (params[9].LowerCase() == "false")
				gs.show_min_marker = false;
			else
				continue;

			if (params[10].LowerCase() == "true")
				gs.show_markers = true;
			else
			if (params[10].LowerCase() == "false")
				gs.show_markers = false;
			else
				continue;

			if (d < GRAPH_TYPE_SIZE)
				m_graph_setting[d] = gs;

			continue;
		}

		if (params[0] == "freq_band_sweep_pad")
		{
			if (params.size() >= 2)
			{
				int d;
				if (TryStrToInt(common.localiseDecimalPoint(params[1]), d))
				{
					if (d < 0)
						d = 0;
					m_freq_band_sweep_padding_percent = d;
				}
			}
			continue;
		}

		if (params[0] == "freq_band")
		{
			//freq_band "100k to 900M" 100000 900000000 f

			int64_t d;
			t_freq_band fb;

			if (params.size() < 5)
				continue;

			fb.name = params[1];

			if (!TryStrToInt64(common.localiseDecimalPoint(params[2]), fb.low_Hz))
				continue;
			if (fb.low_Hz  < MIN_FREQ || fb.low_Hz  > MAX_FREQ)
				continue;

			if (!TryStrToInt64(common.localiseDecimalPoint(params[3]), fb.high_Hz))
				continue;
			if (fb.high_Hz < MIN_FREQ || fb.high_Hz > MAX_FREQ)
				continue;

			if (fb.low_Hz > fb.high_Hz)
			{	// swap
				const int64_t tmp = fb.low_Hz;
				fb.low_Hz = fb.high_Hz;
				fb.high_Hz = tmp;
			}

			if (params[4].LowerCase() == "true")
				fb.enabled = true;
			else
			if (params[4].LowerCase() == "false")
				fb.enabled = false;
			else
				continue;

			m_freq_band.push_back(fb);

			continue;
		}

		if (params[0] == "colour_scheme")
		{
			// colour_scheme 0 0x001c1c1c 0x00303030 0x00c8c8c8 0x00646464 0x00646464 0x00646464 0x005050ff 0x00ffffff 0x008080ff
			// colour_scheme 1 0x00ffffff 0x00c8c8c8 0x00808080 0x00646464 0x00646464 0x00646464 0x005050ff 0x00000000 0x00000080

			int p = 1;
			int d;

			if (params.size() < 3)
				continue;

			if (!TryStrToInt(common.localiseDecimalPoint(params[p++]), d) || d < 0 || d >= (int)m_colour_scheme.size())
				continue;

			t_colour_scheme cs = m_colour_scheme[d];

			while (p < (int)params.size())
			{
				String s = params[p++];
				uint32_t u;
				if (!TryStrToUInt(s, u))
					continue;
				const int index = p - 3;
				switch (index)
				{
					case  0: cs.background      = TColor(u); break;
					case  1: cs.border          = TColor(u); break;
					case  2: cs.grid            = TColor(u); break;
					case  3: cs.mouse_line      = TColor(u); break;
					case  4: cs.marker_line     = TColor(u); break;
					case  5: cs.marker          = TColor(u); break;
					case  6: cs.marker_selected = TColor(u); break;
					case  7: cs.font            = TColor(u); break;
					case  8: cs.vswr2_line      = TColor(u); break;
					case  9: cs.mouse_marker    = TColor(u); break;
					case 10: cs.point           = TColor(u); break;
					default: break;
				}
			}

			m_colour_scheme[d] = cs;
		}

		if (params[0] == "colour_scheme_line")
		{
			// colour_scheme_line 0 0x0064ff64 0x00ff6464 0x006464ff 0x00ee6400 0x00640096 0x00649600 0x00102072 0x00107220
			// colour_scheme_line 1 0x00008000 0x00800000 0x00000080 0x00ee6400 0x00640096 0x00649600 0x00102072 0x00107220

			int p = 1;
			int d;

			if (params.size() < 3)
				continue;

			if (!TryStrToInt(params[p++], d) || d < 0 || d >= (int)m_colour_scheme.size())
				continue;

			t_colour_scheme cs = m_colour_scheme[d];

			//const int num_traces = MAX_CHANNELS;
			const int num_traces = MAX_CAL_TRACES;

			while (p < (int)params.size())
			{
				String s = params[p++];
				uint32_t u;
				if (!TryStrToUInt(s, u))
					continue;
				const int index = p - 3;
				const int mem_index  = index / num_traces;
				const int chan_index = index % num_traces;
				if (mem_index < MAX_MEMORIES && chan_index < num_traces)
					cs.line[mem_index][chan_index] = TColor(u);
			}

			m_colour_scheme[d] = cs;
		}

		// ***********************
	}

	for (int graph_type = 0; graph_type < GRAPH_TYPE_SIZE; graph_type++)
	{
		t_graph_setting *gs = &settings.m_graph_setting[graph_type];
		clipGraphMinMax(graph_type, gs->max, gs->min);
		if (gs->gamma < GAMMA_MIN) gs->gamma = GAMMA_MIN;
		else
		if (gs->gamma > GAMMA_MAX) gs->gamma = GAMMA_MAX;
	}

	sortFreqBand();

//	setColourStyle(m_gui_style_name);

	{	// do something version related
		TVersion current_ver;
		common.GetBuildInfo(Application->ExeName, &current_ver);
		if (version.MajorVer <= 1 && version.MinorVer <= 1 && version.ReleaseVer <= 205)
		{
			if (current_ver.MajorVer >= 1 && current_ver.MinorVer >= 1 && current_ver.ReleaseVer >= 206)
			{	// gone from v1.1.205 or earlier to v1.1.206 or later
				setDefaultGraphScales();
			}
		}
	}
}

void __fastcall CSettings::save()
{	// save all the settings into the settings file

	String s;
	TVersion version;
	std::vector <String> buffer;

	buffer.push_back("");

	s = "saved " + FormatDateTime("yyyy mm dd hh nn ss", Now());
	buffer.push_back(s);

	common.GetBuildInfo(Application->ExeName, &version);
	s.printf(L"version %u %u %u %u", version.MajorVer, version.MinorVer, version.ReleaseVer, version.BuildVer);
	buffer.push_back(s);

	buffer.push_back("");
/*
	s.printf(L"field_sep %c", m_field_sep);
	buffer.push_back(s);

	s.printf(L"decimal_dot %c", m_decimal_point);
	buffer.push_back(s);

	buffer.push_back("");
*/
	s.printf(L"main_form %d %d %d %d",
			m_main_form.left,
			m_main_form.top,
			m_main_form.width,
			m_main_form.height);
	s += m_main_form.showing ? " true" : " false";
	buffer.push_back(s);

	s.printf(L"comms_form %d %d %d %d",
			m_comms_form.left,
			m_comms_form.top,
			m_comms_form.width,
			m_comms_form.height);
	s += m_comms_form.showing ? " true" : " false";
	buffer.push_back(s);

	s.printf(L"vna_usart_comms_form %d %d %d %d",
			m_vna_usart_comms_form.left,
			m_vna_usart_comms_form.top,
			m_vna_usart_comms_form.width,
			m_vna_usart_comms_form.height);
	s += m_vna_usart_comms_form.showing ? " true" : " false";
	buffer.push_back(s);

	s.printf(L"battery_form %d %d %d %d",
			m_battery_form.left,
			m_battery_form.top,
			m_battery_form.width,
			m_battery_form.height);
	s += m_battery_form.showing ? " true" : " false";
	buffer.push_back(s);

	s.printf(L"settings_form %d %d %d %d",
			m_settings_form.left,
			m_settings_form.top,
			m_settings_form.width,
			m_settings_form.height);
	s += m_settings_form.showing ? " true" : " false";
	buffer.push_back(s);

	s.printf(L"firmware_form %d %d %d %d",
			m_firmware_form.left,
			m_firmware_form.top,
			m_firmware_form.width,
			m_firmware_form.height);
	s += m_firmware_form.showing ? " true" : " false";
	buffer.push_back(s);

	s.printf(L"firmware_v2_form %d %d %d %d",
			m_firmware_v2_form.left,
			m_firmware_v2_form.top,
			m_firmware_v2_form.width,
			m_firmware_v2_form.height);
	s += m_firmware_v2_form.showing ? " true" : " false";
	buffer.push_back(s);

	s.printf(L"calibration_form %d %d %d %d",
			m_calibration_form.left,
			m_calibration_form.top,
			m_calibration_form.width,
			m_calibration_form.height);
	s += m_calibration_form.showing ? " true" : " false";
	buffer.push_back(s);

	s.printf(L"screen_capture_form %d %d %d %d",
			m_screen_capture_form.left,
			m_screen_capture_form.top,
			m_screen_capture_form.width,
			m_screen_capture_form.height);
	s += m_screen_capture_form.showing ? " true" : " false";
	buffer.push_back(s);

	buffer.push_back("");

	s = String("record_folder \"") + m_record_folder + "\"";
	buffer.push_back(s);

	s = String("calibration_folder \"") + m_calibration_folder + "\"";
	buffer.push_back(s);

	buffer.push_back("");

	s.printf(L"s21_offset_db %f", m_s21_offset_dB);
	buffer.push_back(s);

	s.printf(L"edelay_secs %e", m_edelay_secs);
	buffer.push_back(s);

	s.printf(L"spline_enable %s", String(m_spline_enable ? "true" : "false").c_str());
	buffer.push_back(s);

	s.printf(L"show_points_enable %s", String(m_show_points ? "true" : "false").c_str());
	buffer.push_back(s);

	s.printf(L"clip_traces_enable %s", String(m_clip_traces ? "true" : "false").c_str());
	buffer.push_back(s);

	s.printf(L"snap_to_nearest_point_enable %s", String(m_snap_to_nearest_point ? "true" : "false").c_str());
	buffer.push_back(s);

	s.printf(L"auto_scale_peak_hold_enable %s", String(m_auto_scale_peak_hold ? "true" : "false").c_str());
	buffer.push_back(s);

	s.printf(L"smith_both_scales_enable %s", String(m_smith_both_scales ? "true" : "false").c_str());
	buffer.push_back(s);

	s.printf(L"line_width %d", m_line_width);
	buffer.push_back(s);

	s.printf(L"line_alpha %d", m_line_alpha);
	buffer.push_back(s);

	s.printf(L"border_width %d", m_border_width);
	buffer.push_back(s);

	s.printf(L"show_freq_bands_enable %s", String(m_show_freq_bands ? "true" : "false").c_str());
	buffer.push_back(s);

	s.printf(L"lc_matching_enable %s", String(m_lc_matching_enable ? "true" : "false").c_str());
	buffer.push_back(s);

	s.printf(L"marker_fill_enable %s", String(m_marker_fill ? "true" : "false").c_str());
	buffer.push_back(s);

	s.printf(L"show_markers_on_graph_enable %s", String(m_show_markers_on_graph ? "true" : "false").c_str());
	buffer.push_back(s);

	s.printf(L"show_marker_text %s", String(m_show_marker_text ? "true" : "false").c_str());
	buffer.push_back(s);

	s = String("gui_style_name \"") + m_gui_style_name + "\"";
	buffer.push_back(s);

	s.printf(L"info_panel_enable %s", String(m_info_panel ? "true" : "false").c_str());
	buffer.push_back(s);

	s.printf(L"serial_port %d", m_serial_port.baudrate);
	s += " \"" + m_serial_port.name + "\"";
	buffer.push_back(s);

	s.printf(L"tcpip %d", m_tcpip.port);
	s += " \"" + m_tcpip.address + "\"";
	buffer.push_back(s);

	s.printf(L"time_average_level %d", m_time_average_level);
	buffer.push_back(s);

	s.printf(L"velocity_factor %f", m_velocity_factor.value);
	s += " \"" + m_velocity_factor.name + "\"";
	buffer.push_back(s);

	s = String("sweep_name \"") + m_sweep_name + "\"";
	buffer.push_back(s);

	buffer.push_back("");

	s = String("calibration_file \"") + m_calibration_file + "\"";
	buffer.push_back(s);

	s.printf(L"average_calibration %d", m_average_calibration);
	buffer.push_back(s);

	s.printf(L"smoothing_calibration %d", m_smoothing_calibration);
	buffer.push_back(s);

	s.printf(L"median_calibration %d", m_median_calibration);
	buffer.push_back(s);

	s.printf(L"calibration_selection %d", (int)m_calibration_selection);
	buffer.push_back(s);

	buffer.push_back("");

	s.printf(L"tdr_window %d", m_tdr_window);
	buffer.push_back(s);

	s.printf(L"output_power %d", m_output_power);
	buffer.push_back(s);

	s.printf(L"median_filter_level %d", m_median_filter_level);
	buffer.push_back(s);

	s.printf(L"curve_smoothing_level %d", m_curve_smoothing_level);
	buffer.push_back(s);

	s.printf(L"graph_arrangement %d", m_graph_arrangement);
	buffer.push_back(s);

	s.printf(L"vna_usart_command_ends %d", m_vna_usart_command_ends);
	buffer.push_back(s);

	buffer.push_back("");

	if (m_graph_font)
	{
		s = "graph_font_name \"" + m_graph_font->Name + "\"";
		buffer.push_back(s);
		s.printf(L"graph_font_charset %d", m_graph_font->Charset);
		buffer.push_back(s);
		s.printf(L"graph_font_colour 0x%08x", Graphics::ColorToRGB(m_graph_font->Color));
		buffer.push_back(s);
		s.printf(L"graph_font_size %d", m_graph_font->Size);
		buffer.push_back(s);

		buffer.push_back("");
	}

	s.printf(L"start_freq %lld", m_start_Hz);
	buffer.push_back(s);

	s.printf(L"stop_freq %lld", m_stop_Hz);
	buffer.push_back(s);

	s.printf(L"cw_freq %lld", m_cw_Hz);
	buffer.push_back(s);

	s.printf(L"point_bandwidth %d", m_point_bandwidth);
	buffer.push_back(s);

	s.printf(L"num_of_points %d", m_num_points);
	buffer.push_back(s);

	buffer.push_back("");

	if (!m_markers_freq.empty())
	{
		for (unsigned int i = 0; i < m_markers_freq.size(); i++)
		{
			const t_marker_freq marker = m_markers_freq[i];
			if (marker.Hz > 0)
			{
				s.printf(L"marker_freq %lld", marker.Hz);
				s += " " + IntToStr(marker.type);
				s += " " + IntToStr(marker.graph);
				s += " " + IntToStr(marker.trace);
				buffer.push_back(s);
			}
		}

		buffer.push_back("");
	}

	if (!m_markers_time.empty())
	{
		for (unsigned int i = 0; i < m_markers_time.size(); i++)
		{
			const t_marker_time marker = m_markers_time[i];
			if (marker.secs > 0)
			{
				s.printf(L"marker_secs %#.9g", marker.secs);
				s += " " + IntToStr(marker.type);
				s += " " + IntToStr(marker.graph);
				s += " " + IntToStr(marker.trace);
				buffer.push_back(s);
			}
		}

		buffer.push_back("");
	}

	s.printf(L"norm_enable %s", String(m_norm_enabled ? "true" : "false").c_str());
	buffer.push_back(s);

	for (unsigned int i = 0; i < MAX_MEMORIES; i++)
	{
		s.printf(L"memory %u %s \"%s\"", i, String(m_memory[i].enabled ? "true" : "false").c_str(), m_memory[i].name.c_str());
		buffer.push_back(s);
	}

	buffer.push_back("");

	for (int i = 0; i < MAX_GRAPHS; i++)
	{
		s.printf(L"graph_type %u %d ", i, m_graph_type[i]);
		buffer.push_back(s);
	}

	buffer.push_back("");

	for (int i = 0; i < GRAPH_TYPE_SIZE; i++)
	{
		const t_graph_setting gs = m_graph_setting[i];
		s.printf(
			L"graph_setting %d %s %s %g %g %g %s %s %s %s",
			i,
			String(gs.auto_max ? "true" : "false").c_str(),
			String(gs.auto_min ? "true" : "false").c_str(),
			gs.max,
			gs.min,
			gs.gamma,
			String(gs.show_min_max ? "true" : "false").c_str(),
			String(gs.show_max_marker ? "true" : "false").c_str(),
			String(gs.show_min_marker ? "true" : "false").c_str(),
			String(gs.show_markers ? "true" : "false").c_str()
		);
		buffer.push_back(s);
	}

	buffer.push_back("");

	s.printf(L"freq_band_sweep_pad %d", m_freq_band_sweep_padding_percent);
	buffer.push_back(s);

	if (!m_freq_band.empty())
	{
		for (unsigned int i = 0; i < m_freq_band.size(); i++)
		{
			AnsiString s2;
			const t_freq_band fb = m_freq_band[i];
			s = "freq_band \"" + fb.name + "\"";
			s2.printf(" %lld", fb.low_Hz);
			s += s2;
			s2.printf(" %lld", fb.high_Hz);
			s += s2;
			s += (fb.enabled ? " true" : " false");
			buffer.push_back(s);
		}

		buffer.push_back("");
	}

	for (unsigned int i = 0; i < m_colour_scheme.size(); i++)
	{
		const t_colour_scheme cs = m_colour_scheme[i];
		s.printf(L"colour_scheme %u 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x",
			i,
			Graphics::ColorToRGB(cs.background),
			Graphics::ColorToRGB(cs.border),
			Graphics::ColorToRGB(cs.grid),
			Graphics::ColorToRGB(cs.mouse_line),
			Graphics::ColorToRGB(cs.marker_line),
			Graphics::ColorToRGB(cs.marker),
			Graphics::ColorToRGB(cs.marker_selected),
			Graphics::ColorToRGB(cs.font),
			Graphics::ColorToRGB(cs.vswr2_line),
			Graphics::ColorToRGB(cs.mouse_marker),
			Graphics::ColorToRGB(cs.point));
		buffer.push_back(s);
	}

	buffer.push_back("");

	for (unsigned int i = 0; i < m_colour_scheme.size(); i++)
	{
		s.printf(L"colour_scheme_line %d", i);
		const t_colour_scheme cs = m_colour_scheme[i];
		for (unsigned m = 0; m < MAX_MEMORIES; m++)
		{
			//for (unsigned c = 0; c < MAX_CHANNELS; c++)
			for (unsigned c = 0; c < MAX_CAL_TRACES; c++)
			{
				String s2;
				s2.printf(L" 0x%08x", Graphics::ColorToRGB(cs.line[m][c]));
				s += s2;
			}
		}
		buffer.push_back(s);
	}

	common.saveFile(m_settings_filename, buffer);
}

void __fastcall CSettings::defaultFreqBands()
{
	m_freq_band.resize(0);
	for (unsigned int i = 0; i < ARRAYSIZE(default_freq_band); i++)
	{
		const t_freq_band_c fb_c = default_freq_band[i];
		t_freq_band fb;
		fb.name    = String(fb_c.name);
		fb.low_Hz  = fb_c.low_Hz;
		fb.high_Hz = fb_c.high_Hz;
		fb.enabled = fb_c.enabled;
		m_freq_band.push_back(fb);
	}
	sortFreqBand();
}

void __fastcall CSettings::setTCPIPAddress(String value)
{
	m_tcpip.address = value;
}

void __fastcall CSettings::setTCPIPPort(int value)
{
	if (value >= 0 && value <= 65535)
		m_tcpip.port = value;
}

void __fastcall CSettings::sortFreqBand()
{
	for (int i = 0; i < (int)m_freq_band.size() - 1; i++)
	{
		t_freq_band fb1 = m_freq_band[i];
		for (int k = i + 1; k < (int)m_freq_band.size(); k++)
		{
			const t_freq_band fb2 = m_freq_band[k];
			if (fb2.low_Hz < fb1.low_Hz)
			{	// swap
				m_freq_band[i] = fb2;
				m_freq_band[k] = fb1;
				fb1 = fb2;
			}
		}
	}
}

void __fastcall CSettings::setGraphType(const int index, const int type)
{
	if (index >= 0 && index < MAX_GRAPHS)
		if (type >= 0 && type < GRAPH_TYPE_SIZE)
			m_graph_type[index] = type;
}

int __fastcall CSettings::getGraphType(const int index)
{
	return (index >= 0 && index < MAX_GRAPHS) ? m_graph_type[index] : GRAPH_TYPE_LOGMAG_S11S21;
}

bool __fastcall CSettings::getMemoryEnabled()
{	// return true if any memory is enabled
	bool enabled = false;
	for (int i = 0; i < MAX_MEMORIES; i++)
	{
		if (m_memory[i].enabled)
		{
			enabled = true;
			break;
		}
	}
	return enabled;
}

bool __fastcall CSettings::getMemoryEnable(const int index)
{
	return (index >= 0 && index < MAX_MEMORIES) ? m_memory[index].enabled : false;
}

void __fastcall CSettings::setMemoryEnable(const int index, const bool enabled)
{
	if (index >= 0 && index < MAX_MEMORIES)
		m_memory[index].enabled = enabled;
}

String __fastcall CSettings::getMemoryName(const int index)
{
	return (index >= 0 && index < MAX_MEMORIES) ? m_memory[index].name : String("");
}

void __fastcall CSettings::setMemoryName(const int index, String name)
{
	if (index >= 0 && index < MAX_MEMORIES)
		m_memory[index].name = name.Trim();
}

int __fastcall CSettings::styleIndex(String name)
{
	int style_index = -1;

	name = name.Trim().LowerCase();
	if (name.IsEmpty())
		return -1;

	// compute a style index value for the current style

	for (unsigned int i = 0; i < ARRAY_SIZE(styles); i++)
	{
		String style_name = String(styles[i].name).Trim().LowerCase();
		if (name == style_name)
		{
			style_index = i;
			break;
		}
	}

	return style_index;
}

void __fastcall CSettings::setDefaultColourScheme(const int style_index, const bool dark_colours)
{
	if (style_index < 0 || style_index >= (int)m_colour_scheme.size() || style_index >= (int)ARRAY_SIZE(styles))
		return;

//	m_dark_mode = styles[style_index].dark_mode;
	m_dark_mode = dark_colours;

	if (m_dark_mode)
	{	// dark scheme
//		m_colour_scheme[style_index].background      = TColor(RGB(  0,   0,   0));
		m_colour_scheme[style_index].background      = TColor(RGB( 16,  20,  28));	// slight blue tint
//		m_colour_scheme[style_index].background      = TStyleManager::ActiveStyle->GetStyleColor(scGenericBackground);
//		m_colour_scheme[style_index].background      = TStyleManager::ActiveStyle->GetStyleColor(scWindow);

		m_colour_scheme[style_index].border          = TColor(RGB(  0,   0,   0));
//		m_colour_scheme[style_index].border          = TStyleManager::ActiveStyle->GetStyleColor(scGenericBackground);
//		m_colour_scheme[style_index].border          = TStyleManager::ActiveStyle->GetStyleColor(scWindow);

		m_colour_scheme[style_index].grid            = TColor(RGB(100, 100, 100));

		m_colour_scheme[style_index].mouse_line      = TColor(RGB(200, 200, 200));
		m_colour_scheme[style_index].marker_line     = TColor(RGB(128, 128, 128));

		m_colour_scheme[style_index].marker          = TColor(RGB(255, 255, 255));
		m_colour_scheme[style_index].marker_selected = TColor(RGB(255,   0,   0));

		m_colour_scheme[style_index].font            = TColor(RGB(255, 255, 255));

		m_colour_scheme[style_index].vswr2_line      = TColor(RGB(100,  68,  68));

		m_colour_scheme[style_index].mouse_marker    = TColor(RGB(255, 255, 255));

		m_colour_scheme[style_index].point           = TColor(RGB(200, 200, 200));

//		m_colour_scheme[style_index].line[0][0]      = TColor(RGB(255, 160,   0));	// s11
		m_colour_scheme[style_index].line[0][0]      = TColor(RGB(255,   0,   0));	// s11
		m_colour_scheme[style_index].line[0][1]      = TColor(RGB(255, 255, 255));	// s21
		m_colour_scheme[style_index].line[0][2]      = TColor(RGB(255,   0, 255));	// s11
		m_colour_scheme[style_index].line[0][3]      = TColor(RGB(  0, 255, 255));	// s22
		m_colour_scheme[style_index].line[0][4]      = TColor(RGB(255, 255,   0));

		m_colour_scheme[style_index].line[1][0]      = TColor(RGB(  0, 255,   0)); // s11
		m_colour_scheme[style_index].line[1][1]      = TColor(RGB(  0,   0, 255)); // s21
		m_colour_scheme[style_index].line[1][2]      = TColor(RGB(255,  255,  0)); // s22
		m_colour_scheme[style_index].line[1][3]      = TColor(RGB(255,   0,   0)); // s22
		m_colour_scheme[style_index].line[1][4]      = TColor(RGB(255, 255, 255));

		m_colour_scheme[style_index].line[2][0]      = TColor(RGB(255,   0, 255));	// s11
		m_colour_scheme[style_index].line[2][1]      = TColor(RGB(  0, 255, 255)); // s21
		m_colour_scheme[style_index].line[2][2]      = TColor(RGB(255, 255,   0)); // s12
		m_colour_scheme[style_index].line[2][3]      = TColor(RGB(255,   0, 128)); // s22
		m_colour_scheme[style_index].line[2][4]      = TColor(RGB(255, 255, 255));

		m_colour_scheme[style_index].line[3][0]      = TColor(RGB(255, 100,  64));
		m_colour_scheme[style_index].line[3][1]      = TColor(RGB(255,  16, 255));
		m_colour_scheme[style_index].line[3][2]      = TColor(RGB(100, 255, 255));
		m_colour_scheme[style_index].line[3][3]      = TColor(RGB(255,  16, 100));
		m_colour_scheme[style_index].line[3][4]      = TColor(RGB(255, 255, 255));

		m_colour_scheme[style_index].line[4][0]      = TColor(RGB(255,   0, 128));
		m_colour_scheme[style_index].line[4][1]      = TColor(RGB(  0, 160, 160));
		m_colour_scheme[style_index].line[4][2]      = TColor(RGB(160, 160,   0));
		m_colour_scheme[style_index].line[4][3]      = TColor(RGB(160,   0, 160));
		m_colour_scheme[style_index].line[4][4]      = TColor(RGB(255, 255, 255));
	}
	else
	{	// light scheme
		m_colour_scheme[style_index].background      = TColor(RGB(255, 255, 255));
//		m_colour_scheme[style_index].background      = TStyleManager::ActiveStyle->GetStyleColor(scGenericBackground);
//		m_colour_scheme[style_index].background      = TStyleManager::ActiveStyle->GetStyleColor(scWindow);

		m_colour_scheme[style_index].border          = TColor(RGB(200, 200, 200));
//		m_colour_scheme[style_index].border          = TStyleManager::ActiveStyle->GetStyleColor(scGenericBackground);
//		m_colour_scheme[style_index].border          = TStyleManager::ActiveStyle->GetStyleColor(scWindow);

		m_colour_scheme[style_index].grid            = TColor(RGB( 80,  80,  80));

		m_colour_scheme[style_index].marker_line     = TColor(RGB(100, 100, 100));
		m_colour_scheme[style_index].mouse_line      = TColor(RGB(100, 100, 100));

		m_colour_scheme[style_index].marker          = TColor(RGB(  0,   0,   0));
		m_colour_scheme[style_index].marker_selected = TColor(RGB(255,  80,  80));

		m_colour_scheme[style_index].font            = TColor(RGB(  0,   0,   0));

		m_colour_scheme[style_index].vswr2_line      = TColor(RGB(225, 128, 128));

		m_colour_scheme[style_index].mouse_marker    = TColor(RGB(160,  80,   0));

		m_colour_scheme[style_index].point           = TColor(RGB( 80,  80,  80));

		m_colour_scheme[style_index].line[0][0]      = TColor(RGB(  0, 140,   0));	// s11
		m_colour_scheme[style_index].line[0][1]      = TColor(RGB(  0,   0, 200));	// s21
		m_colour_scheme[style_index].line[0][2]      = TColor(RGB(  0, 128,   0));	// s12
		m_colour_scheme[style_index].line[0][3]      = TColor(RGB(128,   0, 128));	// s22
		m_colour_scheme[style_index].line[0][4]      = TColor(RGB(128, 128,   0));

		m_colour_scheme[style_index].line[1][0]      = TColor(RGB(  0, 128,   0));	// s11
		m_colour_scheme[style_index].line[1][1]      = TColor(RGB(240,   0, 200));	// s21
		m_colour_scheme[style_index].line[1][2]      = TColor(RGB(0,   240, 200));	// s12
		m_colour_scheme[style_index].line[1][3]      = TColor(RGB(200,   0, 240));	// s22
		m_colour_scheme[style_index].line[1][4]      = TColor(RGB(  0,   0,   0));

		m_colour_scheme[style_index].line[2][0]      = TColor(RGB(150,   0, 100));	// s11
		m_colour_scheme[style_index].line[2][1]      = TColor(RGB(  0, 150, 100));	// s21
		m_colour_scheme[style_index].line[2][2]      = TColor(RGB(150,   0, 100));	// s12
		m_colour_scheme[style_index].line[2][3]      = TColor(RGB(150, 150, 100));	// s22
		m_colour_scheme[style_index].line[2][4]      = TColor(RGB(  0,   0,   0));

		m_colour_scheme[style_index].line[3][0]      = TColor(RGB(114,  32,  16));	// s11
		m_colour_scheme[style_index].line[3][1]      = TColor(RGB( 32, 114,  16));	// s21
		m_colour_scheme[style_index].line[3][2]      = TColor(RGB(114, 114,  16));	// s12
		m_colour_scheme[style_index].line[3][3]      = TColor(RGB( 32, 114, 114));	// s22
		m_colour_scheme[style_index].line[3][4]      = TColor(RGB(  0,   0,   0));

		m_colour_scheme[style_index].line[4][0]      = TColor(RGB(255,   0, 128));	// s11
		m_colour_scheme[style_index].line[4][1]      = TColor(RGB(128,   0, 128));	// s21
		m_colour_scheme[style_index].line[4][2]      = TColor(RGB(128, 128, 128));	// s12
		m_colour_scheme[style_index].line[4][3]      = TColor(RGB(128, 128,   0));	// s22
		m_colour_scheme[style_index].line[4][4]      = TColor(RGB(  0,   0,   0));
	}

	m_colours = m_colour_scheme[style_index];
}

void __fastcall CSettings::setColourStyle(String style_name)
{
	style_name = style_name.Trim();
	if (style_name.IsEmpty())
		return;

	if (TStyleManager::ActiveStyle->Name != style_name)
		TStyleManager::TrySetStyle(style_name, false);

	const int style_index = styleIndex(style_name);
	if (style_index < 0)
		return;

	m_dark_mode = styles[style_index].dark_mode;
	m_colours   = m_colour_scheme[style_index];

	if (Form1)
	{
/*
sfButtonTextDisabled, sfButtonTextFocused, sfButtonTextHot, sfButtonTextNormal, sfButtonTextPressed,
sfCaptionTextInactive, sfCaptionTextNormal,
sfCategoryPanelGroupHeaderHot, sfCategoryPanelGroupHeaderNormal, sfCatgeoryButtonsCategoryNormal, sfCatgeoryButtonsCategorySelected,
sfCatgeoryButtonsHot, sfCatgeoryButtonsNormal, sfCatgeoryButtonsSelected,
sfCheckBoxTextDisabled, sfCheckBoxTextFocused, sfCheckBoxTextHot, sfCheckBoxTextNormal, sfCheckBoxTextPressed,
sfComboBoxItemDisabled, sfComboBoxItemFocused, sfComboBoxItemHot, sfComboBoxItemNormal, sfComboBoxItemSelected,
sfEditBoxTextDisabled, sfEditBoxTextFocused, sfEditBoxTextHot, sfEditBoxTextNormal, sfEditBoxTextSelected,
sfGridItemFixedHot, sfGridItemFixedNormal, sfGridItemFixedPressed, sfGridItemNormal, sfGridItemSelected,
sfGroupBoxTextDisabled, sfGroupBoxTextNormal,
sfHeaderSectionTextDisabled, sfHeaderSectionTextHot, sfHeaderSectionTextNormal, sfHeaderSectionTextPressed,
sfListItemTextDisabled, sfListItemTextFocused, sfListItemTextHot, sfListItemTextNormal, sfListItemTextSelected,
sfMenuItemTextDisabled, sfMenuItemTextHot, sfMenuItemTextNormal, sfMenuItemTextSelected,
sfPanelTextDisabled, sfPanelTextNormal,
sfPopupMenuItemTextDisabled, sfPopupMenuItemTextHot, sfPopupMenuItemTextNormal, sfPopupMenuItemTextSelected,
sfRadioButtonTextDisabled, sfRadioButtonTextFocused, sfRadioButtonTextHot, sfRadioButtonTextNormal, sfRadioButtonTextPressed,
sfSmCaptionTextInactive, sfSmCaptionTextNormal,
sfStatusPanelTextDisabled, sfStatusPanelTextNormal,
sfTabTextActiveDisabled, sfTabTextActiveHot, sfTabTextActiveNormal, sfTabTextInactiveDisabled, sfTabTextInactiveHot, sfTabTextInactiveNormal,
sfTextLabelDisabled, sfTextLabelFocused, sfTextLabelHot, sfTextLabelNormal,
sfToolItemTextDisabled, sfToolItemTextHot, sfToolItemTextNormal, sfToolItemTextSelected,
sfTreeItemTextDisabled, sfTreeItemTextFocused, sfTreeItemTextHot, sfTreeItemTextNormal, sfTreeItemTextSelected,
sfWindowTextDisabled, sfWindowTextNormal
*/
		TColor colour = TStyleManager::ActiveStyle->GetStyleFontColor(sfStatusPanelTextNormal);
//		TColor colour = (m_dark_mode) ? clWhite : clBlack;
		Form1->StatusBar1->Font->Color = colour;
		Form1->StatusBar2->Font->Color = colour;
	}
}

void __fastcall CSettings::saveColourScheme()
{
	const int style_index = styleIndex(TStyleManager::ActiveStyle->Name);

	if (style_index >= 0)
		m_colour_scheme[style_index] = m_colours;
}

void __fastcall CSettings::setDefaultColourScheme(const bool dark_colours)
{
	const int style_index = styleIndex(TStyleManager::ActiveStyle->Name);
	setDefaultColourScheme(style_index, dark_colours);
}

void __fastcall CSettings::setDefaultColourScheme()
{
	const int style_index = styleIndex(TStyleManager::ActiveStyle->Name);
	setDefaultColourScheme(style_index, styles[style_index].dark_mode);
}

void __fastcall CSettings::setGraphFont(TFont *font)
{
	if (m_graph_font && font)
	{
		m_graph_font->Name    = font->Name;
		m_graph_font->Charset = font->Charset;
		m_graph_font->Color   = font->Color;
		m_graph_font->Size    = font->Size;
//		m_graph_font->Pitch   = TFontPitch::fpFixed;
//		m_graph_font->Quality = TFontQuality::fqAntialiased;
	}
}

void __fastcall CSettings::setRecordFolder(String folder)
{
	folder = folder.Trim();
	while (!folder.IsEmpty())
	{
		const int i = folder.Pos("\\.\\");
		if (i <= 0)
			break;
		folder = folder.Delete(i, 2).Trim();
	}
	m_record_folder = IncludeTrailingPathDelimiter(folder);
}

void __fastcall CSettings::setCalibrationFolder(String folder)
{
	folder = folder.Trim();
	while (!folder.IsEmpty())
	{
		const int i = folder.Pos("\\.\\");
		if (i <= 0)
			break;
		folder = folder.Delete(i, 2).Trim();
	}
	m_calibration_folder = IncludeTrailingPathDelimiter(folder);
}

void __fastcall CSettings::clipGraphMinMax(const int graph_type, double &max_val, double &min_val)
{
	if (graph_type < 0 || graph_type >= GRAPH_TYPE_SIZE)
		return;

	// ***************
	// min value

	switch (graph_type)
	{
		case GRAPH_TYPE_LOGMAG_S11:
		case GRAPH_TYPE_LOGMAG_S21:
		case GRAPH_TYPE_LOGMAG_S11S21:
		case GRAPH_TYPE_CAL_LOGMAG:
		case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11:
		case GRAPH_TYPE_TDR_LOG_BP_S11:
			if (min_val > LOG_MAG_MAX) min_val = LOG_MAG_MAX;
			else
			if (min_val < LOG_MAG_MIN) min_val = LOG_MAG_MIN;
			break;
		case GRAPH_TYPE_LINMAG_S11:
		case GRAPH_TYPE_LINMAG_S21:
		case GRAPH_TYPE_LINMAG_S11S21:
		case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
		case GRAPH_TYPE_TDR_LIN_BP_S11:
			if (min_val > LIN_MAG_MAX) min_val = LIN_MAG_MAX;
			else
			if (min_val < LIN_MAG_MIN) min_val = LIN_MAG_MIN;
			break;
		case GRAPH_TYPE_PHASE_S11:
		case GRAPH_TYPE_PHASE_S21:
		case GRAPH_TYPE_PHASE_S11S21:
		case GRAPH_TYPE_PHASE_UNWRAP_S11:
		case GRAPH_TYPE_PHASE_UNWRAP_S21:
		case GRAPH_TYPE_PHASE_UNWRAP_S11S21:
			if (min_val > PHASE_DEG_MAX) min_val = PHASE_DEG_MAX;
			else
			if (min_val < PHASE_DEG_MIN) min_val = PHASE_DEG_MIN;
			break;
		case GRAPH_TYPE_VSWR_S11:
			if (min_val > (VSWR_MAX - 1)) min_val = VSWR_MAX - 1;
			else
			if (min_val < VSWR_MIN) min_val = VSWR_MIN;
			//min_val = VSWR_MIN;
			break;
		case GRAPH_TYPE_IMPEDANCE_S11:
		case GRAPH_TYPE_TDR_IMPEDANCE_S11:
			if (min_val > IMPEDANCE_MAX) min_val = IMPEDANCE_MAX;
			else
			if (min_val < IMPEDANCE_MIN) min_val = IMPEDANCE_MIN;
			break;
		case GRAPH_TYPE_SERIES_RJX_S11:
		case GRAPH_TYPE_PARALLEL_RJX_S11:
		case GRAPH_TYPE_SERIES_RESISTANCE_S11:
		case GRAPH_TYPE_SERIES_REACTANCE_S11:
		case GRAPH_TYPE_GJB_S11:
			if (min_val > RJX_MAX) min_val = RJX_MAX;
			else
			if (min_val < RJX_MIN) min_val = RJX_MIN;
			break;
		case GRAPH_TYPE_QUALITY_FACTOR_S11:
			if (min_val > QUALITY_FACTOR_MAX) min_val = QUALITY_FACTOR_MAX;
			else
			if (min_val < QUALITY_FACTOR_MIN) min_val = QUALITY_FACTOR_MIN;
			break;
		case GRAPH_TYPE_SMITH_S11:
		case GRAPH_TYPE_SMITH_S21:
		case GRAPH_TYPE_ADMITTANCE_S11:
		case GRAPH_TYPE_ADMITTANCE_S21:
		case GRAPH_TYPE_POLAR_S11:
		case GRAPH_TYPE_POLAR_S21:
			min_val = 0;
			break;
		case GRAPH_TYPE_GROUP_DELAY_S11:
		case GRAPH_TYPE_GROUP_DELAY_S21:
		case GRAPH_TYPE_GROUP_DELAY_S11S21:
			if (min_val > GROUP_DELAY_MAX) min_val = GROUP_DELAY_MAX;
			else
			if (min_val < GROUP_DELAY_MIN) min_val = GROUP_DELAY_MIN;
			break;
		case GRAPH_TYPE_REAL_IMAG_S11:
		case GRAPH_TYPE_REAL_IMAG_S21:
			if (min_val > RI_MAX) min_val = RI_MAX;
			else
			if (min_val < RI_MIN) min_val = RI_MIN;
			break;
		case GRAPH_TYPE_SERIES_CAPACITANCE_S11:
			if (min_val > CAP_MAX) min_val = CAP_MAX;
			else
			if (min_val < CAP_MIN) min_val = CAP_MIN;
			break;
		case GRAPH_TYPE_SERIES_INDUCTANCE_S11:
			if (min_val > IND_MAX) min_val = IND_MAX;
			else
			if (min_val < IND_MIN) min_val = IND_MIN;
			break;
		case GRAPH_TYPE_COAX_LOSS_S11:
			if (min_val > LOG_MAG_MAX) min_val = LOG_MAG_MAX;
			else
			if (min_val < LOG_MAG_MIN) min_val = LOG_MAG_MIN;
			break;
		case GRAPH_TYPE_PHASE_VECTOR_S11:
		case GRAPH_TYPE_PHASE_VECTOR_S21:
			break;
		default:
			break;
	}

	// ***************
	// max value

	switch (graph_type)
	{
		case GRAPH_TYPE_LOGMAG_S11:
		case GRAPH_TYPE_LOGMAG_S21:
		case GRAPH_TYPE_LOGMAG_S11S21:
		case GRAPH_TYPE_CAL_LOGMAG:
		case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11:
		case GRAPH_TYPE_TDR_LOG_BP_S11:
			if (max_val > LOG_MAG_MAX) max_val = LOG_MAG_MAX;
			else
			if (max_val < LOG_MAG_MIN) max_val = LOG_MAG_MIN;
			break;
		case GRAPH_TYPE_LINMAG_S11:
		case GRAPH_TYPE_LINMAG_S21:
		case GRAPH_TYPE_LINMAG_S11S21:
		case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
		case GRAPH_TYPE_TDR_LIN_BP_S11:
			if (max_val > LIN_MAG_MAX) max_val = LIN_MAG_MAX;
			else
			if (max_val < LIN_MAG_MIN) max_val = LIN_MAG_MIN;
			break;
		case GRAPH_TYPE_PHASE_S11:
		case GRAPH_TYPE_PHASE_S21:
		case GRAPH_TYPE_PHASE_S11S21:
		case GRAPH_TYPE_PHASE_UNWRAP_S11:
		case GRAPH_TYPE_PHASE_UNWRAP_S21:
		case GRAPH_TYPE_PHASE_UNWRAP_S11S21:
			if (max_val > PHASE_DEG_MAX) max_val = PHASE_DEG_MAX;
			else
			if (max_val < PHASE_DEG_MIN) max_val = PHASE_DEG_MIN;
			break;
		case GRAPH_TYPE_VSWR_S11:
			if (max_val > VSWR_MAX) max_val = VSWR_MAX;
			else
			if (max_val < (VSWR_MIN + 0.05)) max_val = VSWR_MIN + 0.05;
			break;
		case GRAPH_TYPE_IMPEDANCE_S11:
		case GRAPH_TYPE_TDR_IMPEDANCE_S11:
			if (max_val > IMPEDANCE_MAX) max_val = IMPEDANCE_MAX;
			else
			if (max_val < IMPEDANCE_MIN) max_val = IMPEDANCE_MIN;
			break;
		case GRAPH_TYPE_SERIES_RJX_S11:
		case GRAPH_TYPE_PARALLEL_RJX_S11:
		case GRAPH_TYPE_SERIES_RESISTANCE_S11:
		case GRAPH_TYPE_SERIES_REACTANCE_S11:
		case GRAPH_TYPE_GJB_S11:
			if (max_val > RJX_MAX) max_val = RJX_MAX;
			else
			if (max_val < RJX_MIN) max_val = RJX_MIN;
			break;
		case GRAPH_TYPE_QUALITY_FACTOR_S11:
			if (max_val > QUALITY_FACTOR_MAX) max_val = QUALITY_FACTOR_MAX;
			else
			if (max_val < QUALITY_FACTOR_MIN) max_val = QUALITY_FACTOR_MIN;
			break;
		case GRAPH_TYPE_SMITH_S11:
		case GRAPH_TYPE_SMITH_S21:
		case GRAPH_TYPE_ADMITTANCE_S11:
		case GRAPH_TYPE_ADMITTANCE_S21:
		case GRAPH_TYPE_POLAR_S11:
		case GRAPH_TYPE_POLAR_S21:
			if (max_val > 10.0) max_val = 10.0;
			else
			if (max_val < 1e-6) max_val = 1e-6;
			break;
		case GRAPH_TYPE_GROUP_DELAY_S11:
		case GRAPH_TYPE_GROUP_DELAY_S21:
		case GRAPH_TYPE_GROUP_DELAY_S11S21:
			if (max_val > GROUP_DELAY_MAX) max_val = GROUP_DELAY_MAX;
			else
			if (max_val < GROUP_DELAY_MIN) max_val = GROUP_DELAY_MIN;
			break;
		case GRAPH_TYPE_REAL_IMAG_S11:
		case GRAPH_TYPE_REAL_IMAG_S21:
			if (max_val > RI_MAX) max_val = RI_MAX;
			else
			if (max_val < RI_MIN) max_val = RI_MIN;
			break;
		case GRAPH_TYPE_SERIES_CAPACITANCE_S11:
			if (max_val > CAP_MAX) max_val = CAP_MAX;
			else
			if (max_val < CAP_MIN) max_val = CAP_MIN;
			break;
		case GRAPH_TYPE_SERIES_INDUCTANCE_S11:
			if (max_val > IND_MAX) max_val = IND_MAX;
			else
			if (max_val < IND_MIN) max_val = IND_MIN;
			break;
		case GRAPH_TYPE_COAX_LOSS_S11:
			if (max_val > LOG_MAG_MAX) max_val = LOG_MAG_MAX;
			else
			if (max_val < LOG_MAG_MIN) max_val = LOG_MAG_MIN;
			break;
		case GRAPH_TYPE_PHASE_VECTOR_S11:
		case GRAPH_TYPE_PHASE_VECTOR_S21:
			break;
		default:
			break;
	}

	// ***************
	// minimum allowed min/max difference

	switch (graph_type)
	{
		case GRAPH_TYPE_LOGMAG_S11:
		case GRAPH_TYPE_LOGMAG_S21:
		case GRAPH_TYPE_LOGMAG_S11S21:
		case GRAPH_TYPE_CAL_LOGMAG:
		case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11:
		case GRAPH_TYPE_TDR_LOG_BP_S11:
			if (max_val < (min_val + 1e-4))
				 max_val =  min_val + 1e-4;
			break;
		case GRAPH_TYPE_LINMAG_S11:
		case GRAPH_TYPE_LINMAG_S21:
		case GRAPH_TYPE_LINMAG_S11S21:
		case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
		case GRAPH_TYPE_TDR_LIN_BP_S11:
			if (max_val < (min_val + 1e-4))
				 max_val =  min_val + 1e-4;
			break;
		case GRAPH_TYPE_PHASE_S11:
		case GRAPH_TYPE_PHASE_S21:
		case GRAPH_TYPE_PHASE_S11S21:
		case GRAPH_TYPE_PHASE_UNWRAP_S11:
		case GRAPH_TYPE_PHASE_UNWRAP_S21:
		case GRAPH_TYPE_PHASE_UNWRAP_S11S21:
			if (max_val < (min_val + 1e-4))
				 max_val =  min_val + 1e-4;
			break;
		case GRAPH_TYPE_VSWR_S11:
			if (max_val < (min_val + 0.05))
				 max_val =  min_val + 0.05;
			break;
		case GRAPH_TYPE_IMPEDANCE_S11:
		case GRAPH_TYPE_TDR_IMPEDANCE_S11:
			if (max_val < (min_val + 1e-4))
				 max_val =  min_val + 1e-4;
			break;
		case GRAPH_TYPE_SERIES_RJX_S11:
		case GRAPH_TYPE_PARALLEL_RJX_S11:
		case GRAPH_TYPE_SERIES_RESISTANCE_S11:
		case GRAPH_TYPE_SERIES_REACTANCE_S11:
//check
		case GRAPH_TYPE_GJB_S11:
			if (max_val < (min_val + 1e-4))
				 max_val =  min_val + 1e-4;
			break;
		case GRAPH_TYPE_QUALITY_FACTOR_S11:
			if (max_val < (min_val + 1e-4))
				 max_val =  min_val + 1e-4;
			break;
		case GRAPH_TYPE_SMITH_S11:
		case GRAPH_TYPE_SMITH_S21:
		case GRAPH_TYPE_ADMITTANCE_S11:
		case GRAPH_TYPE_ADMITTANCE_S21:
		case GRAPH_TYPE_POLAR_S11:
		case GRAPH_TYPE_POLAR_S21:
			if (max_val < (min_val + 1e-6))
				 max_val =  min_val + 1e-6;
			break;
		case GRAPH_TYPE_GROUP_DELAY_S11:
		case GRAPH_TYPE_GROUP_DELAY_S21:
		case GRAPH_TYPE_GROUP_DELAY_S11S21:
			if (max_val < (min_val + 1e-4))
				 max_val =  min_val + 1e-4;
			break;
		case GRAPH_TYPE_REAL_IMAG_S11:
		case GRAPH_TYPE_REAL_IMAG_S21:
			if (max_val < (min_val + 1e-9))
				 max_val =  min_val + 1e-9;
			break;
		case GRAPH_TYPE_SERIES_CAPACITANCE_S11:
			if (max_val < (min_val + 1e-15))
				 max_val =  min_val + 1e-15;
			break;
		case GRAPH_TYPE_SERIES_INDUCTANCE_S11:
			if (max_val < (min_val + 1e-15))
				 max_val =  min_val + 1e-15;
			break;
		case GRAPH_TYPE_COAX_LOSS_S11:
			if (max_val < (min_val + 1e-5))
				 max_val =  min_val + 1e-5;
			break;
		case GRAPH_TYPE_PHASE_VECTOR_S11:
		case GRAPH_TYPE_PHASE_VECTOR_S21:
			if (max_val < (min_val + 1e-4))
				 max_val =  min_val + 1e-4;
			break;
		default:
			if (max_val < (min_val + 1e-4))
				 max_val =  min_val + 1e-4;
			break;
	}

	// ***************
}

void __fastcall CSettings::setDefaultGraphScales()
{
	for (int i = 0; i < GRAPH_TYPE_SIZE; i++)
	{
		t_graph_setting *gs = &m_graph_setting[i];

		gs->auto_max        = false;
		gs->auto_min        = false;
		gs->max             =  100.0;
		gs->min             = -100.0;
		gs->gamma           = 1.0;
		gs->show_min_max    = false;
		gs->show_max_marker = false;
		gs->show_min_marker = false;
		gs->show_markers    = true;

		switch (i)
		{
			case GRAPH_TYPE_LOGMAG_S11:
			case GRAPH_TYPE_LOGMAG_S21:
			case GRAPH_TYPE_LOGMAG_S11S21:
			case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11:
			case GRAPH_TYPE_TDR_LOG_BP_S11:
				gs->max      =  10.0;
				gs->min      = -100.0;
			  break;
			case GRAPH_TYPE_CAL_LOGMAG:
				gs->max      =  10.0;
				gs->min      = -100.0;
			  break;
			case GRAPH_TYPE_LINMAG_S11:
			case GRAPH_TYPE_LINMAG_S21:
			case GRAPH_TYPE_LINMAG_S11S21:
			case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
			case GRAPH_TYPE_TDR_LIN_BP_S11:
				gs->max      = 1.2;
				gs->min      = 0.0;
				break;
			case GRAPH_TYPE_PHASE_S11:
			case GRAPH_TYPE_PHASE_S21:
			case GRAPH_TYPE_PHASE_S11S21:
			case GRAPH_TYPE_PHASE_UNWRAP_S11:
			case GRAPH_TYPE_PHASE_UNWRAP_S21:
			case GRAPH_TYPE_PHASE_UNWRAP_S11S21:
				gs->max      =  210.0;
				gs->min      = -210.0;
				break;
			case GRAPH_TYPE_VSWR_S11:
				gs->max   = 50.0;
				gs->min   = VSWR_MIN;
				gs->gamma = VSWR_GAMMA_DEFAULT;
				break;
			case GRAPH_TYPE_IMPEDANCE_S11:
			case GRAPH_TYPE_TDR_IMPEDANCE_S11:
				gs->max = 100.0;
				gs->min =   0.0;
				break;
			case GRAPH_TYPE_SERIES_RJX_S11:
			case GRAPH_TYPE_SERIES_REACTANCE_S11:
				gs->max =  500.0;
				gs->min = -500.0;
				break;
			case GRAPH_TYPE_SERIES_RESISTANCE_S11:
				gs->max = 100.0;
				gs->min =   0.0;
				break;
			case GRAPH_TYPE_PARALLEL_RJX_S11:
				gs->max =  1000.0;
				gs->min = -1000.0;
				break;
			case GRAPH_TYPE_GJB_S11:
				gs->max =  1/1000.0;
				gs->min = -1/1000.0;
				break;
			case GRAPH_TYPE_QUALITY_FACTOR_S11:
				gs->max = 150.0;
				gs->min =   0.0;
				break;
			case GRAPH_TYPE_GROUP_DELAY_S11:
			case GRAPH_TYPE_GROUP_DELAY_S21:
			case GRAPH_TYPE_GROUP_DELAY_S11S21:
				gs->auto_max = true;
				gs->auto_min = true;
				gs->max      =  100.0;
				gs->min      = -100.0;
				break;
			case GRAPH_TYPE_REAL_IMAG_S11:
			case GRAPH_TYPE_REAL_IMAG_S21:
				gs->max      =  1.2;
				gs->min      = -1.2;
				break;
			case GRAPH_TYPE_SERIES_CAPACITANCE_S11:
				gs->auto_max = true;
				gs->auto_min = true;
				gs->max      =  1e-9;
				gs->min      = -1e-9;
				break;
			case GRAPH_TYPE_SERIES_INDUCTANCE_S11:
				gs->auto_max = true;
				gs->auto_min = true;
				gs->max      =  1e-9;
				gs->min      = -1e-9;
				break;
			case GRAPH_TYPE_COAX_LOSS_S11:
				gs->max      =   5.0;
				gs->min      = -20.0;
				break;
			case GRAPH_TYPE_SMITH_S11:
			case GRAPH_TYPE_SMITH_S21:
			case GRAPH_TYPE_ADMITTANCE_S11:
			case GRAPH_TYPE_ADMITTANCE_S21:
			case GRAPH_TYPE_POLAR_S11:
			case GRAPH_TYPE_POLAR_S21:
				gs->max = 1.0;
				gs->min = 0.0;
				break;
			case GRAPH_TYPE_PHASE_VECTOR_S11:
			case GRAPH_TYPE_PHASE_VECTOR_S21:
				break;
			default:
				break;
		}
	}
}

void __fastcall CSettings::setBorderWidth(int value)
{
	if (value <  0) value = 0;
	else
	if (value > 10) value = 10;

	m_border_width = value;
}

