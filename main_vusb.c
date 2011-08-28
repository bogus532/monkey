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
#include <avr/eeprom.h>
#include <util/delay.h>
#include "usbdrv.h"
#include "oddebug.h"
#include "host_vusb.h"
#include "keyboard.h"
#include "debug.h"
#include "monkey.h"

#if 0
#define DEBUGP_INIT() do { DDRC = 0xFF; } while (0)
#define DEBUGP(x) do { PORTC = x; } while (0)
#else
#define DEBUGP_INIT()
#define DEBUGP(x)
#endif

monkeyconf_t monkey_config;
bool save_monkey_config;

int main(void)
{
    DEBUGP_INIT();
    wdt_enable(WDTO_1S);
    odDebugInit();
    usbInit();

    /* enforce re-enumeration, do this while interrupts are disabled! */
    usbDeviceDisconnect();
    uint8_t i = 0;
    /* fake USB disconnect for > 250 ms */
    while(--i){
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();

    keyboard_init();
		save_monkey_config = false;
				
    sei();
    while (1) {
        DEBUGP(0x1);
        wdt_reset();
        usbPoll();
        DEBUGP(0x2);
        keyboard_proc();
        DEBUGP(0x3);
        host_vusb_keyboard_send();
				if (save_monkey_config) {
					//debug("save conf begin\n");
					wdt_disable();
					eeprom_write_block(&monkey_config, 0, sizeof(monkeyconf_t));
					wdt_reset();
					wdt_enable(WDTO_1S);
					//debug("save conf end\n");
					save_monkey_config = false;
				}
    }
}
