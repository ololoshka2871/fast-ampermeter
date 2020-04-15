#ifndef INA219_H
#define INA219_H

#include <hw_includes.h>

#include "result.h"

struct INA219 {
  static constexpr uint8_t DEFAULT_ADDRESS = 0x40;

  struct Error {};

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

  Result<void, Error> start(const VoltageRange maxBusVoltage,
                            float Shunt_Resistance_ohm = 0.1f,
                            float max_current = 1.0);

  Result<int16_t, Error> shunt_voltage_raw() const;
  Result<float, Error> shunt_voltage_V() const;

  Result<uint16_t, Error> voltage_raw() const;
  Result<float, Error> voltage_V() const;

  Result<int16_t, Error> power_raw() const;
  Result<float, Error> power_W() const;

  Result<int16_t, Error> current_raw() const;
  Result<float, Error> current_A() const;

  Result<uint16_t, Error> calibrationValue() const;

  Result<void, INA219::Error> StartDMAreadAllTo();

private:
  I2C_HandleTypeDef &bus;
  uint32_t Timeout;

  float currentMultiplier, powerMultiplier;

  uint16_t raw_data[4];

  static INA219 *__this;

  uint8_t address;

  Result<uint16_t, Error> read(uint8_t Register) const;
  Result<void, Error> write(uint8_t Register, uint16_t value) const;

  void DMARead_raw_vals();
  void send_error();
  void send_result();

  void reset_i2c() const;

  static void DMA_TXtransferComplead(I2C_HandleTypeDef *i2c);
  static void DMA_RXtransferComplead(I2C_HandleTypeDef *i2c);
  static void DMAError(I2C_HandleTypeDef *i2c);
};

#endif // INA219_H
