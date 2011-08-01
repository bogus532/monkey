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
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usbdrv.h"
#include "oddebug.h"
#include "host_vusb.h"
#include "keyboard.h"
#include "uart.h"
#include "debug.h"


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

    debug("main loop\n");
    while (1) {
        usbPoll();
        keyboard_proc();
        host_vusb_keyboard_send();
    }
}
