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

#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>
#include <stdbool.h>

// matrix state buffer(1:on, 0:off)
#if (MATRIX_COLS <= 8)
typedef uint8_t Columnstate_t;
#define COLUMNSTATE_EMPTY     0xff
#else
typedef uint16_t Columnstate_t;
#define COLUMNSTATE_EMPTY     0xffff
#endif

/* number of matrix rows */
uint8_t matrix_rows(void);

/* number of matrix columns */
uint8_t matrix_cols(void);

/* intialize matrix for scaning. should be called once. */
void matrix_init(void);

/* scan all key states on matrix */
uint8_t matrix_scan(void);

/* whether modified from previous scan. used after matrix_scan. */
bool matrix_is_modified(void);

/* whether ghosting occur on matrix. */
bool matrix_has_ghost(void);

/* whether a swtich is on */
bool matrix_is_on(uint8_t row, uint8_t col);

/* matrix state on row */
Columnstate_t matrix_get_row(uint8_t row);

/* count keys pressed */
uint8_t matrix_key_count(void);

/* print matrix for debug */
void matrix_print(void);

/* matrix io pin func */
void matrixpin_init(void);
void matrixpin_clear_rows(void);
void matrixpin_select_row(uint8_t row);
Columnstate_t matrixpin_read_columns(void);

#endif
