
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef DataUnitH
#define DataUnitH

#include <vector>

#include "types.h"
#include "settings.h"

class CData
{
	private:

	public:
		CData();

		std::vector <uint8_t> beep_wav;
		std::vector <uint8_t> phurp_wav;
		std::vector <uint8_t> stereophonic_wav;
		std::vector <uint8_t> squeak_wav;

		float m_velocity_factor;
		float m_max_distance_meters;

		struct
		{
			String name;

			String help;
			String version;
			std::vector <String> info;

			bool dislord;
			bool oneofeleven;

			bool cmd_capture;
			bool cmd_integrator;
			bool cmd_scan_bin;
			bool cmd_scanraw;
			bool cmd_sd_list;
			bool cmd_sd_readfile;
			bool cmd_bandwidth;
			bool cmd_vbat;
			bool cmd_vbat_offset;
         bool cmd_marker;
			bool cmd_time;
			bool cmd_threshold;
			bool cmd_pause;
			bool cmd_resume;
			bool cmd_reset;
			bool cmd_cal;
			bool cmd_power;
			bool cmd_usart;
			bool cmd_usart_cfg;
			bool cmd_deviceid;
			bool cmd_sweep;
			bool cmd_mode;
			bool cmd_edelay;

			t_unit_type type;
			int lcd_width;
			int lcd_height;
			int max_points;
			int max_bandwidth_Hz;
			int if_Hz;
			int adc_Hz;
			int audio_samples_count;
			int bandwidth;
			int bandwidth_Hz;
			int vbat_mv;
			int vbat_offset_mv;
			int power;
			int usart_speed;
			int64_t deviceid;
			bool cal;
			float edelay;

			int num_points;

			int64_t freq_max_Hz;
			int64_t freq_min_Hz;

			int64_t freq_threshold_Hz;
			int64_t freq_start_Hz;
			int64_t freq_stop_Hz;
			int64_t freq_center_Hz;
			int64_t freq_span_Hz;
			int64_t freq_cw_Hz;
			int64_t freq_Hz;

			// NanoVNA V2 specific
			uint8_t protool_version;
			uint8_t hardware_revision;
			uint8_t firmware_major;
			uint8_t firmware_minor;
		} m_vna_data;

		int64_t m_freq_start_Hz;
		int64_t m_freq_stop_Hz;
		int64_t m_freq_center_Hz;
		int64_t m_freq_span_Hz;
		int64_t m_freq_cw_Hz;

		// where we store the all the past sweep results, used for time averaging
		std::vector <t_data_point_hist> m_point;

		// incoming s-points
		std::vector <t_data_point>   m_point_incoming;

		// SnP memories .. mem[0] is the live memory
		std::vector <t_data_point>   m_point_mem[MAX_MEMORIES];

		// filtered SnP memories
		std::vector <t_data_point>   m_point_filt[MAX_MEMORIES];

		// normalise memory
		std::vector <t_data_point>   m_point_norm;

		// FFT results - used for the TDR calculations
		std::vector <float>    m_fft_window[MAX_GRAPHS][MAX_MEMORIES];
		std::vector < complexf > m_fft_buffer[MAX_GRAPHS][MAX_MEMORIES];
		std::vector <float>    m_fft_pwr[MAX_GRAPHS][MAX_MEMORIES];
		std::vector <float>    m_fft_mag[MAX_GRAPHS][MAX_MEMORIES];
		std::vector <float>    m_fft_dB10[MAX_GRAPHS][MAX_MEMORIES];
		float                  m_fft_peak_index[MAX_GRAPHS][MAX_MEMORIES];
		float                  m_fft_peak_mag[MAX_GRAPHS][MAX_MEMORIES];

		// total number of incomings sweeps/scans we've had
		int m_total_frames;

		int m_history_index;
		int m_history_frames;

		int m_points;
		int m_points_per_segment;
		int m_segments;
		int m_segment;

		int m_bandwidth_Hz;

//		int m_power;

		bool __fastcall validFrequencySettings();

		void __fastcall resetUnitData();

		void __fastcall minMaxFreqHz(int64_t &min_Hz, int64_t &max_Hz);

		int __fastcall freqArraySize(const int mem);
		int __fastcall indexFreq(const int64_t freq, const int mem);
		int __fastcall firstUsedMem(const bool only_enabled = true, int mem = 0);

		//__property int64_t frequency[const int mem][const int index] = {read = getFrequency};
		int64_t __fastcall getFrequency(const int mem, const int index);

		void __fastcall clearHistory()
		{
			m_history_index  = 0;
			m_history_frames = 0;
		}

		// ***************************************

		//m_max_distance_meters = (data_unit.freq_array.freq_stop_Hz > 0) ? (5850000000 * m_velocity_factor) / data_unit.freq_array.freq_stop_Hz : 0;
		//stop_freq_Hz          = (5850000000 * m_velocity_factor) / m_max_distance_meters;

		double __fastcall freq_step(const int mem);
		double __fastcall max_time(const double freq_step);
		double __fastcall max_dist(const double freq_step, const double velocity_factor);
		float __fastcall power(complexf c);
		float __fastcall magnitude(complexf c);
		float __fastcall gain10(complexf c);
		float __fastcall gain20(complexf z);
		float __fastcall phase(complexf c);
		float __fastcall VSWR(complexf c);
		complexf __fastcall parallelToSerial(complexf c);
		complexf __fastcall serialToParallel(complexf z);
		float __fastcall impedanceToCapacitance(complexf z, const double freq);
		float __fastcall impedanceToInductance(complexf z, double freq);
		complexf __fastcall impedanceToNorm(complexf z, const float ref_impedance = 50.0f);
		complexf __fastcall normToImpedance(complexf z, const float ref_impedance = 50.0f);
		complexf __fastcall reflectionCoefficient(complexf z, const float ref_impedance = 50.0f);
		complexf __fastcall gammaToImpedance(complexf gamma, const float ref_impedance = 50.0f);
		complexf __fastcall impedance(complexf c, const float ref_impedance = 50.0f);
		float __fastcall qualityFactor(complexf c, const float ref_impedance = 50.0f);
		float __fastcall capacitiveEquivalent(complexf c, const double freq, const float ref_impedance = 50.0f);
		float __fastcall inductiveEquivalent(complexf c, const double freq, const float ref_impedance = 50.0f);

		// ***************************************

//		__property std::vector <t_data_point_hist> point         = {read = m_point,          write = m_point};
//		__property std::vector <t_data_point>      pointIncoming = {read = m_point_incoming, write = m_point_incoming};
//		__property std::vector <t_data_point>      pointMem      = {read = m_point_mem,      write = m_point_mem};
//		__property std::vector <t_data_point>      pointFilt     = {read = m_point_filt,     write = m_point_filt};
//		__property std::vector <t_data_point>      pointNorm     = {read = m_point_norm,     write = m_point_norm};
};

extern CData data_unit;

#endif
