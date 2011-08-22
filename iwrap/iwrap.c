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

/* host driver for Bulegiga iWRAP */
/* Bluegiga BT12
 * Connections
 *    Hardware UART       Software UART            BlueTooth
 * PC=====UART=======AVR=====SUART====iWRAP(BT12)-----------PC
 *
 * - Hardware UART for Debug Console to communicate iWRAP
 * - Software UART for iWRAP control to send keyboard/mouse data
 */

#include <stdint.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usb_keycodes.h"
#include "suart.h"
#include "uart.h"
#include "report.h"
#include "host_driver.h"
#include "iwrap.h"
#include "print.h"


/* iWRAP MUX mode utils. 3.10 HID raw mode(iWRAP_HID_Application_Note.pdf) */
#define MUX_HEADER(LINK, LENGTH) do { \
    xmit(0xbf);     /* SOF    */ \
    xmit(LINK);     /* Link   */ \
    xmit(0x00);     /* Flags  */ \
    xmit(LENGTH);   /* Length */ \
} while (0)
#define MUX_FOOTER(LINK) xmit(LINK^0xff)


/* iWRAP buffer */
#define MUX_BUF_SIZE 32
static char buf[MUX_BUF_SIZE];
static uint8_t snd_pos = 0;

#define MUX_RCV_BUF_SIZE 256
static char rcv_buf[MUX_RCV_BUF_SIZE];
static uint8_t rcv_head = 0;
static uint8_t rcv_tail = 0;


/* receive buffer */
static void rcv_enq(char c)
{
    //if (snd_pos)
        //return;
    uint8_t next = (rcv_head + 1) % MUX_RCV_BUF_SIZE;
    if (next != rcv_tail) {
        rcv_buf[rcv_head] = c;
        rcv_head = next;
    }
}

static char rcv_deq(void)
{
    char c = 0;
    //if (snd_pos)
        //return 0;
    if (rcv_head != rcv_tail) {
        c = rcv_buf[rcv_tail++];
        rcv_tail %= MUX_RCV_BUF_SIZE;
    }
    return c;
}

static char rcv_peek(void)
{
    if (rcv_head == rcv_tail)
        return 0;
    return rcv_buf[rcv_tail];
}

/* iWRAP response to */
//ISR(PCINT1_vect, ISR_NOBLOCK)
ISR(PCINT1_vect, ISR_BLOCK)
{
    if ((PINC&(1<<5)))
        return;

    static volatile uint8_t mux_state = 0xff;
    static volatile uint8_t mux_link = 0xff;
    uint8_t c = recv();
    switch (mux_state) {
        case 0xff: // SOF
            if (c == 0xbf)
                mux_state--;
            break;
        case 0xfe: // Link
            mux_state--;
            mux_link = c;
            break;
        case 0xfd: // Flags
            mux_state--;
            break;
        case 0xfc: // Length
            mux_state = c;
            break;
        case 0x00:
            mux_state = 0xff;
            mux_link = 0xff;
            break;
        default:
            if (mux_state--) {
                uart_putchar(c);
                rcv_enq(c);
            }
    }
}


/*------------------------------------------------------------------*
 * iWRAP communication
 *------------------------------------------------------------------*/
void iwrap_init(void)
{
    // reset iWRAP if in already MUX mode after AVR software-reset
    iwrap_mux_send("RESET");
    _delay_ms(3000);
    iwrap_send("\r\nSET CONTROL MUX 1\r\n");
    _delay_ms(500);
}

void iwrap_mux_send(const char *s)
{
    rcv_tail = rcv_head = 0;    // reset rcv_buf
    MUX_HEADER(0xff, strlen((char *)s));
    iwrap_send(s);
    MUX_FOOTER(0xff);
}

void iwrap_send(const char *s)
{
    while (*s)
        xmit(*s++);
}

/* send buffer */
void iwrap_buf_add(uint8_t c)
{
    // need space for '\0'
    if (snd_pos < MUX_BUF_SIZE-1)
        buf[snd_pos++] = c;
}

void iwrap_buf_del(void)
{
    if (snd_pos)
        snd_pos--;
}

void iwrap_buf_send(void)
{
    buf[snd_pos] = '\0';
    snd_pos = 0;
    iwrap_mux_send(buf);
}

void iwrap_call(void)
{
    iwrap_mux_send("SET BT PAIR");
    _delay_ms(500);

    char c;
    char s[16];
    char *p;
    uint8_t len;

/*
    while (rcv_peek() == '\r' || rcv_peek() == '\n') {
            rcv_deq();
    }
    phex(sizeof("SET BT PAIR "));
    while (( c = rcv_deq()))
        sendchar(c);
    //rcv_buf[rcv_head] = '\0';
    while (( c = rcv_deq()))
        sendchar(c);
*/
    len = sizeof("SET BT PAIR ") - 1;
    if (!strncmp(rcv_buf, "SET BT PAIR ", len)) {
        print("match!!!!\n");
        rcv_tail += len;
    }
    p = rcv_buf + len;

    strncpy(p-5, "CALL ", 5);
    strncpy(p+17, " 11 HID\0", 8);
    print_S(p-5);
/*
    while (( c = rcv_deq()))
        sendchar(c);
*/
}

void iwrap_sniff(void)
{
}

void iwrap_subrate(void)
{
}


/*------------------------------------------------------------------*
 * Host driver
 *------------------------------------------------------------------*/
static uint8_t keyboard_leds(void);
static void send_keyboard(report_keyboard_t *report);
static void send_mouse(report_mouse_t *report);
static void send_system(uint16_t data);
static void send_consumer(uint16_t data);

static host_driver_t driver = {
        keyboard_leds,
        send_keyboard,
        send_mouse,
        send_system,
        send_consumer
};

host_driver_t *iwrap_driver(void)
{
    return &driver;
}

static uint8_t keyboard_leds(void) {
    return 0;
}

static void send_keyboard(report_keyboard_t *report)
{
    MUX_HEADER(0x01, 0x0c);
    // HID raw mode header
    xmit(0x9f);
    xmit(0x0a); // Length
    xmit(0xa1); // keyboard report
    xmit(0x01);
    xmit(report->mods);
    xmit(0x00); // reserved byte(always 0)
    xmit(report->keys[0]);
    xmit(report->keys[1]);
    xmit(report->keys[2]);
    xmit(report->keys[3]);
    xmit(report->keys[4]);
    xmit(report->keys[5]);
    MUX_FOOTER(0x01);
}

static void send_mouse(report_mouse_t *report)
{
#if defined(MOUSEKEY_ENABLE) || defined(PS2_MOUSE_ENABLE)
    MUX_HEADER(0x01, 0x07);
    // HID raw mode header
    xmit(0x9f);
    xmit(0x05); // Length
    xmit(0xa1); // mouse report
    xmit(0x02);
    xmit(report->buttons);
    xmit(report->x);
    xmit(report->y);
    MUX_FOOTER(0x01);
#endif
}

static void send_system(uint16_t data)
{
    /* not supported */
}

static void send_consumer(uint16_t data)
{
#ifdef EXTRAKEY_ENABLE
    static uint16_t last_data = 0;
    uint8_t bits1 = 0;
    uint8_t bits2 = 0;
    uint8_t bits3 = 0;

    if (data == last_data) return;
    last_data = data;

    // 3.10 HID raw mode(iWRAP_HID_Application_Note.pdf)
    switch (data) {
        case AUDIO_VOL_UP:
            bits1 = 0x01;
            break;
        case AUDIO_VOL_DOWN:
            bits1 = 0x02;
            break;
        case AUDIO_MUTE:
            bits1 = 0x04;
            break;
        case TRANSPORT_PLAY_PAUSE:
            bits1 = 0x08;
            break;
        case TRANSPORT_NEXT_TRACK:
            bits1 = 0x10;
            break;
        case TRANSPORT_PREV_TRACK:
            bits1 = 0x20;
            break;
        case TRANSPORT_STOP:
            bits1 = 0x40;
            break;
        case TRANSPORT_EJECT:
            bits1 = 0x80;
            break;
        case AL_EMAIL:
            bits2 = 0x01;
            break;
        case AC_SEARCH:
            bits2 = 0x02;
            break;
        case AC_BOOKMARKS:
            bits2 = 0x04;
            break;
        case AC_HOME:
            bits2 = 0x08;
            break;
        case AC_BACK:
            bits2 = 0x10;
            break;
        case AC_FORWARD:
            bits2 = 0x20;
            break;
        case AC_STOP:
            bits2 = 0x40;
            break;
        case AC_REFRESH:
            bits2 = 0x80;
            break;
        case AL_CC_CONFIG:
            bits3 = 0x01;
            break;
        case AL_CALCULATOR:
            bits3 = 0x04;
            break;
        case AL_LOCK:
            bits3 = 0x08;
            break;
        case AL_LOCAL_BROWSER:
            bits3 = 0x10;
            break;
        case AC_MINIMIZE:
            bits3 = 0x20;
            break;
        case TRANSPORT_RECORD:
            bits3 = 0x40;
            break;
        case TRANSPORT_REWIND:
            bits3 = 0x80;
            break;
    }

    MUX_HEADER(0x01, 0x07);
    xmit(0x9f);
    xmit(0x05); // Length
    xmit(0xa1); // consumer report
    xmit(0x03);
    xmit(bits1);
    xmit(bits2);
    xmit(bits3);
    MUX_FOOTER(0x01);
#endif
}
