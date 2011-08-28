/*
Copyright 2011 sapphire zhao <sapphire.zhao@gmail.com>

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

#ifndef MONKEY_H
#define MONKEY_H

#include <stdint.h>

#define FLAG_NKRO     1
#define FLAG_FN_LED   2
#define FLAG_FN_LOCK  4

/* Vender-specific control requests. */
typedef enum
{
	KURQ_AIKON_SET_KEY_MIN = 0x0,
	KURQ_AIKON_SET_KEY_MAX = 0xA8,

  /* Aikon requests */
  KURQ_AIKON_READ_MATRIX_NORMAL = 0xc9,   /* buffer[0:A8] */
  KURQ_AIKON_READ_MATRIX_FN = 0xca,
  KURQ_AIKON_READ_MATRIX_NUMLOCK = 0xcb,
	KURQ_AIKON_WRITE_TO_EEPROM = 0xcc,
  KURQ_AIKON_GET_LAST_KEY_INFO = 0xcd,    /* buffer[0:4] */
  KURQ_AIKON_GET_ROWCOL = 0xce,           /* value */
	KURQ_AIKON_SET_ROW = 0xcf,              /* value */
  KURQ_AIKON_SET_COLUMN = 0xd0,           /* value */
  KURQ_AIKON_GET_FLAGS = 0xd1,            /* buffer[0], bit 0: NK, bit 1: scrolllock led->fn led, bit 2: fn lock */
  KURQ_AIKON_SET_FLAGS = 0xd2,            /* value */

	KURQ_RESET = 0xff,
	
} KURequest;

typedef enum
{
	layer_normal,
	layer_fn1,
	layer_numlock,
	layer_fn2,
	layer_max,
	layer_keys = 200,
	layer_aikon_keys = 169,
} monkey_layer;

typedef struct
{
  uint8_t flags;
  uint8_t row;
  uint8_t column;
  uint8_t matrix[layer_max][layer_keys];
} monkeyconf_t;

extern monkeyconf_t monkey_config;
extern uint8_t last_row, last_column;
extern bool save_monkey_config;

#endif
