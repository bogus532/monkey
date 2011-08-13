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
#include <avr/io.h>
#include <util/delay.h>
#include "keyboard.h"
#include "host.h"
#include "iwrap.h"
#include "uart.h"
#include "suart.h"
#include "debug.h"

int main(void)
{
    // Clock Prescaler: div1
    CLKPR = 0x80, CLKPR = 0;

    print_enable = true;

    print("uart_init()\n");
    uart_init(115200);
    print("keyboard_init()\n");
    keyboard_init();

    // TODO: move to iWRAP/suart file
    print("suart init\n");
    // suart init
    // PC4: Tx Output IDLE(Hi)
    PORTC |= (1<<4);
    DDRC  |= (1<<4);
    // PC5: Rx Input(pull-up)
    PORTC |= (1<<5);
    DDRC  &= ~(1<<5);
    // suart receive interrut(PC5/PCINT13)
    PCMSK1 = 0b00100000;
    PCICR  = 0b00000010;

    host_set_driver(iwrap_driver());
#ifdef MUX_MODE
    print("iwrap_init()\n");
    iwrap_init();
    iwrap_call();
#endif
    while (true) {
        keyboard_proc();
        // Send to Bluetoot module WT12
        if (uart_available()) {
            uint8_t c;
            c = uart_getchar();
#ifdef MUX_MODE
            uart_putchar(c);
            switch (c) {
                case 0x00:
                    // break
                    print("break\n");
                    print("iwrap_init()\n");
                    iwrap_init();
                    iwrap_call();
                    break;
                case '\r':
                    iwrap_buf_send();
                    break;
                case '\b':
                    iwrap_buf_del();
                    break;
                default:
                    iwrap_buf_add(c);
                    break;
            }
#else
            xmit(c);
#endif
        }
    }
}
