#ifndef INA219_H
#define INA219_H

#include <hw_includes.h>

#include <vector>

#include "result.h"

struct INA219 {
  static constexpr uint8_t DEFAULT_ADDRESS = 0x41;

  struct Error {};

  INA219(I2C_HandleTypeDef &bus, uint8_t addr = DEFAULT_ADDRESS);

  Result<void, Error> calibrate(const std::vector<uint8_t> &values);

  Result<int16_t, Error> shunt_voltage();

  Result<uint16_t, Error> voltage();

  Result<int16_t, Error> power();

  Result<int16_t, Error> current();

private:
  I2C_HandleTypeDef &bus;
  uint8_t address;

  Result<uint16_t, Error> read(uint8_t Register);
};

#endif // INA219_H
