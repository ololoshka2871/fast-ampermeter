#include "ina219dma_reader.h"

static constexpr auto ValuesCount =
    sizeof(INA219DMA_Reader::RawValues) / sizeof(INA219DMA_Reader::raw_data_t);

static constexpr uint8_t addresses_to_read_from[ValuesCount] = {
    INA219::ShuntVoltage, INA219::BusVoltage, INA219::Power, INA219::Current};

static INA219DMA_Reader *__this = nullptr;

static INA219DMA_Reader::RawValues storage;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void INA219DMA_Reader::DMA_TXtransferComplead(I2C_HandleTypeDef *i2c) {
  __this->state = Read_REG;
}

void INA219DMA_Reader::DMA_RXtransferComplead(I2C_HandleTypeDef *i2c) {
  __this->state = (++__this->position == ValuesCount) ? END : Send_ADDR;
}

void INA219DMA_Reader::DMAError(I2C_HandleTypeDef *i2c) {
  __this->errorStatus = HAL_ERROR;
  __this->state = END;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void INA219DMA_Reader::Start_sending_address() {
  auto &bus = ina219.getBus();
  auto address = ina219.getAddress();

  const uint8_t *regAddress = &addresses_to_read_from[position];

  auto res = HAL_I2C_Master_Seq_Transmit_DMA(
      &bus, address, const_cast<uint8_t *>(regAddress),
      sizeof(addresses_to_read_from[0]), I2C_FIRST_FRAME | I2C_LAST_FRAME);

  if (res != HAL_OK) {
    state = END;
    errorStatus = res;
    reset_callbacks();
    pool();
    return;
  }

  bus.MasterTxCpltCallback = DMA_TXtransferComplead;
  bus.MasterRxCpltCallback = nullptr;

  state = BUSY;
}

void INA219DMA_Reader::Start_reading_data() {
  auto &bus = ina219.getBus();
  auto address = ina219.getAddress();

  auto write_to = reinterpret_cast<uint8_t *>(&storage.raw_vals[position]);

  auto res = HAL_I2C_Master_Seq_Receive_DMA(&bus, address, write_to,
                                            sizeof(raw_data_t),
                                            I2C_FIRST_FRAME | I2C_LAST_FRAME);

  if (res != HAL_OK) {
    state = END;
    errorStatus = res;
    reset_callbacks();
    pool();
    return;
  }

  bus.MasterTxCpltCallback = nullptr;
  bus.MasterRxCpltCallback = DMA_RXtransferComplead;

  state = BUSY;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void INA219DMA_Reader::reset_callbacks() {
  auto &bus = ina219.getBus();

  bus.MasterRxCpltCallback = nullptr;
  bus.MasterTxCpltCallback = nullptr;
  bus.ErrorCallback = nullptr;
}

INA219DMA_Reader::INA219DMA_Reader(INA219 &&_ina219,
                                   const DMA_inicialisator_t &initialisator)
    : ina219(std::move(_ina219)), state{INITIALISING} {
  state = initialisator(ina219.getBus()).isErr() ? DEAD : READY;
  __this = this;
}

HAL_StatusTypeDef
INA219DMA_Reader::update(INA219DMA_Reader::callback_t callback) {
  if (state != INA219DMA_Reader::READY) {
    return HAL_BUSY;
  }
  this->callback = callback;
  start();
  pool();
  return HAL_OK;
}

void INA219DMA_Reader::pool() {
  switch (state) {
  case Send_ADDR:
    Start_sending_address();
    break;
  case Read_REG:
    Start_reading_data();
    break;
  case END:
    state = READY;
    if (callback) {
      callback(format_result(), *this);
    }
    break;
  default:
    break;
  }
}

Result<INA219DMA_Reader::Values, HAL_StatusTypeDef>
INA219DMA_Reader::format_result() const {
  if (errorStatus != HAL_OK) {
    return Err(errorStatus);
  } else {
    return Ok(Values(storage, ina219)
    /*{
        INA219::shunt_voltage_from_raw(
            INA219::convert_u16tos16(storage._s.ShuntVoltage_cell_as_LE())),
        INA219::voltage_from_raw(
            INA219::voltage_raw(storage._s.BusVoltage_cell_as_LE())),
        ina219.power_from_raw(
            INA219::convert_u16tos16(storage._s.Power_cell_as_LE())),
        ina219.current_from_raw(
            INA219::convert_u16tos16(storage._s.Current_cell_as_LE())),
    }*/);
  }
}

void INA219DMA_Reader::start() {
  state = Send_ADDR;
  position = 0;
  errorStatus = HAL_OK;

  ina219.getBus().ErrorCallback = DMAError;
}

INA219DMA_Reader::Values::Values(const INA219DMA_Reader::RawValues &raw_values,
                                 const INA219 &device)
    : raw_values(raw_values), device(device) {}

float INA219DMA_Reader::Values::ShuntVoltage() const {
  return INA219::shunt_voltage_from_raw(
      INA219::convert_u16tos16(raw_values._s.ShuntVoltage_cell_as_LE()));
}

float INA219DMA_Reader::Values::BusVoltage() const {
  return INA219::voltage_from_raw(
      INA219::voltage_raw(raw_values._s.BusVoltage_cell_as_LE()));
}

float INA219DMA_Reader::Values::Power() const {
  return device.power_from_raw(
      INA219::convert_u16tos16(raw_values._s.Power_cell_as_LE()));
}

float INA219DMA_Reader::Values::Current() const {
  return device.current_from_raw(
      INA219::convert_u16tos16(raw_values._s.Current_cell_as_LE()));
}
