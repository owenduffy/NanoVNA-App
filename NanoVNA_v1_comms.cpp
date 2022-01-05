
#pragma hdrstop

#include "NanoVNA_v1_comms.h"
#include "CommsUnit.h"
#include "DataUnit.h"
#include "Unit1.h"
#include "SettingsUnit.h"
#include "VNAUsartUnit.h"
#include "BatteryVoltageUnit.h"

#pragma package(smart_init)

#ifdef _DEBUG
	//#define SHOW_CMD_START_END
#endif

#define SCAN_MASK_OUT_FREQ       (1u << 0)
#define SCAN_MASK_OUT_DATA0      (1u << 1)
#define SCAN_MASK_OUT_DATA1      (1u << 2)
#define SCAN_MASK_NO_CALIBRATION (1u << 3)
#define SCAN_MASK_BINARY         (1u << 7)

#define COMMAND_PROMPT "ch>"

CNanoVNA1Comms nanovna1_comms;

CNanoVNA1Comms::CNanoVNA1Comms()
{
	m_capture_bm         = NULL;
	m_pause_comms        = false;
	m_get_screen_capture = false;
	m_mode               = MODE_NONE;
	m_rx_block.type      = SERIAL_STATE_IDLE;
}

CNanoVNA1Comms::~CNanoVNA1Comms()
{
	if (m_capture_bm)
		delete m_capture_bm;
	m_capture_bm = NULL;
}

String __fastcall CNanoVNA1Comms::getSerialStateString(t_serial_state state)
{
	String s;
	switch (state)
	{
		case SERIAL_STATE_IDLE:					s = "idle";           break;
		case SERIAL_STATE_HELP:					s = "help";           break;
		case SERIAL_STATE_VERSION:				s = "version";        break;
		case SERIAL_STATE_INFO:					s = "info";           break;
		case SERIAL_STATE_MARKER:				s = "marker";         break;
		case SERIAL_STATE_SWEEP:				s = "sweep";          break;
		case SERIAL_STATE_STAT:					s = "stat";           break;
		case SERIAL_STATE_TIME:					s = "time";           break;
		case SERIAL_STATE_VBAT:					s = "vbat";           break;
		case SERIAL_STATE_VBAT_OFFSET:		s = "vbat_offset";    break;
		case SERIAL_STATE_INTEGRATOR:			s = "integrator";     break;
		case SERIAL_STATE_BANDWIDTH:			s = "bandwidth";      break;
		case SERIAL_STATE_THRESHOLD:			s = "threshold";      break;
		case SERIAL_STATE_EDELAY:				s = "edelay";         break;
		case SERIAL_STATE_DEVICE_ID:			s = "deviceid";       break;
		case SERIAL_STATE_CAL:					s = "cal";            break;
		case SERIAL_STATE_POWER:				s = "power";          break;
      case SERIAL_STATE_USART:            s = "uart";           break;
      case SERIAL_STATE_USART_CFG:        s = "uart_cfg";       break;
		case SERIAL_STATE_CLEARCONFIG:		s = "clearconfig";    break;
		case SERIAL_STATE_SAVECONFIG:			s = "saveconfig";     break;
		case SERIAL_STATE_SCAN:					s = "scan";           break;
		case SERIAL_STATE_SCAN_BIN:			s = "scan_bin";       break;
		case SERIAL_STATE_SCANRAW:				s = "scanraw";        break;
		case SERIAL_STATE_PAUSE:				s = "pause";          break;
		case SERIAL_STATE_RESUME:				s = "resume";         break;
		case SERIAL_STATE_RESET:				s = "reset";          break;
		case SERIAL_STATE_DATA0:				s = "data 0";         break;
		case SERIAL_STATE_DATA1:				s = "data 1";         break;
		case SERIAL_STATE_DATA2:				s = "data 2";         break;
		case SERIAL_STATE_DATA3:				s = "data 3";         break;
		case SERIAL_STATE_DATA4:				s = "data 4";         break;
		case SERIAL_STATE_DATA5:				s = "data 5";         break;
		case SERIAL_STATE_DATA6:				s = "data 6";         break;
		case SERIAL_STATE_DATA7:				s = "data 7";         break;
		case SERIAL_STATE_FREQDATA:			s = "freq_data";      break;
		case SERIAL_STATE_FREQDATA_BIN:		s = "freq_data_bin";  break;
		case SERIAL_STATE_FREQDATA_RAW:		s = "freq_data_raw";  break;
		case SERIAL_STATE_SCREEN_CAPTURE:	s = "screen_capture"; break;
		case SERIAL_STATE_SCREEN_FILL:      s = "fill"; break;
		case SERIAL_STATE_SCREEN_BULK:      s = "bulk"; break;
		case SERIAL_STATE_SD_LIST:				s = "sd_list";        break;
		case SERIAL_STATE_SD_READFILE:		s = "sd_readfile";    break;
		case SERIAL_STATE_MODE:					s = "mode";           break;
		default:										s = "unknown";        break;
	}
	return s;
}

void __fastcall CNanoVNA1Comms::reset()
{
	m_tx_command = "";
	m_tx_commands.resize(0);

	m_rx_string        = "";
	m_pause_comms      = false;
	m_poll_ms          = DEFAULT_POLL_V1_MS;
	m_state_timeout_ms = DEFAULT_STATE_TIME_OUT_V1_MS;
	m_link_timeout_ms  = DEFAULT_LINK_TIME_OUT_V1_MS;
	m_mode             = MODE_INIT1;

	m_poll_timer.mark();

	resetRx();
}

void __fastcall CNanoVNA1Comms::resetRx()
{
	m_rx_string = "";
	m_rx_block.type = SERIAL_STATE_IDLE;
	m_rx_block.lines.resize(0);
	m_rx_block.bin_data.resize(0);
	m_rx_block.bin_data_index = 0;
	m_rx_block.timer.mark();
}

void __fastcall CNanoVNA1Comms::setMode(const t_mode mode)
{
//	const t_mode prev_mode = m_mode;
	m_mode = mode;
//	if (mode != prev_mode)
//	{
//		m_retries = 0;
//		if (prev_mode > MODE_IDLE && prev_mode != MODE_POLL && mode == MODE_IDLE)
//		{
//			m_mode = MODE_POLL;
//			poll();
//		}
//	}
}

void __fastcall CNanoVNA1Comms::pauseComms(bool pause)
{
	const bool was_paused = m_pause_comms;
	m_pause_comms  = pause;

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

	// was paused, but no more
	// do stuff that wasn't being done whilst we were paused

	poll();

	if (m_mode != MODE_SCAN)
		return;

	if (data_unit.m_vna_data.cmd_pause)
		addSerialTxCommand("pause");

	requestScan();
}

int __fastcall CNanoVNA1Comms::serialCommandsSize()
{
	return (m_tx_command.IsEmpty()) ? (int)m_tx_commands.size() : 1 + (int)m_tx_commands.size();
}

void __fastcall CNanoVNA1Comms::insertSerialTxCommand(String s, const int command_ends)
{
	s = s.Trim();

	// remove any trailing "\r\n"
	while (s.Length() >= 2)
	{
		const int i = s.Pos("\r\n");
		if (i < (s.Length() - 2))
			break;
		s = s.SubString(i, s.Length() - 2).Trim();
	}

	if (s.IsEmpty())
		return;

	switch (command_ends)
	{
		case 0: s += "\r\n"; break;
		case 1: s += "\n\r"; break;
		case 2: s += "\n";   break;
		case 3: s += "\r";   break;
	}
	// insert the command at as the first to next be sent
	m_tx_commands.insert(m_tx_commands.begin() + 0, s);
}

void __fastcall CNanoVNA1Comms::addSerialTxCommand(String s, const int command_ends)
{
	s = s.Trim();

	// remove any trailing "\r\n"
	while (s.Length() >= 2)
	{
		const int i = s.Pos("\r\n");
		if (i < (s.Length() - 2))
			break;
		s = s.SubString(i, s.Length() - 2).Trim();
	}

//	if (s.IsEmpty())
//		return;

	switch (command_ends)
	{
		case 0: s += "\r\n"; break;
		case 1: s += "\n\r"; break;
		case 2: s += "\n";   break;
		case 3: s += "\r";   break;
	}
	m_tx_commands.push_back(s);
}

void __fastcall CNanoVNA1Comms::sendOutputPowerCommand(int power)
{
	if (!data_unit.m_vna_data.cmd_power)
		return;

	const int value = (power < 0) ? 255 : power;
	String s;
	s.printf(L"power %d", value);
	addSerialTxCommand(s);
}

void __fastcall CNanoVNA1Comms::sendBandwidthCommand()
{
	if (!data_unit.m_vna_data.cmd_bandwidth)
		return;

	String s;

	if (data_unit.m_vna_data.adc_Hz <= 0 || data_unit.m_vna_data.audio_samples_count <= 0 || data_unit.m_vna_data.bandwidth_Hz <= 0)
		return;

	int Hz = data_unit.m_bandwidth_Hz;
	if (Hz > data_unit.m_vna_data.max_bandwidth_Hz) Hz = data_unit.m_vna_data.max_bandwidth_Hz;
	else
	if (Hz < 10) Hz = 10;

	int divider = (data_unit.m_vna_data.adc_Hz / data_unit.m_vna_data.audio_samples_count) / (Hz + 1);
//	int divider = (BW_HZ / Hz) - 1;
	if (divider < 0)
		divider = 0;

	s.printf(L"bandwidth %d", divider);
//	s.printf(L"bandwidth %d Hz", data_unit.m_bandwidth_Hz);
	addSerialTxCommand(s);

//	addSerialTxCommand("bandwidth");
}

bool __fastcall CNanoVNA1Comms::requestCapture()
{	// request a screen capture from the VNA

	if (!m_get_screen_capture || m_rx_block.type != SERIAL_STATE_IDLE)
		return false;

	m_get_screen_capture = false;

	if (!data_unit.m_vna_data.cmd_capture)
		return false;

	addSerialTxCommand("capture");

	return true;
}

void __fastcall CNanoVNA1Comms::setCW(int64_t Hz, int power)
{
	String s;

//	if (m_poll_timer.millisecs(false) >= m_poll_ms)
//		poll();

	s.printf(L"scan %lld %lld", Hz, Hz);
	addSerialTxCommand(s);

	if (data_unit.m_vna_data.cmd_power)
	{
		const int value = (power < 0) ? 255 : power;
		s.printf(L"power %d", value);
		addSerialTxCommand(s);
	}
}

void __fastcall CNanoVNA1Comms::requestScan()
{
//	const int segments           = data_unit.m_segments;
	const int segment            = data_unit.m_segment;
//	const int points_per_segment = data_unit.m_points_per_segment;
	const int num_points         = data_unit.m_points;

	const int segments = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? data_unit.m_segments : 1;

	// clear the incoming buffer
	if (segment <= 0)
		data_unit.m_point_incoming.resize(0);

	if (segments <= 0)
		return;

	const uint32_t seg_span  = data_unit.m_freq_span_Hz / segments;
	const uint32_t seg_start = data_unit.m_freq_start_Hz + (seg_span * data_unit.m_segment);
	const uint32_t seg_stop  = seg_start + seg_span;

	if (m_poll_timer.millisecs(false) >= m_poll_ms)
		poll();

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
	{

	}
	else
	if (data_unit.m_vna_data.type == UNIT_TYPE_TINYSA)
	{	// TinySA
		uint16_t mask = 0x07;
		String s;
		if (data_unit.m_vna_data.cmd_scanraw)
			s.printf(L"scanraw %u %u %d", seg_start, seg_stop, data_unit.m_points);
		else
			s.printf(L"scan %u %u %d %u", seg_start, seg_stop, data_unit.m_points, mask);
		addSerialTxCommand(s);
	}
	else
	{	// NanoVNA
		uint16_t mask = 0;

		#if 0
			// only request S11 data if the graph is using S11 data
			if (settings.timeAverageLevel > 0 || GraphTypeComboBox1->Text.LowerCase().Pos("s11") > 0 || GraphTypeComboBox2->Text.LowerCase().Pos("s11") > 0)
				mask |= SCAN_MASK_OUT_DATA0;
		#else
			mask |= SCAN_MASK_OUT_DATA0;
		#endif

		#if 0
			// only request S21 data if the graph is using S21 data
			if (settings.timeAverageLevel > 0 || GraphTypeComboBox1->Text.LowerCase().Pos("s21") > 0 || GraphTypeComboBox2->Text.LowerCase().Pos("s21") > 0)
				mask |= SCAN_MASK_OUT_DATA1;
		#else
			mask |= SCAN_MASK_OUT_DATA1;
		#endif

		if (settings.calibrationSelection != CAL_SELECT_VNA)
			mask |= SCAN_MASK_NO_CALIBRATION;	// don't use the VNA's own calibration

		//mask |= SCAN_MASK_BINARY;

		#if 0
			// only request the frequency data on the first scan
			if (data_unit.m_total_frames <= 0)
		#endif
				mask |= SCAN_MASK_OUT_FREQ;

		String s;
		s.printf(L"%u %u %d %u", seg_start, seg_stop, data_unit.m_points_per_segment, mask);
//		s = "scan " + s;
		s = (data_unit.m_vna_data.cmd_scan_bin ? "scan_bin " : "scan ") + s;

		addSerialTxCommand(s);
	}
}

void __fastcall CNanoVNA1Comms::sendTxCommand()
{
	if (m_tx_command.IsEmpty())
		return;

	if (!Form1)
	{
		m_tx_command = "";
		m_tx_commands.resize(0);
		return;
	}

	if (Form1->m_comms.serial.connected)
	{	// send the new command through the serial link

		Form1->pushCommMessage("tx: " + m_tx_command);

		if (Form1->m_comms.serial.TxBytes(AnsiString(m_tx_command).c_str()) <= 0)
		{	// failed

			Form1->pushCommMessage("tx: error .. " + m_tx_command);

			//Form1->m_comms.serial.Disconnect();
			//::PostMessage(Form1->Handle, WM_DISCONNECT, 0, 0);

			return;
		}

		Form1->m_comms.rx_timer.mark();
	}

	#ifdef TCPIPH
		if (Form1->m_comms.tcpip.connected)
		{	// send the new command through the tcpip link

			Form1->pushCommMessage("tx: " + m_tx_command);

			if (m_comms.tcpip.TxBytes(AnsiString(m_tx_command).c_str()) <= 0)
			{	// failed

				Form1->pushCommMessage("tx: " + String("error ") + m_comms.tcpip.lastErrorStr + " .. " + m_tx_command);

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

					Form1->pushCommMessage("tx: " + m_tx_command);

					Form1->m_comms.tcpip->IOHandler->Write(m_tx_command);
					Form1->m_comms.rx_timer.mark();
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

void __fastcall CNanoVNA1Comms::processTxCommands()
{
	if (!m_tx_command.IsEmpty())
		return;	// still waiting for a reply from the previous command

	while (!m_tx_commands.empty())
	{
		// fetch the next queued command
		m_tx_command = m_tx_commands.front();

		// remove it from the tx queue
		m_tx_commands.erase(m_tx_commands.begin() + 0);

		if (!m_tx_command.IsEmpty())
			break;
	}

	if (m_tx_command.IsEmpty())
		return;	// nothing to send

	sendTxCommand();
}

void __fastcall CNanoVNA1Comms::poll()
{
	m_poll_timer.mark();

	switch (m_mode)
	{
		case MODE_INIT1:	// we first just want to see the response to a CR/LF
			addSerialTxCommand("");
			break;

		case MODE_INIT2:
			addSerialTxCommand("help");
			addSerialTxCommand("version");
			addSerialTxCommand("info");
			break;

		case MODE_INIT3:
			if (data_unit.m_vna_data.cmd_deviceid)
				addSerialTxCommand("deviceid");

			if (data_unit.m_vna_data.cmd_edelay)
				addSerialTxCommand("edelay");

			if (data_unit.m_vna_data.cmd_vbat_offset)
				addSerialTxCommand("vbat_offset");

			if (data_unit.m_vna_data.cmd_vbat)
				addSerialTxCommand("vbat");

			if (data_unit.m_vna_data.cmd_usart_cfg)
				addSerialTxCommand("usart_cfg");

			if (data_unit.m_vna_data.cmd_threshold)
				addSerialTxCommand("threshold");

			if (data_unit.m_vna_data.cmd_marker)
				addSerialTxCommand("marker");

			if (data_unit.m_vna_data.cmd_bandwidth)
				addSerialTxCommand("bandwidth");

			if (data_unit.m_vna_data.cmd_power)
				addSerialTxCommand("power");

			if (data_unit.m_vna_data.cmd_sweep)
				addSerialTxCommand("sweep");

			if (data_unit.m_vna_data.cmd_mode)
				addSerialTxCommand("mode");

			m_mode = MODE_IDLE;

//			if (Form1)
//			{
//				Form1->ScanOnceSpeedButton->Enabled = true;
//				Form1->ScanSpeedButton->Enabled     = true;
//			}
			break;

		default:
			m_mode = MODE_IDLE;

		case MODE_IDLE:
			if (m_pause_comms)
				break;

			if (data_unit.m_vna_data.cmd_usart_cfg && data_unit.m_vna_data.usart_speed <= 0)
				addSerialTxCommand("usart_cfg");

			if (data_unit.m_vna_data.cmd_deviceid && data_unit.m_vna_data.deviceid < 0)
				addSerialTxCommand("deviceid");

			if (data_unit.m_vna_data.cmd_vbat_offset && data_unit.m_vna_data.vbat_offset_mv <= 0)
				addSerialTxCommand("vbat_offset");

			if (data_unit.m_vna_data.cmd_vbat)
				addSerialTxCommand("vbat");

			if (data_unit.m_vna_data.cmd_marker)
				addSerialTxCommand("marker");

			if (data_unit.m_vna_data.cmd_sweep)
				addSerialTxCommand("sweep");

			if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
			{
				if (data_unit.m_vna_data.cmd_threshold && data_unit.m_vna_data.freq_threshold_Hz <= 0)
					addSerialTxCommand("threshold");

				//if (data_unit.m_vna_data.cmd_cal)
				//	addSerialTxCommand("cal");

				if (data_unit.m_vna_data.cmd_bandwidth && data_unit.m_vna_data.bandwidth_Hz <= 0)
					addSerialTxCommand("bandwidth");
			}
			else
			{
				if (data_unit.m_vna_data.cmd_mode)
					addSerialTxCommand("mode");
			}

			break;

		case MODE_SINGLE_SCAN:
		case MODE_SCAN:
		case MODE_GENERATOR:
			if (m_pause_comms)
				break;

			if (data_unit.m_vna_data.cmd_vbat)
				addSerialTxCommand("vbat");

			break;
	}
}

void __fastcall CNanoVNA1Comms::extractVersionInfo(String s)
{
	int i;

	// Version: 1.0.25 [p:101, IF:12k, ADC:192k, Lcd:320x240]

	s = s.Trim().LowerCase();

	data_unit.m_vna_data.dislord     = (s.Pos("dislord")     > 0) ? true : false;	// https://github.com/DiSlord/NanoVNA
	data_unit.m_vna_data.oneofeleven = (s.Pos("oneofeleven") > 0) ? true : false;	// https://github.com/OneOfEleven/NanoVNA

	i = s.Pos('[');
	if (i <= 0)
		return;

	s = s.SubString(i + 1, s.Length()).Trim();
	i = s.Pos(']');
	if (i <= 0)
		return;

	s = s.SubString(1, i - 1).Trim();

	std::vector <String> params;
	common.parseString(s, ",", params);

	for (unsigned int k = 0; k < params.size(); k++)
	{
		String s1;
		String s2;

		s1 = params[k];

		if (s1.Pos("p:") == 1)
		{
			s1 = s1.SubString(3, s1.Length()).Trim();
			if (s1.IsEmpty())
				continue;

			if (!TryStrToInt(s1, i) || i < 51)
				continue;

			data_unit.m_vna_data.max_points = i;

			continue;
		}

		if (s1.Pos("if:") == 1)
		{
			s1 = s1.SubString(4, s1.Length()).Trim();
			if (s1.IsEmpty())
				continue;

			const char units = s1[s1.Length()];
			s1 = s1.SubString(1, s1.Length() - 1).Trim();

			if (!TryStrToInt(s1, i) || i <= 0)
				continue;

			if (units == 'k')
				i *= 1000;
			else
			if (units == 'm')
				i *= 1000000;

			data_unit.m_vna_data.if_Hz = i;

			continue;
		}

		if (s1.Pos("adc:") == 1)
		{
			s1 = s1.SubString(5, s1.Length()).Trim();
			if (s1.IsEmpty())
				continue;

			const char units = s1[s1.Length()];
			s1 = s1.SubString(1, s1.Length() - 1).Trim();

			if (!TryStrToInt(s1, i) || i <= 0)
				continue;

			if (units == 'k')
				i *= 1000;
			else
			if (units == 'm')
				i *= 1000000;

			data_unit.m_vna_data.adc_Hz = i;

			if (data_unit.m_vna_data.adc_Hz > 0 && data_unit.m_vna_data.audio_samples_count > 0)
				data_unit.m_vna_data.max_bandwidth_Hz = data_unit.m_vna_data.adc_Hz / data_unit.m_vna_data.audio_samples_count;

			continue;
		}

		if (s1.Pos("lcd:") == 1)
		{
			int j;

			s1 = s1.SubString(5, s1.Length()).Trim();
			if (s1.IsEmpty())
				continue;

			i = s1.Pos('x');
			if (i <= 0)
				continue;

			s2 = s1.SubString(1, i - 1).Trim();
			s1 = s1.SubString(i + 1, s1.Length()).Trim();

			if (!TryStrToInt(s2, i) || !TryStrToInt(s1, j) || i <= 0 || j <= 0)
				continue;

			data_unit.m_vna_data.lcd_width  = i;
			data_unit.m_vna_data.lcd_height = j;

			continue;
		}
	}
}

void __fastcall CNanoVNA1Comms::extractInfoCommand()
{
	data_unit.m_vna_data.type = UNIT_TYPE_NONE;

	data_unit.m_vna_data.dislord     = false;
	data_unit.m_vna_data.oneofeleven = false;

	data_unit.m_vna_data.info = m_rx_block.lines;

	data_unit.m_vna_data.lcd_width           = 0;
	data_unit.m_vna_data.lcd_height          = 0;
	data_unit.m_vna_data.max_points          = 0;
	data_unit.m_vna_data.max_bandwidth_Hz    = 0;
	data_unit.m_vna_data.if_Hz               = 0;
	data_unit.m_vna_data.adc_Hz              = 0;
	data_unit.m_vna_data.audio_samples_count = 0;

	// original version ..
	// Kernel:       4.0.0
	// Compiler:     GCC 8.3.1 20190703 (release) [gcc-8-branch revision 273027]
	// Architecture: ARMv6-M
	// Core Variant: Cortex-M0
	// Port Info:    Preemption through NMI
	// Platform:     STM32F072xB Entry Level Medium Density devices
	// Board:        NanoVNA-H
	// Build time:   Jan 18 2020 - 23:46:28

	// dislord version ..
	// Board: NanoVNA
	// 2016-2020 Copyright @edy555
	// Licensed under GPL. See: https://github.com/ttrftech/NanoVNA
	// Version: 0.9.3.4 beta FAT16/FAT32- SD Card, by DiSlord
	// Build Time: Jun 20 2020 - 17:08:40
	// Kernel: 4.0.0
	// Compiler: GCC 9.2.1 20191025 (release) [ARM/arm-9-branch revision 277599]
	// Architecture: ARMv6-M Core Variant: Cortex-M0
	// Port Info: Preemption through NMI
	// Platform: STM32F072xB Entry Level Medium Density devices

	// Board: NanoVNA
	// 2019-2020 Copyright @DiSlord (based on @edy555 source)
	// Licensed under GPL. See: https://github.com/DiSlord/NanoVNA-D
	// Version: 1.0.25 [p:101, IF:12k, ADC:192k, Lcd:320x240]
	// Build Time: Sep  2 2020 - 12:38:59
	// Kernel: 4.0.0
	// Compiler: GCC 9.2.1 20191025 (release) [ARM/arm-9-branch revision 277599]
	// Architecture: ARMv6-M Core Variant: Cortex-M0
	// Port Info: Preemption through NMI
	// Platform: STM32F072xB Entry Level Medium Density devices



	// tinySA info reply ..
	//
	// tinySA v0.3
	// 2019-2020 Copyright @Erik Kaashoek
	// 2016-2020 Copyright @edy555
	// SW licensed under GPL. See: https://github.com/erikkaashoek/tinySA
	// Version: tinySA_v1.0-80-gb798ea2
	// Build Time: Sep  9 2020 - 16:39:59
	// Kernel: 4.0.0
	// Compiler: GCC 7.2.1 20170904 (release) [ARM/embedded-7-branch revision 255204]
	// Architecture: ARMv6-M Core Variant: Cortex-M0
	// Port Info: Preemption through NMI
	// Platform: STM32F072xB Entry Level Medium Density devices


	// remember the current unit type
	const int prev_unit_type = data_unit.m_vna_data.type;

	for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
	{
		String s = m_rx_block.lines[i].Trim().LowerCase();

//		if (s.Pos("board:") <= 0)
//			continue;

		if (s.Pos("nanovna-h7") > 0 || s.Pos("nanovna-h 7") > 0)
		{
			data_unit.m_vna_data.type = UNIT_TYPE_NANOVNA_H7;
			break;
		}
		if (s.Pos("nanovna-h4") > 0 || s.Pos("nanovna-h 4") > 0)
		{
			data_unit.m_vna_data.type = UNIT_TYPE_NANOVNA_H4;
			break;
		}
		if (s.Pos("nanovna") > 0 || s.Pos("nanovna-h") > 0)
		{
			data_unit.m_vna_data.type = UNIT_TYPE_NANOVNA_H;
			break;
		}
		if (s.Pos("tinysa") > 0)
		{
			data_unit.m_vna_data.type = UNIT_TYPE_TINYSA;
			break;
		}
	}

	// set new unit type
	switch (data_unit.m_vna_data.type)
	{
		case UNIT_TYPE_NONE:       data_unit.m_vna_data.name = "NONE-unit-type";    break;
		case UNIT_TYPE_UNKNOWN:    data_unit.m_vna_data.name = "UNKNOWN-unit-type"; break;
		case UNIT_TYPE_NANOVNA_H:  data_unit.m_vna_data.name = "NanoVNA-H";         break;
		case UNIT_TYPE_NANOVNA_H4: data_unit.m_vna_data.name = "NanoVNA-H4";        break;
		case UNIT_TYPE_NANOVNA_H7: data_unit.m_vna_data.name = "NanoVNA-H7";        break;
		case UNIT_TYPE_NANOVNA_V2: data_unit.m_vna_data.name = "NanoVNA-V2";        break;
		case UNIT_TYPE_JANVNA_V2:  data_unit.m_vna_data.name = "JanVNA-V2";         break;
		case UNIT_TYPE_TINYSA:     data_unit.m_vna_data.name = "tinySA";            break;
		default:                	data_unit.m_vna_data.name = "ERROR-unit-type";   break;
	}

	for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
	{
		const String s = m_rx_block.lines[i].Trim().LowerCase();
		if (s.Pos("version:") > 0)
			extractVersionInfo(s);
	}

	switch (data_unit.m_vna_data.type)
	{
		default:
		case UNIT_TYPE_NONE:
		case UNIT_TYPE_NANOVNA_H:
			if (data_unit.m_vna_data.audio_samples_count <= 0)
			{
				data_unit.m_vna_data.audio_samples_count = 48;
				if (data_unit.m_vna_data.adc_Hz > 0 && data_unit.m_vna_data.audio_samples_count > 0)
					data_unit.m_vna_data.max_bandwidth_Hz = data_unit.m_vna_data.adc_Hz / data_unit.m_vna_data.audio_samples_count;
			}

			if (data_unit.m_vna_data.lcd_width <= 0 && data_unit.m_vna_data.lcd_height <= 0)
			{
				data_unit.m_vna_data.lcd_width  = 320;
				data_unit.m_vna_data.lcd_height = 240;
			}

			if (data_unit.m_vna_data.max_points <= 0)
				data_unit.m_vna_data.max_points = 101;

			if (data_unit.m_vna_data.max_bandwidth_Hz <= 0)
				data_unit.m_vna_data.max_bandwidth_Hz = data_unit.m_vna_data.dislord ? 4000 : 2000;

			break;

		case UNIT_TYPE_NANOVNA_H4:
			if (data_unit.m_vna_data.audio_samples_count <= 0)
			{
				data_unit.m_vna_data.audio_samples_count = 96;
				if (data_unit.m_vna_data.adc_Hz > 0 && data_unit.m_vna_data.audio_samples_count > 0)
					data_unit.m_vna_data.max_bandwidth_Hz = data_unit.m_vna_data.adc_Hz / data_unit.m_vna_data.audio_samples_count;
			}

			if (data_unit.m_vna_data.lcd_width <= 0 && data_unit.m_vna_data.lcd_height <= 0)
			{
				data_unit.m_vna_data.lcd_width  = 480;
				data_unit.m_vna_data.lcd_height = 320;
			}

			if (data_unit.m_vna_data.max_points <= 0)
				data_unit.m_vna_data.max_points = 401;

			if (data_unit.m_vna_data.max_bandwidth_Hz <= 0)
				data_unit.m_vna_data.max_bandwidth_Hz = 4000;

			break;

		case UNIT_TYPE_NANOVNA_H7:
			if (data_unit.m_vna_data.audio_samples_count <= 0)
			{
				data_unit.m_vna_data.audio_samples_count = 96;
				if (data_unit.m_vna_data.adc_Hz > 0 && data_unit.m_vna_data.audio_samples_count > 0)
					data_unit.m_vna_data.max_bandwidth_Hz = data_unit.m_vna_data.adc_Hz / data_unit.m_vna_data.audio_samples_count;
			}

			if (data_unit.m_vna_data.lcd_width <= 0 && data_unit.m_vna_data.lcd_height <= 0)
			{
				data_unit.m_vna_data.lcd_width  = 480;
				data_unit.m_vna_data.lcd_height = 320;
			}

			if (data_unit.m_vna_data.max_points <= 0)
				data_unit.m_vna_data.max_points = 401;

			if (data_unit.m_vna_data.max_bandwidth_Hz <= 0)
				data_unit.m_vna_data.max_bandwidth_Hz = 4000;

			break;

//		case UNIT_TYPE_NANOVNA_V2:
//			break;

//		case UNIT_TYPE_JANVNA_V2:
//			break;

		case UNIT_TYPE_TINYSA:
				data_unit.m_vna_data.lcd_width  = 320;
				data_unit.m_vna_data.lcd_height = 240;
				break;
	}

	::PostMessage(Form1->Handle, WM_NEW_UNIT_TYPE, 0, 0);
}

void __fastcall CNanoVNA1Comms::extractPower(String s)
{
	const int pos = s.Pos("power:");
	if (pos <= 0)
		return;

	s = s.SubString(pos + 6, s.Length()).Trim();

	int value = 255;
	if (!TryStrToInt(common.localiseDecimalPoint(s), value))
		return;

//	const int prev_power = data_unit.m_vna_data.power;
	data_unit.m_vna_data.power = value;

	if (value >= 255)
		value = -1;

	settings.outputPower = value;

	::PostMessage(Form1->Handle, WM_UPDATE_OUTPUT_POWER, value, 0);
}

void __fastcall CNanoVNA1Comms::processRxBlock()
{
	const int segments = (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA) ? data_unit.m_segments : 1;

	if (m_rx_block.type == SERIAL_STATE_IDLE)
	{
		if (m_mode == MODE_INIT1)
		{
			m_mode = MODE_INIT2;
			poll();
		}
	}
	else
	{
		String s = getSerialStateString(m_rx_block.type);

//		if (m_rx_block.timer.millisecs(false) < 5000)
		{
			if (m_rx_block.lines.size() > 0 || m_rx_block.bin_data.size() > 0)
			{
//				m_poll_timer.mark();

				switch (m_rx_block.type)
				{
					default:
					case SERIAL_STATE_IDLE:
						if (m_mode == MODE_INIT1)
						{
							m_mode = MODE_INIT2;
							poll();
						}
						break;

					case SERIAL_STATE_HELP:
							data_unit.m_vna_data.help = "";
							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								// original version
								// Commands: help exit info threads version reset freq offset saveconfig clearconfig data frequencies port stat gain power sample scan sweep test touchcal touchtest pause resume cal save recall trace marker edelay capture vbat transform threshold vbat_offset

								// DiSlords new version
								// Commands: scan scan_bin data frequencies freq sweep power bandwidth time saveconfig clearconfig touchcal touchtest pause resume cal save recall trace marker edelay capture vbat reset usart_cfg usart vbat_offset transform threshold help info version color

								// TinySA
								// Commands: version reset freq dac saveconfig clearconfig data frequencies scan scanraw sweep test touchcal touchtest pause resume caloutput save recall trace trigger marker capture vbat vbat_offset help info color if attenuate level sweeptime leveloffset levelchange modulation rbw mode spur load output deviceid selftest correction threads

								//std::vector <String> params;
								//common.parseString(m_rx_block.lines[i].Trim().LowerCase(), " ", params);

								String s2 = m_rx_block.lines[i].Trim().LowerCase();
								if (!s2.IsEmpty())
								{
									if (s2[s2.Length()] == '?')
									{	// the unit replied with an error .. try again
										insertSerialTxCommand("help");
									}
									else
									{
										const int pos = s2.Pos("commands:");
										if (pos > 0)
										{
											data_unit.m_vna_data.help              = s2;
											s2                                     = " " + s2.SubString(pos + 10, s2.Length()).Trim() + " ";
											data_unit.m_vna_data.cmd_capture       = (s2.Pos(" capture ")     > 1) ? true : false;
											data_unit.m_vna_data.cmd_bandwidth     = (s2.Pos(" bandwidth ")   > 1) ? true : false;
											data_unit.m_vna_data.cmd_vbat          = (s2.Pos(" vbat ")        > 1) ? true : false;
											data_unit.m_vna_data.cmd_vbat_offset   = (s2.Pos(" vbat_offset ") > 1) ? true : false;
											data_unit.m_vna_data.cmd_marker        = (s2.Pos(" marker ")      > 1) ? true : false;
											data_unit.m_vna_data.cmd_integrator    = (s2.Pos(" integrator ")  > 1) ? true : false;
											data_unit.m_vna_data.cmd_scan_bin      = (s2.Pos(" scan_bin ")    > 1) ? true : false;
											data_unit.m_vna_data.cmd_scanraw       = (s2.Pos(" scanraw ")     > 1) ? true : false;
											data_unit.m_vna_data.cmd_sd_list       = (s2.Pos(" sd_list ")     > 1) ? true : false;
											data_unit.m_vna_data.cmd_sd_readfile   = (s2.Pos(" sd_readfile ") > 1) ? true : false;
											data_unit.m_vna_data.cmd_time          = (s2.Pos(" time ")        > 1) ? true : false;
											data_unit.m_vna_data.cmd_threshold     = (s2.Pos(" threshold ")   > 1) ? true : false;
											data_unit.m_vna_data.cmd_pause         = (s2.Pos(" pause ")       > 1) ? true : false;
											data_unit.m_vna_data.cmd_resume        = (s2.Pos(" resume ")      > 1) ? true : false;
											data_unit.m_vna_data.cmd_reset         = (s2.Pos(" reset ")       > 1) ? true : false;
											data_unit.m_vna_data.cmd_cal           = (s2.Pos(" cal ")         > 1) ? true : false;
											data_unit.m_vna_data.cmd_power         = (s2.Pos(" power ")       > 1) ? true : false;
											data_unit.m_vna_data.cmd_usart         = (s2.Pos(" usart ")       > 1) ? true : false;
											data_unit.m_vna_data.cmd_usart_cfg     = (s2.Pos(" usart_cfg ")   > 1) ? true : false;
											data_unit.m_vna_data.cmd_deviceid      = (s2.Pos(" deviceid ")    > 1) ? true : false;
											data_unit.m_vna_data.cmd_sweep         = (s2.Pos(" sweep ")       > 1) ? true : false;
											data_unit.m_vna_data.cmd_mode          = (s2.Pos(" mode ")        > 1) ? true : false;
										}
									}
								}
							}

						if (m_mode == MODE_INIT2 &&
							 !data_unit.m_vna_data.help.IsEmpty() &&
							 !data_unit.m_vna_data.version.IsEmpty() &&
							 !data_unit.m_vna_data.info.empty())
						{
							m_mode = MODE_INIT3;
							poll();
						}
						break;

					case SERIAL_STATE_VERSION:
							data_unit.m_vna_data.version = "";

							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								// original version ..
								// 0.4.5-4-g96e7efe

								// disload version ..
								// 0.7.1-56-g76223f5

								String s2 = m_rx_block.lines[i].Trim();
								if (!s2.IsEmpty())
								{
									if (s2[s2.Length()] == '?')
									{	// the unit replied with an error .. try again
										insertSerialTxCommand("version");
									}
									else
									{
										data_unit.m_vna_data.version = s2;
									}
								}
							}

						if (m_mode == MODE_INIT2 &&
							 !data_unit.m_vna_data.help.IsEmpty() &&
							 !data_unit.m_vna_data.version.IsEmpty() &&
							 !data_unit.m_vna_data.info.empty())
						{
							m_mode = MODE_INIT3;
							poll();
						}
						break;

					case SERIAL_STATE_INFO:
						extractInfoCommand();

						if (m_mode == MODE_INIT2 &&
							 !data_unit.m_vna_data.help.IsEmpty() &&
							 !data_unit.m_vna_data.version.IsEmpty() &&
							 !data_unit.m_vna_data.info.empty())
						{
							m_mode = MODE_INIT3;
							poll();
						}
						break;

					case SERIAL_STATE_MARKER:
						{
							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								// "1 50 145000000"
								// "2 43 144300000"
								String s2 = m_rx_block.lines[i];

								//std::vector <String> params;
								//s2 = common.localiseDecimalPoint(s2);
								//common.parseString(s2, " ", params);


							}
						}
						break;

					case SERIAL_STATE_SWEEP:
							data_unit.m_vna_data.freq_start_Hz  = 0;
							data_unit.m_vna_data.freq_stop_Hz   = 0;
							data_unit.m_vna_data.freq_center_Hz = 0;
							data_unit.m_vna_data.freq_span_Hz   = 0;
							data_unit.m_vna_data.num_points     = 0;

							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								// "140000000 150000000 101"
								// "50400000 -3000000 101"
								String s2 = m_rx_block.lines[i];

								std::vector <String> params;
								s2 = common.localiseDecimalPoint(s2);
								common.parseString(s2.LowerCase(), " ", params);

								if (params.size() >= 3)
								{
									int64_t Hz;
									int num_points;

									if (TryStrToInt64(params[0], Hz))
										if (Hz >= 0)
											data_unit.m_vna_data.freq_start_Hz = Hz;

									if (TryStrToInt64(params[1], Hz))
										if (Hz >= 0 && Hz >= data_unit.m_vna_data.freq_start_Hz)
											data_unit.m_vna_data.freq_stop_Hz = Hz;

									if (data_unit.m_vna_data.freq_start_Hz < data_unit.m_vna_data.freq_stop_Hz)
									{
										data_unit.m_vna_data.freq_center_Hz = (data_unit.m_vna_data.freq_start_Hz + data_unit.m_vna_data.freq_stop_Hz) / 2;
										data_unit.m_vna_data.freq_span_Hz   = data_unit.m_vna_data.freq_stop_Hz - data_unit.m_vna_data.freq_start_Hz;
									}

									if (TryStrToInt(params[2], num_points))
										if (num_points > 0)
											data_unit.m_vna_data.num_points = num_points;
								}
							}
						break;

					case SERIAL_STATE_STAT:
							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								// "average: -130 -110"
								// "rms: 4428 5"
								// "callback count: 3572321"
								// "awd: 9"

								String s2 = m_rx_block.lines[i];

								//std::vector <String> params;
								//s2 = common.localiseDecimalPoint(s2);
								//common.parseString(s2, " ", params);



							}
						break;

					case SERIAL_STATE_TIME:
							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								// ch> time
								// 2020/04/25 14:07:48
								// usage: time [y|m|d|h|min|sec] 0-99

								String s2 = m_rx_block.lines[i];

								//std::vector <String> params;
								//s2 = common.localiseDecimalPoint(s2);
								//common.parseString(s2, ' ', params);


							}
						break;

					case SERIAL_STATE_VBAT:
							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								// "4151 mV"

								String s2 = m_rx_block.lines[i].Trim().LowerCase();

								std::vector <String> params;
								s2 = common.localiseDecimalPoint(s2);
								common.parseString(s2.LowerCase(), " ", params);

								for (unsigned int k = 0; k < params.size(); k++)
								{
									String s = params[k];
									const int pos = s.Pos("mv");
									if (pos > 0)
										s = s.SubString(1, pos - 1).Trim();

									int value;
									if (TryStrToInt(common.localiseDecimalPoint(s), value) && value >= 0)
									{
										data_unit.m_vna_data.vbat_mv = value;
										m_poll_timer.mark();
										::PostMessage(Form1->Handle, WM_UPDATE_BATTERY_VOLTAGE, data_unit.m_vna_data.vbat_mv, 0);
										break;
									}
								}
							}
						break;

					case SERIAL_STATE_VBAT_OFFSET:
						{
							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								// "200"
								// "-200"

								String s2 = m_rx_block.lines[i];

								std::vector <String> params;
								s2 = common.localiseDecimalPoint(s2);
								common.parseString(s2.LowerCase(), " ", params);

								for (unsigned int k = 0; k < params.size(); k++)
								{
									String s = params[k];

									int value;
									if (TryStrToInt(common.localiseDecimalPoint(s), value))
									{
										const int prev_vbat_offset_mv = data_unit.m_vna_data.vbat_offset_mv;
										data_unit.m_vna_data.vbat_offset_mv = value;
										::PostMessage(Form1->Handle, WM_UPDATE_BATTERY_VOLTAGE, 0, data_unit.m_vna_data.vbat_offset_mv);
										break;
									}
								}
							}
						}
						break;

					case SERIAL_STATE_INTEGRATOR:
						{
							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								// "integrator off"
								// "integrator on"

								String s2 = m_rx_block.lines[i];

								if (SettingsForm)
									SettingsForm->update();

								break;
							}
						}
						break;

					case SERIAL_STATE_BANDWIDTH:
						{
							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								// "bandwidth 0"
								// "bandwidth 1"
								// "bandwidth 2"
								// "bandwidth 4 (400Hz)"

								String s2 = m_rx_block.lines[i];

								std::vector <String> params;
								s2 = common.localiseDecimalPoint(s2);
								common.parseString(s2.LowerCase(), " ", params);

								if (params.size() >= 2)
								{
									if (params[0].LowerCase() == "bandwidth")
									{
										int value;
										if (TryStrToInt(common.localiseDecimalPoint(params[1]), value))
										{
											String s;

											if (value <= 0)
												value = 0;

											data_unit.m_vna_data.bandwidth = value;

											if (params.size() >= 3)
											{	// extract the bandwidth frequency
												s = params[2].Trim();
												int i = 1;
												while (i <= s.Length())
												{
													if (s[i] == '(' || s[i] == ')')
														s[i] = ' ';
													i++;
												}
												s = s.Trim();
												i = 1;
												while (i <= s.Length() && s[i] >= '0' && s[i] <= '9')
													i++;
												String units = s.SubString(i, s.Length()).Trim();
												s = s.SubString(1, i - 1).Trim();

												if (TryStrToInt(s, value) && value > 0)
												{
													if (units ==  "hz") value *= 1;
													else
													if (units == "khz") value *= 1000;
													else
													if (units == "mhz") value *= 1000000;

													data_unit.m_vna_data.bandwidth_Hz = value;

													if (data_unit.m_vna_data.adc_Hz > 0 && data_unit.m_vna_data.audio_samples_count > 0)
													{
														const int clk = data_unit.m_vna_data.adc_Hz / data_unit.m_vna_data.audio_samples_count;
														const int bw = (data_unit.m_vna_data.bandwidth_Hz <= 2000) ? data_unit.m_vna_data.bandwidth_Hz : 2000;
														m_state_timeout_ms = (DEFAULT_STATE_TIME_OUT_V1_MS * clk) / bw;
														m_link_timeout_ms  = (DEFAULT_LINK_TIME_OUT_V1_MS  * clk) / bw;
													}

													::PostMessage(Form1->Handle, WM_UPDATE_POINT_BANDWIDTH, value, 0);
												}
											}

											//data_unit.m_vna_data.bandwidth    = value;
											//data_unit.m_vna_data.bandwidth_Hz = BW_HZ / (value + 1);

											//m_state_timeout_ms = (DEFAULT_STATE_TIME_OUT_MS * BW_HZ) / data_unit.m_vna_data.bandwidth_Hz;
											//m_link_timeout_ms  = (DEFAULT_LINK_TIME_OUT_MS  * BW_HZ) / data_unit.m_vna_data.bandwidth_Hz;

											::PostMessage(Form1->Handle, WM_UPDATE_POINT_BANDWIDTH, 0, 0);

											break;
										}
									}
								}
							}
						}
						break;

					case SERIAL_STATE_THRESHOLD:
//						data_unit.m_vna_data.freq_threshold_Hz = 0;

						for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
						{
							// threshold
							// usage: threshold {frequency in harmonic mode}
							// current: 290000000
							// ch>

							String s2 = m_rx_block.lines[i].Trim().LowerCase();

							//std::vector <String> params;
							//s2 = common.localiseDecimalPoint(s2);
							//common.parseString(s2, " ", params);

							const int pos = s2.Pos("current:");
							if (pos > 0)
							{
								String s3 = s2.SubString(pos + 8, s2.Length()).Trim();
								//s2 = s2.SubString(1, pos - 1).Trim();

								int64_t value = 0;
								if (TryStrToInt64(common.localiseDecimalPoint(s3), value) && value > 0)
								{
									data_unit.m_vna_data.freq_threshold_Hz = value;
									break;
								}
							}
						}

						::PostMessage(Form1->Handle, WM_UPDATE_THRESHOLD_HZ, 0, 0);

						break;

					case SERIAL_STATE_EDELAY:
						for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
						{
							// 0.000000
							// ch>

							String s2 = m_rx_block.lines[i].Trim().LowerCase();

							if (!s2.IsEmpty())
							{
								float value;
								if (TryStrToFloat(s2, value))
								{
									data_unit.m_vna_data.edelay = value;
									settings.eDelaySecs = value;
									::PostMessage(Form1->Handle, WM_UPDATE_EDELAY, 0, 0);
								}
							}

						}
						break;

					case SERIAL_STATE_DEVICE_ID:
						for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
						{
							// deviceid 0
							// ch>

							String s2 = m_rx_block.lines[i].Trim().LowerCase();

							std::vector <String> params;
							common.parseString(s2, " ", params);

							if (params.size() >= 2)
							{
								int64_t value;
								if (TryStrToInt64(params[1], value))
								{
									data_unit.m_vna_data.deviceid = value;
								}
							}
						}
						break;

					case SERIAL_STATE_CAL:
						for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
						{
							// cal on
							// cal off
							// ch>

							//String s2 = m_rx_block.lines[i].Trim().LowerCase();


						}
						break;

					case SERIAL_STATE_POWER:
						for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
						{
							// power: 255 .. this auto mode
							// power: 0 .... 2mA Si5351 output current
							// power: 1 .... 4mA Si5351 output current
							// power: 2 .... 6mA Si5351 output current
							// power: 3 .... 8mA Si5351 output current
							// ch>

							extractPower(m_rx_block.lines[i].Trim());
						}
						break;

					case SERIAL_STATE_USART:
						for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
						{
							// tx: usart "AT+VERSION?"
							// rx: usart "AT+VERSION?"
							// rx: +VERSION:2.0-20100601
							// rx: OK
							// rx: ch>

							String s2 = m_rx_block.lines[i].Trim();

							if (VNAUsartCommsForm)
								VNAUsartCommsForm->addText(FormatDateTime("ss.zzz ", Now()) + "rx: ", s2);
						}
						break;

					case SERIAL_STATE_USART_CFG:
						for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
						{
							// Serial: 38400 baud
							// ch>

							String s2 = m_rx_block.lines[i].Trim();

							std::vector <String> params;
							s2 = common.localiseDecimalPoint(s2);
							common.parseString(s2, " ", params);

							if (params.size() >= 3)
							{
								if (params[0].LowerCase() == "serial:" && params[2].LowerCase() == "baud")
								{
									int baudrate;
									if (TryStrToInt(params[1], baudrate))
									{
										data_unit.m_vna_data.usart_speed = baudrate;

										if (VNAUsartCommsForm)
											VNAUsartCommsForm->setBaudrate(baudrate);
                           }
								}
							}
						}
						break;

					case SERIAL_STATE_CLEARCONFIG:
						for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
						{
							String s2 = m_rx_block.lines[i].Trim().LowerCase();

							//std::vector <String> params;
							//s2 = common.localiseDecimalPoint(s2);
							//common.parseString(s2, " ", params);


						}
						break;

					case SERIAL_STATE_SAVECONFIG:
						for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
						{
							String s2 = m_rx_block.lines[i].Trim().LowerCase();

							//std::vector <String> params;
							//s2 = common.localiseDecimalPoint(s2);
							//common.parseString(s2, ' ', params);


						}
						break;

					case SERIAL_STATE_SCAN:
							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								String s2 = m_rx_block.lines[i];

							}
						break;

					case SERIAL_STATE_SCAN_BIN:
							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								String s2 = m_rx_block.lines[i];

							}
						break;

					case SERIAL_STATE_SCANRAW:
							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								String s2 = m_rx_block.lines[i];

							}
						break;

					case SERIAL_STATE_PAUSE:
						break;

					case SERIAL_STATE_RESUME:
						break;

					case SERIAL_STATE_RESET:
						break;

					case SERIAL_STATE_DATA0:
					case SERIAL_STATE_DATA1:
					case SERIAL_STATE_DATA2:
					case SERIAL_STATE_DATA3:
					case SERIAL_STATE_DATA4:
					case SERIAL_STATE_DATA5:
					case SERIAL_STATE_DATA6:
					case SERIAL_STATE_DATA7:

						if (!Form1)
							break;

						if ((int)Form1->m_freq_data_list.size() != data_unit.m_points_per_segment || m_rx_block.lines.size() != Form1->m_freq_data_list.size())
						{
							Form1->resetFreqArray();
							::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
							break;
						}

						{
							const unsigned int bins = m_rx_block.lines.size();

							int channel = -1;
							switch (m_rx_block.type)
							{
								case SERIAL_STATE_DATA0: channel =  0; break;	// channel 0
								case SERIAL_STATE_DATA1: channel =  1; break;	// channel 1
								//case SERIAL_STATE_DATA2: channel =  2; break;	// calibration channel 0
								//case SERIAL_STATE_DATA3: channel =  3; break;	// calibration channel 1
								//case SERIAL_STATE_DATA4: channel =  4; break;	// calibration channel 2
								//case SERIAL_STATE_DATA5: channel =  5; break;	// calibration channel 3
								//case SERIAL_STATE_DATA6: channel =  6; break;	// calibration channel 4
								//case SERIAL_STATE_DATA7: channel =  7; break;	// calibration channel 5
								default :                channel = -1; break;
							}
							if (channel < 0)
								break;

							for (unsigned int i = 0; i < bins; i++)
							{
								// "0.981099784 -0.221877455"
								// "0.981573939 -0.222379073"
								// "0.981298089 -0.221447259"
								//     ...
								// "0.977009832 -0.238004878"
								// "0.976749658 -0.237871572"
								// "0.976810872 -0.237944498"

								String s2 = m_rx_block.lines[i].Trim();

								std::vector <String> params;
								s2 = common.localiseDecimalPoint(s2);
								common.parseString(s2.LowerCase(), " ", params);

								if (params.size() < 2)
								{
									Form1->resetFreqArray();
//									::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
									break;
								}

								float real;
								float imag;
								if (!TryStrToFloat(params[0], real) || !TryStrToFloat(params[1], imag))
								{
									Form1->resetFreqArray();
//									::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
									break;
								}

								const complexf c(real, imag);

								if (channel < MAX_CHANNELS)
								{	// S11 or S21
									Form1->m_freq_data_list[i].sParam[channel] = c;
								}
								//else
								//if (channel < (MAX_CHANNELS + 5))
								//{	// calibration data
								//	//Form1->m_calibration_data_list[i].sParam[channel - MAX_CHANNELS] = c;
								//}
							}

							if (Form1->m_freq_data_list.size() > 0 && channel >= (MAX_CHANNELS - 1))
							{
								Form1->addNewRxData(data_unit.m_point_incoming);
								data_unit.m_point_incoming.resize(0);
							}
							//else
							//if (Form1->m_calibration_data_list.size() > 0 && channel >= MAX_CHANNELS && channel <= (MAX_CHANNELS + 5))
							//{
								// TODO
							//}
						}

						break;

					case SERIAL_STATE_FREQDATA:
						{
							if ((int)m_rx_block.lines.size() != data_unit.m_points_per_segment)
							{
								Form1->pushCommMessage("rx: " + String("freq_data incorrect number of points error ") + IntToStr((int)m_rx_block.lines.size()));

								Form1->m_freq_data_list.resize(0);
								//Form1->resetFreqArray();
								//::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
								break;
							}

							//const int segments           = data_unit.m_segments;
							//const int segment            = data_unit.m_segment;
							//const int points_per_segment = data_unit.m_points_per_segment;
							//const int num_points         = data_unit.m_points;

							Form1->m_freq_data_list.resize(0);

							for (unsigned int i = 0; i < m_rx_block.lines.size(); i++)
							{
								int pos;

								// NanoVNA
								//
								// 110000 -0.021064780 0.000593003 -0.001578232 0.000524658
								// 114009900 -0.024328330 -0.003553952 -0.000195407 -0.000264995
								// 128009800 -0.024820130 -0.003727459 -0.000324236 0.000186244
								//     ...
								// 1372000200 -0.140656896 0.306687040 -0.132128984 -0.068297504
								// 1386000100 -0.201044400 0.193266832 -0.078333568 -0.032276042
								// 1400000000 -0.289619072 0.232937696 0.046997888 -0.199658688

								// TinySA
								//
								// 50000 -6.656249984 0.000000000 0.000000000 0.000000000
								// 3164014 -76.781249984 0.000000000 0.000000000 0.000000000
								// 6278028 -88.250000000 0.000000000 0.000000000 0.000000000
								// 9392042 -86.718750016 0.000000000 0.000000000 0.000000000

								String s2 = m_rx_block.lines[i].Trim();

								std::vector <String> params;
								s2 = common.localiseDecimalPoint(s2);
								common.parseString(s2.LowerCase(), " ", params);

								// convert the text params into floating point values
								std::vector <double> values;
								for (unsigned int k = 0; k < params.size(); k++)
								{
									double value = 0;
									if (!TryStrToFloat(params[k], value))
									{
										Form1->pushCommMessage("rx: " + String("freq_data value error line ") + IntToStr((int)i) + " [" + params[0] + "]");
										Form1->m_freq_data_list.resize(0);
										//::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
										break;
									}
									values.push_back(value);
								}

								if (values.size() < 3)
								{
									Form1->pushCommMessage("rx: " + String("freq_data missing data line ") + IntToStr((int)i));

									Form1->m_freq_data_list.resize(0);
									//::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
									break;
								}

								// validate the frequency
								if (values[0] <= 0)
								{
									Form1->pushCommMessage("rx: " + String("freq_data freq value error line ") + IntToStr((int)i) + " [" + params[0] + "]");

									Form1->m_freq_data_list.resize(0);
									//::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
									break;
								}

								t_data_point fp;
								if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
								{	// NanoVNA
									fp.Hz = I64ROUND(values[0]);
									if (params.size() == 3)
									{
										fp.s11 = complexf(values[1], values[2]);
										fp.s21 = complexf(0);
									}
									else
									if (params.size() >= 5)
									{
										fp.s11 = complexf(values[1], values[2]);
										fp.s21 = complexf(values[3], values[4]);
									}
								}
								else
								{	// TinySA
									fp.Hz = I64ROUND(values[0]);
									const float v1 = powf(10.0f, values[1] / 20.0f);	// dB to absolute
									const float v2 = values[2];
									fp.s11 = complexf(v1, v2);
									fp.s21 = complexf(v1, v2);
								}

								Form1->m_freq_data_list.push_back(fp);
/*
								if ((int)data_unit.m_point_incoming.size() != num_points)
								{
									data_unit.m_point_incoming.resize(num_points);
									memset(&data_unit.m_point_incoming[0], 0, sizeof(data_unit.m_point_incoming[0]) * num_points);
								}

								data_unit.m_point_incoming[pos] = fp;

								// give the user some feedback that we are actually receiving scanned points
								if ((i & 31) == 0)
									::PostMessage(Form1->Handle, WM_INCOMING_POINTS, (WPARAM)i, 0);
*/
							}
/*
							if (segment >= (segments - 1))
							{
								#if 0
									s.printf(L"%d points received", num_points);
									Form1->pushCommMessage("rx: " + s);
								#endif
								Form1->addNewRxData();
								data_unit.m_point_incoming.resize(0);
							}
*/
							if (Form1->m_freq_data_list.size() > 0)
							{
								Form1->addNewRxData(data_unit.m_point_incoming);
								data_unit.m_point_incoming.resize(0);
							}
						}
						break;

					case SERIAL_STATE_FREQDATA_BIN:
						{
							int k = 0;
							int i = 0;

							Form1->m_freq_data_list.resize(0);

							const uint16_t mask = ((uint16_t)m_rx_block.bin_data[k + 1] << 8) | (m_rx_block.bin_data[k + 0] << 0);
							k += 2;

							if (mask & SCAN_MASK_OUT_FREQ)  i += 4;	// frequency
							if (mask & SCAN_MASK_OUT_DATA0) i += 8;	// s11
							if (mask & SCAN_MASK_OUT_DATA1) i += 8;	// s21
							// SCAN_MASK_BINARY

							//const int segments           = data_unit.m_segments;
							//const int segment            = data_unit.m_segment;
							//const int points_per_segment = data_unit.m_points_per_segment;
							//const int num_points         = data_unit.m_points;

							// number of points to follow
							const uint16_t points = ((uint16_t)m_rx_block.bin_data[k + 1] << 8) | (m_rx_block.bin_data[k + 0] << 0);
							k += 2;
							i *= points;

							if (points != data_unit.m_points_per_segment)
							{
//								Form1->resetFreqArray();
//								::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
								break;
							}

							if ((int)m_rx_block.bin_data.size() != (4 + i))
							{
//								Form1->resetFreqArray();
//								::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
								break;
							}

							// compute where in our list we are going to save this new block of data
							unsigned int bin_index = (data_unit.m_point.size() * data_unit.m_segment) / segments;
							if ((mask & SCAN_MASK_OUT_FREQ) == 0 && data_unit.m_segment > 0)
							{	// we need to compute where where to save this data too

								if (data_unit.m_total_frames <= 0)
								{	// we have no frequency table to refer too .. this should have been supplied on the first scan
									Form1->m_freq_data_list.resize(0);
									break;
								}

								const int64_t start_Hz = data_unit.m_point[0].Hz + ((data_unit.m_freq_span_Hz * data_unit.m_segment) / segments);
								while (bin_index < data_unit.m_point.size())
								{
									if (data_unit.m_point[bin_index].Hz >= start_Hz)
										break;
									bin_index++;
								}

								#if 0
									{
										String s;
										s.printf(L"test %d/%d %u/%u %lld", data_unit.m_segment, segments, bin_index, data_unit.m_point.size(), start_Hz);
										Form1->pushCommMessage(s);
									}
								#endif
							}

							for (uint16_t i = 0; i < points; i++)
							{
								int64_t freq    = 0;
								double s11_real = 0;
								double s11_imag = 0;
								double s21_real = 0;
								double s21_imag = 0;

								union
								{
									uint8_t  u8[4];
									uint16_t u16[2];
									uint32_t u32;
									float    f32;
								} value;

								if (mask & SCAN_MASK_OUT_FREQ)
								{	// frequency
									memcpy(&value, &m_rx_block.bin_data[k], 4);
									k += 4;
									freq = (int64_t)value.u32;
								}
								else
								{	// compute the frequency as it hasn't been supplied to us
									freq = data_unit.m_point[bin_index + i].Hz;
								}

								if (mask & SCAN_MASK_OUT_DATA0)
								{	// s11
									memcpy(&value, &m_rx_block.bin_data[k], 4);
									k += 4;
									s11_real = value.f32;

									memcpy(&value, &m_rx_block.bin_data[k], 4);
									k += 4;
									s11_imag = value.f32;
								}

								if (mask & SCAN_MASK_OUT_DATA1)
								{	// s21
									memcpy(&value, &m_rx_block.bin_data[k], 4);
									k += 4;
									s21_real = value.f32;

									memcpy(&value, &m_rx_block.bin_data[k], 4);
									k += 4;
									s21_imag = value.f32;
								}

								if (freq <= 0)
								{
									Form1->m_freq_data_list.resize(0);
									break;
								}

								if (s11_real < -MAX_S_PARAM_VALUE) s11_real = -MAX_S_PARAM_VALUE;
								else
								if (s11_real >  MAX_S_PARAM_VALUE) s11_real =  MAX_S_PARAM_VALUE;
								if (s11_imag < -MAX_S_PARAM_VALUE) s11_imag = -MAX_S_PARAM_VALUE;
								else
								if (s11_imag >  MAX_S_PARAM_VALUE) s11_imag =  MAX_S_PARAM_VALUE;

								if (s21_real < -MAX_S_PARAM_VALUE) s21_real = -MAX_S_PARAM_VALUE;
								else
								if (s21_real >  MAX_S_PARAM_VALUE) s21_real =  MAX_S_PARAM_VALUE;
								if (s21_imag < -MAX_S_PARAM_VALUE) s21_imag = -MAX_S_PARAM_VALUE;
								else
								if (s21_imag >  MAX_S_PARAM_VALUE) s21_imag =  MAX_S_PARAM_VALUE;

								t_data_point fp;
								fp.Hz  = freq;
								fp.s11 = complexf(s11_real, s11_imag);
								fp.s21 = complexf(s21_real, s21_imag);

								Form1->m_freq_data_list.push_back(fp);
/*
								data_unit.m_point_incoming[pos] = fp;

								// give the user some feedback that we are actually receiving scanned points
								if ((i & 31) == 0)
									::PostMessage(Form1->Handle, WM_INCOMING_POINTS, (WPARAM)i, 0);
*/
							}
/*
							if (segment >= (segments - 1))
							{
								#if 0
									s.printf(L"%d points received", num_points);
									Form1->pushCommMessage("rx: " + s);
								#endif
								Form1->addNewRxData();
								data_unit.m_point_incoming.resize(0);
							}
*/
							if (Form1->m_freq_data_list.size() > 0)
							{
								Form1->addNewRxData(data_unit.m_point_incoming);
								data_unit.m_point_incoming.resize(0);
							}
						}
						break;

					case SERIAL_STATE_FREQDATA_RAW:
						{
							// '{'
							//
							// [n + 0] = 'x'
							// [n + 1] = round((dBm + 128) * 32) LS-Byte binary format
							// [n + 2] = round((dBm + 128) * 32) MS-Byte binary format
							//           .. repeats for as many points requested
							// '}'

							//const int segments           = data_unit.m_segments;
							//const int segment            = data_unit.m_segment;
							//const int points_per_segment = data_unit.m_points_per_segment;
							//const int num_points         = data_unit.m_points;

							Form1->m_freq_data_list.resize(0);

							const int num_points = ((int)m_rx_block.bin_data.size() - (1 + 1)) / (1 + 2);

							if (num_points > 0)
							{
								int i = 0;

								if (m_rx_block.bin_data[i++] != '{')
								{
									Form1->pushCommMessage("rx: " + String("freq_data_raw invalid start character at index ") + IntToStr(i));
									Form1->m_freq_data_list.resize(0);
									//::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
									break;
								}

								if (m_rx_block.bin_data[m_rx_block.bin_data.size() - 1] != '}')
								{
									Form1->pushCommMessage("rx: " + String("freq_data_raw invalid end character at index ") + IntToStr(i));
									Form1->m_freq_data_list.resize(0);
									//::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
									break;
								}

								while (i < (int)(m_rx_block.bin_data.size() - 1))
								{
									const char c = m_rx_block.bin_data[i++];
									if (c != 'x' && c != 'X')
									{	// unknown binary data type
										Form1->pushCommMessage("rx: " + String("freq_data_raw unknown binary data type at index ") + IntToStr(i));
										Form1->m_freq_data_list.resize(0);
										//::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
										break;
									}

									if (i > (int)(m_rx_block.bin_data.size() - 1 - 2))
									{	// missing data
										Form1->pushCommMessage("rx: " + String("freq_data_raw missing data at index ") + IntToStr(i));
										Form1->m_freq_data_list.resize(0);
										//::PostMessage(Form1->Handle, WM_CAPTURE_STOP, 0, 0);
										break;
									}

									const uint16_t data = ((uint16_t)m_rx_block.bin_data[i + 1] << 8) | m_rx_block.bin_data[i + 0];
									i += 2;

									const int64_t freq = data_unit.m_freq_start_Hz + I64ROUND(((double)data_unit.m_freq_span_Hz * Form1->m_freq_data_list.size()) / (num_points - 1));

									const float dBm = ((float)data / 32) - 128;
									const float lin = powf(10.0f, dBm / 20.0f);

									t_data_point fp;
									fp.Hz  = freq;
									fp.s11 = complexf (lin, 0);
									fp.s21 = 0;

									Form1->m_freq_data_list.push_back(fp);
/*
									data_unit.m_point_incoming[pos] = fp;

									// give the user some feedback that we are actually receiving scanned points
									if ((i & 31) == 0)
										::PostMessage(Form1->Handle, WM_INCOMING_POINTS, (WPARAM)i, 0);
*/
								}
/*
								#if 0
									s.printf(L"%d points received", num_points);
									Form1->pushCommMessage("rx: " + s);
								#endif
								Form1->addNewRxData();
								data_unit.m_point_incoming.resize(0);
*/
								if ((int)Form1->m_freq_data_list.size() == num_points)	// all seem good ?
								{
									Form1->addNewRxData(data_unit.m_point_incoming);
									data_unit.m_point_incoming.resize(0);
								}
							}
						}
						break;

					case SERIAL_STATE_SCREEN_CAPTURE:
					case SERIAL_STATE_SCREEN_FILL:
					case SERIAL_STATE_SCREEN_BULK:
						if (data_unit.m_vna_data.lcd_width >= 320 && data_unit.m_vna_data.lcd_height >= 240 && data_unit.m_vna_data.lcd_width <= 4096 && data_unit.m_vna_data.lcd_height <= 4096)
						{
							if (m_rx_block.bin_data_index == m_rx_block.bin_data.size())
							{
								if (m_capture_bm == NULL) {
									m_capture_bm = new Graphics::TBitmap();
									m_capture_bm->Monochrome  = false;
									m_capture_bm->Transparent = false;
									m_capture_bm->PixelFormat = pf16bit;
								}
								if (m_capture_bm == NULL) break;
								if (m_capture_bm->Width  != data_unit.m_vna_data.lcd_width ) m_capture_bm->Width = data_unit.m_vna_data.lcd_width;
								if (m_capture_bm->Height != data_unit.m_vna_data.lcd_height) m_capture_bm->Height = data_unit.m_vna_data.lcd_height;
								int16_t x0, x1, y0, y1;
								uint16_t *data;
								int step = 1;
								if (m_rx_block.type == SERIAL_STATE_SCREEN_CAPTURE) {
								   x0 = 0; x1 = m_capture_bm->Width;
								   y0 = 0; y1 = m_capture_bm->Height;
								   data = (uint16_t*)&m_rx_block.bin_data[0];
								}
								else if (m_rx_block.type == SERIAL_STATE_SCREEN_BULK){
								   x0 = m_region.x;
								   y0 = m_region.y;
								   x1 = m_region.w + x0;
								   y1 = m_region.h + y0;
								   data = (uint16_t*)&m_rx_block.bin_data[8];
								}
								else {
								   x0 = m_region.x;
								   y0 = m_region.y;
								   x1 = m_region.w + x0;
								   y1 = m_region.h + y0;
								   data = (uint16_t*)&m_rx_block.bin_data[8];
								   step = 0;
								}
								for (int16_t y = y0; y < y1; y++) {
									uint16_t *dst = (uint16_t *)m_capture_bm->ScanLine[y] + x0;
									for (int16_t x = x0; x < x1; x++, data+=step){
										*dst++ = (*data<<8)|(*data>>8);
									}
								}

								::PostMessage(Form1->Handle, WM_SCREEN_CAPTURE, 0, 0);
							}
						}

						break;

					case SERIAL_STATE_SD_LIST:
						break;

					case SERIAL_STATE_SD_READFILE:
						break;

					case SERIAL_STATE_MODE:
						break;
				}
			}
		}
//		else
//		{
//			Form1->pushCommMessage("rx: " + s.UpperCase() + " timed out");
//		}
	}

	m_rx_block.type = SERIAL_STATE_IDLE;
	m_rx_block.lines.resize(0);
	m_rx_block.bin_data.resize(0);
	m_rx_block.bin_data_index = 0;
	m_rx_block.timer.mark();
}

bool __fastcall CNanoVNA1Comms::processRxLine()
{
	if (m_rx_string.IsEmpty())
		return false;  // nothing to process

	const int pos = m_rx_string.LowerCase().Pos(COMMAND_PROMPT);
	if (pos > 0)
	{
		#ifdef SHOW_CMD_START_END
			if (m_rx_block.type != SERIAL_STATE_IDLE)
				Form1->pushCommMessage("rx: " + getSerialStateString(m_rx_block.type).UpperCase() + " done");
		#endif
		//Form1->pushCommMessage("rx: " + m_rx_string);

		m_rx_string = m_rx_string.SubString(pos + 3, m_rx_string.Length());

		if (!m_tx_command.IsEmpty())
		{
			m_tx_command = "";
		}

		processRxBlock();
	}
	else
	{
		if (m_rx_string != "fill" && m_rx_string != "bulk")
			Form1->pushCommMessage("rx: " + m_rx_string.Trim());
	}

	if (m_rx_string.IsEmpty())
		return false;

	if (m_rx_block.type != SERIAL_STATE_IDLE)
	{	// add the rx'ed text line into the string list
		m_rx_block.lines.push_back(m_rx_string.Trim());
		return true;
	}

	// parse up the rx'ed line
	std::vector <String> params;
	common.parseString(common.localiseDecimalPoint(m_rx_string), " ", params);

	if (params.empty())
		return true;

	m_rx_block.type = SERIAL_STATE_IDLE;
	m_rx_block.lines.resize(0);
	m_rx_block.bin_data.resize(0);
	m_rx_block.bin_data_index = 0;

	String cmd = params[0].LowerCase();

	t_serial_state block_type = SERIAL_STATE_IDLE;

	if (cmd == "help")
	{
		block_type = SERIAL_STATE_HELP;
	}
	else
	if (cmd == "version")
	{
		block_type = SERIAL_STATE_VERSION;
	}
	else
	if (cmd == "info")
	{
		block_type = SERIAL_STATE_INFO;
	}
	else
	if (cmd == "marker")
	{
		block_type = SERIAL_STATE_MARKER;
	}
	else
	if (cmd == "sweep")
	{
		block_type = SERIAL_STATE_SWEEP;
	}
	else
	if (cmd == "stat")
	{
		block_type = SERIAL_STATE_STAT;
	}
	else
	if (cmd == "time")
	{
		block_type = SERIAL_STATE_TIME;
	}
	else
	if (cmd == "vbat")
	{
		block_type = SERIAL_STATE_VBAT;
	}
	else
	if (cmd == "vbat_offset")
	{
		block_type = SERIAL_STATE_VBAT_OFFSET;
	}
	else
	if (cmd == "integrator")
	{
		block_type = SERIAL_STATE_INTEGRATOR;
	}
	else
	if (cmd == "bandwidth")
	{
		block_type = SERIAL_STATE_BANDWIDTH;	// "bandwidth 1 (1000Hz)"
	}
	else
	if (cmd == "threshold")
	{
		block_type = SERIAL_STATE_THRESHOLD;
	}
	else
	if (cmd == "edelay")
	{
		block_type = SERIAL_STATE_EDELAY;
	}
	else
	if (cmd == "deviceid")
	{
		block_type = SERIAL_STATE_DEVICE_ID;
	}
	else
	if (cmd == "cal")
	{
		block_type = SERIAL_STATE_CAL;
		// >ch cal on
		// >ch cal off
		if (params.size() >= 2)
		{
			String s = params[1].LowerCase();
			if (s == "off") data_unit.m_vna_data.cal = false;
			else
			if (s ==  "on") data_unit.m_vna_data.cal = true;
		}
	}
	else
	if (cmd == "power")
	{
		block_type = SERIAL_STATE_POWER;
	}
	else
	if (cmd == "usart")
	{
		block_type = SERIAL_STATE_USART;

		// tx: usart "AT+VERSION?"
		// rx: usart "AT+VERSION?"

		if (params.size() >= 2)
		{
			String s = params[1];

			// remove the double quotes
//			if (s.Length() >= 2 && s[1] == '\"')
//				if (s[s.Length()] == '\"')
//					s = s.SubString(2, s.Length() - 2).Trim();

			if (VNAUsartCommsForm)
				VNAUsartCommsForm->addText(FormatDateTime("ss.zzz ", Now()) + "rx: ", s);
		}
	}
	else
	if (cmd == "usart_cfg")
	{
		block_type = SERIAL_STATE_USART_CFG;
	}
	else
	if (cmd == "clearconfig")
	{
		block_type = SERIAL_STATE_CLEARCONFIG;
	}
	else
	if (cmd == "saveconfig")
	{
		block_type = SERIAL_STATE_SAVECONFIG;
	}
	else
	if (cmd == "capture")
	{
		if (data_unit.m_vna_data.lcd_width > 0 && data_unit.m_vna_data.lcd_height > 0)
		{
			block_type = SERIAL_STATE_SCREEN_CAPTURE;

			m_rx_block.bin_data.resize(data_unit.m_vna_data.lcd_width * data_unit.m_vna_data.lcd_height * 2);
			m_rx_block.bin_data_index = 0;
		}
	}
	else
	if (cmd == "fill")
	{
		if (data_unit.m_vna_data.lcd_width > 0 && data_unit.m_vna_data.lcd_height > 0)
		{
			block_type = SERIAL_STATE_SCREEN_FILL;

			m_rx_block.bin_data.resize(10);
			m_rx_block.bin_data_index = 0;
		}
	}
	else
	if (cmd == "bulk")
	{
		if (data_unit.m_vna_data.lcd_width > 0 && data_unit.m_vna_data.lcd_height > 0)
		{
			block_type = SERIAL_STATE_SCREEN_BULK;

			m_rx_block.bin_data.resize(8);
			m_rx_block.bin_data_index = 0;
		}
	}
	else
	if (cmd == "scan")
	{
		if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
		{	// NanoVNA
			// "scan 50000 900000000 101 7"
			if (params.size() >= 5)
			{
				block_type = SERIAL_STATE_FREQDATA;
				//block_type = SERIAL_STATE_FREQDATA_BIN;	// always use binary mode now
			}
		}
		else
		{	// TinySA
			// "scan 1000 200000000 290 7"
			if (params.size() >= 5)
			{
				block_type = SERIAL_STATE_FREQDATA;
			}
		}
	}
	else
	if (cmd == "scan_bin")
	{
		// "scan 50000 900000000 101 7"
		if (params.size() >= 5)
		{
			int num_points = 0;
			if (TryStrToInt(params[3], num_points))
			{
				if (num_points > 0)
				{
					// make the binary rx buffer the exact size of the expected data to follow
//					m_rx_block.bin_data.resize(xxxxxxxx);

					block_type = SERIAL_STATE_FREQDATA_BIN;
				}
			}
		}
	}
	else
	if (cmd == "scanraw")
	{
		if (data_unit.m_vna_data.type != UNIT_TYPE_TINYSA)
		{	// NanoVNA

		}
		else
		{	// TinySA
			// "scanraw 50000 900000000 290"
			if (params.size() >= 4)
			{
				int num_points = 0;
				if (TryStrToInt(params[3], num_points))
				{
					if (num_points > 0)
					{
						// make the binary rx buffer the exact size of the expected data to follow
						m_rx_block.bin_data.resize(1 + ((1 + 2) * num_points) + 1);

						block_type = SERIAL_STATE_FREQDATA_RAW;
					}
				}
			}
		}
	}
	else
	if (cmd == "pause")
	{
		block_type = SERIAL_STATE_PAUSE;
	}
	else
	if (cmd == "resume")
	{
		block_type = SERIAL_STATE_RESUME;
	}
	else
	if (cmd == "mode")
	{
		block_type = SERIAL_STATE_MODE;
	}
	else
	if (cmd == "reset")
	{
		block_type = SERIAL_STATE_RESET;
	}
	else
	if (cmd == "data")
	{
		if (params.size() >= 2)
		{
			if (params[1] == "0") block_type = SERIAL_STATE_DATA0;
			else
			if (params[1] == "1") block_type = SERIAL_STATE_DATA1;
			else
			if (params[1] == "2") block_type = SERIAL_STATE_DATA2;
			else
			if (params[1] == "3") block_type = SERIAL_STATE_DATA3;
			else
			if (params[1] == "4") block_type = SERIAL_STATE_DATA4;
			else
			if (params[1] == "5") block_type = SERIAL_STATE_DATA5;
			else
			if (params[1] == "6") block_type = SERIAL_STATE_DATA6;
			else
			if (params[1] == "7") block_type = SERIAL_STATE_DATA7;
		}
	}
	else
	if (cmd == "touch" || cmd == "release" || cmd == "refresh")
	{
		return true;
	}
	else
	if (cmd == "sd_list")
	{
		//block_type = SERIAL_STATE_SD_LIST;
		return true;
	}
	else
	if (cmd == "sd_readfile")
	{
		//block_type = SERIAL_STATE_SD_READFILE;
		return true;
	}
	else
	{	// unknown block tyoe
		#ifdef _DEBUG
			Form1->pushCommMessage("rx: unknown block type [" + m_rx_string + "]");
		#endif
/*
		if (!cmd.IsEmpty())
		{
			if (cmd[cmd.Length()] == '?')
			{	// the unit replied with an error .. try again

			}
		}
*/
		return true;
	}

	// found start of rx block

	m_rx_block.type = block_type;
	m_rx_block.timer.mark();

	#ifdef SHOW_CMD_START_END
		Form1->pushCommMessage("rx: " + getSerialStateString(block_type).UpperCase() + " start");
	#endif

	return true;
}

int __fastcall CNanoVNA1Comms::processRx(t_serial_buffer &serial_buffer)
{
	if (serial_buffer.buffer_wr <= 0)
		return 0;	// no data to process

	// add the received bytes into the correct rx buffer for processing

	unsigned int k = 0;
	while (k < serial_buffer.buffer_wr)
	{
		switch (m_rx_block.type)
		{
			case SERIAL_STATE_SCREEN_BULK:
			case SERIAL_STATE_SCREEN_FILL:
			{
				const uint8_t b = serial_buffer.buffer[k];
#if 0
				if (m_rx_block.bin_data_index == 0)
				{
					if (b == '\r' || b == '\n') {	// drop any initial carriage return and line feed characters
						k++;
						break;
					}
				}
#endif
				if (m_rx_block.bin_data_index < 8)
				{
					m_rx_block.bin_data[m_rx_block.bin_data_index++] = b;
					k++;
					if (m_rx_block.bin_data_index >= 8)
					{
						int i = 0;
						m_region.x = ((uint16_t)m_rx_block.bin_data[1] << 8) | (m_rx_block.bin_data[0] << 0);
						m_region.y = ((uint16_t)m_rx_block.bin_data[3] << 8) | (m_rx_block.bin_data[2] << 0);
						m_region.w = ((uint16_t)m_rx_block.bin_data[5] << 8) | (m_rx_block.bin_data[4] << 0);
						m_region.h = ((uint16_t)m_rx_block.bin_data[7] << 8) | (m_rx_block.bin_data[6] << 0);
						if (m_region.w*m_region.h*2 <= 2048 && m_region.x >= 0 && m_region.x < 480 && m_region.y >= 0 && m_region.y < 320)
						{
							if (m_rx_block.type == SERIAL_STATE_SCREEN_BULK)
								m_rx_block.bin_data.resize(8 + m_region.w*m_region.h*2);	// make room for the following data
							else
								m_rx_block.bin_data.resize(8 + 2);												// make room for the following data
						}
					}
				} else {
					int available_bytes = serial_buffer.buffer_wr - k;
					int available_space = (int)m_rx_block.bin_data.size() - (int)m_rx_block.bin_data_index;
					if (available_bytes > available_space)
						available_bytes  = available_space;
					if (available_bytes > 0)
					{	// copy the rx'ed data into the rx binary buffer
						memcpy(&m_rx_block.bin_data[m_rx_block.bin_data_index], &serial_buffer.buffer[k], available_bytes);
						m_rx_block.bin_data_index += available_bytes;
						k += available_bytes;
					}

					if (m_rx_block.bin_data_index >= m_rx_block.bin_data.size())
					{	// done
						if (k > 0)
						{	// remove used data from the rx serial buffer
							if (k < serial_buffer.buffer_wr)
								memmove(&serial_buffer.buffer[0], &serial_buffer.buffer[k], serial_buffer.buffer_wr - k);
							serial_buffer.buffer_wr -= k;
							k = 0;
						}
						processRxBlock();
					}
				}
				break;

			}
			case SERIAL_STATE_SD_READFILE:
			case SERIAL_STATE_SCREEN_CAPTURE:
			case SERIAL_STATE_FREQDATA_RAW:
				{
					if (m_rx_block.bin_data_index == 0)
					{
						const uint8_t b = serial_buffer.buffer[k];
						if (b == '\r' || b == '\n')	// drop any initial carriage return and line feed characters
						{
							k++;
							break;
						}
					}

					int available_bytes = serial_buffer.buffer_wr - k;
					int available_space = (int)m_rx_block.bin_data.size() - (int)m_rx_block.bin_data_index;
					if (available_bytes > available_space)
						available_bytes  = available_space;
					if (available_bytes > 0)
					{	// copy the rx'ed data into the rx binary buffer
						memcpy(&m_rx_block.bin_data[m_rx_block.bin_data_index], &serial_buffer.buffer[k], available_bytes);
						m_rx_block.bin_data_index += available_bytes;
						k += available_bytes;
					}

					if (m_rx_block.bin_data_index >= m_rx_block.bin_data.size())
					{	// done

						if (k > 0)
						{	// remove used data from the rx serial buffer
							if (k < serial_buffer.buffer_wr)
								memmove(&serial_buffer.buffer[0], &serial_buffer.buffer[k], serial_buffer.buffer_wr - k);
							serial_buffer.buffer_wr -= k;
							k = 0;
						}

						String s;
						s.printf(L"%u bytes received", m_rx_block.bin_data_index);
						Form1->pushCommMessage("rx: " + s);
						#ifdef SHOW_CMD_START_END
							Form1->pushCommMessage("rx: " + getSerialStateString(m_rx_block.type).UpperCase() + " done");
						#endif

						processRxBlock();
					}
				}
				break;

			case SERIAL_STATE_FREQDATA_BIN:		// expecting binary data
				{
					const uint8_t b = serial_buffer.buffer[k++];

					if (m_rx_block.bin_data_index == 0)
					{
						if (b == '\r' || b == '\n')	// drop any initial carriage return and line feed characters
							break;
					}

					if (m_rx_block.bin_data_index < 4)
					{
						m_rx_block.bin_data.push_back(b);
						if (++m_rx_block.bin_data_index >= 4)
						{
							int i = 0;

							const uint16_t mask = ((uint16_t)m_rx_block.bin_data[1] << 8) | (m_rx_block.bin_data[0] << 0);
							if (mask & SCAN_MASK_OUT_FREQ)  i += 4;	// frequency
							if (mask & SCAN_MASK_OUT_DATA0) i += 8;	// s11
							if (mask & SCAN_MASK_OUT_DATA1) i += 8;	// s21
							// SCAN_MASK_BINARY

							// number of points to follow
							const uint16_t points = ((uint16_t)m_rx_block.bin_data[3] << 8) | (m_rx_block.bin_data[2] << 0);
							if (points < 1024)
							{
								i *= points;
								m_rx_block.bin_data.resize(m_rx_block.bin_data.size() + i);	// make room for the following data
							}
							else
							{	// error ?
								String s;
								s.printf(L"%u bytes receive error", m_rx_block.bin_data_index);
								Form1->pushCommMessage("rx: " + s);

								m_rx_block.type = SERIAL_STATE_IDLE;
								m_rx_block.bin_data_index = 0;
								m_rx_block.bin_data.resize(0);
							}
						}
					}
					else
					{	// data
						m_rx_block.bin_data[m_rx_block.bin_data_index++] = b;

						{	// fetch the rest in one go
							int available_bytes = serial_buffer.buffer_wr - k;
							int available_space = m_rx_block.bin_data.size() - m_rx_block.bin_data_index;
							if (available_bytes > available_space)
								available_bytes = available_space;
							if (available_bytes > 0)
							{
								memcpy(&m_rx_block.bin_data[m_rx_block.bin_data_index], &serial_buffer.buffer[k], available_bytes);
								m_rx_block.bin_data_index += available_bytes;
								k += available_bytes;
							}
						}

						if (m_rx_block.bin_data_index >= m_rx_block.bin_data.size())
						{	// done

							if (k > 0)
							{	// remove used data
								if (k < serial_buffer.buffer_wr)
									memmove(&serial_buffer.buffer[0], &serial_buffer.buffer[k], serial_buffer.buffer_wr - k);
								serial_buffer.buffer_wr -= k;
								k = 0;
							}

							String s;
							s.printf(L"%u bytes received", m_rx_block.bin_data_index);
							Form1->pushCommMessage("rx: " + s);
							#ifdef SHOW_CMD_START_END
								Form1->pushCommMessage("rx: " + getSerialStateString(m_rx_block.type).UpperCase() + " done");
							#endif

							processRxBlock();
						}
					}
				}
				break;

			default:		// expecting text data
				{
					char c = serial_buffer.buffer[k++];

					// replace all tabs with spaces
					if (c == '\t')
						c = ' ';

					if (c >= 32)
					{  // non-control code character .. simply add it to the rx line
						m_rx_string += c;
						break;
					}

					if (c == '\n')
						break;	// ignore line feeds

					if (c != '\r')
					{	// unexpected control character
						String s;
						s.printf(L"[%02x]", c);
						m_rx_string += s;
						break;
					}

					// carriage return

					// skip over any following linefeed
					if (k < serial_buffer.buffer_wr)
						if (serial_buffer.buffer[k] == '\n')
							k++;

					if (k > 0)
					{	// remove used data from the rx buffer
						if (k < serial_buffer.buffer_wr)
							memmove(&serial_buffer.buffer[0], &serial_buffer.buffer[k], serial_buffer.buffer_wr - k);
						serial_buffer.buffer_wr -= k;
						k = 0;
					}

					// process the rx'ed line
					if (processRxLine())
					{
						// clear the rx line ready for the next
						m_rx_string = "";
					}
				}
				break;
		}
	}

	if (k > 0)
	{	// remove used data from the rx buffer
		if (k < serial_buffer.buffer_wr)
			memmove(&serial_buffer.buffer[0], &serial_buffer.buffer[k], serial_buffer.buffer_wr - k);
		serial_buffer.buffer_wr -= k;
		//k = 0;
	}

	if (	m_rx_block.type == SERIAL_STATE_SD_READFILE ||
			m_rx_block.type == SERIAL_STATE_SCREEN_CAPTURE ||
			m_rx_block.type == SERIAL_STATE_SCREEN_FILL ||
			m_rx_block.type == SERIAL_STATE_SCREEN_BULK ||
			m_rx_block.type == SERIAL_STATE_FREQDATA_BIN ||
			m_rx_block.type == SERIAL_STATE_FREQDATA_RAW)
	{	// currently receiving binary data
		return 0;
	}

	// text mode

	const int pos = m_rx_string.LowerCase().Pos(COMMAND_PROMPT);
	if (pos > 0)
	{
		#ifdef SHOW_CMD_START_END
			if (m_rx_block.type != SERIAL_STATE_IDLE)
				Form1->pushCommMessage("rx: " + getSerialStateString(m_rx_block.type).UpperCase() + " done");
		#endif
		Form1->pushCommMessage("rx: " + m_rx_string);

		//serial_buffer.buffer_wr = 0;

		m_rx_string = m_rx_string.SubString(pos + 3, m_rx_string.Length());

		if (!m_tx_command.IsEmpty())
		{
			m_tx_command = "";
		}

		processRxBlock();
	}

	return 0;
}

