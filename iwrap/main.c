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
#include <avr/interrupt.h>
#include <avr/io.h>
//#include <avr/wdt.h>
#include "wd.h"
#include <avr/sleep.h>
#include <util/delay.h>
#include "keyboard.h"
#include "matrix.h"
#include "host.h"
#include "iwrap.h"
#include "uart.h"
#include "suart.h"
#include "timer.h"
#include "debug.h"


static void sleep(void);


int main(void)
{
    static bool sleeping = false;
    static bool insomniac = true;   // TODO: should be false for power saving
    static uint16_t last_timer = 0;

    // Clock Prescaler: div1
    CLKPR = 0x80, CLKPR = 0;
    MCUSR = 0;
    WD_SET(WD_OFF);

    print_enable = true;
    debug_enable = false;

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
    //iwrap_call();
#endif

    last_timer = timer_read();
    while (true) {
        keyboard_proc();
        if (matrix_is_modified()) {
            sleeping = false;
        } else if (!sleeping && timer_elapsed(last_timer) > 4000) {
            sleeping = true;
            //uart_putchar('S');
        }

        // Send to Bluetoot module WT12
        static bool breaked = false;
        if (uart_available()) {
            uint8_t c;
            c = uart_getchar();
#ifdef MUX_MODE
            uart_putchar(c);
            switch (c) {
                case 0x00:
                    // break
                    print("break-");
                    breaked = true;
                    _delay_ms(1000);
                    sleeping = false;
                    break;
                case '\r':
                    uart_putchar('\n');
                    iwrap_buf_send();
                    break;
                case '\b':
                    iwrap_buf_del();
                    break;
                default:
                    if (breaked) {
                        print("\n");
                        switch (c) {
                            case 'r':
                                print("reset\n");
                                WD_AVR_RESET();
                                break;
                            case 'i':
                                insomniac = !insomniac;
                                if (insomniac)
                                    print("insomniac\n");
                                else
                                    print("not insomniac\n");
                                break;
                            case 'c':
                                print("iwrap_call()\n");
                                iwrap_call();
                                break;
                        }
                        breaked = false;
                    } else {
                        iwrap_buf_add(c);
                    }
                    break;
            }
#else
            xmit(c);
#endif
        }

        if (!insomniac && sleeping) {
            _delay_ms(1);
            sleep();
            last_timer = timer_read();
        }
    }
}

static void sleep(void)
{
    WD_SET(WD_IRQ, WDTO_15MS);
    //wdt_enable(WDTO_500MS);

    cli();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_bod_disable();
    sei();
    sleep_cpu();
    sleep_disable();

    WD_SET(WD_OFF);
    //wdt_disable();

    //uart_putchar('W');
}

ISR(WDT_vect)
{
    //uart_putchar('w');
}
