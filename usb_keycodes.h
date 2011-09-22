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
 * Key codes from HID Keyboard/Keypad Page
 * http://www.usb.org/developers/devclass_docs/Hut1_12.pdf
 */
#ifndef USB_KEYCODES_H
#define USB_KEYCODES_H


#define IS_ERROR(code)           (KB_ROLL_OVER <= (code) && (code) <= KB_UNDEFINED)
#define IS_KEY(code)             (KB_A         <= (code) && (code) <= KB_EXSEL)
#define IS_MOD(code)             (KB_LCTRL     <= (code) && (code) <= KB_RGUI)
#define IS_FN(code)              (KB_FN0       <= (code) && (code) <= KB_FN7)
#define IS_MOUSEKEY(code)        (KB_MS_UP     <= (code) && (code) <= KB_MS_WH_RIGHT)
#define IS_MOUSEKEY_MOVE(code)   (KB_MS_UP     <= (code) && (code) <= KB_MS_RIGHT)
#define IS_MOUSEKEY_BUTTON(code) (KB_MS_BTN1   <= (code) && (code) <= KB_MS_BTN5)
#define IS_MOUSEKEY_WHEEL(code)  (KB_MS_WH_UP  <= (code) && (code) <= KB_MS_WH_RIGHT)

#define MOD_BIT(code) (1<<((code) & 0x07))
#define FN_BIT(code)  (1<<((code) - KB_FN0))

/* Special keycode */
enum special_keycodes {
    /* System Control */
    KB_SYSTEM_POWER = 0xB0,
    KB_SYSTEM_SLEEP,
    KB_SYSTEM_WAKE,

    /* Consumer Page */
    KB_AUDIO_MUTE,
    KB_AUDIO_VOL_UP,
    KB_AUDIO_VOL_DOWN,
    KB_MEDIA_NEXT_TRACK,
    KB_MEDIA_PREV_TRACK,
    KB_MEDIA_STOP,
    KB_MEDIA_PLAY_PAUSE,
    KB_MEDIA_SELECT,
    KB_MAIL,
    KB_CALCULATOR,
    KB_MY_COMPUTER,
    KB_WWW_SEARCH,
    KB_WWW_HOME,
    KB_WWW_BACK,        /* 0xC0 */
    KB_WWW_FORWARD,
    KB_WWW_STOP,
    KB_WWW_REFRESH,
    KB_WWW_FAVORITES,

    /* reserve 0xE0-E7 for Modifiers */

    /* Layer Switching */
    KB_FN0 = 0xE8,
    KB_FN1,
    KB_FN2,
    KB_FN3,
    KB_FN4,
    KB_FN5,
    KB_FN6,
    KB_FN7,

    /* Mousekey */
    KB_MS_UP = 0xF0,
    KB_MS_DOWN,
    KB_MS_LEFT,
    KB_MS_RIGHT,
    KB_MS_BTN1,
    KB_MS_BTN2,
    KB_MS_BTN3,
    KB_MS_BTN4,
    KB_MS_BTN5,
    /* Mousekey wheel */
    KB_MS_WH_UP,
    KB_MS_WH_DOWN,
    KB_MS_WH_LEFT,
    KB_MS_WH_RIGHT,
};

enum keycodes {
    KB_NO = 0,
    KB_ROLL_OVER,
    KB_POST_FAIL,
    KB_UNDEFINED,
    KB_A,
    KB_B,
    KB_C,
    KB_D,
    KB_E,
    KB_F,
    KB_G,
    KB_H,
    KB_I,
    KB_J,
    KB_K,
    KB_L,
    KB_M,               /* 0x10 */
    KB_N,
    KB_O,
    KB_P,
    KB_Q,
    KB_R,
    KB_S,
    KB_T,
    KB_U,
    KB_V,
    KB_W,
    KB_X,
    KB_Y,
    KB_Z,
    KB_1,
    KB_2,
    KB_3,               /* 0x20 */
    KB_4,
    KB_5,
    KB_6,
    KB_7,
    KB_8,
    KB_9,
    KB_0,
    KB_ENTER,
    KB_ESCAPE,
    KB_BSPACE,
    KB_TAB,
    KB_SPACE,
    KB_MINUS,
    KB_EQUAL,
    KB_LBRACKET,
    KB_RBRACKET,        /* 0x30 */
    KB_BSLASH,          /* \ (and |) */
    KB_NONUS_HASH,      /* Non-US # and ~ */
    KB_SCOLON,          /* ; (and :) */
    KB_QUOTE,           /* ' and " */
    KB_GRAVE,           /* Grave accent and tilde */
    KB_COMMA,           /* , and < */
    KB_DOT,             /* . and > */
    KB_SLASH,           /* / and ? */
    KB_CAPSLOCK,
    KB_F1,
    KB_F2,
    KB_F3,
    KB_F4,
    KB_F5,
    KB_F6,
    KB_F7,              /* 0x40 */
    KB_F8,
    KB_F9,
    KB_F10,
    KB_F11,
    KB_F12,
    KB_PSCREEN,
    KB_SCKLOCK,
    KB_BREAK,
    KB_INSERT,
    KB_HOME,
    KB_PGUP,
    KB_DELETE,
    KB_END,
    KB_PGDOWN,
    KB_RIGHT,
    KB_LEFT,            /* 0x50 */
    KB_DOWN,
    KB_UP,
    KB_NUMLOCK,
    KB_KP_SLASH,
    KB_KP_ASTERISK,
    KB_KP_MINUS,
    KB_KP_PLUS,
    KB_KP_ENTER,
    KB_KP_1,
    KB_KP_2,
    KB_KP_3,
    KB_KP_4,
    KB_KP_5,
    KB_KP_6,
    KB_KP_7,
    KB_KP_8,            /* 0x60 */
    KB_KP_9,
    KB_KP_0,
    KB_KP_DOT,
    KB_NONUS_BSLASH,    /* Non-US \ and | */
    KB_APPLICATION,
    KB_POWER,
    KB_KP_EQUAL,
    KB_F13,
    KB_F14,
    KB_F15,
    KB_F16,
    KB_F17,
    KB_F18,
    KB_F19,
    KB_F20,
    KB_F21,             /* 0x70 */
    KB_F22,
    KB_F23,
    KB_F24,
    KB_EXECUTE,
    KB_HELP,
    KB_MENU,
    KB_SELECT,
    KB_STOP,
    KB_AGAIN,
    KB_UNDO,
    KB_CUT,
    KB_COPY,
    KB_PASTE,
    KB_FIND,
    KB__MUTE,
    KB__VOLUP,          /* 0x80 */
    KB__VOLDOWN,
    KB_LOCKING_CAPS,    /* locking Caps Lock */
    KB_LOCKING_NUM,     /* locking Num Lock */
    KB_LOCKING_SCROLL,  /* locking Scroll Lock */
    KB_KP_COMMA,
    KB_KP_EQUAL_AS400,  /* equal sign on AS/400 */
    KB_INT1,
    KB_INT2,
    KB_INT3,
    KB_INT4,
    KB_INT5,
    KB_INT6,
    KB_INT7,
    KB_INT8,
    KB_INT9,
    KB_LANG1,           /* 0x90 */
    KB_LANG2,
    KB_LANG3,
    KB_LANG4,
    KB_LANG5,
    KB_LANG6,
    KB_LANG7,
    KB_LANG8,
    KB_LANG9,
    KB_ALT_ERASE,
    KB_SYSREQ,
    KB_CANCEL,
    KB_CLEAR,
    KB_PRIOR,
    KB_RETURN,
    KB_SEPARATOR,
    KB_OUT,             /* 0xA0 */
    KB_OPER,
    KB_CLEAR_AGAIN,
    KB_CRSEL,
    KB_EXSEL,

    /* NOTE: uses 0xB0-DF for special keycodes */
    KB_KP_00 = 0xB0,
    KB_KP_000,
    KB_THOUSANDS_SEPARATOR,
    KB_DECIMAL_SEPARATOR,
    KB_CURRENCY_UNIT,
    KB_CURRENCY_SUB_UNIT,
    KB_KP_LPAREN,
    KB_KP_RPAREN,
    KB_KP_LCBRACKET,    /* { */
    KB_KP_RCBRACKET,    /* } */
    KB_KP_TAB,
    KB_KP_BSPACE,
    KB_KP_A,
    KB_KP_B,
    KB_KP_C,
    KB_KP_D,
    KB_KP_E,            /* 0xC0 */
    KB_KP_F,
    KB_KP_XOR,
    KB_KP_HAT,
    KB_KP_PERC,
    KB_KP_LT,
    KB_KP_GT,
    KB_KP_AND,
    KB_KP_LAZYAND,
    KB_KP_OR,
    KB_KP_LAZYOR,
    KB_KP_COLON,
    KB_KP_HASH,
    KB_KP_SPACE,
    KB_KP_ATMARK,
    KB_KP_EXCLAMATION,
    KB_KP_MEM_STORE,    /* 0xD0 */
    KB_KP_MEM_RECALL,
    KB_KP_MEM_CLEAR,
    KB_KP_MEM_ADD,
    KB_KP_MEM_SUB,
    KB_KP_MEM_MUL,
    KB_KP_MEM_DIV,
    KB_KP_PLUS_MINUS,
    KB_KP_CLEAR,
    KB_KP_CLEAR_ENTRY,
    KB_KP_BINARY,
    KB_KP_OCTAL,
    KB_KP_DECIMAL,
    KB_KP_HEXADECIMAL,

    /* Modifiers */
    KB_LCTRL = 0xE0,
    KB_LSHIFT,
    KB_LALT,
    KB_LGUI,
    KB_RCTRL,
    KB_RSHIFT,
    KB_RALT,
    KB_RGUI,

    /* NOTE: uses 0xE8-FF for special keycodes */
};

#endif /* USB_KEYCODES_H */
