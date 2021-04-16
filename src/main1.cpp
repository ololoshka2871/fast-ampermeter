
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

  uint16_t report = 25;

  while (1) {
    /* USER CODE END WHILE */
    HAL_Delay(500);

    USBD_CUSTOM_HID_SendReport_FS(reinterpret_cast<uint8_t*>(&report),
                                  sizeof (report));
    report = !report;
  }
}
