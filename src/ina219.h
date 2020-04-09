#ifndef INA219_H
#define INA219_H

#include <hw_includes.h>

#include <vector>

#include "result.h"

struct INA219 {
  static constexpr uint8_t DEFAULT_ADDRESS = 0x40;

  struct Error {};

  // see
  // https://github.com/adafruit/Adafruit_INA219/blob/master/Adafruit_INA219.cpp
  enum Mode { MODE_32V1A = 10240 };

  INA219(I2C_HandleTypeDef &bus, uint8_t addr = DEFAULT_ADDRESS,
         uint32_t Timeout = HAL_MAX_DELAY);

  Result<void, Error> start(const Mode mode);

  Result<int16_t, Error> shunt_voltage();

  Result<uint16_t, Error> voltage();

  Result<int16_t, Error> power();

  Result<int16_t, Error> current();

private:
  I2C_HandleTypeDef &bus;
  uint32_t Timeout;
  uint8_t address;

  Result<uint16_t, Error> read(uint8_t Register);
  Result<void, Error> write(uint8_t Register, uint16_t value);
};

#endif // INA219_H
