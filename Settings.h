
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef SettingsH
#define SettingsH

#pragma once

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <vector>

#include "types.h"

class CSettings
{
private:
	String m_settings_filename;
/*
		char m_field_sep;
		char m_decimal_point;
*/
	t_window_pos m_main_form;
	t_window_pos m_comms_form;
	t_window_pos m_vna_usart_comms_form;
	t_window_pos m_battery_form;
	t_window_pos m_settings_form;
	t_window_pos m_firmware_form;
	t_window_pos m_firmware_v2_form;
	t_window_pos m_calibration_form;
	t_window_pos m_screen_capture_form;

	struct
	{
		String address;
		int    port;
	} m_tcpip;

	double m_edelay_secs;

	float m_s21_offset_dB;

	bool m_spline_enable;
	bool m_show_points;
	bool m_clip_traces;
	bool m_snap_to_nearest_point;
	bool m_auto_scale_peak_hold;

	bool m_smith_both_scales;

	int m_line_width;
	int m_line_alpha;

	int m_border_width;

	bool m_show_freq_bands;

	bool m_lc_matching_enable;

	bool m_marker_fill;

	bool m_show_markers_on_graph;
	bool m_show_marker_text;

	String m_gui_style_name;

	t_graph_arrange m_graph_arrangement;

	bool m_info_panel;

	struct
	{
		String name;
		int    baudrate;
	} m_serial_port;

	int64_t m_start_Hz;
	int64_t m_stop_Hz;
	int64_t m_cw_Hz;

	int m_point_bandwidth;

	int m_num_points;

	int m_time_average_level;

	struct
	{
		float value;
		String name;
	} m_velocity_factor;

	String m_sweep_name;

	String m_record_folder;

	String m_calibration_folder;
	String m_calibration_file;
	t_calibration_selection m_calibration_selection;
	int m_average_calibration;
	int m_smoothing_calibration;
	int m_median_calibration;

	int m_tdr_window;

	int m_output_power;

	int m_median_filter_level;

	int m_curve_smoothing_level;

	int m_vna_usart_command_ends;

	bool m_norm_enabled;

	struct
	{
		bool   enabled;
		String name;
	} m_memory[MAX_MEMORIES];

	int m_graph_type[MAX_GRAPHS];

	TFont *m_graph_font;

	int m_freq_band_sweep_padding_percent;

	std::vector <t_colour_scheme> m_colour_scheme;

	void __fastcall setGraphType(const int index, const int type);
	int __fastcall getGraphType(const int index);

	bool __fastcall getMemoryEnabled();
	bool __fastcall getMemoryEnable(const int index);
	void __fastcall setMemoryEnable(const int index, const bool enabled);
	String __fastcall getMemoryName(const int index);
	void __fastcall setMemoryName(const int index, String name);

	int __fastcall styleIndex(String name);
	void __fastcall setDefaultColourScheme(const int style_index, const bool dark_colours);

	void __fastcall setGraphFont(TFont *font);

	void __fastcall setRecordFolder(String folder);

	void __fastcall setCalibrationFolder(String folder);

	void __fastcall setTCPIPAddress(String value);
	void __fastcall setTCPIPPort(int value);

	void __fastcall setBorderWidth(int value);

public:
	CSettings();
	~CSettings();

	std::vector <t_marker_freq> m_markers_freq;
	std::vector <t_marker_time> m_markers_time;

	t_graph_setting m_graph_setting[GRAPH_TYPE_SIZE];

	std::vector <t_freq_band> m_freq_band;

	bool m_dark_mode;
	t_colour_scheme m_colours;

	void __fastcall load();
	void __fastcall save();

	void __fastcall clipGraphMinMax(const int graph_type, double &max_val, double &min_val);
	void __fastcall setDefaultGraphScales();

	void __fastcall defaultFreqBands();
	void __fastcall sortFreqBand();

	void __fastcall saveColourScheme();
	void __fastcall setColourStyle(String style_name);
	void __fastcall setDefaultColourScheme(const bool dark_colours);
	void __fastcall setDefaultColourScheme();

	__property String settingsFilename          = {read = m_settings_filename};
/*
		__property char fieldSep     = {read = m_field_sep, write = m_field_sep};
		__property char decimalPoint = {read = m_decimal_point, write = m_decimal_point};
*/
	__property t_window_pos mainWindowPos          = {read = m_main_form,            write = m_main_form};
	__property t_window_pos commsWindowPos         = {read = m_comms_form,           write = m_comms_form};
	__property t_window_pos vnaUSARTcommsWindowPos = {read = m_vna_usart_comms_form, write = m_vna_usart_comms_form};
	__property t_window_pos batteryWindowPos       = {read = m_battery_form,         write = m_battery_form};
	__property t_window_pos settingsWindowPos      = {read = m_settings_form,        write = m_settings_form};
	__property t_window_pos firmwareWindowPos      = {read = m_firmware_form,        write = m_firmware_form};
	__property t_window_pos firmwareV2WindowPos    = {read = m_firmware_v2_form,     write = m_firmware_v2_form};
	__property t_window_pos calibrationWindowPos   = {read = m_calibration_form,     write = m_calibration_form};
	__property t_window_pos screenCaptureWindowPos = {read = m_screen_capture_form,  write = m_screen_capture_form};

	__property bool splineEnable           = {read = m_spline_enable, write = m_spline_enable};
	__property bool showPoints             = {read = m_show_points, write = m_show_points};
	__property bool clipTraces             = {read = m_clip_traces, write = m_clip_traces};
	__property bool snapToNearestPoint     = {read = m_snap_to_nearest_point, write = m_snap_to_nearest_point};
	__property bool autoScalePeakHold      = {read = m_auto_scale_peak_hold, write = m_auto_scale_peak_hold};

	__property bool smithBothScales = {read = m_smith_both_scales, write = m_smith_both_scales};

	__property float s21OffsetdB           = {read = m_s21_offset_dB, write = m_s21_offset_dB};
	__property double eDelaySecs           = {read = m_edelay_secs, write = m_edelay_secs};
	__property int lineWidth               = {read = m_line_width, write = m_line_width};
	__property int lineAlpha               = {read = m_line_alpha, write = m_line_alpha};
	__property int borderWidth             = {read = m_border_width, write = setBorderWidth};
	__property bool showFrequencyBands     = {read = m_show_freq_bands, write = m_show_freq_bands};
	__property bool markerFill             = {read = m_marker_fill, write = m_marker_fill};
	__property bool showMarkersOnGraph     = {read = m_show_markers_on_graph, write = m_show_markers_on_graph};
	__property bool showMarkerText         = {read = m_show_marker_text, write = m_show_marker_text};
	__property bool lcMatchingEnable       = {read = m_lc_matching_enable, write = m_lc_matching_enable};
	__property String guiStyleName         = {read = m_gui_style_name, write = m_gui_style_name};
	__property float velocityFactor        = {read = m_velocity_factor.value, write = m_velocity_factor.value};
	__property String velocityFactorName   = {read = m_velocity_factor.name, write = m_velocity_factor.name};
	__property bool infoPanel              = {read = m_info_panel, write = m_info_panel};

	__property String serialPortName       = {read = m_serial_port.name, write = m_serial_port.name};
	__property int serialPortBaudrate      = {read = m_serial_port.baudrate, write = m_serial_port.baudrate};

	__property String sweepName            = {read = m_sweep_name, write = m_sweep_name};

	__property int64_t startHz             = {read = m_start_Hz, write = m_start_Hz};
	__property int64_t stopHz              = {read = m_stop_Hz, write = m_stop_Hz};
	__property int64_t cwHz                = {read = m_cw_Hz, write = m_cw_Hz};
	__property int pointBandwidth          = {read = m_point_bandwidth, write = m_point_bandwidth};
	__property int numOfPoints             = {read = m_num_points, write = m_num_points};
	__property int timeAverageLevel        = {read = m_time_average_level, write = m_time_average_level};
	__property int medianFilterLevel       = {read = m_median_filter_level, write = m_median_filter_level};
	__property int curveSmoothingLevel     = {read = m_curve_smoothing_level, write = m_curve_smoothing_level};

	__property int vnaUSARTCommandEnds     = {read = m_vna_usart_command_ends, write = m_vna_usart_command_ends};

	__property t_graph_arrange graphArrangement = {read = m_graph_arrangement, write = m_graph_arrangement};

	__property int averageCalibration      = {read = m_average_calibration,   write = m_average_calibration};
	__property int smoothingCalibration    = {read = m_smoothing_calibration, write = m_smoothing_calibration};
	__property int medianCalibration       = {read = m_median_calibration,    write = m_median_calibration};
	__property t_calibration_selection calibrationSelection = {read = m_calibration_selection, write = m_calibration_selection};

	__property int tdrWindow               = {read = m_tdr_window, write = m_tdr_window};

	__property int outputPower             = {read = m_output_power, write = m_output_power};

	__property int graphType[const int index] = {read = getGraphType, write = setGraphType};

	__property bool normalisationEnabled   = {read = m_norm_enabled, write = m_norm_enabled};

	__property bool memoryEnabled                     = {read = getMemoryEnabled};
	__property bool memoryEnable[const int index]     = {read = getMemoryEnable, write = setMemoryEnable};
	__property String memoryName[const int index]     = {read = getMemoryName,   write = setMemoryName};

	__property TFont *graphFont           = {read = m_graph_font, write = setGraphFont};

	__property String recordFolder        = {read = m_record_folder, write = setRecordFolder};

	__property String calibrationFolder   = {read = m_calibration_folder, write = setCalibrationFolder};
	__property String calibrationFile     = {read = m_calibration_file,   write = m_calibration_file};

	__property int freqBandPaddingPercent = {read = m_freq_band_sweep_padding_percent, write = m_freq_band_sweep_padding_percent};

	__property String tcpipAddress  = {read = m_tcpip.address, write = setTCPIPAddress};
	__property int    tcpipPort     = {read = m_tcpip.port,    write = setTCPIPPort};
};

extern PACKAGE CSettings settings;

#endif
