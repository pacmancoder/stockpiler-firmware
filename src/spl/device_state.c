#include <spl/device_state.h>

uint8_t gTxCommand[SPL_COMMAND_SIZE];
uint8_t gRxCommand[SPL_COMMAND_SIZE];

bool gIdleTimerArmed;
bool gSleepPending;
uint8_t gState; // SPL_STATE
uint8_t gErrorCause;

uint8_t gRedLedValue;
uint8_t gGreenLedValue;
uint8_t gBlueLedValue;
uint8_t gRedLedShadow;
uint8_t gGreenLedShadow;
uint8_t gBlueLedShadow;
uint8_t gPwmCounter;

uint8_t* gMemPtr;
uint8_t gCurrentChecksum;