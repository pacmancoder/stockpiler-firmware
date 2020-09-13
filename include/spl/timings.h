#ifndef _SPL_TIMINGS_H_
#define _SPL_TIMINGS_H_

#include <pdk-utils/delay.h>

#define WAIT_RECEIVE_FAILURE   _delay_us(200)
#define WAIT_BIT_1_HIGH        _delay_us(60)
#define WAIT_BIT_1_LOW         _delay_us(20)
#define WAIT_BIT_0_HIGH        _delay_us(20)
#define WAIT_BIT_0_LOW         _delay_us(60)
#define WAIT_START_BIT         _delay_us(80)
#define WAIT_STOP_BIT          _delay_us(80)
#define WAIT_BUS_OWNER_SWITCH  _delay_us(100)
#define WAIT_STARTUP           _delay_ms(50)
#define WAIT_ERROR             _delay_ms(1)


#define CLOCKS_10US (F_CPU / 100000)
#define CLOCKS_200US (CLOCKS_10US * 20)
#define CLOCKS_RECEIVE_FAILURE CLOCKS_200US


#endif // _SPL_TIMINGS_H_