#include <pdk/device.h>
#include <spl/spl.h>
#include <spl/timings.h>
#include <spl/device_state.h>
#include <spl/loop.h>

void main(void) {
    // >>> IO Initialization
	// LED RED; Sink output; Default value: 1
	// LED BLU; Sink output; Default value: 1
	// LED GRN; Sink output; Default value: 1
	// SPL BUS; Input/[Output]; Open drain; Default value: 1
	// DBG PIN; Output; Default value: 0
	// SPL BTN; Input; With pull up; Default value: 0

	PAPH = ((uint8_t)1 << SPL_PIN_BUTTON);
    PA = (1 << SPL_PIN_LED_R) | (1 << SPL_PIN_LED_G) | (1 << SPL_PIN_LED_B) | (1 << SPL_PIN_BUS);
    PAC = (1 << SPL_PIN_LED_R) | (1 << SPL_PIN_LED_G) | (1 << SPL_PIN_LED_B) | (1 << SPL_PIN_DEBUG);
    PADIER = (1 << SPL_PIN_BUS) | (1 << SPL_PIN_BUTTON);

	// >>> Initialize peripherals
	MISC = 0b00100000;  // Enable fast wake up

	// >>> Initialize tx command deviceId;
	gTxCommand[CMD_OFFSET_DEVICE_ID] = (uint8_t)((SPL_DEVICE_ID & 0x000000FF) >> 0);
	gTxCommand[CMD_OFFSET_DEVICE_ID + 1] = (uint8_t)((SPL_DEVICE_ID & 0x0000FF00) >> 8);
	gTxCommand[CMD_OFFSET_DEVICE_ID + 2] = (uint8_t)((SPL_DEVICE_ID & 0x00FF0000) >> 16);
	gTxCommand[CMD_OFFSET_DEVICE_ID + 3] = (uint8_t)((SPL_DEVICE_ID & 0xFF000000) >> 24);

	// >>> Interrupts
	// On boot interrupt flags are undefined; No interrupt handling is required
	INTRQ = 0b00000000;
	INTEN = 0b00000000;

	// >>>  Code state
	gState = SPL_STATE_IDLE;
	gRedLedValue = 0;
	gGreenLedValue = 0;
	gBlueLedValue = 0;
	gPwmCounter = 0;

	WAIT_STARTUP;

	for(;;) { spl_step(); }
}

unsigned char _sdcc_external_startup(void) {
	#if F_CPU == 8000000
    	PDK_USE_8MHZ_IHRC_SYSCLOCK();
	#else
		#error "Invalid F_CPU"
	#endif

  	return 0; 
}