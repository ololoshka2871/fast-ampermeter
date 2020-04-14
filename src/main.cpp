/*!
 *  \file
 *  \brief fast-ampermeter main
 */

#include "hw_includes.h"

#include "result.h"

#include "BoardInit.h"
#include "Debug.h"

#include "ina219.h"

static Result<I2C_HandleTypeDef, HAL_StatusTypeDef> init_I2C() {
  RCC_PeriphCLKInitTypeDef PeriphClkInit{
      RCC_PERIPHCLK_USB | RCC_PERIPHCLK_I2C1,
      0,                        // RTC
      0,                        // USART1
      RCC_I2C1CLKSOURCE_SYSCLK, // i2c1
      0,                        // HDMI CEC
      RCC_USBCLKSOURCE_HSI48,   // USB
  };

  HAL_StatusTypeDef res;

  res = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
  if (res != HAL_OK) {
    return Err(res);
  }

  I2C_HandleTypeDef i2c1{I2C1,
                         {0x0010091A, // сгенерировано кубом
                          0, I2C_ADDRESSINGMODE_7BIT, I2C_DUALADDRESS_DISABLE,
                          0, I2C_OA2_NOMASK, I2C_GENERALCALL_DISABLE,
                          I2C_NOSTRETCH_DISABLE}};

  res = HAL_I2C_Init(&i2c1);
  if (res != HAL_OK) {
    return Err(res);
  }

  return Ok(std::move(i2c1));
}

/*!
 * \brief fast-ampermeter entry point
 * \return Never
 */
int main(void) {

  InitBoard();

  auto i2c1 = init_I2C().unwrap();

  INA219 ina219{i2c1, INA219::DEFAULT_ADDRESS, 10};

  ina219.start(INA219::MAX_16V, 0.4f, 0.3f);

  volatile float I, V, c;

  while (true) {
    auto current = ina219.current_A();
    if (current.isOk()) {
      I = current.unwrap();
    }

    auto volatage = ina219.voltage_V();
    if (volatage.isOk()) {
      V = volatage.unwrap();
    }

    auto _c = ina219.calibrationValue();
    if (_c.isOk()) {
      c = _c.unwrap();
    }

    // HAL_Delay(100);
  }
}
