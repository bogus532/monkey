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
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "usbdrv.h"
#include "oddebug.h"
#include "vusb.h"
#include "keyboard.h"
#include "timer.h"
#include "uart.h"
#include "suart.h"
#include "debug.h"

#ifdef USB_COUNT_SOF
bool suspended = false;
#endif

/* This is from main.c of USBaspLoader */
static void initForUsbConnectivity(void)
{
    uint8_t i = 0;

    usbInit();
    /* enforce USB re-enumerate: */
    usbDeviceDisconnect();  /* do this while interrupts are disabled */
    while(--i){         /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();
    sei();
}

#define CPU_PRESCALE(n)    (CLKPR = 0x80, CLKPR = (n))
int main(void)
{
    CPU_PRESCALE(0);
    uart_init(115200);
    debug_enable = true;
    print_enable = true;
    debug("keyboard_init()\n");
    keyboard_init();

    debug("initForUsbConnectivity()\n");
    initForUsbConnectivity();



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


#ifdef USB_COUNT_SOF
    //bool suspended = false;
    uint16_t last_timer = timer_read();
#endif
    debug("main loop\n");
    host_set_driver(vusb_driver());
    while (1) {
#ifdef USB_COUNT_SOF
        if (usbSofCount != 0) {
            suspended = false;
            usbSofCount = 0;
            last_timer = timer_read();
        } else {
            // Suspend when no SOF in 3ms-10ms(7.1.7.4 Suspending of USB1.1)
            if (timer_elapsed(last_timer) > 5) {
                suspended = true;
/*
                uart_putchar('S');
                _delay_ms(1);
                cli();
                set_sleep_mode(SLEEP_MODE_PWR_DOWN);
                sleep_enable();
                sleep_bod_disable();
                sei();
                sleep_cpu();
                sleep_disable();
                _delay_ms(10);
                uart_putchar('W');
*/
            }
        }
#endif
        if (!suspended)
            usbPoll();
        keyboard_proc();
        if (!suspended)
            vusb_transfer_keyboard();

        // Send to Bluetoot module WT12
        if (suspended) {
            if (uart_available()) {
                uint8_t c;
                c = uart_getchar();
                xmit(c);
            }
        }
    }
}

ISR(PCINT1_vect, ISR_NOBLOCK)
{
    if ((PINC&(1<<5)))
        return;
    if (suspended)
        uart_putchar(recv());
}
