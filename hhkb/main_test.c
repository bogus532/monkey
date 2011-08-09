/* Name: main.c
 * Project: hid-mouse, a very simple HID example
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-07
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id: main.c 790 2010-05-30 21:00:26Z cs $
 */
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "uart.h"
#include "suart.h"


#define BAUD_RATE 115200
// write a string to the uart
#define uart_print(s) uart_print_P(PSTR(s))
void uart_print_P(const char *str)
{
        char c;
        while (1) {
                c = pgm_read_byte(str++);
                if (!c) break;
                uart_putchar(c);
        }
}


int main(void)
{
    CLKPR = (1<<CLKPCE), CLKPR = 0x00;  // clock prescaler
    uart_init(BAUD_RATE);


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


    sei();
    uart_print("start\n");
    while (1) {
        if (uart_available()) {
            uint8_t c;
            c = uart_getchar();
            //uart_putchar(c);
            xmit(c);
        }
    }
}

ISR(PCINT1_vect)
{
    if ((PINC&(1<<5)))
        return;
    uart_putchar(recv());
}
