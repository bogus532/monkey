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
#include "platform.h"
#include "util.h"
#include "matrix.h"
#include "monkey.h"

#if (MATRIX_COLS > 16)
#   error "MATRIX_COLS must not exceed 16"
#endif
#if (MATRIX_ROWS > 255)
#   error "MATRIX_ROWS must not exceed 255"
#endif

static Columnstate_t *matrix;
static Columnstate_t *matrix_prev;
static Columnstate_t _matrix0[MATRIX_ROWS];
static Columnstate_t _matrix1[MATRIX_ROWS];
static uint8_t debouncing_count[MATRIX_ROWS*MATRIX_COLS];

#ifdef MATRIX_HAS_GHOST
static bool matrix_has_ghost_in_row(uint8_t row);
#endif

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
    matrixpin_init();

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
    matrixpin_clear_rows();
    matrixpin_select_row(i);
    matrix[i] = ~matrixpin_read_columns();
  }
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
	for(uint8_t r=0; r < MATRIX_ROWS-1; ++r) {
    /* no ghost keys present for this row if no more than a single column got
     * activated */
    Columnstate_t temp=matrix[r];
    if(!(temp&(Columnstate_t)(temp-1)))
      continue;

    /* search for equal column states, report ghost key condition if found */
    for(uint8_t s=r+1; s < MATRIX_ROWS; ++s)
      if(matrix[r] == matrix[s]) return true;
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
