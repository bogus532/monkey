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

#include "platform.h"
#include "keyboard.h"
#include "host.h"
#include "matrix.h"
#include "led.h"
#include "usb_keycodes.h"


static uint8_t last_leds = 0;
uint8_t last_row, last_column;

void keyboard_init(void)
{
    timer_init();
    matrix_init();
}

void keyboard_proc(void)
{
    uint8_t fn_bits = 0;
#ifdef USB_EXTRA_ENABLE
    uint16_t consumer_code = 0;
#endif

    matrix_scan();

    if (matrix_is_modified()) {
			if (matrix_has_ghost()) {
        return;
			}

			host_swap_keyboard_report();
			host_clear_keyboard_report();
			for (int row = 0; row < matrix_rows(); row++) {
        for (int col = 0; col < matrix_cols(); col++) {
					if (!matrix_is_on(row, col)) continue;
					last_row = row;
					last_column = col;

					uint8_t code = keymap_get_keycode(0, row, col);
					if (code == KB_NO) {
						// do nothing
					} else if (IS_MOD(code)) {
						host_add_mod_bit(MOD_BIT(code));
					} else if (IS_FN(code)) {
						fn_bits |= FN_BIT(code);
					}	else if (IS_KEY(code)) {
						host_add_key(code);
					}
        }
			}

			host_send_keyboard_report();
    }

    if (last_leds != host_keyboard_leds()) {
			keyboard_set_leds(host_keyboard_leds());
			last_leds = host_keyboard_leds();
    }
}

void keyboard_set_leds(uint8_t leds)
{
	led_set(leds);
}
