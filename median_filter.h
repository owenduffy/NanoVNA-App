
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef median_filterH
#define median_filterH

#pragma once

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <vector>

#include "types.h"
//#include "DataUnit.h"

class CMedianFilter
{
	private:
		std::vector <float> tmp_f;
		std::vector < complexf > tmp_c;

		float __fastcall weighted_median(std::vector <float> &points)
		{
			const int size = points.size();
			// insert_sort
			for (int i = 0; i < size; i++)
				tmp_f[i] = points[i];
			for (int i = 1; i < size; i++)
			{
				int j;
				const float value = tmp_f[i];
				for (j = i; j >= 1 && value < tmp_f[j - 1]; j--)
					tmp_f[j] = tmp_f[j - 1];
				tmp_f[j] = value;
			}

			// take the average
			float value = 0.0f;
			for (int i = 0; i < size; i++)
				value += tmp_f[i];
			value -= tmp_f[size / 2];	// drop the center value
			value /= size - 1;

			return value;
		}

		float __fastcall median(std::vector <float> &points)
		{
			const int size = points.size();
			// insert_sort
			for (int i = 0; i < size; i++)
				tmp_f[i] = points[i];
			for (int i = 1; i < size; i++)
			{
				int j;
				const float value = tmp_f[i];
				for (j = i; j >= 1 && value < tmp_f[j - 1]; j--)
					tmp_f[j] = tmp_f[j - 1];
				tmp_f[j] = value;
			}
			return tmp_f[size / 2];	// return the median value (center of sorted array)
		}
/*
		float __fastcall median(std::vector <float> &points)
		{
			const int size = points.size();
			for (int i = 0; i < size; i++)
				tmp_f[i] = points[i];
			for (int i = 0; i < size - 1; i++)
			{
				float v1 = tmp_f[i];
				for (int k = i + 1; k < size; k++)
				{
					const float v2 = tmp_f[k];
					if (v1 > v2)
					{	// swap
						tmp_f[i] = v2;
						tmp_f[k] = v1;
						v1 = v2;
					}
				}
			}
			return tmp_f[size / 2];	// return the median value (center of sorted array)
		}
*/
		complexf __fastcall weighted_median(std::vector < complexf > &points)
		{
			const int size = points.size();
			// insert_sort
			for (int i = 0; i < size; i++)
				tmp_c[i] = points[i];

			for (int i = 1; i < size; i++)
			{
				int j = i;
				const complexf v1 = tmp_c[i];
				while (j >= 1)
				{
					const complexf v2 = tmp_c[j - 1];
					if (v1.real() >= v2.real())
						break;
					tmp_c[j] = v2;
					j--;
				}
				tmp_c[j] = v1;
			}

			for (int i = 1; i < size; i++)
			{
				int j = i;
				const complexf v1 = tmp_c[i];
				while (j >= 1)
				{
					const complexf v2 = tmp_c[j - 1].imag();
					if (v1.imag() >= v2.imag())
						break;
					tmp_c[j] = v2;
					j--;
				}
				tmp_c[j] = v1;
			}

			// take the average
			complexf value = 0;
			for (int i = 0; i < size; i++)
				value += tmp_c[i];
			value -= tmp_c[size / 2];	// drop the center value
			value /= size - 1;

			return value;
		}

		complexf __fastcall median(std::vector < complexf > &points)
		{
			const int size = points.size();
			// insert_sort
			for (int i = 0; i < size; i++)
				tmp_c[i] = points[i];

			for (int i = 1; i < size; i++)
			{
				int j = i;
				const complexf v1 = tmp_c[i];
				while (j >= 1)
				{
					const complexf v2 = tmp_c[j - 1];
					if (v1.real() >= v2.real())
						break;
					tmp_c[j] = v2;
					j--;
				}
				tmp_c[j] = v1;
			}

			for (int i = 1; i < size; i++)
			{
				int j = i;
				const complexf v1 = tmp_c[i];
				while (j >= 1)
				{
					const complexf v2 = tmp_c[j - 1];
					if (v1.imag() >= v2.imag())
						break;
					tmp_c[j] = v2;
					j--;
				}
				tmp_c[j] = v1;
			}

			return tmp_c[size / 2];	// return the median value (center of sorted array)
		}

/*		complexf __fastcall median(std::vector < complexf > &points)
		{
			const int size = points.size();
			for (int i = 0; i < size; i++)
				tmp_c[i] = points[i];

			for (int i = 0; i < size - 1; i++)
			{
				float v1 = tmp_c[i].real;
				for (int k = i + 1; k < size; k++)
				{
					const float v2 = tmp_c[k].real;
					if (v1 > v2)
					{	// swap
						tmp_c[i].real = v2;
						tmp_c[k].real = v1;
						v1 = v2;
					}
				}
			}

			for (int i = 0; i < size - 1; i++)
			{
				float v1 = tmp_c[i].imag;
				for (int k = i + 1; k < size; k++)
				{
					const float v2 = tmp_c[k].imag;
					if (v1 > v2)
					{	// swap
						tmp_c[i].imag = v2;
						tmp_c[k].imag = v1;
						v1 = v2;
					}
				}
			}

			return tmp_c[size / 2];	// return the median value (center of sorted array)
		}
*/
	public:
		void __fastcall process(std::vector <float> &points, const int size = 1, const bool weighted = false)
		{
			if (size <= 0)
				return;

			const int width = size + 1 + size;

			std::vector <float> p(width);
			tmp_f.resize(width);

			int n = 0;

			unsigned int i = 0;

			{	// first point
				int k = 0;
				int m = 0;
				while (k < size)	// was <=
					p[k++] = points[m];
				while (k < width)
					p[k++] = points[++m];
				points[i++] = weighted ? weighted_median(p) : median(p);
			}

			while (i < (points.size() - size))
			{
//				memmove(&p[0], &p[1], sizeof(p[0]) * (width - 1));
//				p[width - 1] = points[i + size];
				p[n] = points[i + size];
				if (++n >= width)
					n = 0;
				points[i++] = weighted ? weighted_median(p) : median(p);
			}

			// last points
			const float lp = points[i - 1];
			while (i < points.size())
			{
//				memmove(&p[0], &p[1], sizeof(p[0]) * (width - 1));
				p[n] = lp;
				if (++n >= width)
					n = 0;
				points[i++] = weighted ? weighted_median(p) : median(p);
			}
		}

		void __fastcall process(std::vector < complexf > &points, const int size = 1, const bool weighted = false)
		{
			if (size <= 0)
				return;

			const int width = size + 1 + size;

			std::vector < complexf > p(width);
			tmp_c.resize(width);

			int n = 0;

			unsigned int i = 0;

			{	// first point
				int k = 0;
				int m = 0;
				while (k < size)	// was <=
					p[k++] = points[m];
				while (k < width)
					p[k++] = points[++m];
				points[i++] = weighted ? weighted_median(p) : median(p);
			}

			while (i < (points.size() - size))
			{
//				memmove(&p[0], &p[1], sizeof(p[0]) * (width - 1));
//				p[width - 1] = points[i + size];
				p[n] = points[i + size];
				if (++n >= width)
					n = 0;
				points[i++] = weighted ? weighted_median(p) : median(p);
			}

			// last points
			const complexf lp = points[i - 1];
			while (i < points.size())
			{
//				memmove(&p[0], &p[1], sizeof(p[0]) * (width - 1));
				p[n] = lp;
				if (++n >= width)
					n = 0;
				points[i++] = weighted ? weighted_median(p) : median(p);
			}
		}
};

#endif
