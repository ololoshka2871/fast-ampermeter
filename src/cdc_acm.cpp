#include "hw_includes.h"

#include "usbd_cdc.h"
#include "usbd_core.h"

#include "usbd_desc.h"

#include "usb_cdc_interface.h"

#include "cdc_acm.h"

void CDC_ACM::init() {
  extern USBD_CDC_ItfTypeDef USBD_CDC_fops;

  /* Init Device Library */
  USBD_Init(&USBD_Device, &VCP_Desc, 0);

  /* Add Supported Class */
  USBD_RegisterClass(&USBD_Device, &USBD_CDC);

  /* Add CDC Interface Class */
  USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

  /* Start Device Process */
  USBD_Start(&USBD_Device);
}

CDC_ACM::RxData CDC_ACM::tryReadData() {
  RxData res(usb_data_ressived);
  usb_data_ressived = 0;
  return res;
}

CDC_ACM::RxData::~RxData() {
  // Говорим драйверу, что мы закончили работу с буфером и можно его освободить
  if (size()) {
    USBD_CDC_ReceivePacket(&USBD_Device);
  }
}

uint8_t *CDC_ACM::RxData::pData() const { return UserRxBuffer; }
