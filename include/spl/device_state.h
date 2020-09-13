#include <spl/spl.h>

extern uint8_t gTxCommand[SPL_COMMAND_SIZE];
extern uint8_t gRxCommand[SPL_COMMAND_SIZE];

extern bool gIdleTimerArmed;
extern bool gSleepPending;
extern uint8_t gState; // SPL_STATE
extern uint8_t gErrorCause;

extern uint8_t gRedLedValue;
extern uint8_t gGreenLedValue;
extern uint8_t gBlueLedValue;
extern uint8_t gRedLedShadow;
extern uint8_t gGreenLedShadow;
extern uint8_t gBlueLedShadow;
extern uint8_t gPwmCounter;

extern uint8_t* gMemPtr;
extern uint8_t gCurrentChecksum;