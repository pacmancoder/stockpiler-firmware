/*
  delay.h: Factory to pull in appropriate device/xxx.h (and other common) include files.

  Copyright (C) 2020  serisman  <github@serisman.com>
  Copyright (C) 2019-2020  freepdk  https://free-pdk.github.io

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this library. If not, see <http://www.gnu.org/licenses/>.

  As a special exception, if you link this library with other files,
  some of which are compiled with SDCC, to produce an executable,
  this library does not by itself cause the resulting executable to
  be covered by the GNU General Public License. This exception does
  not however invalidate any other reasons why the executable file
  might be covered by the GNU General Public License.
*/

#ifndef __PDK_DELAY_H__
#define __PDK_DELAY_H__

#if !defined(F_CPU)
  #error "F_CPU is not defined!"
#endif

#include <stdint.h>

#if !defined(MAX)
  #define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define NS_TO_CYCLES(ns)        ((ns)/(1000000000L/F_CPU))
#define US_TO_CYCLES(us)        (NS_TO_CYCLES(us*1000L))
#define MS_TO_CYCLES(ms)        (NS_TO_CYCLES(ms*1000000L))

#define LOOP_CTR(cycles,ovh,lp) (MAX(0,(cycles-ovh-lp)/lp)+1)
#define LOOP_CTR_8(cycles)      LOOP_CTR(cycles,7,3)
#define LOOP_CTR_16(cycles)     LOOP_CTR(cycles,9,8)
#define LOOP_CTR_32(cycles)     LOOP_CTR(cycles,13,12)

#define _delay_us(us)           \
	(LOOP_CTR_8(US_TO_CYCLES(us)) < 256L) ? \
	_delay_loop_8((uint8_t)LOOP_CTR_8(US_TO_CYCLES(us))) : \
	_delay_loop_16((uint16_t)LOOP_CTR_16(US_TO_CYCLES(us)))

#define _delay_ms(ms)           \
	(LOOP_CTR_16(MS_TO_CYCLES(ms)) < 65536L) ? \
	_delay_loop_16((uint16_t)LOOP_CTR_16(MS_TO_CYCLES(ms))) : \
	_delay_loop_32((uint32_t)LOOP_CTR_32(MS_TO_CYCLES(ms)))

// 3 cycles per loop, 7 cycles overhead
void _delay_loop_8(uint8_t loop_ctr);
// 8 cycles per loop, 9 cycles overhead
void _delay_loop_16(uint16_t loop_ctr);
// 12 cycles per loop, 13 cycles overhead
void _delay_loop_32(uint32_t loop_ctr);

#endif //__PDK_DELAY_H__
