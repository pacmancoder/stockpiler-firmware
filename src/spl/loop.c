#include <spl/loop.h>

#include <spl/spl.h>
#include <spl/utils.h>
#include <spl/device_state.h>
#include <spl/timings.h>

static volatile uint16_t clocksSpent;
static volatile uint16_t clocksHigh;
static volatile uint16_t clocksLow;

uint8_t gRxCommandIsNotBroadcast;

// Error handling


void on_idle(void) {
	if (!(PA & (1 << SPL_PIN_BUTTON))) {
		gState = SPL_STATE_BUTTON_PRESSED;
		goto FINNALIZE_IDLE_STATE;
	}

	if (!(PA & (1 << SPL_PIN_BUS))) {
		gState = SPL_STATE_BUS_ACTIVATED;
		goto FINNALIZE_IDLE_STATE;
	}

	// Process LED PWM with max possible PWM speed for SysClock (8MHz Sysclock = 31 KHz PWM)
    if (gPwmCounter < gRedLedValue) { __set0(PA, SPL_PIN_LED_R); } else { __set1(PA, SPL_PIN_LED_R); }
    if (gPwmCounter < gGreenLedValue) { __set0(PA, SPL_PIN_LED_G); } else { __set1(PA, SPL_PIN_LED_G); }
    if (gPwmCounter < gBlueLedValue) { __set0(PA, SPL_PIN_LED_B); } else { __set1(PA, SPL_PIN_LED_B); }
	gPwmCounter++;

	return;

FINNALIZE_IDLE_STATE:
	disable_rgb_led();
}

void on_bus_activated(void) {
	reset_timer16_counter();
    T16M = T16M_CLK_SYSCLK | T16M_CLK_DIV1 | T16M_INTSRC_15BIT;

	while(1) {
        __asm__("ldt16 _clocksSpent");
		if (clocksSpent > CLOCKS_RECEIVE_FAILURE) {
			// Detected busy bus; wait unil it is clear
			gState = SPL_STATE_IDLE;
			goto WAIT_BUS_CLEAR;
		}

		if (PA & (1 << SPL_PIN_BUS)) {
			// Detected start bit; read command.
			gState = SPL_STATE_READ_COMMAND;
			goto FINALIZE_BUS_ACTIVATION;
		}
	}
WAIT_BUS_CLEAR:
	while (!(PA & (1 << SPL_PIN_BUS))) {
        __nop();
	}

FINALIZE_BUS_ACTIVATION:
    T16M = T16M_CLK_DISABLE;
}


void on_button_pressed(void) {
	send_btn_command();
	// Wait for button release
	while (!(PA & (1 << SPL_PIN_BUTTON))) {
		__nop();
	}
	// go back to idle
	gState = SPL_STATE_IDLE;
}

void on_read_command(void) {
	reset_timer16_counter();
    T16M = T16M_CLK_SYSCLK | T16M_CLK_DIV1 | T16M_INTSRC_15BIT;

	uint8_t cmdByteIdx = SPL_COMMAND_SIZE;
	clocksHigh = 0;
	clocksLow = 0;

	gMemPtr = gRxCommand;
	do {
		uint8_t cmdByte = 0x00;
        uint8_t bitIndex = 8;
        do {
			// Wait for 1 -> 0 transition
			reset_timer16_counter();
			while (PA & (1 << SPL_PIN_BUS)) {
				// validate that high signal is not greater than 100us
                __asm__("ldt16 _clocksHigh");
				if (clocksHigh > CLOCKS_RECEIVE_FAILURE) {
					gErrorCause = SPL_ERR_HIGH_TIMEOUT;
					goto ERROR;
				}
		    }
			// Wait for 0 -> 1 transition
			reset_timer16_counter();
			while (!(PA & (1 << SPL_PIN_BUS)))  {
				// validate that low signal is not greater than 100us
                __asm__("ldt16 _clocksLow");
				if (clocksLow > CLOCKS_RECEIVE_FAILURE) {
					gErrorCause = SPL_ERR_LOW_TIMEOUT;
					goto ERROR;
				}
			}
			// [|^|___] -> LOW; [|^^^|_] -> HIGH
			if (clocksHigh > clocksLow) {
                cmdByte |= 0x01;
			}
            cmdByte <<= 1;

		} while (--bitIndex);

		*gMemPtr = cmdByte;
		gMemPtr++;
	} while (--cmdByteIdx);

	// Validate checksum
	gMemPtr = gRxCommand;
	calculate_command_checksum();
	gMemPtr = gRxCommand + CMD_OFFSET_CHECKSUM;
	if (*gMemPtr != gCurrentChecksum) {
		gErrorCause = SPL_ERR_CHECKSUM;
		goto ERROR;
	}

	// Check that device id is matched
	uint8_t expectedDeviceIdByte;
	bool invalidDeviceId = false;
	gRxCommandIsNotBroadcast = 0x00;
    for (int offset = 0; offset < 4; ++offset) {
		gMemPtr = gTxCommand + CMD_OFFSET_DEVICE_ID + offset;
		expectedDeviceIdByte = *gMemPtr;
		gMemPtr = gRxCommand + CMD_OFFSET_DEVICE_ID + offset;
		// Broadcast id detection (0x00000000)
		gRxCommandIsNotBroadcast |= *gMemPtr;
		if (expectedDeviceIdByte != *gMemPtr) {
			invalidDeviceId = true;
		}
    }

	// if not broadcast id and device id not matched -> skip command
	if (invalidDeviceId && (gRxCommandIsNotBroadcast != 0)) {
		gState = SPL_STATE_IDLE;
		goto FINALIZE;
	}

	// Interpret command
	gState = SPL_STATE_EXECUTE_COMMAND;
	goto FINALIZE;

ERROR:
	gState = SPL_STATE_FAILURE;
FINALIZE:
    T16M = T16M_CLK_DISABLE;
}

void on_execute_command(void) {
	gMemPtr = gRxCommand + CMD_OFFSET_ID;
	uint8_t cmd = *gMemPtr;
	uint8_t isAckRequired = cmd & 0x80;
    cmd &= 0x7F; // And with 0x7F to remove ACK requirement

	switch (cmd) {
		case SPL_CMD_RGB_LED: {
			gMemPtr = gRxCommand + CMD_OFFSET_RGB_LED_FLAGS;
			uint8_t ledCommandFlags = *gMemPtr;
			if (ledCommandFlags & SPL_FLAG_RGB_LED_SET_NEXT_COLOR) {
				gMemPtr = gRxCommand + CMD_OFFSET_RGB_LED_RED;
				gRedLedShadow = *gMemPtr;
				gMemPtr = gRxCommand + CMD_OFFSET_RGB_LED_GREEN;
				gGreenLedShadow = *gMemPtr;
				gMemPtr = gRxCommand + CMD_OFFSET_RGB_LED_BLUE;
				gBlueLedShadow = *gMemPtr;
			}
			if (ledCommandFlags & SPL_FLAG_RGB_LED_UPDATE) {
				update_leds();
			}
			break;
		}
		case SPL_CMD_PING: {
			break;
		}
		case SPL_CMD_SLEEP: {
			// send ACK before going to sleep
			if (isAckRequired && gRxCommandIsNotBroadcast) {
				send_ack_command();
				isAckRequired = 0;
			}

			// Stop execution
			__stopsys();
			// Wait untl wake up impulse ends
			while (!(PA & (1 << SPL_PIN_BUS))) {
				__nop();
			}
			break;
		}
	}

	if (isAckRequired && gRxCommandIsNotBroadcast) {
		send_ack_command();
	}

	gState = SPL_STATE_IDLE;
}

void on_failure(void) {
    WAIT_ERROR;
	gState = SPL_STATE_IDLE;
}

void spl_step(void) {
	switch (gState) {
		case SPL_STATE_IDLE: {
			on_idle();
			break;
		}
		case SPL_STATE_BUS_ACTIVATED: {
			on_bus_activated();
			break;
		}
		case SPL_STATE_BUTTON_PRESSED: {
			on_button_pressed();
			break;
		}
		case SPL_STATE_READ_COMMAND: {
			on_read_command();
			break;
		}
		case SPL_STATE_FAILURE: {
			on_failure();
			break;
		}
		case SPL_STATE_EXECUTE_COMMAND: {
			on_execute_command();
			break;
		}
		default: {
			gState = SPL_STATE_IDLE;
		}
	}
}