/*!
 *  \file
 *  \brief fast-ampermeter main
 */

#include "BoardInit.h"
#include "Debug.h"

#include "ina219.h"

/*!
 * \brief fast-ampermeter entry point
 * \return Never
 */
int main(void) {

  InitBoard();

  I2C_HandleTypeDef i2c1{I2C1,
                         {400, 0, I2C_ADDRESSINGMODE_7BIT,
                          I2C_DUALADDRESS_DISABLE, 0, I2C_OA2_NOMASK,
                          I2C_GENERALCALL_DISABLE, I2C_NOSTRETCH_DISABLE}};

  HAL_I2C_Init(&i2c1);

  INA219 ina219{i2c1};

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

    HAL_Delay(100);
  }
}
