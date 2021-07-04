/*
 *   (c) Yury Kuchura
 *   kuchura@gmail.com
 *
 *   This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net/).
 *
 *   Heavily messed about with by OneOfEleven July 2020
 */

#ifndef LCMatchH
#define LCMatchH

#include <float.h>

//#include <math.h>
#include <Math.hpp>

// L-Network solution structure
typedef struct
{
	float xps;	// Reactance parallel to source (can be NAN if not applicable)
	float xs;	// Serial reactance (can be 0.0 if not applicable)
	float xpl;	// Reactance parallel to load (can be NAN if not applicable)
} t_lc_match;

class LCMatch
{
	public:
		static void __fastcall quadratic_equation(float a, float b, float c, float *result)
		{
			const float d = (b * b) - (4 * a * c);
			if (d < 0)
			{
				result[0] = 0.0f;
				result[1] = 0.0f;
			}
			else
			{
				const float sd = sqrtf(d);
				const float a2 = 2.0f * a;
				result[0] = (-b + sd) / a2;
				result[1] = (-b - sd) / a2;
			}
		}

		// Calculate two solutions for ZL where (R + X * X / R) > R0
		static void __fastcall calc_hi(float R0, float RL, float XL, t_lc_match *result)
		{
			float xs[2];
			float xp[2];

			const float RLS = RL * RL;

			const float a = R0 - RL;
			const float b =  2 * XL * R0;
			const float c = R0 * ((XL * XL) + (RL * RL));
			quadratic_equation(a, b, c, xp);

			// found two impedances parallel to load
			//
			// now calculate serial impedances

			const float RL1 = -XL * xp[0];
			const float XL1 =  RL * xp[0];
			const float XL2 =  XL + xp[0];
			xs[0] = -((RL * XL1) - (RL1 * XL2)) / (RLS + (XL2 * XL2));

			const float RL3 = -XL * xp[1];
			const float XL3 =  RL * xp[1];
			const float XL4 =  XL + xp[1];
			xs[1] = -((RL * XL3) - (RL3 * XL4)) / (RLS + (XL4 * XL4));

			result[0].xs  = xs[0];
			result[0].xps = NaN;
			result[0].xpl = xp[0];

			result[1].xs  = xs[1];
			result[1].xps = NaN;
			result[1].xpl = xp[1];
		}

		// Calculate two solutions for ZL where R < R0
		static void __fastcall calc_lo(float R0, float RL, float XL, t_lc_match *result)
		{
			float xs[2];
			float xp[2];

			// Calculate Xs

			const float a = 1.0f;
			const float b = 2.0f * XL;
			const float c = (RL * RL) + (XL * XL) - (R0 * RL);
			quadratic_equation(a, b, c, xs);

			// got two serial impedances that change ZL to the Y.real = 1/R0
			//
			// now calculate impedances parallel to source

			const float XL1 = XL  + xs[0];
			const float RL3 = RL  * R0;
			const float XL3 = XL1 * R0;
			const float RL5 = RL  - R0;
			xp[0] = ((RL5 * XL3) - (RL3 * XL1)) / ((RL5 * RL5) + (XL1 * XL1));

			const float XL2 = XL  + xs[1];
			const float RL4 = RL  * R0;
			const float XL4 = XL2 * R0;
			const float RL6 = RL  - R0;
			xp[1] = ((RL6 * XL4) - (RL4 * XL2)) / ((RL6 * RL6) + (XL2 * XL2));

			result[0].xs  = xs[0];
			result[0].xps = xp[0];
			result[0].xpl = NaN;

			result[1].xs  = xs[1];
			result[1].xps = xp[1];
			result[1].xpl = NaN;
		}

		static float __fastcall _nonz(float f)
		{
			return (0.0f == f || -0.0f == f) ? 1e-30f : f;
		}

		static float __fastcall dsp_calcVSWR(float RL, float XL, float ref_impedance)
		{
			const float X2 = XL * XL;
			const float R  = (RL > 0.0f) ? RL : 0.0f;
			const float n  = R - ref_impedance;
			const float p  = R + ref_impedance;

			float ro = sqrtf(((n * n) + X2) / _nonz((p * p) + X2));
			if (ro > 0.9999f)
				ro = 0.9999f;

			const float vswr = (1.0f + ro) / (1.0f - ro);

			return vswr;
		}

		static int __fastcall calc(complexf ZL, t_lc_match *result, float ref_impedance)
		{
			const float R0 = ref_impedance;

		//	const complexf ZL = calcImpedance(c, ref_impedance);

			if (ZL.real() <= 0.5f)
				return -1;

			const float vswr = dsp_calcVSWR(ZL.real(), ZL.imag(), ref_impedance);
			if (vswr <= 1.05f)
				return 0;		// low VSWR .. no need for any matching

			const float q_factor = ZL.imag() / ZL.real();
			if (q_factor > 100.0f)
				return -1;		// Q-factor too high

			if (ZL.real() > (R0 / 1.05f) && ZL.real() < (R0 * 1.05f))
			{	// only one solution is enough: just a serial reactance
				// this gives SWR < 1.1 if R is within the range 0.91 .. 1.1 of R0
				result[0].xpl = NaN;
				result[0].xps = NaN;
				result[0].xs = -ZL.imag();
				return 1;
			}

			if (ZL.real() >= R0)
			{	// two Hi-Z solutions
				calc_hi(R0, ZL.real(), ZL.imag(), result);
				return 2;
			}

			// compute Lo-Z solutions
			calc_lo(R0, ZL.real(), ZL.imag(), result);
			if ((ZL.real() + (ZL.imag() * q_factor)) <= R0)
				return 2;

			// two more Hi-Z solutions exist
			calc_hi(R0, ZL.real(), ZL.imag(), &result[2]);
			return 4;
		}

		static void __fastcall x_str(double Hz, float X, char *str)
		{
			if (IsNan(X))
			{
				//strcpy(str, "   ---   ");
				strcpy(str, "         ");
				return;
			}

			if (0.0f == X || -0.0f == X)
			{	// catch divide-by-zero
				strcpy(str, "  0      ");
				return;
			}

			const double phi = 2 * M_PI * Hz;

			if (X < 0.0f)
			{
				const float c = 1.0f / (phi * -X);
				if (fabsf(c) >= 1e0f)   sprintf(str, "%6.2f F ", c * 1e0f);
				else
				if (fabsf(c) >= 1e-3f)  sprintf(str, "%6.2f mF", c * 1e3f);
				else
				if (fabsf(c) >= 1e-6f)  sprintf(str, "%6.2f uF", c * 1e6f);
				else
				if (fabsf(c) >= 1e-9f)  sprintf(str, "%6.2f nF", c * 1e9f);
				else                    sprintf(str, "%6.2f pF", c * 1e12f);
//				if (fabsf(c) >= 1e-12f) sprintf(str, "%6.2f pF", c * 1e12f);
//				else                    sprintf(str, "%6.2f fF", c * 1e15f);
			}
			else
			if (phi != 0)
			{
				const float l = X / phi;
				if (fabsf(l) >= 1e0f)    sprintf(str, "%6.2f H ", l * 1e0f);
				else
				if (fabsf(l) >= 1e-3f)   sprintf(str, "%6.2f mH", l * 1e3f);
				else
				if (fabsf(l) >= 1e-6f)   sprintf(str, "%6.2f uH", l * 1e6f);
				else                     sprintf(str, "%6.2f nH", l * 1e9f);
//				if (fabsf(l) >= 1e-9f)   sprintf(str, "%6.2f nH", l * 1e9f);
//				else                     sprintf(str, "%6.2f pH", l * 1e12f);
			}
			else
			{	// catch divide-by-zero
				strcpy(str, "  0      ");
				return;
			}
		}
};

#endif
