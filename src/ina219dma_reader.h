#ifndef INA219DMA_READER_H
#define INA219DMA_READER_H

#include <functional>

#include "ina219.h"

class INA219DMA_Reader {
public:
  enum State {
    INITIALISING = 0,
    DEAD = -1,
    READY = 1,
    Send_ADDR = 2,
    Read_REG = 3,
    BUSY = 0xa0,
    END = 0xff
  };

  using raw_data_t = uint16_t;

  union RawValues {
    struct {
      uint16_t ShuntVoltage_cell;
      uint16_t BusVoltage_cell;
      uint16_t Power_cell;
      uint16_t Current_cell;

      uint16_t ShuntVoltage_cell_as_LE() const {
        return toLE(ShuntVoltage_cell);
      }
      uint16_t BusVoltage_cell_as_LE() const { return toLE(BusVoltage_cell); }
      uint16_t Power_cell_as_LE() const { return toLE(Power_cell); }
      uint16_t Current_cell_as_LE() const { return toLE(Current_cell); }

    private:
      static uint16_t toLE(const uint16_t v) { return (v << 8) | (v >> 8); }
    } _s;

    uint8_t raw[sizeof(_s)];
    raw_data_t raw_vals[sizeof(_s) / sizeof(raw_data_t)];
  };

  struct Values {
    float ShuntVoltage, BusVoltage, Power, Current;
  };

  using callback_t = std::function<void(Result<Values, HAL_StatusTypeDef>)>;
  using DMA_inicialisator_t =
      std::function<Result<void, HAL_StatusTypeDef>(I2C_HandleTypeDef &)>;

  INA219DMA_Reader(INA219 &&_ina219, const DMA_inicialisator_t &initialisator);

  HAL_StatusTypeDef update(callback_t callback);

  void pool();

  Result<Values, HAL_StatusTypeDef> format_result() const;

private:
  INA219 ina219;
  volatile State state;
  size_t position;
  HAL_StatusTypeDef errorStatus;

  callback_t callback;

  void start();

  void Start_sending_address();
  void Start_reading_data();

  void reset_callbacks();

  static void DMA_TXtransferComplead(I2C_HandleTypeDef *i2c);
  static void DMA_RXtransferComplead(I2C_HandleTypeDef *i2c);
  static void DMAError(I2C_HandleTypeDef *i2c);
};

#endif // INA219DMA_READER_H
