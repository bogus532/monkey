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

#ifndef HOST_H
#define HOST_H

#include <stdint.h>


/* report id */
#define REPORT_ID_MOUSE     1
#define REPORT_ID_SYSTEM    2
#define REPORT_ID_CONSUMER  3

/* keyboard Modifiers in boot protocol report */
#define BIT_LCTRL   (1<<0)
#define BIT_LSHIFT  (1<<1)
#define BIT_LALT    (1<<2)
#define BIT_LGUI    (1<<3)
#define BIT_RCTRL   (1<<4)
#define BIT_RSHIFT  (1<<5)
#define BIT_RALT    (1<<6)
#define BIT_RGUI    (1<<7)
#define BIT_LCTL BIT_LCTRL
#define BIT_RCTL BIT_RCTRL
#define BIT_LSFT BIT_LSHIFT
#define BIT_RSFT BIT_RSHIFT

/* mouse buttons */
#define MOUSE_BTN1 (1<<0)
#define MOUSE_BTN2 (1<<1)
#define MOUSE_BTN3 (1<<2)
#define MOUSE_BTN4 (1<<3)
#define MOUSE_BTN5 (1<<4)

// Consumer Page(0x0C)
// following are supported by Windows: http://msdn.microsoft.com/en-us/windows/hardware/gg463372.aspx
#define AUDIO_MUTE              0x00E2
#define AUDIO_VOL_UP            0x00E9
#define AUDIO_VOL_DOWN          0x00EA
#define TRANSPORT_NEXT_TRACK    0x00B5
#define TRANSPORT_PREV_TRACK    0x00B6
#define TRANSPORT_STOP          0x00B7
#define TRANSPORT_PLAY_PAUSE    0x00CD
#define AL_CC_CONFIG            0x0183
#define AL_EMAIL                0x018A
#define AL_CALCULATOR           0x0192
#define AL_LOCAL_BROWSER        0x0194
#define AC_SEARCH               0x0221
#define AC_HOME                 0x0223
#define AC_BACK                 0x0224
#define AC_FORWARD              0x0225
#define AC_STOP                 0x0226
#define AC_REFRESH              0x0227
#define AC_BOOKMARKS            0x022A
// supplement for Bluegiga iWRAP HID(not supported by Windows?)
#define AL_LOCK                 0x019E
#define TRANSPORT_RECORD        0x00B2
#define TRANSPORT_REWIND        0x00B4
#define TRANSPORT_EJECT         0x00B8
#define AC_MINIMIZE             0x0206

// Generic Desktop Page(0x01)
#define SYSTEM_POWER_DOWN       0x0081
#define SYSTEM_SLEEP            0x0082
#define SYSTEM_WAKE_UP          0x0083


#if defined(HOST_PJRC)
#   include "usb.h"
#   if defined(KBD2_REPORT_KEYS) && KBD2_REPORT_KEYS > KBD_REPORT_KEYS
#       define REPORT_KEYS KBD2_REPORT_KEYS
#   else
#       define REPORT_KEYS KBD_REPORT_KEYS
#   endif
#else
#   define REPORT_KEYS 6
#endif

typedef struct {
    uint8_t mods;
    uint8_t rserved;
    uint8_t keys[REPORT_KEYS];
} report_keyboard_t;

typedef struct {
    uint8_t report_id;
    uint8_t buttons;
    int8_t x;
    int8_t y;
    int8_t v;
    int8_t h;
} report_mouse_t;


#ifdef USB_NKRO_ENABLE
extern bool keyboard_nkro;
#endif

extern report_keyboard_t *keyboard_report;
extern report_keyboard_t *keyboard_report_prev;


uint8_t host_keyboard_leds(void);

/* keyboard report operations */
void host_add_key(uint8_t key);
void host_add_mod_bit(uint8_t mod);
void host_set_mods(uint8_t mods);
void host_add_code(uint8_t code);
void host_swap_keyboard_report(void);
void host_clear_keyboard_report(void);
uint8_t host_has_anykey(void);
uint8_t host_get_first_key(void);


void host_send_keyboard_report(void);
#if defined(MOUSEKEY_ENABLE) || defined(PS2_MOUSE_ENABLE)
void host_mouse_send(report_mouse_t *report);
#endif
#ifdef USB_EXTRA_ENABLE
void host_system_send(uint16_t data);
void host_consumer_send(uint16_t data);
#endif

#endif
