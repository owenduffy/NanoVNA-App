
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef tfftH
#define tfftH

#include "types.h"

//#include <fastmath.h>

enum t_fft_window : int
{
	FFT_WINDOW_NONE = 0,
	FFT_WINDOW_RECTANGLE,
	FFT_WINDOW_TRIANGLE,
	FFT_WINDOW_BARTLETT,
	FFT_WINDOW_HANN,
	FFT_WINDOW_HAMMING,
	FFT_WINDOW_NUTTALL,
	FFT_WINDOW_BLACKMAN,
	FFT_WINDOW_BLACKMAN_NUTTALL,
	FFT_WINDOW_BLACKMAN_HARRIS,
	FFT_WINDOW_GAUSSIAN,
	FFT_WINDOW_WELCH,
	FFT_WINDOW_PARZEN,
	FFT_WINDOW_FLAT_TOP,
	FFT_WINDOW_DOLPH_CHEBYSHEV,
	FFT_WINDOW_KAISER
};

class TFFT
{
private:
	int m_size;

	std::vector <int> m_bit_rev_tab;

	std::vector <float> m_isin_tab;
	std::vector <float> m_sin_tab;
	std::vector <float> m_cos_tab;

	t_fft_window m_window_type;
	float m_window_scale;
	std::vector <float> m_window;

	// returns the highest number closest to v, which is a power of 2
	// assumes 32 bit ints
	int __fastcall next_pow2(int value)
	{
		value--;
		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;
		value++;
		return value;
	}

	double __fastcall acosh(double x)
	{
		if (x == 1)
			return 0;

		if (x < 1)
			x = -1;	// NaN
		else
//		if (x > BIG_CRITERIA)
//			x += x;
//		else
			x += sqrt((x + 1) * (x - 1));
		return log(1e-20 + x);
	}

	float __fastcall bessel0(const float x)
	{
		const float eps = 0.0001f;

		float ret = 0.0f;
		float term = 1.0f;
		int m = 0;	// cathy .. was float

		while (term > (eps * ret))
		{
			ret += term;
			m++;
			term *= (x * x) / (4 * m * m);
		}

		return ret;
	}

	// calculate a chebyshev window of size N, store coeffs in out as in Antoniou
	// -atten is the required sidelobe attenuation (e.g. if you want -60dB atten, use '60')
	void __fastcall dolph_chebyshev_win(std::vector <float> &window, const int N, const float atten)
	{
		window.resize(N);

		// ***********

		double tg = pow(10, atten / 20);		// 1/r term [2], 10^gamma [2]
		double x0 = cosh((1.0 / (N - 1)) * acosh(tg));

		double M = (N - 1) / 2;

		if ((N & 1) == 0)
			M += 0.5;	// handle even length windows

		for (int j = 0; j < (N / 2 + 1); j++)
		{
			double n = j - M;
			double sum = 0;
			for (int i = 1; i <= M; i++)
			{
				double y = cos((2.0 * n * M_PI * i) / N);
				double x = x0 * cos((M_PI * i) / N);
				if (fabs(x) <= 1)
					y *=  cos((N - 1) *  acos(x));
				else
					y *= cosh((N - 1) * acosh(x));
				sum += y;
			}
			window[N - 1 - j] = window[j] = tg + (2 * sum);
		}

		// ***********
		// normalise

		float max = window[0];
		for (int i = 1; i < (N / 2 + 1); i++)
			if (max < window[i])
				max = window[i];

		if (max != 0)
		{
			const float scale = 1.0f / max;
			for (int i = 0; i < N; i++)
				window[i] *= scale;
		}

		// ***********
	}

	#define Izero_EPSILON	1e-21	// Max error acceptable in Izero

	double __fastcall Izero(double x)
	{
		double sum = 1;
		double u = 1;
		int n = 1;
		double halfx = x / 2;
		do {
			const double temp = halfx / n;
			u *= temp * temp;
			sum += u;
			n++;
		} while (u >= (Izero_EPSILON * sum));
		return sum;
	}

	void __fastcall kaiser(std::vector <float> &window, int size, double beta)
	{
		if (beta < 0)
		{
			window.clear();
			return;
		}

		window.resize(size);

		float IBeta = 1.0f / Izero(beta);
		float inm1  = 1.0f / (size - 1);

		for (int i = 0; i < size; i++)
		{
			const float r = 2 * i * inm1 - 1;
			window[i] = Izero(beta * sqrtf(1.0f - r * r)) * IBeta;
		}
	}

public:
	TFFT()
	{
		m_size         = 0;
		m_window_type  = FFT_WINDOW_NONE;
		m_window_scale = 1.0f;
	}

	int __fastcall size() const
	{
		return m_size;
	}

	t_fft_window __fastcall windowType() const
	{
		return m_window_type;
	}

	float __fastcall windowScale() const
	{
		return (1.0f / m_window_scale);
	}

	float __fastcall createWindow(int size, t_fft_window window_type, bool scale_window, std::vector <float> &window, float beta = 3.0f)
	{
		switch (window_type)
		{
			default:
				window_type = FFT_WINDOW_NONE;
			case FFT_WINDOW_NONE:
			case FFT_WINDOW_RECTANGLE:
				window.clear();
				break;

			case FFT_WINDOW_TRIANGLE:
				window.resize(size);
				{
					const float arg = 2.0f / (size - 1);
					for (int i = 0; i < size / 2; i++)
						window[i] = i * arg;
					for (int i = size / 2; i < size; i++)
						window[i] = 2.0f - (i * arg);
				}
				break;

			case FFT_WINDOW_BARTLETT:
				window.resize(size);
				{
					for (int i = 0; i < (size / 2); i++)
						window[i] = i * (2.0f / (size - 1));
					for (int i = (size / 2); i < size; i++)
						window[i] = 2.0f - (i * (2.0f / (size - 1)));
				}
				break;

			case FFT_WINDOW_HANN:
				window.resize(size);
				{
					const float arg = M_PI / size;
					const float alpha = 0.5f;
					for (int i = 0; i < size; i++)
						window[i] = alpha - (1.0f - alpha) * cosf(2.0f * arg * i);
				}
				break;

			case FFT_WINDOW_HAMMING:
				window.resize(size);
				{
					const float arg = M_PI / size;
					const float alpha = 0.54;
					for (int i = 0; i < size; i++)
						window[i] = alpha - (1.0f - alpha) * cosf(2.0f * arg * i);
				}
				break;

			case FFT_WINDOW_NUTTALL:
				window.resize(size);
				{
					const float arg = M_PI / size;
					for (int i = 0; i < size; i++)
						window[i] = 0.355768f - (0.487396f * cosf(2.0f * arg * i)) + (0.144232f * cosf(4.0f * arg * i)) - (0.012604f * cosf(6.0f * arg * i));
				}
				break;

			case FFT_WINDOW_BLACKMAN:
				window.resize(size);
				{
					const float arg = M_PI / size;
					for (int i = 0; i < size; i++)
						window[i] = 0.42f - (0.5f * cosf(2.0f * arg * i)) + (0.08f * cosf(4.0f * arg * i));
				}
				break;

			case FFT_WINDOW_BLACKMAN_NUTTALL:
				window.resize(size);
				{
					const float arg = M_PI / size;
					for (int i = 0; i < size; i++)
						window[i] = 0.35875f - (0.48829f * cosf(2.0f * arg * i)) + (0.14128f * cosf(4.0f * arg * i)) - (0.01168f * cosf(6.0f * arg * i));
				}
				break;

			case FFT_WINDOW_BLACKMAN_HARRIS:
				window.resize(size);
				{
					const float arg = M_PI / size;
					for (int i = 0; i < size; i++)
						window[i] = 0.3635819f - (0.4891775f * cosf(2.0f * arg * i)) + (0.1365995f * cosf(4.0f * arg * i)) - (0.0106411f * cosf(6.0f * arg * i));
				}
				break;

			case FFT_WINDOW_GAUSSIAN:
				window.resize(size);
				{
//					const float alpha = 4.0;
					const float alpha = beta;
					for (int i = 0; i < size; i++)
						window[i] = expf(-alpha / (size * size) * (2 * i - size) * (2 * i - size));
				}
				break;

			case FFT_WINDOW_WELCH:
				window.resize(size);
				{
					//	const float arg = (PI * 2) / (size - 1);
					for (int i = 0; i < size; i++)
						window[i] = 1.0f - ((float)(2 * i - size) / (size + 1)) * ((float)(2 * i - size) / (size + 1));
				}
				break;

			case FFT_WINDOW_PARZEN:
				window.resize(size);
				{
					for (int i = 0; i < size; i++)
						window[i] = 1.0f - fabsf((float)(2 * i - size)) / (size + 1);
				}
				break;

			case FFT_WINDOW_FLAT_TOP:
				window.resize(size);
				{
					const float arg = M_PI / size;
					for (int i = 0; i < size; i++)
						window[i] = 0.2156f - (0.416f * cosf(2.0f * arg * i)) + (0.2781f * cosf(4.0f * arg * i)) - (0.0836f * cosf(6.0f * arg * i)) + (0.0069f * cosf(8.0f * arg * i));
				}
				break;

			case FFT_WINDOW_DOLPH_CHEBYSHEV:
//				dolph_chebyshev_win(window, size, 106);	// -106dB attenuation
				dolph_chebyshev_win(window, size, 40);	// -80dB attenuation
				break;

			case FFT_WINDOW_KAISER:
				window.resize(size);
				if (beta <= 0)
				{
					for (int i = 0; i < size; i++)
						window[i] = 1.0f;
				}
				else
				{
					for (int i = 0; i < size; i++)
					{
						const float r = ((float)(2 * i) / (size - 1)) - 1.0f;
						window[i] = bessel0(beta * sqrtf(1.0f - (r * r))) / bessel0(beta);
					}
				}
				break;
		}

		float window_scale = 1.0f;

		if (!window.empty())
		{
			window_scale = 0.0f;
			for (unsigned int i = 0; i < window.size(); i++)
				window_scale += window[i];
			window_scale /= m_size;

			if (scale_window)
			{	// scale the window so that we get a normalised gain of one
				const float scale = 1.0f / window_scale;
				for (unsigned int i = 0; i < window.size(); i++)
					window[i] *= scale;
			}
		}

		return window_scale;
	}

	void __fastcall init(int size, t_fft_window window_type, bool scale_window)
	{
		m_size = 0;
		m_bit_rev_tab.clear();
		m_isin_tab.clear();
		m_sin_tab.clear();
		m_cos_tab.clear();
		m_window.clear();

		if (size <= 0)
			return;

		// make sure size is a power of 2
		m_size = next_pow2(size);

		// create the sine and cosine lookup tables
		m_isin_tab.resize(m_size / 2);
		m_sin_tab.resize(m_size / 2);
		m_cos_tab.resize(m_size / 2);
		for (int i = 0; i < m_size / 2; i++)
		{
			const float phi = (M_PI * i) / (m_size / 2);
			const float s   = sinf(phi);
			const float c   = cosf(phi);
			m_isin_tab[i] =  s;
			m_sin_tab[i]  = -s;
			m_cos_tab[i]  =  c;
		}

		// set up bit reverse mapping
		m_bit_rev_tab.resize(m_size);
		int rev = 0;
		int half_points = m_size / 2;
		for (int i = 0; i < m_size - 1; i++)
		{
			m_bit_rev_tab[i] = rev;
			int mask = half_points;
			// add 1 backwards
			while (rev >= mask)
			{
				rev -= mask; // turn off this bit
				mask >>= 1;
			}
			rev += mask;
		}
		m_bit_rev_tab[m_size - 1] = m_size - 1;

		// *************************
		// create the window table

		m_window_type = window_type;

		if (m_size < 2)
		{
			m_window.clear();
			return;
		}

		// https://en.wikipedia.org/wiki/Window_function

		m_window_scale = createWindow(m_size, window_type, scale_window, m_window);

		// *************************
	}

	void __fastcall transform(complexf *in, complexf *out, bool inv = false)
	{
		if (!in || !out || m_size <= 0)
			return;

		if (m_window_type != FFT_WINDOW_NONE && (int)m_window.size() == m_size)
		{	// window the input data
			for (int i = 0; i < m_size; i++)
				out[i] = in[i] * m_window[i];
		}
		else
		if (out != in)
			memcpy(out, in, sizeof(out[0]) * m_size);

		// bit reversed addressing .. inplace method
		for (int i = 0; i < m_size; i++)
		{
			const int j = m_bit_rev_tab[i];
			if (j > i)
			{	// swap
				const complexf temp = out[i];
				out[i] = out[j];
				out[j] = temp;
			}
		}

		std::vector <float> &cos_tab = m_cos_tab;
		std::vector <float> &sin_tab = (inv) ? m_isin_tab : m_sin_tab;

		for (int a = 1, phi_step = m_size >> 1; a < m_size; a <<= 1, phi_step >>= 1)
		{
			const int step = a << 1;

			complexf *p0 = out + 0;
			complexf *p1 = out + a;

			// 1st iteration requires no multiplication due to sin=0 and cos=1
			complexf *p2 = p0++;
			complexf *p3 = p1++;
			for (int m = 0; m < m_size; m += step)
			{
				const complexf cpx = *p3;
				*p3 = *p2 - cpx;
				*p2 += cpx;
				p2 += step;
				p3 += step;
			}

			// do the rest of the interations
			for (int j = 1, phi = phi_step; j < a; j++, phi += phi_step)
			{
				p2 = p0++;
				p3 = p1++;
				complexf cs = complexf (cos_tab[phi], sin_tab[phi]);
				for (int m = j; m < m_size; m += step)
				{
					complexf cpx = *p3;
					complexf tmp( (cs.real() * cpx.real()) + (cs.imag() * cpx.imag()), (cs.real() * cpx.imag()) - (cs.imag() * cpx.real()) );
					*p3 = *p2 - tmp;
					*p2 += tmp;
					p2 += step;
					p3 += step;
				}
			}
		}
	}

	void __fastcall correctBinOrder(complexf *bins)
	{
		if (bins == NULL || m_size <= 0)
			return;

		for (int i = 0, k = m_size / 2; i < m_size / 2; i++, k++)
		{	// swap a pair of bins
			const complexf temp = bins[i];
			bins[i] = bins[k];
			bins[k] = temp;
		}
	}

};

#endif

