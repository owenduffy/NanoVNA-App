
#ifndef NanoVNA_v2_commsH
#define NanoVNA_v2_commsH

#include "types.h"
#include "HighResolutionTick.h"

// *******************************
// NanoVNA V2 binary commands

#define CMD_V2_NOP                       0x00    // no operation
#define CMD_V2_INDICATE                  0x0D    // device always replies with ascii ‘2’ (0x32)
#define CMD_V2_READ_1                    0x10    // read 1 byte  from register
#define CMD_V2_READ_2                    0x11    // read 2 bytes from register
#define CMD_V2_READ_4                    0x12    // read 4 bytes from register
#define CMD_V2_READ_8                    0x13    // read 8 bytes from register
#define CMD_V2_READ_FIFO                 0x18    // read n bytes from an FIFO
#define CMD_V2_WRITE_1                   0x20    // write 1 byte to a register
#define CMD_V2_WRITE_2                   0x21    // write 2 bytes to a register
#define CMD_V2_WRITE_4                   0x22    // write 4 bytes to a register
#define CMD_V2_WRITE_8                   0x23    // write 8 bytes to a register
#define CMD_V2_WRITE_FIFO                0x28    // write n bytes to an FIFO

// *******************************
// NanoVNA V2 registers

// 20 E2 05 .. sets ecal interval to 5, maybe try settings it to 2

#define REG_V2_SWEEP_START_HZ            0x00    // uint64 ... sets the sweep start frequency in Hz
#define REG_V2_SWEEP_STEP_HZ             0x10    // uint64 ... sets the sweep step frequency in Hz
#define REG_V2_SWEEP_POINTS              0x20    // uint16 ... sets the number of sweep frequency points
#define REG_V2_VALUES_PER_FREQUENCY      0x22    // uint16 ... sets the number of data points to output for each frequency

#define REG_V2_RAW_SAMPLES_MODE          0x26    // uint8 .... writing '1' switches USB data format to raw sample mode and leaves this protocol

#define REG_V2_VALUES_FIFO               0x30    //            returns VNA sweep data points
																 //            each value is 32 bytes - see 't_v2_fifo' below
																 //            writing any value (using WRITE command) clears the FIFO

#define REG_V2_AVERAGE_SETTING           0x40    // uint8_t .. average setting
#define REG_V2_SI5351_POWER              0x41    // uint8_t .. power (reserved)
#define REG_V2_ADF4350_POWER             0x42    // uint8_t .. power

#define REG_V2_UNIX_TIME                 0x58    // uint32_t .. unix time

#define REG_V2_CAPTURE_SCREEN            0xEE    // t_v2_capture_screen_header followed by image data
#define REG_V2_DEVICE_VARIANT            0xF0    // uint8 .... the type of device this is - always 0x02 for NanoVNA V2
#define REG_V2_PROTCOL_VERSION           0xF1    // uint8 .... version of this wire protocol - always 0x01
#define REG_V2_HARDWARE_REVISION         0xF2    // uint8 .... hardware revision
#define REG_V2_FIRMWARE_MAJOR            0xF3    // uint8 .... firmware major version
#define REG_V2_FIRMWARE_MINOR            0xF4    // uint8 .... firmware minor version

// *******************************

#define CMD_V2_INDICATE_ACK              0x32    // 1 byte ACK we should receive after sending the above INDICATE command

#define REG_V2_DEVICE_VARIANT_ACK        0x02    // uint8
#define REG_V2_PROTOCOL_VERSION_ACK      0x01    // uint8
#define REG_V2_HARDWARE_REVISION_ACK_2_2 0x02    // uint8
#define REG_V2_HARDWARE_REVISION_ACK_2_3 0x03    // uint8
#define REG_V2_HARDWARE_REVISION_ACK_2_4 0x04    // uint8

#define REG_V2_AVERAGE_SETTING_MAX       255
#define REG_V2_AVERAGE_SETTING_MIN       1

#define REG_V2_SI5351_POWER_MAX          3
#define REG_V2_SI5351_POWER_MIN          0

#define REG_V2_ADF4350_POWER_MAX         3
#define REG_V2_ADF4350_POWER_MIN         0

// *******************************
// NanoVNA v2 DFU mode registers

#define REG_DFU_V2_FLASH_WRITE_START     0xE0    // uint32 ... current flash write address - set this to the address to start writing at
#define REG_DFU_V2_FLASH_FIFO            0xE4    // uint8 .... writing to this FIFO will write data into flash starting at flashWriteStart
																 //            flashWriteStart will be incremented by the number of bytes written
#define REG_DFU_V2_USER_ARGUMENT         0xE8    // uint32 ... the user argument to pass to the program upon soft reset
#define REG_DFU_V2_DO_REBOOT             0xEF    // uint8 .... write 0x5e to initiate a soft reset
#define REG_DFU_V2_DEVICE_VARIANT        0xF0    // uint8 .... the type of device this is - always 0x02 for NanoVNA V2
#define REG_DFU_V2_PROTOCOL_VERSION      0xF1    // uint8 .... version of this wire protocol - always 0x01
#define REG_DFU_V2_HARDWARE_REVISION     0xF2    // uint8 .... hardware revision - always 0x00 in DFU mode
#define REG_DFU_V2_FIRMWARE_MAJOR        0xF3    // uint8 .... firmware major version - always 0xff in DFU mode
#define REG_DFU_V2_FIRMWARE_MINOR        0xF4    // uint8 .... firmware minor version (of the bootloader)

#define REG_DFU_V2_DEVICE_VARIANT_ACK    0x02    // uint8
#define REG_DFU_V2_PROTOCOL_VERSION_ACK  0x01    // uint8
#define REG_DFU_V2_HARDWARE_REVISION_ACK 0x00    // uint8
#define REG_DFU_V2_FIRMWARE_MAJOR_ACK    0xFF    // uint8

// *******************************
// maximum number of data points that can be requested via the USB link

#define V2_USB_POINTS_MAX                1024
#define V2PLUS_USB_POINTS_MAX            1024
#define V2PLUS4_USB_POINTS_MAX           65536

// *******************************

#pragma pack(push, 1)

// FIF0 format .. 32 bytes in size
struct t_v2_fifo_sparam
{
	int32_t  real_ch0_fwd;
	int32_t  imag_ch0_fwd;

	int32_t  real_ch0_rev;
	int32_t  imag_ch0_rev;

	int32_t  real_ch1_rev;
	int32_t  imag_ch1_rev;

	uint16_t freq_index;

	uint8_t  reserved[5];

	uint8_t  checksum;
};

// capture screen format details .. 5 bytes in size
struct t_v2_capture_screen_header
{
	uint16_t width;
	uint16_t height;
	uint8_t  pixel_format;
};

#pragma pack(pop)

// *******************************

class CNanoVNA2Comms
{
	private:
		t_mode m_mode;			// current mode of operation .. idle mode, initialisation mode, scanning/sweeping mode etc
		t_mode m_new_mode;	// requested mode

		bool m_usb_data_mode;

		int m_points_requested;

		int64_t m_start;
		int64_t m_step;
		uint16_t m_points_per_segment;
		uint16_t m_data_points_per_frequency;

		t_v2_capture_screen_header m_cap_header;

		int __fastcall requestPoints(int num_points);
		void __fastcall newUnit();
		void __fastcall setMode(const t_mode mode);

	public:
		CNanoVNA2Comms();
		~CNanoVNA2Comms();

		std::vector <uint8_t> m_tx_cmd;

		int m_retries;

		bool m_pause_comms;

		int m_poll_ms;
		int m_state_timeout_ms;

		Graphics::TBitmap *m_capture_bm;
		bool m_get_screen_capture;

		CHighResolutionTick m_poll_timer;
		CHighResolutionTick m_new_points_timer;

		void __fastcall sendData(void *data = NULL, int size = -1);
		void __fastcall resetScan();
		void __fastcall reset(const bool reset_vna);
		void __fastcall clearTxCommands();
		void __fastcall addTxNulls(const bool reset_buf = true, const int num = 8);
		void __fastcall addTxRead1(const uint8_t reg_addr);
		void __fastcall addTxRead2(const uint8_t reg_addr);
		void __fastcall addTxRead4(const uint8_t reg_addr);
		void __fastcall addTxRead8(const uint8_t reg_addr);
		void __fastcall addTxWrite1(const uint8_t reg_addr, uint8_t value);
		void __fastcall addTxWrite2(const uint8_t reg_addr, uint16_t value);
		void __fastcall addTxWrite4(const uint8_t reg_addr, uint32_t value);
		void __fastcall addTxWrite8(const uint8_t reg_addr, uint64_t value);
		void __fastcall addTxFlashWriteAddr(const uint32_t addr);
		void __fastcall addTxUserArgument(const uint32_t arg);
		void __fastcall addTxWriteFlashData(const bool reset_buf, std::vector <uint8_t> &data, const bool ack = true);
		void __fastcall enterUSBDataMode(const bool send_nulls);
		void __fastcall leaveUSBDataMode(const bool send_nulls);
		void __fastcall setAverageSetting(int value);
		void __fastcall setSI5351OutputPower(int value);
		void __fastcall setADF4350OutputPower(int value);
		void __fastcall setTime(int year, int month, int day, int hour, int min, int sec);
		bool __fastcall inDFUMode();
		void __fastcall softReboot();
		bool __fastcall requestCapture();
		void __fastcall requestScan();
		void __fastcall pauseComms(bool pause);
		void __fastcall poll();
		int __fastcall processRx(t_serial_buffer &serial_buffer);

		__property t_mode mode = {read = m_mode, write = setMode};
};

extern CNanoVNA2Comms nanovna2_comms;

#endif

