/*!
 *  \file MCU-specific callbacks. Its overrides HAL's default weak empty
 * realizations.
 */

#include "hw_includes.h"

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
  if (hi2c->Instance == I2C1) {
    __HAL_RCC_GPIOB_CLK_ENABLE();

    __HAL_RCC_I2C1_CLK_ENABLE();

    GPIO_InitTypeDef gpiob{GPIO_PIN_6 | GPIO_PIN_7, GPIO_MODE_AF_OD,
                           GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, GPIO_AF4_I2C1};
    HAL_GPIO_Init(GPIOB, &gpiob);
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c) {
  if (hi2c->Instance == I2C1) {
    __HAL_RCC_I2C1_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6 | GPIO_PIN_7);
  }
}
