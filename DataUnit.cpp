
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

//#include <fastmath.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include "DataUnit.h"
#include "common.h"
#include "settings.h"
#include "Calibration.h"

#ifdef __BORLANDC__
	#pragma package(smart_init)
#endif

CData data_unit;

/*
// where we store the all the past sweep results (64 of them) .. used for time averaging
std::vector <t_data_point_hist> m_point;
// incoming s-points
std::vector <t_data_point>   m_point_incoming;
// SnP memories .. mem[0] is the live memory
std::vector <t_data_point>   m_point_mem[MAX_MEMORIES];
// filtered SnP memories
std::vector <t_data_point>   m_point_filt[MAX_MEMORIES];
// normalise memory
std::vector <t_data_point>   m_point_norm;
*/

CData::CData()
{
	// load the BEEP sound
	common.fetchResource("BEEP_WAV", beep_wav);

	// load the pherp sound
//	common.fetchResource("PHURP_WAV", phurp_wav);

//	common.fetchResource("STEREOPHONIC_WAV", stereophonic_wav);

//	common.fetchResource("SQUEAK_WAV", squeak_wav);

	for (int g = 0; g < MAX_GRAPHS; g++)
	{
		for (int m = 0; m < MAX_MEMORIES; m++)
		{
			m_fft_peak_index[g][m] = -1;
			m_fft_peak_mag[g][m]   = 0.0f;
		}
	}

	m_points             = 101;
	m_points_per_segment = 101;
	m_segments           = 1;
	m_segment            = 0;

	m_freq_start_Hz  = 0;
	m_freq_stop_Hz   = 0;
	m_freq_center_Hz = 0;
	m_freq_span_Hz   = 0;
	m_freq_cw_Hz     = 0;

	m_total_frames   = 0;

	m_history_index  = 0;
	m_history_frames = 0;

	m_velocity_factor     = 0;
	m_max_distance_meters = 0;

	resetUnitData();

	// running out of memory here
	const int reserve_size = 32768;
	try
	{
		m_point_incoming.reserve(reserve_size);
		m_point.reserve(reserve_size);
		m_point_norm.reserve(reserve_size);
		for (int m = 0; m < MAX_MEMORIES; m++)
			m_point_filt[m].reserve(reserve_size);
	}
	catch (Exception &exception)
	{
		//Application->ShowException(&exception);
		String s = exception.ToString();

		Application->NormalizeTopMosts();
		Application->MessageBox(String("Out of point memory in data_unit ..\n" + s).w_str(), L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();

//		m_point_incoming.reserve(32768);
//		m_point.reserve(32768);
	}
}

void __fastcall CData::resetUnitData()
{
	m_vna_data.name                = "NanoVNA";

	m_vna_data.info.resize(0);
	m_vna_data.help                = "";
	m_vna_data.version             = "";

	m_vna_data.dislord             = false;
	m_vna_data.oneofeleven         = false;

	m_vna_data.cmd_capture         = false;
	m_vna_data.cmd_vbat            = false;
	m_vna_data.cmd_vbat_offset     = false;
   m_vna_data.cmd_marker          = false;
	m_vna_data.cmd_bandwidth       = false;
	m_vna_data.cmd_integrator      = false;
	m_vna_data.cmd_scan_bin        = false;
	m_vna_data.cmd_scanraw         = false;
	m_vna_data.cmd_sd_list         = false;
	m_vna_data.cmd_sd_readfile     = false;
	m_vna_data.cmd_time            = false;
	m_vna_data.cmd_threshold       = false;
	m_vna_data.cmd_pause           = false;
	m_vna_data.cmd_resume          = false;
	m_vna_data.cmd_reset           = false;
	m_vna_data.cmd_cal             = false;
	m_vna_data.cmd_power           = false;
	m_vna_data.cmd_usart           = false;
	m_vna_data.cmd_usart_cfg       = false;
	m_vna_data.cmd_deviceid        = false;
	m_vna_data.cmd_sweep           = false;
	m_vna_data.cmd_mode            = false;
	m_vna_data.cmd_edelay          = false;

   m_vna_data.type                = UNIT_TYPE_NONE;
	m_vna_data.lcd_width           = 320;
	m_vna_data.lcd_height          = 240;
	m_vna_data.max_bandwidth_Hz    = 2000;
	m_vna_data.bandwidth           = 0;
	m_vna_data.bandwidth_Hz        = 2000;
	m_vna_data.max_points          = DEFAULT_MAX_POINTS;
	m_vna_data.if_Hz               = 0;
	m_vna_data.adc_Hz              = 0;
	m_vna_data.audio_samples_count = 0;
	m_vna_data.vbat_mv             = 0;
	m_vna_data.vbat_offset_mv      = 0;
	m_vna_data.power               = -1;
	m_vna_data.usart_speed         = 0;
	m_vna_data.deviceid            = -1;
	m_vna_data.cal                 = false;
	m_vna_data.edelay              = 0.0f;

	m_vna_data.num_points          = 0;

	m_vna_data.freq_max_Hz         = 0;
	m_vna_data.freq_min_Hz         = 0;

	m_vna_data.freq_threshold_Hz   = 0;
	m_vna_data.freq_start_Hz       = 0;
	m_vna_data.freq_stop_Hz        = 0;
	m_vna_data.freq_center_Hz      = 0;
	m_vna_data.freq_span_Hz        = 0;
	m_vna_data.freq_cw_Hz          = 0;
	m_vna_data.freq_Hz             = 0;

	// NanoVNA V2 specific
	m_vna_data.protool_version     = 0;
	m_vna_data.hardware_revision   = 0;
	m_vna_data.firmware_major      = 0;
	m_vna_data.firmware_minor      = 0;
}

double __fastcall CData::freq_step(const int mem)
{
	if (mem < 0)
	{
		return (m_points > 1) ? (double)(m_freq_stop_Hz - m_freq_start_Hz) / (m_points - 1) : 0;
	}
	else
	{
		const int size = freqArraySize(mem);
		return (size > 1) ? (double)(m_point_mem[mem][size - 1].Hz - m_point_mem[mem][0].Hz) / (size - 1) : 0;
	}
}

double __fastcall CData::max_time(const double freq_step)
{
	return (freq_step <= 0) ? 0 : 0.5 / freq_step;
}

double __fastcall CData::max_dist(const double freq_step, const double velocity_factor)
{
	return (freq_step <= 0) ? 0 : (0.25 * velocity_factor * SPEED_OF_LIGHT) / freq_step;
}

float __fastcall CData::power(complexf c)
{
	return SQR(c.real()) + SQR(c.imag());
}

float __fastcall CData::magnitude(complexf c)
{
	const float p = power(c);
	return sqrtf(p);
}

float __fastcall CData::gain10(complexf c)
{
	const float pwr = power(c);
	return (pwr > 0) ? 10.0f * log10f(pwr) : 0.0f;
}

float __fastcall CData::gain20(complexf z)
{
	const float pwr = power(z);
	return (pwr > 0) ? 20.0f * log10f(pwr) : 0.0f;
}

float __fastcall CData::phase(complexf c)
{
	return (c.real() != 0) ? atan2f(c.imag(), c.real()) : 0.0f;
}

float __fastcall CData::VSWR(complexf c)
{
	const float mag = magnitude(c);
	return (mag < 1) ? (1.0f + mag) / (1.0f - mag) : VSWR_MAX;
}

complexf __fastcall CData::parallelToSerial(complexf c)
{	// Convert parallel impedance to serial impedance equivalent
	const float p = power(c);
	if (p <= 0.0f)
	{
		return complexf(IMPEDANCE_MAX, IMPEDANCE_MAX);
	}
	else
	{
		const float re = (c.imag() * c.imag() * c.real()) / p;
		const float im = (c.real() * c.real() * c.imag()) / p;
		return complexf (re, im);
	}
}

complexf __fastcall CData::serialToParallel(complexf z)
{	// Convert serial impedance to parallel impedance equivalent
	const float pwr = power(z);
	const float re = (z.real() != 0.0f) ? pwr / z.real() : IMPEDANCE_MAX;
	const float im = (z.imag() != 0.0f) ? pwr / z.imag() : IMPEDANCE_MAX;
	return complexf(re, im);
}

float __fastcall CData::impedanceToCapacitance(complexf z, const double freq)
{	// Calculate capacitive equivalent for reactance
	if (freq <= 0)
		return 0.0f;
	return (z.imag() == 0) ? CAP_MAX : -(1.0f / ((float)(2 * M_PI * freq) * z.imag()));
}

float __fastcall CData::impedanceToInductance(complexf z, double freq)
{	// Calculate inductive equivalent for reactance
	return (freq <= 0) ? 0.0f : z.imag() / (float)(2 * M_PI * freq);
}

complexf __fastcall CData::impedanceToNorm(complexf z, const float ref_impedance)
{	// Calculate normalized z from impedance
	return z / ref_impedance;
}

complexf __fastcall CData::normToImpedance(complexf z, const float ref_impedance)
{	// Calculate impedance from normalized z
	return z * ref_impedance;
}

complexf __fastcall CData::reflectionCoefficient(complexf z, const float ref_impedance)
{	// Calculate reflection coefficient for z
	return (z - ref_impedance) / (z + ref_impedance);
}

complexf __fastcall CData::gammaToImpedance(complexf gamma, const float ref_impedance)
{
	return ((-gamma - 1.0f) / (gamma - 1.0f)) * ref_impedance;
}

complexf __fastcall CData::impedance(complexf c, const float ref_impedance)
{
	//	return gammaToImpedance(c, ref_impedance);
	//	return complexf((1.0f + c.real) * (1.0f - c.real) - (c.imag() * c.imag), 2.0f * c.imag) * ref_impedance;

	const float div = ((1.0f - c.real()) * (1.0f - c.real()) + (c.imag() * c.imag()));
	if (div == 0.0f)
	{
		return complexf(0);
	}
	else
	{
		const float d = ref_impedance / div;
		return complexf((((1.0f + c.real()) * (1.0f - c.real())) - (c.imag() * c.imag())), 2.0f * c.imag()) * d;
	}
}

float __fastcall CData::qualityFactor(complexf c, const float ref_impedance)
{
	complexf imp = impedance(c, ref_impedance);
//	complexf imp((1.0f + c.real) * (1.0f - c.real) - (c.imag() * c.imag), 2.0f * c.imag);
	return (imp.real() != 0.0f) ? fabsf(imp.imag() / imp.real()) : 0.0f;
}

float __fastcall CData::capacitiveEquivalent(complexf c, const double freq, const float ref_impedance)
{
	complexf imp = impedance(c, ref_impedance);
	return impedanceToCapacitance(imp, freq);
}

float __fastcall CData::inductiveEquivalent(complexf c, const double freq, const float ref_impedance)
{
	complexf imp = impedance(c, ref_impedance);
	return impedanceToInductance(imp, freq);
}

bool __fastcall CData::validFrequencySettings()
{
	int64_t max_Hz;
	int64_t min_Hz;
	minMaxFreqHz(min_Hz, max_Hz);

	if (m_freq_start_Hz < min_Hz || m_freq_stop_Hz < min_Hz)
		return false;

	if (m_freq_start_Hz > max_Hz || m_freq_stop_Hz > max_Hz)
		return false;

	if (m_freq_stop_Hz < m_freq_start_Hz)
		return false;

	if (m_freq_center_Hz != ((m_freq_start_Hz + m_freq_stop_Hz) / 2))
		return false;

//	if (m_freq_span_Hz != (m_freq_stop_Hz - m_freq_start_Hz))
//		return false;

	return true;
}

void __fastcall CData::minMaxFreqHz(int64_t &min_Hz, int64_t &max_Hz)
{
	max_Hz = 19e9;//MAX_VNA_JANVNAV2_FREQ_HZ;
	min_Hz = MIN_VNA_JANVNAV2_FREQ_HZ;

	if (m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
		max_Hz = MAX_VNA_JANVNAV2_FREQ_HZ;
		min_Hz = MIN_VNA_JANVNAV2_FREQ_HZ;
	}
	else
	if (m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{
		max_Hz = MAX_VNA_V2_FREQ_HZ*3;
		min_Hz = MIN_VNA_V2_FREQ_HZ;
	}
	else
	if (m_vna_data.type != UNIT_TYPE_NANOVNA_V2 && m_vna_data.type != UNIT_TYPE_NONE)
	{
		max_Hz = MAX_VNA_V1_FREQ_HZ;
		min_Hz = MIN_VNA_V1_FREQ_HZ;
	}
	else
	if (m_vna_data.type == UNIT_TYPE_TINYSA)
	{
		max_Hz = MAX_TINYSA_FREQ_HZ;
		min_Hz = MIN_TINYSA_FREQ_HZ;
	}
}

int __fastcall CData::freqArraySize(const int mem)
{
	int size = 0;

	if (mem < 0)
	{
		if (!m_point.empty())
		{
			size = m_point.size();
			while (size > 0 && m_point[size - 1].Hz <= 0)
				size--;
		}
	}
	else
	if (mem >= 0 && mem < MAX_MEMORIES)
	{
		if (!m_point_mem[mem].empty())
		{
			size = m_point_mem[mem].size();
			while (size > 0 && m_point_mem[mem][size - 1].Hz <= 0)
				size--;
		}
	}

	return size;
}

int __fastcall CData::indexFreq(const int64_t freq, const int mem)
{
	// mem <  -1 = calibrations
	// mem >= -1 = memories

	const int size = (mem >= -1) ? freqArraySize(mem) : calibration_module.m_calibration.point.size();

	int index = -1;

	if (size <= 0 || freq < m_freq_start_Hz || freq > m_freq_stop_Hz)
		return index;

	// TODO: make this many times faster

	if (mem <= -2)
	{	// calibrations

		// find the nearest index

		const int64_t min_Hz = calibration_module.m_calibration.point[0].HzCal;
		const int64_t max_Hz = calibration_module.m_calibration.point[size - 1].HzCal;

		if (freq < min_Hz || freq > max_Hz)
			return index;

		// slow version
		int64_t Hz_diff = m_freq_span_Hz;
		for (int i = 0; i < size; i++)
		{
			const int64_t Hz   = calibration_module.m_calibration.point[i].HzCal;
			const int64_t diff = ABS(freq - Hz);
			if (Hz_diff > diff)
			{
				Hz_diff = diff;
				index   = i;
			}
		}
	}
	else
	if (mem == -1)
	{
		// find the nearest index

		const int64_t min_Hz = m_point[0].Hz;
		const int64_t max_Hz = m_point[size - 1].Hz;

		if (freq < min_Hz || freq > max_Hz)
			return index;

		// slow version
		int64_t Hz_diff = m_freq_span_Hz;
		for (int i = 0; i < size; i++)
		{
			const int64_t Hz   = m_point[i].Hz;
			const int64_t diff = ABS(freq - Hz);
			if (Hz_diff > diff)
			{
				Hz_diff = diff;
				index   = i;
			}
		}

		// very fast version
	}
	else
	if (mem < MAX_MEMORIES)
	{
		// find the nearest index

		const int64_t min_Hz = m_point_mem[mem][0].Hz;
		const int64_t max_Hz = m_point_mem[mem][size - 1].Hz;

		if (freq < min_Hz || freq > max_Hz)
			return index;

		// slow version
		int64_t Hz_diff = m_freq_span_Hz;
		for (int i = 0; i < size; i++)
		{
			const int64_t Hz   = m_point_mem[mem][i].Hz;
			const int64_t diff = ABS(freq - Hz);
			if (Hz_diff > diff)
			{
				Hz_diff = diff;
				index   = i;
			}
		}
	}

	return index;
}

int __fastcall CData::firstUsedMem(const bool only_enabled, int mem)
{
	// find the first memory that contains data

	if (mem < 0)
		mem = 0;

	while (mem < MAX_MEMORIES)
	{
		const int size = freqArraySize(mem);
		if (size > 0)
		{	// the memory contains data
			if (!only_enabled)
				break;	// the memory doesn't have to be enabled
			if (only_enabled && settings.memoryEnable[mem])
				break;	// the memory does have to be enabled and is enabled
		}
		mem++;
	}

	if (mem >= MAX_MEMORIES)
		mem = -1;	// no memory found with data

	return mem;
}

int64_t __fastcall CData::getFrequency(const int mem, const int index)
{
	int64_t Hz = -1;

	if (mem < 0)
	{
		if (index < 0 || index >= (int)m_point.size())
			return Hz;

		const int size = freqArraySize(mem);
		if (index >= size)
			return Hz;

		Hz = m_point[index].Hz;
	}
	else
	{
		if (mem < 0 || mem >= MAX_MEMORIES)
			return Hz;

		if (index < 0 || index >= (int)m_point_mem[mem].size())
			return Hz;

		const int size = freqArraySize(mem);
		if (index >= size)
			return Hz;

		Hz = m_point_mem[mem][index].Hz;
	}

	return Hz;
}

