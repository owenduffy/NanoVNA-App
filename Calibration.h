
#ifndef CalibrationH
#define CalibrationH

#include "types.h"

class CCalibration
{
	private:
		complexf m_ideal_short;
		complexf m_ideal_open;
		complexf m_ideal_load;
		complexf m_ideal_isolation;
		complexf m_ideal_through;

		double m_short_l0;
		double m_short_l1;
		double m_short_l2;
		double m_short_l3;
		double m_short_length;

		double m_open_c0;
		double m_open_c1;
		double m_open_c2;
		double m_open_c3;
		double m_open_length;

		double m_load_r;
		double m_load_l;
		double m_load_c;
		double m_load_length;

		double m_through_length;

		void __fastcall interpolateErrorTerms(t_calibration &calibration, std::vector <t_data_point> &points, const bool linear);

	public:
		CCalibration();

		bool m_use_ideal_short;
		bool m_use_ideal_open;
		bool m_use_ideal_load;
		bool m_use_ideal_through;

		// calibration data
		t_calibration m_calibration;
		std::vector <t_calibration_point> m_inter_cal;	// interpolated calibration data

		void __fastcall computeErrorTerms(t_calibration &calibration);

		void __fastcall correct(std::vector <t_data_point> &points);

		__property complexf idealShort     = {read = m_ideal_short};
		__property complexf idealOpen      = {read = m_ideal_open};
		__property complexf idealLoad      = {read = m_ideal_load};
		__property complexf idealIsolation = {read = m_ideal_isolation};
		__property complexf idealThrough   = {read = m_ideal_through};
};

extern CCalibration calibration_module;

#endif

