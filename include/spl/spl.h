#ifndef __SPL_H__
#define __SPL_H__

#include <stdint.h>
#include <stdbool.h>
#include <pdk/device.h>


#define FEATURE_DEBUG_MODE

#define SPL_COMMAND_SIZE 10
#define SPL_LED_COUNT 3
#define SPL_DEVICE_ID_SIZE 4

#define SPL_PIN_LED_R 4
#define SPL_PIN_LED_G 3
#define SPL_PIN_LED_B 0

#define SPL_PIN_BUTTON 6
#define SPL_PIN_ALT_ADDRESS 7
#define SPL_PIN_BUS 5

typedef enum {
	SPL_STATE_IDLE,
	SPL_STATE_BUS_ACTIVATED,
	SPL_STATE_READ_COMMAND,
	SPL_STATE_BUTTON_PRESSED,
	SPL_STATE_FAILURE,
	SPL_STATE_EXECUTE_COMMAND,
} SPL_STATE;

typedef enum {
	SPL_CMD_ACK = 0x00,
	SPL_CMD_PING = 0x01,
	SPL_CMD_SLEEP = 0x02,
	SPL_CMD_BUTTON_PRESSED = 0x03,
	SPL_CMD_RGB_LED = 0x04,
} SPL_CMD;

typedef enum {
	SPL_FLAG_RGB_LED_SET_NEXT_COLOR = 0x01,
	SPL_FLAG_RGB_LED_UPDATE = 0x02,
} SPL_FLAG_RGB;

// General offsets
#define CMD_OFFSET_ID 0
#define CMD_OFFSET_DEVICE_ID 1
#define CMD_OFFSET_PAYLOAD 5
#define CMD_OFFSET_CHECKSUM 9
// Command-specific offsets
#define CMD_OFFSET_RGB_LED_FLAGS (CMD_OFFSET_PAYLOAD + 0)
#define CMD_OFFSET_RGB_LED_RED (CMD_OFFSET_PAYLOAD + 1)
#define CMD_OFFSET_RGB_LED_GREEN (CMD_OFFSET_PAYLOAD + 2)
#define CMD_OFFSET_RGB_LED_BLUE (CMD_OFFSET_PAYLOAD + 3)

typedef enum {
	SPL_ERR_HIGH_TIMEOUT = 1,
	SPL_ERR_LOW_TIMEOUT = 2,
	SPL_ERR_CHECKSUM = 3,
} SPL_ERR;

extern uint8_t gTxCommand[SPL_COMMAND_SIZE];
extern uint8_t gRxCommand[SPL_COMMAND_SIZE];

extern bool gIdleTimerArmed;
extern bool gSleepPending;

extern uint8_t gState; // SPL_STATE
extern uint8_t gRedLedValue;
extern uint8_t gGreenLedValue;
extern uint8_t gBlueLedValue;

extern uint8_t gPwmCounter;

void spl_initialize(void);

#endif // __SPL_H__