
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

//#include <windows.h>
//#include <Windows.hpp>

#include <sysutils.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>	// TSavePictureDialog
#include <System.StrUtils.hpp>

#ifdef __BORLANDC__
	#if (__BORLANDC__ < 0x0600)
		#include <pngimage.hpp>
		#include <Jpeg.hpp>
		#include <GIFImg.hpp>
	#else
		#include <Vcl.Imaging.pngimage.hpp>
		#include <Vcl.Imaging.jpeg.hpp>
		#include <Vcl.Imaging.GIFImg.hpp>
	#endif
#endif

#include <locale>
//#include <WinNls.h>

//#include "VersionHelpers.h"	// IsWindowsVistaOrGreater()

#include <stdio.h>
#include <string.h>
#include <math.h>

//#include <wininet.h>
#pragma comment (lib, "wininet.lib")

#include "common.h"
#include "DataUnit.h"
#include "Settings.h"
#include "libusb.h"

const float pi = 3.141592653589793238462643f;
const float pi2 = pi * 2;

const float rad_2_deg = 180.0 / M_PI;
const float deg_2_rad = M_PI / 180.0;

#define LIBUSB_FILENAME "libusb-1.0.dll"

// **************************

#define POLY_32 0xEDB88320	// Polynomial

const uint32_t CRC_TABLE_32[] =
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

// **************************

CCommon common;

//#include <lm.h>

CCommon::CCommon()
{
	m_log_file  = NULL;
	m_data_file = NULL;

	setlocale(LC_ALL, "");

	{	// get windows version
		DWORD major;
		DWORD minor;
		if (getWinMajorMinorVersion(major, minor))
		{
			m_windows_ver.printf(L"(v%u.%u)", major, minor);
			m_windows_ver = getWindowsVersionString() + " " + m_windows_ver;
		}
	}

	#if 1
		{  // Windows XP compatible way

			//const LCID lcid = GetSystemDefaultLCID();
			const LCID lcid_user = GetUserDefaultLCID();

			char name[32];
			ZeroMemory(name, sizeof(name));
			//const int i = GetLocaleInfoEx(LOCALE_SYSTEM_DEFAULT, LOCALE_SISO639LANGNAME, name, sizeof(name));
			//const int i = GetLocaleInfoEx(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, name, sizeof(name));
			const int i = GetLocaleInfo(lcid_user, LOCALE_SISO639LANGNAME, name, sizeof(name));
			if (i > 0)
			{
				name[i - 1] = '-';
//				name[i]     = '\0';
				//const int k = GetLocaleInfoEx(LOCALE_SYSTEM_DEFAULT, LOCALE_SISO3166CTRYNAME, &name[i], sizeof(name) - i);
				//const int k = GetLocaleInfoEx(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, &name[i], sizeof(name) - i);
				const int k = GetLocaleInfo(lcid_user, LOCALE_SISO3166CTRYNAME, &name[i], sizeof(name) - i);
				if (k > 0)
					m_local_name = String(name);
				else
					m_local_name = String(name) + "??";
			}
			else
			{
				m_local_name = "??-??";
			}
		}
	#else
		{  // Windows XP none-compatible way
			#if 0
				OSVERSIONINFOEXW osvi = {sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0};
				const DWORDLONG cm1 = VerSetConditionMask(  0, VER_MAJORVERSION,     VER_GREATER_EQUAL);
				const DWORDLONG cm2 = VerSetConditionMask(cm1, VER_MINORVERSION,     VER_GREATER_EQUAL);
				const DWORDLONG cm3 = VerSetConditionMask(cm2, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
				osvi.dwMajorVersion    = HIBYTE(_WIN32_WINNT_VISTA);
				osvi.dwMinorVersion    = LOBYTE(_WIN32_WINNT_VISTA);
				osvi.wServicePackMajor = 0;
				if (VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, cm3) != FALSE)
			#endif
				{
					wchar_t name[LOCALE_NAME_MAX_LENGTH] = {0};
					if (GetUserDefaultLocaleName(name, ARRAY_SIZE(name)) > 0)
						m_local_name = String(name);
				}
		}
	#endif

	// ******************************************

	if (!load_libuSB(LIBUSB_FILENAME))
		load_libuSB(cleanFilename(IncludeTrailingPathDelimiter(ExtractFilePath(Application->ExeName)) + LIBUSB_FILENAME, true, true));
}

CCommon::~CCommon()
{
	unloadLIBUSB();

	logFileClose();
	dataFileClose();
}

bool __fastcall CCommon::getWinMajorMinorVersion(DWORD &major, DWORD &minor)
{
	major = 0;
	minor = 0;

	memset(&m_wsi, 0, sizeof(WKSTA_INFO_100));

	LPBYTE pinfoRawData = 0;
	if (NERR_Success == ::NetWkstaGetInfo(NULL, 100, &pinfoRawData))
	{
		memcpy(&m_wsi, pinfoRawData, sizeof(WKSTA_INFO_100));

		major = m_wsi.wki100_ver_major;
		minor = m_wsi.wki100_ver_minor;

		::NetApiBufferFree(pinfoRawData);
		return true;
	}

	return false;
}

String __fastcall CCommon::getWindowsVersionString()
{
	String          winver = "unknown";
	OSVERSIONINFOEX osver;
	SYSTEM_INFO     sysInfo;
	typedef void(__stdcall *GETSYSTEMINFO) (LPSYSTEM_INFO);

//	__pragma(warning(push))
//	__pragma(warning(disable:4996))

	memset(&osver, 0, sizeof(osver));
	osver.dwOSVersionInfoSize = sizeof(osver);
	GetVersionEx((LPOSVERSIONINFO)&osver);

//	__pragma(warning(pop))

	DWORD major;
	DWORD minor;

	if (getWinMajorMinorVersion(major, minor))
	{
		osver.dwMajorVersion = major;
		minor = minor;
	}
	else
	if (osver.dwMajorVersion == 6 && minor == 2)
	{
		OSVERSIONINFOEXW osvi;
		ULONGLONG cm = 0;
		cm = VerSetConditionMask(cm, VER_MINORVERSION, VER_EQUAL);
		ZeroMemory(&osvi, sizeof(osvi));
		osvi.dwOSVersionInfoSize = sizeof(osvi);
		osvi.dwMinorVersion = 3;
		if (VerifyVersionInfoW(&osvi, VER_MINORVERSION, cm))
			minor = 3;
	}

	GETSYSTEMINFO getSysInfo = (GETSYSTEMINFO)GetProcAddress(GetModuleHandle("kernel32.dll"), "GetNativeSystemInfo");
	if (getSysInfo == NULL)
		getSysInfo = ::GetSystemInfo;
	getSysInfo(&sysInfo);

	if (osver.dwMajorVersion == 10)
	{
		if (minor >= 0)
		{
			if (osver.wProductType != VER_NT_WORKSTATION)
				winver = "Win-10 Server";
			else
				winver = "Win-10";
		}
	}
	else
	if (osver.dwMajorVersion == 6)
	{
		if (minor == 3)
		{
			if (osver.wProductType != VER_NT_WORKSTATION)
				winver = "Win-Server 2012 R2";
			else
				winver = "Win-8.1";
		}
		else
		if (minor == 2)
		{
			if (osver.wProductType != VER_NT_WORKSTATION)
				winver = "Win-Server 2012";
			else
				winver = "Win-8";
		}
		else
		if (minor == 1)
		{
			if (osver.wProductType != VER_NT_WORKSTATION)
				winver = "Win-Server 2008 R2";
			else
				winver = "Win-7";
		}
		else
		if (minor == 0)
		{
			if (osver.wProductType != VER_NT_WORKSTATION)
				winver = "Win-Server 2008";
			else
				winver = "Win-Vista";
		}
	}
	else
	if (osver.dwMajorVersion == 5)
	{
		if (minor == 2)
		{
			if (osver.wProductType == VER_NT_WORKSTATION && sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				winver = "Win-XP x64";
			else
				winver = "Win-Server 2003";
		}
		else
		if (minor == 1)
			winver = "Win-XP";
		else
		if (minor == 0)
			winver = "Win-2k";
	}

	if (osver.wServicePackMajor != 0)
	{
		String s;
		s.printf(L" SP-%hd", osver.wServicePackMajor);
		winver += s;
	}

	return winver;
}

bool __fastcall CCommon::isWinXP()
{
	return (m_wsi.wki100_ver_major == 5 && m_wsi.wki100_ver_minor == 1) ? true : false;
}

bool __fastcall CCommon::load_libuSB(String filename)
{
	m_libusb_filename = "";
	m_libusb_version  = "";

	int res = loadLIBUSB(AnsiString(filename).c_str());
	if (res < 0)
	{
		unloadLIBUSB();
		return false;
	}

	struct libusb_version *libusb_ver;
	try
	{
		libusb_ver = (struct libusb_version *)libusb_get_version();
	}
	catch (...)
	{
		libusb_ver = NULL;
	}

	if (libusb_ver == NULL)
	{
		unloadLIBUSB();
		return false;
	}

	m_libusb_filename = filename;
	m_libusb_version.printf(L" v%d.%d.%d.%d", libusb_ver->major, libusb_ver->minor, libusb_ver->micro, libusb_ver->nano);
	m_libusb_version = ExtractFileName(filename) + m_libusb_version;

	return true;
}

// **************************

void __fastcall CCommon::logFileAppend(String s)
{
	if (s.IsEmpty())
		return;

	if (m_log_file == NULL)
	{
		m_log_filename = "log.txt";
		//m_log_file = fopen(AnsiString(m_log_filename).c_str(), "wt+");	// new file
		m_log_file = fopen(AnsiString(m_log_filename).c_str(), "at+");		// append file
	}

	if (m_log_file)
	{
		fputs(AnsiString(s).c_str(), m_log_file);
//		fflush(m_log_file);
	}
}

void __fastcall CCommon::logFileClose()
{
	if (m_log_file)
	{
		fflush(m_log_file);
		fclose(m_log_file);
	}
	m_log_file = NULL;
}

void __fastcall CCommon::logFileDelete()
{
	logFileClose();

	if (!m_log_filename.IsEmpty())
		remove(AnsiString(m_log_filename).c_str());
}

void __fastcall CCommon::dataFileAppend(const void *data, const int size)
{
	if (!data || size <= 0)
		return;

	if (m_data_file == NULL)
	{
		m_data_filename = "log.bin";
		//m_data_file = fopen(AnsiString(m_data_filename).c_str(), "wb+");   // new file
		m_data_file = fopen(AnsiString(m_data_filename).c_str(), "ab+");		// append to file
	}

	if (m_data_file)
	{
		fwrite(data, 1, size, m_data_file);
//		fflush(m_data_file);
	}
}

void __fastcall CCommon::dataFileClose()
{
	if (m_data_file)
	{
		fflush(m_data_file);
		fclose(m_data_file);
	}
	m_data_file = NULL;
}

void __fastcall CCommon::dataFileDelete()
{
	dataFileClose();

	if (!m_data_filename.IsEmpty())
		remove(AnsiString(m_data_filename).c_str());
}

// **************************

char __fastcall CCommon::decimalPoint()
{
	#if (__BORLANDC__ < 0x0600)
		return DecimalSeparator;
	#else
		//return std::use_facet< std::numpunct<char> >(std::cout.getloc()).decimal_point();
		return FormatSettings.DecimalSeparator;
	#endif
}

// **************************

bool __fastcall CCommon::GetBuildInfo(String filename, TVersion *version)
{
	DWORD ver_info_size;
	char *ver_info;
	UINT buffer_size;
	LPVOID buffer;
	DWORD dummy;

	if (version == NULL || filename.IsEmpty())
		return false;

	memset(version, 0, sizeof(TVersion));

	ver_info_size = ::GetFileVersionInfoSizeW(filename.w_str(), &dummy);
	if (ver_info_size == 0)
		return false;

	ver_info = new char [ver_info_size];
	if (ver_info == NULL)
		return false;

	if (::GetFileVersionInfoW(filename.w_str(), 0, ver_info_size, ver_info) == FALSE)
	{
		delete [] ver_info;
		return false;
	}

	if (::VerQueryValue(ver_info, _T("\\"), &buffer, &buffer_size) == FALSE)
	{
		delete [] ver_info;
		return false;
	}

	PVSFixedFileInfo ver = (PVSFixedFileInfo)buffer;
	version->MajorVer   = (ver->dwFileVersionMS >> 16) & 0xFFFF;
	version->MinorVer   = (ver->dwFileVersionMS >>  0) & 0xFFFF;
	version->ReleaseVer = (ver->dwFileVersionLS >> 16) & 0xFFFF;
	version->BuildVer   = (ver->dwFileVersionLS >>  0) & 0xFFFF;

	delete [] ver_info;

	return true;
}

void __fastcall CCommon::comboBoxAutoWidth(TComboBox *comboBox)
{
	if (!comboBox)
		return;

	#define COMBOBOX_HORIZONTAL_PADDING	4

	int itemsFullWidth = comboBox->Width;

	// get the max needed with of the items in dropdown state
	for (int i = 0; i < comboBox->Items->Count; i++)
	{
		int itemWidth = comboBox->Canvas->TextWidth(comboBox->Items->Strings[i]);
		itemWidth += 2 * COMBOBOX_HORIZONTAL_PADDING;
		if (itemsFullWidth < itemWidth)
			itemsFullWidth = itemWidth;
	}

	if (comboBox->DropDownCount < comboBox->Items->Count)
		itemsFullWidth += ::GetSystemMetrics(SM_CXVSCROLL);

	::SendMessage(comboBox->Handle, CB_SETDROPPEDWIDTH, itemsFullWidth, 0);
}

bool __fastcall CCommon::createPath(const char *path)
{
	DWORD attr;
	int len;
	char path_str[MAX_PATH];
	bool result = true;

	if (path == NULL)
		return false;

	len = strlen(path);
	if (len <= 0)
		return false;
	if (len > (int)sizeof(path_str) - 1)
		len = (int)sizeof(path_str) - 1;

	memset(path_str, 0, sizeof(path_str));
	memcpy(path_str, path, len);

	// remove trailing slashes
	while (len > 0 && (path_str[len - 1] == '\\'))
		path_str[--len] = '\0';

	if (path_str[0] == '\0')
		return true;

	attr = ::GetFileAttributes(path_str);
	if (0xFFFFFFFF == attr)		// folder doesn't exist yet - create it!
	{
		int i = len - 1;
		while (i > 0 && path_str[i] != '\\' && path_str[i] != ':')
			i--;
		if (i > 0)
		{	// create parent folders
			char c = path_str[i];
			if (c != ':')
			{
				path_str[i] = '\0';
				result = createPath(path_str);
				path_str[i] = c;
			}
		}
		// create folder
		if (path_str[len - 1] != ':')
			result = (result && ::CreateDirectory(path_str, NULL)) ? true : false;
	}
	else
	if ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{  // something already exists, but is not a folder
		::SetLastError(ERROR_FILE_EXISTS);
		result = false;
	}

	return result;
}

String __fastcall CCommon::getLastErrorStr(DWORD err)
{
	String str(_T("unknown error"));
	char *buf = NULL;

	WORD prevErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);	// ignore critical errors

	HMODULE wnet_handle = ::GetModuleHandleA("wininet.dll");

	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM;
	if (wnet_handle)
		flags |= FORMAT_MESSAGE_FROM_HMODULE;		// retrieve message from specified DLL

	::FormatMessageA(flags, wnet_handle, err, 0, (LPSTR)&buf, 0, NULL);
//	DWORD res = ::FormatMessageA(flags, wnet_handle, err, 0, (LPSTR)&buf, 0, NULL);

	if (wnet_handle)
		::FreeLibrary(wnet_handle);

	if (buf)
	{
		str.printf(L"[%d] %s", err, buf);
		::LocalFree(buf);
	}

	::SetErrorMode(prevErrorMode);

	return str;
}

int __fastcall CCommon::getLastErrorStr(DWORD err, void *err_str, int max_size)
{
	int str_len = 0;

	if (!err_str || max_size <= 0)
		return str_len;

	memset(err_str, 0, max_size);

	char *buf = NULL;

	WORD prevErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);	// ignore critical errors

	HMODULE wnet_handle = ::GetModuleHandleA("wininet.dll");

	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM;
	if (wnet_handle)
		flags |= FORMAT_MESSAGE_FROM_HMODULE;		// retrieve message from specified DLL

	DWORD res = ::FormatMessageA(flags, wnet_handle, err, 0, (LPSTR)&buf, 0, NULL);

	if (wnet_handle)
		::FreeLibrary(wnet_handle);

	if (buf != NULL)
	{
		#if defined(__BORLANDC__) && (__BORLANDC__ < 0x0600)
			if (res > 0)
				str_len = sprintf((char *)err_str, "[%d] %s", err, buf);
		#else
			if (res > 0)
				str_len = sprintf_s((char *)err_str, max_size - 1, "[%d] %s", err, buf);
		#endif

		::LocalFree(buf);
	}

	::SetErrorMode(prevErrorMode);

	return str_len;
}

int __fastcall CCommon::popcount32(int32_t v)
{
	v = v - ((v >> 1) & 0x55555555);					// put count of each 2 bits into those 2 bits
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);		// put count of each 4 bits into those 4 bits
	return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}

void __fastcall CCommon::putBitsLS(void *out_bits, uint32_t bits, int num_bits)
{
	if (out_bits == 0 || num_bits <= 0 || num_bits > 32)
		return;

	uint8_t *out = (uint8_t *)out_bits;

	for (int i = 0; i < num_bits; i++)
		*out++ = (bits >> i) & 1;
}

void __fastcall CCommon::putBitsMS(void *out_bits, uint32_t bits, int num_bits)
{
	if (out_bits == 0 || num_bits <= 0 || num_bits > 32)
		return;

	uint8_t *out = (uint8_t *)out_bits;

	for (int i = num_bits - 1; i >= 0; i--)
		*out++ = (bits >> i) & 1;
}

uint32_t __fastcall CCommon::getBitsLS(void *in_bits, int num_bits)
{
	if (in_bits == 0 || num_bits <= 0 || num_bits > 32)
		return 0;

	uint8_t *in = (uint8_t *)in_bits;

	uint32_t bits = 0;
	for (int i = 0; i < num_bits; i++)
		bits |= (uint32_t)(*in++ & 1) << i;

	return bits;
}

uint32_t __fastcall CCommon::getBitsMS(void *in_bits, int num_bits)
{
	if (in_bits == 0 || num_bits <= 0 || num_bits > 32)
		return 0;

	uint8_t *in = (uint8_t *)in_bits;

	uint32_t bits = 0;
	for (int i = num_bits - 1; i >= 0; i--)
		bits |= (uint32_t)(*in++ & 1) << i;

	return bits;
}

void __fastcall CCommon::bytesToBitsLS(void *in_bytes, void *out_bits, int num_bits)
{
	if (in_bytes == 0 || out_bits == 0 || num_bits <= 0)
		return;

	uint8_t *in = (uint8_t *)in_bytes;
	uint8_t *out = (uint8_t *)out_bits;

	while (num_bits > 0)
	{
		uint8_t b = *in++;
		for (int k = 0; k < 8 && num_bits > 0; k++, num_bits--)
			*out++ = (b >> k) & 1;
	}
}

void __fastcall CCommon::bitsToBytesLS(void *in_bits, void *out_bytes, int num_bits)
{
	if (in_bits == 0 || out_bytes == 0 || num_bits <= 0)
		return;

	uint8_t *in = (uint8_t *)in_bits;
	uint8_t *out = (uint8_t *)out_bytes;

	while (num_bits > 0)
	{
		uint8_t b = 0;
		for (int k = 0; k < 8 && num_bits > 0; k++, num_bits--)
			b |= (*in++ & 1) << k;
		*out++ = b;
	}
}

void __fastcall CCommon::bytesToBitsMS(void *in_bytes, void *out_bits, int num_bits)
{
	if (in_bytes == 0 || out_bits == 0 || num_bits <= 0)
		return;

	uint8_t *in = (uint8_t *)in_bytes;
	uint8_t *out = (uint8_t *)out_bits;

	while (num_bits > 0)
	{
		uint8_t b = *in++;
		for (int k = 7; k >= 0 && num_bits > 0; k--, num_bits--)
			*out++ = (b >> k) & 1;
	}
}

void __fastcall CCommon::bitsToBytesMS(void *in_bits, void *out_bytes, int num_bits)
{
	if (in_bits == 0 || out_bytes == 0 || num_bits <= 0)
		return;

	uint8_t *in = (uint8_t *)in_bits;
	uint8_t *out = (uint8_t *)out_bytes;

	while (num_bits > 0)
	{
		uint8_t b = 0;
		for (int k = 7; k >= 0 && num_bits > 0; k--, num_bits--)
			b |= (*in++ & 1) << k;
		*out++ = b;
	}
}

void __fastcall CCommon::reverseByteBits(void *out_bits, void *in_bits, int num_bits)
{
	if (in_bits == 0 || out_bits == 0 || num_bits <= 0)
		return;

	uint8_t *in = (uint8_t *)in_bits;
	uint8_t *out = (uint8_t *)out_bits;

	while (num_bits > 0)
	{
		int num = num_bits;
		if (num > 8)
			num = 8;
		register uint8_t b = 0;
		for (int i = num - 1; i >= 0; i--)
			b = (b << 1) | (*in++ & 1);
		for (int i = num - 1; i >= 0; i--, b >>= 1)
			*out++ = b & 1;
		num_bits -= num;
	}
}

uint8_t __fastcall CCommon::bitReverse8(uint8_t x)
{
	x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
	x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
	x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
	return x;
}

uint16_t __fastcall CCommon::bitReverse16(uint16_t x)
{
	x = ((x >> 1) & 0x5555) | ((x << 1) & 0xaaaa);
	x = ((x >> 2) & 0x3333) | ((x << 2) & 0xcccc);
	x = ((x >> 4) & 0x0f0f) | ((x << 4) & 0xf0f0);
	x = ((x >> 8) & 0x00ff) | ((x << 8) & 0xff00);
	return x;
}

uint32_t __fastcall CCommon::bitReverse32(uint32_t x)
{
	x = ((x >> 1)  & 0x55555555) | ((x << 1)  & 0xaaaaaaaa);
	x = ((x >> 2)  & 0x33333333) | ((x << 2)  & 0xcccccccc);
	x = ((x >> 4)  & 0x0f0f0f0f) | ((x << 4)  & 0xf0f0f0f0);
	x = ((x >> 8)  & 0x00ff00ff) | ((x << 8)  & 0xff00ff00);
	x = ((x >> 16) & 0x0000ffff) | ((x << 16) & 0xffff0000);
	return x;
}
/*
__inline int __fastcall CCommon::parity8(register uint8_t x)
{
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return (x & 1);
}

__inline int __fastcall CCommon::parity16(register uint16_t x)
{
	x ^= x >> 8;
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return (x & 1);
}

__inline int __fastcall CCommon::parity32(register uint32_t x)
{
	x ^= x >> 16;
	x ^= x >> 8;
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return (x & 1);
}

__inline int __fastcall CCommon::parity64(register uint64_t x)
{
	x ^= x >> 32;
	return parity32((uint32_t)x);
}
*/
// return a parity bit.
// if the parity is even then return '0', otherwise return '1'
uint8_t __fastcall CCommon::parity32(uint32_t cw)
{
	register uint8_t p;

	// XOR the bytes of the codeword
	p  = *((uint8_t *)&cw + 0);
	p ^= *((uint8_t *)&cw + 1);
	p ^= *((uint8_t *)&cw + 2);

	// XOR the halves of the intermediate result
	p ^= p >> 4;
	p ^= p >> 2;
	p ^= p >> 1;

	// return the parity result
	return (p & 1);
}

String __fastcall CCommon::localiseDecimalPoint(String s)
{
	const char dp = decimalPoint();

	for (int i = 1; i <= s.Length(); i++)
		if (s[i] == ',' || s[i] == '.')
			s[i] = dp;

	return s;
}

int __fastcall CCommon::parseString(String s, String separator, std::vector <String> &params)
{
	params.resize(0);

	// replace any tabs with spaces
	while (true)
	{
		const int pos = s.Pos('\t');
		if (pos <= 0)
			break;
		s[pos] = ' ';
	}

	s = s.Trim();

	while (!s.IsEmpty())
	{
		const int m = s.Pos(separator);
		if (m < 1)
		{
			params.push_back(s);
			s = "";
		}
		else
		if (m == 1)
		{
			s = s.SubString(m + separator.Length(), s.Length()).Trim();
		}
		else
		{
			const String s2 = s.SubString(1, m - 1).Trim();
			s = s.SubString(m + separator.Length(), s.Length()).Trim();
			params.push_back(s2);
		}
	}

	return params.size();
}

String __fastcall CCommon::freqToStrMHz(double Hz)
{
	String s;
	s.printf(L"%0.6f", Hz / 1e6);
	return trimTrailingZeros(s);
}

String __fastcall CCommon::freqToStr1(double Hz, bool trim_trailing_zero, bool space_units, int fraction_size, bool show_sign)
{
	String s1;
	String s2;
	String s3;

	const bool sign = (Hz < 0) ? true : false;

	Hz = fabs(Hz);

	if (Hz == 0)
	{
		if (!show_sign)
			return "0";
		else
			return sign ? "-0" : "+0";
	}

	if (Hz >= 1e9)
	{
		Hz *= 1e-9;
		s1 = "G";
	}
	else
	if (Hz >= 1e6)
	{
		Hz *= 1e-6;
		s1 = "M";
	}
	else
	if (Hz >= 1e3)
	{
		Hz *= 1e-3;
		s1 = "k";
	}
	else
	{
		//Hz *= 1e0;
		s1 = "";
	}

	if (fraction_size < 1)
		fraction_size = 1;

	s3.printf(L"%%.%df", fraction_size);
	s2.printf(s3.c_str(), Hz);

	if (trim_trailing_zero && (s2.Pos('.') > 0 || s2.Pos(',') > 0))
	{
		while (!s2.IsEmpty() && s2[s2.Length()] == '0')
			s2 = s2.SubString(1, s2.Length() - 1).Trim();
		if (!s2.IsEmpty() && (s2[s2.Length()] == '.' || s2[s2.Length()] == ','))
			s2 = s2.SubString(1, s2.Length() - 1).Trim();
	}

	if (show_sign)
		s2 = sign ? "-" + s2 : "+" + s2;

	return space_units ? s2 + " " + s1 : s2 + s1;
}

String __fastcall CCommon::freqToStr2(int64_t Hz, const int left_padding)
{
	String s;

//	if (Hz < 0)
//		Hz = -Hz;

	if (Hz == 0)
		return "0";

	s.printf(L"%lld", Hz);
	while (s.Length() < left_padding)
		s = " " + s;

	int i = s.Length();
	while (i > 0)
	{
		i -= 2;
		if (i > 0)
		{
			s.Insert(' ', i);
			i--;
		}
	}

	return s;
}

String __fastcall CCommon::secsToStr(double time, bool trim_trailing_zero)
{
	String s1;
	String s2;

	time = fabs(time);

	if (time == 0)
		return "0";

	if (time >= 1e0)
	{
		time *= 1e0;
		s1 = "s";
	}
	else
	if (time >= 1e-3)
	{	// milli
		time *= 1e3;
		s1 = "ms";
	}
	else
	if (time >= 1e-6)
	{	// micro
		time *= 1e6;
		s1 = "us";
	}
	else
	if (time >= 1e-9)
	{	// nano
		time *= 1e9;
		s1 = "ns";
	}
	else
	if (time >= 1e-12)
	{	// pico
		time *= 1e12;
		s1 = "ps";
	}
	else
	if (time >= 1e-15)
	{	// femto
		time *= 1e15;
		s1 = "fs";
	}
	else
	if (time >= 1e-18)
	{	// atto
		time *= 1e18;
		s1 = "as";
	}
	else
	if (time >= 1e-21)
	{	// zepto
		time *= 1e21;
		s1 = "zs";
	}
	else
	{	// yocto
		time *= 1e24;
		s1 = "ys";
	}

	s2.printf(L"%5.3f", time);

	if (trim_trailing_zero && (s2.Pos('.') > 0 || s2.Pos(',') > 0))
	{
		while (!s2.IsEmpty() && s2[s2.Length()] == '0')
			s2 = s2.SubString(1, s2.Length() - 1).Trim();
		if (!s2.IsEmpty() && (s2[s2.Length()] == '.' || s2[s2.Length()] == ','))
			s2 = s2.SubString(1, s2.Length() - 1).Trim();
	}

	return s2 + s1;
}

String __fastcall CCommon::distToStr(double distance, bool trim_trailing_zero)
{
	String s1;
	String s2;

	distance = fabs(distance);

	if (distance == 0)
		return "0";

	if (distance >= 1e0)
	{
		distance *= 1e0;
		s1 = "m";
	}
	else
	if (distance >= 1e-3)
	{
		distance *= 1e3;
		s1 = "mm";
	}
	else
	if (distance >= 1e-6)
	{
		distance *= 1e6;
		s1 = "um";
	}
	else
	if (distance >= 1e-9)
	{
		distance *= 1e9;
		s1 = "nm";
	}
	else
	{
		distance *= 1e12;
		s1 = "pm";
	}

	s2.printf(L"%5.3f", distance);

	if (trim_trailing_zero && (s2.Pos('.') > 0 || s2.Pos(',') > 0))
	{
		while (!s2.IsEmpty() && s2[s2.Length()] == '0')
			s2 = s2.SubString(1, s2.Length() - 1).Trim();
		if (!s2.IsEmpty() && (s2[s2.Length()] == '.' || s2[s2.Length()] == ','))
			s2 = s2.SubString(1, s2.Length() - 1).Trim();
	}

	return s2 + s1;
}

String __fastcall CCommon::trimTrailingZeros(String s)
{
	const char dp = decimalPoint();

	// replace any tabs with spaces
	while (true)
	{
		const int pos = s.Pos('\t');
		if (pos <= 0)
			break;
		s[pos] = ' ';
	}

	s = s.Trim();
	while (s.Length() > 1 && s.Pos(dp) > 0)
	{
		const char c = s[s.Length()];
		if (c != '0' && c != dp)
			break;
		s = s.SubString(1, s.Length() - 1).Trim();
	}
	while (!s.IsEmpty() && s[s.Length()] == dp)
		s = s.SubString(1, s.Length() - 1).Trim();
//	if (!s.IsEmpty() && s[s.Length()] == dp)
//		s += '0';
/*
	s = s.Trim();
	while (s.Length() > 1 && (s.Pos('.') > 0 || s.Pos(',') > 0))
	{
		const char c = s[s.Length()];
		if (c != '0' && (c != '.' || c != ','))
			break;
		s = s.SubString(1, s.Length() - 1).Trim();
	}
	while (!s.IsEmpty() && (s[s.Length()] == '.' || s[s.Length()] == ','))
		s = s.SubString(1, s.Length() - 1).Trim();
//	if (!s.IsEmpty() && (s[s.Length()] == '.' || s[s.Length()] == ','))
//		s += '0';
*/
	return s;
}
/*
uint32_t __fastcall CCommon::updateCRC32(uint32_t crc, const void *data, const int len)
{	// bit bang
	#ifndef CRC_REVERSE
		const uint8_t *p = (const uint8_t *)data;
		for (int i = 0; i < len; i++)
		{
			const uint32_t val = crc >> 8;
			crc = (crc & 0x000000ff) ^ *p++;
			for (int k = 0; k < 8; k++)
				crc = (crc & 1) ? (crc >> 1) ^ POLY32 : crc >> 1;
			crc ^= val;
		}
	#else
		// STM32 normal CRC (not reversed)
		const uint32_t *p = (const uint32_t *)data;
		for (int i = 0; i < (len >> 2); i++)
		{
			crc ^= *p++;
			for (int k = 0; k < 32; k++)
				crc = (crc & 0x80000000) ? (crc << 1) ^ POLY32_REV : crc << 1;
		}
		return crc;
	#endif
}
*/
uint32_t __fastcall CCommon::updateCRC32(uint32_t crc, uint8_t b)
{
	return ((crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ b]);
}

uint32_t __fastcall CCommon::updateCRC32(uint32_t crc, void *data, int data_len)
{
	uint8_t *p = (uint8_t *)data;

	while (data_len >= 8)
	{
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		data_len -= 8;
	}

	while (data_len >= 4)
	{
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		data_len -= 4;
	}

	while (data_len >= 2)
	{
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		data_len -= 2;
	}

	while (data_len >= 1)
	{
		crc = (crc >> 8) ^ CRC_TABLE_32[(crc & 0xff) ^ *p++];
		data_len--;
	}

	return crc;
}

int __fastcall CCommon::resourceSize(String name)
{
	const HMODULE handle = GetModuleHandle(NULL);
	if (handle == NULL)
		return -1;

	const HRSRC res_info = FindResource(handle, AnsiString(name).c_str(), RT_RCDATA);
	if (res_info == NULL)
		return -2;

	const HGLOBAL res_data = LoadResource(handle, res_info);
	if (res_data == NULL)
		return -3;

	const DWORD dwSize = SizeofResource(handle, res_info);

	return (int)dwSize;
}

int __fastcall CCommon::fetchResource(String name, std::vector <uint8_t> &buffer)
{
	buffer.resize(0);

	const HMODULE handle = GetModuleHandle(NULL);
	if (handle == NULL)
		return -1;

	const HRSRC res_info = FindResource(handle, AnsiString(name).c_str(), RT_RCDATA);
	if (res_info == NULL)
		return -2;

	const HGLOBAL res_data = LoadResource(handle, res_info);
	if (res_data == NULL)
		return -3;

	const DWORD dwSize = SizeofResource(handle, res_info);
	if (dwSize == 0)
		return -4;

	const VOID *p_res_data = LockResource(res_data);
	if (p_res_data == NULL)
		return -5;

	buffer.resize(dwSize);

	memmove(&buffer[0], p_res_data, dwSize);

	return (int)dwSize;
}

int __fastcall CCommon::loadFile(const String name, std::vector <uint8_t> &buffer)
{
	buffer.resize(0);

	const int file_handle = FileOpen(name, fmOpenRead | fmShareDenyNone);
	if (file_handle <= 0)
		return -1;

	const int file_size = FileSeek(file_handle, 0, 2);
	FileSeek(file_handle, 0, 0);

	if (file_size <= 0)
	{
		FileClose(file_handle);
		return -2;
	}

	buffer.resize(file_size);

	const int bytes_loaded = FileRead(file_handle, &buffer[0], file_size);

	FileClose(file_handle);

	if (bytes_loaded != (int)buffer.size())
	{
		buffer.resize(0);
		return -3;
	}

	return (int)buffer.size();
}

int __fastcall CCommon::loadFile(const String name, std::vector <String> &buffer)
{
	buffer.resize(0);

	// ******************************

	#if (__BORLANDC__ < 0x0600)
		FILE *fin = fopen(AnsiString(name).c_str(), "rb");
	#else
		FILE *fin = NULL;
		int res = fopen_s(&fin, AnsiString(name).c_str(), "rb");
		if (res != 0 || fin == NULL)
		{
			if (fin != NULL)
				fclose(fin);
			fin = NULL;
		}
	#endif

	if (fin == NULL)
		return -1;

	if (fseek(fin, 0, SEEK_END) != 0)
	{
		fclose(fin);
		return -2;
	}

	const int file_size = (int)ftell(fin);
	if (fseek(fin, 0, SEEK_SET) != 0 || file_size <= 0)
	{
		fclose(fin);
		return -2;
	}

	std::vector <char> buffer2(file_size);

	if (fread(&buffer2[0], 1, buffer2.size(), fin) != buffer2.size())
	{
		fclose(fin);
		return -3;
	}

	fclose(fin);

/*
	const int file_handle = FileOpen(name, fmOpenRead | fmShareDenyNone);
	if (file_handle <= 0)
		return -1;

	const int file_size = FileSeek(file_handle, 0, 2);
	FileSeek(file_handle, 0, 0);

	if (file_size <= 0)
	{
		FileClose(file_handle);
		return -2;
	}

	std::vector <char> buffer2(file_size);

	const int bytes_loaded = FileRead(file_handle, &buffer2[0], file_size);

	FileClose(file_handle);

	if (bytes_loaded != (int)buffer2.size())
		return -3;
*/
	// ******************************

	{	// replace tabs with spaces
		for (unsigned int i = 0; i < buffer2.size(); i++)
			if (buffer2[i] == '\t')
				buffer2[i] = ' ';
	}

	{	// extract each line
		UTF8String s;
		unsigned int i = 0;
		while (i < buffer2.size())
		{
			const char c = buffer2[i++];
			if (c == '\r' || c == '\n')
			{
				buffer.push_back(s);
				s = "";

				if (c == '\r' && (i+1) < buffer2.size())
					if (buffer2[i+1] == '\n')
						i++;	// skip over the following '\n'
			}
      else
				s += c;
		}

		if (!s.IsEmpty())
			buffer.push_back(s);
	}

	return (int)buffer.size();
}

int __fastcall CCommon::saveFile(const String name, std::vector <uint8_t> &buffer)
{
	const int file_handle = FileCreate(name);
	if (file_handle <= 0)
		return -1;

	const int bytes_written = FileWrite(file_handle, &buffer[0], buffer.size());

	FileClose(file_handle);

	if (bytes_written != (int)buffer.size())
	{
		DeleteFile(name);
		return -2;
	}

	return (int)buffer.size();
}

int __fastcall CCommon::saveFile(const String name, std::vector <String> &buffer)
{
	int saved = 0;

	const int file_handle = FileCreate(name);
	if (file_handle <= 0)
		return -1;

	for (unsigned int i = 0; i < buffer.size(); i++)
	{
		UTF8String s = UTF8String(buffer[i]) + "\r\n";
		const int bytes_written = FileWrite(file_handle, s.c_str(), s.Length());
		if (bytes_written != s.Length())
		{
			FileClose(file_handle);
			DeleteFile(name);
			return -2;
		}
		saved += bytes_written;
	}

	FileClose(file_handle);

	return saved;
}

int __fastcall CCommon::saveFileAnsi(const String name, std::vector <String> &buffer)
{
	int saved = 0;

	const int file_handle = FileCreate(name);
	if (file_handle <= 0)
		return -1;

	for (unsigned int i = 0; i < buffer.size(); i++)
	{
		AnsiString s = AnsiString(buffer[i]) + "\r\n";
		const int bytes_written = FileWrite(file_handle, s.c_str(), s.Length());
		if (bytes_written != s.Length())
		{
			FileClose(file_handle);
			DeleteFile(name);
			return -2;
		}
		saved += bytes_written;
	}

	FileClose(file_handle);

	return saved;
}

String __fastcall CCommon::cleanFilename(String filename, const bool contains_dir, const bool trim)
{
	String drive;
	String dir;
	String ext;

	const String bad_chars = ",/*|?<>:\"";

	const String bad_name[] =
	{
		"CON",  "PRN",  "AUX",  "NUL",
		"COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
		"LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
	};

	if (trim)
		filename = filename.Trim();
	if (filename.IsEmpty())
		return filename;

	if (contains_dir)
	{
		drive = ExtractFileDrive(filename);

		// remove the drive from file name
		if (!drive.IsEmpty())
			filename = filename.SubString(1 + drive.Length(), filename.Length() - drive.Length());

		dir = ExtractFileDir(filename);

		// remove the directory from file name
		if (!dir.IsEmpty())
			filename = filename.SubString(1 + dir.Length(), filename.Length() - dir.Length());
	}

	// replace any invalid filename characters with spaces
	for (int i = 1; i <= filename.Length(); i++)
	{
		if (filename[i] < 32)
			filename[i] = ' ';
		else
		if (bad_chars.Pos(filename[i]) > 0)
			filename[i] = ' ';
	}
	if (trim)
		filename = filename.Trim();

	ext = ExtractFileExt(filename);

	// remove the file extension from file name
	if (!ext.IsEmpty())
		filename = filename.SubString(1, filename.Length() - ext.Length());

	// certain filenames are not allowed
	for (unsigned int i = 0; i < ARRAY_SIZE(bad_name); i++)
	{
		if (filename.LowerCase() == bad_name[i].LowerCase())
		{
			filename = "";
			break;
		}
	}

	if (filename.IsEmpty())
		return "";

	filename += ext;

	if (trim)
	{	// more cleany stuff
		int i = 1;
		while (i <= filename.Length())
		{
			int k = filename.Pos("__");
			if (k < 1)
				k = filename.Pos("  ");
			if (k >= 1)
				filename = filename.Delete(k, 1).Trim();
			else
				i++;
		}
	}

	if (contains_dir)
	{
		while (!dir.IsEmpty())
		{
			const int i = dir.Pos("\\.\\");
			if (i <= 0)
				break;
			dir = dir.Delete(i, 2).Trim();
		}
	}

	return drive + dir + filename;
 }

String __fastcall CCommon::padLeft(String s, int size)
{
	if (size > 256)
		size = 256;

	while (s.Length() < size)
		s = " " + s;

	return s;
}

String __fastcall CCommon::padRight(String s, int size)
{
	if (size > 256)
		size = 256;

	while (s.Length() < size)
		s += " ";

	return s;
}

bool __fastcall CCommon::strToValue(String s, double &value, double multiplier)
{
	int i;
	double d;
	String s2;

	value = 0;

	const char dp = decimalPoint();

	s = localiseDecimalPoint(s.Trim());

	// remove any spaces and control chars
	i = 1;
	while (i <= s.Length())
	{
		if (s[i] <= 32)
			s = s.Delete(i, 1).Trim();
		else
			i++;
	}

	// remove all unwanted pre-numeral characters
	while (!s.IsEmpty())
	{
		const char c = s[1];
		if ((c >= '0' && c <= '9') || c == dp || c == '+' || c == '-')
			break;
		s = s.SubString(2, s.Length() - 1).Trim();
	}

	// look for the end of the numerals (and the maybe following unit char)
	i = 1;
	if (i <= s.Length())
		if (s[i] == '+' || s[i] == '-')
			i++;
	while (i <= s.Length())
	{
		const char c = s[i];
		if ((c < '0' || c > '9') && c != dp)
			break;
		i++;
	}

	if (--i <= 0)
		return false;  // no numerics found

	// extract the numerals
	s2 = s.SubString(1, i).Trim();

	// extract trailing text .. may contain a unit specifier
	if (i < s.Length())
		s = s.SubString(1 + i, s.Length()).Trim();
	else
		s = "";

	// convert to a value
	if (!TryStrToFloat(s2, d))
		return false;

	// determine the scale from the maybe supplied unit specifier (case sensitive)
	if (!s.IsEmpty())
	{
		switch (s[1])
		{
			case 'Y':	// yotta
				multiplier = 1e24;
				break;
			case 'Z':	// zetta
				multiplier = 1e21;
				break;
			case 'E':	// exa
				multiplier = 1e18;
				break;
			case 'P':	// peta
				multiplier = 1e15;
				break;
			case 'T':	// tera
				multiplier = 1e12;
				break;
			case 'G':   // giga
				multiplier = 1e9;
				break;
			case 'M':	// mega
				multiplier = 1e6;
				break;
			case 'k':	// kilo
			case 'K':
				multiplier = 1e3;
				break;
			case 'H':   // units .. 'H' canused with frequency text
				multiplier = 1e0;
				break;
			case 'm':	// milli
				multiplier = 1e-3;
				break;
			case 'u':	// micro
				multiplier = 1e-6;
				break;
			case 'n':	// nano
				multiplier = 1e-9;
				break;
			case 'p':	// pico
				multiplier = 1e-12;
				break;
			case 'f':	// femto
				multiplier = 1e-15;
				break;
			case 'a':	// atto
				multiplier = 1e-18;
				break;
			case 'z':	// zepto
				multiplier = 1e-21;
				break;
			case 'y':	// yocto
				multiplier = 1e-24;
				break;
			default:    // unknown
				return false;
		}
	}

	// scale according to multiplier
	value = d * multiplier;

	return true;
}

bool __fastcall CCommon::strToHz(String s, double &Hz, double default_multiplier)
{
	return strToValue(s, Hz, default_multiplier);
}

bool __fastcall CCommon::strTokHz(String s, double &kHz)
{
	const bool res = strToHz(s, kHz, 1e3);
	kHz *= 1e-3;
	return res;
}

bool __fastcall CCommon::strToMHz(String s, double &MHz)
{
	const bool res = strToHz(s, MHz, 1e6);
	MHz *= 1e-6;
	return res;
}

bool __fastcall CCommon::strToGHz(String s, double &GHz)
{
	const bool res = strToHz(s, GHz, 1e9);
	GHz *= 1e-9;
	return res;
}

bool __fastcall CCommon::strToTHz(String s, double &THz)
{
	const bool res = strToHz(s, THz, 1e12);
	THz *= 1e-12;
	return res;
}

String __fastcall CCommon::valueToStr(double value, const bool space_units, const bool trim_trailing_zeros, String fmt, const bool decimal_units)
{
	String units;

	fmt = fmt.Trim();

	const bool sign = (value < 0.0) ? true : false;
	value = fabs(value);

	if (value == 0.0)
	{	// zero
		units = "";
	}
/*	else
	if (value < 1e-21)
	{	// yocto
		value *= 1e24;
		units = "y";
	}
	else
	if (value >= 1e-18)
	{	// zepto
		value *= 1e21;
		units = "z";
	}
	else
	if (value < 1e-15)
	{	// atto
		value *= 1e18;
		units = "a";
	}
*/	else
	if (value < 1e-12)
	{	// femto
		value *= 1e15;
		units = "f";
	}
	else
	if (value < 1e-9)
	{	// pico
		value *= 1e12;
		units = "p";
	}
	else
	if (value < 1e-6)
	{	// nano
		value *= 1e9;
		units = "n";
	}
	else
	if (value < 1e-3)
	{	// micro
		value *= 1e6;
		units = "u";
	}
	else
	if (value < 1e0 && !decimal_units)
	{	// milli
		value *= 1e3;
		units = "m";
	}
	else
	if (value < 1e3)
	{	// units
		value *= 1e0;
		units = "";
	}
	else
	if (value < 1e6)
	{	// kilo
		value *= 1e-3;
		units = "k";
	}
	else
	if (value < 1e9)
	{	// mega
		value *= 1e-6;
		units = "M";
	}
	else
	if (value < 1e12)
	{	// giga
		value *= 1e-9;
		units = "G";
	}
	else
	if (value < 1e15)
	{	// tera
		value *= 1e-12;
		units = "T";
	}
	else
	if (value < 1e18)
	{	// peta
		value *= 1e-15;
		units = "P";
	}
	else
	if (value < 1e21)
	{	// exa
		value *= 1e-18;
		units = "E";
	}
	else
	if (value < 1e24)
	{	// zetta
		value *= 1e-21;
		units = "Z";
	}
	else
	if (value < 1e27)
	{	// zetta
		value *= 1e-24;
		units = "Z";
	}
	else
	if (value < 1e30)
	{	// yotta
		value *= 1e-27;
		units = "Y";
	}

	if (sign)
		value = -value;

	String s;
	if (fmt.IsEmpty())
		s.printf(L"%#.5g", value);
	else
		s.printf(fmt.c_str(), value);

	if (trim_trailing_zeros)
	{
		const char dp = decimalPoint();
		if (s.Pos(dp) > 0)
		{
			while (!s.IsEmpty() && s[s.Length()] == '0')
				s = s.SubString(1, s.Length() - 1).Trim();
			if (!s.IsEmpty() && s[s.Length()] == dp)
				s = s.SubString(1, s.Length() - 1).Trim();
		}
	}

	return (space_units) ? s + " " + units : s + units;
}

String __fastcall CCommon::loadSParams(std::vector <t_data_point> &s_params, String filename)
{
	s_params.resize(0);

	filename = filename.Trim();

	String name = filename;

	if (filename.IsEmpty())
	{	// open a dialog box for the user to select a file to read in

		if (m_wsi.wki100_ver_major <= 5)   // windows xp
		{
			TOpenDialog *dialog = new TOpenDialog(Application->MainForm);
			if (dialog == NULL)
				return "";

			dialog->Title = "Load an s-param file from ..";

			dialog->Filter = "s1p Files|*.s1p|s2p Files|*.s2p|snp Files|*.s*p|All files|*.*";
			dialog->FilterIndex = 3;

			dialog->Options.Clear();
			dialog->Options = dialog->Options << ofHideReadOnly << ofPathMustExist << ofFileMustExist << ofEnableSizing;

			String path = ExcludeTrailingPathDelimiter(ExtractFilePath(Application->ExeName));
			while (!path.IsEmpty())
			{
				const int i = path.Pos("\\.\\");
				if (i > 0)
					path = path.Delete(i, 2).Trim();
				else
					break;
			}
			dialog->InitialDir = path;

			Application->NormalizeTopMosts();
			const bool ok = dialog->Execute();
			Application->RestoreTopMosts();

			name = dialog->FileName;
			dialog->Free();

			if (!ok)
				return "";
		}
		else
		{
			TFileOpenDialog *dialog = new TFileOpenDialog(Application->MainForm);
			if (dialog == NULL)
				return "";

			dialog->Title = "Load an s-param file from ..";

			TFileTypeItem *item;
			dialog->FileTypes->Clear();
			item = dialog->FileTypes->Add(); item->DisplayName = "s1p Files"; item->FileMask = "*.s1p";
			item = dialog->FileTypes->Add(); item->DisplayName = "s2p Files"; item->FileMask = "*.s2p";
			item = dialog->FileTypes->Add(); item->DisplayName = "snp Files"; item->FileMask = "*.s*p";
			item = dialog->FileTypes->Add(); item->DisplayName = "All Files"; item->FileMask = "*.*";

			dialog->FileTypeIndex = 3;
//			dialog->DefaultExtension = "*.s*p";

			dialog->Options.Clear();
			dialog->Options = dialog->Options << fdoPathMustExist << fdoFileMustExist;

			String path = ExcludeTrailingPathDelimiter(ExtractFilePath(Application->ExeName));
			while (!path.IsEmpty())
			{
				const int i = path.Pos("\\.\\");
				if (i > 0)
					path = path.Delete(i, 2).Trim();
				else
					break;
			}
			dialog->DefaultFolder = path;

			dialog->OkButtonLabel = "Load file";

			Application->NormalizeTopMosts();
			const bool ok = dialog->Execute();
			Application->RestoreTopMosts();

			name = dialog->FileName;
			dialog->Free();

			if (!ok)
				return "";
		}
	}

	String ext  = ExtractFileExt(name).LowerCase();

	int num_chans = -1;
	if (ext == ".s1p") num_chans = 1;
	else
	if (ext == ".s2p") num_chans = 2;

	if (num_chans < 1 || num_chans > 2)
	{
		if (filename.IsEmpty())
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"Only S1P or S2P extensions", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
		return "";
	}

	std::vector <String> lines;

	{
		String s = "Loading \"" + name + "\" ..";
		//Memo1->Lines->Add(s);

		const int size = loadFile(name, lines);
		if (size <= 0)
		{
			switch (size)
			{
				case -1:
					s = "could not open the file";
					//Memo1->Lines->Add(s);
					if (filename.IsEmpty())
					{
						Application->NormalizeTopMosts();
						Application->MessageBoxA(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
						Application->RestoreTopMosts();
					}
					return "";

				case -2:
					s = "file is too small";
					//Memo1->Lines->Add(s);
					if (filename.IsEmpty())
					{
						Application->NormalizeTopMosts();
						Application->MessageBoxA(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
						Application->RestoreTopMosts();
					}
					return "";

				case -3:
					s = "error reading the file";
					//Memo1->Lines->Add(s);
					if (filename.IsEmpty())
					{
						Application->NormalizeTopMosts();
						Application->MessageBoxA(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
						Application->RestoreTopMosts();
					}
					return "";

				default:
					return "";
			}
		}

		//Memo1->Lines->Add("loaded OK");
		//Memo1->Lines->Add("");
	}

	unsigned int line = 0;

	bool db_format    = false;
	bool ma_format    = false;
	bool ri_format    = false;
	int64_t freq_mult = 0;
	float ref_imp     = 50.0f;

	// look for ..
	//
	// # hz s ri r 50
	// # hz s ma r 50
	// # hz s db r 50.00
	//
	while (line < lines.size())
	{
		float d;

		String s = lines[line++].LowerCase();
		if (s.IsEmpty())
			continue;

		if (s[1] == '!')
			continue;      // comment marker

//		s = localiseDecimalPoint(s);

		std::vector <String> params;
		parseString(s, " ", params);

		if (params.size() < 6)
			continue;

		if (params[0] != "#")
			continue;
//		if (params[1].Pos("hz") <= 0)
//			continue;
		if (params[2] != "s")
			continue;
		if (params[4] != "r")
			continue;

		ref_imp = 0.0f;
		if (!TryStrToFloat(params[5], d))
			continue;
		if (IROUND(d) != 50)
			continue;
		ref_imp = d;

		freq_mult = 0;
		if (params[1] ==  "hz") freq_mult = 1;
		else
		if (params[1] == "khz") freq_mult = 1000;
		else
		if (params[1] == "mhz") freq_mult = 1000000;
		else
		if (params[1] == "ghz") freq_mult = 1000000000;

		db_format = (params[3] == "db") ? true : false;
		ma_format = (params[3] == "ma") ? true : false;
		ri_format = (params[3] == "ri") ? true : false;

		if (freq_mult > 0 && ref_imp > 0.0f && (db_format || ma_format || ri_format))
			break;
	}

	if (freq_mult <= 0 || ref_imp <= 0.0f || !(ma_format || ri_format || db_format) || line >= lines.size())
	{
		if (filename.IsEmpty())
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"File error. \"# hz s ri r 50\" not found", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
		return "";
	}

	while (line < lines.size())
	{
		String s = lines[line++].LowerCase();

		{	// remove everything that follows the comment '!' indicator (including the indicator itself)
			const int i = s.Pos('!');
			if (i > 0)
				s = s.Delete(i, s.Length()).Trim();
		}

		if (s.IsEmpty())
			continue;

		if (s[1] == '#')
//			continue;
			break;

		s = localiseDecimalPoint(s);

		std::vector <String> params;
		parseString(s, " ", params);

		if (params.size() < 1)
		{
			String s3;
			s3.printf(L"Format error on line %u\r\n\n\"%s\"", line, s.c_str());
			if (filename.IsEmpty())
			{
				Application->NormalizeTopMosts();
				Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
			}
			return "";
		}

		double freq = 0;
		float s11_real = 0;
		float s11_imag = 0;
		float s21_real = 0;
		float s21_imag = 0;

		if (!TryStrToFloat(params[0], freq) || freq <= 0)
		{
            continue; //skip lines with invalid frequency
			String s3;
			s3.printf(L"Frequency error on line %u\r\n\n\"%s\"", line, s.c_str());
			if (filename.IsEmpty())
			{
				Application->NormalizeTopMosts();
				Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
			}
			return "";
		}

		freq *= freq_mult;

		if (num_chans == 1)
		{
			//   50000 +0.997205257416 -0.000746459002
			//	9049500 +0.997326910496 -0.008030370809

			if (params.size() < 3)
			{
				String s3;
				s3.printf(L"Format error on line %u\r\n\n\"%s\"", line, s.c_str());
				if (filename.IsEmpty())
				{
					Application->NormalizeTopMosts();
					Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
				}
				return "";
			}

			if (!TryStrToFloat(params[1], s11_real))
			{
				String s3;
				s3.printf(L"S11.real error on line %u\r\n\n\"%s\"", line, s.c_str());
				if (filename.IsEmpty())
				{
					Application->NormalizeTopMosts();
					Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
				}
				return "";
			}

			if (!TryStrToFloat(params[2], s11_imag))
			{
				String s3;
				s3.printf(L"S11.imag error on line %u\r\n\n\"%s\"", line, s.c_str());
				if (filename.IsEmpty())
				{
					Application->NormalizeTopMosts();
					Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
				}
				return "";
			}
		}
		else
		if (num_chans == 2)
		{
			//   50000 +0.997205257416 -0.000746459002 -0.000119486998 +0.000624242995 0 0 0 0
			//	9049500 +0.997326910496 -0.008030370809 +0.000366301014 +0.000065216002 0 0 0 0

			if (params.size() < 5)
			{
				String s3;
				s3.printf(L"Format error on line %u\r\n\n\"%s\"", line, s.c_str());
				if (filename.IsEmpty())
				{
					Application->NormalizeTopMosts();
					Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
				}
				return "";
			}

			if (!TryStrToFloat(params[1], s11_real))
			{
				String s3;
				s3.printf(L"S11.real error on line %u\r\n\n\"%s\"", line, s.c_str());
				if (filename.IsEmpty())
				{
					Application->NormalizeTopMosts();
					Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
				}
				return "";
			}

			if (!TryStrToFloat(params[2], s11_imag))
			{
				String s3;
				s3.printf(L"S11.imag error on line %u\r\n\n\"%s\"", line, s.c_str());
				if (filename.IsEmpty())
				{
					Application->NormalizeTopMosts();
					Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
				}
				return "";
			}

			if (!TryStrToFloat(params[3], s21_real))
			{
				String s3;
				s3.printf(L"S21.real error on line %u\r\n\n\"%s\"", line, s.c_str());
				if (filename.IsEmpty())
				{
					Application->NormalizeTopMosts();
					Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
				}
				return "";
			}

			if (!TryStrToFloat(params[4], s21_imag))
			{
				String s3;
				s3.printf(L"S21.imag error on line %u\r\n\n\"%s\"", line, s.c_str());
				if (filename.IsEmpty())
				{
					Application->NormalizeTopMosts();
					Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
				}
				return "";
			}
		}

		if (db_format || ma_format)
		{
			float mag;
			float ang;

			if (num_chans >= 1)
			{  //	S11
				mag = 0;
				ang = s11_imag * deg_2_rad;

				if (db_format)
					mag = powf(10, s11_real / 20);   // dB to absolute
				else
				if (ma_format)
					mag = s11_real;

				// convert from mag/ang to re/im
				s11_real = cosf(ang) * mag;
				s11_imag = sinf(ang) * mag;
			}

			if (num_chans >= 2)
			{  //	S21
				mag = 0;
				ang = s21_imag * deg_2_rad;

				if (db_format)
					mag = powf(10, s21_real / 20);   // dB to absolute
				else
				if (ma_format)
					mag = s21_real;

				// convert from mag/ang to re/im
				s21_real = cosf(ang) * mag;
				s21_imag = sinf(ang) * mag;
			}
		}

		t_data_point fp;
		fp.Hz  = I64ROUND(freq);
		fp.s11 = complexf (s11_real, s11_imag);
		fp.s21 = complexf (s21_real, s21_imag);

		s_params.push_back(fp);
	}

	// ensure the points are sorted according to increasing frequency
	if (s_params.size() > 1)
	{
		bool sorted = true;

		int64_t f1 = s_params[0].Hz;
		for (int i = 1; i < (int)s_params.size(); i++)
		{
			const int64_t f2 = s_params[i].Hz;
			if (f1 > f2)
			{
				sorted = false;
				break;
			}
			f1 = f2;
		}

		if (!sorted)
		{	// sort them
			for (int i = 0; i < (int)s_params.size() - 1; i++)
			{
				int64_t f1 = s_params[i].Hz;
				for (int k = i + 1; k < (int)s_params.size(); k++)
				{
					const int64_t f2 = s_params[k].Hz;
					if (f1 > f2)
					{	// swap
						const t_data_point fp = s_params[i];
						s_params[i] = s_params[k];
						s_params[k] = fp;
						f1 = f2;
					}
				}
			}
		}
	}

	if (s_params.empty())
	{
		if (!filename.IsEmpty())
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"No s-points found", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
		return "";
	}

	return name;
}

bool __fastcall CCommon::saveSParams(std::vector <t_data_point> &points, int num_chans, String filename)
{
	filename = filename.Trim();

	if (points.empty())
	{
		if (filename.IsEmpty())
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"No data to save", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
		return false;
	}

	if (num_chans < 1 || num_chans > MAX_CHANNELS)
	{
		if (filename.IsEmpty())
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"Only S1P or S2P saving", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
		return false;
	}

	// remove/replace any invalid filename characters
	String name = cleanFilename(filename, true, true);

	if (name.IsEmpty())
	{
		name = data_unit.m_vna_data.name.LowerCase() + "_" + FormatDateTime("yyyy-mm-dd_hh-nn-ss", Now());
		switch (num_chans)
		{
			case 1: name += ".s1p"; break;
			case 2: name += ".s2p"; break;
		}
		name = cleanFilename(name, false, true);

		if (m_wsi.wki100_ver_major <= 5)   // windows xp
		{
			TSaveDialog *dialog = new TSaveDialog(Application->MainForm);
			if (dialog == NULL)
				return "";

			dialog->Title = "Save the SNP file to ..";

			dialog->Filter = "s1p Files|*.s1p|s2p Files|*.s2p|snp Files|*.s*p|All Files|*.*";
			dialog->FilterIndex = (num_chans <= 1) ? 1 : 2;

			dialog->Options.Clear();
			dialog->Options = dialog->Options << ofHideReadOnly << ofPathMustExist << ofFileMustExist << ofEnableSizing;

			String path = ExcludeTrailingPathDelimiter(ExtractFilePath(Application->ExeName));
			while (!path.IsEmpty())
			{
				const int i = path.Pos("\\.\\");
				if (i > 0)
					path = path.Delete(i, 2).Trim();
				else
					break;
			}
			dialog->InitialDir = path;

			dialog->FileName = ExtractFileName(name);

			Application->NormalizeTopMosts();
			const bool ok = dialog->Execute();
			Application->RestoreTopMosts();

			name = dialog->FileName;
			dialog->Free();

			if (!ok)
				return "";
		}
		else
		{
			TFileSaveDialog *dialog = new TFileSaveDialog(Application->MainForm);
			if (dialog == NULL)
				return false;

			dialog->Title = "Save the SNP file to ..";

			TFileTypeItem *item;
			dialog->FileTypes->Clear();
			item = dialog->FileTypes->Add(); item->DisplayName = "s1p Files"; item->FileMask = "*.s1p";
			item = dialog->FileTypes->Add(); item->DisplayName = "s2p Files"; item->FileMask = "*.s2p";
			item = dialog->FileTypes->Add(); item->DisplayName = "snp Files"; item->FileMask = "*.s*p";
			item = dialog->FileTypes->Add(); item->DisplayName = "All Files"; item->FileMask = "*.*";

			dialog->FileTypeIndex = (num_chans <= 1) ? 1 : 2;
//			dialog->DefaultExtension = (num_chans <= 1) ? "*.s1p" : "*.s2p";

			dialog->Options.Clear();
			dialog->Options = dialog->Options << fdoPathMustExist;

			String path = ExcludeTrailingPathDelimiter(ExtractFilePath(Application->ExeName));
			while (!path.IsEmpty())
			{
				const int i = path.Pos("\\.\\");
				if (i > 0)
					path = path.Delete(i, 2).Trim();
				else
					break;
			}
			dialog->DefaultFolder = path;

			dialog->FileName = ExtractFileName(name);

			dialog->OkButtonLabel = "Save file";

			Application->NormalizeTopMosts();
			const bool ok = dialog->Execute();
			Application->RestoreTopMosts();

			name = dialog->FileName;
			dialog->Free();

			if (!ok)
				return false;
		}
	}

	String ext = ExtractFileExt(name).LowerCase();

	if (ext.IsEmpty())
	{
		switch (num_chans)
		{
			case 1: name += ".s1p"; break;
			case 2: name += ".s2p"; break;
			case 3: name += ".s2p"; break;
			case 4: name += ".s2p"; break;
		}
		ext = ExtractFileExt(name).LowerCase();
	}

	if (name.IsEmpty())
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Invalid filename characters", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return false;
	}

	String s;
	std::vector <String> buffer;
  
	s = "! " + common.title;
	buffer.push_back(s);

	s = "! Date: " + FormatDateTime("yyyy-mm-dd hh:nn:ss", Now());
	buffer.push_back(s);

	s.printf(L"! Start: %sHz Stop: %sHz Points: %u", valueToStr(points[0].Hz, false, true).c_str(), valueToStr(points[points.size() - 1].Hz, false, true).c_str(), points.size());
	buffer.push_back(s);

	s = (num_chans <= 1) ? "! S-Parameter data: F S11" : "! S-Parameter data: F S11 S21 S12 S22";
	buffer.push_back(s);

//	s = "# Hz S DB R 50.0";
//	s = "# HZ S MA R 50";
	s = "# Hz S RI R 50";
	buffer.push_back(s);

	if (num_chans <= 1)
	{
		//s = "!     Frequency           S11_dB              S11_Deg";
		//s = "!     Frequency           S11_Mag             S11_Ang";
		s = "!     Frequency           S11_Real           S11_Imag";
	}
	else
	{
		//s = "!     Frequency           S11_dB              S11_Deg            S21_dB             S21_Deg            S12_dB             S12_Deg            S22_dB             S22_Deg";
		//s = "!     Frequency           S11_Mag             S11_Ang            S21_Mag            S21_Ang            S12_Mag            S12_Ang            S22_Mag            S22_Ang";
		s = "!     Frequency           S11_Real           S11_Imag           S21_Real           S21_Imag           S12_Real           S12_Imag           S22_Real           S22_Imag";
	}
	buffer.push_back(s);

  //make a "deep copy" of current locale name.
//#include <cstddef>
  std::string prev_loc = std::setlocale(LC_ALL,NULL);
  // set numeric decilmal for Touchstone
  std::setlocale(LC_NUMERIC, "en_US.UTF-8");

	for (unsigned int i = 0; i < points.size(); i++)
	{
		String str;
		t_data_point *p = &points[i];
		str.printf(L"%lld", p->Hz);
		s = padLeft(str, 15);
		for (int k = 0; k < num_chans; k++)
		{
			const complexf cpx = p->sParam[k];
			str.printf(L"%0.9e", cpx.real());
			s += " " + padLeft(str, 18);
			str.printf(L"%0.9e", cpx.imag());
			s += " " + padLeft(str, 18);
		}
		buffer.push_back(s);
	}
  //restore the previous locale.
  std::setlocale(LC_ALL, prev_loc.c_str());

	const int res = saveFileAnsi(name, buffer);
	if (res <= 0)
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Failed to save the file", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return false;
	}

	return true;
}

String __fastcall CCommon::loadCSV(std::vector <t_data_point> &points, String filename)
{
	points.resize(0);

	filename = filename.Trim();

	String name = filename;

	if (filename.IsEmpty())
	{	// open a dialog box for the user to select a file to read in

		if (m_wsi.wki100_ver_major <= 5)   // windows xp
		{
			TOpenDialog *dialog = new TOpenDialog(Application->MainForm);
			if (dialog == NULL)
				return "";

			dialog->Title = "Load CSV file from ..";

			dialog->Filter = "CSV Files|*.csv|All files|*.*";
			dialog->FilterIndex = 1;

			dialog->Options.Clear();
			dialog->Options = dialog->Options << ofHideReadOnly << ofPathMustExist << ofFileMustExist << ofEnableSizing;

			String path = ExcludeTrailingPathDelimiter(ExtractFilePath(Application->ExeName));
			while (!path.IsEmpty())
			{
				const int i = path.Pos("\\.\\");
				if (i > 0)
					path = path.Delete(i, 2).Trim();
				else
					break;
			}
			dialog->InitialDir = path;

			Application->NormalizeTopMosts();
			const bool ok = dialog->Execute();
			Application->RestoreTopMosts();

			name = dialog->FileName;
			dialog->Free();

			if (!ok)
				return "";
		}
		else
		{
			TFileOpenDialog *dialog = new TFileOpenDialog(Application->MainForm);
			if (dialog == NULL)
				return "";

			dialog->Title = "Load CSV file from ..";

			TFileTypeItem *item;
			dialog->FileTypes->Clear();
			item = dialog->FileTypes->Add(); item->DisplayName = "CSV Files"; item->FileMask = "*.csv";
			item = dialog->FileTypes->Add(); item->DisplayName = "All Files"; item->FileMask = "*.*";

			dialog->FileTypeIndex = 1;
//			dialog->DefaultExtension = "*.csv";

			dialog->Options.Clear();
			dialog->Options = dialog->Options << fdoPathMustExist << fdoFileMustExist;

			String path = ExcludeTrailingPathDelimiter(ExtractFilePath(Application->ExeName));
			while (!path.IsEmpty())
			{
				const int i = path.Pos("\\.\\");
				if (i > 0)
					path = path.Delete(i, 2).Trim();
				else
					break;
			}
			dialog->DefaultFolder = path;

			dialog->OkButtonLabel = "Load file";

			Application->NormalizeTopMosts();
			const bool ok = dialog->Execute();
			Application->RestoreTopMosts();

			name = dialog->FileName;
			dialog->Free();

			if (!ok)
				return "";
      }
	}

	String ext  = ExtractFileExt(name).LowerCase();

	std::vector <String> lines;

	{
		String s = "Loading \"" + name + "\" ..";
		//Memo1->Lines->Add(s);

		const int size = loadFile(name, lines);
		if (size <= 0)
		{
			switch (size)
			{
				case -1:
					s = "could not open the file";
					//Memo1->Lines->Add(s);
					if (filename.IsEmpty())
					{
						Application->NormalizeTopMosts();
						Application->MessageBoxA(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
						Application->RestoreTopMosts();
					}
					return "";

				case -2:
					s = "file is too small";
					//Memo1->Lines->Add(s);
					if (filename.IsEmpty())
					{
						Application->NormalizeTopMosts();
						Application->MessageBoxA(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
						Application->RestoreTopMosts();
					}
					return "";

				case -3:
					s = "error reading the file";
					//Memo1->Lines->Add(s);
					if (filename.IsEmpty())
					{
						Application->NormalizeTopMosts();
						Application->MessageBoxA(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
						Application->RestoreTopMosts();
					}
					return "";

				default:
					return "";
			}
		}

		//Memo1->Lines->Add("loaded OK");
		//Memo1->Lines->Add("");
	}


	// https://en.wikipedia.org/wiki/Comma-separated_values
	// basically CSV files are free-for-all - no proper standard
	String field_sep = ";";

	unsigned int line = 0;

	//int64_t freq_mult = 0;

	while (line < lines.size())
	{
		String s = lines[line++].Trim().LowerCase();

		{	// remove everything that follows the comment '#' indicator (including the indicator itself)
//			const int i = s.Pos('#');
//			if (i > 0)
//				s = s.Delete(i, s.Length()).Trim();
		}

		if (s.IsEmpty())	// empty line
			continue;
		if (s[1] == '#')	// skip comment lines
			continue;

		std::vector <String> params;
		parseString(s, field_sep, params);

		if (params.size() < 1)
		{
			String s3;
			s3.printf(L"Format error on line %u\r\n\n\"%s\"", line, s.c_str());
			if (filename.IsEmpty())
			{
				Application->NormalizeTopMosts();
				Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
			}
			return "";
		}

		if (params[0][1] == '"')
		{	// re-combine any parsed up params that were located within a pair of quotes
			int k = 0;
			while (k < (int)params.size())
			{
				const int m = k;
				String s1 = params[k++];
				if (s1.IsEmpty())
					continue;
				if (s1[1] == '\"' && s1[s1.Length()] == '\"' && s1.Length() >= 2)
				{	// simply remove the double quotes
					s1 = s1.SubString(2, s1.Length() - 2).Trim();
					params[k - 1] = s1;
				}
				else
				if (s1[1] == '\"')
				{
					while (k < (int)params.size())
					{
						String s2 = params[k++];
						s1 += " " + s2;
						if (s2[s2.Length()] == '\"')
							break;
					}

					if (s1[1] == '\"' && s1[s1.Length()] == '\"')
					{
						s1 = s1.SubString(2, s1.Length() - 2).Trim();
						while (--k > m)
							params.erase(params.begin() + k);
						params[k++] = s1;
					}
				}
			}
		}

		if (params.size() < 1)
		{
			String s3;
			s3.printf(L"Format error on line %u\r\n\n\"%s\"", line, s.c_str());
			if (filename.IsEmpty())
			{
				Application->NormalizeTopMosts();
				Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
			}
			return "";
		}

		double freq = 0.0;
		double value[4] = {0.0, 0.0, 0.0, 0.0};

		if (!TryStrToFloat(localiseDecimalPoint(params[0]), freq) || freq < 0)
		{
			String s3;
			s3.printf(L"Frequency error on line %u\r\n\n\"%s\"", line, s.c_str());
			if (filename.IsEmpty())
			{
				Application->NormalizeTopMosts();
				Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
			}
			return "";
		}

		for (unsigned int i = 1; i < params.size(); i++)
		{
			double val;
			if (!TryStrToFloat(localiseDecimalPoint(params[i]), val))
			{
				String s3;
				s3.printf(L"Value error on line %u\r\n\n\"%s\"", line, s.c_str());
				if (filename.IsEmpty())
				{
					Application->NormalizeTopMosts();
					Application->MessageBox(s3.w_str(), L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
				}
				return "";
			}
			if (i <= ARRAY_SIZE(value))
				value[i - 1] = val;
		}

		t_data_point fp;
		fp.Hz  = I64ROUND(freq);
		fp.s11 = complexf (value[0], value[1]);
		fp.s21 = complexf (value[2], value[3]);

		points.push_back(fp);
	}

	// ensure the points are sorted according to increasing frequency
	if (points.size() > 1)
	{
		bool sorted = true;

		int64_t f1 = points[0].Hz;
		for (int i = 1; i < (int)points.size(); i++)
		{
			const int64_t f2 = points[i].Hz;
			if (f1 > f2)
			{
				sorted = false;
				break;
			}
			f1 = f2;
		}

		if (!sorted)
		{
			for (int i = 0; i < (int)points.size() - 1; i++)
			{
				int64_t f1 = points[i].Hz;
				for (int k = i + 1; k < (int)points.size(); k++)
				{
					const int64_t f2 = points[k].Hz;
					if (f1 > f2)
					{	// swap
						const t_data_point fp = points[i];
						points[i] = points[k];
						points[k] = fp;
						f1 = f2;
					}
				}
			}
		}
	}

	if (points.empty())
	{
		if (!filename.IsEmpty())
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"No data found", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
		return "";
	}

	return name;
}

bool __fastcall CCommon::saveCSV(std::vector <t_data_point> &points, const int channels, const bool double_quotes, String filename)
{
	filename = filename.Trim();

	if (points.empty())
	{
		if (filename.IsEmpty())
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"No data to save", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
		return false;
	}

	String name = cleanFilename(filename, true, true);

	if (name.IsEmpty())
	{
		name = data_unit.m_vna_data.name.LowerCase() + "_" + FormatDateTime("yyyy-mm-dd_hh-nn-ss", Now()) + ".csv";

		name = cleanFilename(name, false, true);

		if (m_wsi.wki100_ver_major <= 5)   // windows xp
		{
			TSaveDialog *dialog = new TSaveDialog(Application->MainForm);
			if (dialog == NULL)
				return "";

			dialog->Title = "Save the CSV file to ..";

			dialog->Filter = "CSV Files|*.csv|All Files|*.*";
			dialog->FilterIndex = 1;

			dialog->Options.Clear();
			dialog->Options = dialog->Options << ofHideReadOnly << ofPathMustExist << ofFileMustExist << ofEnableSizing;

			String path = ExcludeTrailingPathDelimiter(ExtractFilePath(Application->ExeName));
			while (!path.IsEmpty())
			{
				const int i = path.Pos("\\.\\");
				if (i > 0)
					path = path.Delete(i, 2).Trim();
				else
					break;
			}
			dialog->InitialDir = path;

			dialog->FileName = ExtractFileName(name);

			Application->NormalizeTopMosts();
			const bool ok = dialog->Execute();
			Application->RestoreTopMosts();

			name = dialog->FileName;
			dialog->Free();

			if (!ok)
				return "";
		}
		else
		{
			TFileSaveDialog *dialog = new TFileSaveDialog(Application->MainForm);
			if (dialog == NULL)
				return false;

			dialog->Title = "Save the CSV file to ..";

			TFileTypeItem *item;
			dialog->FileTypes->Clear();
			item = dialog->FileTypes->Add(); item->DisplayName = "CSV Files"; item->FileMask = "*.csv";
			item = dialog->FileTypes->Add(); item->DisplayName = "All Files"; item->FileMask = "*.*";

			dialog->FileTypeIndex = 1;
//			dialog->DefaultExtension = "*.csv";

			dialog->Options.Clear();
			dialog->Options = dialog->Options << fdoPathMustExist;

			String path = ExcludeTrailingPathDelimiter(ExtractFilePath(Application->ExeName));
			while (!path.IsEmpty())
			{
				const int i = path.Pos("\\.\\");
				if (i > 0)
					path = path.Delete(i, 2).Trim();
				else
					break;
			}
			dialog->DefaultFolder = path;

			dialog->FileName = name;

			dialog->OkButtonLabel = "Save file";

			Application->NormalizeTopMosts();
			const bool ok = dialog->Execute();
			Application->RestoreTopMosts();

			name = dialog->FileName;
			dialog->Free();

			if (!ok)
				return false;
		}
	}

	String ext = ExtractFileExt(name).LowerCase();

	if (ext.IsEmpty())
	{
		name += ".csv";
		ext = ExtractFileExt(name).LowerCase();
	}

	if (name.IsEmpty())
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Invalid filename characters", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return false;
	}
/*
	if (ext != ".csv")
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Only CSV extensions", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return false;
	}
*/
	// https://en.wikipedia.org/wiki/Comma-separated_values
	// basically CSV files are free-for-all - no proper standard
	String field_sep = ";";

	String s;
	std::vector <String> buffer;

//	s = "# " + Application->Title;
//	buffer.push_back(s);
//
//	s = "# Date: " + FormatDateTime("yyyy-mm-dd hh:nn:ss", Now());
//	buffer.push_back(s);

	for (unsigned int i = 0; i < points.size(); i++)
	{
		String str;
		t_data_point *p = &points[i];
		str.printf(L"%lld", p->Hz);
		s = padLeft(str, 15);
		for (int k = 0; k < channels; k++)
		{
			const complexf cpx = p->sParam[k];
			s += field_sep;
			str.printf(L"%0.9e", cpx.real());
			if (double_quotes)
				s += " \"" + padLeft(str, 18) + "\"";
			else
				s += " " + padLeft(str, 18);
			s += field_sep;
			str.printf(L"%0.9e", cpx.imag());
			if (double_quotes)
				s += " \"" + padLeft(str, 18) + "\"";
			else
				s += " " + padLeft(str, 18);
		}
		buffer.push_back(s);
	}

	const int res = saveFile(name, buffer);
	if (res <= 0)
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Failed to save the file", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return false;
	}

	return true;
}

void __fastcall CCommon::onFileOpenDialogSelectionChange(TObject *Sender)
{
	TFileOpenDialog *fod = dynamic_cast<TFileOpenDialog *>(Sender);
	if (fod == NULL)
		return;

//	std::vector <t_calibration> calibration;

//	String filename = common.loadCalibrationFile(fod->FileName, calibration);
//	if (filename.IsEmpty() || calibration.empty())
//		return;




}

String __fastcall CCommon::loadCalibrationFile(String filename, std::vector <t_calibration_point> &cal)
{
	String s;

	cal.resize(0);

	filename = filename.Trim();

	if (filename.IsEmpty())
	{	// open a dialog box for the user to select a file to read in

		if (m_wsi.wki100_ver_major <= 5)   // windows xp
		{
			TOpenDialog *dialog = new TOpenDialog(Application->MainForm);
			if (dialog == NULL)
				return "";

			dialog->Title = "Load calibration file ..";

			dialog->Filter = "Calibration files|*.cal|All files|*.*";
			dialog->FilterIndex = 1;

			dialog->Options.Clear();
			dialog->Options = dialog->Options << ofHideReadOnly << ofPathMustExist << ofFileMustExist << ofEnableSizing;

			String path = ExcludeTrailingPathDelimiter(ExtractFilePath(Application->ExeName));
			while (!path.IsEmpty())
			{
				const int i = path.Pos("\\.\\");
				if (i > 0)
					path = path.Delete(i, 2).Trim();
				else
					break;
			}
			dialog->InitialDir = path;

			Application->NormalizeTopMosts();
			const bool ok = dialog->Execute();
			Application->RestoreTopMosts();

			filename = dialog->FileName;
			dialog->Free();

			if (!ok)
				return "";
		}
		else
		{
			TFileOpenDialog *dialog = new TFileOpenDialog(Application->MainForm);
			if (dialog == NULL)
				return "";

			String path = ExcludeTrailingPathDelimiter(ExtractFilePath(filename));
			if (path.IsEmpty())
				path = ExcludeTrailingPathDelimiter(settings.calibrationFolder);

			dialog->Title = "Load calibration file ..";

			TFileTypeItem *item;
			dialog->FileTypes->Clear();
			item = dialog->FileTypes->Add(); item->DisplayName = "Calibration Files"; item->FileMask = "*.cal";
			item = dialog->FileTypes->Add(); item->DisplayName = "All Files"; item->FileMask = "*.*";

			dialog->FileTypeIndex = 1;
//			dialog->DefaultExtension = "*.cal";

			dialog->Options.Clear();
			dialog->Options = dialog->Options << fdoPathMustExist << fdoFileMustExist;
//			dialog->Options = dialog->Options << fdoPathMustExist << fdoPickFolders;
			//fdoOverWritePrompt
			//fdoStrictFileTypes
			//fdoNoChangeDir
			//fdoPickFolders
			//fdoForceFileSystem
			//fdoAllNonStorageItems
			//fdoNoValidate
			//fdoAllowMultiSelect
			//fdoPathMustExist
			//fdoFileMustExist
			//fdoCreatePrompt
			//fdoShareAware
			//fdoNoReadOnlyReturn
			//fdoNoTestFileCreate
			//fdoHideMRUPlaces
			//fdoHidePinnedPlaces
			//fdoNoDereferenceLinks
			//fdoDontAddToRecent
			//fdoForceShowHidden
			//fdoDefaultNoMiniMode
			//fdoForcePreviewPaneOn

			dialog->OkButtonLabel = "Load file";

			while (!path.IsEmpty())
			{
				const int i = path.Pos("\\.\\");
				if (i > 0)
					path = path.Delete(i, 2).Trim();
				else
					break;
			}
			dialog->DefaultFolder = path;

			dialog->FileName = "";

			dialog->OkButtonLabel = "Load file";

			dialog->OnSelectionChange = onFileOpenDialogSelectionChange;

			Application->NormalizeTopMosts();
			const bool ok = dialog->Execute();
			Application->RestoreTopMosts();

			filename = dialog->FileName;
			dialog->Free();

			if (!ok)
				return "";
      }
	}

	String ext  = ExtractFileExt(filename).LowerCase();

	std::vector <String> lines;

	const int size = loadFile(filename, lines);
	if (size <= 0)
	{
		switch (size)
		{
			case -1:
				s = "could not open the file";
				Application->NormalizeTopMosts();
				Application->MessageBox(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
				return "";

			case -2:
				s = "file is too small";
				Application->NormalizeTopMosts();
				Application->MessageBox(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
				return "";

			case -3:
				s = "error reading the file";
				Application->NormalizeTopMosts();
				Application->MessageBox(s.c_str(), L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
				return "";

			default:
				return "";
		}
	}

	// example file format ..
	//
	// # Calibration data for NanoVNA-Saver
	// #      Hz   ShortR       ShortI      OpenR        OpenI       LoadR        LoadI        ThroughR    ThroughI     IsolationR    IsolationI
	// 130000000 0.001159961 -6.739e-06   0.001140687 -0.000120034 0.00105307  -6.1595e-05  -1.2255e-05  0.000106103 -4.2089e-05    3.3912e-05
	// 130300000 0.001306215 -1.55e-07    0.001153938  4.0331e-05  0.001130376 -3.4638e-05  -4.792e-06  -2.6533e-05   3.08e-07     -6.7229e-05
	// 130600000 0.001059723  0.000177196 0.001055034  0.000185995 0.0010398    0.000152485 -5.5821e-05 -1.1299e-05  -8.4228e-05    8.674e-06
	// 130900000 0.001233188 -2.6927e-05  0.001241182 -3.018e-06   0.001260576  3.3662e-05  -7.9192e-05  0.000118836 -7.3112e-05    6.9583e-05
	// 131200000 0.001100919 -8.6547e-05  0.001138604  3.3133e-05  0.001252498  1.866e-06   -1.9061e-05  8.9938e-05  -1.1058e-05    5.3418e-05

	unsigned int line = 0;

	bool found_desc = false;

	while (line < lines.size())
	{
		const String s1 = lines[line++];
		if (s1.IsEmpty())
			continue;

		String s2;
		s2.printf(L"line %3u", line);

		// parse the text line up
		std::vector <String> params;
		parseString(s1, " ", params);

		for (unsigned int i = 0; i < params.size(); i++)
			params[i] = params[i].LowerCase();

		if (params[0][1] == '#' || params[0][1] == '!')
		{	// comment line

			if (params.size() >= 12)
			{
				if (	params[ 1] == "hz" &&
						params[ 2] == "shortr" &&
						params[ 3] == "shorti" &&
						params[ 4] == "openr" &&
						params[ 5] == "openi" &&
						params[ 6] == "loadr" &&
						params[ 7] == "loadi" &&
						params[ 8] == "throughr" &&
						params[ 9] == "throughi" &&
						params[10] == "isolationr" &&
						params[11] == "isolationi")
				{
					found_desc = true;
				}
			}

			continue;
		}

		double values[1 + (2 * 5)];

		// valid number of params ?
		if (params.size() < ARRAY_SIZE(values))
		{	// no
			continue;
		}

		// convert back to binary
		int errors = 0;
		for (unsigned int i = 0; i < ARRAY_SIZE(values); i++)
		{
			String s = localiseDecimalPoint(params[i]);
			if (!TryStrToFloat(s, values[i]))
			{
				values[i] = 0;
				errors++;
			}
		}
		if (errors > 0)
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(String(s2 + "  error").c_str(), L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			cal.resize(0);
			return "";
		}

		t_calibration_point c;
		c.HzCal        = I64ROUND(values[0]);
		c.shortCal     = complexf (values[1], values[2]);
		c.openCal      = complexf (values[3], values[4]);
		c.loadCal      = complexf (values[5], values[6]);
		c.throughCal   = complexf (values[7], values[8]);
		c.isolationCal = complexf (values[9], values[10]);

		cal.push_back(c);
	}

	// ensure the points are sorted according to increasing frequency
	if (cal.size() > 1)
	{
		bool sorted = true;

		int64_t f1 = cal[0].HzCal;
		for (int i = 1; i < (int)cal.size(); i++)
		{
			const int64_t f2 = cal[i].HzCal;
			if (f1 > f2)
			{
				sorted = false;
				break;
			}
			f1 = f2;
		}

		if (!sorted)
		{
			for (int i = 0; i < (int)cal.size() - 1; i++)
			{
				int64_t f1 = cal[i].HzCal;
				for (int k = i + 1; k < (int)cal.size(); k++)
				{
					const int64_t f2 = cal[k].HzCal;
					if (f1 > f2)
					{	// swap
						const t_calibration_point c = cal[i];
						cal[i] = cal[k];
						cal[k] = c;
						f1 = f2;
					}
				}
			}
		}
	}

	return filename;
}

String __fastcall CCommon::saveCalibrationFile(String filename, std::vector <t_calibration_point> &cal, const bool ask_filename)
{
	String s;

	filename = filename.Trim();

	if (cal.empty())
	{
		if (filename.IsEmpty())
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"No data to save", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
		}
		return "";
	}

	// remove/replace any invalid filename characters
	filename = cleanFilename(filename, true, true);

	if (ask_filename || filename.IsEmpty())
	{
		if (m_wsi.wki100_ver_major <= 5)   // windows xp
		{
			TSaveDialog *dialog = new TSaveDialog(Application->MainForm);
			if (dialog == NULL)
				return "";

			dialog->Title = "Save calibration file ..";

			dialog->Filter = "Calibration Files|*.cal|All Files|*.*";
			dialog->FilterIndex = 1;

			dialog->Options.Clear();
			dialog->Options = dialog->Options << ofHideReadOnly << ofPathMustExist << ofFileMustExist << ofEnableSizing;

			String path = ExcludeTrailingPathDelimiter(ExtractFilePath(filename));
			if (path.IsEmpty())
				path = ExcludeTrailingPathDelimiter(settings.calibrationFolder);

			while (!path.IsEmpty())
			{
				const int i = path.Pos("\\.\\");
				if (i > 0)
					path = path.Delete(i, 2).Trim();
				else
					break;
			}
			dialog->InitialDir = path;

			dialog->FileName = ExtractFileName(filename);

			Application->NormalizeTopMosts();
			const bool ok = dialog->Execute();
			Application->RestoreTopMosts();

			filename = dialog->FileName;
			dialog->Free();

			if (!ok)
				return "";
		}
		else
		{
			TFileSaveDialog *dialog = new TFileSaveDialog(Application->MainForm);
			if (dialog == NULL)
				return "";

			dialog->Title = "Save calibration file ..";

			TFileTypeItem *item;
			dialog->FileTypes->Clear();
			item = dialog->FileTypes->Add(); item->DisplayName = "Calibration Files"; item->FileMask = "*.cal";
			item = dialog->FileTypes->Add(); item->DisplayName = "All Files"; item->FileMask = "*.*";

			dialog->FileTypeIndex = 1;
//			dialog->DefaultExtension = "*.cal";

			dialog->Options.Clear();
			dialog->Options = dialog->Options << fdoPathMustExist;
			//fdoOverWritePrompt
			//fdoStrictFileTypes
			//fdoNoChangeDir
			//fdoPickFolders
			//fdoForceFileSystem
			//fdoAllNonStorageItems
			//fdoNoValidate
			//fdoAllowMultiSelect
			//fdoPathMustExist
			//fdoFileMustExist
			//fdoCreatePrompt
			//fdoShareAware
			//fdoNoReadOnlyReturn
			//fdoNoTestFileCreate
			//fdoHideMRUPlaces
			//fdoHidePinnedPlaces
			//fdoNoDereferenceLinks
			//fdoDontAddToRecent
			//fdoForceShowHidden
			//fdoDefaultNoMiniMode
			//fdoForcePreviewPaneOn

			String path = ExcludeTrailingPathDelimiter(ExtractFilePath(filename));
			if (path.IsEmpty())
				path = ExcludeTrailingPathDelimiter(settings.calibrationFolder);

			while (!path.IsEmpty())
			{
				const int i = path.Pos("\\.\\");
				if (i > 0)
					path = path.Delete(i, 2).Trim();
				else
					break;
			}
			dialog->DefaultFolder = path;

			dialog->FileName = ExtractFileName(filename);

			dialog->OkButtonLabel = "Save file";

			Application->NormalizeTopMosts();
			const bool ok = dialog->Execute();
			Application->RestoreTopMosts();

			filename = dialog->FileName;
			dialog->Free();

			if (!ok)
				return "";
		}
	}

	String ext = ExtractFileExt(filename).LowerCase();

	if (ext.IsEmpty())
	{
		filename += ".cal";
		ext = ExtractFileExt(filename).LowerCase();
	}

	if (filename.IsEmpty())
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Invalid filename characters", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return "";
	}

	// example file format ..
	//
	// # Calibration data for NanoVNA-Saver
	// # Device: NanoVNA-V1
	// # Hz ShortR ShortI OpenR OpenI LoadR LoadI ThroughR ThroughI IsolationR IsolationI
	// 130000000 0.001159961 -6.739e-06   0.001140687 -0.000120034 0.00105307  -6.1595e-05  -1.2255e-05  0.000106103 -4.2089e-05 3.3912e-05
	// 130300000 0.001306215 -1.55e-07    0.001153938  4.0331e-05  0.001130376 -3.4638e-05  -4.792e-06  -2.6533e-05   3.08e-07  -6.7229e-05
	// 130600000 0.001059723  0.000177196 0.001055034  0.000185995 0.0010398    0.000152485 -5.5821e-05 -1.1299e-05  -8.4228e-05 8.674e-06
	// 130900000 0.001233188 -2.6927e-05  0.001241182 -3.018e-06   0.001260576  3.3662e-05  -7.9192e-05  0.000118836 -7.3112e-05 6.9583e-05
	// 131200000 0.001100919 -8.6547e-05  0.001138604  3.3133e-05  0.001252498  1.866e-06   -1.9061e-05  8.9938e-05  -1.1058e-05 5.3418e-05

	std::vector <String> lines;

	lines.push_back("# Calibration data for " + Application->Title);
	lines.push_back("# Date: " + FormatDateTime("yyyy-mm-dd hh:nn:ss", Now()));
	lines.push_back("# Device: " + data_unit.m_vna_data.name);
	lines.push_back("# Points: " + UIntToStr(cal.size()));
	lines.push_back("# Hz ShortR ShortI OpenR OpenI LoadR LoadI ThroughR ThroughI IsolationR IsolationI");

	for (unsigned int i = 0; i < cal.size(); i++)
	{
		String str;
		complexf cpx;

		str.printf(L"%lld", cal[i].HzCal);
		s = padLeft(str, 15);

		cpx = cal[i].shortCal;
		str.printf(L"%0.9e", cpx.real());
		s += " " + padLeft(str, 18);
		str.printf(L"%0.9e", cpx.imag());
		s += " " + padLeft(str, 18);

		cpx = cal[i].openCal;
		str.printf(L"%0.9e", cpx.real());
		s += " " + padLeft(str, 18);
		str.printf(L"%0.9e", cpx.imag());
		s += " " + padLeft(str, 18);

		cpx = cal[i].loadCal;
		str.printf(L"%0.9e", cpx.real());
		s += " " + padLeft(str, 18);
		str.printf(L"%0.9e", cpx.imag());
		s += " " + padLeft(str, 18);

		cpx = cal[i].throughCal;
		str.printf(L"%0.9e", cpx.real());
		s += " " + padLeft(str, 18);
		str.printf(L"%0.9e", cpx.imag());
		s += " " + padLeft(str, 18);

		cpx = cal[i].isolationCal;
		str.printf(L"%0.9e", cpx.real());
		s += " " + padLeft(str, 18);
		str.printf(L"%0.9e", cpx.imag());
		s += " " + padLeft(str, 18);

		lines.push_back(s);
	}

	const int res = saveFileAnsi(filename, lines);
	if (res <= 0)
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Failed to save the file", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return "";
	}

	return filename;
}

void __fastcall CCommon::saveBitmap(Graphics::TBitmap *bm, String ID, String dialog_title, const bool ask_filename)
{
	if (bm == NULL)
		return;

	String filename;

	dialog_title = dialog_title.Trim();

	// remove/replace any invalid filename characters
	filename = common.cleanFilename(filename, false, true);

	if (!ask_filename)
	{

	}

	if (ask_filename || filename.IsEmpty())
	{
		ID = ID.Trim();
		if (!ID.IsEmpty())
			filename += ID;

		if (!filename.IsEmpty())
			filename += "_";

		filename += FormatDateTime("yyyy-mm-dd_hh-nn-ss", Now()) + ".png";

		filename = common.cleanFilename(filename, false, true);

		TSavePictureDialog *dialog = new TSavePictureDialog(Application->MainForm);
		if (dialog == NULL)
			return;

		dialog->Options.Clear();
		dialog->Options = dialog->Options << ofHideReadOnly << ofPathMustExist << ofEnableSizing;

		dialog->Filter = "PNG {*.png}|*.png|BMP {*.bmp}|*.bmp|JPG {*.jpg *.jpeg}|*.jpg;*.jpeg|All files (*.*}|*.*";
		dialog->FilterIndex = 1;

		dialog->DefaultExt = "*.png";

		dialog->Title = dialog_title.IsEmpty() ? String("Save image ..") : dialog_title;
		dialog->FileName = filename;

		Application->NormalizeTopMosts();
		const bool ok = dialog->Execute();
		Application->RestoreTopMosts();

		filename = dialog->FileName;
		dialog->Free();

		if (!ok)
			return;
	}

	String ext = ExtractFileExt(filename).LowerCase();

	if (ext == ".bmp")
	{
		try
		{
			bm->SaveToFile(filename);
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
		return;
	}

	if (ext == ".jpg" || ext == ".jpeg")
	{
		TJPEGImage *jpg;
		try
		{
			jpg = new TJPEGImage;
			if (jpg == NULL)
				return;
			jpg->Assign(bm);
			jpg->CompressionQuality = 100;   // 0 to 100
			jpg->SaveToFile(filename);
		}
		__finally
		{
			if (jpg != NULL)
				delete jpg;
		}
		return;
	}

	if (ext == ".png")
	{
		TPngImage *png;
		try
		{
			png = new TPngImage();
			if (png == NULL)
				return;
			png->Assign(bm);
			png->CompressionLevel = 9;	// 0 to 9
			png->SaveToFile(filename);
		}
		__finally
		{
			if (png != NULL)
				delete png;
		}
		return;
	}

	Application->NormalizeTopMosts();
	Application->MessageBox(L"Only PNG, JPG, JPEG and BMP formats are supported", L"Error", MB_ICONERROR | MB_OK);
	Application->RestoreTopMosts();
}

void __fastcall CCommon::phaseUnwrap(float buffer[], const int size, float step)
{
	if (buffer == NULL || size <= 0)
		return;

	for (int i = 0; i < 2; i++)
	{
		float delta = 0;
		float prev_level = buffer[0];
		for (int k = 0; k < size; k++)
		{
			const float level = buffer[k];
			const float diff  = level - prev_level;
			prev_level = level;
			if (diff >  step) delta -= step * 2;
			else
			if (diff < -step) delta += step * 2;
			buffer[k] = level + delta;
		}
		step /= 2;
	}
}

void __fastcall CCommon::setWarning(TWinControl *control, String warning)
{
	if (control == NULL)
		return;

	TEdit *edit = dynamic_cast<TEdit *>(control);
	TComboBox *combo_box = dynamic_cast<TComboBox *>(control);

	warning = warning.Trim();

	const TColor colour = warning.IsEmpty() ? clWindow : clRed;

	if (control->Hint != warning)
		control->Hint = warning;

	if (!warning.IsEmpty() && !control->ShowHint)
		control->ShowHint = true;

	if (warning.IsEmpty())
	{	// disable warning mode
		if (!control->StyleElements.Contains(seClient))
			control->StyleElements = control->StyleElements << seClient;	// enable the client style
	}
	else
	{	// enable warning mode
		if (control->StyleElements.Contains(seClient))
			control->StyleElements = control->StyleElements >> seClient;	// disable the client style so we can set our own colour
	}

	if (edit)
	{
		if (edit->Color != colour)
			edit->Color = colour;
	}
	else
	if (combo_box)
	{
		if (combo_box->Color != colour)
			combo_box->Color = colour;
	}
/*
	if (!warning.IsEmpty())
	{	// show a popup window
		THintWindow *hw = new THintWindow(control);
		if (hw)
		{
			hw->ParentWindow = Application->Handle;
			hw->Color = clInfoBk;
			hw->Hint = control->Name;
			const int th = hw->Canvas->TextHeight(hw->Hint);
			const int tw = hw->Canvas->TextWidth(hw->Hint);

			int x = 5;
			int y = th + 5;
			int w = tw;

			const TPoint point = control->ClientToScreen(Point(0, control->Height));

			TButton *but = new TButton(hw);
			if (but)
			{
				but->Parent = hw;
				but->Top = y;
				but->Left = x;
				but->Caption = "My Button";
				but->OnClick = onClick;
				but->OnMouseLeave = onMouseLeave;
				y += but->Height + 5;
				if (w < but->Width)
					w = but->Width;
			}

			TTrackBar *track_bar = new TTrackBar(hw);
			if (track_bar)
			{
				track_bar->Parent = hw;
				track_bar->Top = y;
				track_bar->Left = x;
				track_bar->Width = 100;
				track_bar->ThumbLength = 19;
				track_bar->Height = track_bar->ThumbLength + 2;
				track_bar->Cursor = crHandPoint;
				track_bar->Orientation = trHorizontal;
				track_bar->TickMarks = tmBoth;
				track_bar->TickStyle = tsNone;
				track_bar->Frequency = 1;
				track_bar->Min = 0;
				track_bar->Max = 10;
				track_bar->Position = 5;
				//track_bar->OnMouseLeave = onMouseLeave;
				y += track_bar->Height + 5;
				if (w < track_bar->Width)
					w = track_bar->Width;
			}

			TRect rect = Rect(point.X, point.Y, point.x + x + w + 5, point.Y + y);
			hw->ActivateHint(rect, hw->Hint);

			// when done ..
			//hw->ReleaseHandle();
		}
	}
	else
	{	// close the popup window
//		for (int i = 0; i < control->ControlCount; i++)
		for (int i = 0; i < control->ComponentCount; i++)
		{
//			THintWindow *hw = dynamic_cast<THintWindow *>(control->Controls[i]);
			THintWindow *hw = dynamic_cast<THintWindow *>(control->Components[i]);
			if (hw)
				hw->ReleaseHandle();
		}
	}
*/
}

void __fastcall CCommon::onClick(TObject *Sender)
{
	TButton *but = dynamic_cast<TButton *>(Sender);
	if (but)
	{
		THintWindow *hw = dynamic_cast<THintWindow *>(but->Parent);
		if (hw)
			hw->ReleaseHandle();
	}
}

void __fastcall CCommon::onMouseLeave(TObject *Sender)
{
/*
	TButton *but = dynamic_cast<TButton *>(Sender);
	if (but)
	{
		THintWindow *hw = dynamic_cast<THintWindow *>(but->Parent);
		if (hw)
			hw->ReleaseHandle();
	}
*/
/*
	TTrackBar *but = dynamic_cast<TrackBar *>(Sender);
	if (track_bar)
	{
		THintWindow *hw = dynamic_cast<THintWindow *>(track_bar->Parent);
		if (hw)
			hw->ReleaseHandle();
	}
*/
}

