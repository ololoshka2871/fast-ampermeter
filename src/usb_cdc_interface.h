#ifndef USB_CDC_INTERFACE_H
#define USB_CDC_INTERFACE_H

extern uint8_t UserRxBuffer[CDC_DATA_FS_MAX_PACKET_SIZE];
extern uint8_t UserTxBuffer[CDC_DATA_FS_MAX_PACKET_SIZE];

extern USBD_HandleTypeDef USBD_Device;
extern uint32_t usb_data_ressived;

#endif // USB_CDC_INTERFACE_H
