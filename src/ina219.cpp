#include <cmath>
#include <cstring>

#include "ina219.h"

/** bus voltage range values **/
enum {
  INA219_CONFIG_BVOLTAGERANGE_16V = (0x0000), // 0-16V Range
  INA219_CONFIG_BVOLTAGERANGE_32V = (0x2000), // 0-32V Range
};

/** mask for gain bits **/
#define INA219_CONFIG_GAIN_MASK (0x1800) // Gain Mask

/** values for gain bits **/
enum Gain {
  INA219_CONFIG_GAIN_1_40MV = (0x0000),  // Gain 1, 40mV Range
  INA219_CONFIG_GAIN_2_80MV = (0x0800),  // Gain 2, 80mV Range
  INA219_CONFIG_GAIN_4_160MV = (0x1000), // Gain 4, 160mV Range
  INA219_CONFIG_GAIN_8_320MV = (0x1800), // Gain 8, 320mV Range
};

/** mask for bus ADC resolution bits **/
#define INA219_CONFIG_BADCRES_MASK (0x0780)

/** values for bus ADC resolution **/
enum {
  INA219_CONFIG_BADCRES_9BIT = (0x0000),  // 9-bit bus res = 0..511
  INA219_CONFIG_BADCRES_10BIT = (0x0080), // 10-bit bus res = 0..1023
  INA219_CONFIG_BADCRES_11BIT = (0x0100), // 11-bit bus res = 0..2047
  INA219_CONFIG_BADCRES_12BIT = (0x0180), // 12-bit bus res = 0..4097
};

/** values for shunt ADC resolution **/
enum {
  INA219_CONFIG_SADCRES_9BIT_1S_84US = (0x0000),   // 1 x 9-bit shunt sample
  INA219_CONFIG_SADCRES_10BIT_1S_148US = (0x0008), // 1 x 10-bit shunt sample
  INA219_CONFIG_SADCRES_11BIT_1S_276US = (0x0010), // 1 x 11-bit shunt sample
  INA219_CONFIG_SADCRES_12BIT_1S_532US = (0x0018), // 1 x 12-bit shunt sample
  INA219_CONFIG_SADCRES_12BIT_2S_1060US =
      (0x0048), // 2 x 12-bit shunt samples averaged together
  INA219_CONFIG_SADCRES_12BIT_4S_2130US =
      (0x0050), // 4 x 12-bit shunt samples averaged together
  INA219_CONFIG_SADCRES_12BIT_8S_4260US =
      (0x0058), // 8 x 12-bit shunt samples averaged together
  INA219_CONFIG_SADCRES_12BIT_16S_8510US =
      (0x0060), // 16 x 12-bit shunt samples averaged together
  INA219_CONFIG_SADCRES_12BIT_32S_17MS =
      (0x0068), // 32 x 12-bit shunt samples averaged together
  INA219_CONFIG_SADCRES_12BIT_64S_34MS =
      (0x0070), // 64 x 12-bit shunt samples averaged together
  INA219_CONFIG_SADCRES_12BIT_128S_69MS =
      (0x0078), // 128 x 12-bit shunt samples averaged together
};

/** values for operating mode **/
enum {
  INA219_CONFIG_MODE_POWERDOWN = 0,
  INA219_CONFIG_MODE_SVOLT_TRIGGERED = 1,
  INA219_CONFIG_MODE_BVOLT_TRIGGERED = 2,
  INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED = 3,
  INA219_CONFIG_MODE_ADCOFF = 4,
  INA219_CONFIG_MODE_SVOLT_CONTINUOUS = 5,
  INA219_CONFIG_MODE_BVOLT_CONTINUOUS = 6,
  INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS = 7
};

struct uint16tocharConvertor {
  uint16tocharConvertor() = default;
  uint16tocharConvertor(uint16_t v) : u8{uint8_t(v >> 8), uint8_t(v)} {}

  uint16_t u16() const { return ((uint16_t)u8[0]) << 8 | u8[1]; }
  uint8_t u8[2];
};

static constexpr float magick = 0.04096f;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int16_t INA219::convert_u16tos16(uint16_t v) { return static_cast<int16_t>(v); }

uint16_t INA219::voltage_raw(uint16_t regval) { return regval >> 3; }

float INA219::shunt_voltage_from_raw(int16_t sv) { return sv * 0.01f * 0.001f; }

float INA219::voltage_from_raw(uint16_t v) { return fabsf(v * 0.001f * 4); }

float INA219::power_from_raw(int16_t raw_value) const {
  return raw_value * powerMultiplier;
}

float INA219::current_from_raw(int16_t raw_value) const {
  return raw_value * currentMultiplier;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INA219::INA219(I2C_HandleTypeDef &bus, uint8_t addr, uint32_t Timeout)
    : bus(&bus), Timeout(Timeout), address(addr << 1) {}

INA219::INA219(INA219 &&ref)
    : bus(ref.bus), Timeout(ref.Timeout),
      currentMultiplier(ref.currentMultiplier),
      powerMultiplier(ref.powerMultiplier), address(ref.address) {
  ref.bus = nullptr;
}

Result<void, HAL_StatusTypeDef>
INA219::start(const INA219::VoltageRange maxBusVoltage,
              float Shunt_Resistance_ohm, float max_current) {
  float VShunt_MAX = max_current * Shunt_Resistance_ohm;

  if (VShunt_MAX > 0.320f) {
    return Err(HAL_ERROR);
  }

  float CurrentLSB = max_current / ((1u << 15) - 1);

  auto Cal = uint16_t(ceilf(magick / (CurrentLSB * Shunt_Resistance_ohm)));

  currentMultiplier = CurrentLSB;
  powerMultiplier = 20 * currentMultiplier;

  auto res = write(Calibration, Cal);
  if (res.isErr()) {
    return res;
  }

  uint16_t config{
      uint8_t(maxBusVoltage == MAX_16V ? INA219_CONFIG_BVOLTAGERANGE_16V
                                       : INA219_CONFIG_BVOLTAGERANGE_32V) |
      uint8_t(INA219_CONFIG_GAIN_8_320MV | INA219_CONFIG_BADCRES_12BIT |
              INA219_CONFIG_SADCRES_12BIT_1S_532US |
              INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS)};

  return write(Configuration, config);
}

Result<int16_t, HAL_StatusTypeDef> INA219::shunt_voltage_raw_sync() {
  return readRawValue(ShuntVoltage, convert_u16tos16);
}

Result<float, HAL_StatusTypeDef> INA219::shunt_voltage_V_sync() {
  return shunt_voltage_raw_sync().map(shunt_voltage_from_raw);
}

Result<uint16_t, HAL_StatusTypeDef> INA219::voltage_raw_sync() {
  return readRawValue(BusVoltage, voltage_raw);
}

Result<float, HAL_StatusTypeDef> INA219::voltage_V_sync() {
  return voltage_raw_sync().map(voltage_from_raw);
}

Result<int16_t, HAL_StatusTypeDef> INA219::power_raw_sync() {
  return readRawValue(Power, convert_u16tos16);
}

Result<float, HAL_StatusTypeDef> INA219::power_W_sync() {
  auto p_raw = power_raw_sync();
  if (p_raw.isErr()) {
    return Err(p_raw.unwrapErr());
  }
  return Ok(power_from_raw(p_raw.unwrap()));
}

Result<int16_t, HAL_StatusTypeDef> INA219::current_raw_sync() {
  return readRawValue(Current, convert_u16tos16);
}

Result<float, HAL_StatusTypeDef> INA219::current_A_sync() {
  auto I = current_raw_sync();
  if (I.isErr()) {
    return Err(I.unwrapErr());
  }
  return Ok(current_from_raw(I.unwrap()));
}

Result<uint16_t, HAL_StatusTypeDef> INA219::getCalibrationValue_sync() {
  return read(Calibration);
}

Result<uint16_t, HAL_StatusTypeDef> INA219::read(uint8_t Register) const {
  uint16tocharConvertor buf;

  HAL_StatusTypeDef res;

  res = HAL_I2C_Master_Transmit(bus, address, &Register, 1, Timeout);
  if (res != HAL_OK) {
    reset_i2c();
    return Err(res);
  }

  res =
      HAL_I2C_Master_Receive(bus, address, buf.u8, std::size(buf.u8), Timeout);
  if (res != HAL_OK) {
    reset_i2c();
    return Err(res);
  }
  return Ok(buf.u16());
}

Result<void, HAL_StatusTypeDef> INA219::write(uint8_t Register,
                                              uint16_t value) const {
  uint8_t buf[sizeof(Register) + sizeof(value)]{Register};
  uint16tocharConvertor v{value};

  std::copy(v.u8, &v.u8[sizeof(v.u8)], &buf[1]);

  auto res = HAL_I2C_Master_Transmit(bus, address, buf, sizeof(buf), Timeout);
  if (res != HAL_OK) {
    reset_i2c();
    return Err(res);
  } else {
    return Ok();
  }
}

void INA219::reset_i2c() const {
  if (bus->Instance == I2C1) {
    __HAL_RCC_I2C1_FORCE_RESET();
    HAL_Delay(1);
    __HAL_RCC_I2C1_RELEASE_RESET();
  }
#ifdef __HAL_RCC_I2C2_FORCE_RESET
  if (bus.Instance == I2C2) {
    __HAL_RCC_I2C2_FORCE_RESET();
    HAL_Delay(1);
    __HAL_RCC_I2C2_RELEASE_RESET();
  }
#endif
  HAL_I2C_Init(bus);
}
