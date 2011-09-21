#include "stm32f10x.h"
#include "usb_lib.h"
#include "usb.h"
#include "usb_keycodes.h"
#include "monkey.h"
#include "keymap.h"

__IO uint32_t bDeviceState; /* USB device status */
__IO bool fSuspendEnabled;  /* true when suspend is possible */
__IO uint8_t EXTI_Enable;
__IO uint8_t PrevXferComplete;
__IO uint16_t wIstr;  /* ISTR register last read value */
__IO uint8_t bIntPackSOF = 0;  /* SOFs received between 2 consecutive packets */
uint32_t ProtocolValue;
uint8_t Receive_Buffer[8];
uint8_t UsbIdleRate = 0;
static uint8_t monkey_short_request_buffer[64];
uint8_t *monkey_msg_ptr;
uint16_t monkey_msg_length;
volatile uint8_t usb_keyboard_leds=0;

#define MONKEY_NO_PROCESS (-1)

extern void monkey_set_string_descriptor(uint16_t wValue, uint16_t wIndex, const char *string);
extern struct descriptor_list_struct *list_get_descriptor(uint16_t wValue, uint16_t wIndex);
extern void monkey_ep_init(void);
extern void monkey_ep_reset(void);
extern void Get_SerialNum(void);
extern void USB_Cable_Config (FunctionalState NewState);

#define monkey_GetConfiguration          NOP_Process
#define monkey_GetInterface              NOP_Process
#define monkey_SetInterface              NOP_Process
#define monkey_GetStatus                 NOP_Process
#define monkey_ClearFeature              NOP_Process
#define monkey_SetEndPointFeature        NOP_Process
#define monkey_SetDeviceFeature          NOP_Process

struct
{
  __IO RESUME_STATE eState;
  __IO uint8_t bESOFcnt;
}ResumeS;

void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;
}

void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;
  
  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else 
  {
    bDeviceState = ATTACHED;
  }
}

RESULT PowerOn(void)
{
  uint16_t wRegVal;

  /*** cable plugged-in ? ***/
  USB_Cable_Config(ENABLE);

  /*** CNTR_PWDN = 0 ***/
  wRegVal = CNTR_FRES;
  _SetCNTR(wRegVal);

  /*** CNTR_FRES = 0 ***/
  wInterrupt_Mask = 0;
  _SetCNTR(wInterrupt_Mask);
  /*** Clear pending interrupts ***/
  _SetISTR(0);
  /*** Set interrupt mask ***/
  wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM;
  _SetCNTR(wInterrupt_Mask);

  return USB_SUCCESS;
}

RESULT PowerOff()
{
  /* disable all interrupts and force USB reset */
  _SetCNTR(CNTR_FRES);
  /* clear interrupt status register */
  _SetISTR(0);
  /* Disable the Pull-Up*/
  USB_Cable_Config(DISABLE);
  /* switch-off device */
  _SetCNTR(CNTR_FRES + CNTR_PDWN);

  /* sw variables reset */
  /* ... */

  return USB_SUCCESS;
}

void Suspend(void)
{
  uint16_t wCNTR;
  /* suspend preparation */
  /* ... */

  /* macrocell enters suspend mode */
  wCNTR = _GetCNTR();
  wCNTR |= CNTR_FSUSP;
  _SetCNTR(wCNTR);

  /* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
  /* power reduction */
  /* ... on connected devices */

  /* switch-off the clocks */
  /* ... */
  Enter_LowPowerMode();

}

void Resume_Init(void)
{
  uint16_t wCNTR;

  /* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
  /* restart the clocks */
  /* ...  */

  /* CNTR_LPMODE = 0 */
  wCNTR = _GetCNTR();
  wCNTR &= (~CNTR_LPMODE);
  _SetCNTR(wCNTR);

  /* restore full power */
  /* ... on connected devices */
  Leave_LowPowerMode();

  /* reset FSUSP bit */
  _SetCNTR(IMR_MSK);

  /* reverse suspend preparation */
  /* ... */

}

void Resume(RESUME_STATE eResumeSetVal)
{
  uint16_t wCNTR;

  if (eResumeSetVal != RESUME_ESOF)
    ResumeS.eState = eResumeSetVal;

  switch (ResumeS.eState)
  {
    case RESUME_EXTERNAL:
      Resume_Init();
      ResumeS.eState = RESUME_OFF;
      break;
    case RESUME_INTERNAL:
      Resume_Init();
      ResumeS.eState = RESUME_START;
      break;
    case RESUME_LATER:
      ResumeS.bESOFcnt = 2;
      ResumeS.eState = RESUME_WAIT;
      break;
    case RESUME_WAIT:
      ResumeS.bESOFcnt--;
      if (ResumeS.bESOFcnt == 0)
        ResumeS.eState = RESUME_START;
      break;
    case RESUME_START:
      wCNTR = _GetCNTR();
      wCNTR |= CNTR_RESUME;
      _SetCNTR(wCNTR);
      ResumeS.eState = RESUME_ON;
      ResumeS.bESOFcnt = 10;
      break;
    case RESUME_ON:
      ResumeS.bESOFcnt--;
      if (ResumeS.bESOFcnt == 0)
      {
        wCNTR = _GetCNTR();
        wCNTR &= (~CNTR_RESUME);
        _SetCNTR(wCNTR);
        ResumeS.eState = RESUME_OFF;
      }
      break;
    case RESUME_OFF:
    case RESUME_ESOF:
    default:
      ResumeS.eState = RESUME_OFF;
      break;
  }
}

void monkey_init(void)
{
  pInformation->Current_Configuration = 0;
  /* Connect the device */
  PowerOn();
  /* Perform basic device initialization operations */
  USB_SIL_Init();
	monkey_ep_init();
  bDeviceState = UNCONNECTED;
}

void monkey_Reset(void)
{
  SetBTABLE(BTABLE_ADDRESS);
  monkey_ep_reset();
  /* Set this device to response on default address */
  SetDeviceAddress(0);
  bDeviceState = ATTACHED;
}

void monkey_SetConfiguration(void)
{
  if (pInformation->Current_Configuration != 0) {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
}

void monkey_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}

void monkey_Status_In(void)
{
}

void monkey_Status_Out (void)
{
}

uint8_t monkey_vendor_request(uint8_t RequestNo)
{
	if(KURQ_AIKON_SET_KEY_MIN  <= RequestNo &&
		 RequestNo <= KURQ_AIKON_SET_KEY_MAX)
	{
		uint8_t layer = pInformation->USBwIndex0;;
		uint8_t key_index = RequestNo;
		if ( layer >= 0 && layer < layer_max)
			monkey_config.matrix[layer][key_index] = pInformation->USBwValue0;
		return 0;
	}
	
  switch(RequestNo)
  {
	case  KURQ_AIKON_READ_MATRIX_NORMAL:
		monkey_msg_ptr = monkey_config.matrix[layer_normal];
		return layer_aikon_keys;
	case  KURQ_AIKON_READ_MATRIX_FN:
		monkey_msg_ptr = monkey_config.matrix[layer_fn1];
		return layer_aikon_keys;
	case  KURQ_AIKON_READ_MATRIX_NUMLOCK:
		monkey_msg_ptr = monkey_config.matrix[layer_numlock];
		return layer_aikon_keys;
	case KURQ_AIKON_WRITE_TO_EEPROM:
		//save_monkey_config = true;
		return 0;
	case KURQ_AIKON_GET_LAST_KEY_INFO:
	{
		monkey_msg_ptr[0] = last_row;
		monkey_msg_ptr[1] = last_column;
		monkey_msg_ptr[2] = keymap_get_keycode(layer_normal, last_row, last_column);
		monkey_msg_ptr[3] = keymap_get_keycode(layer_fn1, last_row, last_column);
		monkey_msg_ptr[4] = keymap_get_keycode(layer_numlock, last_row, last_column);
		return 5;
	}
	case KURQ_AIKON_GET_ROWCOL:
		monkey_msg_ptr[0] = monkey_config.row-1;
		monkey_msg_ptr[1] = monkey_config.column-1;
		return 2;
	case KURQ_AIKON_SET_ROW:
		monkey_config.row =  pInformation->USBwValue0+1;
		return 0;
	case KURQ_AIKON_SET_COLUMN:
		monkey_config.column = pInformation->USBwValue0+1;
		return 0;
	case KURQ_AIKON_GET_FLAGS:
		monkey_msg_ptr[0] = monkey_config.flags;
		return 1;
	case KURQ_AIKON_SET_FLAGS:
		monkey_config.flags =  pInformation->USBwValue0;
		return 0;
	case KURQ_RESET:
    /* loop "endlessly" for ~0.5s */
	default:
    return MONKEY_NO_PROCESS;
  }
}

uint8_t *monkey_copydata(uint16_t Length) {
  if(Length == 0) {
    pInformation->Ctrl_Info.Usb_wLength =
        monkey_msg_length - pInformation->Ctrl_Info.Usb_wOffset;
    return 0;
  }
  return monkey_msg_ptr + pInformation->Ctrl_Info.Usb_wOffset;
}

int monkey_request_handler(uint8_t RequestNo) {
  if(Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT)) {
    if (RequestNo == GET_DESCRIPTOR) {
        struct descriptor_list_struct *desc;
        desc = list_get_descriptor(ByteSwap(pInformation->USBwValue),
                                   ByteSwap(pInformation->USBwIndex));
        if (desc) {
          monkey_msg_ptr = (uint8_t *)desc->addr;
          return desc->length;
        } 
    }
  } else if(Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
		switch(RequestNo) {
      case GET_PROTOCOL:
        *(uint8_t*)monkey_msg_ptr = ProtocolValue;
        return 1;
      case SET_PROTOCOL:
        ProtocolValue = pInformation->USBwValue0;
        return 0;
      case GET_REPORT:
        break;
      case SET_REPORT:
        break;
      case GET_IDLE:
       *(uint8_t*)monkey_msg_ptr = UsbIdleRate;        
        return 1;
      case SET_IDLE:
        UsbIdleRate = pInformation->USBwValue1;
        return 0;
		}
  } else if(Type_Recipient == (VENDOR_REQUEST | INTERFACE_RECIPIENT)) {
    return monkey_vendor_request(RequestNo);
  }
  return MONKEY_NO_PROCESS;
}

RESULT monkey_Data_Setup(uint8_t RequestNo)
{
  monkey_msg_ptr = monkey_short_request_buffer;
  monkey_msg_length = monkey_request_handler(RequestNo);

  if(monkey_msg_length == MONKEY_NO_PROCESS) 
    return USB_UNSUPPORT;
  else if(monkey_msg_length == 0) 
    return USB_SUCCESS;
  
  pInformation->Ctrl_Info.CopyData = monkey_copydata;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*pInformation->Ctrl_Info.CopyData)(0);
  return USB_SUCCESS;
}

RESULT monkey_NoData_Setup(uint8_t RequestNo)
{
  monkey_msg_length = monkey_request_handler(RequestNo);

  if(monkey_msg_length == MONKEY_NO_PROCESS)
    return USB_UNSUPPORT;
  return USB_SUCCESS;
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

RESULT monkey_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
    return USB_UNSUPPORT;
  else if (Interface > 0)
    return USB_UNSUPPORT;
  return USB_SUCCESS;
}

void USB_Istr(void)
{

  wIstr = _GetISTR();

  if (wIstr & ISTR_CTR & wInterrupt_Mask)
  {
    /* servicing of the endpoint correct transfer interrupt */
    /* clear of the CTR flag into the sub */
    CTR_LP();
  }
  
  if (wIstr & ISTR_RESET & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_RESET);
    Device_Property.Reset();
  }
  
  if (wIstr & ISTR_DOVR & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_DOVR);
  }
  
  if (wIstr & ISTR_ERR & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_ERR);
  }
  
  if (wIstr & ISTR_WKUP & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_WKUP);
    Resume(RESUME_EXTERNAL);
  }
  
  if (wIstr & ISTR_SUSP & wInterrupt_Mask)
  {
    /* check if SUSPEND is possible */
    if (fSuspendEnabled) {
      Suspend();
    } else {
      /* if not possible then resume after xx ms */
      Resume(RESUME_LATER);
    }
    /* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
    _SetISTR((uint16_t)CLR_SUSP);
  }
  
  if (wIstr & ISTR_SOF & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_SOF);
    bIntPackSOF++;
  }
  
  if (wIstr & ISTR_ESOF & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_ESOF);
    /* resume handling timing is made with ESOFs */
    Resume(RESUME_ESOF); /* request without change of the machine state */
  }
} /* USB_Istr */

void EP1_OUT_Callback(void)
{
  /* Read received data (2 bytes) */  
  USB_SIL_Read(EP1_OUT, Receive_Buffer);
  
  SetEPRxStatus(ENDP1, EP_RX_VALID);
}

void EP1_IN_Callback(void)
{
  PrevXferComplete = 1;
}

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table = {
  EP_NUM,  /* EP number*/
  1        /* Configuration */
};

DEVICE_PROP Device_Property = {
  monkey_init,
  monkey_Reset,
  monkey_Status_In,
  monkey_Status_Out,
  monkey_Data_Setup,
  monkey_NoData_Setup,
  monkey_Get_Interface_Setting,
  monkey_get_descriptor,
  monkey_get_descriptor,
  monkey_get_descriptor,
  0,
  0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests = {
  monkey_GetConfiguration,
  monkey_SetConfiguration,
  monkey_GetInterface,
  monkey_SetInterface,
  monkey_GetStatus,
  monkey_ClearFeature,
  monkey_SetEndPointFeature,
  monkey_SetDeviceFeature,
  monkey_SetDeviceAddress
};

void (*pEpInt_IN[7])(void) = {
  EP1_IN_Callback,
  EP2_IN_Callback,
  EP3_IN_Callback,
  EP4_IN_Callback,
  EP5_IN_Callback,
  EP6_IN_Callback,
  EP7_IN_Callback,
};

void (*pEpInt_OUT[7])(void) = {
  EP1_OUT_Callback,
  EP2_OUT_Callback,
  EP3_OUT_Callback,
  EP4_OUT_Callback,
  EP5_OUT_Callback,
  EP6_OUT_Callback,
  EP7_OUT_Callback,
};
