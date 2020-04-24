#include <stdbool.h>
#include <stdint.h>

#include "hw_includes.h"

#include "usbd_cdc.h"
#include "usbd_core.h"

USBD_CDC_LineCodingTypeDef LineCoding = {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
};

uint8_t UserRxBuffer[CDC_DATA_FS_MAX_PACKET_SIZE];

/* USB handler declaration */
USBD_HandleTypeDef USBD_Device;
uint32_t usb_data_ressived;

/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_Itf_Init(void);
static int8_t CDC_Itf_DeInit(void);
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_Itf_Receive(uint8_t *pbuf, uint32_t *Len);

USBD_CDC_ItfTypeDef USBD_CDC_fops = {CDC_Itf_Init, CDC_Itf_DeInit,
                                     CDC_Itf_Control, CDC_Itf_Receive};

/**
 * @brief  CDC_Itf_Init
 *         Initializes the CDC media low layer
 * @param  None
 * @retval Result of the opeartion: USBD_OK if all operations are OK else
 * USBD_FAIL
 */
static int8_t CDC_Itf_Init(void) {
  // USBD_CDC_SetTxBuffer(&USBD_Device, UserTxBuffer, 0);
  USBD_CDC_SetRxBuffer(&USBD_Device, UserRxBuffer);
  return (USBD_OK);
}

/**
 * @brief  CDC_Itf_DeInit
 *         DeInitializes the CDC media low layer
 * @param  None
 * @retval Result of the opeartion: USBD_OK if all operations are OK else
 * USBD_FAIL
 */
static int8_t CDC_Itf_DeInit(void) { return (USBD_OK); }

/**
 * @brief  CDC_Itf_Control
 *         Manage the CDC class requests
 * @param  Cmd: Command code
 * @param  Buf: Buffer containing command data (request parameters)
 * @param  Len: Number of data to be sent (in bytes)
 * @retval Result of the opeartion: USBD_OK if all operations are OK else
 * USBD_FAIL
 */
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length) {
  return (USBD_OK);
}

/**
 * @brief  CDC_Itf_DataRx
 *         Data received over USB OUT endpoint are sent over CDC interface
 *         through this function.
 * @param  Buf: Buffer of data to be transmitted
 * @param  Len: Number of data received (in bytes)
 * @retval Result of the opeartion: USBD_OK if all operations are OK else
 * USBD_FAIL
 *
 * Принятые данные лежат в Buf
 * Если придет еще 1 пакет, он будет лежать в собственном буфере USB
 * Еще 1 паект не будет послан хостом, ибо приемник не готов
 *
 * Нужно обработать Buf и дернуть USBD_CDC_ReceivePacket()
 */
static int8_t CDC_Itf_Receive(uint8_t *Buf, uint32_t *Len) {
  // USBD_CDC_ReceivePacket(&USBD_Device);
  usb_data_ressived = *Len;
  return (USBD_OK);
}
