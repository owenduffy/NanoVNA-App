
#ifndef JanVNA2_commsH
#define JanVNA2_commsH

//#include <thread>
//#include <mutex>
#include <vector>

#include "types.h"
#include "CriticalSection.h"
#include "HighResolutionTick.h"
#include "libusb.h"

// *******************************

#define JANV2A_VID				0x0483
#define JANV2A_PID				0x564e

#define JANV2B_VID				0x0483
#define JANV2B_PID				0x4121

// *******************************

#pragma pack(push, 1)

#pragma pack(pop)

struct t_vna_list
{
	uint16_t vid;
	uint16_t pid;
	String   manufacturer;
	String   product;
	String   serial_number;
	String   speed;
	String   dev_class;
};

// *******************************

typedef void __fastcall (__closure *janvna2_threadProcess)();

class CJanVNA2EventsThread : public TThread
{
	private:
		CJanVNA2EventsThread **m_object;	// points to address where out object pointer is located
	protected:
		void __fastcall Execute()
		{
			while (!Terminated)
			{
				//this->Sleep(0);
				this->Sleep(1);
				if (m_process)
					m_process();
			}
			ReturnValue = 0;
		}
	public:
		__fastcall CJanVNA2EventsThread(janvna2_threadProcess process, const bool start, CJanVNA2EventsThread **object) :
			TThread(!start)
		{
			m_process       = process;
			m_object        = object;
			FreeOnTerminate = true;
			Priority        = tpNormal;	// tpIdle tpLowest tpNormal tpHighest tpTimeCritical
		}
		virtual __fastcall ~CJanVNA2EventsThread()
		{
			m_process = NULL;
//			if (m_object != NULL)
//				m_object = NULL;
		}
		volatile janvna2_threadProcess m_process;
		bool __fastcall terminated()
		{
			return this->CheckTerminated();
		}
};

class CJanVNA2Thread : public TThread
{
	private:
		CJanVNA2Thread **m_object;			// points to address where out object pointer is located
	protected:
		void __fastcall Execute()
		{
			while (!this->Terminated)
			{
				this->Sleep(10);
				if (m_process)
					m_process();
			}
			ReturnValue = 0;
		}
	public:
		__fastcall CJanVNA2Thread(janvna2_threadProcess process, const bool start, CJanVNA2Thread **object) :
			TThread(!start)
		{
			m_process       = process;
			m_object        = object;
			FreeOnTerminate = true;
			Priority        = tpNormal;	// tpIdle tpLowest tpNormal tpHighest tpTimeCritical
//			Priority        = tpIdle;	// tpIdle tpLowest tpNormal tpHighest tpTimeCritical
		}
		virtual __fastcall ~CJanVNA2Thread()
		{
			m_process = NULL;
//			if (m_object != NULL)
//				m_object = NULL;
		}
		volatile janvna2_threadProcess m_process;
		bool __fastcall terminated()
		{
			return this->CheckTerminated();
		}
};

// *******************************

typedef int  __fastcall (__closure *t_usb_in_data_callback)(TObject *sender, std::vector <uint8_t> &buffer, const int size);

class CUSBRx
{
private:
	libusb_context        *m_ctx;
	libusb_device_handle  *m_handle;
	HANDLE                 m_event;
	t_usb_in_data_callback m_on_data_callback;
	libusb_transfer       *m_transfer;
	std::vector <uint8_t>  m_transfer_buffer;
	std::vector <uint8_t>  m_data_buffer;
	int                    m_data_buffer_wr;
	void __fastcall addTransferData(libusb_transfer *transfer);
	void __fastcall processCallback(libusb_transfer *transfer);
	static void callback(libusb_transfer *transfer)
	{
		if (transfer)
		{
			CUSBRx *object = reinterpret_cast < CUSBRx *>(transfer->user_data);
			if (object)
				object->processCallback(transfer);
		}
	}
public:
	CUSBRx(libusb_context *ctx, libusb_device_handle *handle, const uint8_t end_point, const int buffer_size, t_usb_in_data_callback on_data_cb);
	~CUSBRx();
	__property t_usb_in_data_callback onData = {read = m_on_data_callback, write = m_on_data_callback};
};

// *******************************

class CJanVNA2_comms
{
	#pragma pack(push, 1)
		// When changing/adding/removing variables from these structs also adjust the decode/encode functions in Protocol.cpp

		// General packet format:
		// 1. 1 byte header
		// 2. 2 byte overall packet length (with header and checksum)
		// 3. packet type
		// 4. packet payload
		// 5. 4 byte CRC32 (with header)
		struct t_janvna2_header
		{
			uint8_t  magic;
			uint16_t total_length;
			uint8_t  packet_type;
		};

		struct t_janvna2_datapoint
		{
			float real_S11, imag_S11;
			float real_S21, imag_S21;
			float real_S12, imag_S12;
			float real_S22, imag_S22;
			uint64_t Hz;
			uint16_t pointNum;
		};

		struct t_janvna2_sweepSettings
		{
			uint64_t f_start;
			uint64_t f_stop;
			uint16_t points;
			uint32_t if_bandwidth;
			int16_t  cdbm_excitation; // in 1/100 dbm
			uint8_t  excitePort1   :1;
			uint8_t  excitePort2   :1;
			uint8_t  suppressPeaks :1;
		};

		struct t_janvna2_referenceSettings
		{
			uint32_t ExtRefOuputFreq;
			uint8_t  AutomaticSwitch :1;
			uint8_t  UseExternalRef  :1;
		};

		struct t_janvna2_generatorSettings
		{
			uint64_t frequency;
			int16_t  cdbm_level;
			uint8_t  activePort;
		};

		struct t_janvna2_deviceInfo
		{
			uint16_t ProtocolVersion;
			uint8_t  FW_major;
			uint8_t  FW_minor;
			uint8_t  FW_patch;
			char     HW_Revision;
			uint8_t  extRefAvailable :1;
			uint8_t  extRefInUse     :1;
			uint8_t  FPGA_configured :1;
			uint8_t  source_locked   :1;
			uint8_t  LO1_locked      :1;
			uint8_t  ADC_overload    :1;
			uint8_t  temp_source;
			uint8_t  temp_LO1;
			uint8_t  temp_MCU;
			uint64_t limits_minFreq;
			uint64_t limits_maxFreq;
			uint32_t limits_minIFBW;
			uint32_t limits_maxIFBW;
			uint16_t limits_maxPoints;
			int16_t  limits_cdbm_min;
			int16_t  limits_cdbm_max;
			uint32_t limits_minRBW;
			uint32_t limits_maxRBW;
		};

		struct t_janvna2_manualStatus
		{
			int16_t port1min,  port1max;
			int16_t port2min,  port2max;
			int16_t refmin,    refmax;
			float   port1real, port1imag;
			float   port2real, port2imag;
			float   refreal,   refimag;
			uint8_t temp_source;
			uint8_t temp_LO;
			uint8_t source_locked :1;
			uint8_t LO_locked     :1;
		};

		struct t_janvna2_manualControl
		{
			// Highband Source
			uint8_t  SourceHighCE      :1;
			uint8_t  SourceHighRFEN    :1;
			uint8_t  SourceHighPower   :2;
			uint8_t  SourceHighLowpass :2;
			uint64_t SourceHighFrequency;
			// Lowband Source
			uint8_t  SourceLowEN       :1;
			uint8_t  SourceLowPower    :2;
			uint32_t SourceLowFrequency;
			// Source signal path
			uint8_t  attenuator        :7;
			uint8_t  SourceHighband    :1;
			uint8_t  AmplifierEN       :1;
			uint8_t  PortSwitch        :1;
			// LO1
			uint8_t  LO1CE             :1;
			uint8_t  LO1RFEN           :1;
			uint64_t LO1Frequency;
			// LO2
			uint8_t  LO2EN             :1;
			uint32_t LO2Frequency;
			// Acquisition
			uint8_t  Port1EN           :1;
			uint8_t  Port2EN           :1;
			uint8_t  RefEN             :1;
			uint32_t Samples;
			uint8_t  WindowType        :2;
		};

		struct t_janvna2_spectrumAnalyzerSettings
		{
			uint64_t f_start;
			uint64_t f_stop;
			uint32_t RBW;
			uint16_t pointNum;
			uint8_t  WindowType :2;	// 2 bits
			uint8_t  SignalID   :1;	// 1 bit
			uint8_t  Detector   :3;	// 3 bits
			uint8_t  UseDFT     :1;	// 1 bit
		};

		struct t_janvna2_spectrumAnalyzerResult
		{
			float    port1;
			float    port2;
			uint64_t frequency;
			uint16_t pointNum;
		};

		struct t_janvna2_firmwarePacket
		{
			uint32_t address;
			uint8_t  data[256];
		};

		enum class PacketType : uint8_t
		{
			None = 0,
			Datapoint = 1,
			SweepSettings = 2,
			Status = 3,
			ManualControl = 4,
			DeviceInfo = 5,
			FirmwarePacket = 6,
			Ack = 7,
			ClearFlash = 8,
			PerformFirmwareUpdate = 9,
			Nack = 10,
			Reference = 11,
			Generator = 12,
			SpectrumAnalyzerSettings = 13,
			SpectrumAnalyzerResult =  14,
			RequestDeviceInfo = 15,
		};

		struct t_packet_info
		{
			t_janvna2_header header;
			union
			{
				t_janvna2_datapoint                datapoint;
				t_janvna2_sweepSettings            sweep_settings;
				t_janvna2_manualStatus             manual_status;
				t_janvna2_manualControl            manual_control;
				t_janvna2_deviceInfo               device_info;
				t_janvna2_firmwarePacket           firmware_packet;
				t_janvna2_referenceSettings        reference_settings;
				t_janvna2_generatorSettings        generator_settings;
				t_janvna2_spectrumAnalyzerSettings spectrum_analyser_settings;
				t_janvna2_spectrumAnalyzerResult   spectrum_analyser_result;
			} data;
         uint32_t crc;
		};
/*
		const DeviceInfo defaultInfo =
		{
			.ProtocolVersion  = Protocol::Version,
			.FW_major         = 0,
			.FW_minor         = 0,
			.FW_patch         = 0,
			.HW_Revision      = '0',
			.extRefAvailable  = 0,
			.extRefInUse      = 0,
			.FPGA_configured  = 0,
			.source_locked    = 0,
			.LO1_locked       = 0,
			.ADC_overload     = 0,
			.temp_source      = 0,
			.temp_LO1         = 0,
			.temp_MCU         = 0,
			.limits_minFreq   = 0,
			.limits_maxFreq   = 6000000000,
			.limits_minIFBW   = 10,
			.limits_maxIFBW   = 1000000,
			.limits_maxPoints = 10000,
			.limits_cdbm_min  = -10000,
			.limits_cdbm_max  = 1000,
			.limits_minRBW    = 1,
			.limits_maxRBW    = 1000000,
		};

		protocol::DeviceInfo Device::lastInfo = defaultInfo;
*/

	#pragma pack(pop)

		struct t_usb_callback
		{
			CRITICAL_SECTION        critical_section;
			libusb_transfer         *transfer;
			std::vector <uint8_t>   buffer;
			int                     rx_size;
			HANDLE                  mutex;
		};

	private:
		t_mode m_mode;			// current mode of operation .. idle mode, initialisation mode, scanning/sweeping mode etc
		t_mode m_new_mode;	// requested mode

		bool m_pause_comms;
		bool m_update_sweep_settings;
		bool m_update_generator_settings;

		std::vector <t_vna_list> m_vna_list;

		int  m_max_retries;
		int  m_retries;
		int  m_ack_timeout_ms;
		int  m_poll_ms;
		bool m_ack;
		bool m_nack;
		bool m_device_info_ok;

		CHighResolutionTick m_rx_data_timer;	// time between rx callbacks
		CHighResolutionTick m_ack_timer;
		CHighResolutionTick m_poll_timer;
		CHighResolutionTick m_new_points_timer;

		uint32_t m_num_frames;

		int m_prev_index;

		struct
		{
			//CCriticalSectionObj cs;
			volatile int  packet_wr;
			volatile int  packet_rd;
			t_packet_info packet[4096];
		} m_rx_packets;

		int m_output_power;

		t_janvna2_generatorSettings m_own_generator_settings;
		t_janvna2_sweepSettings     m_own_sweep_settings;

		t_janvna2_sweepSettings            m_sweep_settings;
		t_janvna2_manualStatus             m_status;
		t_janvna2_manualControl            m_manual_control;
		t_janvna2_deviceInfo               m_device_info;
		t_janvna2_firmwarePacket           m_firmware_packet;
		t_janvna2_referenceSettings        m_reference_settings;
		t_janvna2_generatorSettings        m_generator_settings;
		t_janvna2_spectrumAnalyzerSettings m_spectrum_analyzer_settings;
		t_janvna2_spectrumAnalyzerResult   m_spectrum_analyzer_result;

		std::vector <uint8_t> temp_buf;

		struct
		{
			std::vector <uint8_t> buffer;
			int index_wr;
		} m_data_tx;

		TThreadPriority       m_events_thread_priority;
		CJanVNA2EventsThread *m_events_thread;

		CJanVNA2Thread       *m_thread;

		int __fastcall deviceDetails(const uint16_t vid, const uint16_t pid, const bool show_details);
		int __fastcall updateDeviceList();

		bool __fastcall startThread();
		void __fastcall stopThread(const bool local = false);

		String __fastcall packetTypeToStr(const PacketType packet_type);
		String __fastcall usbSpeedToStr(const int usb_speed);
		String __fastcall usbClassToStr(const uint8_t usb_class);

		void __fastcall processRxPacket(const t_packet_info *p_packet);

		void __fastcall threadEventsProcess();
		void __fastcall threadProcess();

		void __fastcall setSweepSettings(const bool update);
		void __fastcall setGeneratorSettings(const bool update);

		uint32_t __fastcall crc32(uint32_t crc, const void *data, const int len);
//		uint16_t DecodeBuffer(uint8_t *buf, uint16_t len, PacketInfo *info);
//		uint16_t EncodePacket(const PacketInfo &packet, uint8_t *dest, uint16_t destsize);

		bool __fastcall queueTxPacket(const PacketType packet_type, const void *data, const int data_size);
		void __fastcall sendRequestDeviceInfo();
		void __fastcall sendSweepSettings(const bool idle);
		void __fastcall sendGeneratorSettings(const bool enable);

		int __fastcall requestPoints(int num_points);
		void __fastcall newUnit();
		void __fastcall setMode(const t_mode mode);

		void __fastcall setPauseComms(bool pause);

		bool __fastcall areWeConnected()
		{
			return (m_device.hdev != NULL) ? true : false;
		}

		CUSBRx *m_usb_rx_data;
		CUSBRx *m_usb_in_log;
		int  __fastcall usbRxOnDataCallback(TObject *sender, std::vector <uint8_t> &buffer, const int size);
		int  __fastcall usbRxOnLogCallback(TObject *sender, std::vector <uint8_t> &buffer, const int size);

		static void libusb_log_cb(libusb_context *ctx, enum libusb_log_level level, const char *str);

	public:
		CJanVNA2_comms();
		~CJanVNA2_comms();

		struct
		{
			libusb_context *ctx;
			libusb_device_handle *hdev;
			t_vna_list vna;
		} m_device;

		int __fastcall openDevice(const uint16_t vid, const uint16_t pid);
		void __fastcall closeDevice(const bool local = false);

		int __fastcall devicePresent(const uint16_t vid, const uint16_t pid);
		int __fastcall getDeviceList(std::vector <t_vna_list> &vna_list);
		int __fastcall listDevices();

		float __fastcall minOutputPowerdBm();
		float __fastcall maxOutputPowerdBm();
		void __fastcall setOutputPower(const float dBm);

		void __fastcall reset(const bool reset_vna);

		__property t_mode mode    = {read = m_mode, write = setMode};
		__property bool connected = {read = areWeConnected};

		__property bool pauseComms              = {read = m_pause_comms,               write = setPauseComms};

		__property bool updateSweepSettings     = {read = m_update_sweep_settings,     write = setSweepSettings};
		__property bool updateGeneratorSettings = {read = m_update_generator_settings, write = setGeneratorSettings};
};

extern CJanVNA2_comms janvna2_comms;

#endif

