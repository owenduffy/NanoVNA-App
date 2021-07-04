
#ifndef complexH
#define complexH

#include <complex>	// comment this out to use our own faster complex class

#include <float.h>	// FLT_MIN
#include <cmath>
#include <math.h>

namespace nonstd
{	// own complex class - much more efficient (faster) than std:complex

	//template <typename T>
	//	class complex;
	//template <> class complex <float>;

	//template <>
	//class complex <float>
	template <typename T>
	class complex
	{
	private:
		T m_re;
		T m_im;

	public:
		complex() : m_re(0), m_im(0) {}
		complex(const T real) : m_re(real), m_im(0) {}
		complex(const T real, const T imag) : m_re(real), m_im(imag) {}
		template <typename X>
		complex(const complex <X> & c) : m_re((T)c.re), m_im((T)c.im) {}

		__inline T __fastcall real() const
		{
			return m_re;
		}
		__inline T __fastcall imag() const
		{
			return m_im;
		}

		template <typename X>
		__inline T __fastcall real(const complex <X> &c) const
		{
			return (T)c.re;
		}
		template <typename X>
		__inline T __fastcall imag(const complex <X> &c) const
		{
			return (T)c.im;
		}

		__property T re = {read = m_re, write = m_re};
		__property T im = {read = m_im, write = m_im};

		template <typename X>
		__inline complex <T> operator = (const X &rhs)
		{
			m_re = (T)rhs;
			m_im = (T)0;
			return *this;
		}

		template <typename X>
		__inline complex <T> operator = (const complex <X> &rhs)
		{
			m_re = (T)rhs.m_re;
			m_im = (T)rhs.m_im;
			return *this;
		}

		template <typename X>
		__inline complex <T> operator + (const X &rhs)
		{
			return complex <T> (m_re + (T)rhs, m_im);
		}

		template <typename X>
		__inline complex <T> operator + (const complex <X> &rhs)
		{
			return complex <T> (m_re + (T)rhs.m_re, m_im + (T)rhs.m_im);
		}

		template <typename X>
		__inline complex <T> operator += (const X &rhs)
		{
			m_re += (T)rhs;
			return *this;
		}

		template <typename X>
		__inline complex <T> operator += (const complex <X> &rhs)
		{
			m_re += (T)rhs.m_re;
			m_im += (T)rhs.m_im;
			return *this;
		}

		template <typename X>
		__inline complex <T> operator - (const X &rhs)
		{
			return complex <T> (m_re - (T)rhs, m_im);
		}

		template <typename X>
		__inline complex <T> operator - (const complex <X> &rhs)
		{
			return complex <T> (m_re - (T)rhs.m_re, m_im - (T)rhs.m_im);
		}

		template <typename X>
		__inline complex <T> operator -= (const X &rhs)
		{
			m_re -= (T)rhs;
			return *this;
		}

		template <typename X>
		__inline complex <T> operator -= (const complex <X> &rhs)
		{
			m_re -= (T)rhs.m_re;
			m_im -= (T)rhs.m_im;
			return *this;
		}

		template <typename X>
		__inline complex <T> operator * (const X &rhs)
		{
			return complex <T> (m_re * (T)rhs, m_im * (T)rhs);
		}

		template <typename X>
		__inline complex <T> operator * (const complex <X> &rhs)
		{
			const T re = (m_re * (T)rhs.m_re) - (m_im * (T)rhs.m_im);
			const T im = (m_im * (T)rhs.m_re) + (m_re * (T)rhs.m_im);
			return complex <T> (re, im);
		}

		template <typename X>
		__inline complex <T> operator *= (const X &rhs)
		{
			m_re *= (T)rhs;
			m_im *= (T)rhs;
			return *this;
		}

		template <typename X>
		__inline complex <T> & operator *= (const complex <X> &rhs)
		{
			const T re = (m_re * (T)rhs.m_re) - (m_im * (T)rhs.m_im);
			const T im = (m_im * (T)rhs.m_re) + (m_re * (T)rhs.m_im);
			m_re = re;
			m_im = im;
			return *this;
		}

		template <typename X>
		__inline complex <T> operator / (const X & rhs)
		{
         //T denom = rhs * rhs;
         //T re    = (m_re * rhs) / denom;
         //T im    = (m_im * rhs) / denom;

         T re;
         T im;
			if (rhs == (T)0)
			{
				//re = m_re / (T)FLT_MIN;
				//im = m_im / (T)FLT_MIN;
				re = _FPCLASS_QNAN;
				im = _FPCLASS_QNAN;
			}
			else
			{
				re = m_re / (T)rhs;
				im = m_im / (T)rhs;
			}
			return complex <T> (re, im);
		}

		template <typename X>
		__inline complex <T> operator / (const complex <X> & rhs)
		{
			T denom = (T)((rhs.m_re * rhs.m_re) + (rhs.m_im * rhs.m_im));
			if (denom == (T)0)
				denom = FLT_MIN;
			const T re = ((m_re * (T)rhs.m_re) + (m_im * (T)rhs.m_im)) / denom;
			const T im = ((m_im * (T)rhs.m_re) - (m_re * (T)rhs.m_im)) / denom;
			return complex <T> (re, im);
		}

		template <typename X>
		__inline complex <T> & operator /= (const X &rhs)
		{
         //T denom = rhs * rhs;
         //m_re    = (m_re * rhs) / denom;
         //m_im    = (m_im * rhs) / denom;

			if (rhs == (X)0)
			{
				m_re /= FLT_MIN;
				m_im /= FLT_MIN;
			}
			else
			{
				m_re /= (T)rhs;
				m_im /= (T)rhs;
			}
			return *this;
		}

		template <typename X>
		__inline complex <T> & operator /= (const complex <X> & rhs)
		{
			T denom = (T)((rhs.m_re * rhs.m_re) + (rhs.m_im * rhs.m_im));
			if (denom == (T)0)
				denom = (T)FLT_MIN;
			const T re = ((m_re * (T)rhs.m_re) + (m_im * (T)rhs.m_im)) / denom;
			const T im = ((m_im * (T)rhs.m_re) - (m_re * (T)rhs.m_im)) / denom;
			m_re = re;
			m_im = im;
			return *this;
		}
	};
/*
	template <typename T>
	__inline complex <T> & complex <T> :: operator = (const T &rhs)
	{
		m_re = rhs;
		m_im = (T)0;
		return *this;
	}

	template <typename T>
	__inline complex <T> & complex <T> :: operator = (const complex <T> &rhs)
	{
		m_re = rhs.re;
		m_im = rhs.im;
		return *this;
	}

	template <typename T>
	__inline complex <T> & complex <T> :: operator += (const complex <T> &rhs)
	{
		M_re += rhs.re;
		M_im += rhs.im;
		return *this;
	}
	template <typename T>
	__inline complex <T> & complex <T> :: operator += (const T &rhs)
	{
		m_re += rhs;
		return *this;
	}
*/
	template <typename T>
	__inline complex <T> operator - (const complex <T> &rhs)
	{
		return complex <T> (-rhs.re, -rhs.im);
	}

	template <typename T>
	__inline complex <T> operator - (const T &lhs, const complex <T> &rhs)
	{
		return complex <T> (lhs - rhs.re, -rhs.im);
	}

	template <typename T>
	__inline complex <T> operator - (const complex <T> & lhs, const T & rhs)
	{
		return complex <T> (lhs.re - rhs, lhs.im);
	}

	template <typename T>
	__inline complex <T> operator - (const complex <T> & lhs, const complex <T> & rhs)
	{
		return complex <T> (lhs.re - rhs.re, lhs.im - rhs.im);
	}

	template <typename T>
	__inline complex <T> operator + (const complex <T> & lhs, const T &rhs)
	{
		return complex <T> (lhs.re + rhs, lhs.im);
	}

	template <typename T>
	__inline complex <T> operator + (const T &lhs, const complex <T> &rhs)
	{
		return complex <T> (lhs + rhs.re, rhs.im);
	}

	template <typename T>
	__inline complex <T> operator + (const complex <T> & lhs, const complex <T> & rhs)
	{
		return complex <T> (lhs.re + rhs.re, lhs.im + rhs.im);
	}

	template <typename T>
	__inline complex <T> operator * (const complex <T> & lhs, const T & rhs)
	{
		return complex <T> (lhs.re * rhs, lhs.im * rhs);
	}

	template <typename T>
	__inline complex <T> operator * (const T & lhs, const complex <T> & rhs)
	{
		return complex <T> (lhs * rhs.re, lhs * rhs.im);
	}

	template <typename T>
	__inline complex <T> operator * (const complex <T> & lhs, const complex <T> & rhs)
	{
		const T re = (lhs.re * rhs.re) - (lhs.im * rhs.im);
		const T im = (lhs.im * rhs.re) + (lhs.re * rhs.im);
		return complex <T> (re, im);
	}

	template <typename T>
	__inline complex <T> operator / (const complex <T> &lhs, const T &rhs)
	{
		const T d = (rhs != (T)0) ? rhs : (T)FLT_MIN;
		return complex <T> (lhs.re / d, lhs.im / d);
	}

	template <typename T>
	__inline complex <T> operator / (const complex <T> & lhs, const complex <T> & rhs)
	{
		T denom = (rhs.re * rhs.re) + (rhs.im * rhs.im);
		if (denom == (T)0)
			denom = (T)FLT_MIN;
		const T re = ((lhs.re * rhs.re) + (lhs.im * rhs.im)) / denom;
		const T im = ((lhs.im * rhs.re) - (lhs.re * rhs.im)) / denom;
		return complex <T> (re, im);
	}

	template <typename T>
	__inline complex <T> operator / (const T & lhs, const complex <T> & rhs)
	{
		T denom = (rhs.re * rhs.re) + (rhs.im * rhs.im);
		if (denom == (T)0)
			denom = (T)FLT_MIN;
		const T re = ( (lhs * rhs.re)) / denom;
		const T im = (-(lhs * rhs.im)) / denom;
		return complex <T> (re, im);
	}

	template <typename T>
	__inline bool operator != (const complex <T> & lhs, const complex <T> & rhs)
	{
		return (lhs.re != rhs.re || lhs.im != rhs.im) ? true : false;
	}
	template <typename T, typename X>
	__inline bool operator != (const complex <T> & lhs, const complex <X> & rhs)
	{
		return (lhs.re != (T)rhs.re || lhs.im != (T)rhs.im) ? true : false;
	}

	template <typename T>
	__inline bool operator == (const complex <T> & lhs, const complex <T> & rhs)
	{
		return (lhs.re == rhs.re && lhs.im == rhs.im) ? true : false;
	}
	template <typename T, typename X>
	__inline bool operator == (const complex <T> & lhs, const complex <X> & rhs)
	{
		return (lhs.re == (T)rhs.re && lhs.im == (T)rhs.im) ? true : false;
	}

	template <typename T>
	__inline complex <T> conj(const complex <T> &c)
	{
		return complex <T> (c.re, -c.im);
	}

	template <typename T>
	__inline T norm(const complex <T> &c)
	{
		return ((c.re * c.re) + (c.im * c.im));
	}

	template <typename T>
	__inline T absf(const complex <float> &c)
	{
		return sqrtf((c.re * c.re) + (c.im * c.im));
	}

	template <typename T>
	__inline T argf(const complex <float> &c)
	{
		const T ac = acosf(c.re / absf(c));
		return (c.im >= (T)0) ? ac : (T)(M_PI * 2) - ac;
	}

	template <typename T>
	__inline complex <T> logf(const complex <float> &c)
	{
		const complex <T> l(logf(absf(c)), (T)0);
		const complex <T> a((T)0, argf(c));
		return l + a;
	}

	template <typename T>
	__inline complex <T> sqrtf(const complex <float> &c)
	{
		const T r  = std::sqrtf(absf(c));
		const T th = argf(c) / 2.0f;
		return complex <T> (r * cosf(th), r * sinf(th));
	}

	__inline complex <float> exp(const complex <float> &c)
	{
		const float e = std::expf(c.re);
		return complex <float> (e * cosf(c.im), e * sinf(c.im));
	}

	__inline complex <double> exp(const complex <double> &c)
	{
		const double e = std::exp(c.re);
		return complex <double> (e * cos(c.im), e * sin(c.im));
	}

	template <typename T>
	__inline complex <T> inv(const complex <T> &c)
	{
		const T n = (c.re * c.re) + (c.im * c.im);
		if (n == (T)0)
			n = (T)FLT_MIN;
		return complex <T> (c.re / n, -c.im / n);
	}

	// Euler's formula in a new convenient function
	template <typename T>
	__inline complex <T> exp_j(const T phi)
	{
		return complex <T> (cosf(phi), sinf(phi));
	}

	template <typename T>
	__inline complex <T> exp_conj(const T phi)
	{
		return complex <T> (cosf(phi), -sinf(phi));
	}

	// the function pow() returns the complex number z raised to a real power
	template <typename T>
	__inline complex <T> powf(const complex <float> &z, const float p)
	{
		const float paz = std::powf(absf(z), p);
		const float az  = p * argf(z);
		return complex <T> (paz * cosf(az), paz * sinf(az));
	}
};

//#ifdef _COMPLEX_
//	typedef std::complex <float>  complexf;		// slow std:complex
//	typedef std::complex <double> complexd;		//   "     "
//#else
	typedef nonstd::complex <float>  complexf;	// our own faster/more efficient nonstd::complex
	typedef nonstd::complex <double> complexd;	//  "   "       "         "            "
//#endif

#endif

