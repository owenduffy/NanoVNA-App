
#pragma hdrstop

#include "NanoVNA_v2_comms.h"
#include "CommsUnit.h"
#include "DataUnit.h"
#include "Unit1.h"
#include "SettingsUnit.h"

#pragma package(smart_init)

// *******************************

// Normal mode
// tx: 00 00 00 00 00 00 00 00 20 26 02 0D
// rx: 32 ACK
// tx: 10 F0
// rx: 02 DEVICE VARIANT
// tx: 10 F1
// rx: 01 PROTOCOL VERSION
// tx: 10 F2
// rx: 02 HARDWARE REVISION
// tx: 10 F3 10 F4
// rx: 01 02 FIRMWARE VERSION

// DFU mode
// tx: 00 00 00 00 00 00 00 00 20 26 02 0D
// rx: 32 ACK
// tx: 10 F0
// rx: 02 DEVICE VARIANT
// tx: 10 F1
// rx: 01 PROTOCOL VERSION
// tx: 10 F2
// rx: 00 HARDWARE REVISION
// tx: 10 F3 10 F4
// rx: FF 00 FIRMWARE VERSION

// *******************************

CNanoVNA2Comms nanovna2_comms;

CNanoVNA2Comms::CNanoVNA2Comms()
{
	m_capture_bm = NULL;
	reset(false);
	m_mode = MODE_NONE;
}

CNanoVNA2Comms::~CNanoVNA2Comms()
{
	if (m_capture_bm)
	{
		delete m_capture_bm;
		m_capture_bm = NULL;
	}
}

void __fastcall CNanoVNA2Comms::resetScan()
{
	m_start                     = -1;
	m_step                      = -1;
	m_points_per_segment        = 0;
	m_data_points_per_frequency = 0;
}

void __fastcall CNanoVNA2Comms::reset(const bool reset_vna)
{
	if (m_capture_bm)
	{
		delete m_capture_bm;
		m_capture_bm = NULL;
	}

	m_tx_cmd.resize(0);

	m_mode               = MODE_INIT1;
	m_new_mode           = MODE_INIT1;
	m_retries            = 0;
	m_get_screen_capture = false;
	m_pause_comms        = false;
	m_usb_data_mode      = false;
	m_poll_ms            = DEFAULT_POLL_V2_MS;
	m_state_timeout_ms   = DEFAULT_STATE_TIME_OUT_V2_MS;

	resetScan();

//	if (reset_vna)
//		leaveUSBDataMode(true);
}

void __fastcall CNanoVNA2Comms::setMode(const t_mode mode)
{
	// request a new mode
	m_new_mode = mode;

	const t_mode prev_mode = m_mode;

	m_mode = mode;

	if (mode != prev_mode)
	{
		m_retries = 0;

		if (prev_mode > MODE_IDLE)
		{
			if (mode == MODE_IDLE)
			{
				if (prev_mode == MODE_SINGLE_SCAN || prev_mode == MODE_SCAN || prev_mode == MODE_GENERATOR)
				{
					Form1->pushCommMessage("tx: clearing FIFO buffer");
					addTxNulls();
					//addTxNulls(true, 0);
					//addTxWrite1(REG_V2_VALUES_FIFO, 0);
					sendData();
					m_tx_cmd.resize(0);

					leaveUSBDataMode(false);
				}
				if (prev_mode != MODE_POLL)
				{
					m_mode = MODE_POLL;
					poll();
            }
         }
		}
	}
}

void __fastcall CNanoVNA2Comms::sendData(void *data, int size)
{
	if (data == NULL && size < 0)
	{
		data = &m_tx_cmd[0];
		size = m_tx_cmd.size();
	}

	if (!data || size <= 0 || !Form1)
		return;

	String s;
	{
		const uint8_t *ptr = (const uint8_t *)data;
		for (int i = 0; i < size; i++)
		{
			String s2;
			s2.printf(L" %02X", ptr[i]);
			s += s2;
		}
		s = s.Trim();
	}

	if (Form1->m_comms.serial.connected)
	{	// send the new command through the serial link

		Form1->pushCommMessage("tx: " + s);

		if (Form1->m_comms.serial.TxBytes(data, size) < size)
		{	// failed
			Form1->pushCommMessage("tx: error .. " + s);
			//Form1->m_comms.serial.Disconnect();
			//::PostMessage(Form1->Handle, WM_DISCONNECT, 0, 0);
			return;
		}

		Form1->m_comms.rx_timer.mark();
	}

	#ifdef TCPIPH
		if (Form1->m_comms.tcpip.connected)
		{	// send the new command through the tcpip link

			Form1->pushCommMessage("tx: " + s);

			if (m_comms.tcpip.TxBytes(data, size) < size)
			{	// failed
				Form1->pushCommMessage("tx: " + String("error ") + m_comms.tcpip.lastErrorStr + " .. " + s);
				//m_comms.tcpip.Disconnect();
				//::PostMessage(Form1->Handle, WM_DISCONNECT, 0, 0);
				return;
			}

			Form1->m_comms.rx_timer.mark();
		}
	#else
		if (Form1->m_comms.tcpip)
		{
			try
			{
				if (Form1->m_comms.tcpip->Connected())
				{	// send the new command through the tcpip link

					Form1->pushCommMessage("tx: " + s);

					TMemoryStream *stream = new TMemoryStream();
					if (stream)
					{
						stream->Position = 0;
						stream->Write(data, size);
						stream->Position = 0;
						Form1->m_comms.tcpip->IOHandler->Write(stream, 0, false);
						delete stream;

						Form1->m_comms.rx_timer.mark();
					}
				}
			}
			catch (Exception &exception)
			{
				//Application->ShowException(&exception);
				Form1->pushCommMessage("tx: tcpip error " + exception.ToString());
			}
		}
	#endif
}

bool __fastcall CNanoVNA2Comms::inDFUMode()
{
	return (data_unit.m_vna_data.hardware_revision == REG_DFU_V2_HARDWARE_REVISION_ACK && data_unit.m_vna_data.firmware_major == REG_DFU_V2_FIRMWARE_MAJOR_ACK) ? true : false;
}

void __fastcall CNanoVNA2Comms::clearTxCommands()
{
	m_tx_cmd.resize(0);
}

void __fastcall CNanoVNA2Comms::addTxNulls(const bool reset_buf, const int num)
{
	if (reset_buf)
		m_tx_cmd.resize(0);
	for (int i = 0; i < num; i++)
		m_tx_cmd.push_back(0x00);
}

void __fastcall CNanoVNA2Comms::addTxRead1(const uint8_t reg_addr)
{
	m_tx_cmd.push_back(CMD_V2_READ_1);
	m_tx_cmd.push_back(reg_addr);
}

void __fastcall CNanoVNA2Comms::addTxRead2(const uint8_t reg_addr)
{
	m_tx_cmd.push_back(CMD_V2_READ_2);
	m_tx_cmd.push_back(reg_addr);
}

void __fastcall CNanoVNA2Comms::addTxRead4(const uint8_t reg_addr)
{
	m_tx_cmd.push_back(CMD_V2_READ_4);
	m_tx_cmd.push_back(reg_addr);
}

void __fastcall CNanoVNA2Comms::addTxRead8(const uint8_t reg_addr)
{
	m_tx_cmd.push_back(CMD_V2_READ_8);
	m_tx_cmd.push_back(reg_addr);
}

//void __fastcall CNanoVNA2Comms::addTxRead8(const uint8_t reg_addr)
//{
//	m_tx_cmd.push_back(CMD_V2_READ_8);
//	m_tx_cmd.push_back(reg_addr);
//}

void __fastcall CNanoVNA2Comms::addTxWrite1(const uint8_t reg_addr, uint8_t value)
{
	m_tx_cmd.push_back(CMD_V2_WRITE_1);
	m_tx_cmd.push_back(reg_addr);
	m_tx_cmd.push_back(value);
}

void __fastcall CNanoVNA2Comms::addTxWrite2(const uint8_t reg_addr, uint16_t value)
{
	m_tx_cmd.push_back(CMD_V2_WRITE_2);
	m_tx_cmd.push_back(reg_addr);
	m_tx_cmd.push_back((uint8_t)(value & 0xff)); value >>= 8;
	m_tx_cmd.push_back((uint8_t)(value & 0xff));
}

void __fastcall CNanoVNA2Comms::addTxWrite4(const uint8_t reg_addr, uint32_t value)
{
	m_tx_cmd.push_back(CMD_V2_WRITE_4);
	m_tx_cmd.push_back(reg_addr);
	m_tx_cmd.push_back((uint8_t)(value & 0xff)); value >>= 8;
	m_tx_cmd.push_back((uint8_t)(value & 0xff)); value >>= 8;
	m_tx_cmd.push_back((uint8_t)(value & 0xff)); value >>= 8;
	m_tx_cmd.push_back((uint8_t)(value & 0xff));
}

void __fastcall CNanoVNA2Comms::addTxWrite8(const uint8_t reg_addr, uint64_t value)
{
	m_tx_cmd.push_back(CMD_V2_WRITE_8);
	m_tx_cmd.push_back(reg_addr);
	m_tx_cmd.push_back((uint8_t)(value & 0xff)); value >>= 8;
	m_tx_cmd.push_back((uint8_t)(value & 0xff)); value >>= 8;
	m_tx_cmd.push_back((uint8_t)(value & 0xff)); value >>= 8;
	m_tx_cmd.push_back((uint8_t)(value & 0xff)); value >>= 8;
	m_tx_cmd.push_back((uint8_t)(value & 0xff)); value >>= 8;
	m_tx_cmd.push_back((uint8_t)(value & 0xff)); value >>= 8;
	m_tx_cmd.push_back((uint8_t)(value & 0xff)); value >>= 8;
	m_tx_cmd.push_back((uint8_t)(value & 0xff));
}

void __fastcall CNanoVNA2Comms::addTxUserArgument(const uint32_t arg)
{
//	addTxNulls();		                          	// reset the protocol to known state
	addTxWrite4(REG_DFU_V2_USER_ARGUMENT, arg);	// argument value
}

void __fastcall CNanoVNA2Comms::addTxFlashWriteAddr(const uint32_t addr)
{
	Form1->printfCommMessage("tx:flash write address %0X08", addr);

//	addTxNulls();		                              	// reset the protocol to known state
	addTxWrite4(REG_DFU_V2_FLASH_WRITE_START, addr);	// flash address
}

void __fastcall CNanoVNA2Comms::addTxWriteFlashData(const bool reset_buf, std::vector <uint8_t> &data, const bool ack)
{
	if (reset_buf)
		m_tx_cmd.resize(0);

	if (!data.empty() && data.size() <= 255)
	{
		Form1->printfCommMessage("tx:flash write data [%u]", data.size());

		// add the flash data
		m_tx_cmd.push_back(CMD_V2_WRITE_FIFO);
		m_tx_cmd.push_back(REG_DFU_V2_FLASH_FIFO);
		m_tx_cmd.push_back(data.size());
		for (unsigned int i = 0; i < data.size(); i++)
			m_tx_cmd.push_back(data[i]);

		// we want an ACK back after it's written the data
		if (ack)
			m_tx_cmd.push_back(CMD_V2_INDICATE);
	}
}

void __fastcall CNanoVNA2Comms::enterUSBDataMode(const bool send_nulls)
{
	Form1->pushCommMessage("tx: enter USB data mode");

	if (send_nulls)
		addTxNulls();                              // reset the protocol to known state
	else
		addTxNulls(true, 0);
	addTxWrite1(REG_V2_RAW_SAMPLES_MODE, 0x01);   // enter USB raw mode
	sendData();
	m_tx_cmd.resize(0);
	m_usb_data_mode = true;
}

void __fastcall CNanoVNA2Comms::leaveUSBDataMode(const bool send_nulls)
{
	Form1->pushCommMessage("tx: leave USB data mode");

	if (send_nulls)
		addTxNulls();                              // reset the protocol to known state
	else
		addTxNulls(true, 0);
	addTxWrite1(REG_V2_RAW_SAMPLES_MODE, 0x02);   // leave USB raw mode
	sendData();
	m_tx_cmd.resize(0);
	m_usb_data_mode = false;
}

void __fastcall CNanoVNA2Comms::setAverageSetting(int value)
{
	if (value < REG_V2_AVERAGE_SETTING_MIN) value = REG_V2_AVERAGE_SETTING_MIN;
	else
	if (value > REG_V2_AVERAGE_SETTING_MAX) value = REG_V2_AVERAGE_SETTING_MAX;

	Form1->printfCommMessage("tx: set average setting %d", value);

	addTxNulls(true, 0);
	addTxWrite1(REG_V2_AVERAGE_SETTING, value);
	sendData();
	m_tx_cmd.resize(0);
}

void __fastcall CNanoVNA2Comms::setSI5351OutputPower(int value)
{
	if (value < REG_V2_SI5351_POWER_MIN || value > REG_V2_SI5351_POWER_MAX)
		value = REG_V2_SI5351_POWER_MAX;

	Form1->printfCommMessage("tx: set Si5351 output power %d", value);

	addTxNulls(true, 0);
	addTxWrite1(REG_V2_SI5351_POWER, value);
	sendData();
	m_tx_cmd.resize(0);
}

void __fastcall CNanoVNA2Comms::setADF4350OutputPower(int value)
{
	if (value < REG_V2_ADF4350_POWER_MIN || value > REG_V2_ADF4350_POWER_MAX)
		value = REG_V2_ADF4350_POWER_MAX;

	Form1->printfCommMessage("tx: set ADF4350 output power %d", value);

	addTxNulls(true, 0);
	addTxWrite1(REG_V2_ADF4350_POWER, value);
	sendData();
	m_tx_cmd.resize(0);
}

void __fastcall CNanoVNA2Comms::setTime(int year, int month, int day, int hour, int min, int sec)
{
	uint32_t a=((14-month)/12);
	uint32_t y=year+4800-a;
	uint32_t m=month+(12*a)-3;
	uint32_t Uday=(day+((153*m+2)/5)+365*y+(y/4)-(y/100)+(y/400)-32045)-2440588;
	uint32_t time = Uday*60*60*24 + sec + min*60 + hour*3600;

	Form1->printfCommMessage("tx: set UNIX time %u", time);
	nanovna2_comms.addTxWrite4(REG_V2_UNIX_TIME, time);
	sendData();
	m_tx_cmd.resize(0);
}
void __fastcall CNanoVNA2Comms::softReboot()
{
	Form1->pushCommMessage("tx: request soft reboot");

	addTxNulls();		                       	// reset the protocol to known state
	addTxWrite1(REG_DFU_V2_DO_REBOOT, 0x5E);	// soft reboot the VNA
	sendData();
	m_tx_cmd.resize(0);
}

void __fastcall CNanoVNA2Comms::pauseComms(bool pause)
{
	const bool was_paused = m_pause_comms;
	m_pause_comms = pause;

	// reset the link timer so as not to immediately timeout the link when it gets unpaused
	if (Form1)
	{
		Form1->m_comms.rx_timer.mark();
		Form1->m_comms.link_timer.mark();
	}

	if (pause)
		return;

	// un-paused

	if (!was_paused)
		return;

	m_retries = 0;

	// was paused, but no more
	// do stuff that wasn't being done whilst we were paused

//	poll();

	if (m_mode != MODE_SCAN)
		return;

//	if (data_unit.m_vna_data.cmd_pause)
//		enterUSBDataMode();

	requestScan();
}

bool __fastcall CNanoVNA2Comms::requestCapture()
{	// request a screen capture from the VNA

	if (!Form1)
		return false;

	if (m_mode != MODE_IDLE)
		return false;

	if (!m_get_screen_capture)
		return false;

	m_get_screen_capture = false;

	memset(&m_cap_header, 0, sizeof(m_cap_header));

	setMode(MODE_CAPTURE);

	poll();

	return true;
}

void __fastcall CNanoVNA2Comms::requestScan()
{
	if (!Form1)
		return;

	const bool scanning   = (m_mode == MODE_SINGLE_SCAN || m_mode == MODE_SCAN) ? true : false;
	const bool generating = (m_mode == MODE_GENERATOR) ? true : false;

	const int segments           = data_unit.m_segments;
	const int segment            = data_unit.m_segment;
	const int points_per_segment = data_unit.m_points_per_segment;
	const int num_points         = data_unit.m_points;

	const uint64_t span          = scanning ? data_unit.m_freq_span_Hz / segments : 0;
	const uint64_t start         = scanning ? data_unit.m_freq_start_Hz + (span * segment) : data_unit.m_freq_cw_Hz;

	if (segments < 1 || points_per_segment < 8)
		return;

	const uint64_t step = span / (points_per_segment - 1);
	const int points_per_frequency = 1;

	const bool new_start = (m_start != start ||
									m_step != step ||
									m_points_per_segment != points_per_segment ||
									m_data_points_per_frequency != points_per_frequency) ? true : false;

	// global buffer
	Form1->m_freq_data_list.resize(points_per_segment);   // buffer size to the number of points we are to gather from the VNA
	{
		t_data_point fp;
		for (unsigned int i = 0; i < Form1->m_freq_data_list.size(); i++)
			Form1->m_freq_data_list[i] = fp;
	}

	// clear the incoming buffer
	if (segment <= 0)
		data_unit.m_point_incoming.resize(0);

	if (new_start)
	{
		if (Form1)
			Form1->pushCommMessage("tx: setting scan parameters");

		// reset protocol to known state
		addTxNulls();

		// clear the FIFO
		//addTxWrite1(REG_V2_VALUES_FIFO, 0);

		// start frequency
		m_start = start;
		addTxWrite8(REG_V2_SWEEP_START_HZ, start);

		// step frequency
		m_step = step;
		addTxWrite8(REG_V2_SWEEP_STEP_HZ, step);

		// number of points
		m_points_per_segment = points_per_segment;
		addTxWrite2(REG_V2_SWEEP_POINTS, points_per_segment);

		// data points per frequency
		m_data_points_per_frequency = points_per_frequency;
		addTxWrite2(REG_V2_VALUES_PER_FREQUENCY, points_per_frequency);

		sendData();
		m_tx_cmd.resize(0);
	}

	m_points_requested = requestPoints(points_per_segment);
}

int __fastcall CNanoVNA2Comms::requestPoints(int num_points)
{
	// request a number of frequency scanned points

	m_tx_cmd.resize(0);

	if (num_points <= 0)
		return 0;

	if (Form1)
		Form1->printfCommMessage("tx: requesting %d points ..", num_points);

	// reset protocol to known state
//	addTxNulls();
	addTxNulls(true, 0);

	// clear the FIFO
//	addTxWrite1(REG_V2_VALUES_FIFO, 0);

	m_tx_cmd.push_back(CMD_V2_READ_FIFO);
	m_tx_cmd.push_back(REG_V2_VALUES_FIFO);
	m_tx_cmd.push_back(0);	// tell it to send ALL the scan points in one go
/*
	int points = num_points;
	while (points > 0)
	{
		// we can only request up to a maximum of 255 points at a time
		int n = points;
		if (n > 255)
			n = 255;
//		if (n > 128)
//			n = 128;

		// request num_points
		m_tx_cmd.push_back(CMD_V2_READ_FIFO);
		m_tx_cmd.push_back(REG_V2_VALUES_FIFO);
		m_tx_cmd.push_back(n);

		points -= n;
	}
  */
	sendData();
	m_tx_cmd.resize(0);

	return num_points;
}

void __fastcall CNanoVNA2Comms::poll()
{
	m_poll_timer.mark();

	if (!Form1)
		return;

	switch (m_mode)
	{
		case MODE_NONE:
			if (m_new_mode != m_mode)
			{
				m_tx_cmd.resize(0);
				m_retries = 0;
				m_mode = m_new_mode;
			}
			break;

		case MODE_INIT1:	// request a simple ACK to see if it's a V2 we're connected too
/*			if (m_new_mode != m_mode)
			{
				m_tx_cmd.resize(0);
				m_retries = 0;
				m_mode = m_new_mode;
			}
			else
*/			{
				// leave USB raw mode
				leaveUSBDataMode(true);

				// request an simple ACK
				Form1->pushCommMessage("tx: are you a V2 ?");
				addTxNulls(true, 0);
				m_tx_cmd.push_back(CMD_V2_INDICATE);
				sendData();
				m_tx_cmd.resize(0);

				m_retries++;
			}
			break;

		case MODE_INIT2:	// get the type of device
/*			if (m_new_mode != m_mode)
			{
				m_tx_cmd.resize(0);
				m_retries = 0;
				m_mode = m_new_mode;
			}
			else
*/			{
				Form1->pushCommMessage("tx: requesting device variant");
				m_tx_cmd.resize(0);
				addTxRead1(REG_V2_DEVICE_VARIANT);
				sendData();
				m_tx_cmd.resize(0);
				m_retries++;
			}
			break;

		case MODE_INIT3:	// get the serial protocol version
/*			if (m_new_mode != m_mode)
			{
				m_tx_cmd.resize(0);
				m_retries = 0;
				m_mode = m_new_mode;
			}
			else
*/			{
				Form1->pushCommMessage("tx: requesting serial protocol version");
				m_tx_cmd.resize(0);
				addTxRead1(REG_V2_PROTCOL_VERSION);
				sendData();
				m_tx_cmd.resize(0);
				m_retries++;
			}
			break;

		case MODE_INIT4:	// get the hardware revision and the firmware major and minor version
/*			if (m_new_mode != m_mode)
			{
				m_tx_cmd.resize(0);
				m_retries = 0;
				m_mode = m_new_mode;
			}
			else
*/			{
				Form1->pushCommMessage("tx: requesting hw-revision and fw-version");
				m_tx_cmd.resize(0);
				addTxRead1(REG_V2_HARDWARE_REVISION);
				addTxRead1(REG_V2_FIRMWARE_MAJOR);
				addTxRead1(REG_V2_FIRMWARE_MINOR);
				sendData();
				m_tx_cmd.resize(0);
				m_retries++;
			}
			break;

		case MODE_IDLE:
/*			if (m_new_mode != m_mode)
			{
				m_tx_cmd.resize(0);
				m_retries = 0;
				m_mode = m_new_mode;
			}
			else
*/			{
			}
			break;

		case MODE_POLL:
/*			if (m_new_mode != m_mode)
			{
				m_tx_cmd.resize(0);
				m_retries = 0;
				m_mode = m_new_mode;
			}
			else
*/			if (!m_pause_comms)
			{
				Form1->pushCommMessage("tx: polling - requesting hw-revision and fw-version");
//				addTxNulls();
				addTxNulls(true, 0);
				addTxRead1(REG_V2_HARDWARE_REVISION);
				addTxRead1(REG_V2_FIRMWARE_MAJOR);
				addTxRead1(REG_V2_FIRMWARE_MINOR);
				sendData();
				m_tx_cmd.resize(0);
				m_retries++;
			}
			break;

		case MODE_SINGLE_SCAN:
		case MODE_SCAN:
      case MODE_GENERATOR:
/*			if (m_new_mode != m_mode)
			{
				m_tx_cmd.resize(0);
				m_retries = 0;
				m_mode = m_new_mode;
			}
			else
*/			{
			}
			break;

		case MODE_CAPTURE:
/*			if (m_new_mode != m_mode)
			{
				m_tx_cmd.resize(0);
				m_retries = 0;
				m_mode = m_new_mode;
			}
			else
*/			{
				Form1->pushCommMessage("tx: requesting a screen capture");
				memset(&m_cap_header, 0, sizeof(m_cap_header));
//				addTxNulls();
				addTxNulls(true, 0);
				addTxWrite1(REG_V2_CAPTURE_SCREEN, 0);
				sendData();
				m_tx_cmd.resize(0);
				m_retries++;
			}
			break;

		case MODE_DFU_MODE:		// don't do anything while the other window is uploading flash firmware
			break;

		default:
			setMode(MODE_IDLE);
			break;
	}
}

void __fastcall CNanoVNA2Comms::newUnit()
{
	String s;

	if (Form1)
	{
		s = IntToHex((int)data_unit.m_vna_data.hardware_revision, 2) + " ..... HARDWARE REVISION";
		Form1->pushCommMessage("rx: " + s);

		s = IntToHex((int)data_unit.m_vna_data.firmware_major, 2) + " " + IntToHex((int)data_unit.m_vna_data.firmware_minor, 2) + " .. FIRMWARE VERSION";
		Form1->pushCommMessage("rx: " + s);
		Form1->pushCommMessage("rx: [connected to a V2]");
	}

	// ****************

	data_unit.m_vna_data.type = UNIT_TYPE_NANOVNA_V2;

	data_unit.m_vna_data.name = "NanoVNA-";
	switch (data_unit.m_vna_data.hardware_revision)
	{
		case REG_V2_HARDWARE_REVISION_ACK_2_2:
			if (data_unit.m_vna_data.firmware_major == 2) {
				data_unit.m_vna_data.name += "LiteVNA";
				data_unit.m_vna_data.max_points = 65535;
//				data_unit.m_vna_data.hardware_revision = REG_V2_HARDWARE_REVISION_ACK_2_4;
			} else {
				data_unit.m_vna_data.name += "V2.2";
				data_unit.m_vna_data.max_points = 1024;
			}
			break;
		case REG_V2_HARDWARE_REVISION_ACK_2_3:
			data_unit.m_vna_data.name += "V2Plus";
			data_unit.m_vna_data.max_points = 1024;
			break;
		case REG_V2_HARDWARE_REVISION_ACK_2_4:
			data_unit.m_vna_data.name += "V2Plus4";
			data_unit.m_vna_data.max_points = 65535;
			break;
		case REG_DFU_V2_HARDWARE_REVISION_ACK:
			data_unit.m_vna_data.name += "V2-DFU";
			data_unit.m_vna_data.max_points = 201;
			break;
		default:
			data_unit.m_vna_data.name += "?";
			data_unit.m_vna_data.max_points = 1024;
			break;
	}

	data_unit.m_vna_data.version.printf(L"v%u.%u", data_unit.m_vna_data.firmware_major, data_unit.m_vna_data.firmware_minor);

	data_unit.m_vna_data.info.resize(0);
	data_unit.m_vna_data.info.push_back(data_unit.m_vna_data.name);
	s.printf(L"protocol   v%u", data_unit.m_vna_data.protool_version);
	data_unit.m_vna_data.info.push_back(s);
	s.printf(L"hardware   r%u", data_unit.m_vna_data.hardware_revision);
	data_unit.m_vna_data.info.push_back(s);
	s.printf(L"firmware   v%u.%u", data_unit.m_vna_data.firmware_major, data_unit.m_vna_data.firmware_minor);
	data_unit.m_vna_data.info.push_back(s);
	s.printf(L"max points %d", data_unit.m_vna_data.max_points);
	data_unit.m_vna_data.info.push_back(s);

	// tell the exec we have found a VNA
	if (Application->MainForm)
		::PostMessage(Application->MainForm->Handle, WM_NEW_UNIT_TYPE, 0, 0);

	// ****************

	if (inDFUMode() && Form1)
		Form1->pushCommMessage("rx: the unit is in DFU mode");

	setMode(MODE_IDLE);	// onto the next stage
}

int __fastcall CNanoVNA2Comms::processRx(t_serial_buffer &serial_buffer)
{
	String s;

	if (m_mode == MODE_NONE)
		return 0;

	if (!Form1)
		return 0;	// if the main form (window) is not there we can't use any of it's functions

	const int size = serial_buffer.buffer_wr;	// number of available RX bytes
	int k = 0;

	if (size <= 0)
		return 0;	// no data received

	// process any received data

	if (m_mode == MODE_INIT1)
	{	// just want a simple reply

		const uint8_t b = serial_buffer.buffer[k++];		// fetch an RX byte

		s = IntToHex((int)b, 2);
		if (b == CMD_V2_INDICATE_ACK)
			s += " .. ACK OK";
		Form1->pushCommMessage("rx: " + s);

		if (b == CMD_V2_INDICATE_ACK)
		{	// received the correct response
			setMode(MODE_INIT2);	// onto the next stage
			poll();
			m_tx_cmd.resize(0);
		}

	}
	else
	if (m_mode == MODE_INIT2)
	{	// fetching the device variant

		const uint8_t b = serial_buffer.buffer[k++];		// fetch an RX byte

		s = IntToHex((int)b, 2);
		if (b == REG_V2_DEVICE_VARIANT_ACK)
			s += " .. DEVICE VARIANT OK";
		Form1->pushCommMessage("rx: " + s);

		if (b == REG_V2_DEVICE_VARIANT_ACK)
		{	// received the correct response
			setMode(MODE_INIT3);	// onto the next stage

			poll();
			m_tx_cmd.resize(0);
		}

	}
	else
	if (m_mode == MODE_INIT3)
	{	// fetching the serial protocol version

		const uint8_t b = serial_buffer.buffer[k++];		// fetch an RX byte
		data_unit.m_vna_data.protool_version = b;

		s = IntToHex((int)b, 2);
		if (b == REG_V2_PROTOCOL_VERSION_ACK)
			s += " .. PROTOCOL VERSION OK";
		Form1->pushCommMessage("rx: " + s);

		if (b == REG_V2_PROTOCOL_VERSION_ACK)
		{	// received the correct response
			setMode(MODE_INIT4);	// onto the next stage

			poll();
			m_tx_cmd.resize(0);
		}

	}
	else
	if (m_mode == MODE_INIT4)
	{	// fetching the hardware revision

		if (size >= 3)
		{	// we have received enough bytes

			// fetch them
			const uint8_t b1 = serial_buffer.buffer[k++];
			const uint8_t b2 = serial_buffer.buffer[k++];
			const uint8_t b3 = serial_buffer.buffer[k++];

			data_unit.m_vna_data.hardware_revision = b1;
			data_unit.m_vna_data.firmware_major    = b2;
			data_unit.m_vna_data.firmware_minor    = b3;

			newUnit();
		}
	}
	else
	if (m_mode == MODE_IDLE)
	{	// we're not doing anything with the VNA, just idling along

		#if 1
			// display all the received bytes (as hex)
			s = "";
			int i = 0;
			while (k < (int)serial_buffer.buffer_wr)
			{
				String s2;
				s2.printf(L" %02x", serial_buffer.buffer[k++]);
				s += s2;
				if (++i >= (int)sizeof(t_v2_fifo_sparam))
				{
					i = 0;
					Form1->pushCommMessage("rx: *" + s);
					s = "";
				}
			}
			if (!s.IsEmpty())
			{
				Form1->pushCommMessage("rx: *" + s);
				s = "";
			}
		#endif

	}
	else
	if (m_mode == MODE_POLL)
	{	// we are polling the VNA to see if it's still there and working OK

		if (size >= 3)
		{	// we have received enough bytes

			// fetch them
			const uint8_t b1 = serial_buffer.buffer[k++];
			const uint8_t b2 = serial_buffer.buffer[k++];
			const uint8_t b3 = serial_buffer.buffer[k++];

			const bool was_dfu_mode = inDFUMode();
			const bool now_dfu_mode = (b1 == REG_DFU_V2_HARDWARE_REVISION_ACK && b2 == REG_DFU_V2_FIRMWARE_MAJOR_ACK) ? true : false;

			if (b1 == data_unit.m_vna_data.hardware_revision && b2 == data_unit.m_vna_data.firmware_major && b3 == data_unit.m_vna_data.firmware_minor)
			{	// no change in V2 mode (normal/DFU)
				s = IntToHex((int)b1, 2) + " " + IntToHex((int)b2, 2) + " " + IntToHex((int)b3, 2) + " .. POLL OK";
				Form1->pushCommMessage("rx: " + s);

				setMode(MODE_IDLE);
			}
			else
			if (was_dfu_mode != now_dfu_mode)
			{	// the V2 has changed between normal mode and DFU mode
				data_unit.m_vna_data.hardware_revision = b1;
				data_unit.m_vna_data.firmware_major    = b2;
				data_unit.m_vna_data.firmware_minor    = b3;

				newUnit();
			}
		}
	}
	else
	if (m_mode == MODE_SINGLE_SCAN || m_mode == MODE_SCAN || m_mode == MODE_GENERATOR)
	{	// we are in scan/generator mode - lots of s-param points coming in

		const bool generating = (m_mode == MODE_GENERATOR) ? true : false;

		const int fifo_size = sizeof(t_v2_fifo_sparam);	// the number of bytes per frequency point

		while ((size - k) >= fifo_size)
		{	// we have received an s-param point
			String s;
			String s2;

			const uint8_t *data          = (uint8_t *)&serial_buffer.buffer[k];
			const t_v2_fifo_sparam *fifo = (t_v2_fifo_sparam *)data;

			uint8_t checksum = 0x46;
			for (int i = 0; i < (fifo_size - 1); i++)
				checksum = (checksum ^ ((checksum << 1) | 1u)) ^ data[i];
			const bool checksum_ok = (checksum == fifo->checksum) ? true : false;

			if (!checksum_ok)
			{
				for (int i = 0; i < fifo_size; i++)
				{
					s2.printf(L" %02X", data[i]);
					s += s2;
				}
				s = s.Trim();

				if (!checksum_ok)
				{	// checksum error
					s2.printf(L" - %02X", checksum);
					s += s2;
					Form1->pushCommMessage("rx: checksum error " + s);
				}
				else
				{
					Form1->pushCommMessage("rx: " + s);
				}
			}
			else
			{	// checksum is OK
				// process the RX'ed s-param
				const complexf fwd0((float)fifo->real_ch0_fwd, (float)fifo->imag_ch0_fwd);
				const complexf rev0 = complexf ((float)fifo->real_ch0_rev, (float)fifo->imag_ch0_rev) / fwd0;
				const complexf rev1 = complexf ((float)fifo->real_ch1_rev, (float)fifo->imag_ch1_rev) / fwd0;

				const int segments           = data_unit.m_segments;
				const int segment            = data_unit.m_segment;
				const int points_per_segment = data_unit.m_points_per_segment;
				const int num_points         = data_unit.m_points;

				const double seg_span  = generating ? 0 : (double)data_unit.m_freq_span_Hz / segments;
				const double seg_start = generating ? data_unit.m_freq_cw_Hz : (double)data_unit.m_freq_start_Hz + (seg_span * segment);

				t_data_point fp;
				fp.Hz  = I64ROUND(seg_start + ((seg_span * fifo->freq_index) / (points_per_segment - 1)));
				fp.s11 = rev0;
				fp.s21 = rev1;

				#if 0
					s.printf(L"%5d", fifo->freq_index);
					s2.printf(L" %11lld", fp.Hz); s += s2;
					#if 0
						s2.printf(L" %08X", fifo->real_ch0_fwd); s += s2;
						s2.printf(L" %08X", fifo->imag_ch0_fwd); s += s2;
						s += " ";
						s2.printf(L"%0.9e", rev0.real); s += " " + common.padLeft(s2, 17);
						s2.printf(L"%0.9e", rev0.imag); s += " " + common.padLeft(s2, 17);
						s += ",";
						s2.printf(L"%0.9e", rev1.real); s += " " + common.padLeft(s2, 17);
						s2.printf(L"%0.9e", rev1.imag); s += " " + common.padLeft(s2, 17);
					#endif
					Form1->pushCommMessage("rx: " + s);
				#endif

				const int index = fifo->freq_index;

				if ((int)data_unit.m_point_incoming.size() != num_points)
				{
					data_unit.m_point_incoming.resize(num_points);
					memset(&data_unit.m_point_incoming[0], 0, sizeof(data_unit.m_point_incoming[0]) * num_points);
				}

				// save the new point into the incoming buffer
				if (index < points_per_segment)
				{
					const int pos = (points_per_segment * segment) + index;
					if (pos < (int)data_unit.m_point_incoming.size())
					{
						data_unit.m_point_incoming[pos] = fp;

						if (m_new_points_timer.millisecs() >= 100)
						{	// give the user some feedback that we are actually receiving scanned points
							m_new_points_timer.mark();
							::PostMessage(Form1->Handle, WM_INCOMING_POINTS, (WPARAM)index, 0);
						}
					}
				}

				if (index >= (points_per_segment - 1) && segment >= (segments - 1))
				{
					#if 1
						s.printf(L"%d points received", num_points);
						Form1->pushCommMessage("rx: " + s);
					#endif
					Form1->addNewRxData(data_unit.m_point_incoming);
					data_unit.m_point_incoming.resize(0);
				}
			}

			k += fifo_size;
		}

	}
	else
	if (m_mode == MODE_CAPTURE)
	{	// we are in screen capture mode

		if (m_cap_header.width == 0 || m_cap_header.height == 0 || m_cap_header.pixel_format == 0)
		{	// we are waiting for the image header to arrive

			if (size >= (int)sizeof(m_cap_header))
			{	// we have received the header

				// copy the header into our own header buffer
				memcpy(&m_cap_header, &serial_buffer.buffer[k], sizeof(m_cap_header));
				k += sizeof(m_cap_header);

				// sanity check the header values, assume 4096 maximum ever image width/height
				if (m_cap_header.width < 320 || m_cap_header.height < 240 || m_cap_header.width > 4096 || m_cap_header.height > 4096)
				{	// header error
					s.printf(L"screen capture header %u*%u*%u - header error", m_cap_header.width, m_cap_header.height, m_cap_header.pixel_format);
					Form1->pushCommMessage("rx: " + s);

					// stop image capture
					memset(&m_cap_header, 0, sizeof(m_cap_header));
					setMode(MODE_IDLE);
				}

				if (m_cap_header.pixel_format == 16)
				{	// 16-bit pixel format
					s.printf(L"screen capture header %u*%u*%u, %d bytes to follow ..", m_cap_header.width, m_cap_header.height, m_cap_header.pixel_format, m_cap_header.width * 2 * m_cap_header.height);
					Form1->pushCommMessage("rx: " + s);
				}
				else
				{	// unknown pixel format
					s.printf(L"screen capture header %u*%u*%u - unknown pixel format", m_cap_header.width, m_cap_header.height, m_cap_header.pixel_format);
					Form1->pushCommMessage("rx: " + s);

					// stop image capture
					memset(&m_cap_header, 0, sizeof(m_cap_header));
					setMode(MODE_IDLE);
				}
			}
		}

		int image_size = 0;

		if (m_cap_header.width > 0 && m_cap_header.height > 0 && m_cap_header.pixel_format > 0)
		{	// we have the header - now waiting for the image data to arrive

			if (m_cap_header.pixel_format == 16)
			{	// 16-bit pixel format
				image_size = m_cap_header.width * 2 * m_cap_header.height;
			}
			else
			{	// unknown pixel format
				// stop image capture
				memset(&m_cap_header, 0, sizeof(m_cap_header));
				setMode(MODE_IDLE);
			}
		}

		if (image_size > 0 && size >= image_size)
		{	// we have received the image data

			// point to the image data
			uint16_t *data = (uint16_t *)(&serial_buffer.buffer[k]);
			k += image_size;

			if (m_cap_header.pixel_format == 16)
			{	// 16-bit pixel format

				if (m_capture_bm == NULL)
					m_capture_bm = new Graphics::TBitmap();
				if (m_capture_bm)
				{
					m_capture_bm->Monochrome  = false;
					m_capture_bm->Transparent = false;
					m_capture_bm->PixelFormat = pf16bit;
					m_capture_bm->Width       = m_cap_header.width;
					m_capture_bm->Height      = m_cap_header.height;

					for (int y = 0; y < m_capture_bm->Height; y++) {
						uint16_t *dst = (uint16_t *)m_capture_bm->ScanLine[y];
						for (int x = 0; x < m_capture_bm->Width; x++, data++){
							*dst++ = (*data<<8)|(*data>>8);
						}
					}
					s.printf(L"screen captured OK %u*%u*%u, %d bytes", m_cap_header.width, m_cap_header.height, m_cap_header.pixel_format, image_size);
					Form1->pushCommMessage("rx: " + s);

					// tell the exec there is a new image for it to show to the user
					::PostMessage(Form1->Handle, WM_SCREEN_CAPTURE, 0, 0);
				}
			}

			// back to idle mode
			memset(&m_cap_header, 0, sizeof(m_cap_header));
			setMode(MODE_IDLE);
		}

	}
	else
	if (m_mode == MODE_DFU_MODE)
	{
	}
	else
	{  // we're in an unknown mode - should never arrive here
		memset(&m_cap_header, 0, sizeof(m_cap_header));
		setMode(MODE_IDLE);
	}

	if (k > 0)
	{	// remove used data from the rx buffer
		if (k < (int)serial_buffer.buffer_wr)
		{
			memmove(&serial_buffer.buffer[0], &serial_buffer.buffer[k], serial_buffer.buffer_wr - k);
			serial_buffer.buffer_wr -= k;
		}
		else
			serial_buffer.buffer_wr = 0;
		//k = 0;
	}

	return 0;
}

