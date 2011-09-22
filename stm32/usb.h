#ifndef __USB_H
#define __USB_H

typedef enum _RESUME_STATE
{
  RESUME_EXTERNAL,
  RESUME_INTERNAL,
  RESUME_LATER,
  RESUME_WAIT,
  RESUME_START,
  RESUME_ON,
  RESUME_OFF,
  RESUME_ESOF
} RESUME_STATE;

typedef enum _DEVICE_STATE
{
  UNCONNECTED,
  ATTACHED,
  POWERED,
  SUSPENDED,
  ADDRESSED,
  CONFIGURED
} DEVICE_STATE;

typedef enum _HID_REQUESTS
{
  GET_REPORT = 1,
  GET_IDLE,
  GET_PROTOCOL,

  SET_REPORT = 9,
  SET_IDLE,
  SET_PROTOCOL
} HID_REQUESTS;

struct descriptor_list_struct {
	uint16_t	wValue;     // descriptor type
	uint16_t	wIndex;
	const uint8_t	*addr;
	uint8_t		length;
};

extern volatile uint8_t usb_keyboard_leds;

#define ENDPOINT0_SIZE		64
#ifndef VENDOR_ID
#   define VENDOR_ID		0xFEED
#endif

#ifndef PRODUCT_ID
#   define PRODUCT_ID		0xBABE
#endif

enum MONKEY_INTERFACE {
  KBD_INTERFACE = 0,
#ifdef USB_NKRO_ENABLE
  KBD2_INTERFACE,
#endif
  DEBUG_INTERFACE,
};
enum MONKEY_ENDPOINT {
  KBD_ENDPOINT = 1,
#ifdef USB_NKRO_ENABLE
  KBD2_ENDPOINT,
#endif
  DEBUG_TX_ENDPOINT,
};

#define KBD_SIZE		      8
#define KBD_BUFFER		    EP_DOUBLE_BUFFER
#define KBD_REPORT_KEYS		(KBD_SIZE - 2)

// secondary keyboard
#ifdef USB_NKRO_ENABLE
#define KBD2_SIZE		      16
#define KBD2_BUFFER		    EP_DOUBLE_BUFFER
#define KBD2_REPORT_KEYS	(KBD2_SIZE - 1)
#endif

#define DEBUG_TX_SIZE		  32
#define DEBUG_TX_BUFFER		EP_DOUBLE_BUFFER

#endif
