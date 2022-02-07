
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#include <vcl.h>
#include <Clipbrd.hpp>
#include <DateUtils.hpp>
#include <Vcl.FileCtrl.hpp>
#include <System.IOUtils.hpp>	// TPath

#include <time.h>
#include <stdio.h>
//#include <mmsystem.h>		// PlaySound()

#include <float.h>

//#include <math.h>
#include <Math.hpp>

#include <complex>

#pragma hdrstop

#include "Unit1.h"
#include "settings.h"
#include "DataUnit.h"
#include "CommsUnit.h"
#include "VNAUsartUnit.h"
#include "BatteryVoltageUnit.h"
#include "SettingsUnit.h"
#include "UploadFirmwareUnit.h"
#include "UploadFirmwareV2Unit.h"
#include "CalibrationUnit.h"
#include "ScreenCapture.h"
#include "common.h"
#include "LCMatch.h"
#include "Calibration.h"

//#include "shellscalingapi.h"

#include <initguid.h>
#include <Dbt.h>

#define POPUP_MENU_LINE_PADDING  5

const int RBW_DEFAULT[]          = {10, 25, 33, 50, 100, 250, 333, 500, 1000, 2000, 3000, 4000, 5000, 7500, 10000, 20000, 25000, 50000};

const int NUM_POINTS_DEFAULT[]   = {51, 101, 201, 401, 801, 1024, 1601, 3201, 4501, 6401, 12801, 25601};
const int NUM_POINTS_V1[]        = {51, 101, 201, 401, 801, 1601, 3201, 6401, 12801, 25601};
//const int NUM_POINTS_V2[]      = {11, 21, 51, 101, 201, 401, 801, 1024, 1601, 3201, 6401, 12801, 25601};
const int NUM_POINTS_V2[]        = {11, 21, 51, 101, 201, 401, 801, 1024};	// the V2 does not handle segments very well - they half it's scan speed
const int NUM_POINTS_V2PLUS4[]   = {11, 21, 51, 101, 201, 401, 801, 1601, 3201, 6401, 12801, 25601};
const int NUM_POINTS_JANVNA_V2[] = {11, 21, 51, 101, 201, 401, 801, 1601, 3201, 4501};
const int NUM_POINTS_TINYSA[]    = {51, 101, 145, 290, 500, 750, 1000, 2000, 5000};

// ************************************************************************

const GUID GUID_BUS1394_CLASS                       = {0x6BDD1FC1, 0x810F, 0x11d0, {0xBE, 0xC7, 0x08, 0x00, 0x2B, 0xE2, 0x09, 0x2F}};
const GUID GUID_61883_CLASS                         = {0x7EBEFBC0, 0x3200, 0x11d2, {0xB4, 0xC2, 0x00, 0xA0, 0xC9, 0x69, 0x7D, 0x07}};
const GUID GUID_DEVICE_APPLICATIONLAUNCH_BUTTON     = {0x629758EE, 0x986E, 0x4D9E, {0x8E, 0x47, 0xDE, 0x27, 0xF8, 0xAB, 0x05, 0x4D}};
const GUID GUID_DEVICE_BATTERY                      = {0x72631E54, 0x78A4, 0x11D0, {0xBC, 0xF7, 0x00, 0xAA, 0x00, 0xB7, 0xB3, 0x2A}};
const GUID GUID_DEVICE_LID                          = {0x4AFA3D52, 0x74A7, 0x11d0, {0xbe, 0x5e, 0x00, 0xA0, 0xC9, 0x06, 0x28, 0x57}};
const GUID GUID_DEVICE_MEMORY                       = {0x3FD0F03D, 0x92E0, 0x45FB, {0xB7, 0x5C, 0x5E, 0xD8, 0xFF, 0xB0, 0x10, 0x21}};
const GUID GUID_DEVICE_MESSAGE_INDICATOR            = {0xCD48A365, 0xFA94, 0x4CE2, {0xA2, 0x32, 0xA1, 0xB7, 0x64, 0xE5, 0xD8, 0xB4}};
const GUID GUID_DEVICE_PROCESSOR                    = {0x97FADB10, 0x4E33, 0x40AE, {0x35, 0x9C, 0x8B, 0xEF, 0x02, 0x9D, 0xBD, 0xD0}};
const GUID GUID_DEVICE_SYS_BUTTON                   = {0x4AFA3D53, 0x74A7, 0x11d0, {0xbe, 0x5e, 0x00, 0xA0, 0xC9, 0x06, 0x28, 0x57}};
const GUID GUID_DEVICE_THERMAL_ZONE                 = {0x4AFA3D51, 0x74A7, 0x11d0, {0xbe, 0x5e, 0x00, 0xA0, 0xC9, 0x06, 0x28, 0x57}};
const GUID GUID_BTHPORT_DEVICE_INTERFACE            = {0x0850302A, 0xB344, 0x4fda, {0x9B, 0xE9, 0x90, 0x57, 0x6B, 0x8D, 0x46, 0xF0}};
const GUID GUID_DEVINTERFACE_BRIGHTNESS             = {0xFDE5BBA4, 0xB3F9, 0x46FB, {0xBD, 0xAA, 0x07, 0x28, 0xCE, 0x31, 0x00, 0xB4}};
const GUID GUID_DEVINTERFACE_DISPLAY_ADAPTER        = {0x5B45201D, 0xF2F2, 0x4F3B, {0x85, 0xBB, 0x30, 0xFF, 0x1F, 0x95, 0x35, 0x99}};
const GUID GUID_DEVINTERFACE_I2C                    = {0x2564AA4F, 0xDDDB, 0x4495, {0xB4, 0x97, 0x6A, 0xD4, 0xA8, 0x41, 0x63, 0xD7}};
const GUID GUID_DEVINTERFACE_IMAGE                  = {0x6BDD1FC6, 0x810F, 0x11D0, {0xBE, 0xC7, 0x08, 0x00, 0x2B, 0xE2, 0x09, 0x2F}};
const GUID GUID_DEVINTERFACE_MONITOR                = {0xE6F07B5F, 0xEE97, 0x4a90, {0xB0, 0x76, 0x33, 0xF5, 0x7B, 0xF4, 0xEA, 0xA7}};
const GUID GUID_DEVINTERFACE_OPM                    = {0xBF4672DE, 0x6B4E, 0x4BE4, {0xA3, 0x25, 0x68, 0xA9, 0x1E, 0xA4, 0x9C, 0x09}};
const GUID GUID_DEVINTERFACE_VIDEO_OUTPUT_ARRIVAL   = {0x1AD9E4F0, 0xF88D, 0x4360, {0xBA, 0xB9, 0x4C, 0x2D, 0x55, 0xE5, 0x64, 0xCD}};
const GUID GUID_DISPLAY_DEVICE_ARRIVAL              = {0x1CA05180, 0xA699, 0x450A, {0x9A, 0x0C, 0xDE, 0x4F, 0xBE, 0x3D, 0xDD, 0x89}};
const GUID GUID_DEVINTERFACE_HID                    = {0x4D1E55B2, 0xF16F, 0x11CF, {0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}};
const GUID GUID_DEVINTERFACE_KEYBOARD               = {0x884b96c3, 0x56ef, 0x11d1, {0xbc, 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd}};
const GUID GUID_DEVINTERFACE_MOUSE                  = {0x378DE44C, 0x56EF, 0x11D1, {0xBC, 0x8C, 0x00, 0xA0, 0xC9, 0x14, 0x05, 0xDD}};
const GUID GUID_DEVINTERFACE_MODEM                  = {0x2C7089AA, 0x2E0E, 0x11D1, {0xB1, 0x14, 0x00, 0xC0, 0x4F, 0xC2, 0xAA, 0xE4}};
const GUID GUID_DEVINTERFACE_NET                    = {0xCAC88484, 0x7515, 0x4C03, {0x82, 0xE6, 0x71, 0xA8, 0x7A, 0xBA, 0xC3, 0x61}};
const GUID GUID_DEVINTERFACE_SENSOR                 = {0xBA1BB692, 0X9B7A, 0X4833, {0X9A, 0X1E, 0X52, 0X5E, 0XD1, 0X34, 0XE7, 0XE2}};
const GUID GUID_DEVINTERFACE_COMPORT                = {0x86E0D1E0, 0x8089, 0x11D0, {0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73}};	// NanoVNA V1
const GUID GUID_DEVINTERFACE_PARALLEL               = {0x97F76EF0, 0xF883, 0x11D0, {0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C}};
const GUID GUID_DEVINTERFACE_PARCLASS               = {0x811FC6A5, 0xF728, 0x11D0, {0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1}};
const GUID GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR = {0x4D36E978, 0xE325, 0x11CE, {0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18}};
const GUID GUID_DEVINTERFACE_CDCHANGER              = {0x53F56312, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID GUID_DEVINTERFACE_CDROM                  = {0x53F56308, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID GUID_DEVINTERFACE_DISK                   = {0x53F56307, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID GUID_DEVINTERFACE_FLOPPY                 = {0x53F56311, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID GUID_DEVINTERFACE_MEDIUMCHANGER          = {0x53F56310, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID GUID_DEVINTERFACE_PARTITION              = {0x53F5630A, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID GUID_DEVINTERFACE_STORAGEPORT            = {0x2ACCFE60, 0xC130, 0x11D2, {0xB0, 0x82, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID GUID_DEVINTERFACE_TAPE                   = {0x53F5630B, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID GUID_DEVINTERFACE_VOLUME                 = {0x53F5630D, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID GUID_DEVINTERFACE_WRITEONCEDISK          = {0x53F5630C, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID GUID_IO_VOLUME_DEVICE_INTERFACE          = {0x53F5630D, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID MOUNTDEV_MOUNTED_DEVICE_GUID             = {0x53F5630D, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}};
const GUID GUID_AVC_CLASS                           = {0x095780C3, 0x48A1, 0x4570, {0xBD, 0x95, 0x46, 0x70, 0x7F, 0x78, 0xC2, 0xDC}};
const GUID GUID_VIRTUAL_AVC_CLASS                   = {0x616EF4D0, 0x23CE, 0x446D, {0xA5, 0x68, 0xC3, 0x1E, 0xB0, 0x19, 0x13, 0xD0}};
const GUID KSCATEGORY_ACOUSTIC_ECHO_CANCEL          = {0xBF963D80, 0xC559, 0x11D0, {0x8A, 0x2B, 0x00, 0xA0, 0xC9, 0x25, 0x5A, 0xC1}};
const GUID KSCATEGORY_AUDIO                         = {0x6994AD04, 0x93EF, 0x11D0, {0xA3, 0xCC, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_AUDIO_DEVICE                  = {0xFBF6F530, 0x07B9, 0x11D2, {0xA7, 0x1E, 0x00, 0x00, 0xF8, 0x00, 0x47, 0x88}};
const GUID KSCATEGORY_AUDIO_GFX                     = {0x9BAF9572, 0x340C, 0x11D3, {0xAB, 0xDC, 0x00, 0xA0, 0xC9, 0x0A, 0xB1, 0x6F}};
const GUID KSCATEGORY_AUDIO_SPLITTER                = {0x9EA331FA, 0xB91B, 0x45F8, {0x92, 0x85, 0xBD, 0x2B, 0xC7, 0x7A, 0xFC, 0xDE}};
const GUID KSCATEGORY_BDA_IP_SINK                   = {0x71985F4A, 0x1CA1, 0x11d3, {0x9C, 0xC8, 0x00, 0xC0, 0x4F, 0x79, 0x71, 0xE0}};
const GUID KSCATEGORY_BDA_NETWORK_EPG               = {0x71985F49, 0x1CA1, 0x11d3, {0x9C, 0xC8, 0x00, 0xC0, 0x4F, 0x79, 0x71, 0xE0}};
const GUID KSCATEGORY_BDA_NETWORK_PROVIDER          = {0x71985F4B, 0x1CA1, 0x11d3, {0x9C, 0xC8, 0x00, 0xC0, 0x4F, 0x79, 0x71, 0xE0}};
const GUID KSCATEGORY_BDA_NETWORK_TUNER             = {0x71985F48, 0x1CA1, 0x11d3, {0x9C, 0xC8, 0x00, 0xC0, 0x4F, 0x79, 0x71, 0xE0}};
const GUID KSCATEGORY_BDA_RECEIVER_COMPONENT        = {0xFD0A5AF4, 0xB41D, 0x11d2, {0x9C, 0x95, 0x00, 0xC0, 0x4F, 0x79, 0x71, 0xE0}};
const GUID KSCATEGORY_BDA_TRANSPORT_INFORMATION     = {0xA2E3074F, 0x6C3D, 0x11d3, {0xB6, 0x53, 0x00, 0xC0, 0x4F, 0x79, 0x49, 0x8E}};
const GUID KSCATEGORY_BRIDGE                        = {0x085AFF00, 0x62CE, 0x11CF, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}};
const GUID KSCATEGORY_CAPTURE                       = {0x65E8773D, 0x8F56, 0x11D0, {0xA3, 0xB9, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_CLOCK                         = {0x53172480, 0x4791, 0x11D0, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}};
const GUID KSCATEGORY_COMMUNICATIONSTRANSFORM       = {0xCF1DDA2C, 0x9743, 0x11D0, {0xA3, 0xEE, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_CROSSBAR                      = {0xA799A801, 0xA46D, 0x11D0, {0xA1, 0x8C, 0x00, 0xA0, 0x24, 0x01, 0xDC, 0xD4}};
const GUID KSCATEGORY_DATACOMPRESSOR                = {0x1E84C900, 0x7E70, 0x11D0, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}};
const GUID KSCATEGORY_DATADECOMPRESSOR              = {0x2721AE20, 0x7E70, 0x11D0, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}};
const GUID KSCATEGORY_DATATRANSFORM                 = {0x2EB07EA0, 0x7E70, 0x11D0, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}};
const GUID KSCATEGORY_DRM_DESCRAMBLE                = {0xFFBB6E3F, 0xCCFE, 0x4D84, {0x90, 0xD9, 0x42, 0x14, 0x18, 0xB0, 0x3A, 0x8E}};
const GUID KSCATEGORY_ENCODER                       = {0x19689BF6, 0xC384, 0x48fd, {0xAD, 0x51, 0x90, 0xE5, 0x8C, 0x79, 0xF7, 0x0B}};
const GUID KSCATEGORY_ESCALANTE_PLATFORM_DRIVER     = {0x74F3AEA8, 0x9768, 0x11D1, {0x8E, 0x07, 0x00, 0xA0, 0xC9, 0x5E, 0xC2, 0x2E}};
const GUID KSCATEGORY_FILESYSTEM                    = {0x760FED5E, 0x9357, 0x11D0, {0xA3, 0xCC, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_INTERFACETRANSFORM            = {0xCF1DDA2D, 0x9743, 0x11D0, {0xA3, 0xEE, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_MEDIUMTRANSFORM               = {0xCF1DDA2E, 0x9743, 0x11D0, {0xA3, 0xEE, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_MICROPHONE_ARRAY_PROCESSOR    = {0x830A44F2, 0xA32D, 0x476B, {0xBE, 0x97, 0x42, 0x84, 0x56, 0x73, 0xB3, 0x5A}};
const GUID KSCATEGORY_MIXER                         = {0xAD809C00, 0x7B88, 0x11D0, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}};
const GUID KSCATEGORY_MULTIPLEXER                   = {0x7A5DE1D3, 0x01A1, 0x452c, {0xB4, 0x81, 0x4F, 0xA2, 0xB9, 0x62, 0x71, 0xE8}};
const GUID KSCATEGORY_NETWORK                       = {0x67C9CC3C, 0x69C4, 0x11D2, {0x87, 0x59, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_PREFERRED_MIDIOUT_DEVICE      = {0xD6C50674, 0x72C1, 0x11D2, {0x97, 0x55, 0x00, 0x00, 0xF8, 0x00, 0x47, 0x88}};
const GUID KSCATEGORY_PREFERRED_WAVEIN_DEVICE       = {0xD6C50671, 0x72C1, 0x11D2, {0x97, 0x55, 0x00, 0x00, 0xF8, 0x00, 0x47, 0x88}};
const GUID KSCATEGORY_PREFERRED_WAVEOUT_DEVICE      = {0xD6C5066E, 0x72C1, 0x11D2, {0x97, 0x55, 0x00, 0x00, 0xF8, 0x00, 0x47, 0x88}};
const GUID KSCATEGORY_PROXY                         = {0x97EBAACA, 0x95BD, 0x11D0, {0xA3, 0xEA, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_QUALITY                       = {0x97EBAACB, 0x95BD, 0x11D0, {0xA3, 0xEA, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_REALTIME                      = {0xEB115FFC, 0x10C8, 0x4964, {0x83, 0x1D, 0x6D, 0xCB, 0x02, 0xE6, 0xF2, 0x3F}};
const GUID KSCATEGORY_RENDER                        = {0x65E8773E, 0x8F56, 0x11D0, {0xA3, 0xB9, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_SPLITTER                      = {0x0A4252A0, 0x7E70, 0x11D0, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}};
const GUID KSCATEGORY_SYNTHESIZER                   = {0xDFF220F3, 0xF70F, 0x11D0, {0xB9, 0x17, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_SYSAUDIO                      = {0xA7C7A5B1, 0x5AF3, 0x11D1, {0x9C, 0xED, 0x00, 0xA0, 0x24, 0xBF, 0x04, 0x07}};
const GUID KSCATEGORY_TEXT                          = {0x6994AD06, 0x93EF, 0x11D0, {0xA3, 0xCC, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_TOPOLOGY                      = {0xDDA54A40, 0x1E4C, 0x11D1, {0xA0, 0x50, 0x40, 0x57, 0x05, 0xC1, 0x00, 0x00}};
const GUID KSCATEGORY_TVAUDIO                       = {0xA799A802, 0xA46D, 0x11D0, {0xA1, 0x8C, 0x00, 0xA0, 0x24, 0x01, 0xDC, 0xD4}};
const GUID KSCATEGORY_TVTUNER                       = {0xA799A800, 0xA46D, 0x11D0, {0xA1, 0x8C, 0x00, 0xA0, 0x24, 0x01, 0xDC, 0xD4}};
const GUID KSCATEGORY_VBICODEC                      = {0x07DAD660, 0x22F1, 0x11D1, {0xA9, 0xF4, 0x00, 0xC0, 0x4F, 0xBB, 0xDE, 0x8F}};
const GUID KSCATEGORY_VIDEO                         = {0x6994AD05, 0x93EF, 0x11D0, {0xA3, 0xCC, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_VIRTUAL                       = {0x3503EAC4, 0x1F26, 0x11D1, {0x8A, 0xB0, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
const GUID KSCATEGORY_VPMUX                         = {0xA799A803, 0xA46D, 0x11D0, {0xA1, 0x8C, 0x00, 0xA0, 0x24, 0x01, 0xDC, 0xD4}};
const GUID KSCATEGORY_WDMAUD                        = {0x3E227E76, 0x690D, 0x11D2, {0x81, 0x61, 0x00, 0x00, 0xF8, 0x77, 0x5B, 0xF1}};
const GUID KSMFT_CATEGORY_AUDIO_DECODER             = {0x9ea73fb4, 0xef7a, 0x4559, {0x8d, 0x5d, 0x71, 0x9d, 0x8f, 0x04, 0x26, 0xc7}};
const GUID KSMFT_CATEGORY_AUDIO_EFFECT              = {0x11064c48, 0x3648, 0x4ed0, {0x93, 0x2e, 0x05, 0xce, 0x8a, 0xc8, 0x11, 0xb7}};
const GUID KSMFT_CATEGORY_AUDIO_ENCODER             = {0x91c64bd0, 0xf91e, 0x4d8c, {0x92, 0x76, 0xdb, 0x24, 0x82, 0x79, 0xd9, 0x75}};
const GUID KSMFT_CATEGORY_DEMULTIPLEXER             = {0xa8700a7a, 0x939b, 0x44c5, {0x99, 0xd7, 0x76, 0x22, 0x6b, 0x23, 0xb3, 0xf1}};
const GUID KSMFT_CATEGORY_MULTIPLEXER               = {0x059c561e, 0x05ae, 0x4b61, {0xb6, 0x9d, 0x55, 0xb6, 0x1e, 0xe5, 0x4a, 0x7b}};
const GUID KSMFT_CATEGORY_OTHER                     = {0x90175d57, 0xb7ea, 0x4901, {0xae, 0xb3, 0x93, 0x3a, 0x87, 0x47, 0x75, 0x6f}};
const GUID KSMFT_CATEGORY_VIDEO_DECODER             = {0xd6c02d4b, 0x6833, 0x45b4, {0x97, 0x1a, 0x05, 0xa4, 0xb0, 0x4b, 0xab, 0x91}};
const GUID KSMFT_CATEGORY_VIDEO_EFFECT              = {0x12e17c21, 0x532c, 0x4a6e, {0x8a, 0x1c, 0x40, 0x82, 0x5a, 0x73, 0x63, 0x97}};
const GUID KSMFT_CATEGORY_VIDEO_ENCODER             = {0xf79eac7d, 0xe545, 0x4387, {0xbd, 0xee, 0xd6, 0x47, 0xd7, 0xbd, 0xe4, 0x2a}};
const GUID KSMFT_CATEGORY_VIDEO_PROCESSOR           = {0x302ea3fc, 0xaa5f, 0x47f9, {0x9f, 0x7a, 0xc2, 0x18, 0x8b, 0xb1, 0x63, 0x02}};
const GUID GUID_DEVINTERFACE_USB_DEVICE             = {0xA5DCBF10, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};
const GUID GUID_DEVINTERFACE_USB_HOST_CONTROLLER    = {0x3ABF6F2D, 0x71C4, 0x462A, {0x8A, 0x92, 0x1E, 0x68, 0x61, 0xE6, 0xAF, 0x27}};
const GUID GUID_DEVINTERFACE_USB_HUB                = {0xF18A0E88, 0xC30C, 0x11D0, {0x88, 0x15, 0x00, 0xA0, 0xC9, 0x06, 0xBE, 0xD8}};
const GUID GUID_DEVINTERFACE_WPD                    = {0x6AC27878, 0xA6FA, 0x4155, {0xBA, 0x85, 0xF9, 0x8F, 0x49, 0x1D, 0x4F, 0x33}};
const GUID GUID_DEVINTERFACE_WPD_PRIVATE            = {0xBA0C718F, 0x4DED, 0x49B7, {0xBD, 0xD3, 0xFA, 0xBE, 0x28, 0x66, 0x12, 0x11}};
const GUID GUID_DEVINTERFACE_SIDESHOW               = {0x152E5811, 0xFEB9, 0x4B00, {0x90, 0xF4, 0xD3, 0x29, 0x47, 0xAE, 0x16, 0x81}};

//const GUID GUID_DFU                               = {0x3FE809AB, 0xFB91, 0x4CB5, {0xA6, 0x43, 0x69, 0x67, 0x0D, 0x52, 0x36, 0x6E}};
//const GUID GUID_APP                               = {0xcb979912, 0x5029, 0x420a, {0xae, 0xb1, 0x34, 0xfc, 0x0a, 0x7d, 0x57, 0x26}};

#if 0
const GUID GUID_DEVINTERFACE_ALL[] =
{
{0x6BDD1FC1, 0x810F, 0x11d0, {0xBE, 0xC7, 0x08, 0x00, 0x2B, 0xE2, 0x09, 0x2F}},
{0x7EBEFBC0, 0x3200, 0x11d2, {0xB4, 0xC2, 0x00, 0xA0, 0xC9, 0x69, 0x7D, 0x07}},
{0x629758EE, 0x986E, 0x4D9E, {0x8E, 0x47, 0xDE, 0x27, 0xF8, 0xAB, 0x05, 0x4D}},
{0x72631E54, 0x78A4, 0x11D0, {0xBC, 0xF7, 0x00, 0xAA, 0x00, 0xB7, 0xB3, 0x2A}},
{0x4AFA3D52, 0x74A7, 0x11d0, {0xbe, 0x5e, 0x00, 0xA0, 0xC9, 0x06, 0x28, 0x57}},
{0x3FD0F03D, 0x92E0, 0x45FB, {0xB7, 0x5C, 0x5E, 0xD8, 0xFF, 0xB0, 0x10, 0x21}},
{0xCD48A365, 0xFA94, 0x4CE2, {0xA2, 0x32, 0xA1, 0xB7, 0x64, 0xE5, 0xD8, 0xB4}},
{0x97FADB10, 0x4E33, 0x40AE, {0x35, 0x9C, 0x8B, 0xEF, 0x02, 0x9D, 0xBD, 0xD0}},
{0x4AFA3D53, 0x74A7, 0x11d0, {0xbe, 0x5e, 0x00, 0xA0, 0xC9, 0x06, 0x28, 0x57}},
{0x4AFA3D51, 0x74A7, 0x11d0, {0xbe, 0x5e, 0x00, 0xA0, 0xC9, 0x06, 0x28, 0x57}},
{0x0850302A, 0xB344, 0x4fda, {0x9B, 0xE9, 0x90, 0x57, 0x6B, 0x8D, 0x46, 0xF0}},
{0xFDE5BBA4, 0xB3F9, 0x46FB, {0xBD, 0xAA, 0x07, 0x28, 0xCE, 0x31, 0x00, 0xB4}},
{0x5B45201D, 0xF2F2, 0x4F3B, {0x85, 0xBB, 0x30, 0xFF, 0x1F, 0x95, 0x35, 0x99}},
{0x2564AA4F, 0xDDDB, 0x4495, {0xB4, 0x97, 0x6A, 0xD4, 0xA8, 0x41, 0x63, 0xD7}},
{0x6BDD1FC6, 0x810F, 0x11D0, {0xBE, 0xC7, 0x08, 0x00, 0x2B, 0xE2, 0x09, 0x2F}},
{0xE6F07B5F, 0xEE97, 0x4a90, {0xB0, 0x76, 0x33, 0xF5, 0x7B, 0xF4, 0xEA, 0xA7}},
{0xBF4672DE, 0x6B4E, 0x4BE4, {0xA3, 0x25, 0x68, 0xA9, 0x1E, 0xA4, 0x9C, 0x09}},
{0x1AD9E4F0, 0xF88D, 0x4360, {0xBA, 0xB9, 0x4C, 0x2D, 0x55, 0xE5, 0x64, 0xCD}},
{0x1CA05180, 0xA699, 0x450A, {0x9A, 0x0C, 0xDE, 0x4F, 0xBE, 0x3D, 0xDD, 0x89}},
{0x4D1E55B2, 0xF16F, 0x11CF, {0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}},
{0x884b96c3, 0x56ef, 0x11d1, {0xbc, 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd}},
{0x378DE44C, 0x56EF, 0x11D1, {0xBC, 0x8C, 0x00, 0xA0, 0xC9, 0x14, 0x05, 0xDD}},
{0x2C7089AA, 0x2E0E, 0x11D1, {0xB1, 0x14, 0x00, 0xC0, 0x4F, 0xC2, 0xAA, 0xE4}},
{0xCAC88484, 0x7515, 0x4C03, {0x82, 0xE6, 0x71, 0xA8, 0x7A, 0xBA, 0xC3, 0x61}},
{0xBA1BB692, 0X9B7A, 0X4833, {0X9A, 0X1E, 0X52, 0X5E, 0XD1, 0X34, 0XE7, 0XE2}},
{0x86E0D1E0, 0x8089, 0x11D0, {0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73}},	// NanoVNA V1
{0x97F76EF0, 0xF883, 0x11D0, {0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C}},
{0x811FC6A5, 0xF728, 0x11D0, {0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1}},
{0x4D36E978, 0xE325, 0x11CE, {0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18}},
{0x53F56312, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}},
{0x53F56308, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}},
{0x53F56307, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}},
{0x53F56311, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}},
{0x53F56310, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}},
{0x53F5630A, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}},
{0x2ACCFE60, 0xC130, 0x11D2, {0xB0, 0x82, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}},
{0x53F5630B, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}},
{0x53F5630D, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}},
{0x53F5630C, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}},
{0x53F5630D, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}},
{0x53F5630D, 0xB6BF, 0x11D0, {0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B}},
{0x095780C3, 0x48A1, 0x4570, {0xBD, 0x95, 0x46, 0x70, 0x7F, 0x78, 0xC2, 0xDC}},
{0x616EF4D0, 0x23CE, 0x446D, {0xA5, 0x68, 0xC3, 0x1E, 0xB0, 0x19, 0x13, 0xD0}},
{0xBF963D80, 0xC559, 0x11D0, {0x8A, 0x2B, 0x00, 0xA0, 0xC9, 0x25, 0x5A, 0xC1}},
{0x6994AD04, 0x93EF, 0x11D0, {0xA3, 0xCC, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0xFBF6F530, 0x07B9, 0x11D2, {0xA7, 0x1E, 0x00, 0x00, 0xF8, 0x00, 0x47, 0x88}},
{0x9BAF9572, 0x340C, 0x11D3, {0xAB, 0xDC, 0x00, 0xA0, 0xC9, 0x0A, 0xB1, 0x6F}},
{0x9EA331FA, 0xB91B, 0x45F8, {0x92, 0x85, 0xBD, 0x2B, 0xC7, 0x7A, 0xFC, 0xDE}},
{0x71985F4A, 0x1CA1, 0x11d3, {0x9C, 0xC8, 0x00, 0xC0, 0x4F, 0x79, 0x71, 0xE0}},
{0x71985F49, 0x1CA1, 0x11d3, {0x9C, 0xC8, 0x00, 0xC0, 0x4F, 0x79, 0x71, 0xE0}},
{0x71985F4B, 0x1CA1, 0x11d3, {0x9C, 0xC8, 0x00, 0xC0, 0x4F, 0x79, 0x71, 0xE0}},
{0x71985F48, 0x1CA1, 0x11d3, {0x9C, 0xC8, 0x00, 0xC0, 0x4F, 0x79, 0x71, 0xE0}},
{0xFD0A5AF4, 0xB41D, 0x11d2, {0x9C, 0x95, 0x00, 0xC0, 0x4F, 0x79, 0x71, 0xE0}},
{0xA2E3074F, 0x6C3D, 0x11d3, {0xB6, 0x53, 0x00, 0xC0, 0x4F, 0x79, 0x49, 0x8E}},
{0x085AFF00, 0x62CE, 0x11CF, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}},
{0x65E8773D, 0x8F56, 0x11D0, {0xA3, 0xB9, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0x53172480, 0x4791, 0x11D0, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}},
{0xCF1DDA2C, 0x9743, 0x11D0, {0xA3, 0xEE, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0xA799A801, 0xA46D, 0x11D0, {0xA1, 0x8C, 0x00, 0xA0, 0x24, 0x01, 0xDC, 0xD4}},
{0x1E84C900, 0x7E70, 0x11D0, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}},
{0x2721AE20, 0x7E70, 0x11D0, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}},
{0x2EB07EA0, 0x7E70, 0x11D0, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}},
{0xFFBB6E3F, 0xCCFE, 0x4D84, {0x90, 0xD9, 0x42, 0x14, 0x18, 0xB0, 0x3A, 0x8E}},
{0x19689BF6, 0xC384, 0x48fd, {0xAD, 0x51, 0x90, 0xE5, 0x8C, 0x79, 0xF7, 0x0B}},
{0x74F3AEA8, 0x9768, 0x11D1, {0x8E, 0x07, 0x00, 0xA0, 0xC9, 0x5E, 0xC2, 0x2E}},
{0x760FED5E, 0x9357, 0x11D0, {0xA3, 0xCC, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0xCF1DDA2D, 0x9743, 0x11D0, {0xA3, 0xEE, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0xCF1DDA2E, 0x9743, 0x11D0, {0xA3, 0xEE, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0x830A44F2, 0xA32D, 0x476B, {0xBE, 0x97, 0x42, 0x84, 0x56, 0x73, 0xB3, 0x5A}},
{0xAD809C00, 0x7B88, 0x11D0, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}},
{0x7A5DE1D3, 0x01A1, 0x452c, {0xB4, 0x81, 0x4F, 0xA2, 0xB9, 0x62, 0x71, 0xE8}},
{0x67C9CC3C, 0x69C4, 0x11D2, {0x87, 0x59, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0xD6C50674, 0x72C1, 0x11D2, {0x97, 0x55, 0x00, 0x00, 0xF8, 0x00, 0x47, 0x88}},
{0xD6C50671, 0x72C1, 0x11D2, {0x97, 0x55, 0x00, 0x00, 0xF8, 0x00, 0x47, 0x88}},
{0xD6C5066E, 0x72C1, 0x11D2, {0x97, 0x55, 0x00, 0x00, 0xF8, 0x00, 0x47, 0x88}},
{0x97EBAACA, 0x95BD, 0x11D0, {0xA3, 0xEA, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0x97EBAACB, 0x95BD, 0x11D0, {0xA3, 0xEA, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0xEB115FFC, 0x10C8, 0x4964, {0x83, 0x1D, 0x6D, 0xCB, 0x02, 0xE6, 0xF2, 0x3F}},
{0x65E8773E, 0x8F56, 0x11D0, {0xA3, 0xB9, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0x0A4252A0, 0x7E70, 0x11D0, {0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00}},
{0xDFF220F3, 0xF70F, 0x11D0, {0xB9, 0x17, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0xA7C7A5B1, 0x5AF3, 0x11D1, {0x9C, 0xED, 0x00, 0xA0, 0x24, 0xBF, 0x04, 0x07}},
{0x6994AD06, 0x93EF, 0x11D0, {0xA3, 0xCC, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0xDDA54A40, 0x1E4C, 0x11D1, {0xA0, 0x50, 0x40, 0x57, 0x05, 0xC1, 0x00, 0x00}},
{0xA799A802, 0xA46D, 0x11D0, {0xA1, 0x8C, 0x00, 0xA0, 0x24, 0x01, 0xDC, 0xD4}},
{0xA799A800, 0xA46D, 0x11D0, {0xA1, 0x8C, 0x00, 0xA0, 0x24, 0x01, 0xDC, 0xD4}},
{0x07DAD660, 0x22F1, 0x11D1, {0xA9, 0xF4, 0x00, 0xC0, 0x4F, 0xBB, 0xDE, 0x8F}},
{0x6994AD05, 0x93EF, 0x11D0, {0xA3, 0xCC, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0x3503EAC4, 0x1F26, 0x11D1, {0x8A, 0xB0, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}},
{0xA799A803, 0xA46D, 0x11D0, {0xA1, 0x8C, 0x00, 0xA0, 0x24, 0x01, 0xDC, 0xD4}},
{0x3E227E76, 0x690D, 0x11D2, {0x81, 0x61, 0x00, 0x00, 0xF8, 0x77, 0x5B, 0xF1}},
{0x9ea73fb4, 0xef7a, 0x4559, {0x8d, 0x5d, 0x71, 0x9d, 0x8f, 0x04, 0x26, 0xc7}},
{0x11064c48, 0x3648, 0x4ed0, {0x93, 0x2e, 0x05, 0xce, 0x8a, 0xc8, 0x11, 0xb7}},
{0x91c64bd0, 0xf91e, 0x4d8c, {0x92, 0x76, 0xdb, 0x24, 0x82, 0x79, 0xd9, 0x75}},
{0xa8700a7a, 0x939b, 0x44c5, {0x99, 0xd7, 0x76, 0x22, 0x6b, 0x23, 0xb3, 0xf1}},
{0x059c561e, 0x05ae, 0x4b61, {0xb6, 0x9d, 0x55, 0xb6, 0x1e, 0xe5, 0x4a, 0x7b}},
{0x90175d57, 0xb7ea, 0x4901, {0xae, 0xb3, 0x93, 0x3a, 0x87, 0x47, 0x75, 0x6f}},
{0xd6c02d4b, 0x6833, 0x45b4, {0x97, 0x1a, 0x05, 0xa4, 0xb0, 0x4b, 0xab, 0x91}},
{0x12e17c21, 0x532c, 0x4a6e, {0x8a, 0x1c, 0x40, 0x82, 0x5a, 0x73, 0x63, 0x97}},
{0xf79eac7d, 0xe545, 0x4387, {0xbd, 0xee, 0xd6, 0x47, 0xd7, 0xbd, 0xe4, 0x2a}},
{0x302ea3fc, 0xaa5f, 0x47f9, {0x9f, 0x7a, 0xc2, 0x18, 0x8b, 0xb1, 0x63, 0x02}},
{0xA5DCBF10, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}},
{0x3ABF6F2D, 0x71C4, 0x462A, {0x8A, 0x92, 0x1E, 0x68, 0x61, 0xE6, 0xAF, 0x27}},
{0xF18A0E88, 0xC30C, 0x11D0, {0x88, 0x15, 0x00, 0xA0, 0xC9, 0x06, 0xBE, 0xD8}},
{0x6AC27878, 0xA6FA, 0x4155, {0xBA, 0x85, 0xF9, 0x8F, 0x49, 0x1D, 0x4F, 0x33}},
{0xBA0C718F, 0x4DED, 0x49B7, {0xBD, 0xD3, 0xFA, 0xBE, 0x28, 0x66, 0x12, 0x11}},
{0x152E5811, 0xFEB9, 0x4B00, {0x90, 0xF4, 0xD3, 0x29, 0x47, 0xAE, 0x16, 0x81}},

{0x3FE809AB, 0xFB91, 0x4CB5, {0xA6, 0x43, 0x69, 0x67, 0x0D, 0x52, 0x36, 0x6E}},
{0xcb979912, 0x5029, 0x420a, {0xae, 0xb1, 0x34, 0xfc, 0x0a, 0x7d, 0x57, 0x26}}
};
#endif

/*
// FTDI GUID list
const GUID GUID_DEVINTERFACE_FTDI[] =
{
	{0x86e0d1e0, 0x8089, 0x11d0, {0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73}},	// COMPORT
	{0xa5dcbf10, 0x6530, 0x11d2, {0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed}},	// USB_DEVICE
	{0x53f56307, 0xb6bf, 0x11d0, {0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b}},  // DISK
	{0x4d1e55b2, 0xf16f, 0x11Cf, {0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}},  // HID
	{0xad498944, 0x762f, 0x11d0, {0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c}},  //
	{0x219d0508, 0x57a8, 0x4ff5, {0x97, 0xa1, 0xbd, 0x86, 0x58, 0x7c, 0x6c, 0x7e}}	//
};
*/

// VID/PID for STM32F bootloader
#define MD_VID					0x0483
#define MD_PID					0xDF11

// VID's/PID's for GD32 NanoVNA V2's
#define V2A_VID				0x0483	// this when no bootloader and no firmware present ?
#define V2A_PID				0x5740	//
#define V2B_VID				0x04b4	// this when a NanoVNA V2 is present - either DFU bootloader mode or normal mode
#define V2B_PID				0x0008	//   "     "

// ************************************************************************

#pragma package(smart_init)
#pragma resource "*.dfm"

const float rad_2_deg = 180.0 / M_PI;
const float deg_2_rad = M_PI / 180.0;

TForm1 *Form1 = NULL;

// ***************************************

__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall TForm1::FormCreate(TObject *Sender)
{
	String s;

	m_initialised = false;
	m_window_not_minimized_count = 0;

	// A few quick notes on _control87.
	// This function is used to control the behavior of the FPU.
	// The flags that are being passed (MCW_EM) tell the FPU not to throw any exceptions.
	// This is useful when calling into MS libraries that perform FP calculations partly due
	// to the difference in how floating point values are stored.
	// The flag MCW_EM suppresses all FPU exceptions but this can be fine tuned.
	// See float.h for more info.
//	_control87(MCW_EM, MCW_EM);

/*
	{
		CHighResolutionTick timer;

		//value    = pow(base, exponent);
		//value    = exp(exponent);
		//exponent = log(value);
		//exponent = log10(value);

		const double range = 10.0;
		const double gamma = 4.0;

		const double ig = 1.0 / gamma;

		const double d1 = 0.5;

		const double d2 = pow(d1 / range, ig) * range;
		//const double d2 = exp(log(d1 / range) * ig) * range;

//		const double d3 = log(1.0 + d1) * ig;
		const double d3 = (exp(d1) - 1.0) * range / (exp(range) - 1.0);
//		const double d3 = log(1.0 + d1 * gamma) * range / log(1.0 + range * gamma);

		const double d4 = pow(d2 / range, gamma) * range;

		const double d5 = d4 - d1;

		timer.mark();
		for (int i = 0; i < 1000000; i++)
			const double d = pow(d1, ig);
		const double secs1 = timer.secs();
		timer.mark();
		for (int i = 0; i < 1000000; i++)
			const double d = log(d1);
		const double secs2 = timer.secs();
		timer.mark();
		for (int i = 0; i < 1000000; i++)
			const double d = exp(d1);
		const double secs3 = timer.secs();
		timer.mark();
		for (int i = 0; i < 1000000; i++)
			const double d = exp(log(d1) * ig);
		const double secs4 = timer.secs();

		const double da = secs1 / secs4;
		const double db = secs1 / secs2;
		const double dc = secs1 / secs3;
	}
*/
	//	set_terminate(myTerminate);

	// ********************
/*
	{
			std::complex <float> c3s;
		nonstd::complex <float> c3n;

		const std::complex <float> c1s(2.5, 0.1);
		const std::complex <float> c2s(3.5, 4.1);

		const nonstd::complex <float> c1n(2.5, 0.1);
		const nonstd::complex <float> c2n(3.5, 4.1);

		s.printf(L"%f %f  %f %f", c1s.real(), c1s.imag(), c1n.real(), c1n.imag());
		s.printf(L"%f %f  %f %f", c2s.real(), c2s.imag(), c2n.real(), c2n.imag());

		c3s = -c1s;
		c3n = -c1n;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s = c1s + c2s;
		c3n = c1n + c2n;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s = 1.0f + c2s;
		c3n = 1.0f + c2n;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s = 1.0f - c2s;
		c3n = 1.0f - c2n;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s = 2.0f * c2s;
		c3n = 2.0f * c2n;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s = 2.0f / c2s;
		c3n = 2.0f / c2n;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s = c1s + 1.6f;
		c3n = c1n + 1.6f;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s = c1s - 1.6f;
		c3n = c1n - 1.6f;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s = c1s * 1.6f;
		c3n = c1n * 1.6f;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s = c1s / 1.6f;
		c3n = c1n / 1.6f;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s = c1s / c2s;
		c3n = c1n / c2n;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s = c1s * c2s;
		c3n = c1n * c2n;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s /= c2s;
		c3n /= c2n;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s *= 2.0f;
		c3n *= 2.0f;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s *= c2s;
		c3n *= c2n;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s += c2s;
		c3n += c2n;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s -= c2s;
		c3n -= c2n;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s += 2.0f;
		c3n += 2.0f;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());

		c3s -= 2.0f;
		c3n -= 2.0f;
		s.printf(L"%f %f  %f %f", c3s.real(), c3s.imag(), c3n.real(), c3n.imag());
	}
*/
	// ********************

/*
	::GetSystemInfo(&SystemInfo);
//	sprintf(SystemInfoStr,
//					"OEM id: %u"crlf
//					"num of cpu's: %u"crlf
//					"page size: %u"crlf
//					"cpu type: %u"crlf
//					"min app addr: %lx"crlf
//					"max app addr: %lx"crlf
//					"active cpu mask: %u"crlf,
//					SystemInfo.dwOemId,
//					SystemInfo.dwNumberOfProcessors,
//					SystemInfo.dwPageSize,
//					SystemInfo.dwProcessorType,
//					SystemInfo.lpMinimumApplicationAddress,
//					SystemInfo.lpMaximumApplicationAddress,
//					SystemInfo.dwActiveProcessorMask);
//	MemoAddString(Memo1, SystemInfoStr);
*/


	{	// get the phsyical screen size
		m_screen_width  = 0;
		m_screen_height = 0;

		SYSTEM_INFO m_system_info;
		HDC hDC = GetDC(0);
		if (hDC != NULL)
		{
			//ScreenBitsPerPixel = ::GetDeviceCaps(hDC, BITSPIXEL);
			m_screen_width  = ::GetDeviceCaps(hDC, HORZRES);
			m_screen_height = ::GetDeviceCaps(hDC, VERTRES);
			ReleaseDC(0, hDC);
		}
	}

	{	// make a title string to use in the title bar
		TVersion version;
		common.GetBuildInfo(Application->ExeName, &version);
		String s;
		#ifdef _DEBUG
			s.printf(L"v%u.%u.%u.debug", version.MajorVer, version.MinorVer, version.ReleaseVer);
		#else
			s.printf(L"v%u.%u.%u", version.MajorVer, version.MinorVer, version.ReleaseVer);
		#endif

		String windows_ver = common.windowsVer;
		String local_name  = common.localName;

		common.title = Application->Title + " " + s + " by OneOfEleven (+OD05)";

		this->Caption = common.title;
		StatusBar2->Panels->Items[0]->Text = windows_ver + " " + local_name + " '" + String(common.decimalPoint()) + "'";
	}

	#ifndef USE_OPENGL
		GLPanel->Enabled = false;
		GLPanel->Visible = false;
		GraphPaintBox->Enabled = true;
		GraphPaintBox->Visible = true;
	#else
		GraphPaintBox->Enabled = false;
		GraphPaintBox->Visible = false;
		GLPanel->Enabled = true;
		GLPanel->Visible = true;
	#endif

	// help stop flicker
	this->ControlStyle          = this->ControlStyle << csOpaque;
	Panel1->ControlStyle        = Panel1->ControlStyle << csOpaque;
	Panel2->ControlStyle        = Panel2->ControlStyle << csOpaque;
	GLPanel->ControlStyle       = GLPanel->ControlStyle << csOpaque;
	InfoPanel->ControlStyle     = InfoPanel->ControlStyle << csOpaque;
	GraphPaintBox->ControlStyle = GraphPaintBox->ControlStyle << csOpaque;

	// save the current GUI colours
	m_info_panel_colours.push_back(InfoPanel->Color);
	for (int i = 0; i < InfoPanel->ControlCount; i++)
	{
		TControl *control = InfoPanel->Controls[i];
		TLabel *label = dynamic_cast<TLabel *>(control);
		if (label)
			m_info_panel_colours.push_back(label->Color);
	}

	{
		const TNotifyEvent ne = HistoryTrackBar->OnChange;
		HistoryTrackBar->OnChange = NULL;
		HistoryTrackBar->Max = 0;
		HistoryTrackBar->Min = -((int)MAX_HISTORY - 1);
		HistoryTrackBar->Position = 0;
		HistoryTrackBar->OnChange = ne;

		HistoryTrackBar->ShowSelRange = true;
		HistoryTrackBar->SelStart     = HistoryTrackBar->Max;
		HistoryTrackBar->SelEnd       = HistoryTrackBar->Max;
	}

	{
		const TNotifyEvent ne = TimeAverageLevelTrackBar->OnChange;
		TimeAverageLevelTrackBar->OnChange = NULL;
		TimeAverageLevelTrackBar->Max = MAX_HISTORY_POWER;
		TimeAverageLevelTrackBar->OnChange = ne;
	}

	SetScanRangeToVNAScanRangeBitBtn->Enabled = false;

	{
		const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
		ConnectDisconnectSpeedButton->OnClick = NULL;
		ConnectDisconnectSpeedButton->Caption = "Disconnected";
		ConnectDisconnectSpeedButton->Down = false;
		ConnectDisconnectSpeedButton->OnClick = ne;
	}

	#ifndef TCPIPH
		m_comms.tcpip = NULL;
		m_comms.tcpip_stream_rx = new TMemoryStream();
	#endif

	m_comms.connect_timer_tick = 0;
	m_comms.rx.buffer_wr = 0;
	m_comms.rx.buffer.resize(8192);

	m_record.enabled = true;
//	RecordSpeedButton->Hint = "Right click to set the recording path .. " + settings.recordFolder;

	CaptureVNAScreenBitBtn->Enabled = false;
	if (VNAScreenCaptureForm)
		VNAScreenCaptureForm->Hide();

	MemorySpeedButton0->Down    = settings.memoryEnable[0];
	MemorySpeedButton0->Caption = "Live";

	MemorySpeedButton1->Down    = settings.memoryEnable[1];
	MemorySpeedButton1->Caption = "m1";

	MemorySpeedButton2->Down    = settings.memoryEnable[2];
	MemorySpeedButton2->Caption = "m2";

	MemorySpeedButton3->Down    = settings.memoryEnable[3];
	MemorySpeedButton3->Caption = "m3";

	MemorySpeedButton4->Down    = settings.memoryEnable[4];
	MemorySpeedButton4->Caption = "m4";

	m_thread = NULL;

//	gdiplusToken = 0;
//	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	buildMarkerListBox();

	EnableNormaliseSpeedButton->Enabled = !data_unit.m_point_norm.empty();
	if (!EnableNormaliseSpeedButton->Enabled)
	{
		EnableNormaliseSpeedButton->Down = false;
		settings.normalisationEnabled = false;
	}

	DeleteFrequencyMarkersBitBtn->Enabled = false;

	StatusBar1->Panels->Items[1]->Text = "Points";
	StatusBar1->Panels->Items[2]->Text = "Seg";
	StatusBar1->Panels->Items[3]->Text = "History";
	StatusBar1->Panels->Items[4]->Text = "Scans";
	updateStepDisplay();
	StatusBar1->Panels->Items[6]->Text = common.libusbVersion;

	StatusBar2->Panels->Items[1]->Text = "Points/sec";
	StatusBar2->Panels->Items[2]->Text = "Battery";
	StatusBar2->Panels->Items[3]->Text = "Threshold (Hz)";
	StatusBar2->Panels->Items[4]->Text = "Point RBW (Hz)";
	StatusBar2->Panels->Items[5]->Text = "Device";

	// ***************
	// clear warnings

	common.setWarning(StartMHzEdit,  "");
	common.setWarning(StopMHzEdit,   "");
	common.setWarning(CenterMHzEdit, "");
	common.setWarning(SpanMHzEdit,   "");
	common.setWarning(CWMHzEdit,     "");

	common.setWarning(PointBandwidthHzComboBox, "");
	common.setWarning(NumberOfPointsComboBox, "");
	common.setWarning(VelocityFactorEdit, "");

	// ***************

	StopScanBitBtn->Enabled = false;

	ScanOnceSpeedButton->Enabled = false;
	ScanOnceSpeedButton->Down    = false;

	ScanSpeedButton->Enabled = false;
	ScanSpeedButton->Down    = false;

	SaveDialog1->InitialDir = ExtractFilePath(Application->ExeName);

//	NumberOfPointsTrackBarChange(NumberOfPointsTrackBar);

	m_memory_but_index = -1;

	m_popup_menu_memory_index = -1;

	m_popup_menu_mouse_graph  = -1;
	m_popup_menu_mouse_Hz     = -1;
	m_popup_menu_mouse_marker = -1;

	m_popup_menu_marker_index = -1;

	m_popup_menu_graph_type_graph    = -1;
	graphs.m_mouse.graph_type_select = -1;

	createGraphTypeMenus();

	updatePointBandwidthComboBox(true);

	updateNumberOfPointsComboBox(false);

	updateVelocityFactorComboBox();

	updateCalibrationSelectComboBox();

	// *****************

	updateDeviceComboBox();

	{
		TComboBox *cb = SerialPortBaudrateComboBox;

		const TNotifyEvent ne = cb->OnChange;
		cb->OnChange = NULL;
		cb->Items->BeginUpdate();
		cb->Clear();

		cb->AddItem("1200",    (TObject *)1200);
		cb->AddItem("2400",    (TObject *)2400);
		cb->AddItem("4800",    (TObject *)4800);
		cb->AddItem("9600",    (TObject *)9600);
		cb->AddItem("19200",   (TObject *)19200);
		cb->AddItem("38400",   (TObject *)38400);
		cb->AddItem("57600",   (TObject *)57600);
		cb->AddItem("76800",   (TObject *)76800);
		cb->AddItem("115200",  (TObject *)115200);
		cb->AddItem("230400",  (TObject *)230400);
		cb->AddItem("250000",  (TObject *)250000);
		cb->AddItem("460800",  (TObject *)460800);
		cb->AddItem("500000",  (TObject *)500000);
		cb->AddItem("921600",  (TObject *)921600);
		cb->AddItem("1000000", (TObject *)1000000);
		cb->AddItem("1843200", (TObject *)1843200);
		cb->AddItem("2000000", (TObject *)2000000);
		cb->AddItem("3000000", (TObject *)3000000);
		cb->AddItem("3686400", (TObject *)3686400);

		const int i = cb->Items->IndexOfObject((TObject *)115200);
		cb->ItemIndex = (i >= 0) ? i : 8;

		cb->Items->EndUpdate();
		cb->OnChange = ne;

		common.comboBoxAutoWidth(cb);
	}

	// *****************

	InfoPanelToggleSwitch->State = settings.infoPanel ? tssOn : tssOff;
	InfoPanel->Visible = settings.infoPanel;
	updateInfoPanel();

	// move to the saved position
	this->Top    = settings.mainWindowPos.top;
	this->Left   = settings.mainWindowPos.left;
	this->Width  = settings.mainWindowPos.width;
	this->Height = settings.mainWindowPos.height;

	if (Screen)
	{
		Screen->OnActiveFormChange    = ActiveFormChanged;
		Screen->OnActiveControlChange = ActiveControlChanged;
	}

	init();

	Application->OnShowHint = OnShowHint;
//	Application->OnHint     = OnHint;

	#ifdef USE_OPENGL
		graphs.glInit(GLPanel);
	#endif

	this->WindowProc    = WndProc;
//	Application->OnIdle = onIdle;
	Application->OnRestore = onRestore;

//	::PostMessage(this->Handle, WM_UPDATE_INFO_PANEL, 0, 0);
	::PostMessage(this->Handle, WM_INIT_GUI, 0, 0);
}

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
	if (Screen)
	{
		Screen->OnActiveFormChange    = NULL;
		Screen->OnActiveControlChange = NULL;
	}

	Timer1->Enabled = false;

	if (connected())
	{
		stop();
		disconnect();
	}

	for (int i = (int)m_notification_handle.size() - 1; i >= 0; i--)
	{
		if (m_notification_handle[i])
			::UnregisterDeviceNotification(m_notification_handle[i]);
		m_notification_handle[i] = NULL;
	}
	m_notification_handle.resize(0);

	#ifndef TCPIPH
		//if (m_comms.tcpip_stream_tx)
		//	delete m_comms.tcpip_stream_tx;
		//m_comms.tcpip_stream_tx = NULL;

		if (m_comms.tcpip_stream_rx)
			delete m_comms.tcpip_stream_rx;
		m_comms.tcpip_stream_rx = NULL;

		if (m_comms.tcpip)
		{
			try
			{
				delete m_comms.tcpip;
				m_comms.tcpip = NULL;
			}
			catch (Exception &exception)
			{
				//Application->ShowException(&exception);
				//String s = exception.ToString();
				m_comms.tcpip = NULL;
			}
      }
	#endif

//	if (gdiplusToken != 0)
//		Gdiplus::GdiplusShutdown(gdiplusToken);
//	gdiplusToken = 0;
}

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	Timer1->Enabled = false;

	if (connected())
	{
		stop();
		disconnect();
	}

	// save the memories
	for (int i = 0; i < MAX_MEMORIES; i++)
	{
		String filename;
		filename.printf(L"mem_%d.s2p", i);
		//filename.printf(L"mem_%d.csv", i);
		filename = ExtractFilePath(Application->ExeName) + filename;

		if (!data_unit.m_point_mem[i].empty())
		{
			common.saveSParams(data_unit.m_point_mem[i], 4, filename);
			//common.saveCSV(data_unit.m_point_mem[i], 1, false, filename);
			//common.saveCSV(data_unit.m_point_mem[i], 4, false, filename);
		}
		else
		{	// memory is empty, so don't load anything in next time we are run
			if (FileExists(filename))
				DeleteFile(filename);
		}
	}

	{	// save the normalisation memory
		String filename = "norm.s2p";
		filename = ExtractFilePath(Application->ExeName) + filename;
		if (!data_unit.m_point_norm.empty())
		{
			common.saveSParams(data_unit.m_point_norm, 4, filename);
		}
		else
		{	// memory is empty, so don't load anything in next time we are run
			if (FileExists(filename))
				DeleteFile(filename);
		}
	}

	saveSettings();
}

void __fastcall TForm1::myTerminate()
{
	pushCommMessage("exception not caught");
}

void __fastcall TForm1::onIdle(TObject *Sender, bool &done)
{
//	done = false;
}

void __fastcall TForm1::onRestore(TObject *Sender)
{
	//
}

bool __fastcall TForm1::scanUSB(const GUID guid, const uint16_t vid, const uint16_t pid)
{
//	const GUID guid = GUID_DEVINTERFACE_USB_DEVICE;
//	const GUID guid = GUID_DEVINTERFACE_COMPORT;

	HDEVINFO dev_info = ::SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (dev_info == INVALID_HANDLE_VALUE)
		return false;

	int devIndex = -1;
	std::vector <uint8_t> buffer;

	bool found = false;

	while (true)
	{
		BOOL res;
		char str[512];

		t_usb_device usb_device;
		usb_device.index = ++devIndex;
		usb_device.vid   = -1;
		usb_device.pid   = -1;

		SP_INTERFACE_DEVICE_DATA ifData;
		memset(&ifData, 0, sizeof(SP_INTERFACE_DEVICE_DATA));
		ifData.cbSize = sizeof(ifData);

		res = ::SetupDiEnumDeviceInterfaces(dev_info, NULL, &guid, devIndex, &ifData);
		if (!res)
		{	// not found
			::SetupDiDestroyDeviceInfoList(dev_info);
			//dev_info = NULL;
			break;
		}

		SP_DEVINFO_DATA did;
		memset(&did, 0, sizeof(SP_DEVINFO_DATA));
		did.cbSize = sizeof(SP_DEVINFO_DATA);

		DWORD needed = 0;
		::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, NULL, 0, &needed, &did);
		//res = ::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, NULL, 0, &needed, NULL);
		if (needed <= 0)
			continue;

		buffer.resize(needed);
		memset(&buffer[0], 0, needed);

		SP_INTERFACE_DEVICE_DETAIL_DATA *detail = (SP_INTERFACE_DEVICE_DETAIL_DATA *)&buffer[0];
		detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

		//SP_DEVINFO_DATA did;
		//memset(&did, 0, sizeof(SP_DEVINFO_DATA));
		//did.cbSize = sizeof(SP_DEVINFO_DATA);

		//res = ::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, detail, needed, NULL, &did);
		res = ::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, detail, needed, NULL, NULL);
		if (!res)
			continue;

		usb_device.path = AnsiString(detail->DevicePath);

		memset(str, 0, sizeof(str));
		res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_DEVICEDESC, NULL, (PBYTE)str, sizeof(str), NULL);
		if (res)
			usb_device.description = AnsiString(str);

		memset(str, 0, sizeof(str));
		res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_ENUMERATOR_NAME, NULL, (PBYTE)str, sizeof(str), NULL);
		if (res)
			usb_device.enumerator_name = AnsiString(str);

		memset(str, 0, sizeof(str));
		res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_FRIENDLYNAME, NULL, (PBYTE)str, sizeof(str), NULL);
		if (res)
			usb_device.friendly_name = AnsiString(str);

		memset(str, 0, sizeof(str));
		res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_LOCATION_INFORMATION, NULL, (PBYTE)str, sizeof(str), NULL);
		if (res)
			usb_device.location_information = AnsiString(str);

		memset(str, 0, sizeof(str));
		res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_MFG, NULL, (PBYTE)str, sizeof(str), NULL);
		if (res)
			usb_device.mfg = AnsiString(str);

		memset(str, 0, sizeof(str));
		res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_SERVICE, NULL, (PBYTE)str, sizeof(str), NULL);
		if (res)
			usb_device.service = AnsiString(str);

		if (!usb_device.path.IsEmpty())
		{
			int i;
			String s = usb_device.path.LowerCase();

			i = s.Pos("vid_");
			if (i > 0)
			{
				String str = s.SubString(i + 4, 5);
				i = str.Pos('&');
				if (i > 0)
					str = str.SubString(1, i - 1);
				if (!TryStrToInt("0x" + str, usb_device.vid))
					usb_device.vid = -1;
			}

			i = s.Pos("pid_");
			if (i > 0)
			{
				String str = s.SubString(i + 4, 5);
				i = str.Pos('#');
				if (i > 0)
				str = str.SubString(1, i - 1);
				if (!TryStrToInt("0x" + str, usb_device.pid))
					usb_device.pid = -1;
			}
		}

		// JanVNA2
		// "\\?\usb#vid_0483&pid_4121#209d39893350#{a5dcbf10-6530-11d2-901f-00c04fb951ed}"

		if ((usb_device.vid == vid && usb_device.pid == pid) || (vid == 0xffff && pid == 0xffff))
		{
			found = true;
/*			pushCommMessage("    device path: " + usb_device.path);
			pushCommMessage("    device desc: " + usb_device.description);
			pushCommMessage("    device enum: " + usb_device.enumerator_name);
			pushCommMessage("    device name: " + usb_device.friendly_name);
			pushCommMessage("device loc info: " + usb_device.location_information);
			pushCommMessage("     device mfg: " + usb_device.mfg);
			pushCommMessage(" device service: " + usb_device.service);
*/		}
	}

	return found;
}

void __fastcall TForm1::OnDeviceChange(TMessage &msg)
{
	const DWORD wparam = msg.WParam;
	const DWORD lparam = msg.LParam;

	const int event_type = wparam;

	String s;
	String Msg = "OnDeviceChange .. ";
	String name_str;

	Msg += " event type: ";

	switch (event_type)
	{
		case DBT_APPYBEGIN:
			Msg += "DBT_APPYBEGIN";
			break;
		case DBT_APPYEND:
			Msg += "DBT_CONFIGCHANGECANCELED";
			break;
		case DBT_MONITORCHANGE:
			Msg += "DBT_MONITORCHANGE";
			break;
		case DBT_SHELLLOGGEDON:
			Msg += "DBT_SHELLLOGGEDON";
			break;
		case DBT_CONFIGMGAPI32:
			Msg += "DBT_CONFIGMGAPI32";
			break;
		case DBT_VXDINITCOMPLETE:
			Msg += "DBT_VXDINITCOMPLETE";
			break;
		case DBT_VOLLOCKQUERYLOCK:
			Msg += "VOLLOCKQUERYLOCK";
			break;
		case DBT_VOLLOCKLOCKTAKEN:
			Msg += "DBT_VOLLOCKLOCKTAKEN";
			break;
		case DBT_VOLLOCKLOCKFAILED:
			Msg += "DBT_VOLLOCKLOCKFAILED";
			break;
		case DBT_VOLLOCKQUERYUNLOCK:
			Msg += "DBT_VOLLOCKQUERYUNLOCK";
			break;
		case DBT_VOLLOCKLOCKRELEASED:
			Msg += "DBT_VOLLOCKLOCKRELEASED";
			break;
		case DBT_VOLLOCKUNLOCKFAILED:
			Msg += "DBT_VOLLOCKUNLOCKFAILED";
			break;
		case DBT_CONFIGCHANGED:
			Msg += "DBT_CONFIGCHANGED";
			break;
		case DBT_CUSTOMEVENT:
			Msg += "DBT_CUSTOMEVENT";
			break;
		case DBT_DEVICEARRIVAL:
			Msg += "DBT_DEVICEARRIVAL";
			{
				PDEV_BROADCAST_HDR dbc = (PDEV_BROADCAST_HDR)lparam;
				if (dbc != NULL)
				{
					if (dbc->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
					{
						PDEV_BROADCAST_DEVICEINTERFACE dbcc = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(dbc);
						if (dbcc != NULL)
						{
//							name_str = String((const wchar_t *)dbcc->dbcc_name);
//							Msg += " name: \"" + name_str + "\" ";
						}
					}
				}
			}
			break;
		case DBT_DEVICEQUERYREMOVE:
			Msg += "DBT_DEVICEQUERYREMOVE";
			break;
		case DBT_DEVICEQUERYREMOVEFAILED:
			Msg += "DBT_DEVICEQUERYREMOVEFAILED";
			break;
		case DBT_DEVICEREMOVEPENDING:
			Msg += "DBT_DEVICEREMOVEPENDING";
			break;
		case DBT_DEVICEREMOVECOMPLETE:
			Msg += "DBT_DEVICEREMOVECOMPLETE";
			break;
		case DBT_DEVICETYPESPECIFIC:
			Msg += "DBT_DEVICETYPESPECIFIC";
			{
				PDEV_BROADCAST_HDR dbc = (PDEV_BROADCAST_HDR)lparam;
				if (dbc)
				{
					switch (dbc->dbch_devicetype)
					{
						case DBT_DEVTYP_DEVICEINTERFACE:
							{
								PDEV_BROADCAST_DEVICEINTERFACE dbcc = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(dbc);

							}
							break;
						case DBT_DEVTYP_HANDLE:
							{
								PDEV_BROADCAST_HANDLE dbch = reinterpret_cast<PDEV_BROADCAST_HANDLE>(dbc);

							}
							break;
						case DBT_DEVTYP_OEM:
							{
								PDEV_BROADCAST_OEM dbco = reinterpret_cast<PDEV_BROADCAST_OEM>(dbc);

							}
							break;
						case DBT_DEVTYP_PORT:
							{
								PDEV_BROADCAST_PORT dbcp = reinterpret_cast<PDEV_BROADCAST_PORT>(dbc);

							}
							break;
						case DBT_DEVTYP_VOLUME:
							{
								PDEV_BROADCAST_VOLUME dbcv = reinterpret_cast<PDEV_BROADCAST_VOLUME>(dbc);

							}
							break;
					}
				}
			}
			break;
		case DBT_QUERYCHANGECONFIG:
			Msg += "DBT_QUERYCHANGECONFIG";
			break;
		case DBT_DEVNODES_CHANGED:
			Msg += "DBT_DEVNODES_CHANGED";
			break;
		case DBT_USERDEFINED:
			Msg += "DBT_USERDEFINED";
			{
				_DEV_BROADCAST_USERDEFINED *dbcu = (_DEV_BROADCAST_USERDEFINED *)lparam;

			}
			break;

		default:
			s.printf(L"unknown event type [%d]", wparam);
			Msg += s;
			break;
	}

	PDEV_BROADCAST_DEVICEINTERFACE pdbch = (PDEV_BROADCAST_DEVICEINTERFACE)lparam;
	if (pdbch == NULL)
	{
		Msg += ", device type: none";
//		pushCommMessage("device: " + Msg);

		// detect the presence of the JanVNA2
		const bool found_janvna2a = scanUSB(GUID_DEVINTERFACE_USB_DEVICE, JANV2A_VID, JANV2A_PID);
		const bool found_janvna2b = scanUSB(GUID_DEVINTERFACE_USB_DEVICE, JANV2B_VID, JANV2B_PID);
		if (!(found_janvna2a || found_janvna2b) || found_janvna2a || found_janvna2b)
			::PostMessage(this->Handle, WM_COM_DEVICE_CHANGED, 0, 0);
		if (!found_janvna2a && !found_janvna2b && janvna2_comms.connected)
			::PostMessage(this->Handle, WM_DISCONNECT, 0, 0);

		return;
	}

	const DWORD device_type = pdbch->dbcc_devicetype;

	s.printf(L", size: %d", pdbch->dbcc_size);
	Msg += s;

	#if defined(__BORLANDC__)
		name_str = String((const wchar_t *)pdbch->dbcc_name);
		if (!name_str.IsEmpty())
			Msg += ", name: " + name_str + " ";
	#endif

	const GUID guid = pdbch->dbcc_classguid;
	s.printf(L", guid %08X %04X %04X %02X %02X %02X %02X %02X %02X %02X %02X",
				guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	Msg += s;

	Msg += ", device type: ";

	if ((event_type == DBT_DEVICEREMOVECOMPLETE || event_type == DBT_DEVICEARRIVAL) && device_type == DBT_DEVTYP_PORT)
	{
		//const GUID guid = GUID_DEVINTERFACE_USB_DEVICE;
		const GUID guid = GUID_DEVINTERFACE_COMPORT;

		//HDEVINFO dev_info = ::SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
		HDEVINFO dev_info = ::SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
		if (dev_info != INVALID_HANDLE_VALUE)
		{
			int devIndex = -1;
			std::vector <uint8_t> buffer;

			while (true)
			{
				BOOL res;
				char str[512];

				t_usb_device usb_device;
				usb_device.index = ++devIndex;
				usb_device.vid   = -1;
				usb_device.pid   = -1;

				SP_INTERFACE_DEVICE_DATA ifData;
				memset(&ifData, 0, sizeof(SP_INTERFACE_DEVICE_DATA));
				ifData.cbSize = sizeof(ifData);

				res = ::SetupDiEnumDeviceInterfaces(dev_info, NULL, &guid, devIndex, &ifData);
				if (!res)
				{	// not found
					::SetupDiDestroyDeviceInfoList(dev_info);
					//dev_info = NULL;
					break;
				}

				SP_DEVINFO_DATA did;
				memset(&did, 0, sizeof(SP_DEVINFO_DATA));
				did.cbSize = sizeof(SP_DEVINFO_DATA);

				DWORD needed = 0;
				::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, NULL, 0, &needed, &did);
				//res = ::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, NULL, 0, &needed, NULL);
				if (needed <= 0)
					continue;

				buffer.resize(needed);
				memset(&buffer[0], 0, needed);

				SP_INTERFACE_DEVICE_DETAIL_DATA *detail = (SP_INTERFACE_DEVICE_DETAIL_DATA *)&buffer[0];
				detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

				//SP_DEVINFO_DATA did;
				//memset(&did, 0, sizeof(SP_DEVINFO_DATA));
				//did.cbSize = sizeof(SP_DEVINFO_DATA);

				//res = ::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, detail, needed, NULL, &did);
				res = ::SetupDiGetDeviceInterfaceDetail(dev_info, &ifData, detail, needed, NULL, NULL);
				if (!res)
					continue;

				usb_device.path = AnsiString(detail->DevicePath);

				memset(str, 0, sizeof(str));
				res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_DEVICEDESC, NULL, (PBYTE)str, sizeof(str), NULL);
				if (res)
					usb_device.description = AnsiString(str);

				memset(str, 0, sizeof(str));
				res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_ENUMERATOR_NAME, NULL, (PBYTE)str, sizeof(str), NULL);
				if (res)
					usb_device.enumerator_name = AnsiString(str);

				memset(str, 0, sizeof(str));
				res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_FRIENDLYNAME, NULL, (PBYTE)str, sizeof(str), NULL);
				if (res)
					usb_device.friendly_name = AnsiString(str);

				memset(str, 0, sizeof(str));
				res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_LOCATION_INFORMATION, NULL, (PBYTE)str, sizeof(str), NULL);
				if (res)
					usb_device.location_information = AnsiString(str);

				memset(str, 0, sizeof(str));
				res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_MFG, NULL, (PBYTE)str, sizeof(str), NULL);
				if (res)
					usb_device.mfg = AnsiString(str);

				memset(str, 0, sizeof(str));
				res = ::SetupDiGetDeviceRegistryProperty(dev_info, &did, SPDRP_SERVICE, NULL, (PBYTE)str, sizeof(str), NULL);
				if (res)
					usb_device.service = AnsiString(str);

				if (!usb_device.path.IsEmpty())
				{
					int i;
					s = usb_device.path.LowerCase();

					i = s.Pos("vid_");
					if (i > 0)
					{
						String str = s.SubString(i + 4, 5);
						i = str.Pos('&');
						if (i > 0)
							str = str.SubString(1, i - 1);
						if (!TryStrToInt("0x" + str, usb_device.vid))
							usb_device.vid = -1;
					}

					i = s.Pos("pid_");
					if (i > 0)
					{
						String str = s.SubString(i + 4, 5);
						i = str.Pos('#');
						if (i > 0)
						str = str.SubString(1, i - 1);
						if (!TryStrToInt("0x" + str, usb_device.pid))
							usb_device.pid = -1;
					}
				}

				if (usb_device.vid == MD_VID || usb_device.pid == MD_PID)
				{	// STM32F
				}
				else
				if (usb_device.vid == V2A_VID || usb_device.pid == V2A_PID)
				{
				}
				else
				if (usb_device.vid == V2B_VID || usb_device.pid == V2B_PID)
				{	// NanoVNA V2
				}

				// device path: \\?\usb#vid_0483&pid_5740#400#{86e0d1e0-8089-11d0-9ce4-08003e301f73}
				// device name: STMicroelectronics Virtual COM Port (COM25)
				//
				// device path: \\?\ports#vid_04b4&pid_0008#demo#{86e0d1e0-8089-11d0-9ce4-08003e301f73}
				// device name: USB Serial Port (COM12)

//				s.printf(L"  vid:%04X pid:%04X", (uint16_t)usb_device.vid, (uint16_t)usb_device.pid);
				pushCommMessage("    device path: " + usb_device.path);
				//pushCommMessage("    device desc: " + usb_device.description);
				//pushCommMessage("    device enum: " + usb_device.enumerator_name);
				pushCommMessage("    device name: " + usb_device.friendly_name);
				//pushCommMessage("device loc info: " + usb_device.location_information);
				//pushCommMessage("     device mfg: " + usb_device.mfg);
				//pushCommMessage(" device service: " + usb_device.service);
			}
		}
	}

	switch (device_type)
	{
		case DBT_DEVTYP_OEM: 				// oem-defined device type
			Msg += "DBT_DEVTYP_OEM";
			pushCommMessage("device: " + Msg);
			break;

		case DBT_DEVTYP_DEVNODE:			// devnode number
			Msg += "DBT_DEVTYP_DEVNODE";
			pushCommMessage("device: " + Msg);
			break;

		case DBT_DEVTYP_VOLUME:				// logical volume
			Msg += "DBT_DEVTYP_VOLUME";
			pushCommMessage("device: " + Msg);
			break;

		case DBT_DEVTYP_PORT:				// serial, parallel
			Msg += "DBT_DEVTYP_PORT";

			if (event_type == DBT_DEVICEARRIVAL)
			{
				//if (name_str.UpperCase().Pos("USB#VID_0483&PID_5740") > 0)
				{
					pushCommMessage("device: " + Msg);
					::PostMessage(this->Handle, WM_COM_DEVICE_CHANGED, 1, 0);
				}
			}
			else
			if (event_type == DBT_DEVICEREMOVECOMPLETE)
			{
				//if (name_str.UpperCase().Pos("USB#VID_0483&PID_5740") > 0)
				{
					pushCommMessage("device: " + Msg);
					::PostMessage(this->Handle, WM_COM_DEVICE_CHANGED, 0, 0);
				}
			}

			break;

		case DBT_DEVTYP_NET:					// network resource
			Msg += "DBT_DEVTYP_NET";
			pushCommMessage("device: " + Msg);
			break;

		case DBT_DEVTYP_DEVICEINTERFACE:	// device interface class
			Msg += "DBT_DEVTYP_DEVICEINTERFACE";

			// NanoVNA
			// comport .. name: \\?\USB#VID_0483&PID_5740#400#{86e0d1e0-8089-11d0-9ce4-08003e301f73}
			// dfu ...... name: \\?\USB#VID_0483&PID_DF11#FFFFFFFEFFFF#{a5dcbf10-6530-11d2-901f-00c04fb951ed}

			if (event_type == DBT_DEVICEARRIVAL)
			{	// added
				if (name_str.UpperCase().Pos("USB#VID_0483&PID_5740") > 0)
				{
					pushCommMessage("device: " + Msg);
					::PostMessage(this->Handle, WM_COM_DEVICE_CHANGED, 1, 0);
				}
			}
			else
			if (event_type == DBT_DEVICEREMOVECOMPLETE)
			{	// removed
				if (name_str.UpperCase().Pos("USB#VID_0483&PID_5740") > 0)
				{
					pushCommMessage("device: " + Msg);
					::PostMessage(this->Handle, WM_COM_DEVICE_CHANGED, 0, 0);
				}
			}

			break;

		case DBT_DEVTYP_HANDLE:				// file system handle
			Msg += "DBT_DEVTYP_HANDLE";
			pushCommMessage("device: " + Msg);
			break;

//		#if(WINVER >= _WIN32_WINNT_WIN7)
//			case DBT_DEVTYP_DEVINST:			// device instance
//				Msg += "DBT_DEVTYP_DEVINST";
//				pushCommMessage("device: " + Msg);
//				break;
//		#endif

		default:
			s.printf(L"unknown [%u]", device_type);
			Msg += s;

			pushCommMessage("device: " + Msg);
			break;
	}

//	pushCommMessage("device: " + Msg);
}

#if (__BORLANDC__ < 0x0600)
	void __fastcall TForm1::OnShowHint(AnsiString &HintStr, bool &CanShow, THintInfo &HintInfo)
#else
	void __fastcall TForm1::OnShowHint(String &HintStr, bool &CanShow, THintInfo &HintInfo)
#endif
{
	if (HintInfo.HintControl == GraphPaintBox || HintInfo.HintControl == GLPanel)
	{
//		HintInfo.CursorPos
		HintInfo.HintColor     = clAqua;		// Changes only for this hint
		HintInfo.ReshowTimeout = 50;	  		// force update the hint text whilst it's showing
//		HintInfo.HintMaxWidth  = 200;	  		// Hint text word wraps if width is greater than 200
		HintInfo.HintPos.x    += 40;
		HintInfo.HintPos.y    -= 40;
	}
	else
	{
		String class_name = HintInfo.HintControl->ClassName();
		if (class_name == "TTrackBar")
		{
			HintInfo.ReshowTimeout = 50;	// force update the hint text whilst it's showing
		}
	}
}

void __fastcall TForm1::OnHint(TObject *Sender)
{
//	Label5->Caption = GetLongHint(Application->Hint);
}

void __fastcall TForm1::WndProc(TMessage &msg)
{
	if ((msg.Msg & 0xff80) == 0x80)
	{
//		String s;
//		s.printf(L"WndProc %08X", msg.Msg);
//		m_comm_messages.list.push_back(s);
	}

	switch (msg.Msg)
	{
		case 0x0112: // WM_SYSCOMMAND
//			{
//				String s;
//				s.printf(L"WM_SYSCOMMAND %08X", msg.WParam);
//				m_comm_messages.list.push_back(s);
//			}
//			if ((msg.WParam & 0xfff0) == 0xF030)	// Maximize event - SC_MAXIMIZE from Winuser.h
//			{	// THe window is being maximized
//			}
			break;

		case WM_DEVICECHANGE:
//			OnDeviceChange(msg);
			break;

		case WM_WINDOWPOSCHANGED:
//			m_window_not_minimized_count = 0;
			break;

		case WM_NCPAINT:
//			m_comm_messages.list.push_back("WM_NCPAINT");
			#ifdef USE_OPENGL
				m_window_not_minimized_count = 0;
			#endif

		case WM_NCACTIVATE:
//			m_comm_messages.list.push_back("WM_NCACTIVATE");
			#ifdef USE_OPENGL
				m_window_not_minimized_count = 0;
			#endif
			break;

		case WM_SIZE:
			{
//				String s;
//				s.printf(L"WM_SIZE %08X", msg.WParam);
//				m_comm_messages.list.push_back(s);
			}
			if (Application->MainForm)
			{
				switch (msg.WParam)
				{
					case SIZE_RESTORED:	// this is called AFTER we have normalized
						#ifdef USE_OPENGL
							m_window_not_minimized_count = 0;
						#endif
						break;
					case SIZE_MINIMIZED:	// this is called AFTER we have minimized
						break;
					case SIZE_MAXIMIZED:	// this is called AFTER we have maximized
						#ifdef USE_OPENGL
							m_window_not_minimized_count = 0;
						#endif
						break;
				}
			}
			break;

		case WM_EXITSIZEMOVE:
//			m_comm_messages.list.push_back("WM_EXITSIZEMOVE");
			break;

		case WM_ERASEBKGND:
			//msg.Result = TRUE;
			break;
	}

	TForm::WndProc(msg);
}

void _fastcall TForm1::ActiveFormChanged(TObject *Sender)
{
	// make the current active form opaque, and all other forms transparent
	if (Screen)
	{
		TForm *active_form  = Screen->ActiveForm;
//		TForm *focused_form = Screen->FocusedForm;

		for (int i = 0; i < Screen->FormCount; i++)
		{
			TForm *form = Screen->Forms[i];
			if (form)
			{
				if (form->Showing)
				{
					if (form != active_form)
					{	// make the form transparent
						if (form != Application->MainForm)	// always keep the main program form opaque
						{
							form->AlphaBlendValue = FORM_ALPHA_BLEND_VALUE;
							form->AlphaBlend      = true;
						}
					}
					else
					{	// make the form opaque
						form->AlphaBlend = false;
						form->BringToFront();
					}
				}
			}
		}
/*
		for (int i = 0; i < Application->ComponentCount; i++)
		{
			TComponent *comp = Application->Components[i];
			if (comp)
			{
				TForm *form = dynamic_cast<TForm *>(comp);
				if (form)
				{
					if (form->Showing)
					{
						if (form != active_form)
						{	// make the form transparent
							if (form != Application->MainForm)	// always keep the main program form opaque
							{
								form->AlphaBlendValue = FORM_ALPHA_BLEND_VALUE;
								form->AlphaBlend      = true;
							}
						}
						else
						{	// make the form opaque
							form->AlphaBlend = false;
							form->BringToFront();
						}
					}
				}
			}
		}
*/
	}
}

void _fastcall TForm1::ActiveControlChanged(TObject *Sender)
{
//	TWinControl *Active = NULL;
//	for (int i = 0; i < this->ControlCount; i++)
//	{
//		TWinControl *Temp = dynamic_cast<TWinControl *>(Form1->Controls[I]);
//		if (Temp && Temp->Focused())
//			Active = Temp;
//	}
//	if ((Active != NULL) && (Active->Hint != ""))
//		StatusBar2->SimpleText = GetLongHint(Active->Hint) + " focus";
}

void __fastcall TForm1::createGraphTypeMenus()
{
	// reduce the item vertical spacing
	//GraphTypePopupMenu->OwnerDraw = true;
	//GraphPopupMenu->OwnerDraw     = true;

	Graphtype1->Clear();
//	for (int i = Graphtype1->Count - 1; i >= 0; i--)
//		Graphtype1->Delete(i);

	GraphTypePopupMenu->Items->Clear();
//	for (int i = GraphTypePopupMenu->Items->Count - 1; i >= 0; i--)
//		GraphTypePopupMenu->Items->Delete(i);

	if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
	{
		for (int i = 0; i < GRAPH_TYPE_SIZE; i++)
		{
			int graph_type = -1;
			String s = "None";

			switch (i)
			{
				case  0: graph_type = GRAPH_TYPE_LOGMAG_S11;             s = "Log mag S11";               break;
				case  1: graph_type = GRAPH_TYPE_LOGMAG_S21;             s = "Log mag S21";               break;
				case  2: graph_type = GRAPH_TYPE_LOGMAG_S11S21;          s = "Log mag S11 and S21";       break;
				case  3: graph_type = GRAPH_TYPE_LINMAG_S11;             s = "Lin mag S11";               break;
				case  4: graph_type = GRAPH_TYPE_LINMAG_S21;             s = "Lin mag S21";               break;
				case  5: graph_type = GRAPH_TYPE_LINMAG_S11S21;          s = "Lin mag S11 and S21";       break;
				case  6: graph_type = GRAPH_TYPE_PHASE_S11;              s = "Phase S11";                 break;
				case  7: graph_type = GRAPH_TYPE_PHASE_S21;              s = "Phase S21";                 break;
				case  8: graph_type = GRAPH_TYPE_PHASE_S11S21;           s = "Phase S11 and S21";         break;
				case  9: graph_type = GRAPH_TYPE_PHASE_UNWRAP_S11;       s = "Phase unwrap S11";          break;
				case 10: graph_type = GRAPH_TYPE_PHASE_UNWRAP_S21;       s = "Phase unwrap S21";          break;
				case 11: graph_type = GRAPH_TYPE_PHASE_UNWRAP_S11S21;    s = "Phase unwrap S11 and S21";  break;
				case 12: graph_type = GRAPH_TYPE_GROUP_DELAY_S11;        s = "Group delay S11";           break;
				case 13: graph_type = GRAPH_TYPE_GROUP_DELAY_S21;        s = "Group delay S21";           break;
				case 14: graph_type = GRAPH_TYPE_GROUP_DELAY_S11S21;     s = "Group delay S11 and S21";   break;
				case 15: graph_type = GRAPH_TYPE_REAL_IMAG_S11;          s = "S-Parameters S11";          break;
				case 16: graph_type = GRAPH_TYPE_REAL_IMAG_S21;          s = "S-Parameters S21";          break;
				case 17: graph_type = GRAPH_TYPE_VSWR_S11;               s = "VSWR S11";                  break;
				case 18: graph_type = GRAPH_TYPE_IMPEDANCE_S11;          s = "Impedance S11";             break;
				case 19: graph_type = GRAPH_TYPE_SERIES_RJX_S11;         s = "Series R+jX S11";           break;
				case 20: graph_type = GRAPH_TYPE_PARALLEL_RJX_S11;       s = "Parallel R||jX S11";         break;
				case 21: graph_type = GRAPH_TYPE_SERIES_RESISTANCE_S11;  s = "Series resistance S11";     break;
				case 22: graph_type = GRAPH_TYPE_SERIES_REACTANCE_S11;   s = "Series reactance S11";      break;
				case 23: graph_type = GRAPH_TYPE_QUALITY_FACTOR_S11;     s = "Quality factor S11";        break;
				case 24: graph_type = GRAPH_TYPE_SERIES_CAPACITANCE_S11; s = "Series capacitance S11";    break;
				case 25: graph_type = GRAPH_TYPE_SERIES_INDUCTANCE_S11;  s = "Series inductance S11";     break;
				case 26: graph_type = GRAPH_TYPE_COAX_LOSS_S11;          s = "Coax loss S11";             break;
				case 27: graph_type = GRAPH_TYPE_CAL_LOGMAG;             s = "Calibrations log mag";            break;
				case 28: graph_type = GRAPH_TYPE_SMITH_S11;              s = "Smith S11";                       break;
				case 29: graph_type = GRAPH_TYPE_SMITH_S21;              s = "Smith S21";                       break;
				case 30: graph_type = GRAPH_TYPE_ADMITTANCE_S11;         s = "Admittance S11";                  break;
				case 31: graph_type = GRAPH_TYPE_ADMITTANCE_S21;         s = "Admittance S21";                  break;
				case 32: graph_type = GRAPH_TYPE_POLAR_S11;              s = "Polar S11";                       break;
				case 33: graph_type = GRAPH_TYPE_POLAR_S21;              s = "Polar S21";                       break;
//				case 34: graph_type = GRAPH_TYPE_PHASE_VECTOR_S11;       s = "Phase vector S11";                break;
//				case 35: graph_type = GRAPH_TYPE_PHASE_VECTOR_S21;       s = "Phase vector S21";                break;
				case 36: graph_type = GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11; s = "TDR linear low pass impulse S11"; break;
				case 37: graph_type = GRAPH_TYPE_TDR_LIN_BP_S11;         s = "TDR linear band pass S11";        break;
				case 38: graph_type = GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11; s = "TDR log low pass impulse S11";    break;
				case 39: graph_type = GRAPH_TYPE_TDR_LOG_BP_S11;         s = "TDR log band pass S11";           break;
				case 40: graph_type = GRAPH_TYPE_TDR_IMPEDANCE_S11;      s = "TDR impedance S11";               break;
				case 41: graph_type = GRAPH_TYPE_GJB_S11;                s = "Admittance G+jB S11";        break;
			}

			if (graph_type < 0)
				continue;

			TMenuItem *menu_item;

			const bool column_break = (i == ((3 + GRAPH_TYPE_SIZE) / 2)) ? true : false;
			const bool line_break   = (graph_type == GRAPH_TYPE_SMITH_S11 || graph_type == GRAPH_TYPE_PHASE_VECTOR_S11 || graph_type == GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11 || graph_type == GRAPH_TYPE_GJB_S11) ? true : false;

			// add it to the graph popup menu
			if (column_break || line_break)
			{
				menu_item = new TMenuItem(GraphPopupMenu);
				menu_item->Caption = "-";
				menu_item->Tag     = -1;
				menu_item->Break   = column_break ? mbBreak : mbNone;
				menu_item->Enabled = false;
				menu_item->AutoLineReduction = maAutomatic;	//maManual
				Graphtype1->Add(menu_item);
			}
			menu_item = new TMenuItem(GraphPopupMenu);
			if (menu_item != NULL)
			{
				menu_item->OnClick       = MenuItemGraphTypeClick;
				menu_item->Caption       = s;
				menu_item->Tag           = graph_type;
				menu_item->RadioItem     = true;
				Graphtype1->Add(menu_item);
			}

			// add it to the right click popup menu
			if (column_break || line_break)
			{
				menu_item = new TMenuItem(GraphTypePopupMenu);
				menu_item->Caption = "-";
				menu_item->Tag     = -1;
				menu_item->Break   = column_break ? mbBreak : mbNone;
				menu_item->Enabled = false;
				menu_item->AutoLineReduction = maAutomatic;	//maManual
				GraphTypePopupMenu->Items->Add(menu_item);
			}
			menu_item = new TMenuItem(GraphTypePopupMenu);
			if (menu_item != NULL)
			{
				menu_item->OnClick       = MenuItemChangeGraphTypeClick;
				menu_item->Caption       = s;
				menu_item->Tag           = graph_type;
				menu_item->RadioItem     = true;
				GraphTypePopupMenu->Items->Add(menu_item);
			}
		}
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			int graph_type = -1;
			String s = "None";

			switch (i)
			{
				case  0: graph_type = GRAPH_TYPE_LOGMAG_S11; s = "Log mag"; break;
				case  1: graph_type = GRAPH_TYPE_LINMAG_S11; s = "Lin mag"; break;
			}

			TMenuItem *menu_item;

			// add it to the graph popup menu
			menu_item = new TMenuItem(GraphPopupMenu);
			if (menu_item != NULL)
			{
				menu_item->OnClick       = MenuItemGraphTypeClick;
				menu_item->Caption       = s;
				menu_item->Tag           = graph_type;
				menu_item->RadioItem     = true;
				Graphtype1->Add(menu_item);
			}

			menu_item = new TMenuItem(GraphTypePopupMenu);
			if (menu_item != NULL)
			{
				menu_item->OnClick       = MenuItemChangeGraphTypeClick;
				menu_item->Caption       = s;
				menu_item->Tag           = graph_type;
				menu_item->RadioItem     = true;
				GraphTypePopupMenu->Items->Add(menu_item);
			}
		}

		settings.graphArrangement = GRAPH_ARRANGE_1;

		settings.graphType[0] = GRAPH_TYPE_LOGMAG_S11;
		settings.graphType[1] = GRAPH_TYPE_LINMAG_S11;
	}
}
/*
typedef struct {
  TLogPalette lpal;
  TPaletteEntry dummy[256];
} LogPal;

void __fastcall TForm1::saveFormImage()
{
	HWND notUsed;

	AnsiString filename = "NanoVNA_Win.bmp";

	TRect destRect   = Rect(0, 0, Width, Height);
	TRect sourceRect = destRect;

	std::auto_ptr <TCanvas> tempCanvas(new TCanvas);
	tempCanvas->Handle = GetDeviceContext(notUsed);

	std::auto_ptr <TImage> image2save(new TImage(Form1)); // The owner will clean this up.
	image2save->Height = Height;
	image2save->Width  = Width;
	image2save->Canvas->CopyRect(destRect, tempCanvas.get(), sourceRect);

	LogPal SysPal;
	SysPal.lpal.palVersion    = 0x300;
	SysPal.lpal.palNumEntries = 256;
	GetSystemPaletteEntries(tempCanvas->Handle, 0, 256, SysPal.lpal.palPalEntry);

	image2save->Picture->Bitmap->Palette = CreatePalette(dynamic_cast <const tagLOGPALETTE *> (&SysPal.lpal));
	image2save->Picture->SaveToFile(filename);
}
*/

void __fastcall TForm1::saveFormImage()
{
	TClipboard *cb;

	Graphics::TBitmap *bm1 = new Graphics::TBitmap();
	Graphics::TBitmap *bm2 = new Graphics::TBitmap();

	try
	{
		cb = Clipboard();

		const DWORD tick = GetTickCount();
		do
		{
			try
			{
				cb->Clear();

				// take a snap shot of our form
				keybd_event(VK_SNAPSHOT, 1, 0, 0);

				// wait until the snap shot has completed
				const DWORD tick2 = GetTickCount();
				do Application->ProcessMessages();
				while ((GetTickCount() - 500) < tick2 && !cb->HasFormat(CF_BITMAP));

				if (cb->HasFormat(CF_BITMAP))
				{
					bm1->Assign(cb);
					break;
				}

				// failed
				return;
			}
			catch (Exception &exception)
			{
				//String s = exception.ToString();
			}
		} while ((GetTickCount() - 1000) < tick);

		bm2->Width  = this->Width;
		bm2->Height = this->Height;

		//	const int dx = (this->Width - this->ClientWidth) / 2;
		//	const int dy = ::GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME);

		const int dx = 0;
		const int dy = 0;

		BitBlt(bm2->Canvas->Handle, 0, 0, bm2->Width, bm2->Height, bm1->Canvas->Handle, dx, dy, SRCCOPY);
	}
	catch (Exception &exception)
	{
		//Application->ShowException(&exception);
		//String s = exception.ToString();
		delete bm1;
		delete bm2;
		return;
	}

	//cb->Assign(bm2);
	common.saveBitmap(bm2, Application->Title, "Save window image ..", true);

	delete bm2;
	delete bm1;

	cb->Clear();
}

bool __fastcall TForm1::updateInfoPanel2(const int graph)
{
	AnsiString s;

	const int p_graph = graphs.m_mouse.graph;
	const int p_mem   = graphs.m_mouse.point_mem;
	const int p_index = graphs.m_mouse.point_index;

	const int mi = selectedMarker();

	//const int64_t mouse_Hz = xyToFreq(graph, graph_type, m_graph_mouse_x, m_graph_mouse_y);

	const int graph_type = settings.graphType[graph];

	//const bool freq_mode  = graphs.isFrequencyGraph(graph_type);
	const bool smith_mode = graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type);
	const bool tdr_mode   = graphs.isTDRGraph(graph_type);

	//const int gx = graphs.m_graph_pos[graph].x;
	//const int gy = graphs.m_graph_pos[graph].y;

	const int cx = graphs.m_graph_pos[graph].cx;
	const int cy = graphs.m_graph_pos[graph].cy;
	const int gr = graphs.m_graph_pos[graph].cr;

	const int mx = graphs.m_mouse.x;
	const int my = graphs.m_mouse.y;

	const float re = (gr > 0) ? (float)(mx - cx) / gr : 0;	// -1 to +1
	const float im = (gr > 0) ? (float)(cy - my) / gr : 0;	// -1 to +1
	const float re_im_dist = sqrtf((re * re) + (im * im));

	int64_t Hz = 0;

	const int mem = (p_mem >= 0 && p_index >= 0) ? p_mem : data_unit.firstUsedMem(true, 0);
	if(mem < 0)
		return false;

	if (smith_mode && re_im_dist > 1.0)
	{
		InfoPanel->Caption = "";
		return false;
	}

	if (graphs.m_graph_bm)
	{
		if (!tdr_mode && graph == p_graph && p_mem >= 0 && p_index >= 0)
		{
			Hz = data_unit.m_point_filt[p_mem][p_index].Hz;

			s = " Mouse";
			if (p_mem <= 0)
				s += ", live";
			else
				s += ", memory " + IntToStr(p_mem);
			InfoPanel->Caption = s;
		}

		if (Hz <= 0)
		{
			if (smith_mode)
			{
				if (re_im_dist <= 1.0f)
					Hz = (data_unit.m_freq_cw_Hz > 0) ? data_unit.m_freq_cw_Hz : data_unit.m_freq_center_Hz;
			}
			else
			if (!tdr_mode)
			{
				Hz = graphs.xyToFreq(graph, graph_type, mx, my);
			}

			if (Hz <= 0 && !settings.m_markers_freq.empty())
			{
				if (mi >= 0)
				{
					Hz = settings.m_markers_freq[mi].Hz;

					s.printf(" Marker %d", 1 + mi);
					if (mem <= 0)
						s += ", live";
					else
						s += ", memory " + IntToStr(mem);
					InfoPanel->Caption = s;
				}
			}
			else
			if (!tdr_mode && graph == p_graph && Hz > 0)
			{
				s = " Mouse";
				if (mem <= 0)
					s += ", live";
				else
					s += ", memory " + IntToStr(mem);
				InfoPanel->Caption = s;
			}
		}
	}

	const int index = data_unit.indexFreq(Hz, mem);

	//Hz = (index >= 0 && index < size) ? data_unit.m_point_filt[mem][index].Hz : 0;

	if (Hz > 0 && !tdr_mode)
	{
		const int size = data_unit.freqArraySize(mem);
		if (size < 4)
			return false;

		if (index >= 0 && index < size)
		{  // mouse is over a graph line

			const complexf c(re, im);

			const float ref_impedance = 50;
			const complexf c0 = (smith_mode && re_im_dist <= 1.0f) ? c : data_unit.m_point_filt[mem][index].s11;
			const complexf c1 = (smith_mode && re_im_dist <= 1.0f) ? c : data_unit.m_point_filt[mem][index].s21;

			const complexf imp            = data_unit.impedance(c0, ref_impedance);
			const complexf imp_p          = data_unit.serialToParallel(imp);
			const float return_loss       = -data_unit.gain10(c0);
			const float vswr              = data_unit.VSWR(c0);
			const float s11_mag           = data_unit.magnitude(c0);
			const float quality_factor    = data_unit.qualityFactor(c0, ref_impedance);
			const float s11_z             = data_unit.magnitude(imp);
			const float s11_phase         = data_unit.phase(c0);

			const float s21_gain          = data_unit.gain10(c1);
			const float s21_mag           = data_unit.magnitude(c1);
			const float s21_phase         = data_unit.phase(c1);

			const float res               = imp.real();
			const float res_j             = ABS(imp.imag());

			const float resp              = imp_p.real();
			const float resp_j            = ABS(imp_p.imag());

			const float cap               = data_unit.impedanceToCapacitance(imp, Hz);
			const float cap_p             = data_unit.impedanceToCapacitance(imp_p, Hz);

			const float ind               = data_unit.impedanceToInductance(imp, Hz);
			const float ind_p             = data_unit.impedanceToInductance(imp_p, Hz);

			String res_str   = common.valueToStr(res,    false, true).Trim();
			String resj_str  = common.valueToStr(res_j,  false, true).Trim();

			String resp_str  = common.valueToStr(resp,   false, true).Trim();
			String respj_str = common.valueToStr(resp_j, false, true).Trim();

			String cap_str   = common.valueToStr(cap,    false, true).Trim() + "F";
			String capp_str  = common.valueToStr(cap_p,  false, true).Trim() + "F";

			String ind_str   = common.valueToStr(ind,    false, true).Trim() + "H";
			String indp_str  = common.valueToStr(ind_p,  false, true).Trim() + "H";

			float s11_group_delay_sec;
			float s21_group_delay_sec;
			{
				complexf w;
				complexf v;
				complexf cpx;

				const int m                 = (index == 0) ? 0 : index - 1;
				const int n                 = (index >= (size - 1)) ? size - 1 : index + 1;
				const int64_t delta_freq    = data_unit.m_point_filt[mem][n].Hz - data_unit.m_point_filt[mem][m].Hz;

				w                           = data_unit.m_point_filt[mem][m].s11;
				v                           = data_unit.m_point_filt[mem][n].s11;
				cpx                         = w * v;
				s11_group_delay_sec         = (cpx.imag() == 0 || delta_freq == 0) ? 0.0f : (float)(atan2(cpx.real(), cpx.imag()) / (2 * M_PI * delta_freq));

				w                           = data_unit.m_point_filt[mem][m].s21;
				v                           = data_unit.m_point_filt[mem][n].s21;
				cpx                         = w * v;
				s21_group_delay_sec         = (cpx.imag() == 0 || delta_freq == 0) ? 0.0f : (float)(atan2(cpx.real(), cpx.imag()) / (2 * M_PI * delta_freq));
			}



			MarkerFrequencyLabel->Caption = common.freqToStrMHz(Hz) + " MHz";

			MarkerWavelengthLabel1->Caption = (Hz > 0) ? common.valueToStr((double)SPEED_OF_LIGHT / Hz, false, true, "") + "m" : String("");
			MarkerWavelengthLabel2->Caption = (Hz > 0) ? common.valueToStr((double)SPEED_OF_LIGHT / (Hz * 4), false, true, "") + "m" : String("");


			// S11 info


			res_str=common.valueToStr(c.real(),  false, true).Trim();
			resj_str=common.valueToStr(fabs(c.imag()),  false, true).Trim();
			s = res_str + " " + ((c.imag() >= 0) ? "-j" : "+j") + resj_str;
			MarkerS11RealImagLabel->Caption = s;

			s = res_str + " " + ((imp.imag() >= 0) ? "+j" : "-j") + resj_str;
			MarkerS11ImpedanceLabel->Caption = s;
			res_str=common.valueToStr(1/imp_p.real(),  false, true).Trim();
			resj_str=common.valueToStr(fabs(1/imp_p.imag()),  false, true).Trim();
			s = res_str + " " + ((imp_p.imag() >= 0) ? "-j" : "+j") + resj_str;
			MarkerS11AdmittanceLabel2->Caption = s;

			s.printf("%0.3f", imp.real());
			MarkerS11SeriesRLabel->Caption = s;
			s.printf("%0.3f", imp.imag());
			MarkerS11SeriesXLabel->Caption = s;
			MarkerS11SeriesLLabel->Caption = ind_str;
			MarkerS11SeriesCLabel->Caption = cap_str;

			s.printf("%0.3f", imp_p.real());
			MarkerS11ParallelRLabel->Caption = s;
			s.printf("%0.3f", imp_p.imag());
			MarkerS11ParallelXLabel->Caption = s;
			MarkerS11ParallelLLabel->Caption = indp_str;
			MarkerS11ParallelCLabel->Caption = capp_str;

			s.printf("%0.3f", vswr);
			MarkerS11VSWRLabel->Caption = s;

			s.printf("%0.3fdB", return_loss);
			MarkerS11ReturnLossLabel->Caption = s;

			s.printf("%0.3f", s11_mag);
			MarkerS11Label->Caption = s;

			s.printf("%0.3f", quality_factor);
			MarkerS11QualityFactorLabel->Caption = s;

			s.printf("%0.3f", s11_z);
			MarkerS11ZLabel->Caption = s;

			s.printf("%+0.3f\xb0", s11_phase * rad_2_deg);
			MarkerS11PhaseLabel->Caption = s;

			s.printf("%0.3f %+0.3f\xb0", s11_mag, s11_phase * rad_2_deg);
			MarkerS11PolarLabel->Caption = s;

			s = common.valueToStr(s11_group_delay_sec, false, true) + "s";
			MarkerS11GroupDelayLabel->Caption = s;


			// S21 info


//			s = common.valueToStr(c1.real(), false, true, "", true) + " " + common.valueToStr(c1.imag(), false, true, "", true);
			res_str=common.valueToStr(c1.real(),  false, true).Trim();
			resj_str=common.valueToStr(fabs(c1.imag()),  false, true).Trim();
			s = res_str + " " + ((c1.imag() >= 0) ? "-j" : "+j") + resj_str;
			MarkerS21RealImagLabel->Caption = s;

			s.printf("%+0.3fdB", s21_gain);
			MarkerS21GainLabel->Caption = s;

			s.printf("%0.3f", s21_mag);
			MarkerS21Label->Caption = s;

			s.printf("%+0.3f\xb0", s21_phase * rad_2_deg);
			MarkerS21PhaseLabel->Caption = s;

			s.printf("%0.3f %+0.3f\xb0", s21_mag, s21_phase * rad_2_deg);
			MarkerS21PolarLabel->Caption = s;

			s = common.valueToStr(s21_group_delay_sec, false, true) + "s";
			MarkerS21GroupDelayLabel->Caption = s;


			//



			// make everything visible on the info panel
			for (int i = 0; i < InfoPanel->ControlCount; i++)
			{
				TControl *control = InfoPanel->Controls[i];
				TLabel *label = dynamic_cast<TLabel *>(control);
				if (label)
					if (!label->Visible)
						label->Visible = true;
			}

			//InfoPanel->Visible = true;

			// force the info panel to finish display updates
			InfoPanel->Update();

			return true;
		}
		else
		{  // mouse is on empty space

			const complexf c(re, im);

			const float ref_impedance = 50;

			const complexf imp            = data_unit.impedance(c, ref_impedance);
			const complexf imp_p          = data_unit.serialToParallel(imp);
			const float return_loss       = data_unit.gain10(c);
			const float vswr              = data_unit.VSWR(c);
			const float s11_mag           = data_unit.magnitude(c);
			const float quality_factor    = data_unit.qualityFactor(c, ref_impedance);
			const float s11_z             = data_unit.magnitude(imp);
			const float s11_phase         = data_unit.phase(c);

			const float s21_gain          = data_unit.gain10(c);
			const float s21_mag           = data_unit.magnitude(c);
			const float s21_phase         = data_unit.phase(c);

			const float res               = imp.real();
			const float res_j             = ABS(imp.imag());

			const float resp              = imp_p.real();
			const float resp_j            = ABS(imp_p.imag());

			const float cap               = data_unit.impedanceToCapacitance(imp, Hz);
			const float cap_p             = data_unit.impedanceToCapacitance(imp_p, Hz);

			const float ind               = data_unit.impedanceToInductance(imp, Hz);
			const float ind_p             = data_unit.impedanceToInductance(imp_p, Hz);

			String res_str   = common.valueToStr(res,    false, true).Trim();
			String resj_str  = common.valueToStr(res_j,  false, true).Trim();

			String resp_str  = common.valueToStr(resp,   false, true).Trim();
			String respj_str = common.valueToStr(resp_j, false, true).Trim();

			String cap_str   = common.valueToStr(cap,    false, true).Trim() + "F";
			String capp_str  = common.valueToStr(cap_p,  false, true).Trim() + "F";

			String ind_str   = common.valueToStr(ind,    false, true).Trim() + "H";
			String indp_str  = common.valueToStr(ind_p,  false, true).Trim() + "H";

			float s11_group_delay_sec;
			float s21_group_delay_sec;
			{
				complexf w;
				complexf v;
				complexf cpx;

				const int m                 = (index == 0) ? 0 : index - 1;
				const int n                 = (index >= (size - 1)) ? size - 1 : index + 1;
				const int64_t delta_freq    = data_unit.m_point_filt[mem][n].Hz - data_unit.m_point_filt[mem][m].Hz;

				w                           = data_unit.m_point_filt[mem][m].s11;
				v                           = data_unit.m_point_filt[mem][n].s11;
				cpx                         = w * v;
				s11_group_delay_sec         = (cpx.imag() == 0 || delta_freq == 0) ? 0.0f : (float)(atan2(cpx.real(), cpx.imag()) / (2 * M_PI * delta_freq));

				w                           = data_unit.m_point_filt[mem][m].s21;
				v                           = data_unit.m_point_filt[mem][n].s21;
				cpx                         = w * v;
				s21_group_delay_sec         = (cpx.imag() == 0 || delta_freq == 0) ? 0.0f : (float)(atan2(cpx.real(), cpx.imag()) / (2 * M_PI * delta_freq));
			}



			MarkerFrequencyLabel->Caption = common.freqToStrMHz(Hz) + " MHz";

			MarkerWavelengthLabel1->Caption = (Hz > 0) ? common.valueToStr((double)SPEED_OF_LIGHT / Hz, false, true, "") + "m" : String("");
			MarkerWavelengthLabel2->Caption = (Hz > 0) ? common.valueToStr((double)SPEED_OF_LIGHT / (Hz * 4), false, true, "") + "m" : String("");


			// S11 info


			res_str=common.valueToStr(c.real(),  false, true).Trim();
			resj_str=common.valueToStr(fabs(c.imag()),  false, true).Trim();
			s = res_str + " " + ((c.imag() >= 0) ? "-j" : "+j") + resj_str;
			MarkerS11RealImagLabel->Caption = s;

			//s.printf("%0.3f %cj%0.3f", imp.real(), (imp.imag() >= 0) ? '+' : '-', fabsf(imp.imag()));
			s = res_str + " " + ((imp.imag() >= 0) ? "+j" : "-j") + resj_str;
			MarkerS11ImpedanceLabel->Caption = s;

			s.printf("%0.7f %cj%0.7f", 1/imp_p.real(), (imp_p.imag() >= 0) ? '-' : '+', fabsf(1/imp_p.imag()));
			MarkerS11AdmittanceLabel2->Caption = s;

			s.printf("%0.3f", imp.real());
			MarkerS11SeriesRLabel->Caption = s;
			s = (imp.imag() < 0) ? cap_str : ind_str;
			MarkerS11SeriesXLabel->Caption = s;
			MarkerS11SeriesLLabel->Caption = ind_str;
			MarkerS11SeriesCLabel->Caption = cap_str;

			s.printf("%0.3f", imp_p.real());
			MarkerS11ParallelRLabel->Caption = s;
			s = (imp_p.imag() < 0) ? capp_str : indp_str;
			MarkerS11ParallelXLabel->Caption = s;
			MarkerS11ParallelLLabel->Caption = indp_str;
			MarkerS11ParallelCLabel->Caption = capp_str;

			s.printf("%0.3f", vswr);
			MarkerS11VSWRLabel->Caption = s;

			s.printf("%0.3fdB", return_loss);
			MarkerS11ReturnLossLabel->Caption = s;

			s.printf("%0.3f", s11_mag);
			MarkerS11Label->Caption = s;

			s.printf("%0.3f", quality_factor);
			MarkerS11QualityFactorLabel->Caption = s;

			s.printf("%0.3f", s11_z);
			MarkerS11ZLabel->Caption = s;

			s.printf("%+0.3f\xb0", s11_phase * rad_2_deg);
			MarkerS11PhaseLabel->Caption = s;

			s.printf("%0.3f %+0.3f\xb0", s11_mag, s11_phase * rad_2_deg);
			MarkerS11PolarLabel->Caption = s;

			s = common.valueToStr(s11_group_delay_sec, false, true) + "s";
			MarkerS11GroupDelayLabel->Caption = s;


			// S21 info


			MarkerS21RealImagLabel->Caption   = "";
			MarkerS21GainLabel->Caption       = "";
			MarkerS21Label->Caption           = "";
			MarkerS21PhaseLabel->Caption      = "";
			MarkerS21PolarLabel->Caption      = "";
			MarkerS21GroupDelayLabel->Caption = "";


			//


			// make everything visible on the info panel
			for (int i = 0; i < InfoPanel->ControlCount; i++)
			{
				TControl *control = InfoPanel->Controls[i];
				TLabel *label = dynamic_cast<TLabel *>(control);
				if (label)
					if (!label->Visible)
						label->Visible = true;
			}

			//InfoPanel->Visible = true;

			// force the info panel to finish display updates
			InfoPanel->Update();

			return true;
		}
	}

	 return false;
}

void __fastcall TForm1::updateInfoPanel()
{
/*
	if (settings.borderWidth <= 0)
	{	// match the colour of the graphs
		if (InfoPanel->StyleElements.Contains(seClient))
			InfoPanel->StyleElements = InfoPanel->StyleElements >> seClient;	// disable the client style so we can set our own colour
		if (InfoPanel->Color != settings.m_colours.background)
			InfoPanel->Color = settings.m_colours.background;
		for (int i = 0; i < InfoPanel->ControlCount; i++)
		{
			TControl *control = InfoPanel->Controls[i];
			TLabel *label = dynamic_cast<TLabel *>(control);
			if (label)
			{
				if (label->StyleElements.Contains(seFont))
					label->StyleElements = label->StyleElements >> seFont;	// disable the font style so we can set our own colour
				if (label->Font->Color != settings.m_colours.font)
					label->Font->Color = settings.m_colours.font;
			}
		}
	}
	else
	{	// put components back to their original colours
		if (!InfoPanel->StyleElements.Contains(seClient))
			InfoPanel->StyleElements = InfoPanel->StyleElements << seClient;	// re-enable the client style
		if (InfoPanel->Color != m_info_panel_colours[0])
			InfoPanel->Color = m_info_panel_colours[0];
		for (int i = 0, k = 0; i < InfoPanel->ControlCount; i++)
		{
			TControl *control = InfoPanel->Controls[i];
			TLabel *label = dynamic_cast<TLabel *>(control);
			if (label)
			{
				if (!label->StyleElements.Contains(seFont))
					label->StyleElements = label->StyleElements << seFont;	// re-enable the font style
				if (label->Font->Color != m_info_panel_colours[k])
					label->Font->Color = m_info_panel_colours[k];
				k++;
			}
		}
	}
*/
	const int p_graph = graphs.m_mouse.graph;
	const int p_mem   = graphs.m_mouse.point_mem;
	const int p_index = graphs.m_mouse.point_index;

	const int mi = selectedMarker();

	if (p_mem < 0 && mi < 0)
		if (p_graph >= 0)
			if (updateInfoPanel2(p_graph))
				return;

	if (p_mem >= 0 || mi >= 0)
	{
		for (int graph = 0; graph < graphs.numberOfGraphs(); graph++)
			if (updateInfoPanel2(graph))
				return;
	}

	if (!InfoPanel->Visible)
		return;

	if (!MarkerFrequencyLabel->Caption.IsEmpty())
	{
		InfoPanel->Caption              = "";

		MarkerFrequencyLabel->Caption        = "";

		MarkerWavelengthLabel1->Caption      = "";
		MarkerWavelengthLabel2->Caption      = "";

		MarkerS11RealImagLabel->Caption      = "";
		MarkerS11ImpedanceLabel->Caption     = "";
		MarkerS11AdmittanceLabel2->Caption   = "";
		MarkerS11SeriesRLabel->Caption       = "";
		MarkerS11SeriesXLabel->Caption       = "";
		MarkerS11SeriesLLabel->Caption       = "";
		MarkerS11SeriesCLabel->Caption       = "";
		MarkerS11ParallelRLabel->Caption     = "";
		MarkerS11ParallelXLabel->Caption     = "";
		MarkerS11ParallelLLabel->Caption     = "";
		MarkerS11ParallelCLabel->Caption     = "";
		MarkerS11VSWRLabel->Caption          = "";
		MarkerS11ReturnLossLabel->Caption    = "";
		MarkerS11Label->Caption              = "";
		MarkerS11QualityFactorLabel->Caption = "";
		MarkerS11ZLabel->Caption             = "";
		MarkerS11PhaseLabel->Caption         = "";
		MarkerS11PolarLabel->Caption         = "";
		MarkerS11GroupDelayLabel->Caption    = "";

		MarkerS21RealImagLabel->Caption      = "";
		MarkerS21GainLabel->Caption          = "";
		MarkerS21Label->Caption              = "";
		MarkerS21PhaseLabel->Caption         = "";
		MarkerS21PolarLabel->Caption         = "";
		MarkerS21GroupDelayLabel->Caption    = "";
/*
		// make everything invisible on the info panel
		for (int i = 0; i < InfoPanel->ControlCount; i++)
		{
			TControl *control = InfoPanel->Controls[i];
			TLabel *label = dynamic_cast<TLabel *>(control);
			if (label)
				if (label->Visible)
					label->Visible = false;
		}
*/
		// force the info panel to finish display updates
		InfoPanel->Update();
	}
}

// *************************************************************************
// read/write thread

void __fastcall TForm1::addMarker(const int64_t freq_Hz, const int type, const int graph, const int mem, const int trace)
{
	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	if (freq_Hz > 0)
	{
		t_marker_freq marker;

		if (freq_Hz < min_Hz || freq_Hz > max_Hz)
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"Invalid frequency", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		// make sure we don't already have a marker with the same frequency
		for (unsigned int i = 0; i < settings.m_markers_freq.size(); i++)
		{
			if (settings.m_markers_freq[i].Hz == freq_Hz)
				return;
		}

		marker.Hz    = freq_Hz;
		marker.type  = type;
		marker.graph = graph;
		marker.mem   = mem;
		marker.trace = trace;
		settings.m_markers_freq.push_back(marker);

		buildMarkerListBox();

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		//updateInfoPanel();

		DeleteFrequencyMarkersBitBtn->Enabled = !settings.m_markers_freq.empty();
	}
	else
	{
		String fs;

		t_marker_freq marker;

		if (!InputQuery("New marker", "Frequency (default MHz)", fs))
			return;
		fs = fs.Trim();
		if (fs.IsEmpty())
			return;

		double MHz = 0;
		if (!common.strToMHz(fs, MHz))
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"Invalid frequency", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		const int64_t Hz = I64ROUND(MHz * 1e6);
		if (Hz < min_Hz || Hz > max_Hz)
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"Invalid frequency", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		// make sure we don't already have a marker with the same frequency
		for (unsigned int i = 0; i < settings.m_markers_freq.size(); i++)
		{
			if (settings.m_markers_freq[i].Hz == Hz)
				return;
		}

		marker.Hz    = Hz;
		marker.type  = type;
		marker.graph = graph;
		marker.mem   = mem;
		marker.trace = trace;
		settings.m_markers_freq.push_back(marker);

		buildMarkerListBox();

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		//updateInfoPanel();

		DeleteFrequencyMarkersBitBtn->Enabled = !settings.m_markers_freq.empty();
	}
}

void __fastcall TForm1::deleteMarker(const int index)
{
	if (index >= 0 && index < (int)settings.m_markers_freq.size())
	{
//		const int mi = selectedMarker();

		// delete the marker
		settings.m_markers_freq.erase(settings.m_markers_freq.begin() + index);

		// deselect all markers
		for (int i = 0; i < MarkerListBox->Items->Count; i++)
			MarkerListBox->Selected[i] = false;

		if (graphs.m_mouse.marker_index == index)
		{
			graphs.m_mouse.marker_mem   = -1;
			graphs.m_mouse.marker_index = -1;
		}

		buildMarkerListBox();

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		//updateInfoPanel();

		DeleteFrequencyMarkersBitBtn->Enabled = !settings.m_markers_freq.empty();
	}
}

void __fastcall TForm1::moveMarker(const int index, const int64_t freq_Hz)
{
	if (index >= 0 && index < (int)settings.m_markers_freq.size())
	{
		if (settings.m_markers_freq[index].Hz != freq_Hz)
		{
			settings.m_markers_freq[index].Hz = freq_Hz;

			buildMarkerListBox();

			if (Application->MainForm)
				::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
			//updateInfoPanel();

			DeleteFrequencyMarkersBitBtn->Enabled = !settings.m_markers_freq.empty();
		}
	}
}

int __fastcall TForm1::nonDeltaMarker(int start_index)
{
	if (start_index < 0)
		start_index = 0;

	int mi = -1;
	for (int i = start_index; i < (int)settings.m_markers_freq.size(); i++)
	{
		if (settings.m_markers_freq[i].type != MARKER_TYPE_DELTA)
		{	// found first non-delta marker
			mi = i;
			break;
		}
	}
	return mi;
}

int __fastcall TForm1::selectedMarker()
{
	int mi = -1;
	for (int i = 0; i < MarkerListBox->Items->Count; i++)
	{
		if (MarkerListBox->Selected[i])
		{
			mi = (int)MarkerListBox->Items->Objects[i];
			if (mi >= 0)
				break;
		}
	}
	return mi;
}

void __fastcall TForm1::updateNumberOfPointsIndicator(const int index)
{
	String s;
	if (index >= 0)
		s.printf(L"Points %5d / %5u", index, data_unit.m_point.size());
	else
		s.printf(L"Points %5d / %5u", 0, data_unit.m_point.size());
	StatusBar1->Panels->Items[1]->Text = s;
//	StatusBar1->Update();
}

void __fastcall TForm1::resetFreqArray()
{
	String s;

	const int mem = 0;   // live memory

	data_unit.m_point.resize(0);

	data_unit.m_point_mem[mem].resize(0);
	data_unit.m_point_filt[mem].resize(0);

	data_unit.m_total_frames = 0;

	data_unit.m_history_index  = 0;
	data_unit.m_history_frames = 0;

	data_unit.m_segment = 0;

//	m_start_bin = 0;

	m_freq_data_list.resize(0);

	updateNumberOfPointsIndicator();

	if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
		s.printf(L"Seg %d / %d", 1 + data_unit.m_segment, data_unit.m_segments);
	else
		s.printf(L"Seg %d / 1", 1 + data_unit.m_segment);
	StatusBar1->Panels->Items[2]->Text = s;

	updateHistoryFramesInfo();

	s.printf(L"Scans %d", data_unit.m_total_frames);
	StatusBar1->Panels->Items[4]->Text = s;

	updateStepDisplay();
}

void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
//	bool shift = (Shift.Contains(ssShift)) ? true : false;
//	bool ctrl  = (Shift.Contains(ssCtrl))  ? true : false;
//	bool alt   = (Shift.Contains(ssAlt))   ? true : false;

	switch (Key)
	{
		case VK_ESCAPE:
			Key = 0;
			if (connected())
			{
				if (scanning())
				{
					stop();
				}
				else
				{
					m_comms.serial.Disconnect();
					tcpipClientDisconnect();
				}
			}
			else
				Close();
			break;

		case VK_PRIOR:	// page up
			Key = 0;
			//if (!shift)	// shift key not press
			{	// zoom frequency
				int64_t center_Hz = data_unit.m_freq_center_Hz;
				int64_t span_Hz = I64ROUND(data_unit.m_freq_span_Hz / 1.5f);

				if (span_Hz < 10000)
					span_Hz = 10000;

				span_Hz = I64ROUND((double)span_Hz / 1000) * 1000;	// 1kHz rounding

				int64_t start_Hz = center_Hz - (span_Hz / 2);
				int64_t stop_Hz  = center_Hz + (span_Hz / 2);

				setStartStopHz(start_Hz, stop_Hz, false);
			}
			break;

		case VK_NEXT:	// page down
			Key = 0;
			//if (!shift)	// shift key not press
			{	// zoom frequency

				int64_t max_Hz;
				int64_t min_Hz;
				data_unit.minMaxFreqHz(min_Hz, max_Hz);

				int64_t center_Hz = data_unit.m_freq_center_Hz;
				int64_t span_Hz   = I64ROUND(data_unit.m_freq_span_Hz * 1.5f);

				int64_t start_Hz = center_Hz - (span_Hz / 2);
				int64_t stop_Hz  = center_Hz + (span_Hz / 2);

				if (start_Hz < min_Hz)
					span_Hz = (center_Hz - min_Hz) * 2;
				if (stop_Hz > max_Hz)
					span_Hz = (max_Hz - center_Hz) * 2;

				span_Hz = I64ROUND((double)span_Hz / 1000) * 1000;	// 1kHz rounding

				start_Hz = center_Hz - (span_Hz / 2);
				stop_Hz  = center_Hz + (span_Hz / 2);

				setStartStopHz(start_Hz, stop_Hz, false);
			}
			break;
	}
}

void __fastcall TForm1::WMWindowPosChanging(TWMWindowPosChanging &msg)
{
	const int thresh = WINDOW_SNAP;

	if (msg.WindowPos->flags & SWP_STATECHANGED)
	{
		if (msg.WindowPos->flags & SWP_FRAMECHANGED)
		{
			if (msg.WindowPos->x < 0 && msg.WindowPos->y < 0)
			{	// Window state is about to change to MAXIMIZED
				if ((msg.WindowPos->flags & (SWP_SHOWWINDOW | SWP_NOACTIVATE)) == (SWP_SHOWWINDOW | SWP_NOACTIVATE))
				{	// about to minimize
					return;
				}
				else
				{	// about to maximize
					return;
				}
			}
			else
			{	// about to normalize
			}
		}
	}

	if (msg.WindowPos->hwnd != this->Handle || Screen == NULL)
		return;

	const int dtLeft   = Screen->DesktopRect.left;
	//const int dtRight  = Screen->DesktopRect.right;
	const int dtTop    = Screen->DesktopRect.top;
	const int dtBottom = Screen->DesktopRect.bottom;
	const int dtWidth  = Screen->DesktopRect.Width();
	const int dtHeight = Screen->DesktopRect.Height();

	//const int waLeft   = Screen->WorkAreaRect.left;
	//const int waRight  = Screen->WorkAreaRect.right;
	//const int waTop    = Screen->WorkAreaRect.top;
	//const int waBottom = Screen->WorkAreaRect.bottom;
	//const int waWidth  = Screen->WorkAreaRect.Width();
	//const int waHeight = Screen->WorkAreaRect.Height();

	int x = msg.WindowPos->x;
	int y = msg.WindowPos->y;
	int w = msg.WindowPos->cx;
	int h = msg.WindowPos->cy;

	for (int i = 0; i < Screen->MonitorCount; i++)
	{	// sticky screen edges
		const int mLeft   = Screen->Monitors[i]->WorkareaRect.left;
		const int mRight  = Screen->Monitors[i]->WorkareaRect.right;
		const int mTop    = Screen->Monitors[i]->WorkareaRect.top;
		const int mBottom = Screen->Monitors[i]->WorkareaRect.bottom;
		const int mWidth  = Screen->Monitors[i]->WorkareaRect.Width();
		const int mHeight = Screen->Monitors[i]->WorkareaRect.Height();

		if (ABS(x - mLeft) < thresh)
				  x = mLeft;			// stick left to left side
		else
		if (ABS((x + w) - mRight) < thresh)
					x = mRight - w;	// stick right to right side

		if (ABS(y - mTop) < thresh)
				  y = mTop;				// stick top to top side
		else
		if (ABS((y + h) - mBottom) < thresh)
					y = mBottom - h;	// stick bottom to bottm side

		// stick the right side to the right side of the screen if the left side is stuck to the left side of the screen
		if (x == mLeft)
			if ((w >= (mWidth - thresh)) && (w <= (mWidth + thresh)))
				w = mWidth;

		// stick the bottom to the bottom of the screen if the top is stuck to the top of the screen
		if (y == mTop)
			if ((h >= (mHeight - thresh)) && (h <= (mHeight + thresh)))
				h = mHeight;
	}
/*
	{	// sticky screen edges
		if (ABS(x - waLeft) < thresh)
			x = waLeft;			// stick left to left side
		else
		if (ABS((x + w) - waRight) < thresh)
			x = waRight - w;	// stick right to right side

		if (ABS(y - waTop) < thresh)
			y = waTop;			// stick top to top side
		else
		if (ABS((y + h) - waBottom) < thresh)
			y = waBottom - h;	// stick bottom to bottm side

		// stick the right side to the right side of the screen if the left side is stuck to the left side of the screen
		if (x == waLeft)
			if ((w >= (waWidth - thresh)) && (w <= (waWidth + thresh)))
				w = waWidth;

		// stick the bottom to the bottom of the screen if the top is stuck to the top of the screen
		if (y == waTop)
			if ((h >= (waHeight - thresh)) && (h <= (waHeight + thresh)))
				h = waHeight;
	}
*/
	// limit minimum size
	if (w < Constraints->MinWidth)
		 w = Constraints->MinWidth;
	if (h < Constraints->MinHeight)
		 h = Constraints->MinHeight;

	// limit maximum size
	if (w > Constraints->MaxWidth && Constraints->MaxWidth > Constraints->MinWidth)
		 w = Constraints->MaxWidth;
	if (h > Constraints->MaxHeight && Constraints->MaxHeight > Constraints->MinHeight)
		 h = Constraints->MaxHeight;

	// limit maximum size
	if (w > dtWidth)
		 w = dtWidth;
	if (h > dtHeight)
		 h = dtHeight;
/*
	if (Application->MainForm && this != Application->MainForm)
	{	// stick to our main form sides
		const TRect rect = Application->MainForm->BoundsRect;

		if (ABS(x - rect.left) < thresh)
			x = rect.left;			// stick to left to left side
		else
		if (ABS((x + w) - rect.left) < thresh)
			x = rect.left - w;	// stick right to left side
		else
		if (ABS(x - rect.right) < thresh)
			x = rect.right;		// stick to left to right side
		else
		if (ABS((x + w) - rect.right) < thresh)
			x = rect.right - w;	// stick to right to right side

		if (ABS(y - rect.top) < thresh)
			y = rect.top;			// stick top to top side
		else
		if (ABS((y + h) - rect.top) < thresh)
			y = rect.top - h;		// stick bottom to top side
		else
		if (ABS(y - rect.bottom) < thresh)
			y = rect.bottom;		// stick top to bottom side
		else
		if (ABS((y + h) - rect.bottom) < thresh)
			y = rect.bottom - h;	// stick bottom to bottom side
	}
*/
	// stop it completely leaving the desktop area
	if (x < (dtLeft - Width + (dtWidth / 15)))
		 x =  dtLeft - Width + (dtWidth / 15);
	if (x > (dtWidth - (Screen->Width / 15)))
		 x =  dtWidth - (Screen->Width / 15);
	if (y < dtTop)
		 y = dtTop;
	if (y > (dtBottom - (dtHeight / 10)))
		 y =  dtBottom - (dtHeight / 10);

	msg.WindowPos->x  = x;
	msg.WindowPos->y  = y;
	msg.WindowPos->cx = w;
	msg.WindowPos->cy = h;
}

void __fastcall TForm1::init()
{
	String s;
	TNotifyEvent ne;
/*
	// TEST ONLY
	s.printf(L"%d %d %d %d",
		settings.mainWindowPos.top,
		settings.mainWindowPos.left,
		settings.mainWindowPos.width,
		settings.mainWindowPos.height);
	Label5->Caption = s;
*/
	this->Top    = settings.mainWindowPos.top;
	this->Left   = settings.mainWindowPos.left;
	this->Width  = settings.mainWindowPos.width;
	this->Height = settings.mainWindowPos.height;

	if (!settings.calibrationFolder.IsEmpty())
		common.createPath(AnsiString(settings.calibrationFolder).c_str());
	if (!settings.recordFolder.IsEmpty())
		common.createPath(AnsiString(settings.recordFolder).c_str());

	InfoPanelToggleSwitch->State = settings.infoPanel ? tssOn : tssOff;

	{
		ne = TCPIPAddressEdit->OnChange;
		TCPIPAddressEdit->OnChange = NULL;
		TCPIPAddressEdit->Text = settings.tcpipAddress;
		TCPIPAddressEdit->OnChange = ne;
	}

	{
		ne = TCPIPPortEdit->OnChange;
		TCPIPPortEdit->OnChange = NULL;
		TCPIPPortEdit->Text = settings.tcpipPort;
		TCPIPPortEdit->OnChange = ne;
	}

	{
		ne = DeviceComboBox->OnChange;
		DeviceComboBox->OnChange = NULL;
		int i = DeviceComboBox->Items->IndexOf(settings.serialPortName);
		if (i > 0)
			DeviceComboBox->ItemIndex = i;
		DeviceComboBox->OnChange = ne;
	}

	{
		ne = StartMHzEdit->OnChange;
		StartMHzEdit->OnChange = NULL;
		StartMHzEdit->Text = common.freqToStrMHz(settings.startHz);
		StartMHzEdit->OnChange = ne;
	}

	{
		ne = StopMHzEdit->OnChange;
		StopMHzEdit->OnChange = NULL;
		StopMHzEdit->Text = common.freqToStrMHz(settings.stopHz);
		StopMHzEdit->OnChange = ne;
	}

	{
		ne = CenterMHzEdit->OnChange;
		CenterMHzEdit->OnChange = NULL;
		CenterMHzEdit->Text = common.freqToStrMHz((settings.stopHz + settings.startHz) / 2);
		CenterMHzEdit->OnChange = ne;
	}

	{
		ne = SpanMHzEdit->OnChange;
		SpanMHzEdit->OnChange = NULL;
		SpanMHzEdit->Text = common.freqToStrMHz(settings.stopHz - settings.startHz);
		SpanMHzEdit->OnChange = ne;
	}

	{
		ne = CWMHzEdit->OnChange;
		CWMHzEdit->OnChange = NULL;
		CWMHzEdit->Text = common.freqToStrMHz(settings.cwHz);
		CWMHzEdit->OnChange = ne;
	}

	{
		ne = VelocityFactorEdit->OnChange;
		VelocityFactorEdit->OnChange = NULL;
		s.printf(L"%0.3f", settings.velocityFactor);
		VelocityFactorEdit->Text = s;
		VelocityFactorEdit->OnChange = ne;
	}

	{
		TComboBox *cb = PointBandwidthHzComboBox;
		ne = cb->OnChange;
		cb->OnChange = NULL;
		const int i = cb->Items->IndexOfObject((TObject *)settings.pointBandwidth);
		if (i >= 0)
			cb->ItemIndex = i;
		else
			cb->Text = IntToStr(settings.pointBandwidth);
		cb->OnChange = ne;
	}

	{
		ne = NumberOfPointsComboBox->OnChange;
		NumberOfPointsComboBox->OnChange = NULL;
		const int i = NumberOfPointsComboBox->Items->IndexOfObject((TObject *)settings.numOfPoints);
		NumberOfPointsComboBox->ItemIndex = (i >= 0) ? i : 1;
		NumberOfPointsComboBox->OnChange = ne;
	}

	{
		ne = TimeAverageLevelTrackBar->OnChange;
		TimeAverageLevelTrackBar->OnChange = NULL;
//		TimeAverageLevelTrackBar->Position = settings.timeAverageLevel;
		TimeAverageLevelTrackBar->OnChange = ne;
	}

	{
		ne = MedianFilterTrackBar->OnChange;
		MedianFilterTrackBar->OnChange = NULL;
//		MedianFilterTrackBar->Position = settings.medianFilterLevel;
		MedianFilterTrackBar->OnChange = ne;
	}

	{
		ne = CurveSmoothingTrackBar->OnChange;
		CurveSmoothingTrackBar->OnChange = NULL;
//		CurveSmoothingTrackBar->Position = settings.curveSmoothingLevel;
		CurveSmoothingTrackBar->OnChange = ne;
	}

	{
		ne = TDRWindowTrackBar->OnChange;
		TDRWindowTrackBar->OnChange = NULL;
		TDRWindowTrackBar->Position = settings.tdrWindow;
		TDRWindowTrackBar->OnChange = ne;
	}

	{
		ne = OutputPowerTrackBar->OnChange;
		OutputPowerTrackBar->OnChange = NULL;
//		OutputPowerTrackBar->Position = settings.outputPower;
		OutputPowerTrackBar->OnChange = ne;
	}

	{
		ne = LCMatchingToggleSwitch->OnClick;
		LCMatchingToggleSwitch->OnClick = NULL;
		LCMatchingToggleSwitch->State = settings.lcMatchingEnable ? tssOn : tssOff;
		LCMatchingToggleSwitch->OnClick = ne;
	}

	{
		const int i = VelocityFactorComboBox->Items->IndexOf(settings.velocityFactorName.Trim());
		if (i >= 0)
			VelocityFactorComboBox->ItemIndex = i;
	}

	{
		ne = CalibrationSelectComboBox->OnChange;
		CalibrationSelectComboBox->OnChange = NULL;
		const int i = CalibrationSelectComboBox->Items->IndexOfObject((TObject *)settings.calibrationSelection);
		if (i >= 0)
			CalibrationSelectComboBox->ItemIndex = i;
		CalibrationSelectComboBox->OnChange = ne;
	}

	{
		ne = SweepNameEdit->OnChange;
		SweepNameEdit->OnChange = NULL;
		SweepNameEdit->Text = settings.sweepName;
		SweepNameEdit->OnChange = ne;
	}

	{
		ne = EDelayEdit->OnChange;
		EDelayEdit->OnChange = NULL;
		s.printf(L"%#.6f", settings.eDelaySecs * 1e12);	// pico seconds
		EDelayEdit->Text = common.trimTrailingZeros(s);
		EDelayEdit->OnChange = ne;
	}

	{
		ne = EnableNormaliseSpeedButton->OnClick;
		EnableNormaliseSpeedButton->OnClick = NULL;
		EnableNormaliseSpeedButton->Down = settings.normalisationEnabled;
		EnableNormaliseSpeedButton->OnClick = ne;
	}

	s.printf(L"%0.6f", settings.s21OffsetdB);
	S21OffsetEdit->Text = common.trimTrailingZeros(s);

	FreqBandEnableToggleSwitch->State = settings.showFrequencyBands ? tssOn : tssOff;

	ShowMarkersOnGraphToggleSwitch->State = settings.showMarkersOnGraph ? tssOn : tssOff;
	ShowMarkerTextToggleSwitch->State     = settings.showMarkerText ?     tssOn : tssOff;

	setCalibrationSelection(settings.calibrationSelection);

	NumberOfPointsComboBoxChange(NumberOfPointsComboBox);
	TimeAverageLevelTrackBarChange(TimeAverageLevelTrackBar);
	VelocityFactorComboBoxChange(VelocityFactorComboBox);
	MedianFilterTrackBarChange(MedianFilterTrackBar);
	TDRWindowTrackBarChange(TDRWindowTrackBar);
	CurveSmoothingTrackBarChange(CurveSmoothingTrackBar);
	TCPIPAddressPortEditChange(NULL);

	DeleteFrequencyMarkersBitBtn->Enabled = !settings.m_markers_freq.empty();

	updateStepDisplay();

	s.printf(L"%0.6f", settings.s21OffsetdB);
	S21OffsetEdit->Text = common.trimTrailingZeros(s);

	InfoPanel->Visible = settings.infoPanel;

	OutputPowerTrackBarChange(OutputPowerTrackBar);

	// test only
//	ScanSpanTrackBarChange(ScanSpanTrackBar);

	{
		const int i = SerialPortBaudrateComboBox->Items->IndexOfObject((TObject *)settings.serialPortBaudrate);
		if (i >= 0)
			SerialPortBaudrateComboBox->ItemIndex = i;
	}

	fetchUserInput();

	updateDateTime();

	if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
		s.printf(L"Seg %d / %d", 1 + data_unit.m_segment, data_unit.m_segments);
	else
		s.printf(L"Seg %d / 1", 1 + data_unit.m_segment);
	StatusBar1->Panels->Items[2]->Text = s;

	updateHistoryFramesInfo();

	s.printf(L"Scans %d", data_unit.m_total_frames);
	StatusBar1->Panels->Items[4]->Text = s;

	settings.setColourStyle(settings.guiStyleName);

//	if (DeviceComboBox->CanFocus())
//		SerialPortComboBox->SetFocus();

//	if (Application->MainForm)
//		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);

	//	BringToFront();
//	::SetForegroundWindow(Handle);

//	if (Application->MainForm)
//		Application->MainForm->Update();

	#if 1
		janvna2_comms.listDevices();
	#endif

//	Timer1->Enabled = true;
}

void __fastcall TForm1::WMInitGUI(TMessage &msg)
{

	// load the memories back in
	for (int mem = 0; mem < MAX_MEMORIES; mem++)
	{
		String filename;

		filename.printf(L"mem_%d.s2p", mem);
		//filename.printf(L"mem_%d.csv", mem);
		filename = ExtractFilePath(Application->ExeName) + filename;

		if (!FileExists(filename))
			continue;

		std::vector <t_data_point> s_params;

		// read the file in
		String fn = common.loadSParams(s_params, filename);
		//String fn = common.loadCSV(s_params, filename);

		if (fn.IsEmpty())
			continue;	// file not read in

		data_unit.m_point_mem[mem] = s_params;
		data_unit.m_point_filt[mem].resize(0);

		TSpeedButton *sb = NULL;
		switch (mem)
		{
			case 0: sb = MemorySpeedButton0; break;
			case 1: sb = MemorySpeedButton1; break;
			case 2: sb = MemorySpeedButton2; break;
			case 3: sb = MemorySpeedButton3; break;
			case 4: sb = MemorySpeedButton4; break;
		}
		if (sb == NULL)
			continue;

		String s = settings.memoryName[mem];
		if (s.IsEmpty())
		{
			const int size = data_unit.m_point_mem[mem].size();
			if (size > 0)
			{
				s  = common.freqToStr1(data_unit.m_point_mem[mem][0].Hz, true, false, 6, false) + "Hz";
				s += " to ";
				s += common.freqToStr1(data_unit.m_point_mem[mem][size - 1].Hz, true, false, 6, false) + "Hz";
			}
			s += " " + IntToStr(size) + " points";
		}

		const TNotifyEvent ne = sb->OnClick;
		sb->OnClick = NULL;
		sb->Down    = settings.memoryEnable[mem];
		sb->Caption = (mem <= 0) ? String("Live") : "M" + IntToStr(mem);
		sb->Hint    = s;
		sb->OnClick = ne;
	}

	{  // load the normalisation memory back in
		String filename = ExtractFilePath(Application->ExeName) + "norm.s2p";
		if (FileExists(filename))
		{	// read the file in
			String fn = common.loadSParams(data_unit.m_point_norm, filename);
			if (!fn.IsEmpty())
			{
				EnableNormaliseSpeedButton->Enabled = true;

				String s;
				const int size = data_unit.m_point_norm.size();
				if (size > 0)
				{
					s  = common.freqToStr1(data_unit.m_point_norm[0].Hz, true, false, 6, false) + "Hz";
					s += " to ";
					s += common.freqToStr1(data_unit.m_point_norm[size - 1].Hz, true, false, 6, false) + "Hz";
				}
				s += " " + IntToStr(size) + " points";
				EnableNormaliseSpeedButton->Hint = s;
			}
		}
	}

	{	// the font style is disabled on the statusbars so we need to manually set the font colour to match the them style
		TColor colour = TStyleManager::ActiveStyle->GetStyleFontColor(sfStatusPanelTextNormal);
		StatusBar1->Font->Color = colour;
		StatusBar2->Font->Color = colour;
	}

	// ensure this window is completely drawn
	this->Update();

	if (FileExists(settings.calibrationFile) && settings.calibrationSelection==CAL_SELECT_APP)
	{
		if (CalibrationForm)
			CalibrationForm->loadCalibration(settings.calibrationFile);
/*
		std::vector <t_calibration_point> calibration_points;
		String filename = common.loadCalibrationFile(settings.calibrationFile, calibration_points);
		if (!filename.IsEmpty() && !calibration_points.empty())
		{
			calibration_module.m_calibration.filename = filename;
			calibration_module.m_calibration.name     = TPath::GetFileNameWithoutExtension(filename);
			calibration_module.m_calibration.point    = calibration_points;

			// compute the error terms for the correction function
			calibration_module.computeErrorTerms(calibration_module.m_calibration);
		}
*/
	}

	if (settings.commsWindowPos.showing && CommsForm)
		CommsForm->show();

	if (settings.vnaUSARTcommsWindowPos.showing && VNAUsartCommsForm)
		VNAUsartCommsForm->show();

	if (settings.batteryWindowPos.showing && BatteryVoltageForm)
		BatteryVoltageForm->show();

//	if (settings.settingsWindowPos.showing && SettingsForm)
//		SettingsForm->show();

//	if (settings.firmwareWindowPos.showing && UploadFirmwareForm)
//		UploadFirmwareForm->show();

//	if (settings.firmwareV2WindowPos.showing && UploadFirmwareV2Form)
//		UploadFirmwareV2Form->show();

	if (settings.calibrationWindowPos.showing && CalibrationForm)
		CalibrationForm->show();

//	if (settings.screenCaptureWindowPos.showing && VNAScreenCaptureForm)
//		VNAScreenCaptureForm->show();

	// ******************
	// we want to receive notifications for serial device plug-in/out
/*
	usb_devices.clear();

	memset(&m_DfuDesc, 0, sizeof(DFU_FUNCTIONAL_DESCRIPTOR));
	m_DfuInterfaceIdx = 0;
	m_NbOfAlternates = 0;
*/

	#if 1
		{	// USB serial comport
			DEV_BROADCAST_DEVICEINTERFACE dbch;
			memset(&dbch, 0, sizeof(dbch));
			dbch.dbcc_size       = sizeof(dbch);
			dbch.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
			dbch.dbcc_classguid  = GUID_DEVINTERFACE_COMPORT;
			HDEVNOTIFY handle    = ::RegisterDeviceNotification(this->Handle, &dbch, DEVICE_NOTIFY_WINDOW_HANDLE);
			if (handle != NULL)
				m_notification_handle.push_back(handle);
		}
	#endif

	#if 0
		// NanoVNA V2
		for (int i = 0; i < ARRAYSIZE(GUID_DEVINTERFACE_NANOVNA_V2); i++)
		{
			DEV_BROADCAST_DEVICEINTERFACE dbch;
			memset(&dbch, 0, sizeof(dbch));
			dbch.dbcc_size = sizeof(dbch);
			dbch.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
			dbch.dbcc_classguid = GUID_DEVINTERFACE_NANOVNA_V2[i];
			HDEVNOTIFY handle = ::RegisterDeviceNotification(this->Handle, &dbch, DEVICE_NOTIFY_WINDOW_HANDLE);
			if (handle != NULL)
				m_notification_handle.push_back(handle);
		}
	#endif

	#if 0	// all devices
		for (int i = 0; i < ARRAYSIZE(GUID_DEVINTERFACE_ALL); i++)
		{
			DEV_BROADCAST_DEVICEINTERFACE dbch;
			memset(&dbch, 0, sizeof(dbch));
			dbch.dbcc_size = sizeof(dbch);
			dbch.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
			dbch.dbcc_classguid = GUID_DEVINTERFACE_ALL[i];
			HDEVNOTIFY handle = ::RegisterDeviceNotification(this->Handle, &dbch, DEVICE_NOTIFY_WINDOW_HANDLE);
			if (handle != NULL)
				m_notification_handle.push_back(handle);
		}
	#endif

	#if 0
		{	// DFU device
			DEV_BROADCAST_DEVICEINTERFACE dbch;
			memset(&dbch, 0, sizeof(dbch));
			dbch.dbcc_size       = sizeof(dbch);
			dbch.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
			dbch.dbcc_classguid  = GUID_DEVINTERFACE_USB_DEVICE;
			HDEVNOTIFY handle    = ::RegisterDeviceNotification(this->Handle, &dbch, DEVICE_NOTIFY_WINDOW_HANDLE);
			if (handle != NULL)
				m_notification_handle.push_back(handle);
		}
	#endif

	#if 0
		for (int i = 0; i < ARRAYSIZE(GUID_DEVINTERFACE_FTDI); i++)
		{
			DEV_BROADCAST_DEVICEINTERFACE dbch;
			memset(&dbch, 0, sizeof(dbch));
			dbch.dbcc_size = sizeof(dbch);
			dbch.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
			//dbch.dbcc_classguid = FTDI_D2XX_GUID;	// FTDI_VCP_GUID;
			dbch.dbcc_classguid = GUID_DEVINTERFACE_FTDI[i];
			HDEVNOTIFY handle = ::RegisterDeviceNotification(this->Handle, &dbch, DEVICE_NOTIFY_WINDOW_HANDLE);
			if (handle != NULL)
				m_notification_handle.push_back(handle);
		}
	#endif

	// ******************

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);

	this->SetFocus();
	this->BringToFront();

	Timer1->Enabled = true;
}

void __fastcall TForm1::WMConnect(TMessage &msg)
{
	connect();
}

void __fastcall TForm1::WMDisconnect(TMessage &msg)
{
	disconnect();
}

void __fastcall TForm1::WMCaptureScan(TMessage &msg)
{
	scan();
}

void __fastcall TForm1::WMCaptureStop(TMessage &msg)
{
	stop();
}

void __fastcall TForm1::WMScreenCapture(TMessage &msg)
{
	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{
		if (nanovna2_comms.m_capture_bm != NULL)
			if (nanovna2_comms.m_capture_bm->Width > 0 && nanovna2_comms.m_capture_bm->Height > 0)
				if (VNAScreenCaptureForm)
					VNAScreenCaptureForm->addImage(nanovna2_comms.m_capture_bm);
	}
	else
	{
		if (nanovna1_comms.m_capture_bm != NULL)
			if (nanovna1_comms.m_capture_bm->Width > 0 && nanovna1_comms.m_capture_bm->Height > 0)
				if (VNAScreenCaptureForm)
					VNAScreenCaptureForm->addImage(nanovna1_comms.m_capture_bm);
	}
}

void __fastcall TForm1::WMComDeviceChanged(TMessage &msg)
{
	const DWORD wparam = msg.WParam;
	const DWORD lparam = msg.LParam;

	if (wparam != 0)
	{	// connected
		BOOL sound_played = FALSE;
		#ifdef _INC_MMSYSTEM
			if (data_unit.squeak_wav.size() > MIN_WAV_SIZE)
				sound_played = PlaySound(&data_unit.squeak_wav[0], NULL, SND_MEMORY | SND_NODEFAULT | SND_NOWAIT | SND_ASYNC);
		#endif
		if (sound_played == FALSE)
			Beep(440, 80);
	}
	else
	{	// disconnected
		BOOL sound_played = FALSE;
		#ifdef _INC_MMSYSTEM
			if (data_unit.phurp_wav.size() > MIN_WAV_SIZE)
				sound_played = PlaySound(&data_unit.phurp_wav[0], NULL, SND_MEMORY | SND_NODEFAULT | SND_NOWAIT | SND_ASYNC);
		#endif
		if (sound_played == FALSE)
			Beep(349, 80);

		updateDeviceComboBox();

		// get a list of available com-ports
		std::vector <T_SerialPortInfo> serial_port_list;
		m_comms.serial.GetSerialPortList(serial_port_list);

		if (nanovna1_comms.mode != MODE_NONE || nanovna2_comms.mode != MODE_NONE)
		{
			// find out if the connected serial port is still in the list
			String s = m_comms.serial.deviceName.LowerCase();
			int index = -1;
			for (unsigned int i = 0; i < serial_port_list.size(); i++)
			{
				String pn = String(serial_port_list[i].name).LowerCase();
				if (s == pn)
				{
					index = i;
					break;
				}
			}
			if (index < 0)
			{	// serial port no longer present
				disconnect();
			}
		}
	}
}

void __fastcall TForm1::WMUpdateEDelay(TMessage &msg)
{
	updateEDelayEdit();
}

void __fastcall TForm1::WMUpdateThresholdHz(TMessage &msg)
{
	StatusBar2->Panels->Items[3]->Text = "Threshold " + common.freqToStr1(data_unit.m_vna_data.freq_threshold_Hz, true, true, 6, false);

	if (SettingsForm)
		SettingsForm->updateThresholdHz();
}

void __fastcall TForm1::WMUpdatePointBandwidth(TMessage &msg)
{
	const int point_bandwidth = (int)msg.WParam;

	if (point_bandwidth > 0)
	{
		String s;
		s.printf(L"%d", point_bandwidth);
		if (PointBandwidthHzComboBox->Text != s)
			PointBandwidthHzComboBox->Text = s;
		common.setWarning(PointBandwidthHzComboBox, "");
	}

	StatusBar2->Panels->Items[4]->Text = "BW " + common.freqToStr1(point_bandwidth, true, true, 3, false);
}

void __fastcall TForm1::WMUpdateOutputPower(TMessage &msg)
{
	const int output_power = (int)msg.WParam;
	updateOutputPowerTrackBar(output_power);
}

void __fastcall TForm1::WMUpdateBatteryVoltage(TMessage &msg)
{
	const int vbat_mv        = (int)msg.WParam;
	const int vbat_offset_mv = (int)msg.LParam;

	String s;
	s.printf(L"Bat %0.3f V", (float)vbat_mv / 1000);
	StatusBar2->Panels->Items[2]->Text = s;

	if (BatteryVoltageForm)
		BatteryVoltageForm->addBatteryVoltage(vbat_mv, vbat_offset_mv);

	if (SettingsForm && vbat_offset_mv > 0)
		SettingsForm->updateVBatOffset();
}

void __fastcall TForm1::WMNewUnitType(TMessage &msg)
{
	if (UploadFirmwareForm)
		UploadFirmwareForm->Hide();
	if (UploadFirmwareV2Form)
		UploadFirmwareV2Form->Hide();

	StatusBar2->Panels->Items[5]->Text = data_unit.m_vna_data.name + " " + data_unit.m_vna_data.version;

	configGUI();

	if (data_unit.m_vna_data.type == UNIT_TYPE_TINYSA && CalibrationForm)
	{
		CalibrationForm->Hide();
		CalibrationBitBtn->Enabled = false;
	}
	else
	{
		CalibrationBitBtn->Enabled = true;
	}

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
		ScanOnceSpeedButton->Enabled    = true;
		ScanSpeedButton->Enabled        = true;
		CaptureVNAScreenBitBtn->Enabled = false;
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		if (nanovna2_comms.inDFUMode())
		{	// the unit is in DFU mode
			ScanOnceSpeedButton->Enabled    = false;
			ScanSpeedButton->Enabled        = false;
			CaptureVNAScreenBitBtn->Enabled = false;
		}
		else
		{
			ScanOnceSpeedButton->Enabled    = true;
			ScanSpeedButton->Enabled        = true;
			CaptureVNAScreenBitBtn->Enabled = true;
		}
	}
	else
	{	// V1
		ScanOnceSpeedButton->Enabled    = true;
		ScanSpeedButton->Enabled        = true;
		CaptureVNAScreenBitBtn->Enabled = true;
	}

	updateNumberOfPointsComboBox(true);

	updateCalibrationSelectComboBox();

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
		updatePointBandwidthComboBox();
		PointBandwidthHzComboBox->Enabled = true;
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		PointBandwidthHzComboBox->Enabled = false;
	}
	else
	{	// V1
		updatePointBandwidthComboBox();
		PointBandwidthHzComboBox->Enabled = true;
	}

	if (SettingsForm)
		SettingsForm->updateInfo();

	SetScanRangeToVNAScanRangeBitBtn->Enabled = (data_unit.m_vna_data.type != UNIT_TYPE_NANOVNA_V2) ? true : false;

	VNAUsartCommsBitBtn->Enabled = data_unit.m_vna_data.cmd_usart;

	if (VNAUsartCommsForm)
		if (!VNAUsartCommsBitBtn->Enabled || data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2 || data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2 || data_unit.m_vna_data.type == UNIT_TYPE_TINYSA)
			VNAUsartCommsForm->Hide();

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);

	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2 && nanovna2_comms.inDFUMode())
	{
		Application->NormalizeTopMosts();
		const int res = Application->MessageBox(L"Your NanoVNA V2 appears to be in firmware upload mode."
															"\n\nDo you want to upload new firmware to it?",
															Application->Title.w_str(),
															MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
		Application->RestoreTopMosts();

		if (res == IDYES)
		{	// upload new firmware
			if (UploadFirmwareV2Form)
				UploadFirmwareV2Form->show();
		}
		else
		{	// soft reboot the VNA
			nanovna2_comms.softReboot();
		}
	}

	if (BatteryVoltageForm && !BatteryVoltageBitBtn->Enabled)
		BatteryVoltageForm->Hide();
}

void __fastcall TForm1::WMNewData(TMessage &msg)
{
	String s;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);

	const int segment  = (int)msg.WParam;
	const int segments = (int)msg.LParam;

	// fetch the latest scanned points
	if (segment >= (segments - 1) && data_unit.m_history_frames > 0)
	{
		const int size = data_unit.freqArraySize(-1);
		if (size > 0 && size == data_unit.m_points)
		{
			int hist_index = data_unit.m_history_index - 1;	// index of latest frame
			if (hist_index < 0)
				hist_index += (int)MAX_HISTORY;

			std::vector <t_data_point> new_points(size);

			for (int i = 0; i < size; i++)
			{
				new_points[i].Hz  = data_unit.m_point[i].Hz;
				new_points[i].s11 = data_unit.m_point[i].hist[hist_index].s11;
				new_points[i].s21 = data_unit.m_point[i].hist[hist_index].s21;
				new_points[i].s12 = data_unit.m_point[i].hist[hist_index].s12;
				new_points[i].s22 = data_unit.m_point[i].hist[hist_index].s22;
			}

			if (CalibrationForm && data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
				CalibrationForm->scanComplete(new_points);

//			if (!settings.useVNACalibration && data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
//				calibration.correct(calibration_module.m_calibration, points, true, true, true);	// use calibrations to correct

			recordDataToFile(new_points);	// stream the incoming (raw uncorrected) sparams to files
		}

		updateHistoryFramesInfo();

		updateStepDisplay();
	}

	updateNumberOfPointsIndicator();

	s.printf(L"Seg %d / %d", 1 + segment, segments);
	StatusBar1->Panels->Items[2]->Text = s;

	s.printf(L"Scans %d", data_unit.m_total_frames);
	StatusBar1->Panels->Items[4]->Text = s;

	s.printf(L"%d points/sec", IROUND(m_comms.points_per_sec));
	if (StatusBar2->Panels->Items[1]->Text != s)
		StatusBar2->Panels->Items[1]->Text = s;
}

void __fastcall TForm1::WMIncomingPoints(TMessage &msg)
{
	updateNumberOfPointsIndicator(msg.WParam);

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::WMUpdateInfoPanel(TMessage &msg)
{
	updateInfoPanel();
}

void __fastcall TForm1::WMUpdateGraph(TMessage &msg)
{
	#ifdef USE_OPENGL
		if (this->WindowState != wsMinimized)
			graphs.glRenderScene(GLPanel, true, settings.graphType[0], settings.graphType[1], settings.graphType[2], settings.graphType[3], graphs.numberOfGraphs(), HistoryTrackBar->Position);
	#else
		GraphPaintBox->Refresh();
		for (unsigned int i = 0; i < m_graph_form.size(); i++)
		{
			TGraphForm *graph_form = m_graph_form[i];
			if (graph_form)
				if (graph_form->Showing)
				graph_form->GraphPaintBox->Invalidate();
		}
	#endif
}

void __fastcall TForm1::WMRebuildFonts(TMessage &msg)
{
	#ifdef USE_OPENGL
		graphs.glQueuRebuildFonts();
	#endif
	WMUpdateGraph(msg);
}

void __fastcall TForm1::WMGraphFormClosed(TMessage &msg)
{
	#ifdef GraphUnitH
		for (unsigned int i = 0; i < m_graph_form.size(); i++)
		{
			TGraphForm *graph_form = m_graph_form[i];
			if ((WPARAM)graph_form == msg.WParam)
			{
				#ifdef _DEBUG
					Beep(1000, 50);
				#endif
				m_graph_form.erase(m_graph_form.begin() + i);
				arrangeGraphs();
				break;
			}
		}
	#endif
}

void __fastcall TForm1::WMGraphFormDestroyed(TMessage &msg)
{
	#ifdef GraphUnitH
		for (unsigned int i = 0; i < m_graph_form.size(); i++)
		{
			TGraphForm *graph_form = m_graph_form[i];
			if ((WPARAM)graph_form == msg.WParam)
			{
				#ifdef _DEBUG
					Beep(3000, 50);
				#endif
				m_graph_form.erase(m_graph_form.begin() + i);
				arrangeGraphs();
				break;
			}
		}
	#endif
}

void __fastcall TForm1::saveSettings()
{
	if (Form1)
	{
		if (this->WindowState == wsNormal)
		{
			settings.mainWindowPos.top     = Form1->Top;
			settings.mainWindowPos.left    = Form1->Left;
			settings.mainWindowPos.width   = Form1->Width;
			settings.mainWindowPos.height  = Form1->Height;
		}
	}

	if (CommsForm)
	{
		settings.commsWindowPos.top     = CommsForm->Top;
		settings.commsWindowPos.left    = CommsForm->Left;
		settings.commsWindowPos.width   = CommsForm->Width;
		settings.commsWindowPos.height  = CommsForm->Height;
		settings.commsWindowPos.showing = CommsForm->Showing;
	}

	if (VNAUsartCommsForm)
	{
		settings.vnaUSARTcommsWindowPos.top     = VNAUsartCommsForm->Top;
		settings.vnaUSARTcommsWindowPos.left    = VNAUsartCommsForm->Left;
		settings.vnaUSARTcommsWindowPos.width   = VNAUsartCommsForm->Width;
		settings.vnaUSARTcommsWindowPos.height  = VNAUsartCommsForm->Height;
		settings.vnaUSARTcommsWindowPos.showing = VNAUsartCommsForm->Showing;
	}

	if (BatteryVoltageForm)
	{
		settings.batteryWindowPos.top     = BatteryVoltageForm->Top;
		settings.batteryWindowPos.left    = BatteryVoltageForm->Left;
		settings.batteryWindowPos.width   = BatteryVoltageForm->Width;
		settings.batteryWindowPos.height  = BatteryVoltageForm->Height;
		settings.batteryWindowPos.showing = BatteryVoltageForm->Showing;
	}

	if (SettingsForm)
	{
		settings.settingsWindowPos.top     = SettingsForm->Top;
		settings.settingsWindowPos.left    = SettingsForm->Left;
		settings.settingsWindowPos.width   = SettingsForm->Width;
		settings.settingsWindowPos.height  = SettingsForm->Height;
		settings.settingsWindowPos.showing = SettingsForm->Showing;
	}

	if (UploadFirmwareForm)
	{
		settings.firmwareWindowPos.top     = UploadFirmwareForm->Top;
		settings.firmwareWindowPos.left    = UploadFirmwareForm->Left;
		settings.firmwareWindowPos.width   = UploadFirmwareForm->Width;
		settings.firmwareWindowPos.height  = UploadFirmwareForm->Height;
		settings.firmwareWindowPos.showing = UploadFirmwareForm->Showing;
	}

	if (UploadFirmwareV2Form)
	{
		settings.firmwareV2WindowPos.top     = UploadFirmwareV2Form->Top;
		settings.firmwareV2WindowPos.left    = UploadFirmwareV2Form->Left;
		settings.firmwareV2WindowPos.width   = UploadFirmwareV2Form->Width;
		settings.firmwareV2WindowPos.height  = UploadFirmwareV2Form->Height;
		settings.firmwareV2WindowPos.showing = UploadFirmwareV2Form->Showing;
	}

	if (CalibrationForm)
	{
		settings.calibrationWindowPos.top     = CalibrationForm->Top;
		settings.calibrationWindowPos.left    = CalibrationForm->Left;
		settings.calibrationWindowPos.width   = CalibrationForm->Width;
		settings.calibrationWindowPos.height  = CalibrationForm->Height;
		settings.calibrationWindowPos.showing = CalibrationForm->Showing;
	}

	if (VNAScreenCaptureForm)
	{
		settings.screenCaptureWindowPos.top     = VNAScreenCaptureForm->Top;
		settings.screenCaptureWindowPos.left    = VNAScreenCaptureForm->Left;
		settings.screenCaptureWindowPos.width   = VNAScreenCaptureForm->Width;
		settings.screenCaptureWindowPos.height  = VNAScreenCaptureForm->Height;
		settings.screenCaptureWindowPos.showing = VNAScreenCaptureForm->Showing;
	}

	settings.startHz = data_unit.m_freq_start_Hz;
	settings.stopHz  = data_unit.m_freq_stop_Hz;
	settings.cwHz    = data_unit.m_freq_cw_Hz;

	{
		double d;
		if (common.strToValue(PointBandwidthHzComboBox->Text, d, 1.0))
			settings.pointBandwidth = IROUND(d);
	}

	settings.guiStyleName = TStyleManager::ActiveStyle->Name;

	settings.save();
}

void __fastcall TForm1::sortMarkers()
{
	for (int k = 0; k < (int)settings.m_markers_freq.size() - 1; k++)
	{
		t_marker_freq marker1 = settings.m_markers_freq[k];
		for (int m = k + 1; m < (int)settings.m_markers_freq.size(); m++)
		{
			const t_marker_freq marker2 = settings.m_markers_freq[m];
			if (marker1.Hz > marker2.Hz)
			{	// swap
				settings.m_markers_freq[k] = marker2;
				settings.m_markers_freq[m] = marker1;
				marker1 = marker2;
			}
		}
	}
}

void __fastcall TForm1::GraphPaintBoxPaint(TObject *Sender)
{
	#ifdef USE_OPENGL
		return;
	#else
		graphs.onPaint(	Sender,
								true,
								settings.graphType[0],
								settings.graphType[1],
								settings.graphType[2],
								settings.graphType[3],
								graphs.numberOfGraphs(),
								HistoryTrackBar->Position);

		for (unsigned int i = 0; i < m_graph_form.size(); i++)
		{
			TGraphForm *graph_form = m_graph_form[i];
			if (graph_form)
				if (graph_form->Showing)
					graph_form->GraphPaintBox->Invalidate();
		}
	#endif
}

bool __fastcall TForm1::recordDataStart()
{
	if (!RecordSpeedButton->Down)
		return false;

	String folder = settings.recordFolder.Trim();
	if (folder.IsEmpty())
		return false;

	folder = ExcludeTrailingPathDelimiter(folder) + "\\" + FormatDateTime("yyyy-mm-dd hh-nn-ss", Now()) + "\\";

	m_record.folder      = folder;
	m_record.filename    = "";
	m_record.file_number = 0;
	m_record.enabled     = true;

   return true;
}

void __fastcall TForm1::recordDataToFile(std::vector <t_data_point> &points)
{
	if (!m_record.enabled)
	{
		if (RecordSpeedButton->Down)
			RecordSpeedButton->Down = false;
		return;
	}

	// *********************
	// fetch the latest data

	const int size = points.size();
	if (size <= 0)
		return;

/*
	int hist_index = (data_unit.m_history_frames > 0) ? data_unit.m_history_index - 1 : 0;
	if (hist_index < 0)
		hist_index += (int)MAX_HISTORY;

	const int size = data_unit.freqArraySize(-1);
	if (size <= 0)
		return;

	m_record.points.resize(size);

	for (int i = 0; i < size; i++)
	{
		for (int channel = 0; channel < MAX_CHANNELS; channel++)
		{
			m_record.points[i].Hz     = data_unit.m_point[i].Hz;
			m_record.points[i].data[channel] = data_unit.m_point[i].hist[hist_index].data[channel];
		}
	}
*/
	// *********************

	if (!common.createPath(AnsiString(m_record.folder).c_str()))
	{
		m_record.enabled = false;
		if (RecordSpeedButton->Down)
			RecordSpeedButton->Down = false;
		return;
	}

	String filename = m_record.folder;
	if (!m_record.filename.IsEmpty())
		filename += m_record.filename + "_";
	filename += IntToStr(m_record.file_number) + ".s2p";

	if (!common.saveSParams(points, 4, filename))
	//if (!common.saveSParams(m_record.points, 4, filename))
	{

	}

	// *********************

	m_record.file_number++;
}

void __fastcall TForm1::addNewRxData(std::vector <t_data_point> &new_points)
{
	if (m_freq_data_list.size() == 0)
		return;

	const int segment  = data_unit.m_segment;
	const int segments = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? data_unit.m_segments : 1;

	// **************************
	// add the new frequency point data into the main buffer

	if (!new_points.empty())
	{
		const unsigned int size = new_points.size();

		if (data_unit.m_point.size() != size)
			data_unit.m_point.resize(size);

		const int h_index = data_unit.m_history_index;

		for (unsigned int i = 0; i < size; i++)
		{
			data_unit.m_point[i].Hz            = new_points[i].Hz;
			data_unit.m_point[i].hist[h_index] = new_points[i];
		}
	}
	else
	if (!data_unit.m_point_incoming.empty())
	{
		const unsigned int size = data_unit.m_point_incoming.size();

		if (data_unit.m_point.size() != size)
			data_unit.m_point.resize(size);

		const int h_index = data_unit.m_history_index;

		for (unsigned int i = 0; i < size; i++)
		{
			data_unit.m_point[i].Hz            = data_unit.m_point_incoming[i].Hz;
			data_unit.m_point[i].hist[h_index] = data_unit.m_point_incoming[i];
		}
	}
	else
	{
		unsigned int m_start_bin = 0;
//		m_start_bin = 0;
		const int64_t start_freq = m_freq_data_list[0].Hz;
		while (m_start_bin < data_unit.m_point.size())
		{
			if (start_freq <= data_unit.m_point[m_start_bin].Hz)
				break;
			m_start_bin++;
		}

		if (data_unit.m_point.size() < (m_start_bin + m_freq_data_list.size()))
			data_unit.m_point.resize(m_start_bin + m_freq_data_list.size());

		for (unsigned int i = 0; i < m_freq_data_list.size(); i++)
		{
			const unsigned int bin = m_start_bin + i;
			data_unit.m_point[bin].Hz                              = m_freq_data_list[i].Hz;
			data_unit.m_point[bin].hist[data_unit.m_history_index] = m_freq_data_list[i];
		}
	}

	// **************************

	if (segment >= (segments - 1))
	{	// all segments done

		// compute the points per second
		const double secs = m_comms.points_speed_timer.secs(true);
		if (secs > 0)
		{
			const int points = (!data_unit.m_point_incoming.empty()) ? data_unit.m_point_incoming.size() : m_freq_data_list.size() * segments;
			const float points_per_sec = (float)points / secs;

			const int size = ARRAY_SIZE(m_comms.points_per_sec_buf);

			if (data_unit.m_total_frames <= 0)
			{
				ZeroMemory(m_comms.points_per_sec_buf, sizeof(m_comms.points_per_sec_buf));
				m_comms.points_per_sec_buf_count = -1;
			}

			// fifo buffer
			memmove(&m_comms.points_per_sec_buf[0], &m_comms.points_per_sec_buf[1], sizeof(m_comms.points_per_sec_buf[0]) * (size - 1));	// slide
			if (m_comms.points_per_sec_buf_count >= 0)
				m_comms.points_per_sec_buf[size - 1] = points_per_sec;	// add new point
			if (++m_comms.points_per_sec_buf_count > size)
				m_comms.points_per_sec_buf_count = size;

			if (m_comms.points_per_sec_buf_count >= size)
			{	// buffer is now full - compute the median
				float buf[ARRAY_SIZE(m_comms.points_per_sec_buf)];
				memcpy(buf, m_comms.points_per_sec_buf, sizeof(buf));
				for (int i = 0; i < size - 1; i++)
				{
					float v1 = buf[i];
					for (int k = i + 1; k < size; k++)
					{
						const float v2 = buf[k];
						if (v2 < v1)
						{	// swap
							buf[i] = v2;
							buf[k] = v1;
							v1 = v2;
						}
					}
				}
				m_comms.points_per_sec = buf[size / 2];
			}
			else
			if (m_comms.points_per_sec_buf_count > 0)
			{	// buffer not yet full - compute the current average
				float sum = 0.0f;
				for (int i = 0; i < m_comms.points_per_sec_buf_count; i++)
					sum += m_comms.points_per_sec_buf[size - 1 - i];
				m_comms.points_per_sec = sum / m_comms.points_per_sec_buf_count;
			}
			else
				m_comms.points_per_sec = 0.0f;
			#if 0
				{
					String s;
					for (int i = 0; i < size; i++)
					{
						String s2;
						s2.printf(L" %12.3f", m_comms.points_per_sec_buf[i] * 1000);
						s += s2;
					}
					//s.printf(L"%d %0.3fms", data_unit.m_total_frames, m_comms.points_per_sec);
					common.logFileAppend(s + "\n");
				}
			#endif
		}

		data_unit.m_total_frames++;

		if (++data_unit.m_history_index >= (int)MAX_HISTORY)
			data_unit.m_history_index = 0;

		if (++data_unit.m_history_frames > (int)MAX_HISTORY)
			data_unit.m_history_frames = (int)MAX_HISTORY;

		::PostMessage(this->Handle, WM_NEW_DATA, segment, segments);
	}

	m_freq_data_list.resize(0);

	nanovna1_comms.m_rx_block.type = SERIAL_STATE_IDLE;
	nanovna1_comms.m_rx_block.lines.resize(0);
	nanovna1_comms.m_rx_block.bin_data.resize(0);
	nanovna1_comms.m_rx_block.bin_data_index = 0;
	nanovna1_comms.m_rx_block.timer.mark();

	if (++data_unit.m_segment >= segments)
		data_unit.m_segment = 0;

	// keep scanning until we complete a multi-segment scan
	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{	// JanVNAV2

	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		if (data_unit.m_segment >= (segments - 1))
		{	// finished the sweep
			if (nanovna2_comms.mode == MODE_SINGLE_SCAN)
				::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
			else
			if (!nanovna2_comms.m_pause_comms)
				requestScan(); 	// request next segment
			else
				::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
		}
		else
		{
			requestScan(); 	// request next segment
		}
	}
	else
	{	// V1
		if ((nanovna1_comms.mode == MODE_SINGLE_SCAN || nanovna1_comms.mode == MODE_SCAN) &&
			 (!nanovna1_comms.m_pause_comms || (data_unit.m_segment > 0 && data_unit.m_segment < segments)))
		{
			requestScan(); 	// request next segment
		}
		else	// finished scan
		{
//			::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
		}
	}
}

void __fastcall TForm1::fetchUserInput()
{
	double d;

	if (common.strToValue(StartMHzEdit->Text, d, 1e6))
		data_unit.m_freq_start_Hz  = I64ROUND(d);

	if (common.strToValue(StopMHzEdit->Text, d, 1e6))
		data_unit.m_freq_stop_Hz   = I64ROUND(d);

	if (common.strToValue(CenterMHzEdit->Text, d, 1e6))
		data_unit.m_freq_center_Hz = I64ROUND(d);

	if (common.strToValue(SpanMHzEdit->Text, d, 1e6))
		data_unit.m_freq_span_Hz   = I64ROUND(d);

	if (common.strToValue(CWMHzEdit->Text, d, 1e6))
		data_unit.m_freq_cw_Hz     = I64ROUND(d);

	showWarnings();
}

void __fastcall TForm1::buildMarkerListBox()
{
	const int mi = selectedMarker(); 												// currently selected marker
	const int64_t freq_Hz = (mi >= 0) ? settings.m_markers_freq[mi].Hz : -1;	// frequency of the selected marker

	const int top_item = MarkerListBox->TopIndex;	// remember the marker currently positioned at the top of the listbox

	MarkerListBox->Items->BeginUpdate();	// the user doesn't want to see us working on the list box

	// sort the list according the marker frequency
	sortMarkers();

	MarkerListBox->Clear();
	for (unsigned int i = 0; i < settings.m_markers_freq.size(); i++)
	{
		const t_marker_freq marker = settings.m_markers_freq[i];
		String s;
		s.printf(L"%3d  %s Hz", 1 + i, common.freqToStr2(marker.Hz, 10).c_str());
		if (marker.type == MARKER_TYPE_DELTA)
			s += "  Delta";
		else
			s += "       ";
		MarkerListBox->Items->AddObject(s, (TObject *)i);
	}

	// move the visible list back to it's original position
	if (top_item >= 0 && MarkerListBox->Items->Count > 0)
		MarkerListBox->TopIndex = top_item;

	if (freq_Hz > 0)
	{
		for (unsigned int i = 0; i < settings.m_markers_freq.size(); i++)
		{
			if (settings.m_markers_freq[i].Hz == freq_Hz)
			{	// found the previously selected marker .. re-select it
				const int k = (int)MarkerListBox->Items->IndexOfObject((TObject *)i);
				if (k >= 0)
					MarkerListBox->ItemIndex = k;
				break;
			}
		}
	}

	MarkerListBox->Items->EndUpdate();

	DeleteFrequencyMarkersBitBtn->Enabled = !settings.m_markers_freq.empty();
}

void __fastcall TForm1::updateDeviceComboBox()
{
	TComboBox *cb = DeviceComboBox;

	String device_name = cb->Text.Trim();

	std::vector <T_SerialPortInfo> serial_port_list;
	m_comms.serial.GetSerialPortList(serial_port_list);

	std::vector <t_vna_list> libusb_list;
	janvna2_comms.getDeviceList(libusb_list);

	{
		const TNotifyEvent ne = cb->OnChange;
		cb->OnChange = NULL;

		cb->Clear();
		cb->AddItem("None", (TObject *)0xffffffff);

		// serial ports .. 1 to 99
		for (unsigned int i = 0; i < serial_port_list.size(); i++)
			cb->AddItem(serial_port_list[i].name, (TObject *)(1 + i));

		// libusb devices .. 100 to 199
		for (unsigned int i = 0; i < libusb_list.size(); i++)
		{
			const t_vna_list *vna = &libusb_list[i];
			String s;
			s.printf(L" %04X %04X", vna->vid, vna->pid);
			s = "JAN_VNA " + libusb_list[i].serial_number + s;
			cb->AddItem(s, (TObject *)(100 + i));
		}

		// tcpip device .. >= 200
		if (!settings.tcpipAddress.IsEmpty() && settings.tcpipPort >= 1 && settings.tcpipPort <= 65535)
		{
			String s = "TCPIP " + settings.tcpipAddress + ":" + IntToStr(settings.tcpipPort);
			cb->AddItem(s, (TObject *)200);
		}

		const int i = cb->Items->IndexOf(device_name);
		if (!device_name.IsEmpty() && i > 0)
		{
			cb->ItemIndex = i;
		}
		else
		{
			cb->ItemIndex = 0;
			if (m_comms.serial.connected)
				::PostMessage(this->Handle, WM_DISCONNECT, 0, 0);
		}

		cb->OnChange = ne;
	}

	common.comboBoxAutoWidth(cb);
}

bool __fastcall TForm1::requestCapture()
{
	if (!connected())
		return false;

	return (data_unit.m_vna_data.type != UNIT_TYPE_NANOVNA_V2 && data_unit.m_vna_data.type != UNIT_TYPE_JANVNA_V2) ? nanovna1_comms.requestCapture() : nanovna2_comms.requestCapture();
}

void __fastcall TForm1::requestScan()
{
	if (!connected())
		return;

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{

	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		if (nanovna2_comms.mode == MODE_SINGLE_SCAN && data_unit.m_total_frames > 0)
		{
			::PostMessage(this->Handle, WM_CAPTURE_STOP, 0, 0);
		}
		else
		if (nanovna2_comms.mode == MODE_SINGLE_SCAN || nanovna2_comms.mode == MODE_SCAN || nanovna2_comms.mode == MODE_GENERATOR)
		{
			// empty the RX buffer
			m_comms.rx.buffer_wr = 0;

			nanovna2_comms.requestScan();
		}
		else
		{
			::PostMessage(this->Handle, WM_CAPTURE_STOP, 0, 0);
		}
	}
	else
	{	// V1
		if (nanovna1_comms.mode == MODE_SINGLE_SCAN && data_unit.m_total_frames > 0)
		{
			::PostMessage(this->Handle, WM_CAPTURE_STOP, 0, 0);
		}
		else
		{
			nanovna1_comms.requestScan();
		}
	}
}

void __fastcall TForm1::poll()
{
	if (!connected())
		return;

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{	// the janvna comms takes care of of it all
//		janvna2_comms.poll();
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		nanovna2_comms.poll();
	}
	else
	{	// V1
		nanovna1_comms.poll();
	}
}

int __fastcall TForm1::fetchCommsData(const bool clear_rx_buffer)
{
	if (clear_rx_buffer)
		m_comms.rx.buffer_wr = 0;

	if (!connected())
		return -1;

	// ***************
	// fetch any rx'ed data

	int bytes_read = 0;

	if (m_comms.serial.connected)
	{
		const int size = m_comms.serial.rxBytesAvailable;
		if (size > 0)
		{
			if ((int)(m_comms.rx.buffer_wr + size) > (int)m_comms.rx.buffer.size())
			{	// increase our buffer size to hold all the rx'ed data
				const int new_size = ((m_comms.rx.buffer_wr + size + 8192) / 8192) * 8192;
				m_comms.rx.buffer.resize(new_size);
			}

			bytes_read = m_comms.serial.RxBytes(&m_comms.rx.buffer[m_comms.rx.buffer_wr], size);
			if (bytes_read < 0)
			{	// serial port error
				pushCommMessage("rx: serial port GetBytes() error .. disconnecting");
				m_comms.serial.Disconnect();
				tcpipClientDisconnect();
				::PostMessage(this->Handle, WM_DISCONNECT, 0, 0);
				return -1;
			}
		}
	}
	else
	#ifdef TCPIPH
		if (m_comms.tcpip.connected)
		{
			const int size = m_comms.tcpip.rxBytesAvailable;
			if (size > 0)
			{
				if ((m_comms.rx.buffer_wr + size) > (int)m_comms.rx.buffer.size())
				{	// increase our buffer size to hold all the rx'ed data
					const int new_size = ((m_comms.rx.buffer_wr + size + 8192) / 8192) * 8192;
					m_comms.rx.buffer.resize(new_size);
				}

				bytes_read = m_comms.tcpip.RxBytes(&m_comms.rx.buffer[m_comms.rx.buffer_wr], size);
				if (bytes_read < 0)
				{	// tcpip port error
					pushCommMessage("rx: tcpip port GetBytes() error .. disconnecting");
					m_comms.serial.Disconnect();
					m_comms.tcpip.Disconnect();
					::PostMessage(this->Handle, WM_DISCONNECT, 0, 0);
					return -1;
				}
			}
		}
	#else
		if (m_comms.tcpip && m_comms.tcpip_stream_rx)
		{
			try
			{
				if (m_comms.tcpip->Connected())
				{
					if (!m_comms.tcpip->IOHandler->InputBufferIsEmpty())
						m_comms.tcpip->IOHandler->InputBufferToStream(m_comms.tcpip_stream_rx);

					const int size = m_comms.tcpip_stream_rx->Position;
					if (size > 0)
					{
						if ((int)(m_comms.rx.buffer_wr + size) > (int)m_comms.rx.buffer.size())
						{	// increase our buffer size to hold all the rx'ed data
							const int new_size = ((m_comms.rx.buffer_wr + size + 8192) / 8192) * 8192;
							m_comms.rx.buffer.resize(new_size);
						}

						m_comms.tcpip_stream_rx->Position = 0;
						m_comms.tcpip_stream_rx->ReadBuffer(&m_comms.rx.buffer[m_comms.rx.buffer_wr], size);
						m_comms.tcpip_stream_rx->Position = 0;

						bytes_read = size;
					}
				}
			}
			catch (Exception &exception)
			{
				//Application->ShowException(&exception);
				pushCommMessage("rx: tcpip error " + exception.ToString());
				m_comms.serial.Disconnect();
				tcpipClientDisconnect();
				::PostMessage(this->Handle, WM_DISCONNECT, 0, 0);
				return -1;
			}
		}
	#endif

	if (bytes_read > 0)
	{	// we've received some bytes from the unit
		m_comms.rx.buffer_wr += bytes_read;

		// reset the comms time-out timers
		m_comms.rx_timer.mark();

		if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
		{
			if (janvna2_comms.mode > MODE_IDLE)
				m_comms.link_timer.mark();
		}
		else
		if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
		{	// V2
			if (nanovna2_comms.mode > MODE_IDLE)
				m_comms.link_timer.mark();
		}
		else
		{	// V1
			if (nanovna1_comms.mode >= MODE_IDLE)
				m_comms.link_timer.mark();
		}
	}

	return bytes_read;
}

int __fastcall TForm1::processRx(t_serial_buffer &serial_buffer, bool force_print)
{
	const int bytes_read = fetchCommsData(false);
	if (bytes_read >= 0)
	{
		if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
		{
		}
		else
		if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
		{	// V2
			return nanovna2_comms.processRx(serial_buffer);
		}
		else
		{	// V1
			return nanovna1_comms.processRx(serial_buffer);
		}
	}
   return bytes_read;
}

void __fastcall TForm1::pauseComms(bool pause)
{
	if (!connected())
		return;

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{  // V2
		nanovna2_comms.pauseComms(pause);
	}
	else
	{	// V1
		nanovna1_comms.pauseComms(pause);
	}
}

void __fastcall TForm1::processSerial()
{
	if (!connected())
		return;

	while (m_comms.serial.errorCount() > 0)
	{
		t_serial_port_error error = m_comms.serial.pullError();

		pushCommMessage("serial error " + error.str);

		// serial error tx GetOverlappedResult [21] The device is not ready.
		// serial error tx GetOverlappedResult [22] The device does not recognise the command.
//		if (error.code == ERROR_NOT_READY || error.code == ERROR_BAD_COMMAND)
		{
			m_comms.serial.Disconnect();
			tcpipClientDisconnect();
			::PostMessage(this->Handle, WM_DISCONNECT, 0, 0);
			return;
		}
	}

	#ifdef TCPIPH
		while (m_comms.tcpip.errorCount() > 0)
		{
			t_tcpip_error error = m_comms.tcpip.pullError();
			pushCommMessage("tcpip error " + error.str);
/*
			{
				m_comms.serial.Disconnect();
				tcpipClientDisconnect();
				::PostMessage(this->Handle, WM_DISCONNECT, 0, 0);
				return;
			}
*/
		}
	#else
//		if (m_comms.tcpip && m_comms.tcpip_stream_rx)
//		{
//			if (m_comms.tcpip->Connected())
//			{
//
//			}
//		}
	#endif

	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2 && nanovna2_comms.mode == MODE_DFU_MODE)
	{	// the firmware upload window is using the comms
		nanovna2_comms.m_poll_timer.mark();
		return;
	}

	// process the rx'ed data we have in our rx buffer
	processRx(m_comms.rx, false);

	if (!connected())
		return;

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2

		// fast timeouts if we've just connected and are just finding out if it's a V1 that we are connected too
		double poll_ms          = (nanovna2_comms.mode < MODE_IDLE) ? 500 : nanovna2_comms.m_poll_ms;
//		double state_timeout_ms = (nanovna2_comms.mode < MODE_IDLE) ? 700 : nanovna2_comms.m_state_timeout_ms;
		const int max_retries   = 2;

		if (nanovna2_comms.mode == MODE_CAPTURE)
			poll_ms *= 3;

		if (!nanovna2_comms.m_pause_comms)
		{
/*
			// check for rx timeout
			if (m_comms.rx_timer.millisecs(false) >= state_timeout_ms)
			{	// rx timed out

				// clear the RX buffer
				m_comms.rx.buffer_wr = 0;

				if (!nanovna2_comms.m_tx_cmd.empty())
				{	// re-send the command
//					nanovna2_comms.sendData();
				}
				else
				{
					if (nanovna1_comms.mode == MODE_SCAN)
					{	// re-request another block
						m_comms.rx_timer.mark();
//						Beep(880, 40);
						data_unit.m_segment = 0;
						m_start_bin = 0;
						m_freq_data_list.resize(0);
						requestScan();
					}
				}
			}
*/
			if (nanovna2_comms.m_poll_timer.millisecs(false) >= poll_ms)
			{
				if (nanovna2_comms.m_retries >= max_retries)
				{	// too many retries

					if (nanovna2_comms.mode <= MODE_INIT3)
					{  // see if we have a V1 connected

						pushCommMessage("rx link timeout .. scanning for a NanoVNA V1 ..");

						data_unit.resetUnitData();

						nanovna1_comms.reset();
						m_comms.rx_timer.mark();
						m_comms.link_timer.mark();

						poll();
					}
					else
					{	// give up and disconnect
						pushCommMessage("rx link timeout .. disconnecting");
						m_comms.serial.Disconnect();
						tcpipClientDisconnect();
						::PostMessage(this->Handle, WM_DISCONNECT, 0, 0);
					}

					return;
				}

				if (nanovna2_comms.mode == MODE_CAPTURE)
				{	// switch back to IDLE mode
					nanovna2_comms.mode = MODE_IDLE;
					m_comms.rx.buffer_wr = 0;	// empty the RX buffer
				}

				if (nanovna2_comms.mode == MODE_IDLE)
				{
					nanovna2_comms.mode = MODE_POLL;
					m_comms.rx.buffer_wr = 0;	// empty the RX buffer
				}

				if (nanovna2_comms.mode != MODE_SINGLE_SCAN && nanovna2_comms.mode != MODE_SCAN && nanovna2_comms.mode != MODE_GENERATOR)
					poll();
			}
		}
		else
		{
			m_comms.rx_timer.mark();
			m_comms.link_timer.mark();
		}

		// send any queued tx commands to the unit
//		if (connected())
//			nanovna2_comms.processTxCommands();

	}
	else
	{	// V1

		// fast timeouts if we've just connected and are just finding out if it's a V1 that we are connected too
		const double poll_ms          = nanovna1_comms.m_poll_ms;
		const double state_timeout_ms = (nanovna1_comms.mode <= MODE_INIT1) ? 500 : nanovna1_comms.m_state_timeout_ms;
		const double link_timeout_ms  = (nanovna1_comms.mode <= MODE_INIT1) ? 1100 : nanovna1_comms.m_link_timeout_ms;

		if (!nanovna1_comms.m_pause_comms)
		{
			// send a poll if it's time to do so - this is to check if the unit is still there and responding OK
			if (nanovna1_comms.m_rx_block.type == SERIAL_STATE_IDLE)
				if (nanovna1_comms.mode == MODE_IDLE)
					if (nanovna1_comms.m_poll_timer.millisecs(false) >= poll_ms)
						poll();

			// check for rx timeout
			if (m_comms.rx_timer.millisecs(false) >= state_timeout_ms)
			{	// rx timed out

				if (nanovna1_comms.m_rx_block.type != SERIAL_STATE_IDLE)
				{
					String s;
					s += "timeout " + nanovna1_comms.getSerialStateString(nanovna1_comms.m_rx_block.type);
					s += " lines: " + UIntToStr(nanovna1_comms.m_rx_block.lines.size());
					s += " data: " + UIntToStr(nanovna1_comms.m_rx_block.bin_data_index) + "/" + UIntToStr(nanovna1_comms.m_rx_block.bin_data.size());
					pushCommMessage("rx: " + s);
					#ifdef SHOW_CMD_START_END
						pushCommMessage("rx:  " + getSerialStateString(nanovna1_comms.m_rx_block.type).UpperCase() + " stopped");
					#endif
				}

				// reset the rx state
				m_comms.rx.buffer_wr = 0;
				nanovna1_comms.resetRx();

				if (!nanovna1_comms.m_tx_command.IsEmpty())
				{	// re-send the command
					nanovna1_comms.sendTxCommand();
				}
				else
				{
					// clear the tx command
					//m_comms.tx_command = "";

					if (nanovna1_comms.mode == MODE_SCAN)
					{	// re-request another block
						m_comms.rx_timer.mark();
						data_unit.m_segment = 0;
//						m_start_bin = 0;
						m_freq_data_list.resize(0);
						requestScan();
					}
				}
			}

			// check for link connection timeout
			if (m_comms.link_timer.millisecs(false) >= link_timeout_ms)
			{	// connection timeout
				pushCommMessage("rx link timeout .. disconnecting");
				m_comms.serial.Disconnect();
				tcpipClientDisconnect();
				::PostMessage(this->Handle, WM_DISCONNECT, 0, 0);
				return;
			}
		}
		else
		{
			m_comms.rx_timer.mark();
			m_comms.link_timer.mark();
		}

		// send any queued tx commands to the unit
		if (connected())
			nanovna1_comms.processTxCommands();
	}
}

void __fastcall TForm1::threadProcess()
{
	if (m_thread == NULL)
		return;

	processSerial();
}

bool __fastcall TForm1::scanning()
{
	if (!connected())
		return false;

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
		return (janvna2_comms.mode == MODE_SINGLE_SCAN || janvna2_comms.mode == MODE_SCAN) ? true : false;

	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
		return (nanovna2_comms.mode == MODE_SINGLE_SCAN || nanovna2_comms.mode == MODE_SCAN) ? true : false;

	return (nanovna1_comms.mode == MODE_SINGLE_SCAN || nanovna1_comms.mode == MODE_SCAN) ? true : false;
}

bool __fastcall TForm1::generating()
{
	if (!connected())
		return false;

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
		return (janvna2_comms.mode == MODE_GENERATOR) ? true : false;

	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
		return (nanovna2_comms.mode == MODE_GENERATOR) ? true : false;

	return (nanovna1_comms.mode == MODE_GENERATOR) ? true : false;
}

t_mode __fastcall TForm1::commsMode()
{
	if (!connected())
		return MODE_NONE;

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
		return janvna2_comms.mode;

	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
		return nanovna2_comms.mode;

	return nanovna1_comms.mode;
}

void __fastcall TForm1::stop()
{
	const bool dfu_mode = (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2) ? nanovna2_comms.inDFUMode() : false;

	CaptureVNAScreenBitBtn->Enabled = (data_unit.m_vna_data.type != UNIT_TYPE_JANVNA_V2) ? !dfu_mode : false;
	ScanSpeedButton->Enabled        = true;
	ScanOnceSpeedButton->Enabled    = true;

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
		janvna2_comms.mode = MODE_IDLE;
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		if (nanovna2_comms.mode < MODE_IDLE)
			return;

		nanovna2_comms.resetScan();

		nanovna2_comms.mode = MODE_IDLE;
	}
	else
	{	// V1
		if (nanovna1_comms.mode < MODE_IDLE)
			return;

		nanovna1_comms.mode = MODE_IDLE;
	}

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		if (nanovna2_comms.mode > MODE_IDLE)
			nanovna2_comms.mode = MODE_IDLE;
	}
	else
	{	// V1
		if (data_unit.m_vna_data.cmd_resume)
			nanovna1_comms.addSerialTxCommand("resume");
	}

	if (ScanOnceSpeedButton->Down)
	{
		const TNotifyEvent ne = ScanOnceSpeedButton->OnClick;
		ScanOnceSpeedButton->OnClick = NULL;
		ScanOnceSpeedButton->Down    = false;
		ScanOnceSpeedButton->OnClick = ne;
	}

	if (ScanSpeedButton->Down)
	{
		const TNotifyEvent ne = ScanSpeedButton->OnClick;
		ScanSpeedButton->OnClick = NULL;
		ScanSpeedButton->Down    = false;
		ScanSpeedButton->OnClick = ne;
	}

	if (CalibrationForm)
		CalibrationForm->stop();

	if (connected())
	{
		nanovna1_comms.m_poll_timer.mark();
		nanovna2_comms.m_poll_timer.mark();

		m_comms.rx_timer.mark();
//		m_comms.link_timer.mark();

//		Beep(440, 100);

		if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
		{
		}
		else
		if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
		{  // V2
		}
		else
		{	// V1
			if (data_unit.m_vna_data.cmd_resume)
				addSerialTxCommand("resume");
		}

		poll();
	}
}

void __fastcall TForm1::scan()
{
	if (!connected())
		return;

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
		janvna2_comms.mode = ScanOnceSpeedButton->Down ? MODE_SINGLE_SCAN : MODE_SCAN;
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		if (nanovna2_comms.mode != MODE_IDLE)
			return;

		if (nanovna2_comms.inDFUMode())
		{	// the unit if in DFU mode
			if (ScanOnceSpeedButton->Down)
			{
				const TNotifyEvent ne = ScanOnceSpeedButton->OnClick;
				ScanOnceSpeedButton->OnClick = NULL;
				ScanOnceSpeedButton->Down    = false;
				ScanOnceSpeedButton->OnClick = ne;
			}
			if (ScanSpeedButton->Down)
			{
				const TNotifyEvent ne = ScanSpeedButton->OnClick;
				ScanSpeedButton->OnClick = NULL;
				ScanSpeedButton->Down    = false;
				ScanSpeedButton->OnClick = ne;
			}
			return;
		}
	}
	else
	{	// V1
		if (nanovna1_comms.mode != MODE_IDLE)
			return;
	}

	CWModeSpeedButton->Down = false;
	cwPopupWindow(CWMHzEdit, false);	// hide the popup window

	CaptureVNAScreenBitBtn->Enabled = false;
	if (VNAScreenCaptureForm)
		VNAScreenCaptureForm->Hide();

	StopScanBitBtn->Enabled = true;

	if (!connected())
	{
		if (ScanOnceSpeedButton->Down)
		{
			const TNotifyEvent ne = ScanOnceSpeedButton->OnClick;
			ScanOnceSpeedButton->OnClick = NULL;
			ScanOnceSpeedButton->Down    = false;
			ScanOnceSpeedButton->OnClick = ne;
		}
		if (ScanSpeedButton->Down)
		{
			const TNotifyEvent ne = ScanSpeedButton->OnClick;
			ScanSpeedButton->OnClick = NULL;
			ScanSpeedButton->Down    = false;
			ScanSpeedButton->OnClick = ne;
		}
		return;
	}

	graphs.resetAutoMinMaxCounters();

	StatusBar2->Panels->Items[1]->Text = "Points/sec";

	{
		const TNotifyEvent ne = HistoryTrackBar->OnChange;
		HistoryTrackBar->OnChange = NULL;
		HistoryTrackBar->Position = 0;
		HistoryTrackBar->OnChange = ne;
	}

	nanovna1_comms.resetRx();

	nanovna1_comms.m_poll_timer.mark();
	nanovna2_comms.m_poll_timer.mark();

	// these are for measuring the scan speed (points per second)
	m_comms.points_speed_timer.mark();
	ZeroMemory(m_comms.points_per_sec_buf, sizeof(m_comms.points_per_sec_buf));
	m_comms.points_per_sec_buf_count = -1;
	m_comms.points_per_sec = 0.0f;

	m_comms.rx_timer.mark();
//	m_comms.buffer.tx.timer.mark();

//	m_comms.rx.buffer_wr = 0;
//	m_comms.buffer.tx.buffer_wr = 0;

	if (StartMHzEdit->Visible)
		if (!processStartMHzEdit(true, true))
			return;
	if (StopMHzEdit->Visible)
		if (!processStopMHzEdit(true, true))
			return;
	if (CenterMHzEdit->Visible)
		if (!processCenterMHzEdit(true, true))
			return;
	if (SpanMHzEdit->Visible)
		if (!processSpanMHzEdit(true, true))
			return;
	if (CWMHzEdit->Visible)
		if (!processCWMHzEdit(true, true))
			return;
	if (PointBandwidthHzComboBox->Visible)
		if (!processPointBandwidthHzComboBox())
			return;
	if (NumberOfPointsComboBox->Visible)
		if (!processNumberOfPointsComboBox())
			return;

	//	NumberOfPointsComboBox->Enabled = false;

	resetFreqArray();

	calibration_module.m_inter_cal.resize(0);

	if (CalibrationForm)
		CalibrationForm->stop();

	if (RecordSpeedButton->Down)
		if (!recordDataStart())
			RecordSpeedButton->Down = false;

	// always enable live memory display when starting a scan
	settings.memoryEnable[0] = true;
	if (!MemorySpeedButton0->Down)
	{
		const TNotifyEvent ne = MemorySpeedButton0->OnClick;
		MemorySpeedButton0->OnClick = NULL;
		MemorySpeedButton0->Down    = true;
		MemorySpeedButton0->OnClick = ne;
	}

	if (ScanOnceSpeedButton->Down)
	{	// we're going to do a single sweep
		const TNotifyEvent ne = ScanSpeedButton->OnClick;
		ScanSpeedButton->OnClick = NULL;
		ScanSpeedButton->Down    = false;
		ScanSpeedButton->Enabled = false;
		ScanSpeedButton->OnClick = ne;
	}
	else
	if (ScanSpeedButton->Down)
	{	// we're going to do a continuous sweep
		const TNotifyEvent ne = ScanOnceSpeedButton->OnClick;
		ScanOnceSpeedButton->OnClick = NULL;
		ScanOnceSpeedButton->Down    = false;
		ScanOnceSpeedButton->Enabled = false;
		ScanOnceSpeedButton->OnClick = ne;
	}

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		nanovna2_comms.mode = ScanOnceSpeedButton->Down ? MODE_SINGLE_SCAN : MODE_SCAN;

		if (!nanovna2_comms.m_pause_comms)
			requestScan();
	}
	else
	{	// V1

		poll();

		// pause the NanoVNA's own scan
		if (data_unit.m_vna_data.cmd_pause)
			addSerialTxCommand("pause");

		if (data_unit.m_vna_data.cmd_integrator)
		{
			addSerialTxCommand("integrator off");
			addSerialTxCommand("integrator");
		}

		nanovna1_comms.mode = ScanOnceSpeedButton->Down ? MODE_SINGLE_SCAN : MODE_SCAN;

		sendBandwidthCommand();
		sendOutputPowerCommand();

		if (!nanovna1_comms.m_pause_comms)
			requestScan();
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

bool __fastcall TForm1::connected()
{
	bool conn = false;
	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
		conn = janvna2_comms.connected;
	}
	else
	{
		conn = m_comms.serial.connected;
		if (!conn)
		{
			#ifdef TCPIPH
				conn = m_comms.tcpip.connected;
			#else
				try
				{
					if (m_comms.tcpip)
						conn = m_comms.tcpip->Connected();
				}
				catch (Exception &exception)
				{
					//Application->ShowException(&exception);
					//pushCommMessage("connected error: " + exception.ToString());
					conn = false;
				}
			#endif
		}
	}
	return conn;
}

bool __fastcall TForm1::startThread()
{
	if (m_thread == NULL)
//		m_thread = new CMainFormThread(&threadProcess, tpNormal, true);	// create & start the serial processing thread
		m_thread = new CMainFormThread(&threadProcess, tpLowest, true);	// create & start the serial processing thread
	return (m_thread != NULL) ? true : false;
}

void __fastcall TForm1::stopThread()
{
	CMainFormThread *thread = m_thread;
	m_thread = NULL;
	if (thread != NULL)
	{
		thread->m_process = NULL;
		if (!thread->FreeOnTerminate)
		{
			thread->Terminate();
			thread->WaitFor();
			delete thread;
		}
		else
		{	// it will free itself
			thread->Terminate();
		}
	}
}

void __fastcall TForm1::disconnect()
{
	if (connected())
		stop();

	if (CalibrationForm)
		CalibrationForm->stop();

	janvna2_comms.closeDevice();

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
	}
	else
	{	// V1
		nanovna1_comms.m_tx_command = "";
		nanovna1_comms.m_tx_commands.resize(0);
	}

	stopThread();

	if (m_comms.serial.connected)
	{
		m_comms.serial.Disconnect();
		pushCommMessage("disconnect: serial port disconnected");
	}

	#ifdef TCPIPH
		if (m_comms.tcpip.connected)
		{
			tcpipClientDisconnect();
			pushCommMessage("disconnect: tcpip disconnected");
		}
	#else
		try
		{
			if (m_comms.tcpip)
			{
				if (m_comms.tcpip->Connected())
				{
					m_comms.tcpip->Disconnect();
					pushCommMessage("disconnect: tcpip disconnected");
				}
			}
		}
		catch (Exception &exception)
		{
			//Application->ShowException(&exception);
			//String s = exception.ToString();
		}
	#endif

	if (connected() && data_unit.m_vna_data.cmd_resume)
	{
		if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
		{
		}
		else
		if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
		{	// V2
		}
		else
		{	// V1
			nanovna1_comms.m_tx_commands.resize(0);
			nanovna1_comms.m_tx_command = "resume\r\n";
			nanovna1_comms.sendTxCommand();
		}
		Sleep(100);
	}

	{
		const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
		ConnectDisconnectSpeedButton->OnClick = NULL;
		ConnectDisconnectSpeedButton->Caption = "Disconnected";
		ConnectDisconnectSpeedButton->Down = false;
		ConnectDisconnectSpeedButton->OnClick = ne;
	}

	StatusBar2->Panels->Items[5]->Text = "Device";

	DeviceComboBox->Enabled         = true;
	SerialPortBaudrateComboBox->Visible = true;

	TCPIPAddressEdit->Enabled = true;
	TCPIPPortEdit->Enabled    = true;

	CalibrationBitBtn->Enabled = true;

	StopScanBitBtn->Enabled = false;

	ScanOnceSpeedButton->Enabled = false;
	ScanOnceSpeedButton->Down    = false;

	ScanSpeedButton->Enabled = false;
	ScanSpeedButton->Down    = false;

	SetScanRangeToVNAScanRangeBitBtn->Enabled = false;

	PointBandwidthHzComboBox->Enabled = true;

	CaptureVNAScreenBitBtn->Enabled = false;
	if (VNAScreenCaptureForm)
		VNAScreenCaptureForm->Hide();

	BatteryVoltageBitBtn->Enabled = true;
	VNAUsartCommsBitBtn->Enabled = true;

	m_comms.rx.buffer_wr = 0;

	nanovna1_comms.reset();
	nanovna1_comms.mode = MODE_NONE;

	nanovna2_comms.reset(false);
	nanovna2_comms.mode = MODE_NONE;

//	janvnav2_comms.reset();
//	janvnav2_comms.mode = MODE_NONE;
}

void __fastcall TForm1::tcpipClientDisconnect()
{
	#ifdef TCPIPH
		if (m_comms.tcpip->connected())
		{
			m_comms.tcpip.Disconnect();
			pushCommMessage("disconnect: tcpip port disconnected");
		}
	#else
		if (m_comms.tcpip)
		{
			try
			{
				if (m_comms.tcpip->Connected())
				{
					m_comms.tcpip->Disconnect();
					pushCommMessage("disconnect: tcpip port disconnected");
				}
			}
			catch (Exception &exception)
			{
				//Application->ShowException(&exception);
				//pushCommMessage("disconnect: tcpip error " + exception.ToString());
			}

			TIdTCPClient *client = m_comms.tcpip;
			m_comms.tcpip = NULL;
			delete client;
		}
	#endif
}

void __fastcall TForm1::connect()
{
	AnsiString s;

	nanovna1_comms.mode = MODE_IDLE;
	nanovna2_comms.mode = MODE_IDLE;
	janvna2_comms.mode  = MODE_IDLE;

//	if (UploadFirmwareForm)
//		UploadFirmwareForm->Hide();
//	if (UploadFirmwareV2Form)
//		UploadFirmwareV2Form->Hide();

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{	// JanVNAV2
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
	}
	else
	{	// V1
		nanovna1_comms.m_tx_command = "";
		nanovna1_comms.m_tx_commands.resize(0);
	}

	m_comms.connect_timer_tick = 0;

	ScanSpeedButton->Enabled        = true;
	ScanOnceSpeedButton->Enabled    = true;

	CWModeSpeedButton->Down = false;
	cwPopupWindow(CWMHzEdit, false);	// hide the popup window

	if (janvna2_comms.connected)
	{
		ConnectDisconnectSpeedButton->Caption  = "Connected";
		ScanOnceSpeedButton->Enabled = true;
		ScanSpeedButton->Enabled     = true;
		return;
	}

	if (m_comms.serial.connected)
	{
		ConnectDisconnectSpeedButton->Caption  = "Connected";
		ScanOnceSpeedButton->Enabled = true;
		ScanSpeedButton->Enabled     = true;
		return;
	}

	#ifdef TCPIPH
		if (m_comms.tcpip.connected)
		{
			ConnectDisconnectSpeedButton->Caption  = "Connected";
			ScanOnceSpeedButton->Enabled = true;
			ScanSpeedButton->Enabled     = true;
			return;
		}
	#else
		try
		{
			if (m_comms.tcpip)
			{
				if (m_comms.tcpip->Connected())
				{
					ConnectDisconnectSpeedButton->Caption  = "Connected";
					ScanOnceSpeedButton->Enabled = true;
					ScanSpeedButton->Enabled     = true;
					return;
				}
			}
		}
		catch (Exception &exception)
		{
			//Application->ShowException(&exception);
			pushCommMessage("connect: tcpip error " + exception.ToString());
			{
				const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
				ConnectDisconnectSpeedButton->OnClick = NULL;
				ConnectDisconnectSpeedButton->Caption  = "Disconnected";
				ConnectDisconnectSpeedButton->Down = false;
				ConnectDisconnectSpeedButton->OnClick = ne;
			}
			ScanOnceSpeedButton->Enabled = false;
			ScanSpeedButton->Enabled     = false;
			return;
		}
	#endif

	ConnectDisconnectSpeedButton->Caption  = "Disconnected";

	if (StartMHzEdit->Visible)
		if (!processStartMHzEdit(true, true))
			return;
	if (StopMHzEdit->Visible)
		if (!processStopMHzEdit(true, true))
			return;
	if (CenterMHzEdit->Visible)
		if (!processCenterMHzEdit(true, true))
			return;
	if (SpanMHzEdit->Visible)
		if (!processSpanMHzEdit(true, true))
			return;
	if (CWMHzEdit->Visible)
		if (!processCWMHzEdit(true, true))
			return;
	if (PointBandwidthHzComboBox->Visible)
		if (!processPointBandwidthHzComboBox())
			return;
//	if (NumberOfPointsComboBox->Visible)
//		if (!processNumberOfPointsComboBox())
//			return;

	//clearCommMessages();

	janvna2_comms.reset(false);

	nanovna2_comms.reset(false);

	nanovna1_comms.m_get_screen_capture = false;

	CaptureVNAScreenBitBtn->Enabled = false;
	if (VNAScreenCaptureForm)
		VNAScreenCaptureForm->Hide();

	BatteryVoltageBitBtn->Enabled = false;
	VNAUsartCommsBitBtn->Enabled = false;

	ScanOnceSpeedButton->Enabled = false;
	{
		const TNotifyEvent ne = ScanOnceSpeedButton->OnClick;
		ScanOnceSpeedButton->OnClick = NULL;
		ScanOnceSpeedButton->Down    = false;
		ScanOnceSpeedButton->OnClick = ne;
	}

	ScanSpeedButton->Enabled = false;
	{
		const TNotifyEvent ne = ScanSpeedButton->OnClick;
		ScanSpeedButton->OnClick = NULL;
		ScanSpeedButton->Down    = false;
		ScanSpeedButton->OnClick = ne;
	}

	data_unit.resetUnitData();

	nanovna1_comms.m_rx_block.type = SERIAL_STATE_IDLE;
	nanovna1_comms.m_rx_block.lines.resize(0);
	nanovna1_comms.m_rx_block.bin_data.resize(0);
	nanovna1_comms.m_rx_block.bin_data_index = 0;
	nanovna1_comms.m_rx_string        = "";
	nanovna1_comms.m_pause_comms      = false;
	nanovna1_comms.m_poll_ms          = DEFAULT_POLL_V1_MS;
	nanovna1_comms.m_state_timeout_ms = DEFAULT_STATE_TIME_OUT_V1_MS;
	nanovna1_comms.m_link_timeout_ms  = DEFAULT_LINK_TIME_OUT_V1_MS;

	resetFreqArray();

	if (CalibrationForm)
		CalibrationForm->stop();

	updateDateTime();

	StatusBar2->Panels->Items[1]->Text = "Points/sec";
	StatusBar2->Panels->Items[2]->Text = "Battery";
	StatusBar2->Panels->Items[3]->Text = "Threshold (Hz)";
	StatusBar2->Panels->Items[4]->Text = "Point RBW (Hz)";
	StatusBar2->Panels->Items[5]->Text = "Device";

	String device_name = DeviceComboBox->Text.Trim();

	int port_index = DeviceComboBox->ItemIndex;
	if (port_index >= 0)
			port_index = (int)DeviceComboBox->Items->Objects[port_index];

	if (port_index <= 0)
	{
		disconnect();
		return;
	}

	if (port_index >= 200)
	{	// tcpip connection
		pushCommMessage("connect: tcpip port connecting ..");

		#ifdef TCPIPH
			if (!m_comms.tcpip.Connect(settings.tcpipAddress, settings.tcpipPort))
			{
				while (m_comms.tcpip.errorCount() > 0)
				{
					String s = m_comms.tcpip.pullError();
					pushCommMessage("tcpip error " + s);
				}

				pushCommMessage("connect: tcpip port not connected");

				const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
				ConnectDisconnectSpeedButton->OnClick = NULL;
				ConnectDisconnectSpeedButton->Down = false;
				ConnectDisconnectSpeedButton->OnClick = ne;

				return;
			}
		#else
			if (!m_comms.tcpip_stream_rx)
			{
				pushCommMessage("connect: no tcpip rx stream memory");

				const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
				ConnectDisconnectSpeedButton->OnClick = NULL;
				ConnectDisconnectSpeedButton->Down = false;
				ConnectDisconnectSpeedButton->OnClick = ne;

				return;
			}

			m_comms.tcpip_stream_rx->Clear();

			if (!m_comms.tcpip)
			{
				try
				{
					m_comms.tcpip = new TIdTCPClient();
					if (!m_comms.tcpip)
					{
						pushCommMessage("connect: could not create tcpip client");

						const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
						ConnectDisconnectSpeedButton->OnClick = NULL;
						ConnectDisconnectSpeedButton->Down = false;
						ConnectDisconnectSpeedButton->OnClick = ne;
						return;
					}
				}
				catch (Exception &exception)
				{
					//Application->ShowException(&exception);
					pushCommMessage("connect: tcpip create client error " + exception.ToString());

					const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
					ConnectDisconnectSpeedButton->OnClick = NULL;
					ConnectDisconnectSpeedButton->Down = false;
					ConnectDisconnectSpeedButton->OnClick = ne;
					return;
				}
			}

			try
			{
				m_comms.tcpip->ConnectTimeout = 1500;	// ms
				//m_comms.tcpip->ReadTimeout = 1;
				m_comms.tcpip->ReadTimeout = 0;			// don't wait
				m_comms.tcpip->Host = settings.tcpipAddress;
				m_comms.tcpip->Port = settings.tcpipPort;
				m_comms.tcpip->Connect();

				if (!m_comms.tcpip->Connected())
				{
					pushCommMessage("connect: tcpip port not connected");

					const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
					ConnectDisconnectSpeedButton->OnClick = NULL;
					ConnectDisconnectSpeedButton->Down = false;
					ConnectDisconnectSpeedButton->OnClick = ne;

					tcpipClientDisconnect();
					return;
				}
			}
			catch (Exception &exception)
			{
				//Application->ShowException(&exception);
				pushCommMessage("connect: tcpip error " + exception.ToString());

				const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
				ConnectDisconnectSpeedButton->OnClick = NULL;
				ConnectDisconnectSpeedButton->Down = false;
				ConnectDisconnectSpeedButton->OnClick = ne;

				tcpipClientDisconnect();
				return;
			}
		#endif

		pushCommMessage("connect: tcpip port connected");

		TCPIPAddressEdit->Enabled = false;
		TCPIPPortEdit->Enabled    = false;
	}
	else
	if (port_index >= 100)
	{	// libusb connection

		int res;

		std::vector <t_vna_list> vna_list;
		res = janvna2_comms.getDeviceList(vna_list);
		if (res <= 0)
		{
			disconnect();
			return;
		}

		std::vector <String> params;
		common.parseString(device_name, " ", params);

		if (params.size() < 3)
		{
			disconnect();
			return;
		}

		if (params[0].UpperCase() != "JAN_VNA")
		{
			disconnect();
			return;
		}

		uint16_t vid = 0xffff;
		uint16_t pid = 0xffff;
		for (unsigned int i = 0; i < vna_list.size(); i++)
		{
			const t_vna_list *vna = &vna_list[i];
			if (vna->serial_number.UpperCase() == params[1].UpperCase())
			{	// found it
				vid = vna->vid;
				pid = vna->pid;
				break;
			}
		}

		if (vid == 0xffff && pid == 0xffff)
		{
			disconnect();
			return;
		}

		res = janvna2_comms.openDevice(vid, pid);
		if (res < 0)
		{
			pushCommMessage("connect: libusb not connected");

			const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
			ConnectDisconnectSpeedButton->OnClick = NULL;
			ConnectDisconnectSpeedButton->Down = false;
			ConnectDisconnectSpeedButton->OnClick = ne;

			disconnect();
			return;
		}

		pushCommMessage("connect: Jan's VNA connected");
	}
	else
	{	// serial port

		m_comms.serial.rts      = true;
		m_comms.serial.dtr      = true;
		m_comms.serial.byteSize = 8;
		m_comms.serial.parity   = NOPARITY;
		m_comms.serial.stopBits = ONESTOPBIT;
		m_comms.serial.baudRate = settings.serialPortBaudrate;

		pushCommMessage("connect: serial port connecting ..");

		if (m_comms.serial.Connect(device_name) != ERROR_SUCCESS)
		{
			while (m_comms.serial.errorCount() > 0)
			{
				t_serial_port_error err = m_comms.serial.pullError();
				pushCommMessage("serial error " + err.str);
			}

			pushCommMessage("connect: serial port not connected");

			const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
			ConnectDisconnectSpeedButton->OnClick = NULL;
			ConnectDisconnectSpeedButton->Down = false;
			ConnectDisconnectSpeedButton->OnClick = ne;
			return;
		}

		pushCommMessage("connect: serial port connected");
	}

	StopScanBitBtn->Enabled = true;

	{
		const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
		ConnectDisconnectSpeedButton->OnClick = NULL;
		ConnectDisconnectSpeedButton->Caption = "Connected";
		ConnectDisconnectSpeedButton->Down = true;
		ConnectDisconnectSpeedButton->OnClick = ne;
	}

	nanovna1_comms.m_poll_timer.mark();
	nanovna2_comms.m_poll_timer.mark();

	m_comms.rx_timer.mark();
	m_comms.link_timer.mark();

	{
		const TNotifyEvent ne = DeviceComboBox->OnChange;
		DeviceComboBox->OnChange = NULL;
		const int i = DeviceComboBox->Items->IndexOf(device_name);
		if (!device_name.IsEmpty() && i > 0)
			DeviceComboBox->ItemIndex = i;
		if (DeviceComboBox->ItemIndex != i)
			DeviceComboBox->ItemIndex = i;
		DeviceComboBox->OnChange = ne;
	}

	if (janvna2_comms.connected)
	{

	}

	if (m_comms.serial.connected)
	{
		// flush the SERIAL PORT RX
		uint8_t buffer[1024];
		const DWORD tick2 = GetTickCount();
		do Application->ProcessMessages();
		while ((GetTickCount() - 500) < tick2 && m_comms.serial.RxBytes(&buffer[0], sizeof(buffer)) > 0);
	}

	#ifdef TCPIPH
		if (m_comms.tcpip.connected)
		{	// flush the tcpip PORT RX
			uint8_t buffer[1024];
			do Application->ProcessMessages();
			while ((GetTickCount() - 500) < tick2 && m_comms.tcpip.RxBytes(&buffer[0], sizeof(buffer)) > 0);
		}
	#endif

	if (janvna2_comms.connected)
	{	// connected a libusb device
		data_unit.m_vna_data.type = UNIT_TYPE_JANVNA_V2;
	}
	else
	{	// start with checking to see if it's a V2
		data_unit.m_vna_data.type = UNIT_TYPE_NANOVNA_V2;
	}

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		// we need some information from the unit
		nanovna2_comms.reset(true);
//		nanovna2_comms.mode = MODE_INIT1;
	}
	else
	{	// V1
		// we need some information from the unit
		nanovna1_comms.reset();
	}

	// tpIdle tpLowest tpLower tpNormal tpHigher tpHighest tpTimeCritical

	if (data_unit.m_vna_data.type != UNIT_TYPE_JANVNA_V2)
	{
		if (!startThread())
		{
			nanovna1_comms.reset();
			nanovna1_comms.mode = MODE_NONE;

			nanovna2_comms.reset(true);
			nanovna2_comms.mode = MODE_NONE;

			janvna2_comms.reset(true);
			janvna2_comms.mode = MODE_NONE;

			disconnect();
			return;
		}
	}

	poll();
}

void __fastcall TForm1::saveImage(const bool to_clipboard)
{
	#ifdef USE_OPENGL
		Graphics::TBitmap *bm1 = NULL;
		Graphics::TBitmap *gl_bm = graphs.glCopyScreen();
		if (gl_bm == NULL)
		{
			bm1 = graphs.m_graph_bm;
			if (bm1 == NULL)
			{
				if (graphs.m_graph_bm == NULL)
				{
					Application->NormalizeTopMosts();
					Application->MessageBox(L"No image to save", L"Error", MB_ICONERROR | MB_OK);
					Application->RestoreTopMosts();
					return;
				}
			}
		}
		else
			bm1 = gl_bm;
	#else
		Graphics::TBitmap *bm1 = graphs.m_graph_bm;
		if (bm1 == NULL)
		{
			if (graphs.m_graph_bm == NULL)
			{
				Application->NormalizeTopMosts();
				Application->MessageBox(L"No image to save", L"Error", MB_ICONERROR | MB_OK);
				Application->RestoreTopMosts();
				return;
			}
		}
	#endif

	String filename = data_unit.m_vna_data.name.LowerCase();

	String sweep_name = SweepNameEdit->Text.Trim();

	#if 0
		// add the sweep name into the filename
		if (!sweep_name.IsEmpty())
			filename += "_" + sweep_name;
	#endif

	#if 0
		// add the graph type(s) into the filename
		for (int graph = 0; graph < graphs.numberOfGraphs(); graph++)
		{
			const int graph_type = settings.graphType[graph];

			switch (graph_type)
			{
				case GRAPH_TYPE_LOGMAG_S11:              filename += "_LOGMAG_S11";             break;
				case GRAPH_TYPE_LOGMAG_S21:              filename += "_LOGMAG_S21";             break;
				case GRAPH_TYPE_LOGMAG_S11S21:           filename += "_LOGMAG_S11S21";          break;
				case GRAPH_TYPE_LINMAG_S11:              filename += "_LINMAG_S11";             break;
				case GRAPH_TYPE_LINMAG_S21:              filename += "_LINMAG_S21";             break;
				case GRAPH_TYPE_LINMAG_S11S21:           filename += "_LINMAG_S11S21";          break;
				case GRAPH_TYPE_PHASE_S11:               filename += "_PHASE_S11";              break;
				case GRAPH_TYPE_PHASE_S21:               filename += "_PHASE_S21";              break;
				case GRAPH_TYPE_PHASE_S11S21:            filename += "_PHASE_S11S21";           break;
				case GRAPH_TYPE_PHASE_UNWRAP_S11:        filename += "_PHASE_UNWRAP_S11";       break;
				case GRAPH_TYPE_PHASE_UNWRAP_S21:        filename += "_PHASE_UNWRAP_S21";       break;
				case GRAPH_TYPE_PHASE_UNWRAP_S11S21:     filename += "_PHASE_UNWRAP_S11S21";    break;
				case GRAPH_TYPE_VSWR_S11:                filename += "_VSWR_S11";               break;
				case GRAPH_TYPE_IMPEDANCE_S11:           filename += "_IMPEDANCE_S11";          break;
				case GRAPH_TYPE_SERIES_RLC_S11:          filename += "_SERIES_RLC_S11";         break;
				case GRAPH_TYPE_PARALLEL_RLC_S11:        filename += "_PARALLEL_RLC_S11";       break;
				case GRAPH_TYPE_QUALITY_FACTOR_S11:      filename += "_QUALITY_FACTOR_S11";     break;
				case GRAPH_TYPE_SMITH_S11:               filename += "_SMITH_S11";              break;
				case GRAPH_TYPE_SMITH_S21:               filename += "_SMITH_S21";              break;
				case GRAPH_TYPE_ADMITTANCE_S11:          filename += "_ADMITTANCE_S11";         break;
				case GRAPH_TYPE_ADMITTANCE_S21:          filename += "_ADMITTANCE_S21";         break;
				case GRAPH_TYPE_POLAR_S11:               filename += "_POLAR_S11";              break;
				case GRAPH_TYPE_POLAR_S21:               filename += "_POLAR_S21";              break;
				case GRAPH_TYPE_GROUP_DELAY_S11:         filename += "_GROUP_DELAY_S11";        break;
				case GRAPH_TYPE_GROUP_DELAY_S21:         filename += "_GROUP_DELAY_S21";        break;
				case GRAPH_TYPE_GROUP_DELAY_S11S21:      filename += "_GROUP_DELAY_S11S21";     break;
				case GRAPH_TYPE_TDR_LIN_S11:             filename += "_TDR_LIN_S11";            break;
				case GRAPH_TYPE_TDR_LOG_S11:             filename += "_TDR_LOG_S11";            break;
				case GRAPH_TYPE_TDR_IMPEDANCE_S11:       filename += "_TDR_IMPEDANCE_S11";      break;
				case GRAPH_TYPE_REAL_IMAG_S11:           filename += "_REAL_IMAG_S11";          break;
				case GRAPH_TYPE_REAL_IMAG_S21:           filename += "_REAL_IMAG_S21";          break;
				case GRAPH_TYPE_SERIES_CAPACITANCE_S11:  filename += "_SERIES_CAPACITANCE_S11"; break;
				case GRAPH_TYPE_SERIES_INDUCTANCE_S11:   filename += "_SERIES_INDUCTANCE_S11";  break;
				case GRAPH_TYPE_COAX_LOSS_S11:           filename += "_COAX_LOSS_S11";          break;
				case GRAPH_TYPE_CAL_LOGMAG:              filename += "_CALIBRATIONS";           break;
				case GRAPH_TYPE_PHASE_VECTOR_S11:        filename += "_PHASE_VECTOR_S11";       break;
				case GRAPH_TYPE_PHASE_VECTOR_S21:        filename += "_PHASE_VECTOR_S21";       break;
				case GRAPH_TYPE_GJB_S11:                 filename += "_GJB_S11";                break;
				default:                                 filename += "_unknown";                break;
			}
		}
	#endif

	// add the sweep name and date and time onto the image

	Graphics::TBitmap *bm2 = bm2 = new Graphics::TBitmap();
	if (bm2)
	{  // make a copy of the image but with more height to fit the sweep name and date/time at the top

		// ****************
		// date time string

		String datetime_s = FormatDateTime("ddd dd mmm yyyy hh:nn:ss", Now());

		// ***************

		bm2->Monochrome           = false;
		bm2->Transparent          = false;
		bm2->PixelFormat          = pf32bit;

		bm2->Canvas->Font         = SweepNameFontLabel->Font;
//		bm2->Canvas->Font->Style  = bm2->Canvas->Font->Style << fsBold;
		const TSize sz1           = bm2->Canvas->TextExtent(sweep_name);
//		bm2->Canvas->Font->Style  = bm2->Canvas->Font->Style >> fsBold;

		bm2->Canvas->Font         = bm1->Canvas->Font;
//		bm2->Canvas->Font->Style  = bm2->Canvas->Font->Style << fsBold;
		const TSize sz2           = bm2->Canvas->TextExtent(datetime_s);
//		bm2->Canvas->Font->Style  = bm2->Canvas->Font->Style >> fsBold;

		int y1 = 3;
		int y2 = y1;

		if (!sweep_name.IsEmpty())
			y2 += sz1.cy + 3;
		y2 += sz2.cy + 3;

		bm2->Width                = bm1->Width;
		bm2->Height               = bm1->Height + y2;

		// background colour
		bm2->Canvas->Brush->Style = bsSolid;
		bm2->Canvas->Brush->Color = settings.m_colours.background;
		bm2->Canvas->FillRect(bm2->Canvas->ClipRect);

		// draw the current graph image onto the new image
		bm2->Canvas->CopyMode     = cmSrcCopy;
		bm2->Canvas->Draw(0, y2, bm1);

		// remove the border gradient
		bm2->Canvas->Brush->Style = bsClear;
		bm2->Canvas->Pen->Color   = settings.m_colours.background;
		bm2->Canvas->Pen->Style   = psSolid;
		bm2->Canvas->Pen->Width   = 1;
		for (int i = 0; i < settings.borderWidth; i++)
			bm2->Canvas->Rectangle(i, y2 + i, bm1->Width - i, y2 + bm1->Height - i);

		// add the sweep name text onto the new image
		if (!sweep_name.IsEmpty())
		{
			bm2->Canvas->Font         = SweepNameFontLabel->Font;
			bm2->Canvas->Font->Color  = settings.m_colours.font;
			bm2->Canvas->Brush->Style = bsClear;
			bm2->Canvas->TextOutA((bm2->Width - sz1.cx) / 2, y1, sweep_name);
			y1 += sz1.cy + 3;
		}

		// add the date//time text onto the new image
		bm2->Canvas->Font         = bm1->Canvas->Font;
		bm2->Canvas->Font->Color  = settings.m_colours.font;
		bm2->Canvas->Brush->Style = bsClear;
		bm2->Canvas->TextOutA((bm2->Width - sz2.cx) / 2, y1, datetime_s);
		//y1 += sz2.cy + 3;

		bm1 = bm2;
	}

	if (to_clipboard)
	{
		TClipboard *cb;
		try
		{
			cb = Clipboard();
			const DWORD tick = GetTickCount();
			do
			{
				try
				{
					cb->Clear();
					cb->Assign(bm1);		// copy the image to the clipboard
					Sleep(10);
					if (cb->HasFormat(CF_BITMAP))
						break;
				}
				catch (Exception &exception)
				{
				}
			} while ((GetTickCount() - 500) < tick);
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
			//String s = exception.ToString();
			if (bm2 != NULL)
				delete bm2;

			#ifdef USE_OPENGL
				if (gl_bm != NULL)
					delete gl_bm;
			#endif

			return;
		}
	}
	else
		common.saveBitmap(bm1, filename, "Save graph image ..", true);		// save the image to a file

	if (bm2 != NULL)
		delete bm2;

	#ifdef USE_OPENGL
		if (gl_bm != NULL)
			delete gl_bm;
	#endif
}

/*
// saves a DIB to a file
// SaveDIBToFile("my_bitmap.bmp", *bmi, lpBits);
DWORD __fastcall TForm1::saveDIBToFile(
		// name of the file to save as
		IN LPCSTR filename,
		// reference to the DIB
		IN const BITMAPINFO &bmi,
		// pointer to the pixels (if not packed)
		IN const unsigned char *pBits)
{
	DWORD dwBytesWrote = 0;

	// first determine the size (in bytes) of the color table
	int num_colors = bmi.bmiHeader.biClrUsed;
	if (num_colors == 0 && bmi.bmiHeader.biBitCount < 8)
		num_colors = (1u << bmi.bmiHeader.biBitCount);
	const size_t color_table_size = sizeof(RGBQUAD) * num_colors;

	// next, determine the size (in bytes) of the pixels array
	const size_t pixels_size = bmi.bmiHeader.biSizeImage != 0 ? bmi.bmiHeader.biSizeImage : ABS(bmi.bmiHeader.biHeight) * ((((bmi.bmiHeader.biWidth * bmi.bmiHeader.biBitCount) + 31) & ~31) >> 3);

	// initialize the file header
	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 0x4D42;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + color_table_size + pixels_size;
	bmfh.bfOffBits = bmfh.bfSize - pixels_size;

	HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		bool write_ok;
		DWORD dwWritten = 0;

		write_ok = WriteFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
		if (write_ok && (dwWritten == sizeof(BITMAPFILEHEADER)))
		{
			dwBytesWrote += dwWritten;
			write_ok = WriteFile(hFile, &bmi.bmiHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);

			if (write_ok && (dwWritten == sizeof(BITMAPINFOHEADER)))
			{
				dwBytesWrote += dwWritten;

				if (color_table_size != 0)
				{
					WriteFile(hFile, bmi.bmiColors, color_table_size, &dwWritten, NULL);
					dwBytesWrote += dwWritten;
				}

				if (!pBits)
					pBits = reinterpret_cast <unsigned char *> (const_cast <RGBQUAD *>(bmi.bmiColors) + num_colors);

				write_ok = WriteFile(hFile, pBits, pixels_size, &dwWritten, NULL);
				dwBytesWrote += dwWritten;
			}
		}

		CloseHandle(hFile);
	}

	return dwBytesWrote;
}
*/

void __fastcall TForm1::GraphPopupMenuPopup(TObject *Sender)
{
	m_popup_menu_mouse_Hz     = graphs.m_mouse.Hz;
	m_popup_menu_mouse_marker = graphs.m_mouse.marker_index;
	m_popup_menu_mouse_graph  = graphs.m_mouse.graph;

	if (m_popup_menu_mouse_graph < 0 && graphs.m_mouse.graph_v_scale >= 0)
		 m_popup_menu_mouse_graph = graphs.m_mouse.graph_v_scale;

	t_marker_freq marker_freq;
	t_marker_time marker_time;

	if (m_popup_menu_mouse_marker >= 0)
	{
		marker_freq = settings.m_markers_freq[m_popup_menu_mouse_marker];
	}
	else
	{
		marker_freq.Hz   = -1;
		marker_freq.type = MARKER_TYPE_NORMAL;
	}

	const int graph           = m_popup_menu_mouse_graph;
	const int graph_type      = (graph >= 0) ? settings.graphType[graph] : -1;
	const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

//	const bool freq_mode  = graphs.isFrequencyGraph(graph_type);
//	const bool smith_mode = graphs.isSmithGraph(graph_type) || isAdmittanceGraph(graph_type) || isPolarGraph(graph_type);
	const bool tdr_mode   = graphs.isTDRGraph(graph_type);

	Enablespline1->Checked       = settings.splineEnable;
	Showpoints1->Checked         = settings.showPoints;
	Cliptraces1->Checked         = settings.clipTraces;
	Snaptonearestpoint1->Checked = settings.snapToNearestPoint;

	Graphtype1->Enabled = (m_popup_menu_mouse_graph >= 0) ? true : false;
	if (m_popup_menu_mouse_graph >= 0)
	{
		for (int i = 0; i < Graphtype1->Count; i++)
		{
			if (Graphtype1->Items[i]->Tag == graph_type)
			{
				Graphtype1->Items[i]->Checked = true;
				break;
			}
		}
	}

	Addmarker1->Enabled       = !tdr_mode;
	Deletemarker1->Enabled    = (m_popup_menu_mouse_marker >= 0) ? true : false;

	Deltamarker2->Enabled     = (m_popup_menu_mouse_marker >= 0) ? true : false;
	Deltamarker2->Checked     = (marker_freq.type == MARKER_TYPE_DELTA) ? true : false;

	Markergraph1->Enabled     = false;
	//Markergraph1->Enabled     = (m_popup_menu_mouse_marker >= 0) ? true : false;
	if (m_popup_menu_mouse_marker >= 0)
	{
		const int mg = settings.m_markers_freq[m_popup_menu_mouse_marker].graph;
		Allgraphs1->Checked = (mg  < 0) ? true : false;
		Graph11->Checked    = (mg == 0) ? true : false;
		Graph21->Checked    = (mg == 1) ? true : false;
		Graph31->Checked    = (mg == 2) ? true : false;
		Graph41->Checked    = (mg == 3) ? true : false;
	}

	Markertrace1->Enabled     = false;
	//Markertrace1->Enabled     = (m_popup_menu_mouse_marker >= 0) ? true : false;
	if (m_popup_menu_mouse_marker >= 0)
	{
		const int mt = settings.m_markers_freq[m_popup_menu_mouse_marker].trace;
		Alltraces1->Checked = (mt  < 0) ? true : false;
		Trace11->Checked    = (mt == 0) ? true : false;
		Trace21->Checked    = (mt == 1) ? true : false;
	}

	{
		Showmarkers1->Checked     = (gs != NULL && gs->show_markers) ? true : false;
		Showminmaxtext1->Checked  = (gs != NULL && gs->show_min_max) ? true : false;
		Showmaxmarker1->Checked   = (gs != NULL && gs->show_max_marker) ? true : false;
		Showminmarker1->Checked   = (gs != NULL && gs->show_min_marker) ? true : false;

		Showmarkers1->Enabled     = (gs != NULL) ? true : false;
		Showminmaxtext1->Enabled  = (gs != NULL) ? true : false;
		Showmaxmarker1->Enabled   = (gs != NULL) ? true : false;
		Showminmarker1->Enabled   = (gs != NULL) ? true : false;
	}

	if (m_popup_menu_mouse_Hz > 0)
	{
		String s = common.freqToStr1(m_popup_menu_mouse_Hz, true, true, 6, false);

		Setasstartfrequency1->Caption  = "Set as start frequency [" + s + "]";
		Setascenterfrequency1->Caption = "Set as center frequency [" + s + "]";
		Setasstopfrequency1->Caption   = "Set as stop frequency [" + s + "]";

		Setasstartfrequency1->Enabled  = true;
		Setasstopfrequency1->Enabled   = true;
		Setascenterfrequency1->Enabled = true;
	}
	else
	{
		Setasstartfrequency1->Caption  = "Set as start frequency";
		Setascenterfrequency1->Caption = "Set as center frequency";
		Setasstopfrequency1->Caption   = "Set as stop frequency";

		Setasstartfrequency1->Enabled  = false;
		Setasstopfrequency1->Enabled   = false;
		Setascenterfrequency1->Enabled = false;
	}

	if (gs == NULL)
	{
		Automaxscale1->Enabled = false;
		Autominscale1->Enabled = false;

		Automaxscale1->Checked = false;
		Autominscale1->Checked = false;

		Setmaxscale1->Enabled  = false;
		Setminscale1->Enabled  = false;

		Setmaxscale1->Caption = "Set max scale";
		Setminscale1->Caption = "Set min scale";
	}
	else
	{
		bool enable_max_option = true;
		bool enable_min_option = true;

		switch (graph_type)
		{
			case GRAPH_TYPE_SMITH_S11:
			case GRAPH_TYPE_SMITH_S21:
			case GRAPH_TYPE_ADMITTANCE_S11:
			case GRAPH_TYPE_ADMITTANCE_S21:
			case GRAPH_TYPE_POLAR_S11:
			case GRAPH_TYPE_POLAR_S21:
				enable_max_option = true;
				enable_min_option = false;
				break;
			case GRAPH_TYPE_PHASE_VECTOR_S11:
			case GRAPH_TYPE_PHASE_VECTOR_S21:
				enable_max_option = false;
				enable_min_option = false;
				break;
			case GRAPH_TYPE_VSWR_S11:
				//enable_min_option = false;
				break;
		}

		Automaxscale1->Enabled = enable_max_option;
		Autominscale1->Enabled = enable_min_option;

		Automaxscale1->Checked = enable_max_option ? gs->auto_max : false;
		Autominscale1->Checked = enable_min_option ? gs->auto_min : false;

		Autoscalepeakhold1->Checked = settings.autoScalePeakHold;

		Scaletominmaxvalues1->Enabled = enable_max_option && enable_min_option;
		Scaletomaxvalue1->Enabled = enable_max_option;
		Scaletominvalue1->Enabled = enable_min_option;

		String s;

		if (enable_max_option)
		{
			s.printf(L"%0.5f", gs->max);
			// remove trailing zero's and unneeded decimal points
			if (s.Pos('.') > 0 || s.Pos(',') > 0)
			{
				while (!s.IsEmpty() && s[s.Length()] == '0')
					s = s.SubString(1, s.Length() - 1).Trim();
				if (!s.IsEmpty() && (s[s.Length()] == '.' || s[s.Length()] == ','))
					s = s.SubString(1, s.Length() - 1).Trim();
			}
			Setmaxscale1->Caption = "Set max scale [" + s + "]";
		}

		if (enable_min_option)
		{
			s.printf(L"%0.5f", gs->min);
			// remove trailing zero's and unneeded decimal points
			if (s.Pos('.') > 0 || s.Pos(',') > 0)
			{
				while (!s.IsEmpty() && s[s.Length()] == '0')
					s = s.SubString(1, s.Length() - 1).Trim();
				if (!s.IsEmpty() && (s[s.Length()] == '.' || s[s.Length()] == ','))
					s = s.SubString(1, s.Length() - 1).Trim();
			}
			Setminscale1->Caption = "Set min scale [" + s + "]";
		}

		Setmaxscale1->Enabled  = enable_max_option;
		Setminscale1->Enabled  = enable_min_option;
	}

	Deselectmarker1->Enabled = (selectedMarker() >= 0) ? true : false;
}

void __fastcall TForm1::Enablespline1Click(TObject *Sender)
{
	settings.splineEnable = !settings.splineEnable;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::Showpoints1Click(TObject *Sender)
{
	settings.showPoints = !settings.showPoints;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::updateDateTime()
{
	AnsiString datetime_s;
/*
	TIME_ZONE_INFORMATION tz_info;
	DYNAMIC_TIME_ZONE_INFORMATION dtz_info;

	BOOL ok = FALSE;
	SYSTEMTIME st = {0};
	int utcOffset = 0;
	int utcOffset2 = 0;
	do
	{
		utcOffset = GetUTCOffset();
		ok = GetSystemTime(&st);
		utcOffset2 = GetUTCOffset();
	} while(ok && utcOffset != utcOffset2);


	const DWORD res1 = GetTimeZoneInformation(&tz_info);
	const DWORD res2 = GetDynamicTimeZoneInformation(&dtz_info);

	TDateTime now = Now();
//	TTimeZone.Local

	if (res1 != TIME_ZONE_ID_INVALID && res1 != TIME_ZONE_ID_UNKNOWN)
	{
		const DWORD daylight = ABS(tz_info.DaylightBias);
		String s;
		s.printf(L"%02u:%02u", daylight / 60, daylight % 60);
		s = ((tz_info.DaylightBias >= 0) ? " (-" : " (+") + s + ")";
		datetime_s = FormatDateTime("ddd dd mmm yyyy hh:nn:ss", now) + s;
	}
	else
		datetime_s = FormatDateTime("ddd dd mmm yyyy hh:nn:ss", now);

	if (res2 != TIME_ZONE_ID_INVALID && res2 != TIME_ZONE_ID_UNKNOWN)
	{
		tz_info.Bias;
		tz_info.StandardBias;
		tz_info.DaylightBias;
		dtz_info.Bias;
		dtz_info.StandardBias;
		dtz_info.DaylightBias;
		const DWORD bias = ABS(dtz_info.StandardBias);
		String s;
		s.printf(L"%02u:%02u", bias / 60, bias % 60);
		s = ((dtz_info.StandardBias >= 0) ? " (-" : " (+") + s + ")";
		datetime_s = FormatDateTime("ddd dd mmm yyyy hh:nn:ss", now) + s;
	}
	else
		datetime_s = FormatDateTime("ddd dd mmm yyyy hh:nn:ss", now);
*/
	datetime_s = FormatDateTime("ddd dd mmm yyyy hh:nn:ss", Now());

	if (StatusBar1->Panels->Items[0]->Text != datetime_s)
	{
		StatusBar1->Panels->Items[0]->Text = datetime_s;
		StatusBar1->Update();
	}
}

void __fastcall TForm1::updateHistoryFramesInfo(const bool update)
{
	// number of history frames so far
	int hist_frames = data_unit.m_history_frames;
	if (hist_frames > (int)MAX_HISTORY)
		hist_frames = (int)MAX_HISTORY;
	if (hist_frames < 0)
		hist_frames = 0;

	int hist_pos = HistoryTrackBar->Position;
	if (hist_pos < -(hist_frames - 1))
		hist_pos = -(hist_frames - 1);
	if (hist_pos > 0)
		hist_pos = 0;

	String s;
	if (hist_pos == 0)
		s.printf(L"Hist %d / %d", hist_frames, (int)MAX_HISTORY);
	else
		s.printf(L"Hist %d / %d", hist_pos, (int)MAX_HISTORY);
	StatusBar1->Panels->Items[3]->Text = s;

//	if (update)
//		StatusBar1->Update();	// force the update to show immediately (no message delay)

	HistoryTrackBar->ShowSelRange = true;
	HistoryTrackBar->SelStart     = HistoryTrackBar->Max - hist_frames;
	HistoryTrackBar->SelEnd       = HistoryTrackBar->Max;
//	if (update)
//		HistoryTrackBar->Update();
}

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
	// ******************************
	// update the graph on a timer if it needs updating

	bool update_graph = false;

	if (Application->MainForm)
	{
		if (this->WindowState != wsMinimized)
		{
			#ifdef USE_OPENGL
				if (++m_window_not_minimized_count <= (500 / Timer1->Interval))	// for 500ms
					update_graph = true;
			#endif

			if (!update_graph && settings.autoScalePeakHold)
			{
				for (int graph = 0; graph < MAX_GRAPHS; graph++)
				{
					const int graph_type = settings.graphType[graph];
					const t_graph_setting *gs = &settings.m_graph_setting[graph_type];
					if ((gs->auto_max || gs->auto_min) && gs->auto_max_hold_count <= 0)
					{
						update_graph = true;
						break;
					}
				}
			}

			if (update_graph)
				::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);	// keep updating the graphs
		}
//		else
//			m_window_not_minimized_count = 0;
	}

	// ******************************

	updateDateTime();

	{	// set the memory if they held the button down for long enough
		const int mem = m_memory_but_index;
		if (mem >= 0 && m_button_timer.millisecs(false) >= 1000)
		{
			m_memory_but_index = -1;
			if (mem > 0)
			{
				setMemory(mem);
				Beep(2000, 10);
			}
		}
	}

/*
	if ((++m_comms.connect_timer_tick * Timer1->Interval) >= 1000)
	{
		m_comms.connect_timer_tick = 0;
		if (!settings.SerialPortName.IsEmpty() && settings.SerialPortName.Trim().LowerCase() != "none" && !connected())
		{	// connect
			connect();
		}
	}
*/
	// write the comms text messages to the comms window
	if (CommsForm)
	{
		if (CommsForm->Showing)
		{
			int num_done = 0;
			int num;
			while (CommsForm && (num = commMessagesCount()) > 0)
			{
				String s = pullCommMessage();
				if (s.IsEmpty())
					break;
				CommsForm->addText(FormatDateTime("ss.zzz ", Now()), s);
				if (++num_done >= 10)	// max of 10 lines in one go - to give time for the rest of the system to do it's thing
					break;
			}
		}
		else
			clearCommMessages();
	}
}

void __fastcall TForm1::clearCommMessages()
{
	CCriticalSection cs(m_comm_messages.cs);
	m_comm_messages.list.resize(0);
}

unsigned int __fastcall TForm1::commMessagesCount()
{
	CCriticalSection cs(m_comm_messages.cs);
	return m_comm_messages.list.size();
}

void TForm1::printfCommMessage(const char *fmt, ...)
{
	if (fmt == NULL)
		return;

	va_list ap;
	char tmp;

	va_start(ap, fmt);
		int buf_size = vsnprintf(&tmp, 0, fmt, ap);
	va_end(ap);

	if (buf_size == 0)
		return;

	if (buf_size == -1)
		buf_size = 512;

	char *buf = new char [buf_size + 1];
	if (buf == NULL)
		return;

	va_start(ap, fmt);
		vsnprintf_s(buf, buf_size + 1, fmt, ap);
	va_end(ap);

	String s = String(buf);

	delete buf;

	if (!s.IsEmpty())
	{
		CCriticalSection cs(m_comm_messages.cs);
		m_comm_messages.list.push_back(s);
	}
}

void __fastcall TForm1::pushCommMessage(String s)
{
	if (!s.IsEmpty())
	{
		CCriticalSection cs(m_comm_messages.cs);
		m_comm_messages.list.push_back(s);
	}
}

String __fastcall TForm1::pullCommMessage()
{
	String s;
	CCriticalSection cs(m_comm_messages.cs);
	if (!m_comm_messages.list.empty())
	{
		s = m_comm_messages.list[0];
		m_comm_messages.list.erase(m_comm_messages.list.begin() + 0);
	}
	return s;
}

void __fastcall TForm1::SerialPortBaudrateComboBoxChange(TObject *Sender)
{
	int i = SerialPortBaudrateComboBox->ItemIndex;
	if (i >= 0)
	{
		settings.serialPortBaudrate = (int)SerialPortBaudrateComboBox->Items->Objects[i];
		if (m_comms.serial.connected)
			m_comms.serial.baudRate = settings.serialPortBaudrate;
	}
}

void __fastcall TForm1::showWarnings()
{
	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	const bool valid_start  = (data_unit.m_freq_start_Hz < data_unit.m_freq_stop_Hz && data_unit.m_freq_start_Hz >= min_Hz && data_unit.m_freq_start_Hz <= max_Hz) ? true : false;
	const bool valid_stop   = (data_unit.m_freq_stop_Hz > data_unit.m_freq_start_Hz && data_unit.m_freq_stop_Hz  >= min_Hz && data_unit.m_freq_stop_Hz  <= max_Hz) ? true : false;
	const bool valid_center = (data_unit.m_freq_center_Hz >= (data_unit.m_freq_span_Hz / 2) &&
									  (data_unit.m_freq_center_Hz - (data_unit.m_freq_span_Hz / 2)) >= min_Hz &&
									  (data_unit.m_freq_center_Hz + (data_unit.m_freq_span_Hz / 2)) <= max_Hz) ? true : false;
	const bool valid_span   = (data_unit.m_freq_span_Hz <= (data_unit.m_freq_center_Hz * 2)) ? true : false;
	const bool valid_cw     = (data_unit.m_freq_cw_Hz >= min_Hz && data_unit.m_freq_cw_Hz <= max_Hz) ? true : false;

	common.setWarning(StartMHzEdit,  (!valid_start)  ? "Invalid start frequency" : "");
	common.setWarning(StopMHzEdit,   (!valid_stop)   ? "Invalid stop frequency" : "");
	common.setWarning(CenterMHzEdit, (!valid_center) ? "Invalid center frequency" : "");
	common.setWarning(SpanMHzEdit,   (!valid_span)   ? "Invalid span frequency" : "");
	common.setWarning(CWMHzEdit,     (!valid_cw)     ? "Invalid CW frequency" : "");
}

void __fastcall TForm1::VelocityFactorComboBoxChange(TObject *Sender)
{
	TNotifyEvent ne;
	String s;
	settings.velocityFactorName = VelocityFactorComboBox->Text;

	int vf = VelocityFactorComboBox->ItemIndex;
	if (vf > 0)
	{
		vf = (vf >= 0) ? (int)VelocityFactorComboBox->Items->Objects[vf] : 0;

		const float velocity_factor = (float)vf / 1000;

		//const float velocity_factor = 1.0f / sqrtf(dielectric_constant);
		const float dielectric_constant = SQR(1.0f / velocity_factor);

		data_unit.m_velocity_factor = velocity_factor;
		data_unit.m_max_distance_meters = (data_unit.m_freq_stop_Hz > 0) ? (5850000000.0f * data_unit.m_velocity_factor) / data_unit.m_freq_stop_Hz : 0;
		//stop_freq_Hz          = (5850000000 * data_unit.m_velocity_factor) / data_unit.m_max_distance_meters;

		String s;
		s.printf(L" TDR \n Velocity factor %0.3f \n dielectric constant %0.3f ", velocity_factor, dielectric_constant);
		VelocityFactorComboBox->Hint = s;

		VelocityFactorEdit->Enabled = false;
        ne = VelocityFactorEdit->OnChange;
		VelocityFactorEdit->OnChange = NULL;
		s.printf(L"%0.3f", velocity_factor);
		VelocityFactorEdit->Text = s;
		VelocityFactorEdit->OnChange = ne;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
	else
	{
		VelocityFactorComboBox->Hint = " TDR \n velocity Factor ";
		VelocityFactorEdit->Enabled = true;
        ne = VelocityFactorEdit->OnChange;
		VelocityFactorEdit->OnChange = NULL;
		s.printf(L"%0.6f", settings.velocityFactor);
		VelocityFactorEdit->Text = common.trimTrailingZeros(s);
		VelocityFactorEdit->OnChange = ne;
		VelocityFactorEditChange(VelocityFactorEdit);
	}
}

void __fastcall TForm1::TimeAverageLevelTrackBarChange(TObject *Sender)
{
	settings.timeAverageLevel = TimeAverageLevelTrackBar->Position;

	const int value = 1 << settings.timeAverageLevel;

	String s = (value <= 1) ? String("off") : IntToStr(value);

	TimeAverageLevelTrackBar->Hint = "Moving Average Filter: " + s;
	TimeAverageLevelTrackBar->ShowHint = true;

	MovingAverageFilterLabel->Caption = s;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::Saveimage1Click(TObject *Sender)
{
	saveImage(false);
}

void __fastcall TForm1::VelocityFactorEditChange(TObject *Sender)
{
	double velocity_factor;
	if (!common.strToValue(VelocityFactorEdit->Text, velocity_factor, 1e0) || velocity_factor <= 0.0 || velocity_factor > 1.5)
	{
		common.setWarning(VelocityFactorEdit, "Invalid coax velocity factor");
		return;
	}

	settings.velocityFactor = velocity_factor;

	data_unit.m_velocity_factor = velocity_factor;
	data_unit.m_max_distance_meters = (data_unit.m_freq_stop_Hz > 0) ? (5850000000.0f * data_unit.m_velocity_factor) / data_unit.m_freq_stop_Hz : 0;
//	stop_freq_Hz          = (5850000000 * data_unit.m_velocity_factor) / data_unit.m_max_distance_meters;

	const float dielectric_constant = SQR(1.0f / velocity_factor);
	//const float velocity_factor = 1.0f / sqrtf(dielectric_constant);

	common.setWarning(VelocityFactorEdit, "");

	String s;
	s.printf(L" TDR \n Velocity factor %0.3f \n dielectric constant %0.3f ", velocity_factor, dielectric_constant);
	VelocityFactorEdit->Hint = s;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::updateOutputPowerTrackBar(const int value)
{
	if (OutputPowerTrackBar->Position != value)
	{
		const TNotifyEvent ne = OutputPowerTrackBar->OnChange;
		OutputPowerTrackBar->OnChange = NULL;

		OutputPowerTrackBar->Position = value;

		String s;
		if (value < 0)
			s = "auto";
		else
			s.printf(L"%d", value);
		OutputPowerLabel2->Caption = s;

		OutputPowerTrackBar->OnChange = ne;
	}
}

void __fastcall TForm1::updateEDelayEdit()
{
	String s;
	const TNotifyEvent ne = EDelayEdit->OnChange;
	EDelayEdit->OnChange = NULL;
	s.printf(L"%0.6f", settings.eDelaySecs * 1e12);
	EDelayEdit->Text = common.trimTrailingZeros(s);
	EDelayEdit->OnChange = ne;
}

void __fastcall TForm1::updateS21OffsetEdit()
{
	String s;
	const TNotifyEvent ne = S21OffsetEdit->OnChange;
	S21OffsetEdit->OnChange = NULL;
	s.printf(L"%0.6f", settings.s21OffsetdB);
	S21OffsetEdit->Text = common.trimTrailingZeros(s);
	S21OffsetEdit->OnChange = ne;
}

void __fastcall TForm1::updateStepDisplay()
{
	String s = "Step (Hz)";

	const int mem = data_unit.firstUsedMem(true, 0);

	const int size = data_unit.freqArraySize(mem);
	if (mem >= 0 && size > 1)
	{
		const int64_t start_Hz = data_unit.m_point_mem[mem][0].Hz;
		const int64_t stop_Hz  = data_unit.m_point_mem[mem][size - 1].Hz;
		const double step_Hz   = (double)(stop_Hz - start_Hz) / (size - 1);
		s = "Step " + common.freqToStr1(step_Hz, true, true, 6, false) + "Hz";
	}
	else
	if (data_unit.m_freq_span_Hz > 0 && data_unit.m_points > 1)
	{
		const int64_t start_Hz = data_unit.m_freq_start_Hz;
		const int64_t stop_Hz  = data_unit.m_freq_stop_Hz;
		const double step_Hz   = (double)(stop_Hz - start_Hz) / (data_unit.m_points - 1);
		s = "Step " + common.freqToStr1(step_Hz, true, true, 6, false) + "Hz";
	}
	else
	if (settings.numOfPoints > 1)
	{
		const int64_t start_Hz = settings.startHz;
		const int64_t stop_Hz  = settings.stopHz;
		const double step_Hz   = (double)(stop_Hz - start_Hz) / (settings.numOfPoints - 1);
		s = "Step " + common.freqToStr1(step_Hz, true, true, 6, false) + "Hz";
	}

	if (StatusBar1->Panels->Items[5]->Text != s)
		StatusBar1->Panels->Items[5]->Text = s;
}

void __fastcall TForm1::TDRWindowTrackBarChange(TObject *Sender)
{
	settings.tdrWindow = TDRWindowTrackBar->Position;

	AnsiString s;

	s.printf("%d", settings.tdrWindow);
	TDRWindowLabel2->Caption = s;

	s.printf("TDR window %d", settings.tdrWindow);
	TDRWindowTrackBar->Hint = s;

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
		for (int graph = 0; graph < MAX_GRAPHS; graph++)
			data_unit.m_fft_window[graph][mem].resize(0);

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::DeviceComboBoxClick(TObject *Sender)
{
	m_comms.connect_timer_tick = 0;

	if (connected())
		disconnect();

	if (DeviceComboBox->ItemIndex > 0)
		connect();
}

void __fastcall TForm1::MarkerListViewData(TObject *Sender,
		TListItem *Item)
{
	TListView *lv = dynamic_cast<TListView *>(Sender);
	if (!lv || !Item)
		return;
	if (!lv->OwnerData)
		return;

	const int index = Item->Index;
	if (index < 0 || index >= (int)settings.m_markers_freq.size())
		return;

	Item->Data = (void *)(1 + index);
	Item->Caption = "";
	Item->SubItems->Add(IntToStr(1 + index));
	Item->SubItems->Add(common.freqToStr2(settings.m_markers_freq[index].Hz, 10));
}

void __fastcall TForm1::MarkerListViewKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	TListView *lv = dynamic_cast<TListView *>(Sender);
	if (lv == NULL)
		return;

	if (Key == VK_DELETE)
	{
		Key = 0;

		int deleted_items = 0;

		if (lv->Selected == NULL)
			return;

		//const int top_index = (lv->TopItem) ? lv->TopItem->Index : 0;

		int last_selected_index;
		for (last_selected_index = lv->Items->Count - 1; last_selected_index >= 0; last_selected_index--)
			if (lv->Items->Item[last_selected_index]->Selected)
				break;

		// we must go in reverse for this to work
		for (int i = lv->Items->Count - 1; i >= 0; i--)
		{
			TListItem *pItem = lv->Items->Item[i];
			if (!pItem)
				continue;
			if (!pItem->Selected)
				continue;

			//const int index = 1 + pItem->Index;
			const int index = pItem->Index;
			settings.m_markers_freq.erase(settings.m_markers_freq.begin() + index);	// delete a marker
			deleted_items++;
		}

		if (deleted_items > 0)
		{
			lv->ClearSelection();

			// select the one that followed the last selected one
			if (last_selected_index < 0 || last_selected_index > lv->Items->Count - 1)
				last_selected_index = lv->Items->Count - 1;
			if (last_selected_index >= 0)
			{
				lv->Items->Item[last_selected_index]->Selected = true;
				if (lv->Showing && lv->CanFocus())
					lv->Items->Item[last_selected_index]->Focused = true;
			}

			lv->Invalidate();

			if (Application->MainForm)
				::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
			//updateInfoPanel();
		}

		return;
	}
}

void __fastcall TForm1::MarkerListViewChange(TObject *Sender,
		TListItem *Item, TItemChange Change)
{
	TListView *lv = dynamic_cast<TListView *>(Sender);
	if (lv == NULL)
		return;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	//updateInfoPanel();
}

void __fastcall TForm1::MarkerListBoxKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	TListBox *lb = dynamic_cast<TListBox *>(Sender);
	if (lb == NULL)
		return;

	// find the first memory that contains data
	const int mem = data_unit.firstUsedMem(true, 0);

	const int size = data_unit.freqArraySize(mem);

	const int mi = selectedMarker();

	switch (Key)
	{
		case VK_DELETE:
			Key = 0;
			if (mi >= 0)
				settings.m_markers_freq.erase(settings.m_markers_freq.begin() + mi);	// delete a marker
			buildMarkerListBox();

			if (Application->MainForm)
				::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
			//updateInfoPanel();
			break;

		case VK_LEFT:	// left arrow key
			Key = 0;
			if (size > 0 && mi >= 0)
			{	// move the selected marker one point down
				const t_marker_freq marker = settings.m_markers_freq[mi];
				int i = data_unit.indexFreq(marker.Hz, mem);
				if (i >= 0)
				{
					if (--i < 0)
					  i = 0;
					settings.m_markers_freq[mi].Hz = data_unit.m_point_mem[mem][i].Hz;
					buildMarkerListBox();

					if (Application->MainForm)
						::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
					//updateInfoPanel();
				}
			}
			break;

		case VK_RIGHT:	// right arrow key
			Key = 0;
			if (size > 0 && mi >= 0)
			{	// move the selected marker one point up
				t_marker_freq marker = settings.m_markers_freq[mi];
				int i = data_unit.indexFreq(marker.Hz, mem);
				if (i >= 0)
				{
					if (++i > (size - 1))
					  i = size - 1;
					marker.Hz = data_unit.m_point_mem[mem][i].Hz;
					settings.m_markers_freq[mi] = marker;
					buildMarkerListBox();

					if (Application->MainForm)
						::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
					//updateInfoPanel();
				}
			}
			break;
	}
}

void __fastcall TForm1::MarkerListBoxClick(TObject *Sender)
{
	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
//	updateInfoPanel();
}

void __fastcall TForm1::DeviceComboBoxDropDown(TObject *Sender)
{
	updateDeviceComboBox();
}

void __fastcall TForm1::getVNAScreenCapture()
{
	if (!connected())
		return;

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		nanovna2_comms.m_get_screen_capture = true;

		if (nanovna2_comms.mode == MODE_IDLE || nanovna2_comms.m_pause_comms)
			requestCapture();
	}
	else
	{	// V1
		nanovna1_comms.m_get_screen_capture = true;

		if (nanovna1_comms.mode == MODE_IDLE || nanovna1_comms.m_pause_comms)
			requestCapture();
	}
}

void __fastcall TForm1::configGUI()
{
	createGraphTypeMenus();

	if (data_unit.m_vna_data.type == UNIT_TYPE_TINYSA)
	{
		int64_t max_Hz;
		int64_t min_Hz;
		data_unit.minMaxFreqHz(min_Hz, max_Hz);

		int64_t start_Hz = data_unit.m_vna_data.freq_start_Hz;
		int64_t stop_Hz  = data_unit.m_vna_data.freq_stop_Hz;

		if (start_Hz < min_Hz || start_Hz > max_Hz)
			start_Hz = min_Hz;

		if (stop_Hz < start_Hz || stop_Hz > 350000000)
			stop_Hz = 350000000;

		setStartStopHz(start_Hz, stop_Hz, false);
	}

	if (data_unit.m_vna_data.type == UNIT_TYPE_TINYSA)
	{
		PointBandwidthHzComboBox->Visible  = false;

//	 	VelocityFactorLabel->Visible       = false;
		VelocityFactorComboBox->Visible    = false;
		VelocityFactorEdit->Visible        = false;

		PointBandwidthLabel->Visible       = false;

		EDelayLabel->Visible               = false;
		EDelayEdit->Visible                = false;

		S21OffsetLabel->Visible            = false;
		S21OffsetEdit->Visible             = false;

		TDRWindowLabel1->Visible           = false;
		TDRWindowLabel2->Visible           = false;
		TDRWindowTrackBar->Visible         = false;

		OutputPowerLabel1->Visible         = false;
		OutputPowerLabel2->Visible         = false;
		OutputPowerTrackBar->Visible       = false;

		MarkerS11AdmittanceLabel1->Visible = false;
		MarkerS11AdmittanceLabel2->Visible = false;

		LCMatchingLabel->Visible           = false;
		LCMatchingToggleSwitch->Visible    = false;

		InfoPanelLabel2->Visible           = false;
		InfoPanelToggleSwitch->Visible     = false;

		BatteryVoltageBitBtn->Enabled      = true;
		VNAUsartCommsBitBtn->Enabled       = true;
	}
	else
	{
//		VelocityFactorLabel->Visible       = true;
		VelocityFactorComboBox->Visible    = true;
		VelocityFactorEdit->Visible        = true;
		VelocityFactorEdit->Enabled        = (VelocityFactorComboBox->ItemIndex <= 0) ? true : false;

		PointBandwidthLabel->Visible       = true;

		EDelayLabel->Visible               = true;
		EDelayEdit->Visible                = true;

		S21OffsetLabel->Visible            = true;
		S21OffsetEdit->Visible             = true;

		TDRWindowLabel1->Visible           = true;
		TDRWindowLabel2->Visible           = true;
		TDRWindowTrackBar->Visible         = true;

		OutputPowerLabel1->Visible         = true;
		OutputPowerLabel2->Visible         = true;
		OutputPowerTrackBar->Visible       = true;

		MarkerS11AdmittanceLabel1->Visible = true;
		MarkerS11AdmittanceLabel2->Visible = true;

		LCMatchingLabel->Visible           = true;
		LCMatchingToggleSwitch->Visible    = true;

		InfoPanelLabel2->Visible           = true;
		InfoPanelToggleSwitch->Visible     = true;

		if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
		{
			PointBandwidthHzComboBox->Enabled = true;
			BatteryVoltageBitBtn->Enabled     = false;
			VNAUsartCommsBitBtn->Enabled      = false;
		}
		else
		if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
		{	// V2
			PointBandwidthHzComboBox->Enabled = false;

			BatteryVoltageBitBtn->Enabled = false;
			if (BatteryVoltageForm)
				if (BatteryVoltageForm->Showing)
					BatteryVoltageForm->Hide();

			VNAUsartCommsBitBtn->Enabled = false;
			if (VNAUsartCommsForm)
				if (VNAUsartCommsForm->Showing)
					VNAUsartCommsForm->Hide();
		}
		else
		{	// V1
			PointBandwidthHzComboBox->Enabled = true;
			BatteryVoltageBitBtn->Enabled     = true;
			VNAUsartCommsBitBtn->Enabled      = true;
		}
	}
}

void __fastcall TForm1::updatePointBandwidthComboBox(const bool create)
{
	TComboBox *cb = PointBandwidthHzComboBox;

	const String s = cb->Text;

	const TNotifyEvent ne = cb->OnChange;
	cb->OnChange = NULL;
	cb->Items->BeginUpdate();
	cb->Clear();

	if (create)
	{
		for (int i = (int)ARRAY_SIZE(RBW_DEFAULT) - 1; i >= 0; i--)
		{
			const int bw = RBW_DEFAULT[i];
			cb->AddItem(IntToStr(bw), (TObject *)bw);
		}
		const int i = cb->Items->IndexOfObject((TObject *)4000);
		cb->ItemIndex = (i >= 0) ? i : 7;
	}
	else
	{
		int Hz = data_unit.m_vna_data.max_bandwidth_Hz;
		if (Hz > 25000)
		{
			cb->AddItem(IntToStr(Hz), (TObject *)Hz);
			Hz = 25000;
			cb->AddItem(IntToStr(Hz), (TObject *)Hz);
			Hz = 20000;
			cb->AddItem(IntToStr(Hz), (TObject *)Hz);
			Hz = 10000;
			cb->AddItem(IntToStr(Hz), (TObject *)Hz);
			Hz = 7500;
			cb->AddItem(IntToStr(Hz), (TObject *)Hz);
			Hz = 5000;
			cb->AddItem(IntToStr(Hz), (TObject *)Hz);
			Hz = 4000;
			cb->AddItem(IntToStr(Hz), (TObject *)Hz);
			Hz = 3000;
		}
		if (Hz > 2000)
		{
			cb->AddItem(IntToStr(Hz), (TObject *)Hz);
			Hz /= 2;
			while (Hz > 2000)
			{
				cb->AddItem(IntToStr(Hz), (TObject *)Hz);
				Hz /= 2;
			}
		}

		if (data_unit.m_vna_data.type != UNIT_TYPE_JANVNA_V2)
		{
			if (Hz >= 2000)
				cb->AddItem("2000", (TObject *)2000);
			if (Hz >= 1000)
				cb->AddItem("1000", (TObject *)1000);
			if (Hz >= 333)
				cb->AddItem("333",  (TObject *)333);
			if (Hz >= 100)
				cb->AddItem("100",  (TObject *)100);
			if (Hz >= 30)
				cb->AddItem("30",   (TObject *)30);
		}
		else
		{
			if (Hz >= 2000)
				cb->AddItem("2500", (TObject *)2000);
			if (Hz >= 1000)
				cb->AddItem("1000", (TObject *)1000);
			if (Hz >= 500)
				cb->AddItem("500",  (TObject *)500);
			if (Hz >= 200)
				cb->AddItem("250",  (TObject *)200);
			if (Hz >= 100)
				cb->AddItem("100",  (TObject *)100);
			if (Hz >= 50)
				cb->AddItem("50",   (TObject *)50);
			if (Hz >= 20)
				cb->AddItem("25",   (TObject *)20);
			if (Hz >= 10)
				cb->AddItem("10",   (TObject *)10);
		}

		if (!s.IsEmpty())
		{
			const int i = cb->Items->IndexOf(s);
			if (i >= 0)
				cb->ItemIndex = i;	// select the old value
		}
		else
			cb->ItemIndex = 0;
	}

	cb->Items->EndUpdate();
	cb->OnChange = ne;

	common.comboBoxAutoWidth(cb);

	if (!create)
		processPointBandwidthHzComboBox();
}

void __fastcall TForm1::updateNumberOfPointsComboBox(const bool process)
{
	TComboBox *cb = NumberOfPointsComboBox;

	// remember the current settings
	int prev_num_points = cb->ItemIndex;
	if (prev_num_points >= 0)
		prev_num_points = (int)cb->Items->Objects[prev_num_points];

	const TNotifyEvent ne = cb->OnChange;
	cb->OnChange = NULL;

	cb->Items->BeginUpdate();
	cb->Clear();

	switch (data_unit.m_vna_data.type)
	{
		case UNIT_TYPE_NANOVNA_H:
		case UNIT_TYPE_NANOVNA_H4:
		case UNIT_TYPE_NANOVNA_H7:
			{
				for (unsigned int i = 0; i < ARRAY_SIZE(NUM_POINTS_V1); i++)
				{
					const int num = NUM_POINTS_V1[i];
					cb->AddItem(IntToStr(num), (TObject *)num);
				}
			}
			break;

		case UNIT_TYPE_NANOVNA_V2:
			if (data_unit.m_vna_data.hardware_revision != REG_V2_HARDWARE_REVISION_ACK_2_4)
			{
				for (unsigned int i = 0; i < ARRAY_SIZE(NUM_POINTS_V2); i++)
				{
					const int num = NUM_POINTS_V2[i];
					cb->AddItem(IntToStr(num), (TObject *)num);
				}
			}
			else
			{
				for (unsigned int i = 0; i < ARRAY_SIZE(NUM_POINTS_V2PLUS4); i++)
				{
					const int num = NUM_POINTS_V2PLUS4[i];
					cb->AddItem(IntToStr(num), (TObject *)num);
				}
			}
			break;

		case UNIT_TYPE_JANVNA_V2:
			{
				for (unsigned int i = 0; i < ARRAY_SIZE(NUM_POINTS_JANVNA_V2); i++)
				{
					const int num = NUM_POINTS_JANVNA_V2[i];
					cb->AddItem(IntToStr(num), (TObject *)num);
				}
			}
			break;

		case UNIT_TYPE_TINYSA:
			{
				for (unsigned int i = 0; i < ARRAY_SIZE(NUM_POINTS_TINYSA); i++)
				{
					const int num = NUM_POINTS_TINYSA[i];
					cb->AddItem(IntToStr(num), (TObject *)num);
				}
			}
			break;

		default:
			{
				for (unsigned int i = 0; i < ARRAY_SIZE(NUM_POINTS_DEFAULT); i++)
				{
					const int num = NUM_POINTS_DEFAULT[i];
					cb->AddItem(IntToStr(num), (TObject *)num);
				}
			}
			break;
	}

	int item_index = 0;
	// select a default value
	if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
	{
		const int i = cb->Items->IndexOfObject((TObject *)401);
		item_index = (i >= 0) ? i : 1;
	}
	else
	{
		const int i = cb->Items->IndexOfObject((TObject *)500);
		item_index = (i >= 0) ? i : 4;
	}
	// re-select the previous selection
	if (prev_num_points >= 0)
	{
		const int i = cb->Items->IndexOfObject((TObject *)prev_num_points);
		if (i >= 0)
			item_index = i;
	}
	cb->ItemIndex = item_index;

	cb->Items->EndUpdate();

	cb->OnChange = ne;

	common.comboBoxAutoWidth(cb);

	if (process)
		processNumberOfPointsComboBox();
}

void __fastcall TForm1::updateVelocityFactorComboBox()
{
	TComboBox *cb = VelocityFactorComboBox;

	const TNotifyEvent ne = cb->OnChange;
	cb->OnChange = NULL;
	cb->Items->BeginUpdate();
	cb->Clear();

	cb->AddItem("Custom",                                     (TObject *)0);
	cb->AddItem("RG-6/U PE 75R (Belden 8215) (0.66)",         (TObject *)IROUND(0.66  * 1000));
	cb->AddItem("RG-6/U Foam 75R (Belden 9290) (0.81)",       (TObject *)IROUND(0.81  * 1000));
	cb->AddItem("RG-8/U PE 50R (Belden 8237) (0.66)",         (TObject *)IROUND(0.66  * 1000));
	cb->AddItem("RG-8/U Foam (Belden 8214) (0.78)",           (TObject *)IROUND(0.78  * 1000));
	cb->AddItem("RG-8/U (Belden 9913) (0.84)",                (TObject *)IROUND(0.84  * 1000));
	cb->AddItem("RG-8X (Belden 9258) (0.82)",                 (TObject *)IROUND(0.82  * 1000));
	cb->AddItem("RG-8X (Mini-8) (0.8)",                       (TObject *)IROUND(0.80  * 1000));
	cb->AddItem("RG-11/U 75R Foam HDPE (Belden 9292) (0.84)", (TObject *)IROUND(0.84  * 1000));
	cb->AddItem("RG-58/U 52R PE (Belden 9201) (0.66)",        (TObject *)IROUND(0.66  * 1000));
	cb->AddItem("RG-58A/U 54R Foam (Belden 8219) (0.73)",     (TObject *)IROUND(0.73  * 1000));
	cb->AddItem("RG-59A/U PE 75R (Belden 8241) (0.66)",       (TObject *)IROUND(0.66  * 1000));
	cb->AddItem("RG-59A/U Foam 75R (Belden 8241F) (0.78)",    (TObject *)IROUND(0.78  * 1000));
	cb->AddItem("RG-142/U (0.69)",                            (TObject *)IROUND(0.69  * 1000));
	cb->AddItem("RG-174 PE (Belden 8216) (0.66)",             (TObject *)IROUND(0.66  * 1000));
	cb->AddItem("RG-174 Foam (Belden 7805R) (0.735)",         (TObject *)IROUND(0.735 * 1000));
	cb->AddItem("RG-213/U PE (Belden 8267) (0.66)",           (TObject *)IROUND(0.66  * 1000));
	cb->AddItem("RG-213 FOAM (0.8)",                          (TObject *)IROUND(0.80  * 1000));
	cb->AddItem("RG-223/U (0.66)",                            (TObject *)IROUND(0.66  * 1000));
	cb->AddItem("RG-316 (0.695)",                             (TObject *)IROUND(0.695 * 1000));
	cb->AddItem("RG-402 (0.695)",                             (TObject *)IROUND(0.695 * 1000));
	cb->AddItem("WF-103 (0.81)",                              (TObject *)IROUND(0.81  * 1000));
	cb->AddItem("LMR-240 (0.84)",                             (TObject *)IROUND(0.84  * 1000));
	cb->AddItem("LMR-240UF (0.80)",                           (TObject *)IROUND(0.80  * 1000));
	cb->AddItem("LMR-400 (0.85)",                             (TObject *)IROUND(0.85  * 1000));
	cb->AddItem("LMR400UF (0.83)",                            (TObject *)IROUND(0.83  * 1000));
	cb->AddItem("Davis Bury-FLEX (0.82)",                     (TObject *)IROUND(0.82  * 1000));
	cb->AddItem("UltraFlex 7 (0.83)",                         (TObject *)IROUND(0.83  * 1000));
	cb->AddItem("UltraFlex 10 (0.83)",                        (TObject *)IROUND(0.83  * 1000));
	cb->AddItem("EcoFlex 10 (0.86)",                          (TObject *)IROUND(0.86  * 1000));
	cb->AddItem("EcoFlex 10 Heatex (0.85)",                   (TObject *)IROUND(0.85  * 1000));
	cb->AddItem("EcoFlex 15 (0.86)",                          (TObject *)IROUND(0.86  * 1000));
	cb->AddItem("EcoFlex 15+ (0.86)",                         (TObject *)IROUND(0.86  * 1000));
	cb->AddItem("Jelly filled (0.64)",                        (TObject *)IROUND(0.64  * 1000));	// velocity factor * 1000
	cb->AddItem("Polyethylene (0.66)",                        (TObject *)IROUND(0.66  * 1000));
	cb->AddItem("PTFE (Teflon) (0.70)",                       (TObject *)IROUND(0.70  * 1000));
	cb->AddItem("Pulp Insulation (0.72)",                     (TObject *)IROUND(0.72  * 1000));
	cb->AddItem("Foam or Cellular PE (0.78)",                 (TObject *)IROUND(0.78  * 1000));
	cb->AddItem("Semi-solid PE (SSPE) (0.84)",                (TObject *)IROUND(0.84  * 1000));
	cb->AddItem("Air (Helical spacers) (0.94)",               (TObject *)IROUND(0.94  * 1000));

	cb->ItemIndex = 0;

	cb->Items->EndUpdate();
	cb->OnChange = ne;

	common.comboBoxAutoWidth(cb);

	VelocityFactorEdit->Enabled = (cb->ItemIndex <= 0) ? true : false;
	VelocityFactorEdit->Visible = true;
}

void __fastcall TForm1::updateCalibrationSelectComboBox()
{
	TComboBox *cb = CalibrationSelectComboBox;

	// remember the current settings
	int cal_select = cb->ItemIndex;
	if (cal_select >= 0)
		cal_select = (int)cb->Items->Objects[cal_select];

	const TNotifyEvent ne = cb->OnChange;
	cb->OnChange = NULL;
	cb->Items->BeginUpdate();
	cb->Clear();

	cb->AddItem("None", (TObject *)CAL_SELECT_NONE);
	if (!connected() || (data_unit.m_vna_data.type != UNIT_TYPE_JANVNA_V2 && data_unit.m_vna_data.type != UNIT_TYPE_NANOVNA_V2))
		cb->AddItem("VNA",  (TObject *)CAL_SELECT_VNA);
	cb->AddItem("APP",  (TObject *)CAL_SELECT_APP);

	const int i = cb->Items->IndexOfObject((TObject *)cal_select);
	cb->ItemIndex = (i >= 0) ? i : 8;

	cb->Items->EndUpdate();
	cb->OnChange = ne;

	common.comboBoxAutoWidth(cb);

	cal_select = cb->ItemIndex;
	if (cal_select >= 0)
	{
		const int i = reinterpret_cast<const int>(cb->Items->Objects[cal_select]);
		settings.calibrationSelection = (t_calibration_selection)i;
	}

	setCalibrationSelection(settings.calibrationSelection);
}

void __fastcall TForm1::clearMemory(const int mem)
{
	if (mem <= 0 || mem >= MAX_MEMORIES)
		return;

	// clear the memory
	data_unit.m_point_mem[mem].resize(0);
	data_unit.m_point_filt[mem].resize(0);

	TSpeedButton *sb;
	switch (mem)
	{
		case 1: sb = MemorySpeedButton1; break;
		case 2: sb = MemorySpeedButton2; break;
		case 3: sb = MemorySpeedButton3; break;
		case 4: sb = MemorySpeedButton4; break;
		default: return;
	}

	if (sb != NULL)
	{
		sb->Caption = "m" + IntToStr(mem);
		sb->Down    = false;

		String s = settings.memoryName[mem];
		if (s.IsEmpty())
			s = "Right click for options";
		sb->Hint = s;
	}

	settings.memoryEnable[mem] = false;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::loadMemoryFile(const int mem)
{
	if (mem <= 0 || mem >= MAX_MEMORIES)
		return;

	std::vector <t_data_point> s_params;

	String fn = common.loadSParams(s_params, "");

	if (fn.IsEmpty())
		return;

	data_unit.m_point_mem[mem] = s_params;

	data_unit.m_point_filt[mem].resize(0);

	settings.memoryEnable[mem] = true;

	TSpeedButton *sb = NULL;
	switch (mem)
	{
		case 1: sb = MemorySpeedButton1; break;
		case 2: sb = MemorySpeedButton2; break;
		case 3: sb = MemorySpeedButton3; break;
		case 4: sb = MemorySpeedButton4; break;
	}

	if (sb != NULL)
	{
		String s = settings.memoryName[mem];
		if (s.IsEmpty())
		{
			const int size = data_unit.m_point_mem[mem].size();
			if (size > 0)
			{
				s  = common.freqToStr1(data_unit.m_point_mem[mem][0].Hz, true, false, 6, false) + "Hz";
				s += " to ";
				s += common.freqToStr1(data_unit.m_point_mem[mem][size - 1].Hz, true, false, 6, false) + "Hz";
			}
			s += " " + IntToStr(size) + " points";
			s = s.Trim() + " .. " + fn;
		}

		sb->Caption = "M" + IntToStr(mem);
		sb->Down    = settings.memoryEnable[mem];
		sb->Hint = s;
	}
}

void __fastcall TForm1::setMemory(const int mem)
{
	if (mem <= 0 || mem >= MAX_MEMORIES)
		return;

	// copy the live memory into the desired memory
//	data_unit.m_point_mem[mem] = data_unit.m_point_mem[0];
	data_unit.m_point_mem[mem] = data_unit.m_point_filt[0];

	data_unit.m_point_filt[mem].resize(0);

	TSpeedButton *sb;
	switch (mem)
	{
		case 1: sb = MemorySpeedButton1; break;
		case 2: sb = MemorySpeedButton2; break;
		case 3: sb = MemorySpeedButton3; break;
		case 4: sb = MemorySpeedButton4; break;
		default: return;
	}

	if (data_unit.m_point_mem[mem].empty())
	{
		sb->Caption = "m" + IntToStr(mem);
		sb->Down    = false;
		sb->Hint    = settings.memoryName[mem].IsEmpty() ? String("Right click for options") : settings.memoryName[mem];
	}
	else
	{
		String s = settings.memoryName[mem];
		if (s.IsEmpty())
		{
			const int size = data_unit.m_point_mem[mem].size();
			if (size > 0)
			{
				s  = common.freqToStr1(data_unit.m_point_mem[mem][0].Hz, true, false, 6, false) + "Hz";
				s += " to ";
				s += common.freqToStr1(data_unit.m_point_mem[mem][size - 1].Hz, true, false, 6, false) + "Hz";
			}
			s += " " + IntToStr(size) + " points";
			s = "User set - " + s;
		}
		sb->Caption = "M" + IntToStr(mem);
		sb->Down    = true;
		sb->Hint    = s;
		settings.memoryEnable[mem] = sb->Down;
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::FormDblClick(TObject *Sender)
{
//	saveFormImage();
}

void __fastcall TForm1::processEDelayEdit()
{
	double d = 0;
	String s = common.localiseDecimalPoint(EDelayEdit->Text.Trim());
	if (!s.IsEmpty())
	{
		if (!common.strToValue(s, d, 1e-12))	// default to pico seconds
		{
			common.setWarning(EDelayEdit, "Invalid electrical delay value");
			return;
		}
	}
	common.setWarning(EDelayEdit, "");
	settings.eDelaySecs = d;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::processS21OffsetEdit()
{
	double d = 0;
	String s = common.localiseDecimalPoint(S21OffsetEdit->Text.Trim());
	if (!s.IsEmpty())
	{
		if (!common.strToValue(s, d, 1.0))
		{
			common.setWarning(S21OffsetEdit, "Invalid S21 offset value");
			return;
		}
	}
	common.setWarning(S21OffsetEdit, "");
	settings.s21OffsetdB = d;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

bool __fastcall TForm1::processStartMHzEdit(const bool update_other_edits, const bool update_own_edit)
{
	TNotifyEvent ne;
	double d;

	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	if (!common.strToMHz(StartMHzEdit->Text, d))
	{
		common.setWarning(StartMHzEdit, "Invalid start frequency");
		return false;
	}

	const int64_t Hz = I64ROUND(d * 1e6);

	const bool valid = (Hz <= data_unit.m_freq_stop_Hz && Hz >= min_Hz && Hz <= max_Hz) ? true : false;

	const bool changed = (data_unit.m_freq_start_Hz != Hz) ? true : false;

	common.setWarning(StartMHzEdit, (!valid) ? "Invalid start frequency" : "");

	if (valid)
	{
		if (changed)
		{
			if (scanning())
				stop();

			data_unit.m_freq_start_Hz  = Hz;
			data_unit.m_freq_span_Hz   = data_unit.m_freq_stop_Hz  - data_unit.m_freq_start_Hz;
			data_unit.m_freq_center_Hz = data_unit.m_freq_start_Hz + (data_unit.m_freq_span_Hz / 2);
		}

		if (update_other_edits)
		{
			ne                      = CenterMHzEdit->OnChange;
			CenterMHzEdit->OnChange = NULL;
			CenterMHzEdit->Text     = common.freqToStrMHz(data_unit.m_freq_center_Hz);
			CenterMHzEdit->Update();
			CenterMHzEdit->OnChange = ne;

			ne                    = SpanMHzEdit->OnChange;
			SpanMHzEdit->OnChange = NULL;
			SpanMHzEdit->Text     = common.freqToStrMHz(data_unit.m_freq_span_Hz);
			SpanMHzEdit->Update();
			SpanMHzEdit->OnChange = ne;
		}

		if (update_own_edit)
		{
			ne = StartMHzEdit->OnChange;
			StartMHzEdit->OnChange = NULL;
			StartMHzEdit->Text     = common.freqToStrMHz(data_unit.m_freq_start_Hz);
			StartMHzEdit->Update();
			StartMHzEdit->OnChange = ne;
		}

		updateStepDisplay();
	}

//	showWarnings();

	if (valid && changed && scanning())
		resetFreqArray();

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);

	return valid;
}

bool __fastcall TForm1::processStopMHzEdit(const bool update_other_edits, const bool update_own_edit)
{
	TNotifyEvent ne;
	double d;

	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	if (!common.strToMHz(StopMHzEdit->Text, d))
	{
		common.setWarning(StopMHzEdit, "Invalid stop frequency");
		return false;
	}

	const int64_t Hz = I64ROUND(d * 1e6);

	const bool valid = (Hz >= data_unit.m_freq_start_Hz && Hz >= min_Hz && Hz <= max_Hz) ? true : false;

	const bool changed = (data_unit.m_freq_stop_Hz != Hz) ? true : false;

	common.setWarning(StopMHzEdit, (!valid) ? "Invalid stop frequency" : "");

	if (valid)
	{
		if (changed)
		{
			if (scanning())
				stop();

			data_unit.m_freq_stop_Hz   = Hz;
			data_unit.m_freq_span_Hz   = data_unit.m_freq_stop_Hz  - data_unit.m_freq_start_Hz;
			data_unit.m_freq_center_Hz = data_unit.m_freq_start_Hz + (data_unit.m_freq_span_Hz / 2);
		}

		if (update_other_edits)
		{
			ne                      = CenterMHzEdit->OnChange;
			CenterMHzEdit->OnChange = NULL;
			CenterMHzEdit->Text     = common.freqToStrMHz(data_unit.m_freq_center_Hz);
			CenterMHzEdit->Update();
			CenterMHzEdit->OnChange = ne;

			ne                    = SpanMHzEdit->OnChange;
			SpanMHzEdit->OnChange = NULL;
			SpanMHzEdit->Text     = common.freqToStrMHz(data_unit.m_freq_span_Hz);
			SpanMHzEdit->Update();
			SpanMHzEdit->OnChange = ne;
		}

		if (update_own_edit)
		{
			ne                    = StopMHzEdit->OnChange;
			StopMHzEdit->OnChange = NULL;
			StopMHzEdit->Text     = common.freqToStrMHz(data_unit.m_freq_stop_Hz);
			StopMHzEdit->Update();
			StopMHzEdit->OnChange = ne;
		}

		updateStepDisplay();
	}

//	showWarnings();

	if (valid && changed && scanning())
		resetFreqArray();

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);

	return valid;
}

bool __fastcall TForm1::processCenterMHzEdit(const bool update_other_edits, const bool update_own_edit)
{
	TNotifyEvent ne;
	double d;

	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	if (!common.strToMHz(CenterMHzEdit->Text, d))
	{
		common.setWarning(CenterMHzEdit, "Invalid center frequency");
		return false;
	}

	const int64_t Hz = I64ROUND(d * 1e6);

	const bool valid = (Hz >= ((data_unit.m_freq_span_Hz / 2) - min_Hz) && Hz <= ((data_unit.m_freq_span_Hz / 2) + max_Hz)) ? true : false;

	const bool changed = (data_unit.m_freq_center_Hz != Hz) ? true : false;

	common.setWarning(CenterMHzEdit, (!valid) ? "Invalid center frequency" : "");

	if (valid)
	{
		if (changed)
		{
			if (scanning())
				stop();

			data_unit.m_freq_center_Hz = Hz;
			data_unit.m_freq_start_Hz  = data_unit.m_freq_center_Hz - (data_unit.m_freq_span_Hz / 2);
			data_unit.m_freq_stop_Hz   = data_unit.m_freq_center_Hz + (data_unit.m_freq_span_Hz / 2);
		}

		if (update_other_edits)
		{
			ne                     = StartMHzEdit->OnChange;
			StartMHzEdit->OnChange = NULL;
			StartMHzEdit->Text     = common.freqToStrMHz(data_unit.m_freq_start_Hz);
			StartMHzEdit->Update();
			StartMHzEdit->OnChange = ne;

			ne                    = StopMHzEdit->OnChange;
			StopMHzEdit->OnChange = NULL;
			StopMHzEdit->Text     = common.freqToStrMHz(data_unit.m_freq_stop_Hz);
			StopMHzEdit->Update();
			StopMHzEdit->OnChange = ne;
		}

		if (update_own_edit)
		{
			ne                      = CenterMHzEdit->OnChange;
			CenterMHzEdit->OnChange = NULL;
			CenterMHzEdit->Text     = common.freqToStrMHz(data_unit.m_freq_center_Hz);
			CenterMHzEdit->Update();
			CenterMHzEdit->OnChange = ne;
		}

		updateStepDisplay();
	}

//	showWarnings();

	if (valid && changed && scanning())
		resetFreqArray();

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);

	return valid;
}

bool __fastcall TForm1::processSpanMHzEdit(const bool update_other_edits, const bool update_own_edit)
{
	TNotifyEvent ne;
	double d;

	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	if (!common.strToMHz(SpanMHzEdit->Text, d))
	{
		common.setWarning(SpanMHzEdit, "Invalid span frequency");
		return false;
	}

	const int64_t Hz = I64ROUND(d * 1e6);

	const bool valid = (data_unit.m_freq_center_Hz >= ((Hz / 2) - min_Hz) && data_unit.m_freq_center_Hz <= ((Hz / 2) + max_Hz)) ? true : false;

	const bool changed = (data_unit.m_freq_span_Hz != Hz) ? true : false;

	common.setWarning(SpanMHzEdit, (!valid) ? "Invalid span frequency" : "");

	if (valid)
	{
		if (changed)
		{
			if (scanning())
				stop();

			data_unit.m_freq_span_Hz  = Hz;
			data_unit.m_freq_start_Hz = data_unit.m_freq_center_Hz - (data_unit.m_freq_span_Hz / 2);
			data_unit.m_freq_stop_Hz  = data_unit.m_freq_center_Hz + (data_unit.m_freq_span_Hz / 2);
		}

		if (update_other_edits)
		{
			ne                     = StartMHzEdit->OnChange;
			StartMHzEdit->OnChange = NULL;
			StartMHzEdit->Text     = common.freqToStrMHz(data_unit.m_freq_start_Hz);
			StartMHzEdit->Update();
			StartMHzEdit->OnChange = ne;

			ne                    = StopMHzEdit->OnChange;
			StopMHzEdit->OnChange = NULL;
			StopMHzEdit->Text     = common.freqToStrMHz(data_unit.m_freq_stop_Hz);
			StopMHzEdit->Update();
			StopMHzEdit->OnChange = ne;

//			ne = ScanSpanTrackBar->OnChange;
//			ScanSpanTrackBar->OnChange = NULL;
//			double span = (double)ScanSpanTrackBar->Position / ScanSpanTrackBar->Max;  // 0.0 to 1.0
//			span = span * span * span; // non-linear
//			ScanSpanTrackBar->OnChange = ne;
		}

		if (update_own_edit)
		{
			ne                    = SpanMHzEdit->OnChange;
			SpanMHzEdit->OnChange = NULL;
			SpanMHzEdit->Text     = common.freqToStrMHz(data_unit.m_freq_span_Hz);
			SpanMHzEdit->Update();
			SpanMHzEdit->OnChange = ne;
		}

		updateStepDisplay();
	}

//	showWarnings();

	if (valid && changed && scanning())
		resetFreqArray();

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);

	return valid;
}

bool __fastcall TForm1::processCWMHzEdit(const bool update_other_edits, const bool update_own_edit)
{
	TNotifyEvent ne;
	double d;

	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	if (!common.strToMHz(CWMHzEdit->Text, d))
	{
		common.setWarning(CWMHzEdit, "Invalid CW frequency");
		return false;
	}

	const int64_t Hz = I64ROUND(d * 1e6);

	const bool valid = (Hz >= min_Hz && Hz <= max_Hz) ? true : false;

	const bool changed = (data_unit.m_freq_cw_Hz != Hz) ? true : false;

	common.setWarning(CWMHzEdit, (!valid) ? "Invalid CW frequency" : "");

	if (valid)
	{
		if (changed)
		{
			data_unit.m_freq_cw_Hz = Hz;
		}

		if (update_own_edit)
		{
			const TNotifyEvent ne = CWMHzEdit->OnChange;
			CWMHzEdit->OnChange   = NULL;
			CWMHzEdit->Text       = common.freqToStrMHz(data_unit.m_freq_cw_Hz);
			CWMHzEdit->Update();
			CWMHzEdit->OnChange   = ne;
		}

		updateStepDisplay();

		//cwPopupWindowupdatenTrackBar(CWMHzEdit);
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);

	return valid;
}

bool __fastcall TForm1::processPointBandwidthHzComboBox()
{
	String s;
	double d;

	if (!common.strToValue(PointBandwidthHzComboBox->Text, d, 1.0))
	{
		s.printf(L"Invalid bandwidth [10 to %d]", data_unit.m_vna_data.max_bandwidth_Hz);
		common.setWarning(PointBandwidthHzComboBox, s);
		return false;
	}

	common.setWarning(PointBandwidthHzComboBox, "");

	int Hz = IROUND(d);
	if (Hz < 10) Hz = 10;
	else
	if (connected() && data_unit.m_vna_data.max_bandwidth_Hz > 0)
		if (Hz > data_unit.m_vna_data.max_bandwidth_Hz)
			Hz = data_unit.m_vna_data.max_bandwidth_Hz;

	if (data_unit.m_vna_data.adc_Hz > 0 && data_unit.m_vna_data.audio_samples_count > 0)
	{
		const int clk = data_unit.m_vna_data.adc_Hz / data_unit.m_vna_data.audio_samples_count;

		int divider = (clk / Hz) - 1;
		if (divider < 0) divider = 0;
		else
		if (divider > 511) divider = 511;

		Hz = clk / (divider + 1);

		s.printf(L"%lld", Hz);
		PointBandwidthHzComboBox->Text = s;
	}

	data_unit.m_bandwidth_Hz = Hz;

	return true;
}

bool __fastcall TForm1::processNumberOfPointsComboBox()
{
//	if (data_unit.m_vna_data.max_points <= 0)
//		return;

	const int max_points_per_segment = (data_unit.m_vna_data.max_points > 0) ? data_unit.m_vna_data.max_points : 101;

	int points = 101;
	int points_per_segment = points;
	int segments = 1;

	if (!TryStrToInt(NumberOfPointsComboBox->Text, points))
	{
		common.setWarning(NumberOfPointsComboBox, "Invalid number of points");
		return false;
	}

	common.setWarning(NumberOfPointsComboBox, "");

	// compute the number of segments we need to do
	if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
	{  // VNA
		if (points <= max_points_per_segment)
		{	// we can do it one one segment
			points_per_segment = points;
			segments = 1;
		}
		else
		{	// need to scan more than one segment
			switch (points)
			{
				case 11:
					points_per_segment = 11;
					segments = 1;
					break;

				case 21:
					points_per_segment = 21;
					segments = 1;
					break;

				default:
				case 51:
					points_per_segment = 51;
					segments = 1;
					break;

				case 101:
					points_per_segment = 101;
					segments = 1;
					break;

				case 201:
					if (max_points_per_segment >= 201)
					{
						points_per_segment = 201;
						segments = 1;
					}
					else
					{
						points_per_segment = 101;
						segments = 2;
					}
					break;

				case 301:
					points_per_segment = 101;
					segments = 3;
					break;

				case 401:
					if (max_points_per_segment >= 401)
					{
						points_per_segment = 401;
						segments = 1;
					}
					else
					if (max_points_per_segment >= 201)
					{
						points_per_segment = 201;
						segments = 2;
					}
					else
					{
						points_per_segment = 101;
						segments = 4;
					}
					break;

				case 801:
					if (max_points_per_segment >= 801)
					{
						points_per_segment = 801;
						segments = 1;
					}
					else
					if (max_points_per_segment >= 401)
					{
						points_per_segment = 401;
						segments = 2;
					}
					else
					if (max_points_per_segment >= 201)
					{
						points_per_segment = 201;
						segments = 4;
					}
					else
					{
						points_per_segment = 101;
						segments = 8;
					}
					break;

				case 1601:
					if (max_points_per_segment >= 1601)
					{
						points_per_segment = 1601;
						segments = 1;
					}
					else
					if (max_points_per_segment >= 801)
					{
						points_per_segment = 801;
						segments = 2;
					}
					else
					if (max_points_per_segment >= 401)
					{
						points_per_segment = 401;
						segments = 4;
					}
					else
					if (max_points_per_segment >= 201)
					{
						points_per_segment = 201;
						segments = 8;
					}
					else
					{
						points_per_segment = 101;
						segments = 16;
					}
					break;

				case 3201:
					if (max_points_per_segment >= 3201)
					{
						points_per_segment = 3201;
						segments = 1;
					}
					else
					if (max_points_per_segment >= 1601)
					{
						points_per_segment = 1601;
						segments = 2;
					}
					else
					if (max_points_per_segment >= 801)
					{
						points_per_segment = 801;
						segments = 4;
					}
					else
					if (max_points_per_segment >= 401)
					{
						points_per_segment = 401;
						segments = 8;
					}
					else
					if (max_points_per_segment >= 201)
					{
						points_per_segment = 201;
						segments = 16;
					}
					else
					{
						points_per_segment = 101;
						segments = 32;
					}
					break;

				case 6401:
					if (max_points_per_segment >= 6401)
					{
						points_per_segment = 6401;
						segments = 1;
					}
					else
					if (max_points_per_segment >= 3201)
					{
						points_per_segment = 3201;
						segments = 2;
					}
					else
					if (max_points_per_segment >= 1601)
					{
						points_per_segment = 1601;
						segments = 4;
					}
					else
					if (max_points_per_segment >= 801)
					{
						points_per_segment = 801;
						segments = 8;
					}
					else
					if (max_points_per_segment >= 401)
					{
						points_per_segment = 401;
						segments = 16;
					}
					else
					if (max_points_per_segment >= 201)
					{
						points_per_segment = 201;
						segments = 32;
					}
					else
					{
						points_per_segment = 101;
						segments = 64;
					}
					break;

				case 12801:
					if (max_points_per_segment >= 12801)
					{
						points_per_segment = 12801;
						segments = 1;
					}
					else
					if (max_points_per_segment >= 6401)
					{
						points_per_segment = 6401;
						segments = 2;
					}
					else
					if (max_points_per_segment >= 3201)
					{
						points_per_segment = 3201;
						segments = 4;
					}
					else
					if (max_points_per_segment >= 1601)
					{
						points_per_segment = 1601;
						segments = 8;
					}
					else
					if (max_points_per_segment >= 801)
					{
						points_per_segment = 801;
						segments = 16;
					}
					else
					if (max_points_per_segment >= 401)
					{
						points_per_segment = 401;
						segments = 32;
					}
					else
					if (max_points_per_segment >= 201)
					{
						points_per_segment = 201;
						segments = 64;
					}
					else
					{
						points_per_segment = 101;
						segments = 128;
					}
					break;

				case 25601:
					if (max_points_per_segment >= 25601)
					{
						points_per_segment = 25601;
						segments = 1;
					}
					else
					if (max_points_per_segment >= 12801)
					{
						points_per_segment = 12801;
						segments = 2;
					}
					else
					if (max_points_per_segment >= 6401)
					{
						points_per_segment = 6401;
						segments = 4;
					}
					else
					if (max_points_per_segment >= 3201)
					{
						points_per_segment = 3201;
						segments = 8;
					}
					else
					if (max_points_per_segment >= 1601)
					{
						points_per_segment = 1601;
						segments = 16;
					}
					else
					if (max_points_per_segment >= 801)
					{
						points_per_segment = 801;
						segments = 32;
					}
					else
					if (max_points_per_segment >= 401)
					{
						points_per_segment = 401;
						segments = 64;
					}
					else
					if (max_points_per_segment >= 201)
					{
						points_per_segment = 201;
						segments = 128;
					}
					else
					{
						points_per_segment = 101;
						segments = 256;
					}
					break;
			}
		}
	}
	else
	{  // TinySA
		points_per_segment = points;
	}

	if (data_unit.m_points == points &&
		 data_unit.m_points_per_segment == points_per_segment &&
		 data_unit.m_segments == segments)
		return true;  // no change

	settings.numOfPoints = points;

	if (connected())
		stop();

	data_unit.m_points = points;
	data_unit.m_points_per_segment = points_per_segment;
	data_unit.m_segments = segments;
	data_unit.m_segment = 0;

	return true;
}

void __fastcall TForm1::setStartStopHz(int64_t low_Hz, int64_t high_Hz, bool start_scan)
{
	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	if (low_Hz < min_Hz) low_Hz = min_Hz;
	else
	if (low_Hz > max_Hz) low_Hz = max_Hz;

	if (high_Hz < min_Hz) high_Hz = min_Hz;
	else
	if (high_Hz > max_Hz) high_Hz = max_Hz;

	if (low_Hz > high_Hz)
	{	// swap
		const int64_t Hz = low_Hz;
		low_Hz = high_Hz;
		high_Hz = Hz;
	}

	if (low_Hz == high_Hz)
		return;

	if (low_Hz == data_unit.m_freq_start_Hz && high_Hz == data_unit.m_freq_stop_Hz)
		return;	// no change

	StartMHzEdit->Text = common.freqToStrMHz(low_Hz);
	StartMHzEdit->Update();

	StopMHzEdit->Text = common.freqToStrMHz(high_Hz);
	StopMHzEdit->Update();

	processStartMHzEdit(true, true);
	processStopMHzEdit(true, true);

	if (start_scan)
		::PostMessage(this->Handle, WM_CAPTURE_SCAN, 0, 0);
}

void __fastcall TForm1::StartMHzEditKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	if (Key != VK_RETURN)
		return;
	Key = 0;

	processStartMHzEdit(true, true);
}

void __fastcall TForm1::StopMHzEditKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	if (Key != VK_RETURN)
		return;
	Key = 0;

	processStopMHzEdit(true, true);
}

void __fastcall TForm1::CenterMHzEditKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	if (Key != VK_RETURN)
		return;
	Key = 0;

	processCenterMHzEdit(true, true);
}

void __fastcall TForm1::SpanMHzEditKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	if (Key != VK_RETURN)
		return;
	Key = 0;

	processSpanMHzEdit(true, true);
}

void __fastcall TForm1::CWMHzEditKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	if (Key == VK_RETURN)
	{
		Key = 0;
		processCWMHzEdit(true, true);

		if (!CWModeSpeedButton->Down)
		{
			const TNotifyEvent ne = CWModeSpeedButton->OnClick;
			CWModeSpeedButton->OnClick = NULL;
			CWModeSpeedButton->Down = true;
			CWModeSpeedButton->OnClick = ne;

			CWModeSpeedButtonClick(CWModeSpeedButton);
		}
		else
		{
			cwPopupWindowupdatenTrackBar(CWMHzEdit);
		}
	}
}

void __fastcall TForm1::S21OffsetEditKeyDown(TObject *Sender,
		WORD &Key, TShiftState Shift)
{
	if (Key == VK_RETURN)
	{
		Key = 0;
		processS21OffsetEdit();
		updateS21OffsetEdit();
	}
}

void __fastcall TForm1::S21OffsetEditChange(TObject *Sender)
{
	processS21OffsetEdit();
}

void __fastcall TForm1::GraphPaintBoxMouseDown(TObject *Sender,
		TMouseButton Button, TShiftState Shift, int X, int Y)
{
	#ifdef USE_OPENGL
		return;
	#endif

	TPaintBox *sender = dynamic_cast<TPaintBox *>(Sender);
	if (sender == NULL)
		return;

	graphs.m_mouse.down_x = X;
	graphs.m_mouse.down_y = Y;

	graphs.m_mouse.x = X;
	graphs.m_mouse.y = Y;

	graphs.m_mouse.down_graph = graphs.m_mouse.graph;

	graphs.m_mouse.down_marker = -1;

	m_popup_menu_graph_type_graph = -1;

	if (graphs.m_mouse.graph_v_scale >= 0)
	{	// start dragging the v-scale
		const int graph      = graphs.m_mouse.graph_v_scale;
		const int graph_type = settings.graphType[graph];

		if (Shift.Contains(ssLeft))
		{
			graphs.m_mouse.graph_scale_pos_drag = graphs.m_mouse.graph_scale_pos;
			graphs.m_mouse.graph_v_scale_drag   = graph;

			if (graphs.isFrequencyGraph(graph_type) || graphs.isTDRGraph(graph_type))
			{
				if (graphs.m_mouse.down_y >= 0)
				{
					t_graph_setting *gs = &settings.m_graph_setting[graph_type];

					bool center_gamma = false;
					switch (graph_type)
					{
						case GRAPH_TYPE_LINMAG_S11:
						case GRAPH_TYPE_LINMAG_S21:
						case GRAPH_TYPE_LINMAG_S11S21:
						case GRAPH_TYPE_IMPEDANCE_S11:
						case GRAPH_TYPE_QUALITY_FACTOR_S11:
						case GRAPH_TYPE_VSWR_S11:
						case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
						case GRAPH_TYPE_TDR_LIN_BP_S11:
						case GRAPH_TYPE_TDR_IMPEDANCE_S11:
							center_gamma = true;
							break;
						default:
							break;
					}

					// disable auto mode - maybe
					bool disable_auto_mode = true;
					if (center_gamma && graphs.m_mouse.graph_scale_pos == 1)
						disable_auto_mode = false;
					if (disable_auto_mode)
					{
						if (gs->auto_max && graphs.m_mouse.graph_scale_pos_drag <= 1)
						{
							if (graphs.m_max[graph].index >= 0 && graphs.m_max[graph].value != graphs.m_min[graph].value)
								gs->max = graphs.m_max[graph].value;
							gs->auto_max = false;
						}
						if (gs->auto_min && graphs.m_mouse.graph_scale_pos_drag >= 1)
						{
							if (graphs.m_min[graph].index >= 0 && graphs.m_max[graph].value != graphs.m_min[graph].value)
								gs->min = graphs.m_min[graph].value;
							gs->auto_min = false;
						}
					}

					graphs.m_mouse.down_max = gs->max;
					graphs.m_mouse.down_min = gs->min;
					graphs.m_mouse.down_val = gs->gamma;
				}

				TCursor cursor = crHandPoint;
				if (sender->Cursor != cursor)
				{
					sender->Cursor = cursor;
					// force Windows to change the cursor
					sender->Parent->Perform(WM_SETCURSOR, (unsigned int)sender->Parent->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
				}
			}
		}
	}
	else
	if (graphs.m_mouse.graph_h_scale >= 0)
	{	// start dragging the h-scale
		const int graph      = graphs.m_mouse.graph_h_scale;
		const int graph_type = settings.graphType[graph];

		if (Shift.Contains(ssLeft))
		{
			graphs.m_mouse.graph_scale_pos_drag = graphs.m_mouse.graph_scale_pos;
			graphs.m_mouse.graph_h_scale_drag   = graph;

			if (graphs.isFrequencyGraph(graph_type))
			{
				if (graphs.m_mouse.down_x >= 0)
				{
					graphs.m_mouse.down_Hz  = data_unit.m_freq_center_Hz;
					graphs.m_mouse.down_max = data_unit.m_freq_stop_Hz;
					graphs.m_mouse.down_min = data_unit.m_freq_start_Hz;
					graphs.m_mouse.down_val = data_unit.m_freq_center_Hz;
				}
			}
			else
			if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
			{
			}
			else
			if (graphs.isTDRGraph(graph_type))
			{
			}

			TCursor cursor = crHandPoint;
			if (sender->Cursor != cursor)
			{
				sender->Cursor = cursor;
				// force Windows to change the cursor
				sender->Parent->Perform(WM_SETCURSOR, (unsigned int)sender->Parent->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
			}
		}
	}
	else
	if (graphs.m_mouse.graph_type_select >= 0)
	{	// selecting a graph type
		const TPoint point = sender->ClientToScreen(TPoint(X, Y));
		GraphTypePopupMenu->Popup(point.X, point.Y);
	}
	else
	if (graphs.m_mouse.graph >= 0)
	{
		graphs.m_mouse.graph_scale_pos_drag = -1;
		graphs.m_mouse.graph_v_scale_drag   = -1;
		graphs.m_mouse.graph_h_scale_drag   = -1;
		graphs.m_mouse.graph_drag           = -1;

		const int graph_type = settings.graphType[graphs.m_mouse.graph];

		graphs.m_mouse.down_marker = settings.m_graph_setting[graph_type].show_markers ? graphs.m_mouse.marker_index : -1;

		if (graphs.m_mouse.down_marker >= 0)
		{	// the user has clicked on a marker

			if (Shift.Contains(ssLeft))
			{	// toggle it's selected state it if left mouse button clicked
				const int i = MarkerListBox->Items->IndexOfObject((TObject *)graphs.m_mouse.down_marker);
				if (i >= 0)
				{
//					for (int k = 0; k < MarkerListBox->Items->Count; k++)
//						MarkerListBox->Selected[k] = (i == k) ? true : false;

					MarkerListBox->Selected[i] = !MarkerListBox->Selected[i];

					updateInfoPanel();

					// move focus to the marker list box - allows the user to use the left/right arrow keys to step the marker through the sweep points
					if (MarkerListBox->CanFocus())
						MarkerListBox->SetFocus();
				}
			}

			TCursor cursor = crHandPoint;
			if (sender->Cursor != cursor)
			{
				sender->Cursor = cursor;
				// force Windows to change the cursor
				sender->Parent->Perform(WM_SETCURSOR, (unsigned int)sender->Parent->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
			}

		}
		else
		{	// user has not clicked on a marker .. deselect all markers
			for (int k = 0; k < MarkerListBox->Items->Count; k++)
				MarkerListBox->Selected[k] = false;
			updateInfoPanel();

			if (Shift.Contains(ssLeft))
			{
				graphs.m_mouse.graph_drag = graphs.m_mouse.graph;

				TCursor cursor = crDrag;
//				if (sender->Cursor != cursor)
				{
					sender->Cursor = cursor;
					// force Windows to change the cursor
					sender->Parent->Perform(WM_SETCURSOR, (unsigned int)sender->Parent->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
				}
			}
		}

		if (graphs.isFrequencyGraph(graph_type))
		{
			if (graphs.m_mouse.down_x >= 0)
			{
				graphs.m_mouse.down_Hz = data_unit.m_freq_center_Hz;

				//Label33->Caption = common.freqToStrMHz(m_graph_mouse_down_Hz);
			}
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
			if (graphs.m_mouse.down_marker >= 0)
				graphs.m_mouse.down_Hz = settings.m_markers_freq[graphs.m_mouse.down_marker].Hz;
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}
	}

	sender->Invalidate();
}

void __fastcall TForm1::GraphPaintBoxMouseUp(TObject *Sender,
		TMouseButton Button, TShiftState Shift, int X, int Y)
{
	#ifdef USE_OPENGL
		return;
	#endif

	TPaintBox *sender = dynamic_cast<TPaintBox *>(Sender);
	if (sender == NULL)
		return;

	// compute which graph the mouse cursor is over
	int graph_up = -1;
	for (int graph = 0; graph < graphs.numberOfGraphs(); graph++)
	{
		const int gx = graphs.m_graph_pos[graph].x;
		const int gy = graphs.m_graph_pos[graph].y;
		const int gw = graphs.m_graph_pos[graph].w;
		const int gh = graphs.m_graph_pos[graph].h;

		const int mx = X;
		const int my = Y;

		if (gw <= 0 || gh <= 0)
			continue;

		if (mx >= gx && mx <= (gx + gw) && my >= gy && my <= (gy + gh))
		{	// graph type
			graph_up = graph;
			break;
		}
	}

	if (graphs.m_mouse.down_marker >= 0)
	{	// dragging a marker
		const int graph_type = settings.graphType[graphs.m_mouse.down_graph];

		const int mi = graphs.m_mouse.down_marker;

		{	// delete the marker if it's been dragged off the graph
			const t_marker_freq marker = settings.m_markers_freq[mi];
			if (marker.Hz < data_unit.m_freq_start_Hz || marker.Hz > data_unit.m_freq_stop_Hz)
			{
				settings.m_markers_freq.erase(settings.m_markers_freq.begin() + mi);

				// deselect all markers
				for (int k = 0; k < MarkerListBox->Items->Count; k++)
					MarkerListBox->Selected[k] = false;
			}
		}

		buildMarkerListBox();
		DeleteFrequencyMarkersBitBtn->Enabled = !settings.m_markers_freq.empty();

		if (graphs.isFrequencyGraph(graph_type))
		{
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}

	}

	if (graphs.m_mouse.graph_drag >= 0)
	{
		if (graph_up >= 0 && graphs.m_mouse.graph_drag >= 0 && graph_up != graphs.m_mouse.graph_drag)
		{	// dragged the graph to a new position .. swap the graphs over
			const int gt = settings.graphType[graphs.m_mouse.graph_drag];
			settings.graphType[graphs.m_mouse.graph_drag] = settings.graphType[graph_up];
			settings.graphType[graph_up] = gt;
		}

		graphs.m_mouse.graph_drag = -1;
	}

	graphs.m_mouse.graph_scale_pos_drag = -1;
	graphs.m_mouse.graph_v_scale_drag   = -1;
	graphs.m_mouse.graph_h_scale_drag   = -1;

	graphs.m_mouse.down_graph  = -1;
	graphs.m_mouse.down_x      = -1;
	graphs.m_mouse.down_y      = -1;
	graphs.m_mouse.down_marker = -1;

	if (sender->Cursor != crArrow)
	{
		sender->Cursor = crArrow;
		// force Windows to change the cursor
		sender->Parent->Perform(WM_SETCURSOR, (unsigned int)sender->Parent->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
	}

	sender->Invalidate();
}

void __fastcall TForm1::GraphPaintBoxMouseMove(TObject *Sender,
		TShiftState Shift, int X, int Y)
{
	#ifdef USE_OPENGL
		return;
	#endif

	TPaintBox *sender = dynamic_cast<TPaintBox *>(Sender);
	if (sender == NULL)
		return;

	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	graphs.m_mouse.x = X;
	graphs.m_mouse.y = Y;

	TCursor cursor = crArrow;

	graphs.m_mouse.graph = -1;

	if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
	{
		graphs.m_mouse.graph_v_scale = -1;
		graphs.m_mouse.graph_h_scale = -1;
	}

	graphs.m_mouse.graph_type_select = -1;

	graphs.m_mouse.Hz           = -1;
	graphs.m_mouse.point_mem    = -1;
	graphs.m_mouse.point_index  = -1;
	graphs.m_mouse.time_mem     = -1;
	graphs.m_mouse.time_index   = -1;
	graphs.m_mouse.marker_mem   = -1;
	graphs.m_mouse.marker_index = -1;
	graphs.m_mouse.cpx          = 0;
	graphs.m_mouse.mag          = -1;

	const int smith_pixels = (m_screen_width > 0) ? IROUND(m_screen_width * 0.02f) : 20;
	const int pixels       = (m_screen_width > 0) ? IROUND(m_screen_width * 0.01f) : 10;

	// compute which graph the mouse cursor is over
	for (int graph = 0; graph < graphs.numberOfGraphs(); graph++)
	{
		const int gt = settings.graphType[graph];
		const int gx = graphs.m_graph_pos[graph].gx;
		const int gy = graphs.m_graph_pos[graph].gy;
		const int gw = graphs.m_graph_pos[graph].gw;
		const int gh = graphs.m_graph_pos[graph].gh;
		const int cx = graphs.m_graph_pos[graph].cx;
		const int cy = graphs.m_graph_pos[graph].cy;
		const int gr = graphs.m_graph_pos[graph].cr;

		const int mx = graphs.m_mouse.x;
		const int my = graphs.m_mouse.y;

		if (gw <= 0 || gh <= 0 || gr <= 0)
			continue;

		if (mx >= (gx - 50) && mx <= (gx + 50) && my >= (gy - 30) && my <= gy)
		{	// graph type
			graphs.m_mouse.graph_type_select = graph;
			break;
		}

		if (graphs.isFrequencyGraph(gt) || graphs.isTDRGraph(gt))
		{
			if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
			{
				if (mx >= (gx - 50) && mx <= (gx - 5) && my >= gy && my <= (gy + gh))
				{	// graph v-scale
					graphs.m_mouse.graph_v_scale   = graph;
					graphs.m_mouse.graph_scale_pos = ((my - gy) * 3) / gh;	// 0 to 2 (top/mid/bot)
					break;
				}

				if (mx >= gx && mx <= (gx + gw) && my >= (gy + gh + 5) && my <= (gy + gh + 30))
				{	// graph h-scale
					graphs.m_mouse.graph_h_scale   = graph;
					graphs.m_mouse.graph_scale_pos = ((mx - gx) * 3) / gw;	// 0 to 2 (left/mid/right)
					break;
				}
			}

			if (mx >= gx && mx <= (gx + gw) && my >= gy && my <= (gy + gh))
			{
				graphs.m_mouse.graph = graph;
				break;
			}
		}
		else
		if (graphs.isSmithGraph(gt) || graphs.isAdmittanceGraph(gt) || graphs.isPolarGraph(gt))
		{
			const int dx = mx - cx;
			const int dy = cy - my;
			const int dc = IROUND(sqrtf((dx * dx) + (dy * dy)));
			if (dc <= (gr + smith_pixels))   // within the circle and a bit ?
			{	// yes
				graphs.m_mouse.graph = graph;
				break;
			}
		}
	}

//	Label5->Caption = graphs.m_mouse.graph;
//	Label5->Update();

	// ************************************
	// compute the mouse position details - sweep point index, frequency and marker index

	if (graphs.m_mouse.graph >= 0 && graphs.m_mouse.graph_drag < 0)
	{
		const int graph      = graphs.m_mouse.graph;
		const int graph_type = settings.graphType[graph];

		const int gx = graphs.m_graph_pos[graph].gx;
		const int gy = graphs.m_graph_pos[graph].gy;
		const int gw = graphs.m_graph_pos[graph].gw;
		const int gh = graphs.m_graph_pos[graph].gh;
		const int cx = graphs.m_graph_pos[graph].cx;
		const int cy = graphs.m_graph_pos[graph].cy;
		const int gr = graphs.m_graph_pos[graph].cr;

		const int mx = graphs.m_mouse.x;
		const int my = graphs.m_mouse.y;

		const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

		int     m_graph   = -1;
		int     m_mem     = -1;
		int     m_channel = -1;
		int     m_index   = -1;
		int64_t m_Hz      = -1;
		double  m_secs    = -1;

		if (graphs.isFrequencyGraph(graph_type))
		{
			if (my >= gy && my <= (gy + gh))
			{
				const bool near_point = settings.snapToNearestPoint ? graphs.nearestPoint(graph, graph_type, mx, my, pixels, m_graph, m_mem, m_channel, m_index, m_Hz, m_secs) : false;

				if (!near_point)
				{	// use the mouse X position and the first valid memory
					m_Hz = graphs.xyToFreq(graph, graph_type, mx, my);

					int mem   = -1;
					int index = -1;

					if (graph_type == GRAPH_TYPE_CAL_LOGMAG)
					{	// calibration graph
						mem = -2;
						index = data_unit.indexFreq(m_Hz, mem);
					}
					else
					{	// memories
						while (mem < MAX_MEMORIES && index < 0)
						{
							mem = data_unit.firstUsedMem(true, ++mem);
							if (mem < 0)
								break;
							index = data_unit.indexFreq(m_Hz, mem);
						}

						if (mem >= 0 && mem < MAX_MEMORIES && index >= 0 && index < (int)data_unit.m_point_filt[mem].size())
						{
							m_index = index;
							m_Hz    = data_unit.m_point_filt[mem][m_index].Hz;	// snap to the sweep point frequency
						}
					}
				}

				graphs.m_mouse.Hz          = m_Hz;
				graphs.m_mouse.point_mem   = m_mem;
				graphs.m_mouse.point_index = m_index;

				if (graph_type != GRAPH_TYPE_CAL_LOGMAG)
				{	// memories

					if (gs && gs->show_markers)
					{
						if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
						{	// find the nearest marker
							int m_mem   = -1;
							int m_index = -1;
							int m_dist  = -1;
							for (int i = 0; i < (int)settings.m_markers_freq.size(); i++)
							{
								const t_marker_freq marker = settings.m_markers_freq[i];
								for (int m = 0; m < MAX_MEMORIES; m++)
								{
									if (settings.memoryEnable[m])
									{
										const int mi     = data_unit.indexFreq(marker.Hz, m);	// snap the nearest sweep point
										const int64_t Hz = data_unit.getFrequency(m, mi);
										const int kx     = graphs.freqToX(graph, graph_type, Hz);
										const int dx     = ABS(kx - mx);
										if (m_index < 0 || m_dist > dx)
										{	// found a closer marker
											m_mem   = m;
											m_index = i;
											m_dist  = dx;
										}
									}
								}
							}
							if (m_mem >= 0 && m_index >= 0 && m_dist >= 0)
							{
								if (m_dist <= pixels)	// within 'pixels' of the nearest marker ?
								{
									graphs.m_mouse.marker_mem   = m_mem;
									graphs.m_mouse.marker_index = m_index;
								}
							}
						}
					}

				}
			}

		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
			const int dx = mx - cx;
			const int dy = cy - my;
			const int cd = IROUND(sqrtf((dx * dx) + (dy * dy)));	// distance (in pixels) from center of chart

			if (gr > 0)
			{
				if (cd <= gr)
				{
					complexf cpx = complexf((float)dx / gr, (float)dy / gr);	// -1 to +1
					cpx *= gs->max;
					graphs.m_mouse.cpx = cpx;
					graphs.m_mouse.mag = sqrtf(SQR(cpx.real()) + SQR(cpx.imag()));
				}

				if (cd <= (gr + pixels))
				{
					const bool near_point = settings.snapToNearestPoint ? graphs.nearestPoint(graph, graph_type, mx, my, pixels, m_graph, m_mem, m_channel, m_index, m_Hz, m_secs) : false;

					if (near_point)
					{
						graphs.m_mouse.point_mem   = m_mem;
						graphs.m_mouse.point_index = m_index;
						graphs.m_mouse.Hz          = m_Hz;
					}

					if (gs && gs->show_markers)
					{
						if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
						{	// find the nearest marker
							const int g = graph;
							const int c = 0;
							int m_mem   = -1;
							int m_index = -1;
							int m_dist  = -1;
							for (int i = 0; i < (int)settings.m_markers_freq.size(); i++)
							{
								const t_marker_freq marker = settings.m_markers_freq[i];
								for (int m = 0; m < MAX_MEMORIES; m++)
								{
									if (settings.memoryEnable[m])
									{
										const int p_index = data_unit.indexFreq(marker.Hz, m);
										if (p_index >= 0 && p_index < (int)graphs.m_line_points[g][m][c].size())
										{
											const int dx = graphs.m_line_points[g][m][c][p_index].x - mx;
											const int dy = graphs.m_line_points[g][m][c][p_index].y - my;
											const int dp = (dx * dx) + (dy * dy);
											if (m_index < 0 || m_dist > dp)
											{	// found a closer line point
												//m_graph = g;
												m_mem   = m;
												//m_chan  = c;
												m_index = i;
												m_dist  = dp;
											}
										}
									}
								}
							}
							if (m_mem >= 0 && m_index >= 0 && m_dist >= 0)
							{
								m_dist = IROUND(sqrtf(m_dist));	// now in pixels
								if (m_dist <= pixels)	// within 'pixels' of the nearest marker ?
								{
									graphs.m_mouse.marker_mem   = m_mem;
									graphs.m_mouse.marker_index = m_index;
								}
							}
						}
					}
				}
			}

		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
			int mem = data_unit.firstUsedMem(true, 0);
			if (mem < 0)
				mem = 0;

			const float mf = (float)(mx - gx) / gw;	// 0.0 to 1.0

			const double freq_step = data_unit.freq_step(-1);

			if (mf >= 0.0f && mf <= 1.0f && freq_step > 0)
			{
				const double max_time = data_unit.max_time(freq_step);
				const double mt = max_time * mf;

//				const double mem_freq_step = data_unit.freq_step(mem);
//				const double mem_max_time  = data_unit.max_time(mem_freq_step);

				const int size = graphs.m_fft[graph][mem].size() / 2;

				int x = (size > 1) ? IROUND((size * mt) / max_time) : -1;

				const bool near_point = settings.snapToNearestPoint ? graphs.nearestPoint(graph, graph_type, mx, my, pixels, m_graph, m_mem, m_channel, m_index, m_Hz, m_secs) : false;

				if (!near_point)
				{	// use the mouse X position and the first valid memory
//					if (mem < MAX_MEMORIES && size > 0 && x >= 0 && x < size)
					if (mem < MAX_MEMORIES)
					{
						m_graph   = graph;
						m_index   = x;
						m_secs    = max_time * mf;
					}
				}

				graphs.m_mouse.point_mem   = m_mem;
				graphs.m_mouse.point_index = m_index;
				graphs.m_mouse.Hz          = 0;
				graphs.m_mouse.secs        = m_secs;
			}
		}
	}

	// ************************************
	// dragging a graph

	if (graphs.m_mouse.graph_drag >= 0)
	{
		cursor = crDrag;

	}

	// ************************************
	// dragging a marker

	if (graphs.m_mouse.down_marker >= 0)
	{
		cursor = crHandPoint;

		const int graph      = graphs.m_mouse.down_graph;
		const int graph_type = settings.graphType[graph];

		if (graphs.isFrequencyGraph(graph_type))
		{
			settings.m_markers_freq[graphs.m_mouse.down_marker].Hz = graphs.m_mouse.Hz;
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
			settings.m_markers_freq[graphs.m_mouse.down_marker].Hz = graphs.m_mouse.Hz;
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}
	}

	// ************************************
	// dragging v-scale

	if (graphs.m_mouse.graph_v_scale_drag >= 0)
	{
		cursor = crHandPoint;

		const int pos        = graphs.m_mouse.graph_scale_pos_drag;
		const int graph      = graphs.m_mouse.graph_v_scale_drag;
		const int graph_type = settings.graphType[graph];

		if (graphs.isFrequencyGraph(graph_type) || graphs.isTDRGraph(graph_type))
		{
			t_graph_setting *gs = &settings.m_graph_setting[graph_type];

			double range = graphs.m_mouse.down_max - graphs.m_mouse.down_min;
			range *= 2.0;
			if (range < 1e-14)
				range = 1e-14;

			const double delta = ((double)(graphs.m_mouse.y - graphs.m_mouse.down_y) * range) / graphs.m_graph_pos[graph].gh;

			switch (graph_type)
			{
				default:
					switch (pos)
					{
						case 0:	// dragging top section
							gs->max = graphs.m_mouse.down_max + delta;
							break;
						case 1:	// dragging mid section
							gs->max = graphs.m_mouse.down_max + delta;
							gs->min = graphs.m_mouse.down_min + delta;
							break;
						case 2:	// dragging bot section
							gs->min = graphs.m_mouse.down_min + delta;
							break;
					}
					break;

				case GRAPH_TYPE_LINMAG_S11:
				case GRAPH_TYPE_LINMAG_S21:
				case GRAPH_TYPE_LINMAG_S11S21:
				case GRAPH_TYPE_IMPEDANCE_S11:
				case GRAPH_TYPE_QUALITY_FACTOR_S11:
				case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
				case GRAPH_TYPE_TDR_LIN_BP_S11:
				case GRAPH_TYPE_TDR_IMPEDANCE_S11:
					switch (pos)
					{
						case 0:	// dragging top section
							gs->max = graphs.m_mouse.down_max + delta;
							break;
						case 1:	// dragging mid section
							#if 0
								gs->max = graphs.m_mouse.down_max + delta;
								gs->min = graphs.m_mouse.down_min + delta;
							#else
								{	// modify the gamma (graph non-linear scale)
									const double diff = delta / range;
									double gamma = graphs.m_mouse.down_val - (diff * 10);
									if (gamma < GAMMA_MIN) gamma = GAMMA_MIN;
									else
									if (gamma > GAMMA_MAX) gamma = GAMMA_MAX;
									gs->gamma = gamma;
								}
							#endif
							break;
						case 2:	// dragging bot section
							gs->min = graphs.m_mouse.down_min + delta;
							break;
					}
					break;

				case GRAPH_TYPE_VSWR_S11:
					switch (pos)
					{
						case 0:	// dragging top section
							gs->max = graphs.m_mouse.down_max + delta;
							break;
						case 1:	// dragging mid section .. alter the gamma rather than the min/max
						//case 2:	// dragging bottom section
							{
								const double diff = delta / range;
								double gamma = graphs.m_mouse.down_val - (diff * 10);
								if (gamma < GAMMA_MIN) gamma = GAMMA_MIN;
								else
								if (gamma > GAMMA_MAX) gamma = GAMMA_MAX;
								gs->gamma = gamma;
							}
							break;
						case 2:	// dragging bottom section
							gs->min = graphs.m_mouse.down_min + delta;
							break;
					}
					//gs->min = VSWR_MIN;
					break;

				case GRAPH_TYPE_SMITH_S11:
				case GRAPH_TYPE_SMITH_S21:
				case GRAPH_TYPE_ADMITTANCE_S11:
				case GRAPH_TYPE_ADMITTANCE_S21:
				case GRAPH_TYPE_POLAR_S11:
				case GRAPH_TYPE_POLAR_S21:
					break;

				case GRAPH_TYPE_PHASE_VECTOR_S11:
				case GRAPH_TYPE_PHASE_VECTOR_S21:
					break;
			}

			// sanity check
			settings.clipGraphMinMax(graph_type, gs->max, gs->min);
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{

			// TODO: drag the magnitude scale in/out

		}
	}

	// ************************************
	// dragging h-scale

	if (graphs.m_mouse.graph_h_scale_drag >= 0)
	{
		cursor = crHandPoint;

		const int pos        = graphs.m_mouse.graph_scale_pos_drag;
		const int graph      = graphs.m_mouse.graph_h_scale_drag;
		const int graph_type = settings.graphType[graph];

		if (graphs.isFrequencyGraph(graph_type))
		{
			const double range_Hz = graphs.m_mouse.down_max - graphs.m_mouse.down_min;
			const double delta_Hz = ((double)(graphs.m_mouse.x - graphs.m_mouse.down_x) * range_Hz) / graphs.m_graph_pos[graph].gw;

			double start_Hz = graphs.m_mouse.down_Hz - (range_Hz / 2);
			double stop_Hz  = graphs.m_mouse.down_Hz + (range_Hz / 2);

			switch (pos)
			{
				case 0:	// dragging left section
					start_Hz -= delta_Hz;
					if (start_Hz < min_Hz)
						 start_Hz = min_Hz;
					else
					if (start_Hz > (stop_Hz - 1000))
						 start_Hz =  stop_Hz - 1000;
					break;

				case 1:	// dragging mid section
					start_Hz -= delta_Hz;
					if (start_Hz < min_Hz)
						 start_Hz = min_Hz;
					else
					if (start_Hz > (max_Hz - range_Hz))
						 start_Hz =  max_Hz - range_Hz;

					stop_Hz -= delta_Hz;
					if (stop_Hz < (min_Hz + range_Hz))
						 stop_Hz =  min_Hz + range_Hz;
					else
					if (stop_Hz > max_Hz)
						 stop_Hz = max_Hz;
					break;

				case 2:	// dragging right section
					stop_Hz -= delta_Hz;
					if (stop_Hz < (start_Hz + 1000))
						 stop_Hz =  start_Hz + 1000;
					else
					if (stop_Hz > max_Hz)
						 stop_Hz = max_Hz;
					break;
			}

			setStartStopHz(I64ROUND(start_Hz), I64ROUND(stop_Hz), false);
			//setStartStopHz(I64ROUND(start_Hz), I64ROUND(stop_Hz), true);
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}
	}

	// ************************************
	// choose the mouse cursor

	if (graphs.m_mouse.graph_drag < 0 && graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
	{
		if (graphs.m_mouse.graph_v_scale >= 0)
		{
			switch (graphs.m_mouse.graph_scale_pos)
			{
				case 0:	// dragging top section
					cursor = crHandPoint;
					//cursor = crSizeNS;
					break;
				case 1:	// dragging mid section
					cursor = crHandPoint;
					//cursor = crVSplit;
					break;
				case 2:	// dragging bot section
					cursor = crHandPoint;
					//cursor = crSizeNS;
					break;
			}
		}
		else
		if (graphs.m_mouse.graph_h_scale >= 0)
		{
			switch (graphs.m_mouse.graph_scale_pos)
			{
				case 0:	// dragging left section
					cursor = crHandPoint;
					//cursor = crSizeWE;
					break;
				case 1:	// dragging mid section
					cursor = crHandPoint;
					//cursor = crHSplit;
					break;
				case 2:	// dragging right section
					cursor = crHandPoint;
					//cursor = crSizeWE;
					break;
			}
		}
		else
		if (graphs.m_mouse.graph_type_select >= 0)
		{
			cursor = crHandPoint;
		}
		else
		if (graphs.m_mouse.graph >= 0)
		{
			const int graph      = graphs.m_mouse.graph;
			const int graph_type = settings.graphType[graph];

//			const int gx = graphs.m_graph_pos[graph].x;
//			const int gy = graphs.m_graph_pos[graph].y;
//			const int gw = graphs.m_graph_pos[graph].w;
//			const int gh = graphs.m_graph_pos[graph].h;
//			const int cx = graphs.m_graph_pos[graph].cx;
//			const int cy = graphs.m_graph_pos[graph].cy;
//			const int gr = graphs.m_graph_pos[graph].cr;

//			const int mx = graphs.m_mouse.x;
//			const int my = graphs.m_mouse.y;

			if (graphs.m_mouse.marker_index >= 0)
			{
				cursor = crHandPoint;
			}
			else
			{
				if (graphs.isFrequencyGraph(graph_type))
				{
//					if (mx >= gx && mx < (gx + gw) && my >= gy && my < (gy + gh))
//						if (graphs.m_mouse.point_index < 0)
//							cursor = crCross;
				}
				else
				if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
				{
//					const int dx = mx - cx;
//					const int dy = cy - my;
//					const int dc = IROUND(sqrt((dx * dx) + (dy * dy)));	// distance from center
//					if (dc <= gr)
//						if (graphs.m_mouse.point_index < 0)
//							cursor = crCross;
				}
				else
				if (graphs.isTDRGraph(graph_type))
				{
//					if (mx >= gx && mx < (gx + gw) && my >= gy && my < (gy + gh))
//						//if (graphs.m_mouse.time_index < 0)
//						if (graphs.m_mouse.point_index < 0)
//							cursor = crCross;
				}
			}
		}
	}

	// ************************************

	if (sender->Cursor != cursor)
	{
		sender->Cursor = cursor;
		// force Windows to change the cursor
		sender->Parent->Perform(WM_SETCURSOR, (unsigned int)sender->Parent->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
	}

	updateInfoPanel();

	sender->Refresh();
}

void __fastcall TForm1::FreqBandEnableToggleSwitchClick(TObject *Sender)
{
	settings.showFrequencyBands = (FreqBandEnableToggleSwitch->State == tssOn) ? true : false;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::GraphPaintBoxMouseLeave(TObject *Sender)
{
	#ifdef USE_OPENGL
		return;
	#endif

	TPaintBox *sender = dynamic_cast<TPaintBox *>(Sender);
	if (sender == NULL)
		return;

	sender->Hint = "";

//	if (graphs.m_mouse.down_marker >= 0)
	{
		graphs.m_mouse.marker_mem   = -1;
		graphs.m_mouse.marker_index = -1;
		graphs.m_mouse.graph        = -1;
		graphs.m_mouse.Hz           = -1;
		graphs.m_mouse.point_mem    = -1;
		graphs.m_mouse.point_index  = -1;
		graphs.m_mouse.time_mem     = -1;
		graphs.m_mouse.time_index   = -1;
		graphs.m_mouse.x            = -1;
		graphs.m_mouse.y            = -1;
		graphs.m_mouse.cpx          = 0;
		graphs.m_mouse.mag          = -1;
	}

	if (graphs.m_mouse.graph_drag < 0 && graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0)
	{
		graphs.m_mouse.graph_scale_pos = -1;
		graphs.m_mouse.graph_v_scale   = -1;
		graphs.m_mouse.graph_h_scale   = -1;
//		graphs.m_mouse.graph_drag      = -1;
	}

	sender->Invalidate();
}

void __fastcall TForm1::PointBandwidthHzComboBoxKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift)
{
	if (Key != VK_RETURN)
		return;
	Key = 0;

	processPointBandwidthHzComboBox();

	if (connected())
	{
		sendBandwidthCommand();

		m_comms.points_per_sec = 0.0f;

		if (janvna2_comms.connected)
			janvna2_comms.updateSweepSettings = true;
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::LCMatchingToggleSwitchClick(TObject *Sender)
{
	settings.lcMatchingEnable = (LCMatchingToggleSwitch->State == tssOn) ? true : false;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::ScanOnceSpeedButtonClick(TObject *Sender)
{
	if (!connected())
		return;

	if (!ScanOnceSpeedButton->Down)
	{
      stop();
		return;
	}

	if (ScanSpeedButton->Down)
	{
		const TNotifyEvent ne = ScanSpeedButton->OnClick;
		ScanSpeedButton->OnClick = NULL;
		ScanSpeedButton->Down    = false;
		ScanSpeedButton->OnClick = ne;
	}

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
		if (janvna2_comms.mode != MODE_SINGLE_SCAN && janvna2_comms.mode != MODE_SCAN)
			scan();
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		if (nanovna2_comms.mode != MODE_SINGLE_SCAN && nanovna2_comms.mode != MODE_SCAN)
			scan();
	}
	else
	{	// V1
		if (nanovna1_comms.mode != MODE_SINGLE_SCAN && nanovna1_comms.mode != MODE_SCAN)
			scan();
	}
}

void __fastcall TForm1::CurveSmoothingTrackBarChange(TObject *Sender)
{
	settings.curveSmoothingLevel = CurveSmoothingTrackBar->Position;

	String s;
	if (settings.curveSmoothingLevel <= 0)
		s = "off";
	else
		s.printf(L"%d", settings.curveSmoothingLevel);

	CurveSmoothingLabel->Caption = s;
	CurveSmoothingTrackBar->Hint = "Trace smoothing " + s;

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
		data_unit.m_point_filt[mem].resize(0);

	if (Application->MainForm)
		::SendMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::EDelayEditChange(TObject *Sender)
{
	processEDelayEdit();
}

void __fastcall TForm1::EDelayEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if (Key == VK_RETURN)
	{
		Key = 0;
		processEDelayEdit();
		updateEDelayEdit();
	}
}

void __fastcall TForm1::ShowMarkersOnGraphToggleSwitchClick(TObject *Sender)
{
	settings.showMarkersOnGraph = (ShowMarkersOnGraphToggleSwitch->State == tssOn) ? true : false;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::OutputPowerTrackBarChange(TObject *Sender)
{
	int value = OutputPowerTrackBar->Position;

	if (value < 0)
		value = -1;	// auto

	settings.outputPower = value;

	String s;
	if (settings.outputPower < 0)
		s = "auto";
	else
		s.printf(L"%d", settings.outputPower);
	OutputPowerLabel2->Caption = s;
	OutputPowerTrackBar->Hint = "Output power " + s;

	sendOutputPowerCommand();
}

void __fastcall TForm1::MemorySpeedButtonClick(TObject *Sender)
{
	TSpeedButton *sb = dynamic_cast<TSpeedButton *>(Sender);
	if (sb == NULL)
		return;

	int mem = -1;
	if (sb == MemorySpeedButton0) mem = 0;
	else
	if (sb == MemorySpeedButton1) mem = 1;
	else
	if (sb == MemorySpeedButton2) mem = 2;
	else
	if (sb == MemorySpeedButton3) mem = 3;
	else
	if (sb == MemorySpeedButton4) mem = 4;

	if (mem >= 0)
	{
		if (!data_unit.m_point_mem[mem].empty())
		{
			settings.memoryEnable[mem] = sb->Down;

			if (Application->MainForm)
				::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		}
		else
		if (sb && sb != MemorySpeedButton0)
			if (sb->Down)
			  sb->Down = false;
	}
	else
	if (sb)
		if (sb->Down)
		  sb->Down = false;
}

void __fastcall TForm1::MarkerListBoxDblClick(TObject *Sender)
{
	String fs;

	// pop-up an edit box to let the user edit the marker frequency

	TListBox *lb = dynamic_cast<TListBox *>(Sender);
	if (lb == NULL)
		return;

	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	// find the selected marker
	int mi = selectedMarker(); // currently selected marker
	if (mi < 0)
	{	// no marker selected .. add one

		if (!InputQuery("New marker", "Frequency (default MHz)", fs))
			return;
		fs = fs.Trim();
		if (fs.IsEmpty())
			return;

		double MHz = 0;
		if (!common.strToMHz(fs, MHz))
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"Invalid frequency", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		const int64_t Hz = I64ROUND(MHz * 1e6);
		if (Hz < min_Hz || Hz > max_Hz)
		{
			Application->NormalizeTopMosts();
			Application->MessageBox(L"Invalid frequency", L"Error", MB_ICONERROR | MB_OK);
			Application->RestoreTopMosts();
			return;
		}

		addMarker(Hz);
	}
	else
	{	// edit a selected marker
		enterMarkerFrequency(mi);
	}
}

void __fastcall TForm1::GraphPaintBoxMouseEnter(TObject *Sender)
{
	#ifdef USE_OPENGL
		return;
	#endif

	TPaintBox *sender = dynamic_cast<TPaintBox *>(Sender);
	if (sender == NULL)
		return;

	sender->Hint = "";

	sender->Invalidate();
}

void __fastcall TForm1::SetScanRangeToVNAScanRangeBitBtnClick(TObject *Sender)
{
	if (connected())
	{
		setStartStopHz(data_unit.m_vna_data.freq_start_Hz, data_unit.m_vna_data.freq_stop_Hz, false);

		// set the number of point to also match
		if (data_unit.m_vna_data.num_points > 0)
		{
			const int i = NumberOfPointsComboBox->Items->IndexOfObject((TObject *)data_unit.m_vna_data.num_points);
			if (i >= 0)
				NumberOfPointsComboBox->ItemIndex = i;
		}
	}
}

void __fastcall TForm1::MarkerListBoxMouseDown(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y)
{
	const int i = MarkerListBox->ItemAtPos(TPoint(X, Y), true);
	if (i < 0)
	{	// deselect all markers
		for (int k = 0; k < MarkerListBox->Items->Count; k++)
			MarkerListBox->Selected[k] = false;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		//updateInfoPanel();
	}
	else
//	if (Shift.Contains(ssRight))
//	{	// toggle the selection state of the item the user clicked on
//		MarkerListBox->Selected[i] = !MarkerListBox->Selected[i];
//
//		if (Application->MainForm)
//			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
//		//updateInfoPanel();
//	}
//	else
	if (Shift.Contains(ssLeft))
	{	// toggle the selection state of the item the user clicked on
		if (MarkerListBox->Selected[i])
		{
//			MarkerListBox->Selected[i] = false;
//			if (Application->MainForm)
//				::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
			//updateInfoPanel();
		}
	}
}

void __fastcall TForm1::Deselectmarker1Click(TObject *Sender)
{
  // delect all markers
	for (int i = 0; i < MarkerListBox->Items->Count; i++)
		MarkerListBox->Selected[i] = false;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	//updateInfoPanel();
}

void __fastcall TForm1::Deletemarker1Click(TObject *Sender)
{
	if (m_popup_menu_mouse_marker >= 0)
	{
		deleteMarker(m_popup_menu_mouse_marker);
		m_popup_menu_mouse_marker = -1;
	}
}

void __fastcall TForm1::GraphPaintBoxDblClick(TObject *Sender)
{
	#ifdef USE_OPENGL
		return;
	#endif

	if (graphs.m_mouse.marker_index >= 0)
	{	// delete the marker
		deleteMarker(graphs.m_mouse.marker_index);

		graphs.m_mouse.marker_mem   = -1;
		graphs.m_mouse.marker_index = -1;
	}
	else
	if (graphs.m_mouse.Hz > 0)
	{	// add a marker
		addMarker(graphs.m_mouse.Hz);
	}
}

void __fastcall TForm1::Addmarker1Click(TObject *Sender)
{
	addMarker(m_popup_menu_mouse_Hz);
}

void __fastcall TForm1::DeviceComboBoxChange(TObject *Sender)
{
	settings.serialPortName = DeviceComboBox->Text.Trim();

	if (!connected())
	{
		if (DeviceComboBox->ItemIndex > 0)
		{
			ConnectDisconnectSpeedButton->Down = true;
			connect();
		}
	}
}

void __fastcall TForm1::ConnectDisconnectSpeedButtonClick(TObject *Sender)
{
	if (ConnectDisconnectSpeedButton->Down)
	{
		disconnect();

		const TNotifyEvent ne = ConnectDisconnectSpeedButton->OnClick;
		ConnectDisconnectSpeedButton->OnClick = NULL;
		ConnectDisconnectSpeedButton->Down = true;
		ConnectDisconnectSpeedButton->OnClick = ne;

		connect();
	}
	else
	{
		disconnect();
	}
}

void __fastcall TForm1::SweepNameEditChange(TObject *Sender)
{
	settings.sweepName = SweepNameEdit->Text.Trim();
}

void __fastcall TForm1::FormShow(TObject *Sender)
{
	if (!m_initialised)
	{
		// move to the saved position
		this->Top    = settings.mainWindowPos.top;
		this->Left   = settings.mainWindowPos.left;
		this->Width  = settings.mainWindowPos.width;
		this->Height = settings.mainWindowPos.height;

		Timer1->Enabled = true;

		m_initialised = true;
	}
}

void __fastcall TForm1::RecordSpeedButtonClick(TObject *Sender)
{
	m_record.enabled = false;

	if (RecordSpeedButton->Down)
		if (!recordDataStart())
			RecordSpeedButton->Down = false;
}

void __fastcall TForm1::RecordSpeedButtonMouseDown(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y)
{
//   return;

	if (Shift.Contains(ssRight))
	{
		String path = settings.recordFolder;

		const bool path_exists = DirectoryExists(path);

		// if the current path doesn't currently exist the OS's directory dialog box takes well away from this directory :(
		// so create the directory to stop it doing so - even if only temporary
		if (!path_exists)
			common.createPath(AnsiString(path).c_str());

		Application->NormalizeTopMosts();
		bool ok;
		try
		{
//			ok = SelectDirectory("Select the recording folder ..", "", path, TSelectDirExtOpts() << sdNewUI << sdNewFolder << sdShowEdit << sdShowShares << sdShowFiles <<sdValidateDir, this);
			ok = SelectDirectory("Select the recording folder ..", "", path, TSelectDirExtOpts() << sdNewUI << sdNewFolder << sdShowEdit << sdShowShares << sdValidateDir, this);
		}
		catch (...)
		{
			Application->RestoreTopMosts();
			if (!path_exists)
				RemoveDir(path);
			return;
		}
		Application->RestoreTopMosts();

		if (!path_exists)
			RemoveDir(path);

		if (!ok)
			return;

		if (settings.recordFolder != path)
		{
			const bool was_recording = m_record.enabled;
			m_record.enabled = false;

			settings.recordFolder = path;

			if (SettingsForm)
				if (SettingsForm->Showing)
					SettingsForm->update();

			if (was_recording)
				recordDataStart();
		}
	}
}

void __fastcall TForm1::FreqEditChange(TObject *Sender)
{
	TEdit *edit = dynamic_cast<TEdit *>(Sender);
	if (edit == NULL)
		return;

	calibration_module.m_inter_cal.resize(0);

	if (edit == StartMHzEdit)
		processStartMHzEdit(true, false);
	else
	if (edit == StopMHzEdit)
		processStopMHzEdit(true, false);
	else
	if (edit == CenterMHzEdit)
		processCenterMHzEdit(true, false);
	else
	if (edit == SpanMHzEdit)
		processSpanMHzEdit(true, false);
	else
	if (edit == CWMHzEdit)
		processCWMHzEdit(true, false);
}

void __fastcall TForm1::NewGraphBitBtnClick(TObject *Sender)
{
	#ifdef GraphUnitH

		TGraphForm *graph_form = NULL;

		for (unsigned int i = 0; i < m_graph_form.size(); i++)
		{
			TGraphForm *gf = m_graph_form[i];
			if (gf == NULL)
				continue;
			if (gf->Showing)
				continue;
			graph_form = gf;	// reuse an old form
			break;
		}

		if (graph_form == NULL)
		{
			graph_form = new TGraphForm(this);
//			graph_form = new TGraphForm(Application, Panel2);
			if (graph_form != NULL)
				m_graph_form.push_back(graph_form);
		}

		if (graph_form)
		{
			//if (graph_form->Parent == Panel2)
			if (graph_form->Parent == Panel1)
				arrangeGraphs();
			graph_form->Show();
		}

	#endif
}

void __fastcall TForm1::arrangeGraphs()
{
	int num_graphs = 0;

	for (unsigned int i = 0; i < m_graph_form.size(); i++)
	{
		TGraphForm *gf = m_graph_form[i];
		if (gf != NULL)
			//if (gf->Parent == Panel2)
			if (gf->Parent == Panel1)
				num_graphs++;
	}

	for (unsigned int i = 0; i < m_graph_form.size(); i++)
	{
		TGraphForm *gf = m_graph_form[i];
		if (gf != NULL)
		{
			//TPanel *panel = Panel2;
			TPanel *panel = Panel1;
			if (gf->Parent == panel)
			{
				gf->Top    = panel->ClientRect.Top + ((panel->ClientHeight * i) / num_graphs);
				gf->Left   = panel->ClientRect.Left;
				gf->Width  = panel->ClientWidth;
				gf->Height = panel->ClientHeight / num_graphs;
			}
		}
	}
}


void __fastcall TForm1::Panel2Resize(TObject *Sender)
{
	arrangeGraphs();
}

void __fastcall TForm1::CloseBitBtnClick(TObject *Sender)
{
	Close();
}

void __fastcall TForm1::ScanSpeedButtonClick(TObject *Sender)
{
	if (ScanOnceSpeedButton->Down)
	{
		const TNotifyEvent ne = ScanOnceSpeedButton->OnClick;
		ScanOnceSpeedButton->OnClick = NULL;
		ScanOnceSpeedButton->Down    = false;
		ScanOnceSpeedButton->OnClick = ne;
	}

	if (ScanSpeedButton->Down)
		scan();
	else
		stop();
}

void __fastcall TForm1::UploadFirmwareBitBtnClick(TObject *Sender)
{
	if (connected())
	{
		if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
		{	// the connected unit is a NanoVNA V2
			if (!nanovna2_comms.inDFUMode())
			{
				Application->NormalizeTopMosts();
				Application->MessageBox(	L"Your NanoVNA V2 needs to be in firmware upload mode ..\n\n"
													"Disconnect from the VNA (disconnect button top/left main window).\n\n"
													"Turn the VNA off.\n\n"
													"Ensure the VNA is plugged into your PC using the USB cable.\n\n"
													"Press and hold down the left user button on the VNA whilst turning the VNA back on.\n\n"
													"The VNA's screen should then be all white/blank (firmware upload mode).\n\n"
													"Reconnect to the VNA (connect button top/left main window).",
													L"Info",
													MB_ICONINFORMATION | MB_OK);
				Application->RestoreTopMosts();
				return;
			}
		}
	}

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{
	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
	{	// V2
		if (UploadFirmwareV2Form)
		{
			if (!UploadFirmwareV2Form->Showing)
				UploadFirmwareV2Form->show();
			else
				UploadFirmwareV2Form->Hide();
		}
	}
	else
	{	// V1
		if (UploadFirmwareForm)
		{
			if (!UploadFirmwareForm->Showing)
				UploadFirmwareForm->show();
			else
				UploadFirmwareForm->Hide();
		}
	}
}

void __fastcall TForm1::SettingsBitBtnClick(TObject *Sender)
{
	if (SettingsForm)
	{
		if (!SettingsForm->Showing)
			SettingsForm->show();
		else
			SettingsForm->Hide();
	}
}

void __fastcall TForm1::BatteryVoltageBitBtnClick(TObject *Sender)
{
	if (BatteryVoltageForm)
	{
		if (!BatteryVoltageForm->Showing)
			BatteryVoltageForm->show();
		else
			BatteryVoltageForm->Hide();
	}
}

void __fastcall TForm1::CaptureVNAScreenBitBtnClick(TObject *Sender)
{
	if (VNAScreenCaptureForm)
	{
		if (VNAScreenCaptureForm->Showing)
			VNAScreenCaptureForm->Hide();
		else
			getVNAScreenCapture();
	}
}

void __fastcall TForm1::VNACommsBitBtnClick(TObject *Sender)
{
	if (CommsForm)
	{
		if (!CommsForm->Showing)
			CommsForm->show();
		else
			CommsForm->Hide();
	}
}

void __fastcall TForm1::VNAUsartCommsBitBtnClick(TObject *Sender)
{
	if (VNAUsartCommsForm)
	{
		if (!VNAUsartCommsForm->Showing)
			VNAUsartCommsForm->show();
		else
			VNAUsartCommsForm->Hide();
	}
}

void __fastcall TForm1::CalibrationBitBtnClick(TObject *Sender)
{
	if (CalibrationForm)
	{
		if (!CalibrationForm->Showing)
			CalibrationForm->show();
		else
			CalibrationForm->Hide();
	}
}

void __fastcall TForm1::Setasstartfrequency1Click(TObject *Sender)
{
//	int64_t rounding_Hz = data_unit.m_freq_span_Hz / 100;
//	rounding_Hz = I64ROUND((double)rounding_Hz / 500) * 500;	// 500Hz rounding
	const int64_t rounding_Hz = 500;	// 500Hz rounding

	const int64_t start_Hz = I64ROUND((double)m_popup_menu_mouse_Hz / rounding_Hz) * rounding_Hz;

	setStartStopHz(start_Hz, data_unit.m_freq_stop_Hz, false);
}

void __fastcall TForm1::Setasstopfrequency1Click(TObject *Sender)
{
//	int64_t rounding_Hz = data_unit.m_freq_span_Hz / 100;
//	rounding_Hz = I64ROUND((double)rounding_Hz / 500) * 500;	// 500Hz rounding
	const int64_t rounding_Hz = 500;	// 500Hz rounding

	const int64_t stop_Hz = I64ROUND((double)m_popup_menu_mouse_Hz / rounding_Hz) * rounding_Hz;

	setStartStopHz(data_unit.m_freq_start_Hz, stop_Hz, false);
}

void __fastcall TForm1::Setascenterfrequency1Click(TObject *Sender)
{
	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	const int64_t center_Hz = m_popup_menu_mouse_Hz;
	int64_t span_Hz = data_unit.m_freq_span_Hz;

	int64_t start_Hz = center_Hz - (span_Hz / 2);
	int64_t stop_Hz  = center_Hz + (span_Hz / 2);

	if (start_Hz < min_Hz)
		span_Hz = (center_Hz - min_Hz) * 2;
	if (stop_Hz > max_Hz)
		 span_Hz = (max_Hz - center_Hz) * 2;

	span_Hz = I64ROUND((double)span_Hz / 1000) * 1000;	// 1kHz rounding

	start_Hz = center_Hz - (span_Hz / 2);
	stop_Hz  = center_Hz + (span_Hz / 2);

	setStartStopHz(start_Hz, stop_Hz, false);
}

void __fastcall TForm1::FormMouseEnter(TObject *Sender)
{
//	this->SetFocus();
}

void __fastcall TForm1::InfoPanelToggleSwitchClick(TObject *Sender)
{
	settings.infoPanel = (InfoPanelToggleSwitch->State == tssOn) ? true : false;
	InfoPanel->Visible = settings.infoPanel;
}

void __fastcall TForm1::FormMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta,
			 TPoint &MousePos, bool &Handled)
{
//	TPoint pos = ScreenToClient(MousePos);

//	const int x = pos.x;
//	const int y = pos.y;

	//TControl *control = ControlAtPos(pos, false, true, true);
//	TControl *control = ControlAtPos(MousePos, false, true, true);
//	if (control == NULL)
//		return;

//	if (control != GraphPaintBox)
//		return;

	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	if (graphs.m_mouse.graph_type_select >= 0)
	{	// graph type

		Handled = true;

		if (WheelDelta > 0)
		{
			settings.graphType[graphs.m_mouse.graph_type_select] = settings.graphType[graphs.m_mouse.graph_type_select] - 1;

			if (Application->MainForm)
				::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		}
		else
		if (WheelDelta < 0)
		{
			settings.graphType[graphs.m_mouse.graph_type_select] = settings.graphType[graphs.m_mouse.graph_type_select] + 1;

			if (Application->MainForm)
				::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		}
	}
	else
	if (graphs.m_mouse.graph_h_scale >= 0)
	{	// h-scale zoom
		const int graph      = graphs.m_mouse.graph_h_scale;
		const int graph_type = settings.graphType[graph];

		if (graphs.isFrequencyGraph(graph_type))
		{
			if (WheelDelta != 0)
			{	// zoom frequency in/out

				Handled = true;

				const int pos = graphs.m_mouse.graph_scale_pos;

				const double scale = (WheelDelta < 0) ? 1.2 : (WheelDelta > 0) ? 1.0 / 1.2 : 0;

				switch (pos)
				{
					case 0:	// left section
						{
							int64_t start_Hz = data_unit.m_freq_start_Hz;
							int64_t stop_Hz  = data_unit.m_freq_stop_Hz;
							int64_t span_Hz = I64ROUND((stop_Hz - start_Hz) * scale);
							if (span_Hz < 10000)
								span_Hz = 10000;
							span_Hz = I64ROUND((double)span_Hz / 1000) * 1000;	// 1kHz rounding

							start_Hz = stop_Hz - span_Hz;
							if (start_Hz < min_Hz) start_Hz = min_Hz;
							else
							if (start_Hz > stop_Hz) start_Hz = stop_Hz;

							setStartStopHz(start_Hz, stop_Hz, false);
						}
						break;

					case 1:	// mid section
						{
							int64_t center_Hz = data_unit.m_freq_center_Hz;
							int64_t span_Hz   = I64ROUND(data_unit.m_freq_span_Hz * scale);
							int64_t start_Hz  = center_Hz - (span_Hz / 2);
							int64_t stop_Hz   = center_Hz + (span_Hz / 2);

							if (start_Hz < min_Hz)
								span_Hz = (center_Hz - min_Hz) * 2;
							if (stop_Hz > max_Hz)
								span_Hz = (max_Hz - center_Hz) * 2;

							if (span_Hz < 10000)
								span_Hz = 10000;
							span_Hz = I64ROUND((double)span_Hz / 1000) * 1000;	// 1kHz rounding
							start_Hz = center_Hz - (span_Hz / 2);
							stop_Hz  = center_Hz + (span_Hz / 2);
							setStartStopHz(start_Hz, stop_Hz, false);
						}
						break;

					case 2:	// right section
						{
							int64_t start_Hz = data_unit.m_freq_start_Hz;
							int64_t stop_Hz  = data_unit.m_freq_stop_Hz;

							int64_t span_Hz = I64ROUND((stop_Hz - start_Hz) * scale);
							if (span_Hz < 10000)
								span_Hz = 10000;
							span_Hz = I64ROUND((double)span_Hz / 1000) * 1000;	// 1kHz rounding

							stop_Hz = start_Hz + span_Hz;
							if (stop_Hz > max_Hz) stop_Hz = max_Hz;
							else
							if (stop_Hz < start_Hz) stop_Hz = start_Hz;

							setStartStopHz(start_Hz, stop_Hz, false);
						}
						break;
				}

				if (Application->MainForm)
					::SendMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
			}
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}
	}
	else
	if (graphs.m_mouse.graph_v_scale >= 0)
	{	// v-scale zoom
		const int graph      = graphs.m_mouse.graph_v_scale;
		const int graph_type = settings.graphType[graph];

		if (graphs.isFrequencyGraph(graph_type) || graphs.isTDRGraph(graph_type))
		{
			if (WheelDelta != 0)
			{	// zoom level in/out

				Handled = true;

				const int pos = graphs.m_mouse.graph_scale_pos;

				const double direction = (WheelDelta < 0) ? -1 : +1;
				const double scale = (WheelDelta < 0) ? 1.2 : (WheelDelta > 0) ? 1.0 / 1.2 : 0;

				t_graph_setting *gs = &settings.m_graph_setting[graph_type];

				// disable auto mode
				if (gs->auto_max)
				{
					if (graphs.m_max[graph].index >= 0 && graphs.m_max[graph].value != graphs.m_min[graph].value)
						gs->max = graphs.m_max[graph].value;
					gs->auto_max = false;
				}
				if (gs->auto_min)
				{
					if (graphs.m_min[graph].index >= 0 && graphs.m_max[graph].value != graphs.m_min[graph].value)
						gs->min = graphs.m_min[graph].value;
					gs->auto_min = false;
				}

				double range = fabs(gs->max - gs->min);
				if (range < 1e-13)
					 range = 1e-13;

				switch (pos)
				{
					case 0:	// top section
						gs->max += range * direction * 0.1;

						if (gs->max < gs->min)
							gs->max = gs->min;
						break;

					case 1:	// mid section
						switch (graph_type)
						{
							case GRAPH_TYPE_LINMAG_S11:
							case GRAPH_TYPE_LINMAG_S21:
							case GRAPH_TYPE_LINMAG_S11S21:
							case GRAPH_TYPE_IMPEDANCE_S11:
							case GRAPH_TYPE_QUALITY_FACTOR_S11:
							case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
							case GRAPH_TYPE_TDR_LIN_BP_S11:
							case GRAPH_TYPE_TDR_IMPEDANCE_S11:
								{
									double gamma = gs->gamma * scale;
									if (gamma < GAMMA_MIN) gamma = GAMMA_MIN;
									else
									if (gamma > GAMMA_MAX) gamma = GAMMA_MAX;
									gs->gamma = gamma;
								}
								break;

							case GRAPH_TYPE_SMITH_S11:
							case GRAPH_TYPE_SMITH_S21:
							case GRAPH_TYPE_ADMITTANCE_S11:
							case GRAPH_TYPE_ADMITTANCE_S21:
							case GRAPH_TYPE_POLAR_S11:
							case GRAPH_TYPE_POLAR_S21:
								break;

							case GRAPH_TYPE_PHASE_VECTOR_S11:
							case GRAPH_TYPE_PHASE_VECTOR_S21:
								break;

							default:
								gs->max -= range * direction * 0.1;
								gs->min += range * direction * 0.1;
								break;
						}
						break;

					case 2:	// bottom section
						if (gs->gamma <= GAMMA_MIN)
							gs->min += range * direction * 0.1;
						else
							gs->min = gs->min * scale;

						if (gs->min > gs->max)
							gs->min = gs->max;
					break;
				}

				settings.clipGraphMinMax(graph_type, gs->max, gs->min);

				if (Application->MainForm)
					::SendMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
			}
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
		}
	}
	else
	if (graphs.m_mouse.graph >= 0)
	{
		const int graph      = graphs.m_mouse.graph;
		const int graph_type = settings.graphType[graph];

		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
			if (WheelDelta != 0)
			{	// zoom level in/out

				Handled = true;

//				const double direction = (WheelDelta < 0) ? -1 : +1;
				const double scale = (WheelDelta < 0) ? 1.2 : (WheelDelta > 0) ? 1.0 / 1.2 : 0;

				t_graph_setting *gs = &settings.m_graph_setting[graph_type];

				gs->auto_max = false;
				gs->max *= scale;

				settings.clipGraphMinMax(graph_type, gs->max, gs->min);

				if (Application->MainForm)
					::SendMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
			}
		}
	}
}

void __fastcall TForm1::SetNormaliseButtonClick(TObject *Sender)
{
	// copy the memory into the normalise memory
	const int mem = data_unit.firstUsedMem(true, 0);
	if (mem >= 0)
		data_unit.m_point_norm = data_unit.m_point_filt[mem];
	else
		data_unit.m_point_norm = data_unit.m_point_filt[0];

	EnableNormaliseSpeedButton->Enabled = !data_unit.m_point_norm.empty();
	if (!EnableNormaliseSpeedButton->Enabled)
	{
		EnableNormaliseSpeedButton->Down = false;
		settings.normalisationEnabled = false;
		EnableNormaliseSpeedButton->Hint = "";
	}
	else
	{
		String s;
		const int size = data_unit.m_point_norm.size();
		if (size > 0)
		{
			s  = common.freqToStr1(data_unit.m_point_norm[0].Hz, true, false, 6, false) + "Hz";
			s += " to ";
			s += common.freqToStr1(data_unit.m_point_norm[size - 1].Hz, true, false, 6, false) + "Hz";
		}
		s += " " + IntToStr(size) + " points";
		EnableNormaliseSpeedButton->Hint = s;
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::Showmaxmarker1Click(TObject *Sender)
{
	if (m_popup_menu_mouse_graph >= 0)
	{
		const int graph      = m_popup_menu_mouse_graph;
		const int graph_type = (graph >= 0) ? settings.graphType[graph] : -1;
		t_graph_setting *gs  = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;
		if (gs)
			gs->show_max_marker = !gs->show_max_marker;
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::Showminmarker1Click(TObject *Sender)
{
	if (m_popup_menu_mouse_graph >= 0)
	{
		const int graph      = m_popup_menu_mouse_graph;
		const int graph_type = (graph >= 0) ? settings.graphType[graph] : -1;
		t_graph_setting *gs  = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;
		if (gs)
			gs->show_min_marker = !gs->show_min_marker;
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::Automaxscale1Click(TObject *Sender)
{
	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];
		t_graph_setting *gs = &settings.m_graph_setting[graph_type];
		gs->auto_max = !gs->auto_max;
		gs->auto_max_hold_count = 0;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TForm1::Autominscale1Click(TObject *Sender)
{
	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];
		t_graph_setting *gs = &settings.m_graph_setting[graph_type];
		gs->auto_min = !gs->auto_min;
		gs->auto_min_hold_count = 0;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TForm1::Setmaxscale1Click(TObject *Sender)
{
	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];
		t_graph_setting *gs = &settings.m_graph_setting[graph_type];

		String s;
		s.printf(L"%0.5f", gs->max);

		// remove trailing zero's and unneeded decimal points
		if (s.Pos('.') > 0 || s.Pos(',') > 0)
		{
			while (!s.IsEmpty() && s[s.Length()] == '0')
				s = s.SubString(1, s.Length() - 1).Trim();
			if (!s.IsEmpty() && (s[s.Length()] == '.' || s[s.Length()] == ','))
				s = s.SubString(1, s.Length() - 1).Trim();
		}

		String units;
		switch (graph_type)
		{
			case GRAPH_TYPE_LOGMAG_S11:
			case GRAPH_TYPE_LOGMAG_S21:
			case GRAPH_TYPE_LOGMAG_S11S21:
				units = "Max log magnitude (dB)";
				break;
			case GRAPH_TYPE_PHASE_S11:
			case GRAPH_TYPE_PHASE_S21:
			case GRAPH_TYPE_PHASE_S11S21:
			case GRAPH_TYPE_PHASE_UNWRAP_S11:
			case GRAPH_TYPE_PHASE_UNWRAP_S21:
			case GRAPH_TYPE_PHASE_UNWRAP_S11S21:
				units = "Max degrees";
				break;
			case GRAPH_TYPE_SERIES_RJX_S11:
			case GRAPH_TYPE_PARALLEL_RJX_S11:
			case GRAPH_TYPE_SERIES_RESISTANCE_S11:
			case GRAPH_TYPE_SERIES_REACTANCE_S11:
				units = "Max ohms";
				break;
			case GRAPH_TYPE_GJB_S11:
				units = "Max S";
				break;
			case GRAPH_TYPE_GROUP_DELAY_S11:
			case GRAPH_TYPE_GROUP_DELAY_S21:
			case GRAPH_TYPE_GROUP_DELAY_S11S21:
				units = "Max nano seconds";
				break;
			case GRAPH_TYPE_REAL_IMAG_S11:
			case GRAPH_TYPE_REAL_IMAG_S21:
				units = "Max linear value";
				break;
			case GRAPH_TYPE_SERIES_CAPACITANCE_S11:
				units = "Max capacitance value";
				break;
			case GRAPH_TYPE_SERIES_INDUCTANCE_S11:
				units = "Max inductance value";
				break;
			case GRAPH_TYPE_LINMAG_S11:
			case GRAPH_TYPE_LINMAG_S21:
			case GRAPH_TYPE_LINMAG_S11S21:
				units = "Max linear magnitude";
				break;
			case GRAPH_TYPE_IMPEDANCE_S11:
				units = "Max ohms";
				break;
			case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
				units = "Max linear LP magnitude";
				break;
			case GRAPH_TYPE_TDR_LIN_BP_S11:
				units = "Max linear BP magnitude";
				break;
			case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11:
				units = "Max log LP magnitude (dB)";
				break;
			case GRAPH_TYPE_TDR_LOG_BP_S11:
				units = "Max log BP magnitude (dB)";
				break;
			case GRAPH_TYPE_TDR_IMPEDANCE_S11:
				units = "Max Ohms";
				break;
			case GRAPH_TYPE_QUALITY_FACTOR_S11:
				units = "Max quality factor";
				break;
			case GRAPH_TYPE_VSWR_S11:
				units = "Max VSWR";
				break;
			case GRAPH_TYPE_COAX_LOSS_S11:
				units = "Max coax loss (dB)";
				break;
			case GRAPH_TYPE_CAL_LOGMAG:
				units = "Max cal log magnitude (dB)";
				break;
			case GRAPH_TYPE_SMITH_S11:
			case GRAPH_TYPE_SMITH_S21:
			case GRAPH_TYPE_ADMITTANCE_S11:
			case GRAPH_TYPE_ADMITTANCE_S21:
			case GRAPH_TYPE_POLAR_S11:
			case GRAPH_TYPE_POLAR_S21:
				units = "Max mag";
				break;
			case GRAPH_TYPE_PHASE_VECTOR_S11:
			case GRAPH_TYPE_PHASE_VECTOR_S21:
				break;
			default:
				break;
		}

		if (!units.IsEmpty())
		{
			if (InputQuery("Graph MAX scale", units, s))
			{
				double value;
				if (common.strToValue(s, value))
				{
					gs->auto_max = false;	// disable auto max
					gs->max = value;
					settings.clipGraphMinMax(graph_type, gs->max, gs->min);

					if (Application->MainForm)
						::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
            }
			}
		}
	}
}

void __fastcall TForm1::Setminscale1Click(TObject *Sender)
{
	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];
		t_graph_setting *gs = &settings.m_graph_setting[graph_type];

		String s;
		s.printf(L"%0.5f", gs->min);

		// remove trailing zero's and unneeded decimal points
		if (s.Pos('.') > 0 || s.Pos(',') > 0)
		{
			while (!s.IsEmpty() && s[s.Length()] == '0')
				s = s.SubString(1, s.Length() - 1).Trim();
			if (!s.IsEmpty() && (s[s.Length()] == '.' || s[s.Length()] == ','))
				s = s.SubString(1, s.Length() - 1).Trim();
		}

		String units;
		switch (graph_type)
		{
			case GRAPH_TYPE_LOGMAG_S11:
			case GRAPH_TYPE_LOGMAG_S21:
			case GRAPH_TYPE_LOGMAG_S11S21:
				units = "Min log magnitude (dB)";
				break;
			case GRAPH_TYPE_PHASE_S11:
			case GRAPH_TYPE_PHASE_S21:
			case GRAPH_TYPE_PHASE_S11S21:
			case GRAPH_TYPE_PHASE_UNWRAP_S11:
			case GRAPH_TYPE_PHASE_UNWRAP_S21:
			case GRAPH_TYPE_PHASE_UNWRAP_S11S21:
				units = "Min degrees";
				break;
			case GRAPH_TYPE_SERIES_RJX_S11:
			case GRAPH_TYPE_PARALLEL_RJX_S11:
			case GRAPH_TYPE_SERIES_RESISTANCE_S11:
			case GRAPH_TYPE_SERIES_REACTANCE_S11:
				units = "Min Ohms";
				break;
			case GRAPH_TYPE_GJB_S11:
				units = "Min S";
				break;
			case GRAPH_TYPE_GROUP_DELAY_S11:
			case GRAPH_TYPE_GROUP_DELAY_S21:
			case GRAPH_TYPE_GROUP_DELAY_S11S21:
				units = "Min nano seconds";
				break;
			case GRAPH_TYPE_REAL_IMAG_S11:
			case GRAPH_TYPE_REAL_IMAG_S21:
				units = "Min linear value";
				break;
			case GRAPH_TYPE_SERIES_CAPACITANCE_S11:
				units = "Min capacitance value";
				break;
			case GRAPH_TYPE_SERIES_INDUCTANCE_S11:
				units = "Min inductance value";
				break;
			case GRAPH_TYPE_LINMAG_S11:
			case GRAPH_TYPE_LINMAG_S21:
			case GRAPH_TYPE_LINMAG_S11S21:
				units = "Min linear magnitude";
				break;
			case GRAPH_TYPE_IMPEDANCE_S11:
				units = "Min Ohms";
				break;
			case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
				units = "Min linear LP magnitude";
				break;
			case GRAPH_TYPE_TDR_LIN_BP_S11:
				units = "Min linear BP magnitude";
				break;
			case GRAPH_TYPE_TDR_LOG_LP_IMPULSE_S11:
				units = "Min log LP magnitude (dB)";
				break;
			case GRAPH_TYPE_TDR_LOG_BP_S11:
				units = "Min log BP magnitude (dB)";
				break;
			case GRAPH_TYPE_TDR_IMPEDANCE_S11:
				units = "Min Ohms";
				break;
			case GRAPH_TYPE_QUALITY_FACTOR_S11:
				units = "Min quality factor";
				break;
			case GRAPH_TYPE_VSWR_S11:
				units = "Min VSWR";
				break;
			case GRAPH_TYPE_COAX_LOSS_S11:
				units = "Min coax loss (dB)";
				break;
			case GRAPH_TYPE_CAL_LOGMAG:
				units = "Min cal log magnitude (dB)";
				break;
			case GRAPH_TYPE_SMITH_S11:
			case GRAPH_TYPE_SMITH_S21:
			case GRAPH_TYPE_ADMITTANCE_S11:
			case GRAPH_TYPE_ADMITTANCE_S21:
			case GRAPH_TYPE_POLAR_S11:
			case GRAPH_TYPE_POLAR_S21:
				//units = "Min mag";
				break;
			case GRAPH_TYPE_PHASE_VECTOR_S11:
			case GRAPH_TYPE_PHASE_VECTOR_S21:
				break;
			default:
				break;
		}

		if (!units.IsEmpty())
		{
			if (InputQuery("Graph MIN scale", units, s))
			{
				double value;
				if (common.strToValue(s, value))
				{
					gs->auto_min = false;	// disable auto min
					gs->min = value;
					settings.clipGraphMinMax(graph_type, gs->max, gs->min);

					if (Application->MainForm)
						::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
				}
			}
		}
	}
}

void __fastcall TForm1::TCPIPAddressPortEditChange(TObject *Sender)
{
	int i;
	const bool port_ok = (TryStrToInt(TCPIPPortEdit->Text, i) && i >= 1 && i <= 65535) ? true : false;
	if (port_ok)
		settings.tcpipPort = i;

	settings.tcpipAddress = TCPIPAddressEdit->Text.Trim();
	const bool address_ok = !settings.tcpipAddress.IsEmpty();

	updateDeviceComboBox();
	//	ConnectDisconnectSpeedButton->Enabled = address_ok && port_ok;
}

void __fastcall TForm1::MarkersListBoxPopupMenuPopup(TObject *Sender)
{
	m_popup_menu_marker_index = selectedMarker();

	Deleteallmarkers1->Enabled = (!settings.m_markers_freq.empty()) ? true : false;

	if (m_popup_menu_marker_index >= 0)
	{
		String s;

		//const t_marker_freq marker = settings.m_markers_freq[m_popup_menu_marker_index];

		Deletemarker2->Enabled     = true;

		Setmarkerfrequency1->Enabled = true;

		s.printf(L"Delta marker %d", 1 + m_popup_menu_marker_index);
//		s += " " + common.freqToStr1(marker.Hz, true, true, 6, false);
		Deltamarker1->Caption = s;
		Deltamarker1->Checked = (settings.m_markers_freq[m_popup_menu_marker_index].type == MARKER_TYPE_DELTA) ? true : false;
		Deltamarker1->Enabled = true;
	}
	else
	{
		Deletemarker2->Enabled = false;

		Setmarkerfrequency1->Enabled = false;

		Deltamarker1->Caption = "Delta marker";
		Deltamarker1->Enabled = false;
		Deltamarker1->Checked = false;
	}
}

void __fastcall TForm1::Deltamarker1Click(TObject *Sender)
{
	if (m_popup_menu_marker_index >= 0)
	{
		if (settings.m_markers_freq[m_popup_menu_marker_index].type == MARKER_TYPE_DELTA)
		{
			settings.m_markers_freq[m_popup_menu_marker_index].type = MARKER_TYPE_NORMAL;
		}
		else
		{
			if (!settings.m_markers_freq.empty())
				settings.m_markers_freq[m_popup_menu_marker_index].type = MARKER_TYPE_DELTA;
		}

		buildMarkerListBox();

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		//updateInfoPanel();

		DeleteFrequencyMarkersBitBtn->Enabled = !settings.m_markers_freq.empty();

		m_popup_menu_marker_index = -1;
	}
}

void __fastcall TForm1::Deltamarker2Click(TObject *Sender)
{
	if (m_popup_menu_mouse_marker >= 0)
	{
		if (settings.m_markers_freq[m_popup_menu_mouse_marker].type == MARKER_TYPE_DELTA)
		{
			settings.m_markers_freq[m_popup_menu_mouse_marker].type = MARKER_TYPE_NORMAL;
		}
		else
		{
			if (!settings.m_markers_freq.empty())
				settings.m_markers_freq[m_popup_menu_mouse_marker].type = MARKER_TYPE_DELTA;
		}

		buildMarkerListBox();

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		//updateInfoPanel();

		DeleteFrequencyMarkersBitBtn->Enabled = !settings.m_markers_freq.empty();

		m_popup_menu_mouse_marker = -1;
	}
}

void __fastcall TForm1::Setmarkerfrequency1Click(TObject *Sender)
{
	enterMarkerFrequency(m_popup_menu_marker_index);
}

void __fastcall TForm1::enterMarkerFrequency(const int index)
{
	if (index < 0 || index >= (int)settings.m_markers_freq.size())
		return;

	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	String fs = common.freqToStrMHz(settings.m_markers_freq[index].Hz);

	{
		String s1;
		s1.printf(L"Marker %d ", 1 + index);
		String s2;
		s2.printf(L"Frequency (default MHz)", 1 + index);
		if (!InputQuery(s1, s2, fs))
			return;
		fs = fs.Trim();
		if (fs.IsEmpty())
			return;
	}

	double MHz = 0;
	if (!common.strToMHz(fs, MHz))
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Invalid frequency", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	const int64_t Hz = I64ROUND(MHz * 1e6);
	if (Hz < min_Hz || Hz > max_Hz)
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Invalid frequency", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	moveMarker(index, Hz);
}

void __fastcall TForm1::Allgraphs1Click(TObject *Sender)
{
	if (m_popup_menu_mouse_marker >= 0)
	{
		settings.m_markers_freq[m_popup_menu_mouse_marker].graph = -1;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TForm1::Graph11Click(TObject *Sender)
{
	if (m_popup_menu_mouse_marker >= 0)
	{
		settings.m_markers_freq[m_popup_menu_mouse_marker].graph = 0;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TForm1::Graph21Click(TObject *Sender)
{
	if (m_popup_menu_mouse_marker >= 0)
	{
		settings.m_markers_freq[m_popup_menu_mouse_marker].graph = 1;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TForm1::Graph31Click(TObject *Sender)
{
	if (m_popup_menu_mouse_marker >= 0)
	{
//		settings.m_markers_freq[m_popup_menu_mouse_marker].graph = 2;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TForm1::Graph41Click(TObject *Sender)
{
	if (m_popup_menu_mouse_marker >= 0)
	{
//		settings.m_markers_freq[m_popup_menu_mouse_marker].graph = 3;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}


void __fastcall TForm1::Alltraces1Click(TObject *Sender)
{
	if (m_popup_menu_mouse_marker >= 0)
	{
		settings.m_markers_freq[m_popup_menu_mouse_marker].trace = -1;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TForm1::Trace11Click(TObject *Sender)
{
	if (m_popup_menu_mouse_marker >= 0)
	{
		settings.m_markers_freq[m_popup_menu_mouse_marker].trace = 0;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TForm1::Trace21Click(TObject *Sender)
{
	if (m_popup_menu_mouse_marker >= 0)
	{
		settings.m_markers_freq[m_popup_menu_mouse_marker].trace = 1;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
	}
}

void __fastcall TForm1::ShowMarkerTextToggleSwitchClick(TObject *Sender)
{
	settings.showMarkerText = (ShowMarkerTextToggleSwitch->State == tssOn) ? true : false;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::Showminmaxtext1Click(TObject *Sender)
{
	if (m_popup_menu_mouse_graph >= 0)
	{
		const int graph      = m_popup_menu_mouse_graph;
		const int graph_type = (graph >= 0) ? settings.graphType[graph] : -1;
		t_graph_setting *gs  = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;
		if (gs)
			gs->show_min_max = !gs->show_min_max;
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::PopupMeasureItem(System::TObject *Sender, Vcl::Graphics::TCanvas *ACanvas, int &Width, int &Height)
{
	if (ACanvas)
		Height = ACanvas->TextHeight("Hq") + POPUP_MENU_LINE_PADDING;
}

void __fastcall TForm1::MenuItemGraphTypeClick(TObject *Sender)
{
	TMenuItem *menu_item = dynamic_cast<TMenuItem *>(Sender);
	if (menu_item == NULL)
		return;

	const int graph_type = menu_item->Tag;
	if (m_popup_menu_mouse_graph >= 0 && graph_type >= 0 && graph_type < GRAPH_TYPE_SIZE)
		settings.graphType[m_popup_menu_mouse_graph] = graph_type;

	m_popup_menu_mouse_graph = -1;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::MenuItemChangeGraphTypeClick(TObject *Sender)
{
	TMenuItem *menu_item = dynamic_cast<TMenuItem *>(Sender);
	if (menu_item == NULL)
		return;

	const int graph_type = menu_item->Tag;
	if (m_popup_menu_graph_type_graph >= 0 && graph_type >= 0 && graph_type < GRAPH_TYPE_SIZE)
		settings.graphType[m_popup_menu_graph_type_graph] = graph_type;

	m_popup_menu_graph_type_graph = -1;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::GraphTypePopupMenuPopup(TObject *Sender)
{
	m_popup_menu_graph_type_graph = graphs.m_mouse.graph_type_select;

	if (m_popup_menu_graph_type_graph >= 0)
	{
		const int graph_type = settings.graphType[m_popup_menu_graph_type_graph];
		for (int i = 0; i < GraphTypePopupMenu->Items->Count; i++)
		{
			if (GraphTypePopupMenu->Items->Items[i]->Tag == graph_type)
			{
				GraphTypePopupMenu->Items->Items[i]->Checked = true;
				break;
			}
		}
	}

	graphs.m_mouse.graph_type_select = -1;
}

void __fastcall TForm1::Deletemarker2Click(TObject *Sender)
{
	if (m_popup_menu_marker_index >= 0)
	{
		deleteMarker(m_popup_menu_marker_index);
		m_popup_menu_marker_index = -1;
	}
}

void __fastcall TForm1::Addmarker2Click(TObject *Sender)
{
	addMarker(-1);
}

void __fastcall TForm1::GraphArrangeClick(TObject *Sender)
{
	TMenuItem *menu_item = dynamic_cast<TMenuItem *>(Sender);
	if (menu_item == NULL)
		return;

	settings.graphArrangement = (t_graph_arrange)menu_item->Tag;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::OnGraphArrangeMeasureItem(TObject *Sender, TCanvas *ACanvas,
			 int &Width, int &Height)
{
	TMenuItem *menu_item = dynamic_cast<TMenuItem *>(Sender);
	if (menu_item == NULL)
		return;

	if (ACanvas)
	{
		int width  = Width;
		int height = ACanvas->TextHeight("Hq") + POPUP_MENU_LINE_PADDING + 3;
		if (menu_item->Bitmap)
		{
			const int w = menu_item->Bitmap->Width + ACanvas->TextHeight(" " + menu_item->Caption + " ") + POPUP_MENU_LINE_PADDING + 3;
			if (width < w)
				width = w;

			const int h = menu_item->Bitmap->Height + POPUP_MENU_LINE_PADDING + 3;
			if (height < h)
				height = h;
		}
		Width  = width;
		Height = height;
	}
}

void __fastcall TForm1::OnMenuItemDraw(TObject *Sender, TCanvas *ACanvas, TRect &ARect,
			 bool Selected)
{
	TMenuItem *menu_item = dynamic_cast<TMenuItem *>(Sender);
	if (menu_item == NULL)
		return;

	if (Selected)
	{
		String s = menu_item->Caption;

		ACanvas->Brush->Color = clGray;
		ACanvas->FillRect(ARect);

		ACanvas->Font->Color = clWhite;
		ACanvas->Font->Style = ACanvas->Font->Style << fsBold;  // bold

		const int th = ACanvas->TextHeight("Hq");

		// draw right in the middle of the menu
		const int tw = ACanvas->TextWidth(s);
		const int x = (tw > (ARect.Right - ARect.Left)) ? ARect.Left + 3 : ARect.Left + (ARect.Right - ARect.Left - tw) / 2;
		const int y = ARect.Top + (ARect.Bottom - ARect.Top - th) / 2;
		ACanvas->TextOut(x, y, s);
	}
	else
	{

	}
}

void __fastcall TForm1::MedianFilterTrackBarChange(TObject *Sender)
{
	settings.medianFilterLevel = MedianFilterTrackBar->Position;

	AnsiString s;
	if (settings.medianFilterLevel <= 0)
		s = "off";
	else
		s.printf("%d", settings.medianFilterLevel);

	MedianFilterLabel->Caption = s;
	MedianFilterTrackBar->Hint = "Trace median filter " + s;

	for (int mem = 0; mem < MAX_MEMORIES; mem++)
		data_unit.m_point_filt[mem].resize(0);

	if (Application->MainForm)
		::SendMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::Scaletomaxvalue1Click(TObject *Sender)
{
	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];

		if (graphs.m_max[graph].index >= 0)
		{
			t_graph_setting *gs = &settings.m_graph_setting[graph_type];

			// disable auto max
			gs->auto_max = false;

			// set scale max
			gs->max = graphs.m_max[graph].value;

			// sanity check
			settings.clipGraphMinMax(graph_type, gs->max, gs->min);

			if (Application->MainForm)
				::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		}
	}
}

void __fastcall TForm1::Scaletominvalue1Click(TObject *Sender)
{
	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];

		if (graphs.m_min[graph].index >= 0)
		{
			t_graph_setting *gs = &settings.m_graph_setting[graph_type];

			// disable auto min
			gs->auto_min = false;

			// set scale max
			gs->min = graphs.m_min[graph].value;

			// sanity check
			settings.clipGraphMinMax(graph_type, gs->max, gs->min);

			if (Application->MainForm)
				::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		}
	}
}

void __fastcall TForm1::Deleteallmarkers1Click(TObject *Sender)
{
	if (!settings.m_markers_freq.empty())
	{
		settings.m_markers_freq.resize(0);
		MarkerListBox->Clear();
		DeleteFrequencyMarkersBitBtn->Enabled = false;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		//updateInfoPanel();
	}
}

void __fastcall TForm1::GraphPaintBoxGesture(TObject *Sender, const TGestureEventInfo &EventInfo,
			 bool &Handled)
{
	#ifdef USE_OPENGL
		return;
	#endif

/*	String s;
	if (GestureToIdent(EventInfo.GestureID, s))
	{
		ShowMessage(s);
	}
	else
	{
		ShowMessage("Could not translate gesture identifier");
	}
*/
}

void __fastcall TForm1::MemoryPopupMenuPopup(TObject *Sender)
{
	const int mem = m_popup_menu_memory_index;

	const bool empty = (mem >= 0 && mem < MAX_MEMORIES) ? data_unit.m_point_mem[mem].empty() ? true : false : true;

	LoadMemoryFromFile1->Enabled = (mem > 0 && mem < MAX_MEMORIES) ? true : false;
	SetMemory1->Enabled          = (mem > 0 && mem < MAX_MEMORIES) ? true : false;
	Memoryname1->Enabled         = (mem > 0 && mem < MAX_MEMORIES) ? true : false;
	Clearmemoryname1->Enabled    = (mem > 0 && mem < MAX_MEMORIES) ? !settings.memoryName[mem].IsEmpty() : false;

	if (mem >= 1 && mem < MAX_MEMORIES)
		ClearMemory1->Enabled = !empty;
	else
		ClearMemory1->Enabled = false;

	String s = "Set memory name";
	if (Memoryname1->Enabled && !settings.memoryName[mem].IsEmpty())
		s = "Rename memory [" + settings.memoryName[mem] + "]";
	Memoryname1->Caption = s;

	SetScanRangeFromMemory1->Enabled = !empty;
}

void __fastcall TForm1::SetScanRangeFromMemory1Click(TObject *Sender)
{
	const int mem = m_popup_menu_memory_index;
	if (mem >= 0)
	{
		const int size = data_unit.m_point_mem[mem].size();
		if (size > 0)
		{
			const int64_t start_Hz = data_unit.m_point_mem[mem][0].Hz;
			const int64_t stop_Hz  = data_unit.m_point_mem[mem][size - 1].Hz;
			setStartStopHz(start_Hz, stop_Hz, false);
		}
	}
}

void __fastcall TForm1::MemorySpeedButtonMouseDown(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y)
{
	m_memory_but_index = -1;
	m_popup_menu_memory_index = -1;

	TSpeedButton *sb = dynamic_cast<TSpeedButton *>(Sender);
	if (sb == NULL)
		return;

	int mem = -1;
	if (sb == MemorySpeedButton0) mem = 0;
	else
	if (sb == MemorySpeedButton1) mem = 1;
	else
	if (sb == MemorySpeedButton2) mem = 2;
	else
	if (sb == MemorySpeedButton3) mem = 3;
	else
	if (sb == MemorySpeedButton4) mem = 4;

	if (Shift.Contains(ssRight))
	{
		if (mem >= 0)
		{
//			if (!data_unit.m_point_mem[mem].empty())
				m_popup_menu_memory_index = mem;
		}
	}
	else
	if (Shift.Contains(ssLeft))
	{
		if (mem >= 0)
		{
			m_memory_but_index = mem;
			m_button_timer.mark();
		}
	}
}

void __fastcall TForm1::LoadMemoryFromFile1Click(TObject *Sender)
{
	loadMemoryFile(m_popup_menu_memory_index);
}

void __fastcall TForm1::SetMemory1Click(TObject *Sender)
{
	setMemory(m_popup_menu_memory_index);
}

void __fastcall TForm1::ClearMemory1Click(TObject *Sender)
{
	clearMemory(m_popup_menu_memory_index);
}

void __fastcall TForm1::TraceLPFTrackBarChange(TObject *Sender)
{
	graphs.m_lpf_coeff = (float)TraceLPFTrackBar->Position / TraceLPFTrackBar->Max;	// 0.0 to 1.0

	if (Application->MainForm)
		::SendMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::Cliptraces1Click(TObject *Sender)
{
	settings.clipTraces = !settings.clipTraces;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::Scaletominmaxvalues1Click(TObject *Sender)
{
	const int graph = m_popup_menu_mouse_graph;
	if (graph >= 0)
	{
		const int graph_type = settings.graphType[graph];
		t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

		if (gs)
		{
			if (graphs.m_max[graph].index >= 0)
			{
				// disable auto max
				gs->auto_max = false;
				// set scale max
				gs->max = graphs.m_max[graph].value;
			}

			if (graphs.m_min[graph].index >= 0)
			{
				// disable auto min
				gs->auto_min = false;
				// set scale max
				gs->min = graphs.m_min[graph].value;
			}

			// sanity check
			settings.clipGraphMinMax(graph_type, gs->max, gs->min);

			if (Application->MainForm)
				::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		}
	}
}

void __fastcall TForm1::Showmarkers1Click(TObject *Sender)
{
	if (m_popup_menu_mouse_graph >= 0)
	{
		const int graph      = m_popup_menu_mouse_graph;
		const int graph_type = (graph >= 0) ? settings.graphType[graph] : -1;
		t_graph_setting *gs  = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;
		if (gs)
			gs->show_markers = !gs->show_markers;
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::Snaptonearestpoint1Click(TObject *Sender)
{
	settings.snapToNearestPoint = !settings.snapToNearestPoint;
}

void __fastcall TForm1::Autoscalepeakhold1Click(TObject *Sender)
{
	settings.autoScalePeakHold = !settings.autoScalePeakHold;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::NumberOfPointsComboBoxChange(TObject *Sender)
{
	processNumberOfPointsComboBox();

	calibration_module.m_inter_cal.resize(0);

	if (connected())
	{
		ZeroMemory(m_comms.points_per_sec_buf, sizeof(m_comms.points_per_sec_buf));
		m_comms.points_per_sec_buf_count = -1;
		m_comms.points_per_sec = 0.0f;

		if (janvna2_comms.connected)
			janvna2_comms.updateSweepSettings = true;
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::PointBandwidthHzComboBoxChange(TObject *Sender)
{
	double d;

	processPointBandwidthHzComboBox();
/*
	if (!common.strToValue(PointBandwidthHzComboBox->Text, d, 1.0))
	{
		String s;
		s.printf(L"error: invalid bandwidth [10 to %d]", data_unit.m_vna_data.max_bandwidth_Hz);
		common.setWarning(PointBandwidthHzComboBox, s);
	}
	else
		common.setWarning(PointBandwidthHzComboBox, "");
*/
	if (connected())
	{
		sendBandwidthCommand();

		ZeroMemory(m_comms.points_per_sec_buf, sizeof(m_comms.points_per_sec_buf));
		m_comms.points_per_sec_buf_count = -1;
		m_comms.points_per_sec = 0.0f;

		if (janvna2_comms.connected)
			janvna2_comms.updateSweepSettings = true;
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::StatusBarMouseDown(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y)
{
	TStatusBar *sb = dynamic_cast<TStatusBar *>(Sender);
	if (!sb)
		return;

	if (sb == StatusBar2)
	{
		// find out which statusbar panel the user clicked on
		TStatusPanel *sp = NULL;
		int x1 = StatusBar1->Left;
		for (int i = 0; i < sb->Panels->Count; i++)
		{
			const int x2 = x1 + sb->Panels->Items[i]->Width;
			if (X >= x1 && X < x2)
			{
				sp = sb->Panels->Items[i];
				break;
			}
			x1 += sb->Panels->Items[i]->Width;
		}

		if (sp)
		{
			switch (sp->Index)
			{
				case 2:	// clicked on the battery voltage panel
					if (BatteryVoltageBitBtn->Enabled)
						BatteryVoltageBitBtnClick(BatteryVoltageBitBtn);
					break;
				default:
					break;
			}
		}
   }
}

void __fastcall TForm1::Copyimage1Click(TObject *Sender)
{
	saveImage(true);
}

void __fastcall TForm1::SaveGraphImageBitBtnClick(TObject *Sender)
{
	saveImage(false);
}

void __fastcall TForm1::SaveS1PFileBitBtnClick(TObject *Sender)
{
	// save a memory s-params
	const int mem = data_unit.firstUsedMem(true, 0);
	if (mem >= 0)
		common.saveSParams(data_unit.m_point_filt[mem], 1);
	else
		common.saveSParams(data_unit.m_point_filt[0], 1);
}

void __fastcall TForm1::Save2PFileBitBtnClick(TObject *Sender)
{
	// save a memory s-params
	const int mem = data_unit.firstUsedMem(true, 0);
	if (mem >= 0)
		common.saveSParams(data_unit.m_point_filt[mem], 4);
	else
		common.saveSParams(data_unit.m_point_filt[0], 4);
}

void __fastcall TForm1::SaveCSVFileBitBtnClick(TObject *Sender)
{
	// save a memory s-params into a CSV file
	const int mem = data_unit.firstUsedMem(true, 0);
	if (mem >= 0)
		common.saveCSV(data_unit.m_point_filt[mem], 4, false, "");
	else
		common.saveCSV(data_unit.m_point_filt[0], 4, false, "");
}

void __fastcall TForm1::CopyGraphImageBitBtnClick(TObject *Sender)
{
	saveImage(true);
}

void __fastcall TForm1::StopScanBitBtnClick(TObject *Sender)
{
	stop();
}

void __fastcall TForm1::DeleteFrequencyMarkersBitBtnClick(TObject *Sender)
{
	if (!settings.m_markers_freq.empty())
	{
		settings.m_markers_freq.resize(0);
		MarkerListBox->Clear();
		DeleteFrequencyMarkersBitBtn->Enabled = false;

		if (Application->MainForm)
			::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
		//updateInfoPanel();
	}
}

void __fastcall TForm1::SpareBitBtnClick(TObject *Sender)
{
	saveFormImage();
}

void __fastcall TForm1::Memoryname1Click(TObject *Sender)
{
	const int mem = m_popup_menu_memory_index;
	if (mem >= 1 && mem < MAX_MEMORIES)
	{
		String title = "Memory " + IntToStr(mem);
		String s = settings.memoryName[mem];
		if (!InputQuery(title, "Name", s))
			return;
		s = s.Trim();
		settings.memoryName[mem] = s;

		if (s.IsEmpty())
		{
			const int size = data_unit.m_point_mem[mem].size();
			if (size > 0)
			{
				s  = common.freqToStr1(data_unit.m_point_mem[mem][0].Hz, true, false, 6, false) + "Hz";
				s += " to ";
				s += common.freqToStr1(data_unit.m_point_mem[mem][size - 1].Hz, true, false, 6, false) + "Hz";
			}
			s += " " + IntToStr(size) + " points";
		}

		TSpeedButton *sb = NULL;
		switch (mem)
		{
			case 0: sb = MemorySpeedButton0; break;
			case 1: sb = MemorySpeedButton1; break;
			case 2: sb = MemorySpeedButton2; break;
			case 3: sb = MemorySpeedButton3; break;
			case 4: sb = MemorySpeedButton4; break;
		}
		if (sb != NULL)
			sb->Hint = s;
	}
}

void __fastcall TForm1::Clearmemoryname1Click(TObject *Sender)
{
	const int mem = m_popup_menu_memory_index;
	if (mem >= 1 && mem < MAX_MEMORIES)
	{
		settings.memoryName[mem] = "";

		TSpeedButton *sb = NULL;
		switch (mem)
		{
			case 0: sb = MemorySpeedButton0; break;
			case 1: sb = MemorySpeedButton1; break;
			case 2: sb = MemorySpeedButton2; break;
			case 3: sb = MemorySpeedButton3; break;
			case 4: sb = MemorySpeedButton4; break;
		}
		if (sb != NULL)
		{
			String s;
			const int size = data_unit.m_point_mem[mem].size();
			if (size > 0)
			{
				s  = common.freqToStr1(data_unit.m_point_mem[mem][0].Hz, true, false, 6, false) + "Hz";
				s += " to ";
				s += common.freqToStr1(data_unit.m_point_mem[mem][size - 1].Hz, true, false, 6, false) + "Hz";
			}
			s += " " + IntToStr(size) + " points";
			sb->Hint = s;
		}
	}
}

void __fastcall TForm1::EnableNormaliseSpeedButtonClick(TObject *Sender)
{
	settings.normalisationEnabled = EnableNormaliseSpeedButton->Down;

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::HistoryTrackBarChange(TObject *Sender)
{
	String s;

	// number of history frames so far
	int hist_frames = data_unit.m_history_frames;
	if (hist_frames > (int)MAX_HISTORY)
		hist_frames = (int)MAX_HISTORY;
	if (hist_frames < 0)
		hist_frames = 0;

	int hist_pos = HistoryTrackBar->Position;
	if (hist_pos < -(hist_frames - 1))
		hist_pos = -(hist_frames - 1);
	if (hist_pos > 0)
		hist_pos = 0;

	if (hist_pos <= -(hist_frames - 1))
		s = "History position oldest";
	else
	if (hist_pos >= HistoryTrackBar->Max)
		s = "History position live";
	else
		s.printf(L"History position %d", hist_pos);
	HistoryTrackBar->Hint = s;

	updateHistoryFramesInfo(true);

	if (Application->MainForm)
		::SendMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::MemorySpeedButtonMouseUp(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y)
{
	m_memory_but_index = -1;
}

void __fastcall TForm1::BitBtn1Click(TObject *Sender)
{
	data_unit.clearHistory();
	updateHistoryFramesInfo(true);
}

void __fastcall TForm1::setCalibrationSelection(t_calibration_selection cal_select)
{
	settings.calibrationSelection = cal_select;

	const TNotifyEvent ne = CalibrationSelectComboBox->OnChange;
	CalibrationSelectComboBox->OnChange = NULL;
	const int i = CalibrationSelectComboBox->Items->IndexOfObject((TObject *)settings.calibrationSelection);
	if (i >= 0)
		CalibrationSelectComboBox->ItemIndex = i;
	CalibrationSelectComboBox->OnChange = ne;

	switch (settings.calibrationSelection)
	{
		case CAL_SELECT_NONE:
			this->Caption = common.title;
 //     settings.calibrationFile="";
			break;
		case CAL_SELECT_VNA:
			this->Caption = common.title + "           cal: VNA";
 //     settings.calibrationFile="";
			break;
		case CAL_SELECT_APP:
			if (!calibration_module.m_calibration.name.IsEmpty())
			{
				String s;
				s += common.freqToStr1(calibration_module.m_calibration.point[0].HzCal, true, false, 6, false) + "Hz";
				s += " to " + common.freqToStr1(calibration_module.m_calibration.point[calibration_module.m_calibration.point.size() - 1].HzCal, true, false, 6, false) + "Hz";
				s += " " + UIntToStr(calibration_module.m_calibration.point.size()) + "p";
				s = calibration_module.m_calibration.name + " (" + s + ")";
				this->Caption = common.title + "           cal: " + s;
			}
			else
      {
				this->Caption = common.title + "           cal: NONE";
	      if (CalibrationForm)
          CalibrationForm->show();
      }
			break;
	}

	data_unit.clearHistory();

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::CalibrationSelectComboBoxChange(TObject *Sender)
{
	TComboBox *cb = CalibrationSelectComboBox;
	int i = cb->ItemIndex;
	if (i >= 0)
	{
		setCalibrationSelection((t_calibration_selection)(int)cb->Items->Objects[i]);
	}
}

void __fastcall TForm1::CWModeSpeedButtonClick(TObject *Sender)
{
	if (!CWModeSpeedButton->Down)
		cwPopupWindow(CWMHzEdit, false);
	else
		cwPopupWindow(CWMHzEdit, true, "CW frequency");	// test only

	if (connected())
	{
		if (CWModeSpeedButton->Down)
		{	// enable CW mode
			stop();

			if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
			{
				janvna2_comms.mode = MODE_GENERATOR;
//				cwPopupWindow(CWMHzEdit, true, "CW frequency");
			}
			else
			if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
			{	// V2
				nanovna2_comms.mode = MODE_GENERATOR;
//				cwPopupWindow(CWMHzEdit, true, "CW frequency");

				if (!nanovna2_comms.m_pause_comms)
					requestScan();
			}
			else
			{	// V1
				nanovna1_comms.setCW(data_unit.m_freq_cw_Hz, settings.outputPower);
//				cwPopupWindow(CWMHzEdit, true, "CW frequency");
			}
		}
		else
		{	// disable CW mode
			if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
			{
				janvna2_comms.mode = MODE_IDLE;
			}
			else
			if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
			{	// V2
				nanovna2_comms.mode = MODE_IDLE;
			}
			else
			{	// V1
				addSerialTxCommand("resume");
			}
		}
	}
}

void __fastcall TForm1::cwPopupWindow(TObject *sender, const bool open, String title)
{	// show a popup window
	if (!sender)
		return;

	if (!sender->InheritsFrom(__classid(TControl)))
		return;

	TControl *control = (TControl *)sender;
	if (!control)
		return;

//	String class_name = control->ClassName();

	if (!open)
	{	// close the popup window
//		for (int i = 0; i < control->ControlCount; i++)
		for (int i = 0; i < control->ComponentCount; i++)
		{
//			THintWindow *hw = dynamic_cast<THintWindow *>(control->Controls[i]);
			THintWindow *hw = dynamic_cast<THintWindow *>(control->Components[i]);
			if (hw)
				hw->ReleaseHandle();
		}
		return;
	}

	title = title.Trim();

	THintWindow *hw = new THintWindow(control);
	if (hw)
	{
		hw->ParentWindow = Application->Handle;

		hw->Hint = title.IsEmpty() ? control->Name : title;

		hw->StyleElements = hw->StyleElements >> seClient;
		//hw->StyleElements = hw->StyleElements << seFont << seClient << seBorder;
		hw->StyleElements = hw->StyleElements << seFont << seBorder;

		//hw->Color = clInfoBk;
		hw->Color = clBtnFace;

		const int th = hw->Canvas->TextHeight(hw->Hint);
		const int tw = hw->Canvas->TextWidth(hw->Hint);

		int x = 5;
		int y = th + 5;
//		int w = tw;
		int w = 300;

		TButton *but = new TButton(hw);
		if (but)
		{
			but->Parent       = hw;
			but->Width        = tw;
			but->Height       = 19;
			but->Top          = 5;
			but->Left         = w - but->Width;
			but->Caption      = "Close";
			but->OnClick      = cwPopupWindowOnClick;
			but->OnMouseLeave = cwPopupWindowOnMouseLeave;
			y = but->Top + but->Height + 5;
			if (w < (but->Left + but->Width))
				w = but->Left + but->Width;
		}

		TTrackBar *track_bar = new TTrackBar(hw);
		if (track_bar)
		{
			track_bar->Parent      = hw;
			track_bar->Top         = y;
			track_bar->Left        = x;
			track_bar->Width       = w - x;
			track_bar->ThumbLength = 19;
			track_bar->Height      = track_bar->ThumbLength + 6;
			track_bar->Cursor      = crHandPoint;
			track_bar->Orientation = trHorizontal;
			track_bar->TickMarks   = tmBoth;
			//track_bar->TickMarks   = tmTopLeft;
			track_bar->TickStyle   = tsNone;
			//track_bar->TickStyle   = tsAuto;
			//track_bar->TickStyle   = tsManual;
			track_bar->Min         = 0;
			track_bar->Max         = 1999;
			track_bar->PageSize    = 40;
			track_bar->Frequency   = 40;
			track_bar->OnChange = cwPopupWindowOnTrackBarChange;
			//track_bar->OnMouseLeave = cwPopupWindowOnMouseLeave;
			y += track_bar->Height + 5;
			if (w < track_bar->Width)
				w = track_bar->Width;
		}

		//const TPoint point = control->ClientToScreen(Point(0, control->Height + 5));
		//TRect rect = Rect(point.X, point.Y, point.x + x + w + 5, point.Y + y);

		const TPoint point = control->ClientToScreen(Point(control->Width + 5, -control->Height / 2));
		TRect rect = Rect(point.X, point.Y, point.x + x + w + 5, point.Y + y);

		hw->ActivateHint(rect, hw->Hint);

		cwPopupWindowupdatenTrackBar(control);

		// when done ..
		//hw->ReleaseHandle();
	}
}

void __fastcall TForm1::cwPopupWindowupdatenTrackBar(TObject *Sender)
{
	if (!Sender)
		return;

	if (!Sender->InheritsFrom(__classid(TControl)))
		return;

	TControl *control = (TControl *)Sender;
	if (!control)
		return;

//	String class_name = control->ClassName();

	for (int i = 0; i < control->ComponentCount; i++)
	{
		THintWindow *hw = dynamic_cast<THintWindow *>(control->Components[i]);
		if (hw)
		{
			for (int i = 0; i < hw->ComponentCount; i++)
			{
				TTrackBar *tb = dynamic_cast<TTrackBar *>(hw->Components[i]);
				if (tb)
				{
					const int64_t span = data_unit.m_freq_stop_Hz - data_unit.m_freq_start_Hz;
					if (span > 0)
					{
						const double d = (double)(data_unit.m_freq_cw_Hz - data_unit.m_freq_start_Hz) / span;  // 0.0 to 1.0
						const TNotifyEvent ne = tb->OnChange;
						tb->OnChange = NULL;
						tb->Position = IROUND(tb->Min + ((tb->Max - tb->Min) * d));
						tb->OnChange = ne;
						tb->Update();
						return;
					}
				}
			}
		}
	}
}

void __fastcall TForm1::cwPopupWindowOnTrackBarChange(TObject *Sender)
{
	TTrackBar *tb = dynamic_cast<TTrackBar *>(Sender);
	if (!tb)
		return;

	THintWindow *hw = dynamic_cast<THintWindow *>(tb->Parent);
	if (hw)
	{
		const double d = (double)(tb->Position - tb->Min) / (tb->Max - tb->Min);	// 0.0 to 1.0
		double Hz = data_unit.m_freq_start_Hz + ((data_unit.m_freq_stop_Hz - data_unit.m_freq_start_Hz) * d);
		if (Hz > 0)
		{
			data_unit.m_freq_cw_Hz = I64ROUND(Hz);
			String s = common.freqToStr1(data_unit.m_freq_cw_Hz, true, false, 6, false);

			CWMHzEdit->Text = s;
//			CWMHzEdit->Update();

			processCWMHzEdit(true, true);

			if (connected())
			{
				if (scanning())
					stop();

				const TNotifyEvent ne = CWModeSpeedButton->OnClick;
				CWModeSpeedButton->OnClick = NULL;
				CWModeSpeedButton->Down = true;
				CWModeSpeedButton->OnClick = ne;

				if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
				{
					janvna2_comms.mode = MODE_GENERATOR;
					janvna2_comms.updateGeneratorSettings = true;
				}
				else
				if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
				{	// V2

				}
				else
				{	// V1
					nanovna1_comms.setCW(data_unit.m_freq_cw_Hz, settings.outputPower);
				}
			}
		}
	}
}

void __fastcall TForm1::cwPopupWindowOnClick(TObject *Sender)
{
	TButton *but = dynamic_cast<TButton *>(Sender);
	if (but)
	{
		THintWindow *hw = dynamic_cast<THintWindow *>(but->Parent);
		if (hw)
			hw->ReleaseHandle();
	}
}

void __fastcall TForm1::cwPopupWindowOnMouseLeave(TObject *Sender)
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

void __fastcall TForm1::GLPanelResize(TObject *Sender)
{
	#ifdef USE_OPENGL
		TPanel *sender = dynamic_cast<TPanel *>(Sender);
		if (sender != NULL)
		{
			if (this->WindowState != wsMinimized)
			{
				graphs.glResize(sender->ClientWidth, sender->ClientHeight);

				if (!scanning() && Application->MainForm)
					::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
			}
		}
	#endif
}

void __fastcall TForm1::GLPanelDblClick(TObject *Sender)
{
	#ifdef USE_OPENGL
		if (graphs.m_mouse.marker_index >= 0)
		{	// delete the marker
			deleteMarker(graphs.m_mouse.marker_index);

			graphs.m_mouse.marker_mem   = -1;
			graphs.m_mouse.marker_index = -1;
		}
		else
		if (graphs.m_mouse.Hz > 0)
		{	// add a marker
			addMarker(graphs.m_mouse.Hz);
		}
	#endif
}

void __fastcall TForm1::GLPanelEndDrag(TObject *Sender, TObject *Target, int X,
			 int Y)
{
	#ifndef USE_OPENGL
		return;
	#endif

	TPanel *sender = dynamic_cast<TPanel *>(Sender);
	if (sender == NULL)
		return;
	TPanel *target = dynamic_cast<TPanel *>(Target);
	if (target == NULL)
		return;
	if (sender != target)
		return;


}

void __fastcall TForm1::GLPanelGesture(TObject *Sender, const TGestureEventInfo &EventInfo,
			 bool &Handled)
{
	#ifndef USE_OPENGL
		return;
	#endif

/*	String s;
	if (GestureToIdent(EventInfo.GestureID, s))
	{
		ShowMessage(s);
	}
	else
	{
		ShowMessage("Could not translate gesture identifier");
	}
*/
}

void __fastcall TForm1::GLPanelMouseDown(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y)
{
	#ifndef USE_OPENGL
		return;
	#endif

	TPanel *sender = dynamic_cast<TPanel *>(Sender);
	if (sender == NULL)
		return;

	graphs.m_mouse.down_x = X;
	graphs.m_mouse.down_y = Y;

	graphs.m_mouse.x = X;
	graphs.m_mouse.y = Y;

	graphs.m_mouse.down_graph = graphs.m_mouse.graph;

	graphs.m_mouse.down_marker = -1;

	m_popup_menu_graph_type_graph = -1;

	if (graphs.m_mouse.graph_v_scale >= 0)
	{	// start dragging the v-scale
		const int graph      = graphs.m_mouse.graph_v_scale;
		const int graph_type = settings.graphType[graph];

		if (Shift.Contains(ssLeft))
		{
			graphs.m_mouse.graph_scale_pos_drag = graphs.m_mouse.graph_scale_pos;
			graphs.m_mouse.graph_v_scale_drag   = graph;

			if (graphs.isFrequencyGraph(graph_type) || graphs.isTDRGraph(graph_type))
			{
				if (graphs.m_mouse.down_y >= 0)
				{
					t_graph_setting *gs = &settings.m_graph_setting[graph_type];

					bool center_gamma = false;
					switch (graph_type)
					{
						case GRAPH_TYPE_LINMAG_S11:
						case GRAPH_TYPE_LINMAG_S21:
						case GRAPH_TYPE_LINMAG_S11S21:
						case GRAPH_TYPE_IMPEDANCE_S11:
						case GRAPH_TYPE_QUALITY_FACTOR_S11:
						case GRAPH_TYPE_VSWR_S11:
						case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
						case GRAPH_TYPE_TDR_LIN_BP_S11:
						case GRAPH_TYPE_TDR_IMPEDANCE_S11:
							center_gamma = true;
							break;
						default:
							break;
					}

					// disable auto mode - maybe
					bool disable_auto_mode = true;
					if (center_gamma && graphs.m_mouse.graph_scale_pos == 1)
						disable_auto_mode = false;
					if (disable_auto_mode)
					{
						if (gs->auto_max && graphs.m_mouse.graph_scale_pos_drag <= 1)
						{
							if (graphs.m_max[graph].index >= 0 && graphs.m_max[graph].value != graphs.m_min[graph].value)
								gs->max = graphs.m_max[graph].value;
							gs->auto_max = false;
						}
						if (gs->auto_min && graphs.m_mouse.graph_scale_pos_drag >= 1)
						{
							if (graphs.m_min[graph].index >= 0 && graphs.m_max[graph].value != graphs.m_min[graph].value)
								gs->min = graphs.m_min[graph].value;
							gs->auto_min = false;
						}
					}

					graphs.m_mouse.down_max = gs->max;
					graphs.m_mouse.down_min = gs->min;
					graphs.m_mouse.down_val = gs->gamma;
				}

				TCursor cursor = crHandPoint;
				if (sender->Cursor != cursor)
				{
					sender->Cursor = cursor;
					// force Windows to change the cursor
					sender->Perform(WM_SETCURSOR, (unsigned int)sender->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
				}
			}
		}
	}
	else
	if (graphs.m_mouse.graph_h_scale >= 0)
	{	// start dragging the h-scale
		const int graph      = graphs.m_mouse.graph_h_scale;
		const int graph_type = settings.graphType[graph];

		if (Shift.Contains(ssLeft))
		{
			graphs.m_mouse.graph_scale_pos_drag = graphs.m_mouse.graph_scale_pos;
			graphs.m_mouse.graph_h_scale_drag   = graph;

			if (graphs.isFrequencyGraph(graph_type))
			{
				if (graphs.m_mouse.down_x >= 0)
				{
					graphs.m_mouse.down_Hz  = data_unit.m_freq_center_Hz;
					graphs.m_mouse.down_max = data_unit.m_freq_stop_Hz;
					graphs.m_mouse.down_min = data_unit.m_freq_start_Hz;
					graphs.m_mouse.down_val = data_unit.m_freq_center_Hz;
				}
			}
			else
			if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
			{
			}
			else
			if (graphs.isTDRGraph(graph_type))
			{
			}

			TCursor cursor = crHandPoint;
			if (sender->Cursor != cursor)
			{
				sender->Cursor = cursor;
				// force Windows to change the cursor
				sender->Perform(WM_SETCURSOR, (unsigned int)sender->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
			}
		}
	}
	else
	if (graphs.m_mouse.graph_type_select >= 0)
	{	// selecting a graph type
		const TPoint point = sender->ClientToScreen(TPoint(X, Y));
		GraphTypePopupMenu->Popup(point.X, point.Y);
	}
	else
	if (graphs.m_mouse.graph >= 0)
	{
		graphs.m_mouse.graph_scale_pos_drag = -1;
		graphs.m_mouse.graph_v_scale_drag   = -1;
		graphs.m_mouse.graph_h_scale_drag   = -1;
		graphs.m_mouse.graph_drag           = -1;

		const int graph_type = settings.graphType[graphs.m_mouse.graph];

		graphs.m_mouse.down_marker = settings.m_graph_setting[graph_type].show_markers ? graphs.m_mouse.marker_index : -1;

		if (graphs.m_mouse.down_marker >= 0)
		{	// the user has clicked on a marker

			if (Shift.Contains(ssLeft))
			{	// toggle it's selected state it if left mouse button clicked
				const int i = MarkerListBox->Items->IndexOfObject((TObject *)graphs.m_mouse.down_marker);
				if (i >= 0)
				{
//					for (int k = 0; k < MarkerListBox->Items->Count; k++)
//						MarkerListBox->Selected[k] = (i == k) ? true : false;

					MarkerListBox->Selected[i] = !MarkerListBox->Selected[i];

					updateInfoPanel();

					// move focus to the marker list box - allows the user to use the left/right arrow keys to step the marker through the sweep points
					if (MarkerListBox->CanFocus())
						MarkerListBox->SetFocus();
				}
			}

			TCursor cursor = crHandPoint;
			if (sender->Cursor != cursor)
			{
				sender->Cursor = cursor;
				// force Windows to change the cursor
				sender->Perform(WM_SETCURSOR, (unsigned int)sender->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
			}

		}
		else
		{	// user has not clicked on a marker .. deselect all markers
			for (int k = 0; k < MarkerListBox->Items->Count; k++)
				MarkerListBox->Selected[k] = false;
			updateInfoPanel();

			if (Shift.Contains(ssLeft))
			{
				graphs.m_mouse.graph_drag = graphs.m_mouse.graph;

				TCursor cursor = crDrag;
//				if (sender->Cursor != cursor)
				{
					sender->Cursor = cursor;
					// force Windows to change the cursor
					sender->Perform(WM_SETCURSOR, (unsigned int)sender->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
				}
			}
		}

		if (graphs.isFrequencyGraph(graph_type))
		{
			if (graphs.m_mouse.down_x >= 0)
			{
				graphs.m_mouse.down_Hz = data_unit.m_freq_center_Hz;

				//Label33->Caption = common.freqToStrMHz(m_graph_mouse_down_Hz);
			}
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
			if (graphs.m_mouse.down_marker >= 0)
				graphs.m_mouse.down_Hz = settings.m_markers_freq[graphs.m_mouse.down_marker].Hz;
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::GLPanelMouseLeave(TObject *Sender)
{
	#ifndef USE_OPENGL
		return;
	#endif

	TPanel *sender = dynamic_cast<TPanel *>(Sender);
	if (sender == NULL)
		return;

	sender->Hint = "";

//	if (graphs.m_mouse.down_marker >= 0)
	{
		graphs.m_mouse.marker_mem   = -1;
		graphs.m_mouse.marker_index = -1;
		graphs.m_mouse.graph        = -1;
		graphs.m_mouse.Hz           = -1;
		graphs.m_mouse.point_mem    = -1;
		graphs.m_mouse.point_index  = -1;
		graphs.m_mouse.time_mem     = -1;
		graphs.m_mouse.time_index   = -1;
		graphs.m_mouse.x            = -1;
		graphs.m_mouse.y            = -1;
		graphs.m_mouse.cpx          = 0;
		graphs.m_mouse.mag          = -1;
	}

	if (graphs.m_mouse.graph_drag < 0 && graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0)
	{
		graphs.m_mouse.graph_scale_pos = -1;
		graphs.m_mouse.graph_v_scale   = -1;
		graphs.m_mouse.graph_h_scale   = -1;
//		graphs.m_mouse.graph_drag      = -1;
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::GLPanelMouseMove(TObject *Sender, TShiftState Shift,
			 int X, int Y)
{
	#ifndef USE_OPENGL
		return;
	#endif

	TPanel *sender = dynamic_cast<TPanel *>(Sender);
	if (sender == NULL)
		return;

	int64_t max_Hz;
	int64_t min_Hz;
	data_unit.minMaxFreqHz(min_Hz, max_Hz);

	graphs.m_mouse.x = X;
	graphs.m_mouse.y = Y;

	TCursor cursor = crArrow;

	graphs.m_mouse.graph = -1;

	if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
	{
		graphs.m_mouse.graph_v_scale = -1;
		graphs.m_mouse.graph_h_scale = -1;
	}

	graphs.m_mouse.graph_type_select = -1;

	graphs.m_mouse.Hz           = -1;
	graphs.m_mouse.point_mem    = -1;
	graphs.m_mouse.point_index  = -1;
	graphs.m_mouse.time_mem     = -1;
	graphs.m_mouse.time_index   = -1;
	graphs.m_mouse.marker_mem   = -1;
	graphs.m_mouse.marker_index = -1;
	graphs.m_mouse.cpx          = 0;
	graphs.m_mouse.mag          = -1;

	const int smith_pixels = (m_screen_width > 0) ? IROUND(m_screen_width * 0.02f) : 20;
	const int pixels       = (m_screen_width > 0) ? IROUND(m_screen_width * 0.01f) : 10;

	// compute which graph the mouse cursor is over
	for (int graph = 0; graph < graphs.numberOfGraphs(); graph++)
	{
		const int gt = settings.graphType[graph];
		const int gx = graphs.m_graph_pos[graph].gx;
		const int gy = graphs.m_graph_pos[graph].gy;
		const int gw = graphs.m_graph_pos[graph].gw;
		const int gh = graphs.m_graph_pos[graph].gh;
		const int cx = graphs.m_graph_pos[graph].cx;
		const int cy = graphs.m_graph_pos[graph].cy;
		const int gr = graphs.m_graph_pos[graph].cr;

		const int mx = graphs.m_mouse.x;
		const int my = graphs.m_mouse.y;

		if (gw <= 0 || gh <= 0 || gr <= 0)
			continue;

		if (mx >= (gx - 50) && mx <= (gx + 50) && my >= (gy - 30) && my <= gy)
		{	// graph type
			graphs.m_mouse.graph_type_select = graph;
			break;
		}

		if (graphs.isFrequencyGraph(gt) || graphs.isTDRGraph(gt))
		{
			if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
			{
				if (mx >= (gx - 50) && mx <= (gx - 5) && my >= gy && my <= (gy + gh))
				{	// graph v-scale
					graphs.m_mouse.graph_v_scale   = graph;
					graphs.m_mouse.graph_scale_pos = ((my - gy) * 3) / gh;	// 0 to 2 (top/mid/bot)
					break;
				}

				if (mx >= gx && mx <= (gx + gw) && my >= (gy + gh + 5) && my <= (gy + gh + 30))
				{	// graph h-scale
					graphs.m_mouse.graph_h_scale   = graph;
					graphs.m_mouse.graph_scale_pos = ((mx - gx) * 3) / gw;	// 0 to 2 (left/mid/right)
					break;
				}
			}

			if (mx >= gx && mx <= (gx + gw) && my >= gy && my <= (gy + gh))
			{
				graphs.m_mouse.graph = graph;
				break;
			}
		}
		else
		if (graphs.isSmithGraph(gt) || graphs.isAdmittanceGraph(gt) || graphs.isPolarGraph(gt))
		{
			const int dx = mx - cx;
			const int dy = cy - my;
			const int dc = IROUND(sqrtf((dx * dx) + (dy * dy)));
			if (dc <= (gr + smith_pixels))   // within the circle and a bit ?
			{	// yes
				graphs.m_mouse.graph = graph;
				break;
			}
		}
	}

//	Label5->Caption = graphs.m_mouse.graph;
//	Label5->Update();

	// ************************************
	// compute the mouse position details - sweep point index, frequency and marker index

	if (graphs.m_mouse.graph >= 0 && graphs.m_mouse.graph_drag < 0)
	{
		const int graph      = graphs.m_mouse.graph;
		const int graph_type = settings.graphType[graph];

		const int gx = graphs.m_graph_pos[graph].gx;
		const int gy = graphs.m_graph_pos[graph].gy;
		const int gw = graphs.m_graph_pos[graph].gw;
		const int gh = graphs.m_graph_pos[graph].gh;
		const int cx = graphs.m_graph_pos[graph].cx;
		const int cy = graphs.m_graph_pos[graph].cy;
		const int gr = graphs.m_graph_pos[graph].cr;

		const int mx = graphs.m_mouse.x;
		const int my = graphs.m_mouse.y;

		const t_graph_setting *gs = (graph_type >= 0) ? &settings.m_graph_setting[graph_type] : NULL;

		int     m_graph   = -1;
		int     m_mem     = -1;
		int     m_channel = -1;
		int     m_index   = -1;
		int64_t m_Hz      = -1;
		double  m_secs    = -1;

		if (graphs.isFrequencyGraph(graph_type))
		{
			if (my >= gy && my <= (gy + gh))
			{
				const bool near_point = settings.snapToNearestPoint ? graphs.nearestPoint(graph, graph_type, mx, my, pixels, m_graph, m_mem, m_channel, m_index, m_Hz, m_secs) : false;

				if (!near_point)
				{	// use the mouse X position and the first valid memory
					m_Hz = graphs.xyToFreq(graph, graph_type, mx, my);

					int mem   = -1;
					int index = -1;

					if (graph_type == GRAPH_TYPE_CAL_LOGMAG)
					{	// calibration graph
						mem = -2;
						index = data_unit.indexFreq(m_Hz, mem);
					}
					else
					{	// memories
						while (mem < MAX_MEMORIES && index < 0)
						{
							mem = data_unit.firstUsedMem(true, ++mem);
							if (mem < 0)
								break;
							index = data_unit.indexFreq(m_Hz, mem);
						}

						if (mem >= 0 && mem < MAX_MEMORIES && index >= 0 && index < (int)data_unit.m_point_filt[mem].size())
						{
							m_index = index;
							m_Hz    = data_unit.m_point_filt[mem][m_index].Hz;	// snap to the sweep point frequency
						}
					}
				}

				graphs.m_mouse.Hz          = m_Hz;
				graphs.m_mouse.point_mem   = m_mem;
				graphs.m_mouse.point_index = m_index;

				if (graph_type != GRAPH_TYPE_CAL_LOGMAG)
				{	// memories

					if (gs && gs->show_markers)
					{
						if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
						{	// find the nearest marker
							int m_mem   = -1;
							int m_index = -1;
							int m_dist  = -1;
							for (int i = 0; i < (int)settings.m_markers_freq.size(); i++)
							{
								const t_marker_freq marker = settings.m_markers_freq[i];
								for (int m = 0; m < MAX_MEMORIES; m++)
								{
									if (settings.memoryEnable[m])
									{
										const int mi     = data_unit.indexFreq(marker.Hz, m);	// snap the nearest sweep point
										const int64_t Hz = data_unit.getFrequency(m, mi);
										const int kx     = graphs.freqToX(graph, graph_type, Hz);
										const int dx     = ABS(kx - mx);
										if (m_index < 0 || m_dist > dx)
										{	// found a closer marker
											m_mem   = m;
											m_index = i;
											m_dist  = dx;
										}
									}
								}
							}
							if (m_mem >= 0 && m_index >= 0 && m_dist >= 0)
							{
								if (m_dist <= pixels)	// within 'pixels' of the nearest marker ?
								{
									graphs.m_mouse.marker_mem   = m_mem;
									graphs.m_mouse.marker_index = m_index;
								}
							}
						}
					}

				}
			}

		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
			const int dx = mx - cx;
			const int dy = cy - my;
			const int cd = IROUND(sqrtf((dx * dx) + (dy * dy)));	// distance (in pixels) from center of chart

			if (gr > 0)
			{
				if (cd <= gr)
				{
					complexf cpx = complexf((float)dx / gr, (float)dy / gr);	// -1 to +1
					cpx *= gs->max;
					graphs.m_mouse.cpx = cpx;
					graphs.m_mouse.mag = sqrtf(SQR(cpx.real()) + SQR(cpx.imag()));
				}

				if (cd <= (gr + pixels))
				{
					const bool near_point = settings.snapToNearestPoint ? graphs.nearestPoint(graph, graph_type, mx, my, pixels, m_graph, m_mem, m_channel, m_index, m_Hz, m_secs) : false;

					if (near_point)
					{
						graphs.m_mouse.point_mem   = m_mem;
						graphs.m_mouse.point_index = m_index;
						graphs.m_mouse.Hz          = m_Hz;
					}

					if (gs && gs->show_markers)
					{
						if (graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
						{	// find the nearest marker
							const int g = graph;
							const int c = 0;
							int m_mem   = -1;
							int m_index = -1;
							int m_dist  = -1;
							for (int i = 0; i < (int)settings.m_markers_freq.size(); i++)
							{
								const t_marker_freq marker = settings.m_markers_freq[i];
								for (int m = 0; m < MAX_MEMORIES; m++)
								{
									if (settings.memoryEnable[m])
									{
										const int p_index = data_unit.indexFreq(marker.Hz, m);
										if (p_index >= 0 && p_index < (int)graphs.m_line_points[g][m][c].size())
										{
											const int dx = graphs.m_line_points[g][m][c][p_index].x - mx;
											const int dy = graphs.m_line_points[g][m][c][p_index].y - my;
											const int dp = (dx * dx) + (dy * dy);
											if (m_index < 0 || m_dist > dp)
											{	// found a closer line point
												//m_graph = g;
												m_mem   = m;
												//m_chan  = c;
												m_index = i;
												m_dist  = dp;
											}
										}
									}
								}
							}
							if (m_mem >= 0 && m_index >= 0 && m_dist >= 0)
							{
								m_dist = IROUND(sqrtf(m_dist));	// now in pixels
								if (m_dist <= pixels)	// within 'pixels' of the nearest marker ?
								{
									graphs.m_mouse.marker_mem   = m_mem;
									graphs.m_mouse.marker_index = m_index;
								}
							}
						}
					}
				}
			}

		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
			int mem = data_unit.firstUsedMem(true, 0);
			if (mem < 0)
				mem = 0;

			const float mf = (float)(mx - gx) / gw;	// 0.0 to 1.0

			const double freq_step = data_unit.freq_step(-1);

			if (mf >= 0.0f && mf <= 1.0f && freq_step > 0)
			{
				const double max_time = data_unit.max_time(freq_step);
				const double mt = max_time * mf;

//				const double mem_freq_step = data_unit.freq_step(mem);
//				const double mem_max_time  = data_unit.max_time(mem_freq_step);

				const int size = graphs.m_fft[graph][mem].size() / 2;

				int x = (size > 1) ? IROUND((size * mt) / max_time) : -1;

				const bool near_point = settings.snapToNearestPoint ? graphs.nearestPoint(graph, graph_type, mx, my, pixels, m_graph, m_mem, m_channel, m_index, m_Hz, m_secs) : false;

				if (!near_point)
				{	// use the mouse X position and the first valid memory
//					if (mem < MAX_MEMORIES && size > 0 && x >= 0 && x < size)
					if (mem < MAX_MEMORIES)
					{
						m_graph   = graph;
						m_index   = x;
						m_secs    = max_time * mf;
					}
				}

				graphs.m_mouse.point_mem   = m_mem;
				graphs.m_mouse.point_index = m_index;
				graphs.m_mouse.Hz          = 0;
				graphs.m_mouse.secs        = m_secs;
			}
		}
	}

	// ************************************
	// dragging a graph

	if (graphs.m_mouse.graph_drag >= 0)
	{
		cursor = crDrag;
//		if (sender->Cursor != cursor)
		{
			sender->Cursor = cursor;
			// force Windows to change the cursor
			sender->Perform(WM_SETCURSOR, (unsigned int)sender->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
		}

	}

	// ************************************
	// dragging a marker

	if (graphs.m_mouse.down_marker >= 0)
	{
		cursor = crHandPoint;

		const int graph      = graphs.m_mouse.down_graph;
		const int graph_type = settings.graphType[graph];

		if (graphs.isFrequencyGraph(graph_type))
		{
			settings.m_markers_freq[graphs.m_mouse.down_marker].Hz = graphs.m_mouse.Hz;
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
			settings.m_markers_freq[graphs.m_mouse.down_marker].Hz = graphs.m_mouse.Hz;
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}
	}

	// ************************************
	// dragging v-scale

	if (graphs.m_mouse.graph_v_scale_drag >= 0)
	{
		cursor = crHandPoint;

		const int pos        = graphs.m_mouse.graph_scale_pos_drag;
		const int graph      = graphs.m_mouse.graph_v_scale_drag;
		const int graph_type = settings.graphType[graph];

		if (graphs.isFrequencyGraph(graph_type) || graphs.isTDRGraph(graph_type))
		{
			t_graph_setting *gs = &settings.m_graph_setting[graph_type];

			double range = graphs.m_mouse.down_max - graphs.m_mouse.down_min;
			range *= 2.0;
			if (range < 1e-14)
				range = 1e-14;

			const double delta = ((double)(graphs.m_mouse.y - graphs.m_mouse.down_y) * range) / graphs.m_graph_pos[graph].gh;

			switch (graph_type)
			{
				default:
					switch (pos)
					{
						case 0:	// dragging top section
							gs->max = graphs.m_mouse.down_max + delta;
							break;
						case 1:	// dragging mid section
							gs->max = graphs.m_mouse.down_max + delta;
							gs->min = graphs.m_mouse.down_min + delta;
							break;
						case 2:	// dragging bot section
							gs->min = graphs.m_mouse.down_min + delta;
							break;
					}
					break;

				case GRAPH_TYPE_LINMAG_S11:
				case GRAPH_TYPE_LINMAG_S21:
				case GRAPH_TYPE_LINMAG_S11S21:
				case GRAPH_TYPE_IMPEDANCE_S11:
				case GRAPH_TYPE_QUALITY_FACTOR_S11:
				case GRAPH_TYPE_TDR_LIN_LP_IMPULSE_S11:
				case GRAPH_TYPE_TDR_LIN_BP_S11:
				case GRAPH_TYPE_TDR_IMPEDANCE_S11:
					switch (pos)
					{
						case 0:	// dragging top section
							gs->max = graphs.m_mouse.down_max + delta;
							break;
						case 1:	// dragging mid section
							#if 0
								gs->max = graphs.m_mouse.down_max + delta;
								gs->min = graphs.m_mouse.down_min + delta;
							#else
								{	// modify the gamma (graph non-linear scale)
									const double diff = delta / range;
									double gamma = graphs.m_mouse.down_val - (diff * 10);
									if (gamma < GAMMA_MIN) gamma = GAMMA_MIN;
									else
									if (gamma > GAMMA_MAX) gamma = GAMMA_MAX;
									gs->gamma = gamma;
								}
							#endif
							break;
						case 2:	// dragging bot section
							gs->min = graphs.m_mouse.down_min + delta;
							break;
					}
					break;

				case GRAPH_TYPE_VSWR_S11:
					switch (pos)
					{
						case 0:	// dragging top section
							gs->max = graphs.m_mouse.down_max + delta;
							break;
						case 1:	// dragging mid section .. alter the gamma rather than the min/max
						//case 2:	// dragging bottom section
							{
								const double diff = delta / range;
								double gamma = graphs.m_mouse.down_val - (diff * 10);
								if (gamma < GAMMA_MIN) gamma = GAMMA_MIN;
								else
								if (gamma > GAMMA_MAX) gamma = GAMMA_MAX;
								gs->gamma = gamma;
							}
							break;
						case 2:	// dragging bottom section
							gs->min = graphs.m_mouse.down_min + delta;
							break;
					}
					//gs->min = VSWR_MIN;
					break;

				case GRAPH_TYPE_SMITH_S11:
				case GRAPH_TYPE_SMITH_S21:
				case GRAPH_TYPE_ADMITTANCE_S11:
				case GRAPH_TYPE_ADMITTANCE_S21:
				case GRAPH_TYPE_POLAR_S11:
				case GRAPH_TYPE_POLAR_S21:
					break;

				case GRAPH_TYPE_PHASE_VECTOR_S11:
				case GRAPH_TYPE_PHASE_VECTOR_S21:
					break;
			}

			// sanity check
			settings.clipGraphMinMax(graph_type, gs->max, gs->min);
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{

			// TODO: drag the magnitude scale in/out

		}
	}

	// ************************************
	// dragging h-scale

	if (graphs.m_mouse.graph_h_scale_drag >= 0)
	{
		cursor = crHandPoint;

		const int pos        = graphs.m_mouse.graph_scale_pos_drag;
		const int graph      = graphs.m_mouse.graph_h_scale_drag;
		const int graph_type = settings.graphType[graph];

		if (graphs.isFrequencyGraph(graph_type))
		{
			const double range_Hz = graphs.m_mouse.down_max - graphs.m_mouse.down_min;
			const double delta_Hz = ((double)(graphs.m_mouse.x - graphs.m_mouse.down_x) * range_Hz) / graphs.m_graph_pos[graph].gw;

			double start_Hz = graphs.m_mouse.down_Hz - (range_Hz / 2);
			double stop_Hz  = graphs.m_mouse.down_Hz + (range_Hz / 2);

			switch (pos)
			{
				case 0:	// dragging left section
					start_Hz -= delta_Hz;
					if (start_Hz < min_Hz)
						 start_Hz = min_Hz;
					else
					if (start_Hz > (stop_Hz - 1000))
						 start_Hz =  stop_Hz - 1000;
					break;

				case 1:	// dragging mid section
					start_Hz -= delta_Hz;
					if (start_Hz < min_Hz)
						 start_Hz = min_Hz;
					else
					if (start_Hz > (max_Hz - range_Hz))
						 start_Hz =  max_Hz - range_Hz;

					stop_Hz -= delta_Hz;
					if (stop_Hz < (min_Hz + range_Hz))
						 stop_Hz =  min_Hz + range_Hz;
					else
					if (stop_Hz > max_Hz)
						 stop_Hz = max_Hz;
					break;

				case 2:	// dragging right section
					stop_Hz -= delta_Hz;
					if (stop_Hz < (start_Hz + 1000))
						 stop_Hz =  start_Hz + 1000;
					else
					if (stop_Hz > max_Hz)
						 stop_Hz = max_Hz;
					break;
			}

			setStartStopHz(I64ROUND(start_Hz), I64ROUND(stop_Hz), false);
			//setStartStopHz(I64ROUND(start_Hz), I64ROUND(stop_Hz), true);
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}
	}

	// ************************************
	// choose the mouse cursor

	if (graphs.m_mouse.graph_drag < 0 && graphs.m_mouse.graph_v_scale_drag < 0 && graphs.m_mouse.graph_h_scale_drag < 0 && graphs.m_mouse.down_marker < 0)
	{
		if (graphs.m_mouse.graph_v_scale >= 0)
		{
			switch (graphs.m_mouse.graph_scale_pos)
			{
				case 0:	// dragging top section
					cursor = crHandPoint;
					//cursor = crSizeNS;
					break;
				case 1:	// dragging mid section
					cursor = crHandPoint;
					//cursor = crVSplit;
					break;
				case 2:	// dragging bot section
					cursor = crHandPoint;
					//cursor = crSizeNS;
					break;
			}
		}
		else
		if (graphs.m_mouse.graph_h_scale >= 0)
		{
			switch (graphs.m_mouse.graph_scale_pos)
			{
				case 0:	// dragging left section
					cursor = crHandPoint;
					//cursor = crSizeWE;
					break;
				case 1:	// dragging mid section
					cursor = crHandPoint;
					//cursor = crHSplit;
					break;
				case 2:	// dragging right section
					cursor = crHandPoint;
					//cursor = crSizeWE;
					break;
			}
		}
		else
		if (graphs.m_mouse.graph_type_select >= 0)
		{
			cursor = crHandPoint;
		}
		else
		if (graphs.m_mouse.graph >= 0)
		{
			const int graph      = graphs.m_mouse.graph;
			const int graph_type = settings.graphType[graph];

//			const int gx = graphs.m_graph_pos[graph].x;
//			const int gy = graphs.m_graph_pos[graph].y;
//			const int gw = graphs.m_graph_pos[graph].w;
//			const int gh = graphs.m_graph_pos[graph].h;
//			const int cx = graphs.m_graph_pos[graph].cx;
//			const int cy = graphs.m_graph_pos[graph].cy;
//			const int gr = graphs.m_graph_pos[graph].cr;

//			const int mx = graphs.m_mouse.x;
//			const int my = graphs.m_mouse.y;

			if (graphs.m_mouse.marker_index >= 0)
			{
				cursor = crHandPoint;
			}
			else
			{
				if (graphs.isFrequencyGraph(graph_type))
				{
//					if (mx >= gx && mx < (gx + gw) && my >= gy && my < (gy + gh))
//						if (graphs.m_mouse.point_index < 0)
//							cursor = crCross;
				}
				else
				if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
				{
//					const int dx = mx - cx;
//					const int dy = cy - my;
//					const int dc = IROUND(sqrt((dx * dx) + (dy * dy)));	// distance from center
//					if (dc <= gr)
//						if (graphs.m_mouse.point_index < 0)
//							cursor = crCross;
				}
				else
				if (graphs.isTDRGraph(graph_type))
				{
//					if (mx >= gx && mx < (gx + gw) && my >= gy && my < (gy + gh))
//						//if (graphs.m_mouse.time_index < 0)
//						if (graphs.m_mouse.point_index < 0)
//							cursor = crCross;
				}
			}
		}
	}

	// ************************************

	if (sender->Cursor != cursor)
	{
		sender->Cursor = cursor;
		// force Windows to change the cursor
		sender->Perform(WM_SETCURSOR, (unsigned int)sender->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
	}

	updateInfoPanel();

	if (Application->MainForm)
		::SendMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::GLPanelMouseEnter(TObject *Sender)
{
	#ifndef USE_OPENGL
		return;
	#endif

	TPanel *sender = dynamic_cast<TPanel *>(Sender);
	if (sender == NULL)
		return;

	sender->Hint = "";

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}

void __fastcall TForm1::GLPanelMouseUp(TObject *Sender, TMouseButton Button,
			 TShiftState Shift, int X, int Y)
{
	#ifndef USE_OPENGL
		return;
	#endif

	TPanel *sender = dynamic_cast<TPanel *>(Sender);
	if (sender == NULL)
		return;

	// compute which graph the mouse cursor is over
	int graph_up = -1;
	for (int graph = 0; graph < graphs.numberOfGraphs(); graph++)
	{
		const int gx = graphs.m_graph_pos[graph].x;
		const int gy = graphs.m_graph_pos[graph].y;
		const int gw = graphs.m_graph_pos[graph].w;
		const int gh = graphs.m_graph_pos[graph].h;

		const int mx = X;
		const int my = Y;

		if (gw <= 0 || gh <= 0)
			continue;

		if (mx >= gx && mx <= (gx + gw) && my >= gy && my <= (gy + gh))
		{	// graph type
			graph_up = graph;
			break;
		}
	}

	if (graphs.m_mouse.down_marker >= 0)
	{	// dragging a marker
		const int graph_type = settings.graphType[graphs.m_mouse.down_graph];

		const int mi = graphs.m_mouse.down_marker;

		{	// delete the marker if it's been dragged off the graph
			const t_marker_freq marker = settings.m_markers_freq[mi];
			if (marker.Hz < data_unit.m_freq_start_Hz || marker.Hz > data_unit.m_freq_stop_Hz)
			{
				settings.m_markers_freq.erase(settings.m_markers_freq.begin() + mi);

				// deselect all markers
				for (int k = 0; k < MarkerListBox->Items->Count; k++)
					MarkerListBox->Selected[k] = false;
			}
		}

		buildMarkerListBox();
		DeleteFrequencyMarkersBitBtn->Enabled = !settings.m_markers_freq.empty();

		if (graphs.isFrequencyGraph(graph_type))
		{
		}
		else
		if (graphs.isSmithGraph(graph_type) || graphs.isAdmittanceGraph(graph_type) || graphs.isPolarGraph(graph_type))
		{
		}
		else
		if (graphs.isTDRGraph(graph_type))
		{
		}

	}

	if (graphs.m_mouse.graph_drag >= 0)
	{
		if (graph_up >= 0 && graphs.m_mouse.graph_drag >= 0 && graph_up != graphs.m_mouse.graph_drag)
		{	// dragged the graph to a new position .. swap the graphs over
			const int gt = settings.graphType[graphs.m_mouse.graph_drag];
			settings.graphType[graphs.m_mouse.graph_drag] = settings.graphType[graph_up];
			settings.graphType[graph_up] = gt;
		}

		graphs.m_mouse.graph_drag = -1;
	}

	graphs.m_mouse.graph_scale_pos_drag = -1;
	graphs.m_mouse.graph_v_scale_drag   = -1;
	graphs.m_mouse.graph_h_scale_drag   = -1;

	graphs.m_mouse.down_graph  = -1;
	graphs.m_mouse.down_x      = -1;
	graphs.m_mouse.down_y      = -1;
	graphs.m_mouse.down_marker = -1;

	if (sender->Cursor != crArrow)
	{
		sender->Cursor = crArrow;
		// force Windows to change the cursor
		sender->Perform(WM_SETCURSOR, (unsigned int)sender->Handle, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
	}

	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_UPDATE_GRAPH, 0, 0);
}


