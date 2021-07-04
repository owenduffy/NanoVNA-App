
#ifndef splineH
#define splineH

#include <math.h>
#include <vector>
#include <map>

// ***********************************************************

class vec2
{
public:
	vec2()
	{
		x = 0;
		y = 0;
	}

	vec2(float vx, float vy)
	{
		x = vx;
		y = vy;
	}

	vec2(const vec2 &v)
	{
		x = v.x;
		y = v.y;
	}

	vec2 operator * (float mult) const
	{
		return vec2(x * mult, y * mult);
	}

	vec2 operator + (const vec2 &v) const
	{
		return vec2(x + v.x, y + v.y);
	}

	vec2 operator - (const vec2 &v) const
	{
		return vec2(x - v.x, y - v.y);
	}

	float x;
	float y;
};

class vec3
{
public:
	vec3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	vec3(float vx, float vy, float vz)
	{
		x = vx;
		y = vy;
		z = vz;
	}

	vec3(const vec3 &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	vec3 operator * (float mult) const
	{
		return vec3(x * mult, y * mult, z * mult);
	}

	vec3 operator + (const vec3 &v) const
	{
		return vec3(x + v.x, y + v.y, z + v.z);
	}

	vec3 operator - (const vec3 &v) const
	{
		return vec3(x - v.x, y - v.y, z - v.z);
	}

	float x;
	float y;
	float z;
};

// ***********************************************************
// Catmull-Rom spline

class CRSpline1
{
private:
	float delta_t;
	std::vector <float> vp;

public:
	CRSpline1()
	{
		delta_t = 0;
		vp.reserve(2000);
	}

	CRSpline1(const CRSpline1 &s)
	{
		for (unsigned int i = 0; i < s.vp.size(); i++)
			vp.push_back(s.vp[i]);

		delta_t = s.delta_t;
	}

	int __fastcall numPoints()
	{	// return number of control points
		return vp.size();
	}

	float __fastcall point(const int n)
	{	// return a control point
		return (n >= 0 && n < (int)vp.size()) ? vp[n] : 0.0f;
	}

	void __fastcall addPoint(const float x)
	{	// add a control point
		vp.push_back(x);
		delta_t = 1.0f / vp.size();
	}

	void __fastcall addPoint(const float &v)
	{	// add a control point
		vp.push_back(v);
		delta_t = 1.0f / vp.size();
	}

	float __fastcall interpolatedPoint(const float t)   // t = 0...1; 0=vp[0] ... 1=vp[max]
	{
		if (vp.size() < 1)
			return 0.0f;

		// Find out in which interval we are on the spline
		const int p = (int)floor(t * vp.size());

		// Compute local control point indices
		const int m = vp.size() - 1;
		int p0 = p - 1; p0 = (p0 < 0) ? 0 : (p0 > m) ? m : p0;
		int p1 = p;     p1 = (p1 < 0) ? 0 : (p1 > m) ? m : p1;
		int p2 = p + 1; p2 = (p2 < 0) ? 0 : (p2 > m) ? m : p2;
		int p3 = p + 2; p3 = (p3 < 0) ? 0 : (p3 > m) ? m : p3;

		// Relative (local) time
		const float t1 = (t - (p * delta_t)) * vp.size();

		// Interpolate .. compute the Catmull-Rom parametric equation given a time (t) and a vector quadruple (p1,p2,p3,p4).
		const float t2 = t1 * t1;
		const float t3 = t2 * t1;
		const float b1 = 0.5f * (  -t3 + 2*t2 - t1);
		const float b2 = 0.5f * ( 3*t3 - 5*t2 +  2);
		const float b3 = 0.5f * (-3*t3 + 4*t2 + t1);
		const float b4 = 0.5f * (   t3 -   t2    );
		return ((vp[p0] * b1) + (vp[p1] * b2) + (vp[p2] * b3) + (vp[p3] * b4));
	}
};

class CRSpline2
{
private:
	float delta_t;
	std::vector <vec2> vp;

public:
	CRSpline2()
	{
		delta_t = 0;
		vp.reserve(2000);
	}

	CRSpline2(const CRSpline2 &s)
	{
		for (unsigned int i = 0; i < s.vp.size(); i++)
			vp.push_back(s.vp[i]);

		delta_t = s.delta_t;
	}

	int __fastcall numPoints()
	{	// return number of control points
		return vp.size();
	}

	vec2 __fastcall point(const int n)
	{	// return a control point
		return (n >= 0 && n < (int)vp.size()) ? vp[n] : vec2();
	}

	void __fastcall addPoint(const float x, const float y)
	{	// add a control point
		vp.push_back(vec2(x, y));
		delta_t = 1.0f / vp.size();
	}

	void __fastcall addPoint(const vec2 &v)
	{	// add a control point
		vp.push_back(v);
		delta_t = 1.0f / vp.size();
	}

	vec2 __fastcall interpolatedPoint(const float t)   // t = 0...1; 0=vp[0] ... 1=vp[max]
	{
		if (vp.size() < 1)
			return vec2();

		// Find out in which interval we are on the spline
		const int p = (int)floor(t * vp.size());

		// Compute local control point indices
		const int m = vp.size() - 1;
		int p0 = p - 1; p0 = (p0 < 0) ? 0 : (p0 > m) ? m : p0;
		int p1 = p;     p1 = (p1 < 0) ? 0 : (p1 > m) ? m : p1;
		int p2 = p + 1; p2 = (p2 < 0) ? 0 : (p2 > m) ? m : p2;
		int p3 = p + 2; p3 = (p3 < 0) ? 0 : (p3 > m) ? m : p3;

		// Relative (local) time
		const float t1 = (t - (p * delta_t)) * vp.size();

		// Interpolate .. compute the Catmull-Rom parametric equation given a time (t) and a vector quadruple (p1,p2,p3,p4).
		const float t2 = t1 * t1;
		const float t3 = t2 * t1;
		const float b1 = 0.5f * (  -t3 + 2*t2 - t1);
		const float b2 = 0.5f * ( 3*t3 - 5*t2 +  2);
		const float b3 = 0.5f * (-3*t3 + 4*t2 + t1);
		const float b4 = 0.5f * (   t3 -   t2    );
		return ((vp[p0] * b1) + (vp[p1] * b2) + (vp[p2] * b3) + (vp[p3] * b4));
	}
};

class CRSpline3
{
private:
	float delta_t;
	std::vector <vec3> vp;

public:
	CRSpline3()
	{
		delta_t = 0;
		vp.reserve(2000);
	}

	CRSpline3(const CRSpline3 &s)
	{
		for (unsigned int i = 0; i < s.vp.size(); i++)
			vp.push_back(s.vp[i]);

		delta_t = s.delta_t;
	}

	int __fastcall numPoints()
	{	// return number of control points
		return vp.size();
	}

	vec3 __fastcall point(const int n)
	{	// return a control point
		return (n >= 0 && n < (int)vp.size()) ? vp[n] : vec3();
	}

	void __fastcall addPoint(const float x, const float y, const float z)
	{	// add a control point
		vp.push_back(vec3(x, y, z));
		delta_t = 1.0f / vp.size();
	}

	void __fastcall addPoint(const vec3 &v)
	{	// add a control point
		vp.push_back(v);
		delta_t = 1.0f / vp.size();
	}

	vec3 __fastcall interpolatedPoint(const float t)   // t = 0...1; 0=vp[0] ... 1=vp[max]
	{
		if (vp.size() < 1)
			return vec3();

		// Find out in which interval we are on the spline
		const int p = (int)floor(t * vp.size());

		// Compute local control point indices
		const int m = vp.size() - 1;
		int p0 = p - 1; p0 = (p0 < 0) ? 0 : (p0 > m) ? m : p0;
		int p1 = p;     p1 = (p1 < 0) ? 0 : (p1 > m) ? m : p1;
		int p2 = p + 1; p2 = (p2 < 0) ? 0 : (p2 > m) ? m : p2;
		int p3 = p + 2; p3 = (p3 < 0) ? 0 : (p3 > m) ? m : p3;

		// Relative (local) time
		const float t1 = (t - (p * delta_t)) * vp.size();

		// Interpolate .. compute the Catmull-Rom parametric equation given a time (t) and a vector quadruple (p1,p2,p3,p4).
		const float t2 = t1 * t1;
		const float t3 = t2 * t1;
		const float b1 = 0.5f * (  -t3 + 2*t2 - t1);
		const float b2 = 0.5f * ( 3*t3 - 5*t2 +  2);
		const float b3 = 0.5f * (-3*t3 + 4*t2 + t1);
		const float b4 = 0.5f * (   t3 -   t2    );
		return ((vp[p0] * b1) + (vp[p1] * b2) + (vp[p2] * b3) + (vp[p3] * b4));
	}
};

// ***********************************************************

// MonotCubicInterpolator
// Copyright (C) 2006 Statoil ASA
//
// This program is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

// Class to represent a one-dimensional function f with single-valued argument x.
// The function is represented by a table of function values.
// Interpolation between table values is cubic and monotonicity preserving if input values are monotonous.
//
// Outside x_min and x_max, the class will extrapolate using the constant f(x_min) or f(x_max).
//
// Extra functionality:
//  - Can return (x_1+x_2)/2 where x_1 and x_2 are such that
//    abs(f(x_1) - f(x_2)) is maximized. This is used to determine where
//    one should calculate a new value for increased accuracy in the
//    current function
//
// Monotonicity preserving cubic interpolation algorithm is taken
// from Fritsch and Carlson, "Monotone piecewise cubic interpolation",
// SIAM J. Numer. Anal. 17, 238--246, no. 2,
//
// $Id$
//
// Algorithm also described here:
// http://en.wikipedia.org/wiki/Monotone_cubic_interpolation
//
// @author Håvard Berland <havb (at) statoil.com>, December 2006
// @brief Represents one dimensional function f with single valued argument x that can be interpolated using monotone cubic interpolation

class MonotCubicInterpolator
{
	private:
		// Data structure to store x- and f-values
		std::map <double, double> data;

		// Data structure to store x- and d-values
		mutable std::map <double, double> ddata;

		// Storage containers for precomputed interpolation data
		//   std::vector <double> dvalues; // derivatives in Hermite interpolation.

		// Flag to determine whether the boolean strictlyMonotone can be trusted.
		mutable bool strictlyMonotoneCached;
		mutable bool monotoneCached;     // only monotone, not stricly montone

		mutable bool strictlyMonotone;
		mutable bool monotone;

		// if strictlyMonotone is true (and can be trusted), the two next are meaningful
		mutable bool strictlyDecreasing;
		mutable bool strictlyIncreasing;
		mutable bool decreasing;
		mutable bool increasing;

		// Hermite basis functions, t \in [0,1], notation from:
		// http://en.wikipedia.org/w/index.php?title=Cubic_Hermite_spline&oldid=84495502

		double __fastcall H00(double t) const
		{
			return (2 * t * t * t) - (3 * t * t) + 1;
		}
		double __fastcall H10(double t) const
		{
			return (t * t * t) - (2 * t * t) + t;
		}
		double __fastcall H01(double t) const
		{
			return (-2 * t * t * t) + (3 * t * t);
		}
		double __fastcall H11(double t) const
		{
			return (t * t * t) - (t * t);
		}

		void __fastcall computeInternalFunctionData() const;

		// Computes initial derivative values using centered (second order) difference for internal
		// datapoints, and one-sided derivative for endpoints
		//
		// The internal datastructure map<double,double> ddata is populated by this method.
		void __fastcall computeSimpleDerivatives() const;

		// Adjusts the derivative values (ddata) so that we can guarantee that the resulting piecewise Hermite polymial is monotone.
		// This is done according to the algorithm of Fritsch and Carlsson 1980, see Section 4, especially the two last lines.
		void __fastcall adjustDerivativesForMonotoneness() const;

		// Checks if the coefficient alpha and beta is in the region that guarantees monotoneness of the
		// derivative values they represent
		//
		// See Fritsch and Carlson 1980, Lemma 2, alternatively Step 5 in Wikipedia's article
		// on Monotone cubic interpolation.
		bool __fastcall isMonotoneCoeff(double alpha, double beta) const
		{
			return (((alpha * alpha) + (beta * beta)) <= 9) ? true : false;
		}

	public:
/*
		// @param datafilename A datafile with the x values and the corresponding f(x) values
		//
		// Accepts a filename as input and parses this file for
		// two-column floating point data, interpreting the data as
		// representing function values x and f(x).
		//
		// Ignores all lines not conforming to \<whitespace\>\<float\>\<whitespace\>\<float\>\<whatever\>\<newline\>
		MonotCubicInterpolator(const std::string &datafilename) throw (const char *)
		{
			if (!read(datafilename))
				throw("Unable to constuct MonotCubicInterpolator from file.");
		}

		// @param datafilename A datafile with the x values and the corresponding f(x) values
		//
		// Accepts a filename as input and parses this file for
		// two-column floating point data, interpreting the data as
		// representing function values x and f(x).
		//
		// Ignores all lines not conforming to \<whitespace\>\<float\>\<whitespace\>\<float\>\<whatever\>\<newline\>
		//
		// All commas in the file will be treated as spaces when parsing.
		MonotCubicInterpolator(const char *datafilename) throw (const char *)
		{
			if (!read(std::string(datafilename)))
				throw("Unable to constuct MonotCubicInterpolator from file.");
		}

		// @param datafilename data file
		// @param XColumn x values
		// @param fColumn f values
		//
		// Accepts a filename as input, and parses the chosen columns in that file.
		MonotCubicInterpolator(const char* datafilename, int xColumn, int fColumn) throw (const char*)
		{
			if (!read(std::string(datafilename), xColumn, fColumn))
				throw("Unable to constuct MonotCubicInterpolator from file.");
		}

		// @param datafilename data file
		// @param XColumn x values
		// @param fColumn f values
		//
		// Accepts a filename as input, and parses the chosen columns in that file.
		MonotCubicInterpolator(const std::string &datafilename, int xColumn, int fColumn) throw (const char*)
		{
			if (!read(datafilename,xColumn,fColumn))
				throw("Unable to constuct MonotCubicInterpolator from file.");
		}
*/
		// @param x vector of x values
		// @param f vector of corresponding f values
		//
		// Accepts two equal-length vectors as input for constructing the interpolation object.
		// First vector is the x-values, the second vector is the function values
		MonotCubicInterpolator(const std::vector <double> &x, const std::vector <double> &f);

		// No input, an empty function object is created.
		//
		// This object must be treated with care until populated.
		MonotCubicInterpolator()
		{
		}
/*
		// @param datafilename A datafile with the x values and the corresponding f(x) values
		//
		// Accepts a filename as input and parses this file for
		// two-column floating point data, interpreting the data as
		// representing function values x and f(x).
		//
		// returns true on success
		//
		// All commas in file will be treated as spaces when parsing
		//
		// Ignores all lines not conforming to \<whitespace\>\<float\>\<whitespace\>\<float\>\<whatever\>\<newline\>
		bool __fastcall read(const std::string &datafilename)
		{
			return read(datafilename, 1, 2);
		}

		// @param datafilename data file
		// @param XColumn x values
		// @param fColumn f values
		//
		// Accepts a filename as input, and parses the chosen columns in that file.
		bool __fastcall read(const std::string &datafilename, int xColumn, int fColumn);
*/
		// @param x x value
		//
		// Returns f(x) for given x (input). Interpolates (monotone cubic
		// or linearly) if necessary.
		//
		// Extrapolates using the constants f(x_min) or f(x_max) if
		// input x is outside (x_min, x_max)
		//
		// @return f(x) for a given x
		double operator () (double x) const
		{
			return evaluate(x);
		}

		// @param x x value
		//
		// Returns f(x) for given x (input). Interpolates (monotone cubic or linearly) if necessary.
		//
		// Extrapolates using the constants f(x_min) or f(x_max) if
		// input x is outside (x_min, x_max)
		//
		// @return f(x) for a given x
		double __fastcall evaluate(double x) const throw(const char *);

		// @param x x value
		// @param errorestimate_output
		//
		// Returns f(x) and an error estimate for given x (input).
		//
		// Interpolates (linearly) if necessary.
		//
		// Throws an exception if extrapolation would be necessary for
		// evaluation. We do not want to do extrapolation (yet).
		//
		// The error estimate for x1 < x < x2 is
		// (x2 - x1)^2/8 * f''(x) where f''(x) is evaluated using
		// the stencil (1 -2  1) using either (x0, x1, x2) or (x1, x2, x3);
		//
		// Throws an exception if the table contains only two x-values.
		//
		// NOT IMPLEMENTED YET!
		double __fastcall evaluate(double x, double &errorestimate_output) const;

		// Minimum x-value, returns both x and f in a pair.
		//
		// @return minimum x value
		// @return f(minimum x value)
		std::pair <double, double> __fastcall getMinimumX() const
		{
			// Easy since the data is sorted on x:
			return *data.begin();
		}

		// Maximum x-value, returns both x and f in a pair.
		//
		// @return maximum x value
		// @return f(maximum x value)
		std::pair <double, double> __fastcall getMaximumX() const
		{
			// Easy since the data is sorted on x:
			return *data.rbegin();
		}

		// Maximum f-value, returns both x and f in a pair.
		//
		// @return x value corresponding to maximum f value
		// @return maximum f value
		std::pair <double, double> __fastcall getMaximumF() const throw(const char *);

		// Minimum f-value, returns both x and f in a pair
		//
		// @return x value corresponding to minimal f value
		// @return minimum f value
		std::pair <double, double> __fastcall getMinimumF() const throw(const char *);

		// Provide a copy of the x-data as a vector
		//
		// Unspecified order, but corresponds to get_fVector.
		//
		// @return x values as a vector
		std::vector <double> __fastcall get_xVector() const;

		// Provide a copy of the function data as a vector
		//
		// Unspecified order, but corresponds to get_xVector
		//
		// @return f values as a vector
		std::vector <double> __fastcall get_fVector() const;

		// @param factor Scaling constant
		//
		// Scale all the function value data by a constant
		void __fastcall scaleData(double factor);

		// Determines if the current function-value-data is strictly monotone.
		// This is a utility function for outsiders if they want to invert the data for example.
		//
		// @return True if f(x) is strictly monotone, else False
		bool __fastcall isStrictlyMonotone()
		{
			// Use cached value if it can be trusted
			if (strictlyMonotoneCached)
				return strictlyMonotone;

			computeInternalFunctionData();
			return strictlyMonotone;
		}

		// Determines if the current function-value-data is monotone.
		//
		// @return True if f(x) is monotone, else False
		bool __fastcall isMonotone() const
		{
			if (monotoneCached)
			  return monotone;

			computeInternalFunctionData();
			return monotone;
		}

		// Determines if the current function-value-data is strictly increasing.
		// This is a utility function for outsiders if they want to invert the data for example.
		//
		// @return True if f(x) is strictly increasing, else False
		bool __fastcall isStrictlyIncreasing()
		{
			// Use cached value if it can be trusted
			if (strictlyMonotoneCached)
				return (strictlyMonotone && strictlyIncreasing);

			computeInternalFunctionData();
			return (strictlyMonotone && strictlyIncreasing);
		}

		// Determines if the current function-value-data is monotone and increasing.
		//
		// @return True if f(x) is monotone and increasing, else False
		bool __fastcall isMonotoneIncreasing() const
		{
			if (monotoneCached)
				return (monotone && increasing);

			computeInternalFunctionData();
			return (monotone && increasing);
		}

		// Determines if the current function-value-data is strictly decreasing.
		// This is a utility function for outsiders if they want to invert the data for example.
		//
		// @return True if f(x) is strictly decreasing, else False
		bool __fastcall isStrictlyDecreasing()
		{
			// Use cached value if it can be trusted
			if (strictlyMonotoneCached)
				return (strictlyMonotone && strictlyDecreasing);

			computeInternalFunctionData();
			return (strictlyMonotone && strictlyDecreasing);
		}

		// Determines if the current function-value-data is monotone and decreasing
		//
		// @return True if f(x) is monotone and decreasing, else False
		bool __fastcall isMonotoneDecreasing() const
		{
			if (monotoneCached)
				return (monotone && decreasing);

			computeInternalFunctionData();
			return (monotone && decreasing);
		}

		// @param newx New x point
		// @param newf New f(x) point
		//
		// Adds a new datapoint to the function.
		//
		// This causes all the derivatives at all points of the functions to be recomputed and
		// then adjusted for monotone cubic interpolation.
		// If this function ever enters a critical part of any code, the locality of the algorithm
		// for monotone adjustment must be exploited.
		void __fastcall addPair(double newx, double newf, bool compute = true) throw(const char *);

		// Returns an x-value that is believed to yield the best improvement in global accuracy
		// for the interpolation if computed.
		//
		// Searches for the largest jump in f-values, and returns a x value being the average of
		// the two x-values representing the f-value-jump.
		//
		// @return New x value beleived to yield the best improvement in global accuracy
		// @return Maximal difference
		std::pair <double, double> __fastcall getMissingX() const throw(const char *);

		// Constructs a string containing the data in a table
		//
		// @return a string containing the data in a table
		std::string __fastcall toString() const;

		// @return Number of datapoint pairs in this object
		int __fastcall getSize() const
		{
			return data.size();
		}

		// Checks if the function curve is flat at the endpoints, chop off
		// endpoint data points if that is the case.
		//
		// The notion of "flat" is determined by the input parameter "epsilon"
		// Values whose difference are less than epsilon are regarded as equal.
		//
		// This is implemented to be able to obtain a strictly monotone
		// curve from a data set that is strictly monotone except at the
		// endpoints.
		//
		// Example:
		//   The data points
		//      (1,3), (2,3), (3,4), (4,5), (5,5), (6,5)
		//   will become
		//      (2,3), (3,4), (4,5)
		//
		// Assumes at least 3 datapoints. If less than three, this function is a noop.
		void __fastcall chopFlatEndpoints(const double);

		// Wrapper function for chopFlatEndpoints(const double) providing a default epsilon parameter
		void __fastcall chopFlatEndpoints()
		{
			chopFlatEndpoints(1e-14);
		}

		// If function is monotone, but not strictly monotone,
		// this function will remove datapoints from intervals
		// with zero derivative so that the curve become
		// strictly monotone.
		//
		// Example
		//   The data points
		//     (1,2), (2,3), (3,4), (4,4), (5,5), (6,6)
		//   will become
		//     (1,2), (2,3), (3,4), (5,5), (6,6)
		//
		// Assumes at least two datapoints, if one or zero datapoint, this is a noop.
		void __fastcall shrinkFlatAreas(const double);

		// Wrapper function for shrinkFlatAreas(const double) providing a default epsilon parameter
		void __fastcall shrinkFlatAreas()
		{
			shrinkFlatAreas(1e-14);
		}
};

// *********************************************************

#endif

