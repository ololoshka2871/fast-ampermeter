
#include "BoardInit.h"

#include "usb_device.h"
#include "usbd_custom_hid_if.h"

extern "C" int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len);

/*!
 * \brief fast-ampermeter entry point
 * \return Never
 */
int main(void) {
  InitBoard();

  MX_USB_DEVICE_Init();

  uint8_t report = 0;

  while (1) {
    /* USER CODE END WHILE */
    HAL_Delay(500);
    /*
    USBD_CUSTOM_HID_SendReport_FS(&report, 1);
    report = !report;
    */
  }
}
