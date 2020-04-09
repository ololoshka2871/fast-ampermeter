/*!
 *  \file
 *  \brief fast-ampermeter main
 */

#include <cstring>
#include <limits>
#include <regex>

#include "BoardInit.h"
#include "Debug.h"
#include "softreset.h"

#include "ina219.h"

/*!
 * \brief fast-ampermeter entry point
 * \return Never
 */
int main(void) {
  InitBoard();

  I2C_HandleTypeDef i2c1{I2C1,
                         {400000, I2C_DUTYCYCLE_16_9, 0,
                          I2C_ADDRESSINGMODE_7BIT, I2C_DUALADDRESS_DISABLE, 0,
                          I2C_GENERALCALL_DISABLE, I2C_NOSTRETCH_DISABLE}};

  HAL_I2C_Init(&i2c1);

  INA219 ina219{i2c1};

  while (true) {
    auto current = ina219.current();
    if (current.isOk()) {
      DEBUG_PRINTF("Current = %d", current.unwrap());
    } else {
      DEBUG_MSG("Failed to read current");
    }

    HAL_Delay(100);
  }
}
