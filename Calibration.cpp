
#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <math.h>

#include "Calibration.h"
#include "common.h"
#include "DataUnit.h"
#include "Settings.h"
#include "spline.h"

CCalibration calibration_module;

CCalibration::CCalibration()
{
	m_ideal_short     = complexf(-1, 0);
	m_ideal_open      = complexf( 1, 0);
	m_ideal_load      = complexf( 0, 0);
	m_ideal_isolation = complexf( 0, 0);
	m_ideal_through   = complexf( 1, 0);

	m_use_ideal_short = true;
	m_short_l0     =    5.7 * 10e-12;
	m_short_l1     =  -8960 * 10e-24;
	m_short_l2     =  -1100 * 10e-33;
	m_short_l3     = -41200 * 10e-42;
	m_short_length = -34.2;	// Pico second frequencies ds
	// These numbers look very large, considering what Keysight suggests their numbers are.

	m_use_ideal_open = true;
	// Subtract 50fF for the nanoVNA calibration if nanoVNA is calibrated?
	m_open_c0     =  2.1  * 10e-14;
	m_open_c1     =  5.67 * 10e-23;
	m_open_c2     = -2.39 * 10e-31;
	m_open_c3     =  2    * 10e-40;
	m_open_length = 0;

	m_use_ideal_load = true;
	m_load_r      = 25;
	m_load_l      = 0;
	m_load_c      = 0;
	m_load_length = 0;

	m_use_ideal_through = true;
	m_through_length = 0;
}

void __fastcall CCalibration::computeErrorTerms(t_calibration &calibration)
{
	m_inter_cal.resize(0);

	bool OK = true;

	const double norm = 50.0;

	for (unsigned int i = 0; i < calibration.point.size(); i++)
	{
		const double freq = (double)calibration.point[i].HzCal;

		// short
		complexd gshort = m_ideal_short;
		if (!m_use_ideal_short)
		{
			const complexd Zsp = complexd(0, 1) * ((2 * M_PI * freq) * (m_short_l0 + m_short_l1 * freq + m_short_l2 * (freq * freq) + m_short_l3 * (freq * freq * freq)));
			const double k = -2 * M_PI * 2 * freq * m_short_length;
			const complexd n = Zsp / norm;
			gshort = (n - 1.0) / (n + 1.0) * exp(complexd(0, 1) * k);
		}

		// open
		complexd gopen = m_ideal_open;
		if (!m_use_ideal_open)
		{
			const double divisor = ((2 * M_PI * freq) * (m_open_c0 + m_open_c1 * freq + m_open_c2 * (freq * freq) + m_open_c3 * (freq * freq * freq)));
			if (divisor != 0.0f)
			{
				complexd Zop = complexd(0, -1) / divisor;
				const double k = -2.0 * M_PI * 2.0 * freq * m_open_length;
				const complexd n = Zop / norm;
				gopen = (n - 1.0) / (n + 1.0) * exp(complexd(0, 1) * k);
			}
		}

		// load
		complexd gload = m_ideal_load;
		if (!m_use_ideal_load)
		{
			complexd Zl = (complexd(0, 1) * (2.0 * M_PI * freq) * m_load_l) + m_load_r;
			const double k = -2.0 * M_PI * 2.0 * freq * m_load_length;
			const complexd n = Zl / norm;
			gload = (n - 1.0) / (n + 1.0) * exp(complexd(0, 1) * k);
		}

		// isolation
		complexd gisoln = calibration.point[i].isolationCal;

		// through
		complexd gthru = m_ideal_through;
		if (!m_use_ideal_through)
		{
			const double k = -2.0 * M_PI * freq * m_through_length;
			gthru = exp(complexd(0, 1) * k);
		}

		complexd gmShort = calibration.point[i].shortCal;
		complexd gmOpen  = calibration.point[i].openCal;
		complexd gmLoad  = calibration.point[i].loadCal;

		complexd denominator =	(gshort * (gopen - gload) * gmShort) +
										(gopen * gload * gmOpen) -
										(gopen * gload * gmLoad) -
										((gopen * gmOpen) - (gload * gmLoad)) * gshort;

		complexd e00;
		complexd e11;
		complexd delta_e;
		complexd e30 = gisoln;
		complexd e10e32;

		try
		{
			e00     = -((((gopen * gmLoad) - (gload * gmLoad)) * gshort * gmOpen) - ((gopen * gload * gmOpen) - (gopen * gload * gmLoad) - ((gload * gmOpen) - (gopen * gmLoad)) * gshort) * gmShort) / denominator;
			e11     = (((gopen - gload) * gmShort) - (gshort * (gmOpen - gmLoad)) + (gload * gmOpen) - (gopen * gmLoad)) / denominator;
			delta_e = -((((gshort * (gmOpen - gmLoad)) - (gopen * gmOpen) + (gload * gmLoad)) * gmShort) + ((gopen * gmLoad) - (gload * gmLoad)) * gmOpen) / denominator;
		}
		catch (Exception &exception)
		{	// divide by zero error
			//Application->ShowException(&exception);
			//String s = exception.ToString();
			OK = false;
			break;
		}

		const complexd throughCal = calibration.point[i].throughCal;
		try
		{
			e10e32 = ((throughCal / gthru) - e30) * (complexd(1, 0) - (e11 * e11));
//			e10e32 = ((throughCal / gthru) - e30);	// this one drops the load calibration from S21 but produces flat through cable calibrations

			// inverse to let allow live corrections use faster multiply instead of slower divide
			e10e32 = complexd(1, 0) / e10e32;
		}
		catch (Exception &exception)
		{	// divide by zero error
			//Application->ShowException(&exception);
			//String s = exception.ToString();
			OK = false;
			break;
		}

		calibration.point[i].e00     = e00;
		calibration.point[i].e11     = e11;
		calibration.point[i].delta_e = delta_e;
		calibration.point[i].e30     = e30;
		calibration.point[i].e10e32  = e10e32;
	}

	if (!OK)
	{	// clear
//		calibration.point.resize(0);
		for (unsigned int i = 0; i < calibration.point.size(); i++)
		{
			calibration.point[i].e00     = complexf( 0, 0);
			calibration.point[i].e11     = complexf( 0, 0);
			calibration.point[i].delta_e = complexf(-1, 0);
			calibration.point[i].e30     = complexf( 0, 0);
			calibration.point[i].e10e32  = complexf( 1, 0);
		}
	}
}

void __fastcall CCalibration::interpolateErrorTerms(t_calibration &calibration, std::vector <t_data_point> &points, const bool linear)
{
	m_inter_cal.resize(0);

	const int c_size = calibration.point.size();
	const int p_size = points.size();
	if (c_size < 2 || p_size < 2)
		return;

	CRSpline2 spline[5];

	if (!linear)
	{	// going to do spline interpolation

		// add the spline control points for later computing the interpolated points
		for (int m = 0; m < 5; m++)
		{
			for (int i = 0; i < c_size; i++)
			{
				const complexf cpx = calibration.point[i].errorTerm[m];
				spline[m].addPoint(cpx.real(), cpx.imag());
			}
		}
	}

	const int64_t Hz_min   = calibration.point[0].HzCal;
	const int64_t Hz_max   = calibration.point[c_size - 1].HzCal;
	const int64_t Hz_range = ABS(Hz_max - Hz_min);

	if (Hz_min <= 0 || Hz_max <= 0 || Hz_range <= 0)
		return;

	// interpolated calibration data array
	m_inter_cal.resize(p_size);

	for (int i = 0; i < p_size; i++)
	{
		int64_t Hz = points[i].Hz;
		if (Hz <= 0)
			Hz = Hz_min + I64ROUND(((double)Hz_range * i) / (p_size - 1));

		const double ip = (double)((Hz - Hz_min) * (c_size - 1)) / Hz_range;	// 0.0 at Hz_min, (size - 1) at Hz_max
		int k = (int)floor(ip);
		if (k < 0) k = 0;
		else
		if (k > (c_size - 1)) k = c_size - 1;

		complexf error_term[5];

		t_calibration_point cp0 = calibration.point[k + 0];

		if (Hz <= Hz_min || Hz >= Hz_max)
		{	// can't interpolate below or above the range of data we have
			for (int m = 0; m < 5; m++)
				error_term[m] = cp0.errorTerm[m];
		}
		else
		{	// interpolate between cp0 and cp1
			t_calibration_point cp1 = calibration.point[k + 1];
			if (linear)
			{	// linear
				const float frac_ip = (float)(ip - floor(ip));	// 0.0 to 1.0 from point to point
				for (int m = 0; m < 5; m++)
					error_term[m] = cp0.errorTerm[m] + ((cp1.errorTerm[m] - cp0.errorTerm[m]) * frac_ip);
			}
			else
			{	// spline
				const double t = ip / (c_size - 1);	// 0.0 to 1.0
				for (int m = 0; m < 5; m++)
				{
					const vec2 v = spline[m].interpolatedPoint(t);
					error_term[m] = complexf(v.x, v.y);
				}
			}
		}

		// save the interpolated error terms
		m_inter_cal[i].HzCal = Hz;
		for (int m = 0; m < 5; m++)
			m_inter_cal[i].errorTerm[m] = error_term[m];
	}
}

void __fastcall CCalibration::correct(std::vector <t_data_point> &points)
{
	const int size = points.size();
	if (size < 2)
		return;

	if ((int)m_inter_cal.size() != size)
	{
		m_inter_cal.resize(0);	// different size
	}
	else
	{
		if (m_inter_cal[0].HzCal != points[0].Hz || m_inter_cal[size - 1].HzCal != points[size - 1].Hz)
		{
			bool same_freqs = true;
			for (int i = 0; i < size; i++)
			{
				const int64_t Hz1 = points[i].Hz;
				if (points[i].Hz > 0)
				{
					const int64_t Hz2 = m_inter_cal[i].HzCal;
					if (Hz1 != Hz2)
					{
						same_freqs = false;
						break;
					}
				}
			}
			if (!same_freqs)
				m_inter_cal.resize(0);	// different frequency range
		}
	}

	if (m_inter_cal.empty())	// create interpolated calibration data .. this is only done the once when the number of points and/or frequency range changes
	{
		computeErrorTerms(m_calibration);
		interpolateErrorTerms(m_calibration, points, true);	// linear interpolation
//		interpolateErrorTerms(m_calibration, points, false);	// spline interpolation
	}

	if ((int)m_inter_cal.size() == size)
	{
		for (int i = 0; i < size; i++)
		{
			complexf s11 = points[i].s11;
			complexf s21 = points[i].s21;
			s11 = (s11 - m_inter_cal[i].e00) / ((s11 * m_inter_cal[i].e11) - m_inter_cal[i].delta_e);
			s21 -= m_inter_cal[i].e30;
//			s21 /= e10e32;
			s21 *= m_inter_cal[i].e10e32;	// multiply is faster - we inversed 'e10e32' before using it here
			points[i].s11 = s11;
			points[i].s21 = s21;
		}
	}
}

