#ifndef INA219_H
#define INA219_H

#include <hw_includes.h>

#include "result.h"

struct INA219 {
public:
  static constexpr uint8_t DEFAULT_ADDRESS = 0x40;

  enum Register {
    Configuration = 0x00,
    ShuntVoltage = 0x01,
    BusVoltage = 0x02,
    Power = 0x03,
    Current = 0x04,
    Calibration = 0x05
  };

  // see
  // https://github.com/adafruit/Adafruit_INA219/blob/master/Adafruit_INA219.cpp

  /**
   * VBUS_MAX             Максимальное измеряемое напряжение, В (32 или 16)
   * VSHUNT_MAX           Максимальное падение на шунте, мВ (320, 160, 80, 40)
   * RSHUNT               Сопротивлене шунта, Ом
   *
   * 1.  Максимальный ток
   * MaxPossible_I = VSHUNT_MAX / RSHUNT
   *
   * 2. Выбрать какой ток мы будем считать максимальным (меньше теоритического)
   *
   * 3. Цена одного бита для разной разрядности преобразования
   * MinimumLSB_15 = MaxExpected_I/(2^16 - 1)
   * MinimumLSB_12 = MaxExpected_I/(2^13 - 1)
   *
   * 4. Выбираем CurrentLSB
   * MinimumLSB_15 < CurrentLSB < MinimumLSB_12 (круглое)
   *
   * 5. Значение калибровочного регистра
   * Cal = int(0.04096 / (CurrentLSB * RSHUNT))
   *
   * 6. Цена бита в регистре мощности
   * PowerLSB = 20 * CurrentLSB
   *
   * 7. Проверяем
   * Max_Current = CurrentLSB * (2^16 - 1) (максимальный ток)
   *
   * If Max_Current > Max_Possible_I then
   *   Max_Current_Before_Overflow = MaxPossible_I
   * Else
   *   Max_Current_Before_Overflow = Max_Current
   * End If
   *
   * Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
   * If Max_ShuntVoltage >= VSHUNT_MAX
   *    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
   * Else
   *    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
   * End If
   *
   * 8. Максимальная мощность
   * MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
   */
  enum VoltageRange { MAX_16V, MAX_32V };

  INA219(I2C_HandleTypeDef &bus, uint8_t addr = DEFAULT_ADDRESS,
         uint32_t Timeout = HAL_MAX_DELAY);

  INA219(const INA219 &ref) = delete;
  INA219(INA219 &&ref);

  I2C_HandleTypeDef &getBus() { return *bus; }
  uint8_t getAddress() { return address; }

  Result<void, HAL_StatusTypeDef> start(const VoltageRange maxBusVoltage,
                                        float Shunt_Resistance_ohm = 0.1f,
                                        float max_current = 1.0);

  Result<int16_t, HAL_StatusTypeDef> shunt_voltage_raw_sync();
  Result<float, HAL_StatusTypeDef> shunt_voltage_V_sync();

  Result<uint16_t, HAL_StatusTypeDef> voltage_raw_sync();
  Result<float, HAL_StatusTypeDef> voltage_V_sync();

  Result<int16_t, HAL_StatusTypeDef> power_raw_sync();
  Result<float, HAL_StatusTypeDef> power_W_sync();

  Result<int16_t, HAL_StatusTypeDef> current_raw_sync();
  Result<float, HAL_StatusTypeDef> current_A_sync();

  Result<uint16_t, HAL_StatusTypeDef> getCalibrationValue_sync();

  void update();

  static int16_t convert_u16tos16(uint16_t v);
  static uint16_t voltage_raw(uint16_t regval);

  float power_from_raw(int16_t raw_value) const;
  float current_from_raw(int16_t raw_value) const;
  static float shunt_voltage_from_raw(int16_t sv);
  static float voltage_from_raw(uint16_t v);

private:
  template <typename Tc>
  auto readRawValue(const Register reg, const Tc &converter) {
    return read(reg).map(converter);
  }

  I2C_HandleTypeDef *bus;
  uint32_t Timeout;

  float currentMultiplier, powerMultiplier;

  uint8_t address;

  Result<uint16_t, HAL_StatusTypeDef> read(uint8_t Register) const;
  Result<void, HAL_StatusTypeDef> write(uint8_t Register, uint16_t value) const;

  void DMARead_raw_vals();
  void send_error();
  void send_result();

  void reset_i2c() const;
};

#endif // INA219_H
