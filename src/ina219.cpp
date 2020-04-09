#include "ina219.h"

enum Register {
  // Configuration = 0x00,
  ShuntVoltage = 0x01,
  BusVoltage = 0x02,
  Power = 0x03,
  Current = 0x04,
  Calibration = 0x05
};

static int16_t u16tos16(uint16_t v) { return static_cast<int16_t>(v); }

static uint16_t calc_voltage(uint16_t regval) { return (regval >> 3) * 4; }

INA219::INA219(I2C_HandleTypeDef &bus, uint8_t addr)
    : bus(bus), address(addr) {}

Result<void, INA219::Error>
INA219::calibrate(const std::vector<uint8_t> &values) {
  uint8_t data_to_tx[1 + values.size()]{Register::Calibration};

  std::copy(values.cbegin(), values.cend(), &data_to_tx[1]);

  return HAL_I2C_Master_Transmit(&bus, address, data_to_tx, 1 + values.size(),
                                 10) != HAL_OK
             ? Result<void, INA219::Error>(INA219::Error())
             : Ok();
}

Result<int16_t, INA219::Error> INA219::shunt_voltage() {
  return read(ShuntVoltage).map(u16tos16);
}

Result<uint16_t, INA219::Error> INA219::voltage() {

  return read(BusVoltage).map(calc_voltage);
}

Result<int16_t, INA219::Error> INA219::power() {
  return read(Power).map(u16tos16);
}

Result<int16_t, INA219::Error> INA219::current() {
  return read(Current).map(u16tos16);
}

Result<uint16_t, INA219::Error> INA219::read(uint8_t Register) {
  union {
    uint8_t buf[2];
    uint16_t res;
  } buf;

  if (HAL_I2C_Master_Transmit(&bus, address, &Register, 1, 10) != HAL_OK) {
    return Err(INA219::Error());
  }

  if (HAL_I2C_Master_Receive(&bus, address, buf.buf, std::size(buf.buf), 10) !=
      HAL_OK) {
    return Err(INA219::Error());
  }
  return Ok(buf.res);
}
