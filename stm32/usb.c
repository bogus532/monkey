#include "stm32f10x.h"
#include "usb_lib.h"
#include "usb.h"

__IO uint32_t bDeviceState; /* USB device status */
__IO bool fSuspendEnabled;  /* true when suspend is possible */
__IO uint8_t EXTI_Enable;
__IO uint8_t PrevXferComplete;
__IO uint16_t wIstr;  /* ISTR register last read value */
__IO uint8_t bIntPackSOF = 0;  /* SOFs received between 2 consecutive packets */
uint32_t ProtocolValue;
uint8_t Receive_Buffer[8];

extern void monkey_set_string_descriptor(uint16_t wValue, uint16_t wIndex, const char *string);
extern uint8_t *monkey_get_descriptor(uint16_t Length);
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

/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
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
  /* Update the serial number string descriptor with the data from the unique 
  ID*/
  Get_SerialNum();
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

uint8_t ReportBuffer[32] = {0};
uint8_t ReportBufferOut[32] = {0};

uint8_t *monkey_GetReport(uint16_t Length)
{
  uint16_t wOffset;
  wOffset = pInformation->Ctrl_Info.Usb_wOffset;

	if(Length == 0) {
		int len;
		if(pInformation->USBwIndex == 0)
			len=8;
		else if(pInformation->USBwIndex == 1)
			len=32;
		pInformation->Ctrl_Info.Usb_wLength = len - wOffset;
		return 0;
	} else {
		return ReportBuffer + wOffset;
	}
}

uint8_t *monkey_SetReport(uint16_t Length)
{
	if (Length == 0) {
		pInformation->Ctrl_Info.Usb_wLength = 1;
		return 0;
	} else {
		return ReportBufferOut;
	}
}

uint8_t *monkey_GetProtocolValue(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = 1;
    return NULL;
  }
  else
  {
    return (uint8_t *)(&ProtocolValue);
  }
}

RESULT monkey_SetProtocol(void)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  ProtocolValue = wValue0;
  return USB_SUCCESS;
}

RESULT monkey_Data_Setup(uint8_t RequestNo)
{
  uint8_t *(*CopyRoutine)(uint16_t);

  CopyRoutine = NULL;

  if (RequestNo == GET_DESCRIPTOR) {
    CopyRoutine = monkey_get_descriptor;
  } else if(Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
		switch(RequestNo) {
		case GET_PROTOCOL:
			CopyRoutine = monkey_GetProtocolValue;
			break;
		case GET_REPORT:
			CopyRoutine = monkey_GetReport;
			break;
		case SET_REPORT:
			CopyRoutine = monkey_SetReport;
			break;
		}
	}

  if (CopyRoutine == NULL)
    return USB_UNSUPPORT;

  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);
  return USB_SUCCESS;
}

RESULT monkey_NoData_Setup(uint8_t RequestNo)
{
  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
  {
    switch (RequestNo)
    {
      case GET_PROTOCOL:
      case SET_PROTOCOL:
        return monkey_SetProtocol();
      case SET_IDLE:
				return USB_SUCCESS;
    }
  }
  return USB_UNSUPPORT;
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
  BitAction Led_State;

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
