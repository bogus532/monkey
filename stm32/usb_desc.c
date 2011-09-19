/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : usb_desc.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Descriptors for Custom HID Demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
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

#define KBD_SIZE		    8
#define KBD_BUFFER		    EP_DOUBLE_BUFFER
#define KBD_REPORT_KEYS		(KBD_SIZE - 2)

// secondary keyboard
#ifdef USB_NKRO_ENABLE
#define KBD2_SIZE		    16
#define KBD2_BUFFER		    EP_DOUBLE_BUFFER
#define KBD2_REPORT_KEYS	(KBD2_SIZE - 1)
#endif

#define DEBUG_TX_SIZE		32
#define DEBUG_TX_BUFFER		EP_DOUBLE_BUFFER

/* Private macro -------------------------------------------------------------*/
#define LSB(w)              ((uint8_t)((w)&0xff))
#define MSB(w)              ((uint8_t)((w>>8)&0xff))
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**************************************************************************
 *
 *  Descriptor Data
 *
 **************************************************************************/

// Descriptors are the data that your computer reads when it auto-detects
// this USB device (called "enumeration" in USB lingo).  The most commonly
// changed items are editable at the top of this file.  Changing things
// in here should only be done by those who've read chapter 9 of the USB
// spec and relevant portions of any USB class specifications!

const uint8_t device_descriptor[] =
  {
	18,					// bLength
	DEVICE_DESCRIPTOR,	// bDescriptorType
	0x00, 0x02,			// bcdUSB
	0,					// bDeviceClass
	0,					// bDeviceSubClass
	0,					// bDeviceProtocol
	ENDPOINT0_SIZE,		// bMaxPacketSize0
	LSB(VENDOR_ID), MSB(VENDOR_ID),		// idVendor
	LSB(PRODUCT_ID), MSB(PRODUCT_ID),	// idProduct
	0x00, 0x02,			// bcdDevice
	1,					// iManufacturer
	2,					// iProduct
	3,					// iSerialNumber
	1					// bNumConfigurations
  };

// Keyboard Protocol 1, HID 1.11 spec, Appendix B, page 59-60
const uint8_t keyboard_hid_report_desc[] =
  {
    0x05, 0x01,          // Usage Page (Generic Desktop),
    0x09, 0x06,          // Usage (Keyboard),
    0xA1, 0x01,          // Collection (Application),
    0x75, 0x01,          //   Report Size (1),
    0x95, 0x08,          //   Report Count (8),
    0x05, 0x07,          //   Usage Page (Key Codes),
    0x19, 0xE0,          //   Usage Minimum (224),
    0x29, 0xE7,          //   Usage Maximum (231),
    0x15, 0x00,          //   Logical Minimum (0),
    0x25, 0x01,          //   Logical Maximum (1),
    0x81, 0x02,          //   Input (Data, Variable, Absolute), ;Modifier byte
    0x95, 0x01,          //   Report Count (1),
    0x75, 0x08,          //   Report Size (8),
    0x81, 0x03,          //   Input (Constant),                 ;Reserved byte
    0x95, 0x05,          //   Report Count (5),
    0x75, 0x01,          //   Report Size (1),
    0x05, 0x08,          //   Usage Page (LEDs),
    0x19, 0x01,          //   Usage Minimum (1),
    0x29, 0x05,          //   Usage Maximum (5),
    0x91, 0x02,          //   Output (Data, Variable, Absolute), ;LED report
    0x95, 0x01,          //   Report Count (1),
    0x75, 0x03,          //   Report Size (3),
    0x91, 0x03,          //   Output (Constant),                 ;LED report padding
    0x95, KBD_REPORT_KEYS,    //   Report Count (),
    0x75, 0x08,          //   Report Size (8),
    0x15, 0x00,          //   Logical Minimum (0),
    0x25, 0xFF,          //   Logical Maximum(255),
    0x05, 0x07,          //   Usage Page (Key Codes),
    0x19, 0x00,          //   Usage Minimum (0),
    0x29, 0xFF,          //   Usage Maximum (255),
    0x81, 0x00,          //   Input (Data, Array),
    0xc0                 // End Collection
  };

#ifdef USB_NKRO_ENABLE
const uint8_t keyboard2_hid_report_desc[] =
  {
    0x05, 0x01,                     // Usage Page (Generic Desktop),
    0x09, 0x06,                     // Usage (Keyboard),
    0xA1, 0x01,                     // Collection (Application),
    // bitmap of modifiers
    0x75, 0x01,                     //   Report Size (1),
    0x95, 0x08,                     //   Report Count (8),
    0x05, 0x07,                     //   Usage Page (Key Codes),
    0x19, 0xE0,                     //   Usage Minimum (224),
    0x29, 0xE7,                     //   Usage Maximum (231),
    0x15, 0x00,                     //   Logical Minimum (0),
    0x25, 0x01,                     //   Logical Maximum (1),
    0x81, 0x02,                     //   Input (Data, Variable, Absolute), ;Modifier byte
    // LED output report
    0x95, 0x05,                     //   Report Count (5),
    0x75, 0x01,                     //   Report Size (1),
    0x05, 0x08,                     //   Usage Page (LEDs),
    0x19, 0x01,                     //   Usage Minimum (1),
    0x29, 0x05,                     //   Usage Maximum (5),
    0x91, 0x02,                     //   Output (Data, Variable, Absolute),
    0x95, 0x01,                     //   Report Count (1),
    0x75, 0x03,                     //   Report Size (3),
    0x91, 0x03,                     //   Output (Constant),
    // bitmap of keys
    0x95, KBD2_REPORT_KEYS*8,       //   Report Count (),
    0x75, 0x01,                     //   Report Size (1),
    0x15, 0x00,                     //   Logical Minimum (0),
    0x25, 0x01,                     //   Logical Maximum(1),
    0x05, 0x07,                     //   Usage Page (Key Codes),
    0x19, 0x00,                     //   Usage Minimum (0),
    0x29, KBD2_REPORT_KEYS*8-1,     //   Usage Maximum (),
    0x81, 0x02,                     //   Input (Data, Variable, Absolute),
    0xc0                            // End Collection
  };
#endif

#ifdef USB_MOUSE_ENABLE
// Mouse Protocol 1, HID 1.11 spec, Appendix B, page 59-60, with wheel extension
// http://www.microchip.com/forums/tm.aspx?high=&m=391435&mpage=1#391521
// http://www.keil.com/forum/15671/
// http://www.microsoft.com/whdc/device/input/wheel.mspx
const uint8_t mouse_hid_report_desc[] = {
    /* mouse */
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    //0x85, REPORT_ID_MOUSE,         //   REPORT_ID (1)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
                                   // ----------------------------  Buttons
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x05,                    //     USAGE_MAXIMUM (Button 5)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x05,                    //     REPORT_COUNT (5)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x03,                    //     REPORT_SIZE (3)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
                                   // ----------------------------  X,Y position
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
                                   // ----------------------------  Vertical wheel
    0x09, 0x38,                    //     USAGE (Wheel)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x35, 0x00,                    //     PHYSICAL_MINIMUM (0)        - reset physical
    0x45, 0x00,                    //     PHYSICAL_MAXIMUM (0)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
                                   // ----------------------------  Horizontal wheel
    0x05, 0x0c,                    //     USAGE_PAGE (Consumer Devices)
    0x0a, 0x38, 0x02,              //     USAGE (AC Pan)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //   END_COLLECTION
    0xc0,                          // END_COLLECTION
};
#endif

const uint8_t debug_hid_report_desc[] =
	{
		0x06, 0x31, 0xFF,	// Usage Page 0xFF31 (vendor defined)
		0x09, 0x74,				// Usage 0x74
		0xA1, 0x53,				// Collection 0x53
		0x75, 0x08,				// report size = 8 bits
		0x15, 0x00,				// logical minimum = 0
		0x26, 0xFF, 0x00,		  // logical maximum = 255
		0x95, DEBUG_TX_SIZE,	// report count
		0x09, 0x75,				// usage
		0x81, 0x02,				// Input (array)
		0xC0					    // end collection
	};

#ifdef USB_EXTRA_ENABLE
// audio controls & system controls
// http://www.microsoft.com/whdc/archive/w2kbd.mspx
const uint8_t extra_hid_report_desc[] = {
    /* system control */
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x80,                    // USAGE (System Control)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, REPORT_ID_SYSTEM,        //   REPORT_ID (2)
    0x15, 0x01,                    //   LOGICAL_MINIMUM (0x1)
    0x25, 0xb7,                    //   LOGICAL_MAXIMUM (0xb7)
    0x19, 0x01,                    //   USAGE_MINIMUM (0x1)
    0x29, 0xb7,                    //   USAGE_MAXIMUM (0xb7)
    0x75, 0x10,                    //   REPORT_SIZE (16)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x00,                    //   INPUT (Data,Array,Abs)
    0xc0,                          // END_COLLECTION
    /* consumer */
    0x05, 0x0c,                    // USAGE_PAGE (Consumer Devices)
    0x09, 0x01,                    // USAGE (Consumer Control)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, REPORT_ID_CONSUMER,      //   REPORT_ID (3)
    0x15, 0x01,                    //   LOGICAL_MINIMUM (0x1)
    0x26, 0x9c, 0x02,              //   LOGICAL_MAXIMUM (0x29c)
    0x19, 0x01,                    //   USAGE_MINIMUM (0x1)
    0x2a, 0x9c, 0x02,              //   USAGE_MAXIMUM (0x29c)
    0x75, 0x10,                    //   REPORT_SIZE (16)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x00,                    //   INPUT (Data,Array,Abs)
    0xc0,                          // END_COLLECTION
};
#endif

typedef enum _HID_NUMBER{
  KBD_HID_DESC_NUM,
#ifdef USB_MOUSE_ENABLE
  MOUSE_HID_DESC_NUM,
#endif
  DEBUG_HID_DESC_NUM,
#ifdef USB_EXTRA_ENABLE
  EXTRA_HID_DESC_NUM,
#endif
#ifdef USB_NKRO_ENABLE
  KBD2_HID_DESC_NUM,
#endif
  NUM_INTERFACES,
}HID_NUMBER;

#define KBD_HID_DESC_OFFSET        (9+9)
#define KBD_HID_DESC_SIZE          (9+9+7+7)

#ifdef USB_MOUSE_ENABLE
#  define MOUSE_HID_DESC_OFFSET    (9+KBD_HID_DESC_SIZE+9)
#  define MOUSE_HID_DESC_SIZE      (9+9+7)
#else
#  define MOUSE_HID_DESC_SIZE      0
#endif

#define DEBUG_HID_DESC_SIZE        (9+9+7)
#define DEBUG_HID_DESC_OFFSET      (9+KBD_HID_DESC_SIZE+DEBUG_HID_DESC_OFFSET+9)

#ifdef USB_EXTRA_ENABLE
#  define EXTRA_HID_DESC_OFFSET    (9+KBD_HID_DESC_SIZE+DEBUG_HID_DESC_SIZE+DEBUG_HID_DESC_SIZE+9)
#  define EXTRA_HID_DESC_SIZE      (9+9+7)
#else
#  define EXTRA_HID_DESC_SIZE      0
#endif

#ifdef USB_NKRO_ENABLE
#  define KBD2_HID_DESC_OFFSET     (9+KBD_HID_DESC_SIZE+DEBUG_HID_DESC_SIZE+DEBUG_HID_DESC_SIZE+EXTRA_HID_DESC_SIZE+9)
#  define KBD2_HID_DESC_SIZE       (9+9+7)
#else
#  define KBD2_HID_DESC_SIZE       0
#endif

#define CONFIG1_DESC_SIZE          (9+KBD_HID_DESC_SIZE+DEBUG_HID_DESC_SIZE+DEBUG_HID_DESC_SIZE+EXTRA_HID_DESC_SIZE+KBD2_HID_DESC_SIZE)

const uint8_t config1_descriptor[CONFIG1_DESC_SIZE] = {
	// configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
	9, 					// bLength;
	2,	        // bDescriptorType;
	LSB(CONFIG1_DESC_SIZE),			    // wTotalLength
	MSB(CONFIG1_DESC_SIZE),
	NUM_INTERFACES,		// bNumInterfaces
	1,					// bConfigurationValue
	0,					// iConfiguration
	0xA0,				// bmAttributes
	100,				// bMaxPower

	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,					// bLength
	4,		      // bDescriptorType
	KBD_INTERFACE,		// bInterfaceNumber
	0,					// bAlternateSetting
	1,					// bNumEndpoints
	0x03,				// bInterfaceClass (0x03 = HID)
	0x01,				// bInterfaceSubClass (0x01 = Boot)
	0x01,				// bInterfaceProtocol (0x01 = Keyboard)
	0,					// iInterface
    
	// HID descriptor, HID 1.11 spec, section 6.2.1
	9,					// bLength
	21,         // bDescriptorType
	0x11, 0x01,	// bcdHID
	0,					// bCountryCode
	1,					// bNumDescriptors
	0x22,				// bDescriptorType
	sizeof(keyboard_hid_report_desc),     	// wDescriptorLength
	0,
    
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,					// bLength
	5,	        // bDescriptorType
	KBD_ENDPOINT | 0x80,// bEndpointAddress
	0x03,				// bmAttributes (0x03=intr)
	KBD_SIZE, 0,		// wMaxPacketSize
	10,					// bInterval

  // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,					// bLength
	5,	        // bDescriptorType
	KBD_ENDPOINT,// bEndpointAddress
	0x03,				// bmAttributes (0x03=intr)
	1, 0,       // wMaxPacketSize
	10,					// bInterval

#ifdef USB_MOUSE_ENABLE
	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,					// bLength
	4,	        // bDescriptorType
	MOUSE_INTERFACE,	// bInterfaceNumber
	0,					// bAlternateSetting
	1,					// bNumEndpoints
	0x03,				// bInterfaceClass (0x03 = HID)
  // ThinkPad T23 BIOS doesn't work with boot mouse.
	0x00,				// bInterfaceSubClass (0x01 = Boot)
	0x00,				// bInterfaceProtocol (0x02 = Mouse)
  //0x01,				// bInterfaceSubClass (0x01 = Boot)
	//0x02,				// bInterfaceProtocol (0x02 = Mouse)
	0,					// iInterface
    
	// HID descriptor, HID 1.11 spec, section 6.2.1
	9,					// bLength
	21,         // bDescriptorType
	0x11, 0x01,	// bcdHID
	0,					// bCountryCode
	1,					// bNumDescriptors
	0x22,				// bDescriptorType
	sizeof(mouse_hid_report_desc),		// wDescriptorLength
	0,
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,					// bLength
	5,					// bDescriptorType
	MOUSE_ENDPOINT | 0x80,	// bEndpointAddress
	0x03,				// bmAttributes (0x03=intr)
	MOUSE_SIZE, 0,		// wMaxPacketSize
	1,					// bInterval
#endif

	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,					// bLength
	4,					// bDescriptorType
	DEBUG_INTERFACE,	// bInterfaceNumber
	0,					// bAlternateSetting
	1,					// bNumEndpoints
	0x03,				// bInterfaceClass (0x03 = HID)
	0x00,				// bInterfaceSubClass
	0x00,				// bInterfaceProtocol
	0,					// iInterface
	// HID descriptor, HID 1.11 spec, section 6.2.1
	9,					// bLength
	0x21,				// bDescriptorType
	0x11, 0x01,			// bcdHID
	0,					// bCountryCode
	1,					// bNumDescriptors
	0x22,				// bDescriptorType
	sizeof(debug_hid_report_desc),		// wDescriptorLength
	0,
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,					// bLength
	5,					// bDescriptorType
	DEBUG_TX_ENDPOINT | 0x80,		// bEndpointAddress
	0x03,				// bmAttributes (0x03=intr)
	DEBUG_TX_SIZE, 0,	// wMaxPacketSize
	1,					// bInterval

#ifdef USB_EXTRA_ENABLE
	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,					// bLength
	4,					// bDescriptorType
	EXTRA_INTERFACE,			// bInterfaceNumber
	0,					// bAlternateSetting
	1,					// bNumEndpoints
	0x03,					// bInterfaceClass (0x03 = HID)
	0x00,					// bInterfaceSubClass
	0x00,					// bInterfaceProtocol
	0,					// iInterface
	// HID descriptor, HID 1.11 spec, section 6.2.1
	9,					// bLength
	0x21,					// bDescriptorType
	0x11, 0x01,				// bcdHID
	0,					// bCountryCode
	1,					// bNumDescriptors
	0x22,					// bDescriptorType
	sizeof(extra_hid_report_desc),		// wDescriptorLength
	0,
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,					// bLength
	5,					// bDescriptorType
	EXTRA_ENDPOINT | 0x80,			// bEndpointAddress
	0x03,					// bmAttributes (0x03=intr)
	EXTRA_SIZE, 0,				// wMaxPacketSize
	10,					// bInterval
#endif

#ifdef USB_NKRO_ENABLE
	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,					// bLength
	4,					// bDescriptorType
	KBD2_INTERFACE,		// bInterfaceNumber
	0,					// bAlternateSetting
	1,					// bNumEndpoints
	0x03,				// bInterfaceClass (0x03 = HID)
	0x00,				// bInterfaceSubClass (0x01 = Boot)
	0x00,				// bInterfaceProtocol (0x01 = Keyboard)
	0,					// iInterface
    
	// HID descriptor, HID 1.11 spec, section 6.2.1
	9,					// bLength
	0x21,				// bDescriptorType
	0x11, 0x01,	// bcdHID
	0,					// bCountryCode
	1,					// bNumDescriptors
	0x22,				// bDescriptorType
	sizeof(keyboard2_hid_report_desc),     	// wDescriptorLength
	0,
    
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,					// bLength
	5,					// bDescriptorType
	KBD2_ENDPOINT | 0x80,			// bEndpointAddress
	0x03,				// bmAttributes (0x03=intr)
	KBD2_SIZE, 0,		// wMaxPacketSize
	1,					// bInterval
#endif
  };

#ifndef MANUFACTURER
#   define STR_MANUFACTURER	L"t.m.k."
#else
#   define STR_MANUFACTURER	LSTR(MANUFACTURER)
#endif
#ifndef PRODUCT
#   define STR_PRODUCT		L"t.m.k. keyboard"
#else
#   define STR_PRODUCT		LSTR(PRODUCT)
#endif

struct usb_string_descriptor_struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wString[32];
};
static struct usb_string_descriptor_struct strings[4] = {
	{ 4, 3,	{0x0409} },
	{ 0, 3, },
	{ 0, 3, },
	{ 0, 3, },
};

// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
static struct descriptor_list_struct {
	uint16_t	wValue;     // descriptor type
	uint16_t	wIndex;
	const uint8_t	*addr;
	uint8_t		length;
} descriptor_list[] = {
  // DEVICE descriptor
	{0x0100, 0x0000, device_descriptor, sizeof(device_descriptor)},
  // CONFIGURATION descriptor
	{0x0200, 0x0000, config1_descriptor, sizeof(config1_descriptor)},
  // HID/REPORT descriptors
	{0x2100, KBD_INTERFACE, config1_descriptor+KBD_HID_DESC_OFFSET, 9},
	{0x2200, KBD_INTERFACE, keyboard_hid_report_desc, sizeof(keyboard_hid_report_desc)},
#ifdef USB_MOUSE_ENABLE
	{0x2100, MOUSE_INTERFACE, config1_descriptor+MOUSE_HID_DESC_OFFSET, 9},
	{0x2200, MOUSE_INTERFACE, mouse_hid_report_desc, sizeof(mouse_hid_report_desc)},
#endif
	{0x2100, DEBUG_INTERFACE, config1_descriptor+DEBUG_HID_DESC_OFFSET, 9},
	{0x2200, DEBUG_INTERFACE, debug_hid_report_desc, sizeof(debug_hid_report_desc)},
#ifdef USB_EXTRA_ENABLE
	{0x2100, EXTRA_INTERFACE, config1_descriptor+EXTRA_HID_DESC_OFFSET, 9},
	{0x2200, EXTRA_INTERFACE, extra_hid_report_desc, sizeof(extra_hid_report_desc)},
#endif
#ifdef USB_NKRO_ENABLE
	{0x2100, KBD2_INTERFACE, config1_descriptor+KBD2_HID_DESC_OFFSET, 9},
	{0x2200, KBD2_INTERFACE, keyboard2_hid_report_desc, sizeof(keyboard2_hid_report_desc)},
#endif
  // STRING descriptors
	{0x0300, 0x0000, (const uint8_t *)&strings[0], 4}, // language id
	{0x0301, 0x0409, (const uint8_t *)&strings[1], },  // manufacturer
	{0x0302, 0x0409, (const uint8_t *)&strings[2], },  // product
	{0x0303, 0x0409, (const uint8_t *)&strings[3], },  // serial
};

struct descriptor_list_struct *list_get_descriptor(uint16_t wValue, uint16_t wIndex)
{
  int idx;
  for(idx=0; idx<sizeof(descriptor_list)/sizeof(descriptor_list[0]); idx++)
    if (descriptor_list[idx].wValue == wValue && descriptor_list[idx].wIndex == wIndex)
      return &descriptor_list[idx];
  return 0;
}

void monkey_set_string_descriptor(uint16_t wValue, uint16_t wIndex, const char *string) {
  struct descriptor_list_struct *descriptor;
  
  if((descriptor = list_get_descriptor(wValue, wIndex)) != 0) {
    struct usb_string_descriptor_struct *string_descriptor;
    int idx;

    string_descriptor = (struct usb_string_descriptor_struct *)descriptor->addr;
    string_descriptor->bDescriptorType = 3;
    for(idx=0; string[idx]; ++idx)
      string_descriptor->wString[idx] = (uint16_t)string[idx];
    string_descriptor->bLength = (idx+1)*2;
	descriptor->length = string_descriptor->bLength;
  }
}

uint8_t *monkey_get_descriptor(uint16_t Length)
{
  uint16_t wValue, wIndex, wOffset;
  struct descriptor_list_struct *desc;
  
  wValue = ByteSwap(pInformation->USBwValue);
  wIndex = ByteSwap(pInformation->USBwIndex);
  wOffset = pInformation->Ctrl_Info.Usb_wOffset;
  desc = list_get_descriptor(wValue, wIndex);

  if(desc) {
    if (Length == 0) {
      pInformation->Ctrl_Info.Usb_wLength = desc->length - wOffset;
      return 0;
    }
    return (uint8_t *)desc->addr + wOffset;
  }
  return 0;
}

#define EP_IN  1
#define EP_OUT 2

static struct ep_conf {
  uint8_t direct;
	uint8_t attr;
	uint16_t rx_addr;
	uint16_t rx_max;
  uint16_t rx_status;
	uint16_t tx_addr;
	uint16_t tx_max;
  uint16_t tx_status;
}ep_conf_list[8];

void monkey_ep_init(void)
{
  int pos, idx;
  uint16_t packet_buf_base;
  
  monkey_set_string_descriptor(0x0301, 0x0409, "Sapphire Zhao");
  monkey_set_string_descriptor(0x0302, 0x0409, "Monkey Pro");
  monkey_set_string_descriptor(0x0303, 0x0409, "Sapphire Zhao");

  ep_conf_list[0].direct = EP_IN | EP_OUT;
  ep_conf_list[0].attr = 1;
  ep_conf_list[0].rx_max = 64;
  ep_conf_list[0].tx_max = 64;
  ep_conf_list[0].rx_status = EP_RX_VALID;
  ep_conf_list[0].tx_status = EP_TX_STALL;
  packet_buf_base = 8;

	for(pos=0; pos<sizeof(config1_descriptor); pos+=config1_descriptor[pos]) {
		if(config1_descriptor[pos+1] == 0x5) { // ep descriptor
      uint8_t no, direct, attr;
      uint16_t max_packet;

      no = config1_descriptor[pos+2]&0x7;
      direct = config1_descriptor[pos+2]&0x80 ? EP_IN : EP_OUT;
      attr = config1_descriptor[pos+3]&3;
      max_packet = config1_descriptor[pos+4]+config1_descriptor[pos+5]*0x100;
      if(ep_conf_list[no].direct == 0) {
        ep_conf_list[no].tx_status = EP_TX_NAK;
        ep_conf_list[no].rx_status = EP_RX_VALID;
        ep_conf_list[no].attr = attr;
      }
      
      ep_conf_list[no].direct |= direct;
      if(direct & EP_IN) {
        ep_conf_list[no].tx_max = max_packet;
      } else {
        ep_conf_list[no].rx_max = max_packet;
      }

      packet_buf_base += 8;
		}
	}
#define EP_OUT_DEFAULT 1
#ifdef EP_OUT_DEFAULT
  for(idx=0; idx<8; ++idx) {
    if(ep_conf_list[idx].direct == EP_IN) {
        ep_conf_list[idx].direct |= EP_OUT;
        ep_conf_list[idx].rx_max = ep_conf_list[idx].tx_max;
        packet_buf_base += 8;
    }
  }
#endif      
  for(idx=0; idx<8; ++idx) {
    if(ep_conf_list[idx].direct) {
      if(ep_conf_list[idx].direct & EP_IN) {
        ep_conf_list[idx].tx_addr = packet_buf_base;
        packet_buf_base += (ep_conf_list[idx].tx_max+3)&~3;
      }
      
      if(ep_conf_list[idx].direct & EP_OUT) {
        ep_conf_list[idx].rx_addr = packet_buf_base;
        packet_buf_base += (ep_conf_list[idx].rx_max+3)&~3;
      }
    }
  }
}

void monkey_ep_reset(void) {
  int i;

  /* Set Joystick_DEVICE as not configured */
  pInformation->Current_Configuration = 0;
  pInformation->Current_Interface = 0;/*the default Interface*/

  /* Current Feature initialization */
  pInformation->Current_Feature = config1_descriptor[7];

  for(i=0; i<8; ++i) {
    if(ep_conf_list[i].direct==0) continue;
    
		switch(ep_conf_list[i].attr) {
			case 0x00:
        SetEPType(i, EP_BULK);
        ClearEPDoubleBuff(i);
        break;
			case 0x01:
        SetEPType(i, EP_CONTROL);
        Clear_Status_Out(i);
        break;
			case 0x02:
        SetEPType(i, EP_ISOCHRONOUS);
        break;
			case 0x03:
        SetEPType(i, EP_INTERRUPT);
        break;
		}

    if(ep_conf_list[i].direct & EP_IN) {
      SetEPTxStatus(i, ep_conf_list[i].tx_status);
      SetEPTxAddr(i, ep_conf_list[i].tx_addr);
      SetEPTxCount(i, ep_conf_list[i].tx_max);
    }
    if(ep_conf_list[i].direct & EP_OUT) {
      SetEPRxAddr(i, ep_conf_list[i].rx_addr);
      SetEPRxCount(i, ep_conf_list[i].rx_max);
      SetEPRxStatus(i, ep_conf_list[i].rx_status);
    }
	}
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

