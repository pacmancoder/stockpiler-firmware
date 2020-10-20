#include <spl/utils.h>

#include <stdint.h>
#include <pdk/device.h>
#include <spl/spl.h>
#include <spl/timings.h>
#include <spl/device_state.h>

static uint16_t tm16Value = 0; 

void reset_timer16_counter(void) {
	uint8_t timerMode = T16M;
    T16M = T16M_CLK_DISABLE;
    T16C = 0;
	T16M = timerMode;
}


static void send_bit1(void) {
    __set1(PA, SPL_PIN_BUS);
    WAIT_BIT_1_HIGH;
	__set0(PA, SPL_PIN_BUS);
	WAIT_BIT_1_LOW;
}

static void send_bit0(void) {
    __set1(PA, SPL_PIN_BUS);
    WAIT_BIT_0_HIGH;
	__set0(PA, SPL_PIN_BUS);
	WAIT_BIT_0_LOW;
}

void send_command(void) {
	// Set bus line as output
    __set1(PAC, SPL_PIN_BUS);

	// Send start bit
    __set0(PA, SPL_PIN_BUS);
	WAIT_START_BIT;

	// Send command
	gMemPtr = gTxCommand;

	uint8_t cmdByteIdx = SPL_COMMAND_SIZE;
	do {
        uint8_t gCmdByte = *gMemPtr;
        // Send bits from msb to lsb

        uint8_t cmdBitIdx = 8;
        do {
            if (gCmdByte & 0x80) {
                send_bit1();
            } else {
                send_bit0();
            }
            gCmdByte <<= 1;
        } while (--cmdBitIdx);

		// Move to the next byte
		gMemPtr++;
	} while (--cmdByteIdx);

	// Send stop bit
    __set1(PA, SPL_PIN_BUS);
	WAIT_STOP_BIT;

	// Switch back to input mode
	__set0(PAC, SPL_PIN_BUS);
}

void send_ack_command(void) {
	// Wait some time to give receiver time to receive ACK
    WAIT_BUS_OWNER_SWITCH;

	// Set command to ACK
	gMemPtr = gTxCommand + CMD_OFFSET_ID;
	*gMemPtr = SPL_CMD_ACK;
	// device id is set at device initialization
	gMemPtr = gTxCommand + CMD_OFFSET_PAYLOAD;
	*gMemPtr = 'D'; gMemPtr++;
	*gMemPtr = 'O'; gMemPtr++;
	*gMemPtr = 'N'; gMemPtr++;
	*gMemPtr = 'E';
	// calculate  checksum
	update_tx_checksum();
	// command is ready, send it
	send_command();
}

void send_btn_command(void) {
	// Set command to BUTTON_PRESSED
	gMemPtr = gTxCommand + CMD_OFFSET_ID;
	*gMemPtr = SPL_CMD_BUTTON_PRESSED;
	// device id is set at device initialization
	gMemPtr = gTxCommand + CMD_OFFSET_PAYLOAD;
	*gMemPtr = 0x01; gMemPtr++;
	*gMemPtr = 0x00; gMemPtr++;
	*gMemPtr = 0x00; gMemPtr++;
	*gMemPtr = 0x00;
	// calculate  checksum
	update_tx_checksum();
	// command is ready, send it
	send_command();
}


// Calculated checksum for command pointer by gMemPtr
// Note that calculate_checksum modifies gMemPtr
void calculate_command_checksum(void) {
	gCurrentChecksum = 0x00;
    uint8_t bytesLeft = SPL_COMMAND_SIZE - 1;
    do {
		gCurrentChecksum += *gMemPtr;
		gMemPtr++;
    } while (--bytesLeft);
}

void update_tx_checksum(void) {
	gMemPtr = gTxCommand;
	calculate_command_checksum();
	gMemPtr = gTxCommand + CMD_OFFSET_CHECKSUM;
	*gMemPtr = gCurrentChecksum;
}

void update_leds(void) {
    gRedLedValue = gRedLedShadow;
    gGreenLedValue = gGreenLedShadow;
    gBlueLedValue = gBlueLedShadow;
}


void disable_rgb_led(void) {
    PA |= (1 << SPL_PIN_LED_R) | (1 << SPL_PIN_LED_G) | (1 << SPL_PIN_LED_B); 
}