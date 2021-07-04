
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef smoothing_filterH
#define smoothing_filterH

//#pragma once

#include <vector>

#include "types.h"

// https://terpconnect.umd.edu/~toh/spectrum/Smoothing.html

//const float b3_coeffs[] = {1.0f, 1.0f, 1.0f};               // 3-point boxcar
//const float t3_coeffs[] = {1.0f, 2.0f, 1.0f};               // 3-point triangular
//
//const float b5_coeffs[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};   // 5-point boxcar
//const float t5_coeffs[] = {1.0f, 2.0f, 3.0f, 2.0f, 1.0f};   // 5-point triangular

class CSmoothingFilter
{
	public:
		void __fastcall process(std::vector <float> &points, const int iterations = 1, const int size = 1, const bool boxcar = false)
		{
			if (iterations <= 0 || size <= 0)
				return;

			const int width = size + 1 + size;

			if ((int)points.size() < width)
				return;

			std::vector <float> coeffs;

			if (boxcar)
			{	// create a boxcar coeff array
				for (int i = 0; i < width; i++)
					coeffs.push_back(1.0f);
			}
			else
			{	// create a triangular coeff array
				int coeff = 0;
				int i = 0;

				while (i++ <= size)
					coeffs.push_back(++coeff);
				while (i++ <= width)
					coeffs.push_back(--coeff);

				//while (i++ <= size)
				//{
				//	coeff += 2;
				//	coeffs.push_back(coeff);
				//}
				//while (i++ <= width)
				//{
				//	coeff -= 2;
				//	coeffs.push_back(coeff);
				//}
			}

			float scale = 0.0f;
			for (int i = 0; i < width; i++)
				scale += coeffs[i];
			if (scale != 0.0f)
				scale = 1.0f / scale;

			std::vector <float> p(width);

			for (int iteration = 0; iteration < iterations; iteration++)
			{
				unsigned int i = 0;

				{	// first point
					int k = 0;
					int m = 0;
					while (k <= size)
						p[k++] = points[m];
					while (k < width)
						p[k++] = points[++m];

					float pnt = 0.0f;
					for (k = 0; k < width; k++)
						pnt += p[k] * coeffs[k];
					pnt *= scale;

					points[i++] = pnt;
				}

				while (i < (points.size() - size))
				{
					memmove(&p[0], &p[1], sizeof(p[0]) * (width - 1));
					p[width - 1] = points[i + size];

					float pnt = 0.0f;
					for (int k = 0; k < width; k++)
						pnt += p[k] * coeffs[k];
					pnt *= scale;

					points[i++] = pnt;
				}

				while (i < points.size())
				{	// last points
					memmove(&p[0], &p[1], sizeof(p[0]) * (width - 1));

					float pnt = 0.0f;
					for (int k = 0; k < width; k++)
						pnt += p[k] * coeffs[k];
					pnt *= scale;

					points[i++] = pnt;
				}
			}
		}

		void __fastcall process(std::vector < complexf > &points, const int iterations = 1, const int size = 1, const bool boxcar = false)
		{
			// https://terpconnect.umd.edu/~toh/spectrum/Smoothing.html

			if (iterations <= 0 || size <= 0)
				return;

			const int width = size + 1 + size;

			if ((int)points.size() < width)
				return;

			std::vector <float> coeffs;

			if (boxcar)
			{	// create a boxcar coeff array
				for (int i = 0; i < width; i++)
					coeffs.push_back(1.0f);
			}
			else
			{	// create a triangular coeff array
				int coeff = 0;
				int i = 0;

				while (i++ <= size)
					coeffs.push_back(++coeff);
				while (i++ <= width)
					coeffs.push_back(--coeff);

				//while (i++ <= size)
				//{
				//	coeff += 2;
				//	coeffs.push_back(coeff);
				//}
				//while (i++ <= width)
				//{
				//	coeff -= 2;
				//	coeffs.push_back(coeff);
				//}
			}

			float scale = 0.0f;
			for (int i = 0; i < width; i++)
				scale += coeffs[i];
			if (scale != 0.0f)
				scale = 1.0f / scale;

			std::vector < complexf > p(width);

			for (int iteration = 0; iteration < iterations; iteration++)
			{
				unsigned int i = 0;

				{	// first point
					int k = 0;
					int m = 0;
					while (k <= size)
						p[k++] = points[m];
					while (k < width)
						p[k++] = points[++m];

					complexf pnt;
					for (k = 0; k < width; k++)
						pnt += p[k] * coeffs[k];
					pnt *= scale;

					points[i++] = pnt;
				}

				while (i < (points.size() - size))
				{
					memmove(&p[0], &p[1], sizeof(p[0]) * (width - 1));
					p[width - 1] = points[i + size];

					complexf pnt;
					for (int k = 0; k < width; k++)
						pnt += p[k] * coeffs[k];
					pnt *= scale;

					points[i++] = pnt;
				}

				while (i < points.size())
				{	// last points
					memmove(&p[0], &p[1], sizeof(p[0]) * (width - 1));

					complexf pnt;
					for (int k = 0; k < width; k++)
						pnt += p[k] * coeffs[k];
					pnt *= scale;

					points[i++] = pnt;
				}
			}
		}
};

#endif
