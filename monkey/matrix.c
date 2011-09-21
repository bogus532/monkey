/*
Copyright 2011 Jun Wako <wakojun@gmail.com>

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

/*
 * scan matrix
 */
#include <stdint.h>
#include <stdbool.h>
#ifdef HOST_STM32
#else
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#endif
#include "print.h"
#include "util.h"
#include "matrix.h"
#include "monkey.h"

#if (MATRIX_COLS > 16)
#   error "MATRIX_COLS must not exceed 16"
#endif
#if (MATRIX_ROWS > 255)
#   error "MATRIX_ROWS must not exceed 255"
#endif


// matrix state buffer(1:on, 0:off)
#if (MATRIX_COLS <= 8)
typedef uint8_t Columnstate_t;
#define COLUMNSTATE_EMPTY     0xff
#else
typedef uint16_t Columnstate_t;
#define COLUMNSTATE_EMPTY     0xffff
#endif

#ifdef HOST_PJRC
// Ports for Teensy
#elif defined(HOST_VUSB)
// Ports for V-USB
#define ROWS_PORT1  PORTA
#define ROWS_DDR1   DDRA

#define ROWS_PORT2  PORTC
#define ROWS_DDR2   DDRC

#define ROWS_PORT3  PORTD
#define ROWS_DDR3   DDRD
#define ROWS_ALL3   (_BV(PD6)|_BV(PD7))

#define COLS_PORT   PORTB
#define COLS_DDR    DDRB
#define COLS_PIN    PINB

inline
static void init_col_row(void)
{
  // initialize row and col
  unselect_rows();
  // Input with pull-up(DDR:0, PORT:1)
  COLS_DDR = 0x00;
  COLS_PORT = 0xFF;
}

inline
static uint8_t read_columns(void)
{
    return COLS_PIN;
}

inline
static void unselect_rows(void)
{
	ROWS_DDR1=0x00;
	ROWS_PORT1=0xff;
  ROWS_DDR2=0x00;
  ROWS_PORT2=0xff;
	ROWS_DDR3&=~ROWS_ALL3;
  ROWS_PORT3|=ROWS_ALL3;
}

inline
static void select_row(uint8_t row)
{
	if(row<8) {
		ROWS_DDR1=_BV(row);
		ROWS_PORT1=~_BV(row);
	} else if(row<16) {
		ROWS_DDR2=_BV(row&0x07);
		ROWS_PORT2=~_BV(row&0x07);
	} else {
		uint8_t temp=_BV(row&0x03);
		ROWS_DDR3=(ROWS_DDR3&~ROWS_ALL3)|temp;
		ROWS_PORT3=(ROWS_PORT3|ROWS_PORT3)&~temp;
	}
}

#else
#define eeprom_read_block
#define _delay_us(us)
#endif

static Columnstate_t *matrix;
static Columnstate_t *matrix_prev;
static Columnstate_t _matrix0[MATRIX_ROWS];
static Columnstate_t _matrix1[MATRIX_ROWS];

#ifdef MATRIX_HAS_GHOST
static bool matrix_has_ghost_in_row(uint8_t row);
#endif
static void init_col_row(void);
static Columnstate_t read_columns(void);
static void unselect_rows(void);
static void select_row(uint8_t row);

inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

void matrix_init(void)
{
		eeprom_read_block(&monkey_config, 0, sizeof(monkeyconf_t));
		if(monkey_config.row <=0 || monkey_config.row > MATRIX_ROWS)
			monkey_config.row = MATRIX_ROWS;
		if(monkey_config.column <=0 || monkey_config.column > MATRIX_COLS)
			monkey_config.column = MATRIX_COLS;

    init_col_row();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) _matrix0[i] = COLUMNSTATE_EMPTY;
    for (uint8_t i=0; i < MATRIX_ROWS; i++) _matrix1[i] = COLUMNSTATE_EMPTY;
    matrix = _matrix0;
    matrix_prev = _matrix1;
}

uint8_t matrix_scan(void)
{
    Columnstate_t *tmp;

    tmp = matrix_prev;
    matrix_prev = matrix;
    matrix = tmp;

    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        unselect_rows();
        select_row(i);
        _delay_us(30);  // without this wait read unstable value.
        matrix[i] = ~read_columns();
    }
    unselect_rows();
    return 1;
}

bool matrix_is_modified(void)
{
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        if (matrix[i] != matrix_prev[i])
            return true;
    }
    return false;
}

inline
bool matrix_has_ghost(void)
{
	for(uint8_t r=0; r < MATRIX_ROWS-1; ++r)
  {
    /* no ghost keys present for this row if no more than a single column got
     * activated */
    Columnstate_t temp=matrix[r];
    if(!(temp&(Columnstate_t)(temp-1))) continue;

    /* search for equal column states, report ghost key condition if found */
    for(uint8_t s=r+1; s < MATRIX_ROWS; ++s)
    {
      if(matrix[r] == matrix[s]) return true;
    }
  }

  return false;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & (1<<col));
}

inline
Columnstate_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 01234567\n");
    for (uint8_t row = 0; row < matrix_rows(); row++) {
        phex(row); print(": ");
#if (MATRIX_COLS <= 8)
        pbin_reverse(matrix_get_row(row));
#else
        pbin_reverse16(matrix_get_row(row));
#endif
#ifdef MATRIX_HAS_GHOST
        if (matrix_has_ghost_in_row(row)) {
            print(" <ghost");
        }
#endif
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
#if (MATRIX_COLS <= 8)
        count += bitpop(matrix[i]);
#else
        count += bitpop16(matrix[i]);
#endif
    }
    return count;
}
