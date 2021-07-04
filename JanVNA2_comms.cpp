
#pragma hdrstop

#include "JanVNA2_comms.h"
#include "CommsUnit.h"
#include "DataUnit.h"
#include "Unit1.h"
#include "SettingsUnit.h"

#pragma package(smart_init)

#define CDBM_SCALE	100

// *******************************
// protocol

const int EP_Data_Out_Addr = 0x01;
const int EP_Data_In_Addr  = 0x81;
const int EP_Log_In_Addr   = 0x82;

const uint8_t header_magic = 0x5A;

const uint16_t Version = 1;

// *******************************

class Encoder
{
public:
	 Encoder(uint8_t *buf, uint16_t size) :
		  buf(buf),
		  bufSize(size),
		  usedSize(0),
		  bitpos(0)
	{
		  memset(buf, 0, size);
	};

	 template<typename T> bool add(T data)
	 {
		  if (bitpos != 0)
		  {
				// add padding to next byte boundary
				bitpos = 0;
				usedSize++;
		  }

		  if (bufSize - usedSize < (long) sizeof(T))
		  {
				// not enough space left
				return false;
		  }

		  memcpy(&buf[usedSize], &data, sizeof(T));
		  usedSize += sizeof(T);

		  return true;
	 }

	 bool addBits(uint8_t value, uint8_t bits)
	 {
		  if (bits >= 8 || usedSize >= bufSize)
		  {
				return false;
		  }

		  buf[usedSize] |= (value << bitpos) & 0xFF;
		  bitpos += bits;

		  if (bitpos > 8)
		  {
				// the value did not fit completely into the current byte
				if (usedSize >= bufSize - 1)
				{
					 // already at maximum limit, not enough space for remaining bits
					 return false;
				}

				// move access to next byte
				bitpos -= 8;
				usedSize++;
				// add remaining bytes
				buf[usedSize] = value >> (bits - bitpos);
		  }
		  else
		  if (bitpos == 8)
		  {
				bitpos = 0;
				usedSize++;
		  }

		  return true;
	 }

	 uint16_t getSize() const
	 {
		  if (bitpos == 0)
		  {
				return usedSize;
		  }
		  else
		  {
				return usedSize + 1;
		  }
	 };

private:
	 uint8_t *buf;
	 uint16_t bufSize;
	 uint16_t usedSize;
	 uint8_t bitpos;
};

class Decoder
{
public:
	 Decoder(const uint8_t *buf) :
		  buf(buf),
		  usedSize(0),
		  bitpos(0)
	 {
	 };

	 template<typename T> void get(T &t)
	 {
		  if (bitpos != 0)
		  {
				// add padding to next byte boundary
				bitpos = 0;
				usedSize++;
		  }
		  // still enough bytes available
		  memcpy(&t, &buf[usedSize], sizeof(T));
		  usedSize += sizeof(T);
	 }

	 uint8_t getBits(uint8_t bits)
	 {
		  if (bits >= 8)
		  {
				return 0;
		  }

		  uint8_t mask = 0x00;
		  for(uint8_t i=0;i<bits;i++)
		  {
				mask <<= 1;
				mask |= 0x01;
		  }

		  uint8_t value = (buf[usedSize] >> bitpos) & mask;
		  bitpos += bits;

		  if (bitpos > 8)
		  {
				// the current byte did not contain the complete value
				// move access to next byte
				bitpos -= 8;
				usedSize++;
				// get remaining bits
				value |= (buf[usedSize] << (bits - bitpos)) & mask;
		  }
		  else
		  if (bitpos == 8)
		  {
				bitpos = 0;
				usedSize++;
		  }

		  return value;
	 }

private:
	 const uint8_t *buf;
	 uint16_t usedSize;
	 uint8_t bitpos;
};
/*
uint16_t Protocol::EncodePacket(const PacketInfo &packet, uint8_t *dest, uint16_t destsize)
{
	int16_t payload_size = 0;

	switch (packet.type)
	{
		case PacketType::Datapoint:
			payload_size = EncodeDatapoint(packet.datapoint, &dest[4], destsize - 8);
			break;
		case PacketType::SweepSettings:
			payload_size = EncodeSweepSettings(packet.settings, &dest[4], destsize - 8);
			break;
		case PacketType::Reference:
			payload_size = EncodeReferenceSettings(packet.reference, &dest[4], destsize - 8);
			break;
		case PacketType::DeviceInfo:
			payload_size = EncodeDeviceInfo(packet.info, &dest[4], destsize - 8);
			break;
		case PacketType::Status:
			payload_size = EncodeStatus(packet.status, &dest[4], destsize - 8);
			break;
		case PacketType::ManualControl:
			payload_size = EncodeManualControl(packet.manual, &dest[4], destsize - 8);
			break;
		case PacketType::FirmwarePacket:
			payload_size = EncodeFirmwarePacket(packet.firmware, &dest[4], destsize - 8);
			break;
		case PacketType::Generator:
			payload_size = EncodeGeneratorSettings(packet.generator, &dest[4], destsize - 8);
			break;
		case PacketType::SpectrumAnalyzerSettings:
			payload_size = EncodeSpectrumAnalyzerSettings(packet.spectrumSettings, &dest[4], destsize - 8);
			break;
		case PacketType::SpectrumAnalyzerResult:
			payload_size = EncodeSpectrumAnalyzerResult(packet.spectrumResult, &dest[4], destsize - 8);
			break;
		case PacketType::Ack:
		case PacketType::PerformFirmwareUpdate:
		case PacketType::ClearFlash:
		case PacketType::Nack:
		case PacketType::RequestDeviceInfo:
			// no payload, nothing to do
			break;
		case PacketType::None:
			break;
	}

	if (payload_size < 0 || payload_size + 8 > destsize)
	{
		// encoding failed, buffer too small
		return 0;
	}

	// Write header
	dest[0] = header;
	uint16_t overall_size = payload_size + 8;
	memcpy(&dest[1], &overall_size, 2);
	dest[3] = (int)packet.type;

	// Calculate checksum
	uint32_t crc = 0x00000000;
	if (packet.type == PacketType::Datapoint)
	{
		// CRC calculation takes about 18us which is the bulk of the time required to encode and transmit a datapoint.
		// Skip CRC for data points to optimize throughput
		crc = 0x00000000;
	}
	else
	{
		crc = CRC32(0, dest, overall_size - 4);
	}
	memcpy(&dest[overall_size - 4], &crc, 4);

	return overall_size;
}
*/

// *******************************

CUSBRx::CUSBRx(libusb_context *ctx, libusb_device_handle *handle, const uint8_t end_point, const int buffer_size, t_usb_in_data_callback on_data_cb)
{
	int res = 0;

	m_event            = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ctx              = ctx;
	m_handle           = handle;
	m_on_data_callback = on_data_cb;
	m_data_buffer_wr   = 0;

	int num_iso_pack = 0;
//	int num_iso_pack = 16;

	m_transfer_buffer.resize(4096);

	if (buffer_size > 0)
		m_data_buffer.resize(buffer_size);

	try
	{
		m_transfer = libusb_alloc_transfer(num_iso_pack);
		if (m_transfer)
		{
			if (num_iso_pack > 0)
			{
				libusb_fill_iso_transfer(m_transfer, m_handle, end_point, &m_transfer_buffer[0], m_transfer_buffer.size(), num_iso_pack, callback, this, 100);
				libusb_set_iso_packet_lengths(m_transfer, m_transfer_buffer.size() / num_iso_pack);
			}
			else
			{
				libusb_fill_bulk_transfer(m_transfer, m_handle, end_point, &m_transfer_buffer[0], m_transfer_buffer.size(), callback, this, 100);
			}
			res = libusb_submit_transfer(m_transfer);
			if (res < 0)
				libusb_free_transfer(m_transfer);
		}
	}
	catch (Exception &exception)
	{
		//exception.ToString());
	}
	if (res < 0 && Form1)
	{
		const char *str = libusb_strerror(res);
		String s = "rx: usb rx transfer submit error";
		if (str != NULL)
			s += ": " + String(str);
		Form1->pushCommMessage(s);
	}
}

CUSBRx::~CUSBRx()
{
	if (m_transfer)
	{
		int res;

		// clear event flag
		if (m_event)
			ResetEvent(m_event);

		// cancel transfers
		try
		{
			res = libusb_cancel_transfer(m_transfer);
		}
		catch (...)
		{
		}
		if (res < 0 && Form1)
		{
			const char *str = libusb_strerror(res);
			String s = "rx: usb rx transfer cancel error";
			if (str != NULL)
				s += ": " + String(str);
			Form1->pushCommMessage(s);
		}

		// wait for cancellation to complete
		if (m_event)
		{
			res = 0;
			DWORD ret = WAIT_OBJECT_0;
			for (int i = 0; i < 50; i++)
			{
				int completed = 0;
				struct timeval tv = {0, 1000};
				res = libusb_handle_events_timeout_completed(m_ctx, &tv, &completed);
				if (res < 0)
					break;
				ret = WaitForSingleObject(m_event, 1);
				//if (ret == WAIT_OBJECT_0 || completed != 0)
				if (ret == WAIT_OBJECT_0)
					break;
			}
			if (Form1)
			{
				if (res < 0)
				{
					const char *str = libusb_strerror(res);
					String s = "rx: usb rx transfer handle error";
					if (str != NULL)
						s += ": " + String(str);
					Form1->pushCommMessage(s);
				}
				if (ret != WAIT_OBJECT_0)
					Form1->pushCommMessage("rx: usb rx transfer cancel timed out");
			}
		}
	}

	HANDLE event       = m_event;
	m_event            = NULL;
	m_transfer         = NULL;
	m_ctx              = NULL;
	m_handle           = NULL;
	m_on_data_callback = NULL;

	if (event)
		CloseHandle(event);
}

void __fastcall CUSBRx::addTransferData(libusb_transfer *transfer)
{
	if (transfer == NULL)
		return;

	const uint8_t *buf = (uint8_t *)transfer->buffer;
	const int len      = transfer->actual_length;

	if (buf && len > 0)
	{	// copy the data into our buffer
		memcpy(&m_data_buffer[m_data_buffer_wr], buf, len);
		m_data_buffer_wr += len;

		if (m_data_buffer_wr >= ((int)m_data_buffer.size() / 2) && Form1)
		{
			String s;
			s.printf(L"rx: usb rx transfer more than half full %d/%u", m_data_buffer_wr, m_data_buffer.size());
			Form1->pushCommMessage(s);
		}

		#if 0
			// TEST ONLY
			String s;
			s.printf(L"transfer cb %d\n", len);
			common.logFileAppend(s + "\n");
		#endif
	}

	if (len >= (transfer->length / 2))
	{	// the buffer is quite full .. double it's size
		m_transfer_buffer.resize(m_transfer_buffer.size() * 2);
		transfer->buffer = &m_transfer_buffer[0];
		transfer->length = m_transfer_buffer.size();
		if (Form1)
		{
			String s;
			s.printf(L"rx: usb rx transfer buffer was more than half full %d, increased size to %u", len, m_transfer_buffer.size());
			Form1->pushCommMessage(s);
		}
	}

	int res = 0;
	try
	{
		res = libusb_submit_transfer(transfer);
		if (res < 0)
			libusb_free_transfer(transfer);
	}
	catch (...)
	{
	}
	if (res < 0)
	{
		const char *str = libusb_strerror(res);
		if (Form1 && str != NULL)
			Form1->pushCommMessage("rx: usb buffer submit error: " + String(str));
	}

	if (transfer->status == LIBUSB_TRANSFER_TIMED_OUT)
	{
		if (Form1 && len != 0)
		{
			String s;
			s.printf(L"rx: usb buffer timed out .. ep %02X  act-len %d  flags %02X", transfer->endpoint, len, transfer->flags);
			Form1->pushCommMessage(s);
		}
	}
}

void __fastcall CUSBRx::processCallback(libusb_transfer *transfer)
{
	int res;

	if (transfer == NULL)
		return;

	switch (transfer->type)
	{
		case LIBUSB_TRANSFER_TYPE_CONTROL:
			if (Form1)
				Form1->pushCommMessage("rx: usb rx transfer type control");
			break;

		case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
			if (Form1)
				Form1->pushCommMessage("rx: usb rx transfer type iso");

			for (int packet = 0; packet < transfer->num_iso_packets; packet++)
			{
				struct libusb_iso_packet_descriptor *pack = &transfer->iso_packet_desc[packet];

				if (pack->status != LIBUSB_TRANSFER_COMPLETED)
				{
					String s;
					s.printf(L"rx: usb rx transfer iso error .. pack %d status %d", packet, pack->status);
					if (Form1)
						Form1->pushCommMessage(s);
				}
				else
				{
//					const uint8_t *buf = libusb_get_iso_packet_buffer(transfer, packet);

//					if (buf && len > 0)
//					{	// copy the data into our buffer
//						memcpy(&m_data_buffer[m_data_buffer_wr], buf, len);
//						m_data_buffer_wr += len;
//
//						if (m_data_buffer_wr >= ((int)m_data_buffer.size() / 2) && Form1)
//							Form1->pushCommMessage("rx: usb rx transfer half full");
//					}

					if (Form1)
					{
						String s;
						s.printf(L"rx: usb rx transfer .. pack %d  length %u  actual_length %u", packet, pack->length, pack->actual_length);
						Form1->pushCommMessage(s);
					}
				}
			}
			break;

		case LIBUSB_TRANSFER_TYPE_BULK:
//			if (Form1)
//				Form1->pushCommMessage("rx: usb rx transfer type bulk");
			if (transfer->status == LIBUSB_TRANSFER_COMPLETED || transfer->status == LIBUSB_TRANSFER_TIMED_OUT)
			{
				addTransferData(transfer);

				#if 0
					// TEST ONLY
					if (transfer->endpoint == EP_Data_In_Addr)
						common.dataFileAppend(transfer->buffer, transfer->actual_length);
				#endif
			}
			break;

		case LIBUSB_TRANSFER_TYPE_INTERRUPT:
//			if (Form1)
//				Form1->pushCommMessage("rx: usb rx transfer type interrupt");
			break;

		case LIBUSB_TRANSFER_TYPE_BULK_STREAM:
			if (Form1)
				Form1->pushCommMessage("rx: usb rx transfer type bulk stream");
			break;
	}

	switch (transfer->status)
	{
		case LIBUSB_TRANSFER_COMPLETED:
			break;

		case LIBUSB_TRANSFER_NO_DEVICE:
			if (Form1)
				Form1->pushCommMessage("rx: usb rx transfer no device");
			if ((transfer->flags & LIBUSB_TRANSFER_FREE_TRANSFER) == 0)
			{
				try
				{
					libusb_free_transfer(transfer);
				}
				catch (...)
				{
				}
			}
			transfer = NULL;
			break;

		case LIBUSB_TRANSFER_ERROR:
		case LIBUSB_TRANSFER_OVERFLOW:
		case LIBUSB_TRANSFER_STALL:
			if (Form1)
				Form1->pushCommMessage("rx: usb rx transfer error [" + IntToStr(transfer->status) + "]");
			if ((transfer->flags & LIBUSB_TRANSFER_FREE_TRANSFER) == 0)
			{
				try
				{
					libusb_free_transfer(transfer);
				}
				catch (...)
				{
				}
			}
			transfer = NULL;
			break;

		case LIBUSB_TRANSFER_CANCELLED:
			//if (Form1)
			//	Form1->pushCommMessage("rx: usb rx transfer cancelled");
			if ((transfer->flags & LIBUSB_TRANSFER_FREE_TRANSFER) == 0)
			{
				try
				{
					libusb_free_transfer(transfer);
				}
				catch (...)
				{
				}
			}
			if (m_event)
				SetEvent(m_event);	// let the owner know we have cancelled the transfers
			transfer = NULL;
			break;

		case LIBUSB_TRANSFER_TIMED_OUT:
			if (Form1 && transfer->actual_length != 0)
			{
				String s;
				s.printf(L"rx: usb rx transfer timed out .. ep %02X  act-len %d  flags %02X", transfer->endpoint, transfer->actual_length, transfer->flags);
				Form1->pushCommMessage(s);
			}
			break;

		default:
			if (Form1)
				Form1->pushCommMessage("rx: usb rx transfer unknown status [" + UIntToStr((uint32_t)transfer->status) + "]");
			break;
	}

	if (m_on_data_callback && m_data_buffer_wr > 0)
	{
		const int bytes_used = m_on_data_callback((TObject *)this, m_data_buffer, m_data_buffer_wr);
		// remove the used bytes
		if (bytes_used >= m_data_buffer_wr)
		{
			m_data_buffer_wr = 0;
		}
		else
		if (bytes_used > 0)
		{
			memmove(&m_data_buffer[0], &m_data_buffer[bytes_used], m_data_buffer_wr - bytes_used);
			m_data_buffer_wr -= bytes_used;
		}
	}
}

// *******************************

CJanVNA2_comms janvna2_comms;

CJanVNA2_comms::CJanVNA2_comms()
{
	m_usb_rx_data = NULL;
	m_usb_in_log  = NULL;

	m_device.hdev = NULL;

//	m_events_thread_priority = tpTimeCritical;	// tpIdle tpLowest tpNormal tpHighest tpTimeCritical
	m_events_thread_priority = tpNormal;	// tpIdle tpLowest tpNormal tpHighest tpTimeCritical

	m_events_thread = NULL;
	m_thread        = NULL;

	m_data_tx.buffer.resize(4096);
	m_data_tx.index_wr = 0;

	m_rx_packets.packet_rd = 0;
	m_rx_packets.packet_wr = 0;

	reset(false);
 }

CJanVNA2_comms::~CJanVNA2_comms()
{
	stopThread();
	closeDevice();
}

bool __fastcall CJanVNA2_comms::startThread()
{
	if (m_events_thread == NULL)
		m_events_thread = new CJanVNA2EventsThread(&threadEventsProcess, true, &m_events_thread);
	if (m_events_thread == NULL)
	{
		stopThread();
		return false;
	}

	if (m_thread == NULL)
		m_thread = new CJanVNA2Thread(&threadProcess, true, &m_thread);
	if (m_thread == NULL)
	{
		stopThread();
		return false;
	}

	return true;
}

void __fastcall CJanVNA2_comms::stopThread(const bool local)
{
	CJanVNA2EventsThread *events_thread = m_events_thread;
	m_events_thread = NULL;

	CJanVNA2Thread *thread = m_thread;
	m_thread = NULL;

//	HANDLE handle        = GetCurrentProcess();
//	HANDLE thread_handle = GetCurrentThread();

	if (events_thread != NULL)
	{
		events_thread->m_process = NULL;
		events_thread->Terminate();
	}

	if (thread != NULL)
	{
		thread->m_process = NULL;
		thread->Terminate();
	}
}

void __fastcall CJanVNA2_comms::reset(const bool reset_vna)
{
	m_mode                      = MODE_NONE;
	m_new_mode                  = MODE_NONE;
	m_max_retries               = 4;
	m_retries                   = 0;
	m_pause_comms               = false;
	m_ack_timeout_ms            = 800;
	m_poll_ms                   = DEFAULT_POLL_JANVNAV2_MS;
	m_device_info_ok            = false;
	m_ack                       = false;
	m_nack                      = false;
	m_data_tx.index_wr          = 0;
	m_update_sweep_settings     = false;
	m_update_generator_settings = false;
	m_output_power              = IROUND(DEFAULT_JANVNAV2_MAX_CDBM * CDBM_SCALE);

	// default sweep settings
	m_own_sweep_settings.f_start         = data_unit.m_freq_start_Hz;
	m_own_sweep_settings.f_stop          = data_unit.m_freq_stop_Hz;
	m_own_sweep_settings.points          = data_unit.m_points;
	m_own_sweep_settings.if_bandwidth    = data_unit.m_bandwidth_Hz;
	m_own_sweep_settings.cdbm_excitation = m_output_power;
	m_own_sweep_settings.excitePort1     = 1;
	m_own_sweep_settings.excitePort2     = 0;
	m_own_sweep_settings.suppressPeaks   = 1;

	// default generator settings
	m_own_generator_settings.frequency  = data_unit.m_freq_cw_Hz;
	m_own_generator_settings.cdbm_level = m_output_power;
	m_own_generator_settings.activePort = 1;	// 0 = off, 1 = port 1, 2 = port 2

	//CCriticalSection cs(m_rx_packets.cs);
	m_rx_packets.packet_rd = 0;
	m_rx_packets.packet_wr = 0;
}

void __fastcall CJanVNA2_comms::setSweepSettings(const bool update)
{
	if (update)
	{
		m_own_sweep_settings.f_start         = data_unit.m_freq_start_Hz;
		m_own_sweep_settings.f_stop          = data_unit.m_freq_stop_Hz;
		m_own_sweep_settings.points          = data_unit.m_points;
		m_own_sweep_settings.if_bandwidth    = data_unit.m_bandwidth_Hz;
		m_own_sweep_settings.cdbm_excitation = m_output_power;
		m_own_sweep_settings.excitePort1     = 1;
		m_own_sweep_settings.excitePort2     = 0;
		m_own_sweep_settings.suppressPeaks   = 1;
	}

	m_update_sweep_settings = true;
}

void __fastcall CJanVNA2_comms::setGeneratorSettings(const bool update)
{
	if (update)
	{
		m_own_generator_settings.frequency  = data_unit.m_freq_cw_Hz;
		m_own_generator_settings.cdbm_level = m_output_power;
		m_own_generator_settings.activePort = 1;	// 0 = off, 1 = port 1, 2 = port 2
	}

	m_update_generator_settings = true;
}

float __fastcall CJanVNA2_comms::minOutputPowerdBm()
{
	return m_device_info_ok ? (float)m_device_info.limits_cdbm_min / CDBM_SCALE : DEFAULT_JANVNAV2_MIN_CDBM;
}

float __fastcall CJanVNA2_comms::maxOutputPowerdBm()
{
	return m_device_info_ok ? (float)m_device_info.limits_cdbm_max / CDBM_SCALE : DEFAULT_JANVNAV2_MAX_CDBM;
}

void __fastcall CJanVNA2_comms::setOutputPower(const float dBm)
{
	const int cdBm_max = m_device_info_ok ? m_device_info.limits_cdbm_max : IROUND(DEFAULT_JANVNAV2_MAX_CDBM * CDBM_SCALE);
	const int cdBm_min = m_device_info_ok ? m_device_info.limits_cdbm_min : IROUND(DEFAULT_JANVNAV2_MIN_CDBM * CDBM_SCALE);

	int power = IROUND(dBm * CDBM_SCALE);
	if (power < cdBm_min) power = cdBm_min;
	else
	if (power > cdBm_max) power = cdBm_max;

	m_output_power = power;

	m_own_sweep_settings.cdbm_excitation = m_output_power;
	m_own_generator_settings.cdbm_level  = m_output_power;

	if (connected)
	{
		switch (m_mode)
		{
			case MODE_INIT1:
			case MODE_INIT2:
			case MODE_IDLE:
			case MODE_POLL:
			case MODE_SINGLE_SCAN:
			case MODE_SCAN:
				m_update_sweep_settings = true;
				break;
			case MODE_GENERATOR:
				m_update_generator_settings = true;
				break;
			default:
				break;
		}
	}
}

String __fastcall CJanVNA2_comms::packetTypeToStr(const PacketType packet_type)
{
	switch (packet_type)
	{
		case PacketType::None:                     return "None";
		case PacketType::Datapoint:                return "Datapoint";
		case PacketType::SweepSettings:            return "SweepSettings";
		case PacketType::Status:                   return "Status";
		case PacketType::ManualControl:            return "ManualControl";
		case PacketType::DeviceInfo:               return "DeviceInfo";
		case PacketType::FirmwarePacket:           return "FirmwarePacket";
		case PacketType::Ack:                      return "Ack";
		case PacketType::ClearFlash:               return "ClearFlash";
		case PacketType::PerformFirmwareUpdate:    return "PerformFirmwareUpdate";
		case PacketType::Nack:                     return "Nack";
		case PacketType::Reference:                return "Reference";
		case PacketType::Generator:                return "Generator";
		case PacketType::SpectrumAnalyzerSettings: return "SpectrumAnalyzerSettings";
		case PacketType::SpectrumAnalyzerResult:   return "SpectrumAnalyzerResult";
		case PacketType::RequestDeviceInfo:        return "RequestDeviceInfo";
		default:                                   return "Unknown " + IntToStr((int)packet_type);
		};
}

String __fastcall CJanVNA2_comms::usbSpeedToStr(const int usb_speed)
{
	switch (usb_speed)
	{
		default:
		case LIBUSB_SPEED_UNKNOWN:    return "USB-Unknown";
		case LIBUSB_SPEED_LOW:        return "USB2.0_Low";
		case LIBUSB_SPEED_FULL:       return "USB2.0_Full";
		case LIBUSB_SPEED_HIGH:       return "USB2.0_High";
		case LIBUSB_SPEED_SUPER:      return "USB3.0";
		case LIBUSB_SPEED_SUPER_PLUS: return "USB3.0+";
	}
}

String __fastcall CJanVNA2_comms::usbClassToStr(const uint8_t usb_class)
{
	switch (usb_class)
	{
		case LIBUSB_CLASS_PER_INTERFACE:       return "CLASS_PER_INTERFACE";
		case LIBUSB_CLASS_AUDIO:               return "CLASS_AUDIO";
		case LIBUSB_CLASS_COMM:                return "CLASS_COMM";
		case LIBUSB_CLASS_HID:                 return "CLASS_HID";
		case LIBUSB_CLASS_PHYSICAL:            return "CLASS_PHYSICAL";
		case LIBUSB_CLASS_PRINTER:             return "CLASS_PRINTER";
//		case LIBUSB_CLASS_PTP:                 return "CLASS_PTP";
		case LIBUSB_CLASS_IMAGE:               return "CLASS_PTP/IMAGE";
		case LIBUSB_CLASS_MASS_STORAGE:        return "CLASS_MASS_STORAGE";
		case LIBUSB_CLASS_HUB:                 return "CLASS_HUB";
		case LIBUSB_CLASS_DATA:                return "CLASS_DATA";
		case LIBUSB_CLASS_SMART_CARD:          return "CLASS_SMART_CARD";
		case LIBUSB_CLASS_CONTENT_SECURITY:    return "CLASS_CONTENT_SECURITY";
		case LIBUSB_CLASS_VIDEO:               return "CLASS_VIDEO";
		case LIBUSB_CLASS_PERSONAL_HEALTHCARE: return "CLASS_PERSONAL_HEALTHCARE";
		case LIBUSB_CLASS_DIAGNOSTIC_DEVICE:   return "CLASS_DIAGNOSTIC_DEVICE";
		case LIBUSB_CLASS_WIRELESS:            return "CLASS_WIRELESS";
		case LIBUSB_CLASS_APPLICATION:         return "CLASS_APPLICATION";
		case LIBUSB_CLASS_VENDOR_SPEC:         return "CLASS_VENDOR_SPEC";
		default:                               return "CLASS_UNKNOWN";
	}
}

bool __fastcall CJanVNA2_comms::queueTxPacket(const PacketType packet_type, const void *data, const int data_size)
{
	if (m_device.hdev == NULL)
		return false;
	if (data_size < 0)
		return false;
	if (data_size > 0 && !data)
		return false;

	const int size = sizeof(t_janvna2_header) + data_size + sizeof(uint32_t);

	const int space = m_data_tx.buffer.size() - m_data_tx.index_wr;
	if (space < size)
	{
		if (Form1)
			Form1->pushCommMessage("tx: not enough buffer space");
		return false;
	}

	std::vector <uint8_t> packet(size);

	t_janvna2_header *p_header = (t_janvna2_header *)&packet[0];
	uint8_t *p_data            = &packet[sizeof(t_janvna2_header)];
	uint32_t *p_crc            = (uint32_t *)&packet[sizeof(t_janvna2_header) + data_size];

	p_header->magic        = header_magic;
	p_header->total_length = size;
	p_header->packet_type  = (uint8_t)packet_type;

	if (data_size > 0)
		memcpy(p_data, data, data_size);

	*p_crc = crc32(0x00000000, &packet[0], size - sizeof(uint32_t));

	m_ack_timer.mark();
	m_ack  = false;
	m_nack = false;

	memcpy(&m_data_tx.buffer[m_data_tx.index_wr], &packet[0], size);
	m_data_tx.index_wr += size;

	#if 1
		if (Form1)
		{
			String s;
			s.printf(L"tx: [%s] %d %08X %d/%u {", packetTypeToStr((PacketType)p_header->packet_type).c_str(), size, *p_crc, m_data_tx.index_wr, m_data_tx.buffer.size());
			for (unsigned int i = 0; i < packet.size(); i++)
				s += IntToHex(packet[i], 2) + " ";
			s = s.Trim() + "}";
			Form1->pushCommMessage(s);
		}
	#endif

	return true;
}

void __fastcall CJanVNA2_comms::sendRequestDeviceInfo()
{
	queueTxPacket(PacketType::RequestDeviceInfo, NULL, 0);
}

void __fastcall CJanVNA2_comms::sendSweepSettings(const bool idle)
{
	t_janvna2_sweepSettings sweep_settings;

	const uint64_t maxFreq = m_device_info_ok ? m_device_info.limits_maxFreq : MAX_VNA_JANVNAV2_FREQ_HZ;
	const uint64_t minFreq = m_device_info_ok ? m_device_info.limits_minFreq : MIN_VNA_JANVNAV2_FREQ_HZ;
	if (m_own_sweep_settings.f_start > maxFreq) m_own_sweep_settings.f_start = maxFreq;
	else
	if (m_own_sweep_settings.f_start < minFreq) m_own_sweep_settings.f_start = minFreq;
	if (m_own_sweep_settings.f_stop  > maxFreq) m_own_sweep_settings.f_stop  = maxFreq;
	else
	if (m_own_sweep_settings.f_stop  < minFreq) m_own_sweep_settings.f_stop  = minFreq;

	const int maxPoints = m_device_info_ok ? m_device_info.limits_maxPoints : DEFAULT_JANVNAV2_MAX_POINTS;
	const int minPoints = 2;
	if (m_own_sweep_settings.points > maxPoints) m_own_sweep_settings.points = maxPoints;
	else
	if (m_own_sweep_settings.points < minPoints) m_own_sweep_settings.points = minPoints;

	const uint32_t maxIFBW = m_device_info_ok ? m_device_info.limits_maxIFBW : DEFAULT_JANVNAV2_MAX_IF_BW;
	const uint32_t minIFBW = m_device_info_ok ? m_device_info.limits_minIFBW : DEFAULT_JANVNAV2_MIN_IF_BW;
	if (m_own_sweep_settings.if_bandwidth > maxIFBW) m_own_sweep_settings.if_bandwidth = maxIFBW;
	else
	if (m_own_sweep_settings.if_bandwidth < minIFBW) m_own_sweep_settings.if_bandwidth = minIFBW;

	const int cdBm_max = m_device_info_ok ? m_device_info.limits_cdbm_max : IROUND(DEFAULT_JANVNAV2_MAX_CDBM * CDBM_SCALE);
	const int cdBm_min = m_device_info_ok ? m_device_info.limits_cdbm_min : IROUND(DEFAULT_JANVNAV2_MIN_CDBM * CDBM_SCALE);
	if (m_own_sweep_settings.cdbm_excitation > cdBm_max) m_own_sweep_settings.cdbm_excitation = cdBm_max;
	else
	if (m_own_sweep_settings.cdbm_excitation < cdBm_min) m_own_sweep_settings.cdbm_excitation = cdBm_min;

	sweep_settings.f_start         = m_own_sweep_settings.f_start;
	sweep_settings.f_stop          = m_own_sweep_settings.f_stop;
	sweep_settings.points          = m_own_sweep_settings.points;
	sweep_settings.if_bandwidth    = m_own_sweep_settings.if_bandwidth;
	sweep_settings.cdbm_excitation = m_own_sweep_settings.cdbm_excitation;
	sweep_settings.excitePort1     = idle ? 0 : m_own_sweep_settings.excitePort1;	// '0' stops it sweeping
	sweep_settings.excitePort2     = idle ? 0 : m_own_sweep_settings.excitePort2;	// '0' stops it sweeping
	sweep_settings.suppressPeaks   = m_own_sweep_settings.suppressPeaks;

	if (temp_buf.size() < sizeof(t_janvna2_sweepSettings))
		temp_buf.resize(sizeof(t_janvna2_sweepSettings));	// just need to ensure the buffer is big enough to hold the final packet

	Encoder e(&temp_buf[0], temp_buf.size());
	e.add <uint64_t> (sweep_settings.f_start);
	e.add <uint64_t> (sweep_settings.f_stop);
	e.add <uint16_t> (sweep_settings.points);
	e.add <uint32_t> (sweep_settings.if_bandwidth);
	e.add <int16_t>  (sweep_settings.cdbm_excitation);
	e.addBits(sweep_settings.excitePort1, 1);
	e.addBits(sweep_settings.excitePort2, 1);
	e.addBits(sweep_settings.suppressPeaks, 1);

	const int data_size = e.getSize();

	queueTxPacket(PacketType::SweepSettings, &temp_buf[0], data_size);

	data_unit.m_bandwidth_Hz = sweep_settings.if_bandwidth;
	if (Form1)
	{
		String s;
		s = "tx: start           " + common.freqToStr1(sweep_settings.f_start, true, true, 6, false) + "Hz";
		Form1->pushCommMessage(s);
		s = "tx: stop            " + common.freqToStr1(sweep_settings.f_stop, true, true, 6, false) + "Hz";
		Form1->pushCommMessage(s);
		Form1->printfCommMessage("tx: points          %u", sweep_settings.points);
		Form1->printfCommMessage("tx: if_bandwidth    %u", sweep_settings.if_bandwidth);
		Form1->printfCommMessage("tx: cdbm_excitation %d", sweep_settings.cdbm_excitation);
		Form1->printfCommMessage("tx: excitePort1     %u", sweep_settings.excitePort1);
		Form1->printfCommMessage("tx: excitePort2     %u", sweep_settings.excitePort2);
		Form1->printfCommMessage("tx: suppressPeaks   %u", sweep_settings.suppressPeaks);

		::PostMessage(Form1->Handle, WM_UPDATE_POINT_BANDWIDTH, data_unit.m_bandwidth_Hz, 0);
	}
}

void __fastcall CJanVNA2_comms::sendGeneratorSettings(const bool enable)
{
	t_janvna2_generatorSettings generator_settings;

	const uint64_t maxFreq = m_device_info_ok ? m_device_info.limits_maxFreq : MAX_VNA_JANVNAV2_FREQ_HZ;
	const uint64_t minFreq = m_device_info_ok ? m_device_info.limits_minFreq : MIN_VNA_JANVNAV2_FREQ_HZ;
	if (m_own_generator_settings.frequency > maxFreq) m_own_generator_settings.frequency = maxFreq;
	else
	if (m_own_generator_settings.frequency < minFreq) m_own_generator_settings.frequency = minFreq;

	const int cdBm_max = m_device_info_ok ? m_device_info.limits_cdbm_max : IROUND(DEFAULT_JANVNAV2_MAX_CDBM * CDBM_SCALE);
	const int cdBm_min = m_device_info_ok ? m_device_info.limits_cdbm_min : IROUND(DEFAULT_JANVNAV2_MIN_CDBM * CDBM_SCALE);
	if (m_own_generator_settings.cdbm_level > cdBm_max) m_own_generator_settings.cdbm_level = cdBm_max;
	else
	if (m_own_generator_settings.cdbm_level < cdBm_min) m_own_generator_settings.cdbm_level = cdBm_min;

	generator_settings.frequency  = m_own_generator_settings.frequency;
	generator_settings.cdbm_level = m_own_generator_settings.cdbm_level;
	generator_settings.activePort = enable ? m_own_generator_settings.activePort : 0;

	if (temp_buf.size() < sizeof(t_janvna2_generatorSettings))
		temp_buf.resize(sizeof(t_janvna2_generatorSettings));	// just need to ensure the buffer is big enough to hold the final packet

	Encoder e(&temp_buf[0], temp_buf.size());
	e.add <uint64_t> (generator_settings.frequency);
	e.add <int16_t>  (generator_settings.cdbm_level);
	e.add <uint8_t>  (generator_settings.activePort);

	const int data_size = e.getSize();

	queueTxPacket(PacketType::Generator, &temp_buf[0], data_size);

	if (Form1)
	{
		String s;
		s = "tx: frequency     " + common.freqToStr1(generator_settings.frequency, true, true, 6, false) + "Hz";
		Form1->pushCommMessage(s);
		Form1->printfCommMessage("tx: cdbm_level    %d", generator_settings.cdbm_level);
		Form1->printfCommMessage("tx: activePort    %u", generator_settings.activePort);
	}
}

void __fastcall CJanVNA2_comms::processRxPacket(const t_packet_info *p_packet)
{
	String s;

	if (p_packet == NULL)
		return;

//	const uint8_t *packet          = (uint8_t *)p_packet;
	const t_janvna2_header *header = (t_janvna2_header *)&p_packet->header;
	const uint8_t *data            = (uint8_t *)&p_packet->data;
	const int data_size            = header->total_length - sizeof(t_janvna2_header) - sizeof(uint32_t);

	#if 0
		if (Form1)
		{
			if (header->packet_type != PacketType::Datapoint)
			{
				String s;
				s.printf(L"rx: [%s] %d %08X {", packetTypeToStr(header->packet_type).c_str(), data_size, p_packet->crc);
				for (unsigned int i = 0; i < header->total_length; i++)
					s += IntToHex(packet[i], 2) + " ";
				s = s.Trim() + "}";
				Form1->pushCommMessage(s);
				s = "";
			}
		}
	#endif

	switch ((PacketType)header->packet_type)
	{
		case PacketType::None:
			break;

		case PacketType::Datapoint:
			if (data_size >= (int)sizeof(t_janvna2_datapoint))
			{
				t_janvna2_datapoint data_point;

				memcpy(&data_point, data, sizeof(t_janvna2_datapoint));
/*
				Decoder e(data);
				e.get<float>(data_point.real_S11);
				e.get<float>(data_point.imag_S11);
				e.get<float>(data_point.real_S21);
				e.get<float>(data_point.imag_S21);
				e.get<float>(data_point.real_S12);
				e.get<float>(data_point.imag_S12);
				e.get<float>(data_point.real_S22);
				e.get<float>(data_point.imag_S22);
				e.get<uint64_t>(data_point.Hz);
				e.get<uint16_t>(data_point.pointNum);
*/
				t_data_point fp;

				if (m_mode == MODE_SINGLE_SCAN && m_num_frames > 0)
					break;
				if (m_new_mode != MODE_SINGLE_SCAN && m_new_mode != MODE_SCAN)
					break;

				#if 0
					s.printf(L"rx: data_point: %llu Hz, %u   S11 %9.6f %9.6f   S21 %9.6f %9.6f   S12 %9.6f %9.6f   S22 %9.6f %9.6f",
										data_point.Hz,
										data_point.pointNum,
										data_point.real_S11, data_point.imag_S11,
										data_point.real_S21, data_point.imag_S21,
										data_point.real_S12, data_point.imag_S12,
										data_point.real_S22, data_point.imag_S22);
					common.logFileAppend(s + "\n");
					s = "";
				#endif

				if (data_point.pointNum == 0)
				{	// first point .. clear the buffers

					m_prev_index = -1;

					if ((int)data_unit.m_point_incoming.size() != m_own_sweep_settings.points)
						data_unit.m_point_incoming.resize(m_own_sweep_settings.points);

					#if 0
						// clear the buffer
						for (unsigned int i = 0; i < data_unit.m_point_incoming.size(); i++)
							data_unit.m_point_incoming[i] = fp;
					#endif

					if (Form1)
					{
						// global buffer
						if ((int)Form1->m_freq_data_list.size() != m_own_sweep_settings.points)
						{
							Form1->m_freq_data_list.resize(m_own_sweep_settings.points);
							for (unsigned int i = 0; i < Form1->m_freq_data_list.size(); i++)
								Form1->m_freq_data_list[i] = fp;
						}
					}
			  }

				const bool point_is_good = (data_point.pointNum == (m_prev_index + 1)) ? true : false;

				// save the new point into the incoming buffer
				if (data_point.pointNum < (int)data_unit.m_point_incoming.size())
				{
					#if 0
						if (!point_is_good)
						{	// set the lost points data to 0,0
							const complexf cpx(0);
							for (int i = m_prev_index + 1; i < (int)data_point.pointNum; i++)
							{
								data_unit.m_point_incoming[i].s11 = cpx;
								data_unit.m_point_incoming[i].s21 = cpx;
								data_unit.m_point_incoming[i].s12 = cpx;
								data_unit.m_point_incoming[i].s22 = cpx;
							}
							m_prev_index = data_point.pointNum;
						}
					#endif

					fp.Hz  = data_point.Hz;
					fp.s11 = complexf(data_point.real_S11, data_point.imag_S11);
					fp.s21 = complexf(data_point.real_S21, data_point.imag_S21);
					fp.s12 = complexf(data_point.real_S12, data_point.imag_S12);
					fp.s22 = complexf(data_point.real_S22, data_point.imag_S22);
					data_unit.m_point_incoming[data_point.pointNum] = fp;

					#if 0
						s.printf(L"rx: data_point: %llu Hz, %u   S11 %9.6f %9.6f   S21 %9.6f %9.6f   S12 %9.6f %9.6f   S22 %9.6f %9.6f",
								fp.Hz, data_point.pointNum, fp.s11.re, fp.s11.im, fp.s21.re, fp.s21.im, fp.s12.re, fp.s12.im, fp.s22.re, fp.s22.im);
						Form1->pushCommMessage("rx: " + s);
                  s = "";
					#endif

					if (point_is_good)
						m_prev_index = data_point.pointNum;
				}
				else
				{
				}

				if (Form1 && m_new_points_timer.millisecs() >= 100)
				{	// provide some user feedback so that they know it's working
					m_new_points_timer.mark();
					::PostMessage(Form1->Handle, WM_INCOMING_POINTS, (WPARAM)data_point.pointNum, 0);
				}

				if (m_own_sweep_settings.points > 0 && m_own_sweep_settings.points == (int)data_unit.m_point_incoming.size())
				{
					if (data_point.pointNum >= ((int)data_unit.m_point_incoming.size() - 1))
					{	// last point

//						if (point_is_good)
//						{
							if (Form1)
							{
								Form1->addNewRxData(data_unit.m_point_incoming);
								//data_unit.m_point_incoming.resize(0);
							}
//						}

						if (!point_is_good)
						{
							String s;
							//s.printf(L"rx: ****** missed points [index %d]", m_prev_index + 1);
							s = "rx: ****** missed points";
							Form1->pushCommMessage(s);
						}

						m_prev_index = -1;

						//data_unit.m_point_incoming.resize(0);

						if (m_mode == MODE_SINGLE_SCAN)
						{	// stop the sweep
							m_new_mode = MODE_IDLE;
							if (Form1)
								::PostMessage(Form1->Handle, WM_CAPTURE_STOP, m_num_frames, 0);
						}

						m_num_frames++;
					}
				}
			}
			break;

		case PacketType::SweepSettings:
//			if (data_size >= sizeof(m_sweep_settings))
			{
				Decoder e(data);
				e.get<uint64_t>(m_sweep_settings.f_start);
				e.get<uint64_t>(m_sweep_settings.f_stop);
				e.get<uint16_t>(m_sweep_settings.points);
				e.get<uint32_t>(m_sweep_settings.if_bandwidth);
				e.get<int16_t>(m_sweep_settings.cdbm_excitation);
				m_sweep_settings.excitePort1   = e.getBits(1);
				m_sweep_settings.excitePort2   = e.getBits(1);
				m_sweep_settings.suppressPeaks = e.getBits(1);

				data_unit.m_vna_data.freq_start_Hz = m_sweep_settings.f_start;
				data_unit.m_vna_data.freq_stop_Hz  = m_sweep_settings.f_stop;
				data_unit.m_vna_data.num_points    = m_sweep_settings.points;
				data_unit.m_vna_data.bandwidth_Hz  = m_sweep_settings.if_bandwidth;
				data_unit.m_vna_data.power         = m_sweep_settings.cdbm_excitation;

				if (Form1)
				{
					s = "";
					String s2;
					s2.printf(L"rx: sweep_settings: start           %llu Hz\n", m_sweep_settings.f_start); s += s2;
					s2.printf(L"rx: sweep_settings: stop            %llu Hz\n", m_sweep_settings.f_stop); s += s2;
					s2.printf(L"rx: sweep_settings: points          %u\n", m_sweep_settings.points); s += s2;
					s2.printf(L"rx: sweep_settings: if_bandwidth    %u Hz\n", m_sweep_settings.if_bandwidth); s += s2;
					s2.printf(L"rx: sweep_settings: cdbm_excitation %0.2f dBm\n", (float)m_sweep_settings.cdbm_excitation / CDBM_SCALE); s += s2;
					s2.printf(L"rx: sweep_settings: excitePort1     %u\n", m_sweep_settings.excitePort1); s += s2;
					s2.printf(L"rx: sweep_settings: excitePort2     %u\n", m_sweep_settings.excitePort2); s += s2;
					s2.printf(L"rx: sweep_settings: suppressPeaks   %u\n", m_sweep_settings.suppressPeaks); s += s2;
					Form1->pushCommMessage("rx: " + s);
					s = "";
				}

				if (Form1)
				{
//					data_unit.m_vna_data.bandwidth_Hz = m_sweep_settings.if_bandwidth;
//					::PostMessage(Form1->Handle, WM_UPDATE_POINT_BANDWIDTH, m_sweep_settings.if_bandwidth, 0);
				}
			}
			break;

		case PacketType::Status:
//			if (data_size >= sizeof(m_status))
			{
				// port1min      -4, port1max 5
				// port2min      1, port2max 5
				// refmin        1, refmax 8
				// port1real     15.069, port1imag 11.334
				// port2real     20.405, port2imag -47.063
				// refreal       -5.006, refimag -19.664
				// temp_source   22
				// temp_LO       95
				// source_locked 1
				// LO_locked     1

				Decoder e(data);
				e.get<int16_t>(m_status.port1min);
				e.get<int16_t>(m_status.port1max);
				e.get<int16_t>(m_status.port2min);
				e.get<int16_t>(m_status.port2max);
				e.get<int16_t>(m_status.refmin);
				e.get<int16_t>(m_status.refmax);
				e.get<float>(m_status.port1real);
				e.get<float>(m_status.port1imag);
				e.get<float>(m_status.port2real);
				e.get<float>(m_status.port2imag);
				e.get<float>(m_status.refreal);
				e.get<float>(m_status.refimag);
				e.get<uint8_t>(m_status.temp_source);
				e.get<uint8_t>(m_status.temp_LO);
				m_status.source_locked = e.getBits(1);
				m_status.LO_locked     = e.getBits(1);

				if (Form1 && m_mode != MODE_GENERATOR)
				{
					s = "";
					String s2;
					s2.printf(L"rx: status: port1min      %d, port1max %d\n", m_status.port1min, m_status.port1max); s += s2;
					s2.printf(L"rx: status: port2min      %d, port2max %d\n", m_status.port2min, m_status.port2max); s += s2;
					s2.printf(L"rx: status: refmin        %d, refmax %d\n", m_status.refmin, m_status.refmax); s += s2;
					s2.printf(L"rx: status: port1real     %0.3f, port1imag %0.3f\n", m_status.port1real, m_status.port1imag); s += s2;
					s2.printf(L"rx: status: port2real     %0.3f, port2imag %0.3f\n", m_status.port2real, m_status.port2imag); s += s2;
					s2.printf(L"rx: status: refreal       %0.3f, refimag %0.3f\n", m_status.refreal, m_status.refimag); s += s2;
					s2.printf(L"rx: status: temp_source   %u\n", m_status.temp_source); s += s2;
					s2.printf(L"rx: status: temp_LO       %u\n", m_status.temp_LO); s += s2;
					s2.printf(L"rx: status: source_locked %u\n", m_status.source_locked); s += s2;
					s2.printf(L"rx: status: LO_locked     %u\n", m_status.LO_locked); s += s2;
					Form1->pushCommMessage(s);
					s = "";
				}
			}
			break;

		case PacketType::ManualControl:
//			if (data_size >= sizeof(m_manual_control))
			{
				Decoder e(data);
				m_manual_control.SourceHighCE      = e.getBits(1);
				m_manual_control.SourceHighRFEN    = e.getBits(1);
				m_manual_control.SourceHighPower   = e.getBits(2);
				m_manual_control.SourceHighLowpass = e.getBits(2);
				e.get<uint64_t>(m_manual_control.SourceHighFrequency);
				m_manual_control.SourceLowEN       = e.getBits(1);
				m_manual_control.SourceLowPower    = e.getBits( 2);
				e.get<uint32_t>(m_manual_control.SourceLowFrequency);
				m_manual_control.attenuator        = e.getBits(7);
				m_manual_control.SourceHighband    = e.getBits(1);
				m_manual_control.AmplifierEN       = e.getBits(1);
				m_manual_control.PortSwitch        = e.getBits(1);
				m_manual_control.LO1CE             = e.getBits(1);
				m_manual_control.LO1RFEN           = e.getBits(1);
				e.get<uint64_t>(m_manual_control.LO1Frequency);
				m_manual_control.LO2EN             = e.getBits(1);
				e.get<uint32_t>(m_manual_control.LO2Frequency);
				m_manual_control.Port1EN           = e.getBits(1);
				m_manual_control.Port2EN           = e.getBits(1);
				m_manual_control.RefEN             = e.getBits(1);
				e.get<uint32_t>(m_manual_control.Samples);
				m_manual_control.WindowType        = e.getBits(2);

				if (Form1)
				{
					s = "";
					String s2;
					s2.printf(L"rx: manual_control: SourceHighCE        %u\n", m_manual_control.SourceHighCE); s += s2;
					s2.printf(L"rx: manual_control: SourceHighRFEN      %u\n", m_manual_control.SourceHighRFEN); s += s2;
					s2.printf(L"rx: manual_control: SourceHighPower     %u\n", m_manual_control.SourceHighPower); s += s2;
					s2.printf(L"rx: manual_control: SourceHighLowpass   %u\n", m_manual_control.SourceHighLowpass); s += s2;
					s2.printf(L"rx: manual_control: SourceHighFrequency %u\n", m_manual_control.SourceHighFrequency); s += s2;
					s2.printf(L"rx: manual_control: SourceLowEN         %u\n", m_manual_control.SourceLowEN); s += s2;
					s2.printf(L"rx: manual_control: SourceLowPower      %u\n", m_manual_control.SourceLowPower); s += s2;
					s2.printf(L"rx: manual_control: SourceLowFrequency  %u\n", m_manual_control.SourceLowFrequency); s += s2;
					s2.printf(L"rx: manual_control: attenuator          %u\n", m_manual_control.attenuator); s += s2;
					s2.printf(L"rx: manual_control: SourceHighband      %u\n", m_manual_control.SourceHighband); s += s2;
					s2.printf(L"rx: manual_control: AmplifierEN         %u\n", m_manual_control.AmplifierEN); s += s2;
					s2.printf(L"rx: manual_control: PortSwitch          %u\n", m_manual_control.PortSwitch); s += s2;
					s2.printf(L"rx: manual_control: LO1CE               %u\n", m_manual_control.LO1CE); s += s2;
					s2.printf(L"rx: manual_control: LO1RFEN             %u\n", m_manual_control.LO1RFEN); s += s2;
					s2.printf(L"rx: manual_control: LO1Frequency        %u\n", m_manual_control.LO1Frequency); s += s2;
					s2.printf(L"rx: manual_control: LO2EN               %u\n", m_manual_control.LO2EN); s += s2;
					s2.printf(L"rx: manual_control: LO2Frequency        %u\n", m_manual_control.LO2Frequency); s += s2;
					s2.printf(L"rx: manual_control: Port1EN             %u\n", m_manual_control.Port1EN); s += s2;
					s2.printf(L"rx: manual_control: Port2EN             %u\n", m_manual_control.Port2EN); s += s2;
					s2.printf(L"rx: manual_control: RefEN               %u\n", m_manual_control.RefEN); s += s2;
					s2.printf(L"rx: manual_control: Samples             %u\n", m_manual_control.Samples); s += s2;
					s2.printf(L"rx: manual_control: WindowType          %u\n", m_manual_control.WindowType); s += s2;
					Form1->pushCommMessage(s);
					s = "";
				}
			}
			break;

		case PacketType::DeviceInfo:
			//if (data_size >= sizeof(m_device_info))
			{
				//const t_janvna2_deviceInfo device_info = m_device_info;

				Decoder e(data);
				e.get(m_device_info.ProtocolVersion);
				e.get(m_device_info.FW_major);
				e.get(m_device_info.FW_minor);
				e.get(m_device_info.FW_patch);
				e.get(m_device_info.HW_Revision);
				m_device_info.extRefAvailable = e.getBits(1);
				m_device_info.extRefInUse     = e.getBits(1);
				m_device_info.FPGA_configured = e.getBits(1);
				m_device_info.source_locked   = e.getBits(1);
				m_device_info.LO1_locked      = e.getBits(1);
				m_device_info.ADC_overload    = e.getBits(1);
				e.get(m_device_info.temp_source);
				e.get(m_device_info.temp_LO1);
				e.get(m_device_info.temp_MCU);
				e.get(m_device_info.limits_minFreq);
				e.get(m_device_info.limits_maxFreq);
				e.get(m_device_info.limits_minIFBW);
				e.get(m_device_info.limits_maxIFBW);
				e.get(m_device_info.limits_maxPoints);
				e.get(m_device_info.limits_cdbm_min);
				e.get(m_device_info.limits_cdbm_max);
				e.get(m_device_info.limits_minRBW);
				e.get(m_device_info.limits_maxRBW);

				//memcpy(&m_device_info, &p_packet_info->device_info, sizeof(m_device_info));

				//const bool changed = (memcmp(&m_device_info, &device_info, sizeof(t_janvna2_deviceInfo)) != 0) ? true : false;
				const bool changed = false;

				if (!m_device_info_ok)
				{	// first one
					data_unit.m_vna_data.max_points = m_device_info.limits_maxPoints;

					data_unit.m_vna_data.version.printf(L"v%u.%u patch:%u hw_rev:%c\n", m_device_info.FW_major, m_device_info.FW_minor, m_device_info.FW_patch, m_device_info.HW_Revision);

					data_unit.m_vna_data.info.resize(0);
					data_unit.m_vna_data.info.push_back(data_unit.m_vna_data.name);
					s.printf(L"protocol   %u", m_device_info.ProtocolVersion);
					data_unit.m_vna_data.info.push_back(s);
					s.printf(L"hardware   %c", m_device_info.HW_Revision);
					data_unit.m_vna_data.info.push_back(s);
					s.printf(L"firmware   v%u.%u patch:%u", m_device_info.FW_major, m_device_info.FW_minor, m_device_info.FW_patch);
					data_unit.m_vna_data.info.push_back(s);

					data_unit.m_vna_data.freq_max_Hz = m_device_info.limits_maxFreq;
					data_unit.m_vna_data.freq_min_Hz = m_device_info.limits_minFreq;

					data_unit.m_vna_data.max_bandwidth_Hz = m_device_info.limits_maxIFBW;
					//data_unit.m_vna_data.max_bandwidth_Hz = m_device_info.limits_maxRBW;

					data_unit.m_vna_data.freq_start_Hz = m_device_info.limits_maxFreq;
					data_unit.m_vna_data.freq_stop_Hz  = m_device_info.limits_minFreq;
				}

				if (Form1 && (!m_device_info_ok || changed || (m_mode != MODE_SINGLE_SCAN && m_mode != MODE_SCAN)))
				{
					s = "";
					String s2;
					s2.printf(L"rx: device_info: protocol          %u\n", m_device_info.ProtocolVersion); s += s2;
					s2.printf(L"rx: device_info: firmware version  v%u.%u\n", m_device_info.FW_major, m_device_info.FW_minor); s += s2;
					s2.printf(L"rx: device_info: firmware patch    %u\n", m_device_info.FW_patch); s += s2;
					s2.printf(L"rx: device_info: hardware revision %c\n", m_device_info.HW_Revision); s += s2;
					s2.printf(L"rx: device_info: extRefAvailable   %u\n", m_device_info.extRefAvailable); s += s2;
					s2.printf(L"rx: device_info: extRefInUse       %u\n", m_device_info.extRefInUse); s += s2;
					s2.printf(L"rx: device_info: FPGA_configured   %u\n", m_device_info.FPGA_configured); s += s2;
					s2.printf(L"rx: device_info: source_locked     %u\n", m_device_info.source_locked); s += s2;
					s2.printf(L"rx: device_info: LO1_locked        %u\n", m_device_info.LO1_locked); s += s2;
					s2.printf(L"rx: device_info: ADC_overload      %u\n", m_device_info.ADC_overload); s += s2;
					s2.printf(L"rx: device_info: temp_source       %u\n", m_device_info.temp_source); s += s2;
					s2.printf(L"rx: device_info: temp_LO1          %u\n", m_device_info.temp_LO1); s += s2;
					s2.printf(L"rx: device_info: temp_MCU          %u\n", m_device_info.temp_MCU); s += s2;
					s2.printf(L"rx: device_info: limits_minFreq    %llu\n", m_device_info.limits_minFreq); s += s2;
					s2.printf(L"rx: device_info: limits_maxFreq    %llu\n", m_device_info.limits_maxFreq); s += s2;
					s2.printf(L"rx: device_info: limits_minIFBW    %u\n", m_device_info.limits_minIFBW); s += s2;
					s2.printf(L"rx: device_info: limits_maxIFBW    %u\n", m_device_info.limits_maxIFBW); s += s2;
					s2.printf(L"rx: device_info: limits_maxPoints  %u\n", m_device_info.limits_maxPoints); s += s2;
					s2.printf(L"rx: device_info: limits_cdbm_min   %d\n", m_device_info.limits_cdbm_min); s += s2;
					s2.printf(L"rx: device_info: limits_cdbm_max   %d\n", m_device_info.limits_cdbm_max); s += s2;
					s2.printf(L"rx: device_info: limits_minRBW     %u\n", m_device_info.limits_minRBW); s += s2;
					s2.printf(L"rx: device_info: limits_maxRBW     %u\n", m_device_info.limits_maxRBW); s += s2;
					Form1->pushCommMessage(s);
					s = "";
				}

				if (!m_device_info_ok)
				{
					m_device_info_ok = true;

					// tell the exec we have found a VNA
					if (Form1)
						::PostMessage(Form1->Handle, WM_NEW_UNIT_TYPE, 0, 0);
				}

				m_ack = true;
			}
			break;

		case PacketType::FirmwarePacket:
			if (data_size >= (int)sizeof(m_firmware_packet))
			{
				memcpy(&m_firmware_packet, data, sizeof(m_firmware_packet));
				if (Form1)
				{
					s.printf(L"rx: firmware_packet: 0x%08X ", m_firmware_packet.address);
					for (unsigned int i = 0; i < sizeof(m_firmware_packet.data); i++)
					  s += " " + IntToHex(m_firmware_packet.data[i], 2);
					Form1->pushCommMessage(s);
					s = "";
				}
			}
			break;

		case PacketType::Ack:
			// no payload
			m_ack = true;
			Form1->pushCommMessage("rx: Ack");
			break;

		case PacketType::ClearFlash:
			// no payload
			Form1->pushCommMessage("rx: ClearFlash");
			break;

		case PacketType::PerformFirmwareUpdate:
			// no payload
			Form1->pushCommMessage("rx: PerformFirmwareUpdate");
			break;

		case PacketType::Nack:
			// no payload
			m_nack = true;
			Form1->pushCommMessage("rx: Nack");
			break;

		case PacketType::Reference:
//			if (data_size >= sizeof(m_reference_settings))
			{
				Decoder e(data);
				e.get<uint32_t>(m_reference_settings.ExtRefOuputFreq);
				m_reference_settings.AutomaticSwitch = e.getBits(1);
				m_reference_settings.UseExternalRef  = e.getBits(1);

				if (Form1)
				{
					s = "";
					String s2;
					s2.printf(L"rx: Reference: ExtRefOuputFreq %u Hz\n", m_reference_settings.ExtRefOuputFreq); s += s2;
					s2.printf(L"rx: Reference: AutomaticSwitch %u\n", m_reference_settings.AutomaticSwitch); s += s2;
					s2.printf(L"rx: Reference: UseExternalRef  %u\n", m_reference_settings.UseExternalRef); s += s2;
					Form1->pushCommMessage(s);
					s = "";
				}
			}
			break;

		case PacketType::Generator:
			if (data_size >= (int)sizeof(m_generator_settings))
			{
				memcpy(&m_generator_settings, data, sizeof(m_generator_settings));
/*
				Decoder e(data);
				e.get<uint64_t>(m_generator_settings.frequency);
				e.get<int16_t>(m_generator_settings.cdbm_level);
				e.get<uint8_t>(m_generator_settings.activePort);
*/
				if (Form1)
				{
					s = "";
					String s2;
					s2.printf(L"rx: Generator: frequency  %llu Hz\n", m_generator_settings.frequency); s += s2;
					s2.printf(L"rx: Generator: cdbm_level %d\n", m_generator_settings.cdbm_level); s += s2;
					s2.printf(L"rx: Generator: activePort %u\n", m_generator_settings.activePort); s += s2;
					Form1->pushCommMessage(s);
					s = "";
				}
			}
			break;

		case PacketType::SpectrumAnalyzerSettings:
//			if (data_size >= sizeof(m_spectrum_analyzer_settings))
			{
				Decoder e(data);
				e.get<uint64_t>(m_spectrum_analyzer_settings.f_start);
				e.get<uint64_t>(m_spectrum_analyzer_settings.f_stop);
				e.get<uint32_t>(m_spectrum_analyzer_settings.RBW);
				e.get<uint16_t>(m_spectrum_analyzer_settings.pointNum);
				m_spectrum_analyzer_settings.WindowType = e.getBits(2);
				m_spectrum_analyzer_settings.SignalID   = e.getBits(1);
				m_spectrum_analyzer_settings.Detector   = e.getBits(3);
				m_spectrum_analyzer_settings.UseDFT     = e.getBits(1);

				if (Form1)
				{
					s = "";
					String s2;
					s2.printf(L"rx: SpectrumAnalyzerSettings: f_start    %llu Hz\n", m_spectrum_analyzer_settings.f_start); s += s2;
					s2.printf(L"rx: SpectrumAnalyzerSettings: f_stop     %llu Hz\n", m_spectrum_analyzer_settings.f_stop); s += s2;
					s2.printf(L"rx: SpectrumAnalyzerSettings: RBW        %u Hz\n", m_spectrum_analyzer_settings.RBW); s += s2;
					s2.printf(L"rx: SpectrumAnalyzerSettings: pointNum   %u\n", m_spectrum_analyzer_settings.pointNum); s += s2;
					s2.printf(L"rx: SpectrumAnalyzerSettings: WindowType %u\n", m_spectrum_analyzer_settings.WindowType); s += s2;
					s2.printf(L"rx: SpectrumAnalyzerSettings: SignalID   %u\n", m_spectrum_analyzer_settings.SignalID); s += s2;
					s2.printf(L"rx: SpectrumAnalyzerSettings: Detector   %u\n", m_spectrum_analyzer_settings.Detector); s += s2;
					s2.printf(L"rx: SpectrumAnalyzerSettings: UseDFT     %u\n", m_spectrum_analyzer_settings.UseDFT); s += s2;
					Form1->pushCommMessage(s);
					s = "";
				}
			}
			break;

		case PacketType::SpectrumAnalyzerResult:
			if (data_size >= (int)sizeof(m_spectrum_analyzer_result))
			{
				memcpy(&m_spectrum_analyzer_result, data, sizeof(m_spectrum_analyzer_result));
/*
				Decoder e(data);
				e.get<float>(m_spectrum_analyzer_result.port1);
				e.get<float>(m_spectrum_analyzer_result.port2);
				e.get<uint64_t>(m_spectrum_analyzer_result.frequency);
				e.get<uint16_t>(m_spectrum_analyzer_result.pointNum);
*/
				if (Form1)
				{
					s = "";
					String s2;
					s2.printf(L"rx: SpectrumAnalyzerResult: port1     %0.3f\n", m_spectrum_analyzer_result.port1); s += s2;
					s2.printf(L"rx: SpectrumAnalyzerResult: port2     %0.3f\n", m_spectrum_analyzer_result.port2); s += s2;
					s2.printf(L"rx: SpectrumAnalyzerResult: frequency %llu Hz\n", m_spectrum_analyzer_result.frequency); s += s2;
					s2.printf(L"rx: SpectrumAnalyzerResult: pointNum  %u\n", m_spectrum_analyzer_result.pointNum); s += s2;
					Form1->pushCommMessage(s);
					s = "";
				}
			}
			break;

		case PacketType::RequestDeviceInfo:
			// no payload
			Form1->pushCommMessage("rx: RequestDeviceInfo");
			break;

		default:
			Form1->pushCommMessage("rx: Unknown packet");
			break;
	}
}

void __fastcall CJanVNA2_comms::threadEventsProcess()
{
	if (m_events_thread == NULL)
		return;
	if (m_events_thread->CheckTerminated())
		return;

	if ((m_device.hdev == NULL || m_mode <= MODE_IDLE || m_mode == MODE_POLL) && m_new_mode != MODE_SINGLE_SCAN && m_new_mode != MODE_SCAN)
	{	// not doing anything that needs the speed
		m_events_thread->Sleep(5);
		if (m_events_thread->Priority != tpIdle)
			m_events_thread->Priority = tpIdle;	// tpIdle tpLowest tpNormal tpHighest tpTimeCritical
	}
	else
	{	// we need to poll the VNA no less than once every 16ms
		m_events_thread->Sleep(1);
		if (m_events_thread->Priority != m_events_thread_priority)
			m_events_thread->Priority = m_events_thread_priority;
	}

	struct timeval tv = {0, 1000};
	const int res = libusb_handle_events_timeout_completed(m_device.ctx, &tv, NULL);
	if (res < 0)
	{
		const char *str = libusb_strerror(res);
		String s = "rx: libusb_handle_events_timeout_completed error";
		if (str != NULL)
			s += ": " + String(str);
		Form1->pushCommMessage(s);
	}
}

void __fastcall CJanVNA2_comms::threadProcess()
{
	int res;

	if (m_thread == NULL || m_device.hdev == NULL || m_mode == MODE_NONE)
		return;
	if (m_thread->CheckTerminated())
		return;

//	if (m_mode != MODE_SINGLE_SCAN && m_mode != MODE_SCAN && m_mode != MODE_GENERATOR)
//		m_thread->Sleep(10);

	// TX data to the VNA
	if (m_thread != NULL && m_device.hdev != NULL && m_mode != MODE_NONE)
	{
		const int available = m_data_tx.index_wr;
		if (available > 0)
		{
			int num_wrote = 0;
			try
			{
				res = libusb_bulk_transfer(m_device.hdev, EP_Data_Out_Addr, &m_data_tx.buffer[0], available, &num_wrote, 100);
				if (res < 0)
					num_wrote = res;
			}
			catch (Exception &exception)
			{
				if (Form1)
					Form1->pushCommMessage("bulk_write data " + exception.ToString());
				if (num_wrote > 0)
					num_wrote = 0;
			}

			if (num_wrote > 0)
			{	// remove the sent data from the buffer
				const int num = available - num_wrote;
				if (num > 0)
					memmove(&m_data_tx.buffer[0], &m_data_tx.buffer[num_wrote], sizeof(m_data_tx.buffer[0]) * num);
				m_data_tx.index_wr = (num > 0) ? num : 0;
			}
			else
			{
				if (num_wrote < 0 && num_wrote != -116 && num_wrote != LIBUSB_ERROR_TIMEOUT)
				{	// error
					switch (num_wrote)
					{
						case LIBUSB_ERROR_IO:
						case LIBUSB_ERROR_INVALID_PARAM:
						case LIBUSB_ERROR_ACCESS:
						case LIBUSB_ERROR_NO_DEVICE:
						case LIBUSB_ERROR_NOT_FOUND:
						//case LIBUSB_ERROR_OVERFLOW:
						case LIBUSB_ERROR_PIPE:
						//case LIBUSB_ERROR_INTERRUPTED:
						//case LIBUSB_ERROR_NO_MEM:
						case LIBUSB_ERROR_NOT_SUPPORTED:
							closeDevice(true);
							break;
						default:
							{
								const char *str = libusb_strerror(num_wrote);
								String s;
								s.printf(L"error: libusb_bulk_transfer(write) .. [%d]", num_wrote);
								if (str)
									s += " " + String(str);
								if (Form1)
									Form1->pushCommMessage(s);
							}
							break;
					}
				}
			}
		}
	}

	if (m_thread != NULL && m_device.hdev != NULL && m_mode != MODE_NONE)
	{
//		m_poll_timer.mark();

		switch (m_mode)
		{
			case MODE_NONE:
				m_new_mode = MODE_NONE;
				m_mode     = MODE_INIT1;
				break;

			case MODE_INIT1:
				if (m_retries <= 0)
				{
					data_unit.m_point_incoming.resize(0);

					sendSweepSettings(true);	// stop sweeping
					m_ack_timer.mark();
					m_ack     = false;
					m_nack    = false;
					m_retries = 1;
				}
				else
				if (m_ack)
				{	// received the ack
					m_ack     = false;
					m_nack    = false;
					m_retries = 0;
					m_mode    = MODE_INIT2;
					m_update_sweep_settings = false;
				}
				else
				if (m_nack)
				{	// received a NACK
					m_nack = false;
					closeDevice(true);
				}
				else
				if (m_ack_timer.millisecs() >= m_ack_timeout_ms)
				{	// no ack/nack received
					m_ack_timer.mark();
					if (m_retries >= m_max_retries)
					{
						closeDevice(true);
					}
					else
					{
						sendSweepSettings(true);	// stop sweeping
						m_retries++;
					}
				}
				break;

			case MODE_INIT2:	// request device info
				if (m_retries <= 0)
				{
					sendRequestDeviceInfo();
					m_ack_timer.mark();
					m_ack     = false;
					m_nack    = false;
					m_retries = 1;
				}
				else
				if (m_device_info_ok)
				{	// received the device info
					m_poll_timer.mark();
					m_ack     = false;
					m_nack    = false;
					m_retries = 0;
					m_mode    = MODE_IDLE;
				}
				else
				if (m_nack)
				{	// received a NACK
					m_nack = false;
					closeDevice(true);
				}
				else
				if (m_ack_timer.millisecs() >= m_ack_timeout_ms)
				{	// no device info received
					m_ack_timer.mark();
					if (m_retries >= m_max_retries)
					{
						closeDevice(true);
					}
					else
					{
						sendRequestDeviceInfo();
						m_retries++;
					}
				}
				break;

			case MODE_IDLE:
				if (m_new_mode != MODE_IDLE || m_update_sweep_settings)
				{
					if (m_new_mode == MODE_SINGLE_SCAN || m_new_mode == MODE_SCAN || m_update_sweep_settings)
					{
						if (m_retries <= 0)
						{
							// this is the buffer we are going to save the incoming point into
							data_unit.m_point_incoming.resize(0);

							m_prev_index = -1;
							m_num_frames = 0;

							//common.logFileClose();
							//common.dataFileClose();

							common.logFileDelete();
							common.dataFileDelete();

							m_own_sweep_settings.f_start         = data_unit.m_freq_start_Hz;
							m_own_sweep_settings.f_stop          = data_unit.m_freq_stop_Hz;
							m_own_sweep_settings.points          = data_unit.m_points;
							m_own_sweep_settings.if_bandwidth    = data_unit.m_bandwidth_Hz;
							m_own_sweep_settings.cdbm_excitation = m_output_power;
							m_own_sweep_settings.excitePort1     = 1;
//							m_own_sweep_settings.excitePort2     = 1;
							m_own_sweep_settings.excitePort2     = 0;
							m_own_sweep_settings.suppressPeaks   = 1;

							sendSweepSettings((m_new_mode != MODE_IDLE) ? false : true);
							m_ack_timer.mark();
							m_ack     = false;
							m_nack    = false;
							m_retries = 1;
						}
						else
						if (m_ack)
						{	// received the ack
							m_ack     = false;
							m_nack    = false;
							m_retries = 0;
							m_mode    = m_new_mode;
							m_update_sweep_settings = false;
							m_poll_timer.mark();
						}
						else
						if (m_nack)
						{	// received a NACK
							m_nack = false;
							closeDevice(true);
						}
						else
						if (m_ack_timer.millisecs() >= m_ack_timeout_ms)
						{	// no device info received
							m_ack_timer.mark();
							if (m_retries >= m_max_retries)
							{
								closeDevice(true);
							}
							else
							{
								sendSweepSettings((m_new_mode != MODE_IDLE) ? false : true);
								m_retries++;
							}
						}
					}
					else
					if (m_new_mode == MODE_GENERATOR)
					{
						if (m_retries <= 0)
						{
							m_own_generator_settings.frequency  = data_unit.m_freq_cw_Hz;
							m_own_generator_settings.cdbm_level = m_output_power;
							m_own_generator_settings.activePort = 1;	// 0 = off, 1 = port 1, 2 = port 2

							sendGeneratorSettings(true);	// enable generator mode
							m_ack_timer.mark();
							m_ack     = false;
							m_nack    = false;
							m_retries = 1;
						}
						else
						if (m_ack)
						{	// received the ack
							m_ack     = false;
							m_nack    = false;
							m_retries = 0;
							m_mode    = m_new_mode;
							m_update_generator_settings = false;
							m_poll_timer.mark();
						}
						else
						if (m_nack)
						{	// received a NACK
							m_nack = false;
							closeDevice(true);
						}
						else
						if (m_ack_timer.millisecs() >= m_ack_timeout_ms)
						{	// no device info received
							m_ack_timer.mark();
							if (m_retries >= m_max_retries)
							{
								closeDevice(true);
							}
							else
							{
								sendGeneratorSettings(true);	// enable generator mode
								m_retries++;
							}
						}
					}
					else
					{
						m_poll_timer.mark();
						m_ack      = false;
						m_nack     = false;
						m_retries  = 0;
						m_new_mode = MODE_IDLE;
					}
				}
				else
				{	// do our idle stuff here
					if (m_poll_ms > 0 && m_poll_timer.millisecs() >= m_poll_ms)
					{
						m_ack     = false;
						m_nack    = false;
						m_retries = 0;
						m_mode    = MODE_POLL;
					}
				}
				break;

			case MODE_POLL:
				if (m_retries <= 0)
				{
					sendRequestDeviceInfo();
					m_ack_timer.mark();
					m_ack     = false;
					m_nack    = false;
					m_retries = 1;
				}
				else
				if (m_ack)
				{	// received the ACK
					m_poll_timer.mark();
					m_ack     = false;
					m_nack    = false;
					m_retries = 0;
					m_mode    = MODE_IDLE;
				}
				else
				if (m_nack)
				{	// received a NACK
					m_nack = false;
					closeDevice(true);
				}
				else
				if (m_ack_timer.millisecs() >= m_ack_timeout_ms)
				{	// no device info received
					m_ack_timer.mark();
					if (m_retries >= m_max_retries)
					{
						closeDevice(true);
					}
					else
					{
						sendRequestDeviceInfo();
						m_retries++;
					}
				}
				break;

			case MODE_SINGLE_SCAN:
			case MODE_SCAN:
				if ((m_new_mode != MODE_SINGLE_SCAN && m_new_mode != MODE_SCAN && m_new_mode != MODE_NONE) || m_update_sweep_settings)
				{
					if (m_retries <= 0)
					{
						if (m_new_mode == MODE_SINGLE_SCAN || m_new_mode == MODE_SCAN)
							m_events_thread->Priority = m_events_thread_priority;

						data_unit.m_point_incoming.resize(0);
						if (!m_update_sweep_settings)
							sendSweepSettings(true);	// stop sweeping
						else
							sendSweepSettings(false);	// continue sweeping
						m_ack_timer.mark();
						m_ack     = false;
						m_nack    = false;
						m_retries = 1;
					}
					else
					if (m_ack)
					{	// received the ack
						m_ack     = false;
						m_nack    = false;
						m_retries = 0;
//						if (!m_update_sweep_settings)
							m_mode = m_new_mode;
						m_update_sweep_settings = false;
					}
					else
					if (m_nack)
					{	// received a NACK
						m_nack = false;
						closeDevice(true);
					}
					else
					if (m_ack_timer.millisecs() >= m_ack_timeout_ms)
					{	// no device info received
						m_ack_timer.mark();
						if (m_retries >= m_max_retries)
						{
							closeDevice(true);
						}
						else
						{
							if (!m_update_sweep_settings)
								sendSweepSettings(true);	// stop sweeping
							else
								sendSweepSettings(false);	// continue sweeping
							m_retries++;
						}
					}
				}
				else
				{
					if (m_poll_timer.millisecs() >= 1500)
					{	// no data being received
						m_new_mode = MODE_IDLE;
						if (Form1)
							::PostMessage(Form1->Handle, WM_CAPTURE_STOP, m_num_frames, 0);
					}
				}
				break;

			case MODE_GENERATOR:
				if ((m_new_mode != MODE_GENERATOR && m_new_mode != MODE_NONE) || m_update_generator_settings)
				{
					if (m_retries <= 0)
					{
						if (!m_update_generator_settings)
//							sendGeneratorSettings(false);	// disable generator mode
							sendSweepSettings(true);	// stop sweeping
						else
							sendGeneratorSettings(true);	// enable generator mode
						m_ack_timer.mark();
						m_ack     = false;
						m_nack    = false;
						m_retries = 1;
					}
					else
					if (m_ack)
					{	// received the ack
						m_ack     = false;
						m_nack    = false;
						m_retries = 0;
//						if (!m_update_generator_settings)
							m_mode = m_new_mode;
						m_update_generator_settings = false;
					}
					else
					if (m_nack)
					{	// received a NACK
						m_nack = false;
						closeDevice(true);
					}
					else
					if (m_ack_timer.millisecs() >= m_ack_timeout_ms)
					{	// no device info received
						m_ack_timer.mark();
						if (m_retries >= m_max_retries)
						{
							closeDevice(true);
						}
						else
						{
							if (!m_update_generator_settings)
//								sendGeneratorSettings(false);	// disable generator mode
								sendSweepSettings(true);	// stop sweeping
							else
								sendGeneratorSettings(true);	// enable generator mode
							m_retries++;
						}
					}
				}
				else
				{
					if (m_poll_timer.millisecs() >= 1500)
					{	// no data being received
//						m_new_mode = MODE_IDLE;
//						if (Form1)
//							::PostMessage(Form1->Handle, WM_CAPTURE_STOP, m_num_frames, 0);
					}
				}
				break;

			default:	// should never arrive here
				data_unit.m_point_incoming.resize(0);
				m_poll_timer.mark();
				m_ack     = false;
				m_nack    = false;
				m_retries = 0;
				m_mode    = MODE_IDLE;
				break;
		}
	}

	// process the received packets
	//CCriticalSection cs(m_rx_packets.cs);
	while (m_thread != NULL && m_device.hdev != NULL && m_mode != MODE_NONE && m_rx_packets.packet_rd != m_rx_packets.packet_wr)
	{
      m_poll_timer.mark();
		processRxPacket(&m_rx_packets.packet[m_rx_packets.packet_rd]);
		if (++m_rx_packets.packet_rd >= (int)ARRAY_SIZE(m_rx_packets.packet))
			m_rx_packets.packet_rd = 0;
	}
}

/*
#define CRC32_POLYGON 0xEDB88320
uint32_t __fastcall CJanVNA2_comms::crc32(uint32_t crc, const void *data, const int len)
{
	int m = len;
	const uint8_t *u8buf = (const uint8_t *)data;
	crc = ~crc;
	while (m--)
	{
		crc ^= *u8buf++;
		for (int k = 0; k < 8; k++)
			crc = crc & 1 ? (crc >> 1) ^ CRC32_POLYGON : crc >> 1;
	}
	return ~crc;
}
*/
uint32_t __fastcall CJanVNA2_comms::crc32(uint32_t crc, const void *data, const int len)
{
	const uint8_t *u8buf = (const uint8_t *)data;
	crc = ~crc;
	for (int i = 0; i < len; i++)
		crc = common.updateCRC32(crc, *u8buf++);
	return ~crc;
}

void __fastcall CJanVNA2_comms::setMode(const t_mode new_mode)
{
	// request a new mode
	m_new_mode = connected ? new_mode : MODE_NONE;
}

int __fastcall CJanVNA2_comms::usbRxOnDataCallback(TObject *sender, std::vector <uint8_t> &buffer, const int size)
{
	if (m_device.hdev == NULL || m_mode == MODE_NONE || size <= 0 || size > (int)buffer.size() || m_usb_rx_data == NULL)
		return 0;

	// we received data

	// General packet format ..
	//
	// 1 byte packet magic
	// 2 byte packet total length (header, data and CRC)
	// 1 byte packet type
	// n-byte packet payload
	// 4 byte packet CRC32 (header & payload)

	int num_packets = 0;
	int index = 0;

	while (true)
	{
		const int num_bytes = size - index;
		if (num_bytes < 1)
			break;

		t_packet_info *packet = &m_rx_packets.packet[m_rx_packets.packet_wr];

		memcpy(&packet->header, &buffer[index], sizeof(packet->header));
		const uint8_t *data = &buffer[index + sizeof(packet->header)];

		if (packet->header.magic != header_magic)
		{	// not the start of a packet
			index++;
			continue;
		}

		if (num_bytes >= (int)sizeof(packet->header))
		{	// check the header is valid
			if (packet->header.total_length < (sizeof(packet->header) + sizeof(packet->crc)))
			{	// invalid packet length
				index++;
				continue;
			}
			bool valid_packet_type;
			switch ((PacketType)packet->header.packet_type)
			{
				case PacketType::None:
				case PacketType::Datapoint:
				case PacketType::SweepSettings:
				case PacketType::Status:
				case PacketType::ManualControl:
				case PacketType::DeviceInfo:
				case PacketType::FirmwarePacket:
				case PacketType::Ack:
				case PacketType::ClearFlash:
				case PacketType::PerformFirmwareUpdate:
				case PacketType::Nack:
				case PacketType::Reference:
				case PacketType::Generator:
				case PacketType::SpectrumAnalyzerSettings:
				case PacketType::SpectrumAnalyzerResult:
				case PacketType::RequestDeviceInfo:
					valid_packet_type = true;
					break;
				default:
					valid_packet_type = false;
					break;
			}
			if (!valid_packet_type)
			{	// unknown packet type
				index++;
				continue;
			}
		}

		if (num_bytes < (int)(sizeof(packet->header) + sizeof(packet->crc)))
			break;

		if (packet->header.total_length > ((int)buffer.size() - index))
		{	// packet is too big for the buffer
			index++;
			continue;
		}

		if (num_bytes < packet->header.total_length)
			break;	// packet not yet complete

		const int data_size = packet->header.total_length - sizeof(packet->header) - sizeof(packet->crc);

		if (data_size > (int)sizeof(packet->data))
		{	// data won't fit into our packet data buffer
			index++;
			continue;
		}

		packet->crc = *(uint32_t *)&data[data_size];
		if (packet->crc != 0 && (PacketType)packet->header.packet_type != PacketType::Datapoint)
		{
			const uint32_t crc = crc32(0x00000000, &buffer[index], packet->header.total_length - sizeof(packet->crc));
			if (crc != packet->crc)
			{	// invalid crc
				index++;
				continue;
			}
		}

		if (data_size > 0 && data_size <= (int)sizeof(packet->data))
			memcpy(&packet->data, data, data_size);

		index += packet->header.total_length;

		#if 0
			// TEST ONLY
			if (Form1 && packet->header.packet_type == PacketType::Datapoint)
			{
				if (data_size >= (int)sizeof(t_janvna2_datapoint))
				{
					const t_janvna2_datapoint *data_point = (const t_janvna2_datapoint *)&packet->data;
					String s;
					s.printf(L"data point %4u %10llu Hz", data_point->pointNum, data_point->frequency);
					common.logFileAppend(s + "\n");
				}
			}
		#endif

		//CCriticalSection cs(m_rx_packets.cs);
		int i = m_rx_packets.packet_wr;
		if (++i >= (int)ARRAY_SIZE(m_rx_packets.packet))
			i = 0;
		m_rx_packets.packet_wr = i;

		num_packets++;
	}

	if (num_packets > 0)
	{
		const double millisecs = m_rx_data_timer.millisecs(true);
		const double millisecs_per_packet = millisecs / num_packets;
		if (millisecs >= 5)
		{
			#if 0
				// TEST ONLY
				if (m_mode == MODE_SINGLE_SCAN || m_mode == MODE_SCAN)
				{
					String s;
					s.printf(L"%d  %0.3fms  %0.3fms/packet", num_packets, millisecs, millisecs_per_packet);
					common.logFileAppend(s + "\n");
				}
			#endif
		}
		#if 0
			// TEST ONLY
			if (m_mode == MODE_SINGLE_SCAN || m_mode == MODE_SCAN)
			{
				String s;
				s.printf(L"%d  %0.3fms  %0.3fms/packet", num_packets, millisecs, millisecs_per_packet);
				common.logFileAppend(s + "\n");
			}
		#endif
	}

	return index;
}

int __fastcall CJanVNA2_comms::usbRxOnLogCallback(TObject *sender, std::vector <uint8_t> &buffer, const int size)
{
	if (m_device.hdev == NULL || m_mode == MODE_NONE || size <= 0 || size > (int)buffer.size() || m_usb_in_log == NULL)
		return 0;

	// we received data

	int k = 0;

	while (true)
	{
		const int num_bytes = size - k;
		if (num_bytes <= 0)
			break;

		// look for a '\n'
		int n = k;
		while (n < size)
		{
			if (buffer[n] == '\n')
				break;	// found the end of a line
			n++;
		}

		if (n >= size)
			break;	// end of line not found

		String s = "rx log: ";
		while (k <= n)
		{
			const char c = buffer[k++];
			if (c >= 32)
				s += c;
			else
			if (c != '\r' && c != '\n')
				s += "[" + IntToHex((uint8_t)c, 2) + "]";
		}

		if (!s.IsEmpty() && Form1)
		{
			if (s.LowerCase().Pos("log: ") <= 0)
				Form1->pushCommMessage(s);
		}
	}

	return k;
}

void CJanVNA2_comms::libusb_log_cb(libusb_context *ctx, enum libusb_log_level level, const char *str)
{
	// TEST ONLY

	static int debug_count = 0;
	static int debug_block = 0;
	if (str)
	{
//		fprintf(stdout, "%s", str);
		if (Form1)
		{
			String s = String(str);
			if (debug_block <= 0 &&(janvna2_comms.mode == MODE_SINGLE_SCAN || janvna2_comms.mode == MODE_SCAN))
			{
//				debug_block++;
				if (s.LowerCase().Pos("[libusb_submit_transfer]") > 0)
				{
					debug_count = 1000;
//					String s2;
//					s2.printf(L"\n");
//					common.logFileAppend(s2);
				}
			}
			if (debug_count > 0)
			{
				debug_count--;
				common.logFileAppend(s);
			}
		}
	}
}

int __fastcall CJanVNA2_comms::openDevice(const uint16_t vid, const uint16_t pid)
{
	int res;
	char str[256] = {0};

	closeDevice();

	common.logFileDelete();
	common.dataFileDelete();

	reset(true);

	libusb_context *ctx = NULL;
	libusb_device_handle *hdev = NULL;

	ssize_t cnt;
	libusb_device **dev_list = NULL;

	t_vna_list vna;

	try
	{
		res = libusb_init(&ctx);
		if (res < 0)
			return res;
		//libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, verbose);
	}
	catch (Exception &exception)
	{
		if (Form1)
			Form1->pushCommMessage("libusb_init " + exception.ToString());
		return -1;
	}

	try
	{
		cnt = libusb_get_device_list(ctx, &dev_list);
		if (cnt < 0)
		{
			libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
			libusb_exit(ctx);
			return (int)cnt;
		}

		for (int i = 0; i < cnt; i++)
		{
			libusb_device *dev = dev_list[i];

			struct libusb_device_descriptor desc;
			vna.vid           = 0;
			vna.pid           = 0;
			vna.manufacturer  = "";
			vna.product       = "";
			vna.serial_number = "";
			vna.speed         = "";
			vna.dev_class     = "";

			res = libusb_get_device_descriptor(dev, &desc);
			if (res >= 0)
			{
				vna.vid       = desc.idVendor;
				vna.pid       = desc.idProduct;
				vna.speed     = usbSpeedToStr(libusb_get_device_speed(dev));
				vna.dev_class = usbClassToStr(desc.bDeviceClass);
				res = libusb_open(dev, &hdev);
				if (res >= 0 && hdev != NULL)
				{
					res = libusb_get_string_descriptor_ascii(hdev, desc.iSerialNumber, (PBYTE)str, sizeof(str));
					if (res >= 0)
					{
						vna.serial_number = String(str);
						res = libusb_get_string_descriptor_ascii(hdev, desc.iProduct, (PBYTE)str, sizeof(str));
						if (res >= 0)
						{
							vna.product = String(str);
							res = libusb_get_string_descriptor_ascii(hdev, desc.iManufacturer, (PBYTE)str, sizeof(str));
							if (res >= 0)
								vna.manufacturer = String(str);
						}
					}
				}
			}

			if (hdev == NULL)
				continue;

			if (vna.vid != vid || vna.pid != pid || vna.product.UpperCase() != "VNA")
			{
				libusb_close(hdev);
				hdev = NULL;
				continue;
			}

			// found the VNA

			break;
		}

		if (dev_list)
			libusb_free_device_list(dev_list, 1);
		dev_list = NULL;

		if (hdev == NULL)
		{	// not found
			libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
			libusb_exit(ctx);
			return -2;
		}
	}
	catch (Exception &exception)
	{
		if (Form1)
			Form1->pushCommMessage("openDevice " + exception.ToString());
		if (dev_list)
			libusb_free_device_list(dev_list, 1);
		if (hdev)
			libusb_close(hdev);
		libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
		libusb_exit(ctx);
		return -3;
	}

	if (Form1)
	{
		String s;
		s.printf(L"connected to Jan's VNA %04X %04X", vna.vid, vna.pid);
		s += " " + vna.serial_number;
		s += " " + vna.speed;
		s += " " + vna.dev_class;
		Form1->pushCommMessage(s);
	}

	try
	{
		libusb_set_log_cb(ctx, libusb_log_cb, LIBUSB_LOG_CB_CONTEXT);	// LIBUSB_LOG_CB_GLOBAL LIBUSB_LOG_CB_CONTEXT
		libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
		//libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_DEBUG);	// LIBUSB_LOG_LEVEL_NONE LIBUSB_LOG_LEVEL_ERROR LIBUSB_LOG_LEVEL_WARNING LIBUSB_LOG_LEVEL_INFO LIBUSB_LOG_LEVEL_DEBUG

//		if (libusb_kernel_driver_active(hdev, 0))
//			libusb_detach_kernel_driver(hdev, 0);

		res = libusb_set_auto_detach_kernel_driver(hdev, 1);
		if (res < 0)
		{
			if (Form1)
			{
				const char *str = libusb_strerror(res);
				String s;
				s.printf(L"usb auto detach", vna.vid, vna.pid);
				if (str)
					s += ": " + String(str);
				Form1->pushCommMessage(s);
			}
		}

		res = libusb_claim_interface(hdev, 0);
		if (res < 0)
		{
			libusb_close(hdev);
			hdev = NULL;
			if (Form1)
			{
				const char *str = libusb_strerror(res);
				String s;
				s.printf(L"disconnected from Jan's VNA %04X %04X. could not claim interface", vna.vid, vna.pid);
				s += " " + vna.serial_number;
				if (str)
					s += ": " + String(str);
				Form1->pushCommMessage(s);
			}
			libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
			libusb_exit(ctx);
			return res;
		}
	}
	catch (Exception &exception)
	{
		if (Form1)
			Form1->pushCommMessage("openDevice " + exception.ToString());
		if (hdev)
			libusb_close(hdev);
		libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
		libusb_exit(ctx);
		return -4;
	}

	data_unit.m_vna_data.type = UNIT_TYPE_JANVNA_V2;
	data_unit.m_vna_data.name = "Jan's VNA";

	//CCriticalSection cs(m_rx_packets.cs);
	m_rx_packets.packet_rd = 0;
	m_rx_packets.packet_wr = 0;
	//m_rx_packets.cs.leave();

	m_data_tx.index_wr = 0;

	m_usb_rx_data = new CUSBRx(ctx, hdev, EP_Data_In_Addr, 8192, usbRxOnDataCallback);
	m_usb_in_log  = new CUSBRx(ctx, hdev, EP_Log_In_Addr,  4096, usbRxOnLogCallback);

	m_device.vna  = vna;
	m_device.ctx  = ctx;
	m_device.hdev = hdev;

	m_mode = MODE_INIT1;

	if (!startThread())
	{
		if (m_usb_rx_data)
			delete m_usb_rx_data;
		m_usb_rx_data = NULL;

		if (m_usb_in_log)
			delete m_usb_in_log;
		m_usb_in_log = NULL;

		m_device.ctx  = NULL;
		m_device.hdev = NULL;

		m_mode = MODE_NONE;

		if (Form1)
			Form1->pushCommMessage("error: cannot create the thread for Jan's VNA comms");

		libusb_close(hdev);

		libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
		libusb_exit(ctx);

//		closeDevice();
		return -3;
	}

	Beep(440, 80);

	return 0;
}

void __fastcall CJanVNA2_comms::closeDevice(const bool local)
{
	libusb_context *ctx        = m_device.ctx;
	libusb_device_handle *hdev = m_device.hdev;

	if (hdev != NULL && !local)
	{
		sendSweepSettings(true);

		m_ack_timer.mark();
		while (m_ack_timer.millisecs() < 500 && m_data_tx.index_wr > 0)
			Application->ProcessMessages();
		if (m_data_tx.index_wr <= 0)
		{	// wait for the USB link to complete - we should get an "Ack" back from the VNA
			m_ack_timer.mark();
			while (m_ack_timer.millisecs() < m_ack_timeout_ms && !(m_ack || m_nack))
				Application->ProcessMessages();
		}
	}

	stopThread(local);

	if (m_usb_rx_data)
		delete m_usb_rx_data;
	m_usb_rx_data = NULL;

	if (m_usb_in_log)
		delete m_usb_in_log;
	m_usb_in_log = NULL;

	m_device.hdev = NULL;
	m_device.ctx  = NULL;
	m_mode        = MODE_NONE;

	//CCriticalSection cs(m_rx_packets.cs);
	m_rx_packets.packet_rd = 0;
	m_rx_packets.packet_wr = 0;
	//m_rx_packets.cs.leave();

	m_data_tx.index_wr = 0;

	if (hdev != NULL)
	{
		try
		{
			libusb_release_interface(hdev, 0);
			libusb_close(hdev);
			libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
			libusb_exit(ctx);
		}
		catch (Exception &exception)
		{
			if (Form1)
				Form1->pushCommMessage("closeDevice " + exception.ToString());
		}

		Beep(349, 80);

		if (Form1)
		{
			String s;
			s.printf(L"disconnected from Jan's VNA %04X %04X", m_device.vna.vid, m_device.vna.pid);
			s += " " + m_device.vna.serial_number;
			Form1->pushCommMessage(s);
		}

		if (local)
			::PostMessage(Form1->Handle, WM_DISCONNECT, 0, 0);
	}

	common.logFileClose();
	common.dataFileClose();
}

int __fastcall CJanVNA2_comms::devicePresent(const uint16_t vid, const uint16_t pid)
{
	int res = -1;

	char str[256];
	libusb_context *ctx = NULL;
	libusb_device_handle *hdev = NULL;
	libusb_device **dev_list = NULL;
	ssize_t cnt;

	try
	{
		res = libusb_init(&ctx);
		if (res < 0)
			return res;
		cnt = libusb_get_device_list(ctx, &dev_list);
		if (cnt < 0)
		{
			libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
			libusb_exit(ctx);
			return (int)cnt;
		}
	}
	catch (Exception &exception)
	{
		if (Form1)
			Form1->pushCommMessage("libusb_init " + exception.ToString());
		if (res >= 0)
		{
			libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
			libusb_exit(ctx);
		}
		return -1;
	}

	for (int i = 0; i < cnt; i++)
	{
		libusb_device *dev = dev_list[i];

		struct libusb_device_descriptor desc;
		t_vna_list vna;

		try
		{
			res = libusb_get_device_descriptor(dev, &desc);
			if (res >= 0)
			{
				vna.vid       = desc.idVendor;
				vna.pid       = desc.idProduct;
				vna.speed     = usbSpeedToStr(libusb_get_device_speed(dev));
				vna.dev_class = usbClassToStr(desc.bDeviceClass);
				res = libusb_open(dev, &hdev);
				if (res >= 0 && hdev != NULL)
				{
					res = libusb_get_string_descriptor_ascii(hdev, desc.iSerialNumber, (PBYTE)str, sizeof(str));
					if (res >= 0)
					{
						vna.serial_number = String(str);
						res = libusb_get_string_descriptor_ascii(hdev, desc.iProduct, (PBYTE)str, sizeof(str));
						if (res >= 0)
						{
							vna.product = String(str);
							res = libusb_get_string_descriptor_ascii(hdev, desc.iManufacturer, (PBYTE)str, sizeof(str));
							if (res >= 0)
								vna.manufacturer = String(str);
						}
					}
				}
			}
		}
		catch (Exception &exception)
		{
			continue;
		}

		if (hdev == NULL)
			continue;

		if (vna.vid != vid || vna.pid != pid || vna.product.UpperCase() != "VNA")
		{
			libusb_close(hdev);
			hdev = NULL;
			continue;
		}

		// found one

		break;
	}

	if (hdev != NULL)
		libusb_close(hdev);

	try
	{
		if (dev_list)
			libusb_free_device_list(dev_list, 1);
		libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
		libusb_exit(ctx);
	}
	catch (Exception &exception)
	{
		if (Form1)
			Form1->pushCommMessage("libusb_free_device_list libusb_exit" + exception.ToString());
	}

	return (hdev != NULL) ? 0 : -1;	// -1 = not found
}

int __fastcall CJanVNA2_comms::deviceDetails(const uint16_t vid, const uint16_t pid, const bool show_details)
{
	String s;
	int res = -1;

	libusb_context *ctx = NULL;
	libusb_device_handle *hdev = NULL;
	libusb_device **dev_list = NULL;
	ssize_t cnt;

	try
	{
		res = libusb_init(&ctx);
		if (res < 0)
			return res;
		cnt = libusb_get_device_list(ctx, &dev_list);
		if (cnt < 0)
		{
			libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
			libusb_exit(ctx);
			return (int)cnt;
		}
	}
	catch (Exception &exception)
	{
		if (Form1)
			Form1->pushCommMessage("libusb_init " + exception.ToString());
		if (res >= 0)
		{
			libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
			libusb_exit(ctx);
		}
		return -1;
	}

	char str[256] = {0};

	for (int i = 0; i < cnt; i++)
	{
		libusb_device *dev = dev_list[i];

		struct libusb_device_descriptor desc;
		t_vna_list vna;

		try
		{
			res = libusb_get_device_descriptor(dev, &desc);
			if (res >= 0)
			{
				vna.vid       = desc.idVendor;
				vna.pid       = desc.idProduct;
				vna.speed     = usbSpeedToStr(libusb_get_device_speed(dev));
				vna.dev_class = usbClassToStr(desc.bDeviceClass);
				res = libusb_open(dev, &hdev);
				if (res >= 0 && hdev != NULL)
				{
					res = libusb_get_string_descriptor_ascii(hdev, desc.iSerialNumber, (PBYTE)str, sizeof(str));
					if (res >= 0)
					{
						vna.serial_number = String(str);
						res = libusb_get_string_descriptor_ascii(hdev, desc.iProduct, (PBYTE)str, sizeof(str));
						if (res >= 0)
						{
							vna.product = String(str);
							res = libusb_get_string_descriptor_ascii(hdev, desc.iManufacturer, (PBYTE)str, sizeof(str));
							if (res >= 0)
								vna.manufacturer = String(str);
						}
					}
				}
			}
		}
		catch (Exception &exception)
		{
			continue;
		}

		if (hdev == NULL)
			continue;

		libusb_close(hdev);
		hdev = NULL;

		if (vna.vid != vid || vna.pid != pid || vna.product.UpperCase() != "VNA")
			continue;

		if (Form1 && show_details)
		{
			Form1->pushCommMessage("device_details: vid/pid       " + IntToHex(vna.vid, 4) + " " + IntToHex(vna.pid, 4));
			Form1->pushCommMessage("device_details: bus speed     " + vna.speed);
			Form1->pushCommMessage("device_details: device class  " + vna.dev_class);
			Form1->pushCommMessage("device_details: manufacture   " + vna.manufacturer);
			Form1->pushCommMessage("device_details: product       " + vna.product);
			Form1->pushCommMessage("device_details: serial number " + vna.serial_number);
		}

		break;
	}

	try
	{
		if (dev_list)
			libusb_free_device_list(dev_list, 1);
		libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
		libusb_exit(ctx);
	}
	catch (Exception &exception)
	{
		if (Form1)
			Form1->pushCommMessage("libusb_free_device_list libusb_exit" + exception.ToString());
	}

	return 0;
}

int __fastcall CJanVNA2_comms::updateDeviceList()
{
	int res = -1;
	char str[256];

	m_vna_list.resize(0);

	libusb_context *ctx = NULL;
	libusb_device **dev_list = NULL;
	libusb_device_handle *hdev = NULL;
	ssize_t cnt;

	try
	{
		res = libusb_init(&ctx);
		if (res < 0)
			return res;
		cnt = libusb_get_device_list(ctx, &dev_list);
		if (cnt < 0)
		{
			libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
			libusb_exit(ctx);
			return (int)cnt;
		}
	}
	catch (Exception &exception)
	{
		if (Form1)
			Form1->pushCommMessage("libusb_init libusb_get_device_list libusb_exit " + exception.ToString());
		if (res >= 0)
		{
			libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
			libusb_exit(ctx);
		}
		return -1;
	}

	for (int i = 0; i < cnt; i++)
	{
		libusb_device *dev = dev_list[i];

		struct libusb_device_descriptor desc;
		t_vna_list vna;

		try
		{
			res = libusb_get_device_descriptor(dev, &desc);
			if (res >= 0)
			{
				vna.vid       = desc.idVendor;
				vna.pid       = desc.idProduct;
				vna.speed     = usbSpeedToStr(libusb_get_device_speed(dev));
				vna.dev_class = usbClassToStr(desc.bDeviceClass);
				res = libusb_open(dev, &hdev);
				if (res >= 0 && hdev != NULL)
				{
					res = libusb_get_string_descriptor_ascii(hdev, desc.iSerialNumber, (PBYTE)str, sizeof(str));
					if (res >= 0)
					{
						vna.serial_number = String(str);
						res = libusb_get_string_descriptor_ascii(hdev, desc.iProduct, (PBYTE)str, sizeof(str));
						if (res >= 0)
						{
							vna.product = String(str);
							res = libusb_get_string_descriptor_ascii(hdev, desc.iManufacturer, (PBYTE)str, sizeof(str));
							if (res >= 0)
								vna.manufacturer = String(str);
						}
					}
				}
			}
		}
		catch (Exception &exception)
		{
			continue;
		}

		if (hdev == NULL)
			continue;

		libusb_close(hdev);
		hdev = NULL;

		if (vna.product.UpperCase() == "VNA")
			m_vna_list.push_back(vna);
	}

	try
	{
		if (dev_list)
			libusb_free_device_list(dev_list, 1);
		libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
		libusb_exit(ctx);
	}
	catch (Exception &exception)
	{
		if (Form1)
			Form1->pushCommMessage("libusb_free_device_list libusb_exit" + exception.ToString());
	}

	return m_vna_list.size();
}

int __fastcall CJanVNA2_comms::getDeviceList(std::vector <t_vna_list> &vna_list)
{
	vna_list.resize(0);
	updateDeviceList();
	vna_list = m_vna_list;
	return vna_list.size();
}

int __fastcall CJanVNA2_comms::listDevices()
{
	String s;
	int res = -1;
	char str[256];

	m_vna_list.resize(0);

	libusb_context *ctx = NULL;
	libusb_device_handle *hdev = NULL;
	libusb_device **dev_list = NULL;
	ssize_t cnt;

	try
	{
		res = libusb_init(&ctx);
		if (res < 0)
			return res;
		cnt = libusb_get_device_list(ctx, &dev_list);
		if (cnt < 0)
		{
			libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
			libusb_exit(ctx);
			return (int)cnt;
		}
	}
	catch (Exception &exception)
	{
		if (Form1)
			Form1->pushCommMessage("libusb_init " + exception.ToString());
		if (res >= 0)
		{
	  		libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
			libusb_exit(ctx);
		}
		return -1;
	}

	for (int i = 0; i < cnt; i++)
	{
		libusb_device *dev = dev_list[i];

		struct libusb_device_descriptor desc;
		t_vna_list vna;

		try
		{
			res = libusb_get_device_descriptor(dev, &desc);
			if (res >= 0)
			{
				vna.vid       = desc.idVendor;
				vna.pid       = desc.idProduct;
				vna.speed     = usbSpeedToStr(libusb_get_device_speed(dev));
				vna.dev_class = usbClassToStr(desc.bDeviceClass);
				res = libusb_open(dev, &hdev);
				if (res >= 0 && hdev != NULL)
				{
					res = libusb_get_string_descriptor_ascii(hdev, desc.iSerialNumber, (PBYTE)str, sizeof(str));
					if (res >= 0)
					{
						vna.serial_number = String(str);
						res = libusb_get_string_descriptor_ascii(hdev, desc.iProduct, (PBYTE)str, sizeof(str));
						if (res >= 0)
						{
							vna.product = String(str);
							res = libusb_get_string_descriptor_ascii(hdev, desc.iManufacturer, (PBYTE)str, sizeof(str));
							if (res >= 0)
								vna.manufacturer = String(str);
						}
					}
				}
			}
		}
		catch (Exception &exception)
		{
			continue;
		}

		if (hdev == NULL)
			continue;

		libusb_close(hdev);
		hdev = NULL;

		m_vna_list.push_back(vna);

		if (Form1)
		{
			Form1->pushCommMessage("list_devices: index            " + IntToStr((int)m_vna_list.size() - 1));
			Form1->pushCommMessage("list_devices: vid/pid          " + IntToHex(vna.vid, 4) + " " + IntToHex(vna.pid, 4));
			Form1->pushCommMessage("list_devices: bus speed        " + vna.speed);
			Form1->pushCommMessage("list_devices: device class     " + vna.dev_class);
			Form1->pushCommMessage("list_devices: manufacturer     " + vna.manufacturer);
			Form1->pushCommMessage("list_devices: product          " + vna.product);
			Form1->pushCommMessage("list_devices: serial number    " + vna.serial_number);
		}
	}

	try
	{
		if (dev_list)
			libusb_free_device_list(dev_list, 1);
		libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_NONE);
		libusb_exit(ctx);
	}
	catch (Exception &exception)
	{
		if (Form1)
			Form1->pushCommMessage("libusb_free_device_list libusb_exit" + exception.ToString());
	}

	return m_vna_list.size();
}

