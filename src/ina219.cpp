#include <cmath>
#include <cstring>

#include "ina219.h"

enum Register {
  Configuration = 0x00,
  ShuntVoltage = 0x01,
  BusVoltage = 0x02,
  Power = 0x03,
  Current = 0x04,
  Calibration = 0x05
};

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

static int16_t u16tos16(uint16_t v) { return static_cast<int16_t>(v); }

static uint16_t calc_voltage(uint16_t regval) { return regval >> 3; }

static float shunt_voltage_from_raw(int16_t sv) {
  return fabsf(sv * 0.01f * 0.001f);
}

static float voltage_from_raw(uint16_t v) { return fabsf(v * 0.001f * 4); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INA219::INA219(I2C_HandleTypeDef &bus, uint8_t addr, uint32_t Timeout)
    : bus(bus), Timeout(Timeout), address(addr << 1) {}

Result<void, INA219::Error>
INA219::start(const INA219::VoltageRange maxBusVoltage,
              float Shunt_Resistance_ohm, float max_current) {
  float VShunt_MAX = max_current * Shunt_Resistance_ohm;

  if (VShunt_MAX > 0.320f) {
    return Err(Error());
  }

  float CurrentLSB = max_current / ((1u << 15) - 1);

  auto Cal = uint16_t(ceilf(magick / (CurrentLSB * Shunt_Resistance_ohm)));

  currentMultiplier = CurrentLSB;
  powerMultiplier = 20 * currentMultiplier;

  auto r = write(Calibration, Cal);
  if (r.isErr()) {
    return r;
  }

  uint16_t config{(maxBusVoltage == MAX_16V ? INA219_CONFIG_BVOLTAGERANGE_16V
                                            : INA219_CONFIG_BVOLTAGERANGE_32V) |
                  INA219_CONFIG_GAIN_8_320MV | INA219_CONFIG_BADCRES_12BIT |
                  INA219_CONFIG_SADCRES_12BIT_1S_532US |
                  INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS};

  return write(Configuration, config);
}

Result<int16_t, INA219::Error> INA219::shunt_voltage_raw() const {
  return read(ShuntVoltage).map(u16tos16);
}

Result<float, INA219::Error> INA219::shunt_voltage_V() const {
  return shunt_voltage_raw().map(shunt_voltage_from_raw);
}

Result<uint16_t, INA219::Error> INA219::voltage_raw() const {
  return read(BusVoltage).map(calc_voltage);
}

Result<float, INA219::Error> INA219::voltage_V() const {
  return voltage_raw().map(voltage_from_raw);
}

Result<int16_t, INA219::Error> INA219::power_raw() const {
  return read(Power).map(u16tos16);
}

Result<float, INA219::Error> INA219::power_W() const {
  auto p = power_raw();
  if (p.isErr()) {
    return Err(p.storage().get<Error>());
  }
  return Ok(p.storage().get<int16_t>() * powerMultiplier);
}

Result<int16_t, INA219::Error> INA219::current_raw() const {
  return read(Current).map(u16tos16);
}

Result<float, INA219::Error> INA219::current_A() const {
  auto I = current_raw();
  if (I.isErr()) {
    return Err(I.storage().get<Error>());
  }
  return Ok(I.storage().get<int16_t>() * currentMultiplier);
}

Result<uint16_t, INA219::Error> INA219::calibrationValue() const {
  return read(Calibration);
}

Result<uint16_t, INA219::Error> INA219::read(uint8_t Register) const {
  uint16tocharConvertor buf;

  if (HAL_I2C_Master_Transmit(&bus, address, &Register, 1, Timeout) != HAL_OK) {
    return Err(INA219::Error());
  }

  if (HAL_I2C_Master_Receive(&bus, address, buf.u8, std::size(buf.u8),
                             Timeout) != HAL_OK) {
    return Err(INA219::Error());
  }
  return Ok(buf.u16());
}

Result<void, INA219::Error> INA219::write(uint8_t Register,
                                          uint16_t value) const {
  uint8_t buf[sizeof(Register) + sizeof(value)]{Register};
  uint16tocharConvertor v{value};

  std::copy(v.u8, &v.u8[sizeof(v.u8)], &buf[1]);

  return (HAL_I2C_Master_Transmit(&bus, address, buf, sizeof(buf), Timeout) !=
          HAL_OK)
             ? Result<void, INA219::Error>(Err(INA219::Error()))
             : Ok();
}
