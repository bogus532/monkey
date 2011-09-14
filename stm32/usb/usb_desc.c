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
#include "usb_desc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ENDPOINT0_SIZE		64
#ifndef VENDOR_ID
#   define VENDOR_ID		0xFEED
#endif

#ifndef PRODUCT_ID
#   define PRODUCT_ID		0xBABE
#endif

#define KBD_INTERFACE		0
#define KBD_ENDPOINT		1
#define KBD_SIZE		    8
#define KBD_BUFFER		    EP_DOUBLE_BUFFER
#define KBD_REPORT_KEYS		(KBD_SIZE - 2)

// secondary keyboard
#ifdef USB_NKRO_ENABLE
#define KBD2_INTERFACE		4
#define KBD2_ENDPOINT		5
#define KBD2_SIZE		    16
#define KBD2_BUFFER		    EP_DOUBLE_BUFFER
#define KBD2_REPORT_KEYS	(KBD2_SIZE - 1)
#endif

#define DEBUG_INTERFACE		2
#define DEBUG_TX_ENDPOINT	3
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
	USB_DEVICE_DESCRIPTOR_TYPE,	// bDescriptorType
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
		0x06, 0x31, 0xFF,			// Usage Page 0xFF31 (vendor defined)
		0x09, 0x74,				// Usage 0x74
		0xA1, 0x53,				// Collection 0x53
		0x75, 0x08,				// report size = 8 bits
		0x15, 0x00,				// logical minimum = 0
		0x26, 0xFF, 0x00,		// logical maximum = 255
		0x95, DEBUG_TX_SIZE,	// report count
		0x09, 0x75,				// usage
		0x81, 0x02,				// Input (array)
		0xC0					// end collection
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

#define KBD_HID_DESC_NUM                0
#define KBD_HID_DESC_OFFSET             (9+(9+9+7)*KBD_HID_DESC_NUM+9)

#ifdef USB_MOUSE_ENABLE
#   define MOUSE_HID_DESC_NUM           (KBD_HID_DESC_NUM + 1)
#   define MOUSE_HID_DESC_OFFSET        (9+(9+9+7)*MOUSE_HID_DESC_NUM+9)
#else
#   define MOUSE_HID_DESC_NUM           (KBD_HID_DESC_NUM + 0)
#endif

#define DEBUG_HID_DESC_NUM              (MOUSE_HID_DESC_NUM + 1)
#define DEBUG_HID_DESC_OFFSET           (9+(9+9+7)*DEBUG_HID_DESC_NUM+9)

#ifdef USB_EXTRA_ENABLE
#   define EXTRA_HID_DESC_NUM           (DEBUG_HID_DESC_NUM + 1)
#   define EXTRA_HID_DESC_OFFSET        (9+(9+9+7)*EXTRA_HID_DESC_NUM+9)
#else
#   define EXTRA_HID_DESC_NUM           (DEBUG_HID_DESC_NUM + 0)
#endif

#ifdef USB_NKRO_ENABLE
#   define KBD2_HID_DESC_NUM            (EXTRA_HID_DESC_NUM + 1)
#   define KBD2_HID_DESC_OFFSET         (9+(9+9+7)*EXTRA_HID_DESC_NUM+9)
#else
#   define KBD2_HID_DESC_NUM            (EXTRA_HID_DESC_NUM + 0)
#endif

#define NUM_INTERFACES                  (KBD2_HID_DESC_NUM + 1)
#define CONFIG1_DESC_SIZE               (9+(9+9+7)*NUM_INTERFACES)

const uint8_t config1_descriptor[CONFIG1_DESC_SIZE] = {
	// configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
	9, 					// bLength;
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	// bDescriptorType;
	LSB(CONFIG1_DESC_SIZE),			    // wTotalLength
	MSB(CONFIG1_DESC_SIZE),
	NUM_INTERFACES,		// bNumInterfaces
	1,					// bConfigurationValue
	0,					// iConfiguration
	0xA0,				// bmAttributes
	100,				// bMaxPower

	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,					// bLength
	USB_INTERFACE_DESCRIPTOR_TYPE,		// bDescriptorType
	KBD_INTERFACE,		// bInterfaceNumber
	0,					// bAlternateSetting
	1,					// bNumEndpoints
	0x03,				// bInterfaceClass (0x03 = HID)
	0x01,				// bInterfaceSubClass (0x01 = Boot)
	0x01,				// bInterfaceProtocol (0x01 = Keyboard)
	0,					// iInterface
    
	// HID descriptor, HID 1.11 spec, section 6.2.1
	9,					// bLength
	HID_DESCRIPTOR_TYPE,// bDescriptorType
	0x11, 0x01,			// bcdHID
	0,					// bCountryCode
	1,					// bNumDescriptors
	0x22,				// bDescriptorType
	sizeof(keyboard_hid_report_desc),     	// wDescriptorLength
	0,
    
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,					// bLength
	USB_ENDPOINT_DESCRIPTOR_TYPE,	// bDescriptorType
	KBD_ENDPOINT | 0x80,// bEndpointAddress
	0x03,				// bmAttributes (0x03=intr)
	KBD_SIZE, 0,		// wMaxPacketSize
	10,					// bInterval

#ifdef USB_MOUSE_ENABLE
	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,					// bLength
	USB_INTERFACE_DESCRIPTOR_TYPE,	// bDescriptorType
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
	HID_DESCRIPTOR_TYPE,// bDescriptorType
	0x11, 0x01,			// bcdHID
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
	0x11, 0x01,			// bcdHID
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

/* USB String Descriptors (optional) */
const uint8_t CustomHID_StringLangID[CUSTOMHID_SIZ_STRING_LANGID] =
  {
    CUSTOMHID_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
  }
  ; /* LangID = 0x0409: U.S. English */

ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)device_descriptor,
    sizeof(device_descriptor)
  };

ONE_DESCRIPTOR Config_Descriptor =
  {
    (uint8_t*)config_descriptor,
    sizeof(config_descriptor)
  };

ONE_DESCRIPTOR CustomHID_Report_Descriptor =
  {
    (uint8_t *)CustomHID_ReportDescriptor,
    CUSTOMHID_SIZ_REPORT_DESC
  };

ONE_DESCRIPTOR CustomHID_Hid_Descriptor =
  {
    (uint8_t*)CustomHID_ConfigDescriptor + CUSTOMHID_OFF_HID_DESC,
    CUSTOMHID_SIZ_HID_DESC
  };

/*******************************************************************************
* Function Name  : CustomHID_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *CustomHID_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : CustomHID_GetConfigDescriptor.
* Description    : Gets the configuration descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *CustomHID_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : CustomHID_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *CustomHID_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  if (wValue0 > 4)
  {
    return NULL;
  }
  else 
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}

/*******************************************************************************
* Function Name  : CustomHID_GetReportDescriptor.
* Description    : Gets the HID report descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *CustomHID_GetReportDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &CustomHID_Report_Descriptor);
}

/*******************************************************************************
* Function Name  : CustomHID_GetHIDDescriptor.
* Description    : Gets the HID descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *CustomHID_GetHIDDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &CustomHID_Hid_Descriptor);
}

struct ep_conf
{
	uint8_t ep_no;
	uint8_t attr;
	uint16_t max_packet_size;
	uint8_t interval;

	uint16_t rx_addr;
	uint16_t tx_addr;
}eps[8] = { {0, 0, 64, 0}, };

void ep_init(uint8_t *conf_desc, size_t desc_size)
{
	for(int i=1; i<(sizeof(eps)/sizeof(eps[0])); ++i)
		eps[i].ep_no = 0xff;

	int pos=0, ep_num=1;
	while(pos < desc_size) {
		if (conf_desc[pos+1] == USB_DESC_TYPE_ENDPOINT) {
			eps[ep_num].ep_no = conf_desc[pos+2];
			eps[ep_num].attr = conf_desc[pos+3];
			eps[ep_num].max_packet_size = conf_desc[pos+4]+conf_desc[5]*0x100;
			eps[ep_num].interval = conf_desc[pos+6];
			ep_num++;
		}
		pos += conf_desc[0];
	}

	struct ep_conf *p = eps;
	uint16_t base = ep_num*8;
	while(p->ep_no != 0xff) {
		uint8_t no = p->ep_no & 0x7f;
		switch (p->attr & 0x03) {
			case 0x00: SetEPType(no, EP_BULK);	break;
			case 0x01: SetEPType(no, EP_CONTROL);	break;
			case 0x02: SetEPType(no, EP_ISOCHRONOUS);	break;
			case 0x03: SetEPType(no, EP_INTERRUPT);	break;
		}

		SetEPTxAddr(no, ENDP1_TXADDR);
		SetEPRxAddr(no, ENDP1_RXADDR);
		SetEPTxCount(no, 2);
		SetEPRxCount(no, 2);
		SetEPRxStatus(no, EP_RX_VALID);
		SetEPTxStatus(no, EP_TX_NAK);
	}
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

