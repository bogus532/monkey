/*
Copyright 2011 Sapphire Zhao <sapphire.zhao@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#if (MCU != cortex-m3)
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#endif

void timer_init(void);
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);
uint16_t tick_count(void);
void tick_isr(void);
int8_t sendchar(uint8_t c);

#if MCU == cortex-m3
#  define PROGMEM
#  define PSTR(s) (s)
#  define rom_byte(p) (*(p))
#  define rom_str(p) (p)
#else
#  define rom_byte(p) pgm_read_byte(p)
#  define rom_str(p) PSTR(p)
#endif

#endif
