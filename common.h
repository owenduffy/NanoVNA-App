
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef commonH
#define commonH

#pragma once

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <Vcl.ComCtrls.hpp>

#include <lm.h>
#include <windows.h>
#include <vector>

#include "types.h"

class CCommon
{
	private:
		WKSTA_INFO_100 m_wsi;
		String m_windows_ver;
		String m_local_name;

		String m_title;

		String m_libusb_filename;
		String m_libusb_version;

		String m_log_filename;
		FILE *m_log_file;

		String m_data_filename;
		FILE *m_data_file;

		bool __fastcall getWinMajorMinorVersion(DWORD &major, DWORD &minor);
		String __fastcall getWindowsVersionString();

		bool __fastcall load_libuSB(String filename);

		void __fastcall onClick(TObject *Sender);
		void __fastcall onMouseLeave(TObject *Sender);

		void __fastcall onFileOpenDialogSelectionChange(TObject *Sender);

	public:
		CCommon();
		~CCommon();

		bool __fastcall isWinXP();

		void __fastcall logFileAppend(String s);
		void __fastcall logFileClose();
		void __fastcall logFileDelete();

		void __fastcall dataFileAppend(const void *data, const int size);
		void __fastcall dataFileClose();
		void __fastcall dataFileDelete();

		char __fastcall decimalPoint();

		bool __fastcall GetBuildInfo(String filename, TVersion *version);

		bool __fastcall createPath(const char *path);

		String __fastcall getLastErrorStr(DWORD err);
		int __fastcall getLastErrorStr(DWORD err, void *err_str, int max_size);

		void __fastcall comboBoxAutoWidth(TComboBox *comboBox);

		int __fastcall popcount32(int32_t v);

		void __fastcall putBitsLS(void *out_bits, uint32_t bits, int num_bits);
		void __fastcall putBitsMS(void *out_bits, uint32_t bits, int num_bits);

		uint32_t __fastcall getBitsLS(void *in_bits, int num_bits);
		uint32_t __fastcall getBitsMS(void *in_bits, int num_bits);

		void __fastcall bytesToBitsLS(void *in_bytes, void *out_bits, int num_bits);
		void __fastcall bitsToBytesLS(void *in_bits, void *out_bytes, int num_bits);

		void __fastcall bytesToBitsMS(void *in_bytes, void *out_bits, int num_bits);
		void __fastcall bitsToBytesMS(void *in_bits, void *out_bytes, int num_bits);

		void __fastcall reverseByteBits(void *out_bits, void *in_bits, int num_bits);

		uint8_t __fastcall bitReverse8(uint8_t x);
		uint16_t __fastcall bitReverse16(uint16_t x);
		uint32_t __fastcall bitReverse32(uint32_t x);

		uint8_t __fastcall parity32(uint32_t cw);

		String __fastcall localiseDecimalPoint(String s);

		int __fastcall parseString(String s, String separator, std::vector <String> &params);

		String __fastcall freqToStrMHz(double Hz);
		String __fastcall freqToStr1(double Hz, bool trim_trailing_zero, bool space_units, int fraction_size, bool show_sign);
		String __fastcall freqToStr2(int64_t Hz, const int left_padding);

		String __fastcall secsToStr(double time, bool trim_trailing_zero = true);

		String __fastcall distToStr(double distance, bool trim_trailing_zero = true);

		String __fastcall trimTrailingZeros(String s);

		uint32_t __fastcall updateCRC32(uint32_t crc, uint8_t b);
		uint32_t __fastcall updateCRC32(uint32_t crc, void *data, int data_len);

		int __fastcall resourceSize(String name);
		int __fastcall fetchResource(String name, std::vector <uint8_t> &buffer);

		int __fastcall loadFile(const String name, std::vector <uint8_t> &buffer);
		int __fastcall loadFile(const String name, std::vector <String> &buffer);

		int __fastcall saveFile(const String name, std::vector <uint8_t> &buffer);
		int __fastcall saveFile(const String name, std::vector <String> &buffer);

		void __fastcall saveBitmap(Graphics::TBitmap *bm, String ID, String dialog_title, const bool ask_filename);

		String __fastcall cleanFilename(String filename, const bool contains_dir, const bool trim);

		String __fastcall padLeft(String s, int size);
		String __fastcall padRight(String s, int size);

		bool __fastcall strToValue(String s, double &value, double multiplier = 1e0);

		bool __fastcall strToHz(String s, double &Hz, double default_multiplier = 1e0);
		bool __fastcall strTokHz(String s, double &kHz);
		bool __fastcall strToMHz(String s, double &MHz);
		bool __fastcall strToGHz(String s, double &GHz);
		bool __fastcall strToTHz(String s, double &THz);

		String __fastcall valueToStr(double value, const bool space_units, const bool trim_trailing_zeros, String fmt = "", const bool decimal_units = false);

		String __fastcall loadSParams(std::vector <t_data_point> &s_params, String filename = "");
		bool __fastcall saveSParams(std::vector <t_data_point> &points, int num_chans, String filename = "");

		String __fastcall loadCSV(std::vector <t_data_point> &points, String filename = "");
		bool __fastcall saveCSV(std::vector <t_data_point> &points, const int channels, const bool double_quotes = false, String filename = "");

		String __fastcall loadCalibrationFile(String filename, std::vector <t_calibration_point> &cal);
		String __fastcall saveCalibrationFile(String filename, std::vector <t_calibration_point> &cal, const bool ask_filename);

		void __fastcall saveBitmap(Graphics::TBitmap *bm, AnsiString ID);

		void __fastcall phaseUnwrap(float buffer[], const int size, float step = M_PI);
		void __fastcall phaseUnwrapRad(float buffer[], const int size)
		{
			phaseUnwrap(buffer, size, M_PI);
		}
		void __fastcall phaseUnwrapDeg(float buffer[], const int size)
		{
			phaseUnwrap(buffer, size, 180);
		}

		void __fastcall setWarning(TWinControl *control, String warning);

		__property String windowsVer = {read = m_windows_ver};
		__property String localName  = {read = m_local_name};
		__property String title      = {read = m_title, write = m_title};

		__property String libusbFilename = {read = m_libusb_filename};
		__property String libusbVersion  = {read = m_libusb_version};
};

extern CCommon common;

#endif

