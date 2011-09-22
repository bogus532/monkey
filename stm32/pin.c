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

#include <stdint.h>
#include "matrix.h"
#include "stm32f10x.h"

void matrixpin_init(void)
{
  int idx;
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

  /* USB_DISCONNECT used as USB pull-up */
  for (idx=0; idx<8; idx++) {
    GPIO_InitStructure.GPIO_Pin = idx;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);  
  }
  for (idx=0; idx<16; idx++) {
    GPIO_InitStructure.GPIO_Pin = idx;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
  }
}

void matrixpin_clear_rows(void)
{
  GPIO_Write(GPIOD, 0);
}

void matrixpin_select_row(uint8_t row)
{
  if (row < 16)
    GPIO_SetBits(GPIOD, row);
  else
    GPIO_SetBits(GPIOE, row-16);
}

Columnstate_t matrixpin_read_columns(void)
{
  return (Columnstate_t)GPIO_ReadInputData(GPIOC);
}
