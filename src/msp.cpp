/*!
 *  \file MCU-specific callbacks. Its overrides HAL's default weak empty
 * realizations.
 */

#include "hw_includes.h"

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
  if (hi2c->Instance == I2C1) {

#if defined(STM32F0411xB)
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef gpiob{GPIO_PIN_6 | GPIO_PIN_7, GPIO_MODE_AF_OD,
                           GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, GPIO_AF4_I2C1};
    HAL_GPIO_Init(GPIOB, &gpiob);
#elif defined(STM32F042x6)
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef gpiob{GPIO_PIN_10 | GPIO_PIN_11, GPIO_MODE_AF_OD,
                           GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, GPIO_AF1_I2C1};
    HAL_GPIO_Init(GPIOB, &gpiob);
#endif

    __HAL_RCC_I2C1_CLK_ENABLE();
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c) {
  if (hi2c->Instance == I2C1) {
    __HAL_RCC_I2C1_CLK_DISABLE();
#if defined(STM32F0411xB)
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6 | GPIO_PIN_7);
#elif defined(STM32F042x6)
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10 | GPIO_PIN_11);
#endif
  }
}
