/*! \ingroup Utils Utility and halpers functions
 * @{
 */

#include "hw_includes.h"

/*
extern I2C_HandleTypeDef i2c1;

void DMA1_Channel2_3_IRQHandler() {
    HAL_DMA_IRQHandler(i2c1.hdmarx);
    HAL_DMA_IRQHandler(i2c1.hdmatx);
}

void I2C1_IRQHandler() {
    HAL_I2C_EV_IRQHandler(&i2c1);
    HAL_I2C_ER_IRQHandler(&i2c1);
}
*/
void USB_IRQHandler(void) {
  extern PCD_HandleTypeDef hpcd_USB_FS;
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

/*! @} */
