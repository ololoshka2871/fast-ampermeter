/*!
 *  \file
 *  \brief fast-ampermeter main
 */

#include <cassert>

#include "hw_includes.h"

#include "BoardInit.h"
#include "Debug.h"

#include "ina219.h"
#include "ina219dma_reader.h"

#include "cdc_acm.h"

#include "rxmessagereader.h"
#include "txmessagewriter.h"

#include "history.h"

#include "protocol.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct HistoryWriterConfig {
  int64_t start;
  size_t count;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static History<HISTORY_SIZE> history;

static DMA_HandleTypeDef hdma_tx{
    DMA1_Channel2,
    {DMA_MEMORY_TO_PERIPH, DMA_PINC_DISABLE, DMA_MINC_ENABLE,
     DMA_PDATAALIGN_BYTE, DMA_MDATAALIGN_BYTE, DMA_NORMAL, DMA_PRIORITY_LOW}};

static DMA_HandleTypeDef hdma_rx{
    DMA1_Channel3,
    {DMA_PERIPH_TO_MEMORY, DMA_PINC_DISABLE, DMA_MINC_ENABLE,
     DMA_PDATAALIGN_BYTE, DMA_MDATAALIGN_BYTE, DMA_NORMAL, DMA_PRIORITY_HIGH}};

I2C_HandleTypeDef i2c1{I2C1,
                       {0x2010091A, // сгенерировано кубом
                        0, I2C_ADDRESSINGMODE_7BIT, I2C_DUALADDRESS_DISABLE, 0,
                        I2C_OA2_NOMASK, I2C_GENERALCALL_DISABLE,
                        I2C_NOSTRETCH_DISABLE}};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
result_read_cb(Result<INA219DMA_Reader::Values, HAL_StatusTypeDef> r,
               INA219DMA_Reader &reader) {
  static int64_t counter = 0;

  if (r.isOk()) {
    auto uwr = r.unwrap();
    history.add(counter, uwr.BusVoltage(), uwr.Current());
    ++counter;
  }

  reader.update(result_read_cb);
}

template <typename TxMessage> static void writeLastMeasure(TxMessage &resp) {
  const auto lastMeasure = history.getLastMeasure();
  resp.number = lastMeasure.first;
  resp.voltage = lastMeasure.second->voltage;
  resp.current = lastMeasure.second->current;
}

template <typename Treq, typename TxMessage, typename Thwc>
static void writeMeasureHistory(const Treq &req, TxMessage &resp, Thwc &hwc) {
  auto history_start = history.start();
  auto history_elements = history.elements();

  hwc.start = req.has_from_element ? std::max(history_start, req.from_element)
                                   : history_start;

  hwc.count =
      req.has_count ? std::min(history_elements, req.count) : history_elements;

  resp.HistoryElements.arg = &hwc;
  // колбэк ДОЛЖЕН зависеть ТОЛЬКО от arg
  resp.HistoryElements.funcs.encode =
      [](pb_ostream_t *stream, const pb_field_t *field, void *const *arg) {
        auto config = *reinterpret_cast<const HistoryWriterConfig *>(*arg);

        for (auto N = config.start; N < config.start + config.count; ++N) {
          auto res = history.read(N);
          ru_sktbelpa_fast_freqmeter_SingleMeasure item{N, res.voltage,
                                                        res.current};
          if (!pb_encode_tag_for_field(stream, field)) {
            return false;
          }
          if (!pb_encode_submessage(stream, field->submsg_desc, &item)) {
            return false;
          }
        }
        return true;
      };
}

template <typename RxMessage, typename TxMessage, typename Thwc>
static void process_message(const RxMessage &req, TxMessage &resp, Thwc &hwc) {
  resp.id = req.id;
  resp.deviceID = ru_sktbelpa_fast_freqmeter_INFO_FAST_AMPERMETER_ID;
  resp.protocolVersion = ru_sktbelpa_fast_freqmeter_INFO_PROTOCOL_VERSION;
  resp.Global_status = ru_sktbelpa_fast_freqmeter_STATUS_OK;

  // Т.К resp не перенинициализируется с прошлого раза, то нужно вручную
  // обновлять флаги has_*

  if ((resp.has_lastMeasure = req.has_lastMeasureRequest)) {
    writeLastMeasure(resp.lastMeasure);
  }

  if ((resp.has_measureHistory = req.has_getMeasureHistory)) {
    writeMeasureHistory(req.getMeasureHistory, resp.measureHistory, hwc);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static Result<void, HAL_StatusTypeDef> init_DMA(I2C_HandleTypeDef &i2c) {
  HAL_StatusTypeDef res;

  __HAL_RCC_DMA1_CLK_ENABLE();

  res = HAL_DMA_Init(&hdma_tx);
  if (res != HAL_OK) {
    return Err(res);
  }

  /* Associate the initialized DMA handle to the the I2C handle */
  __HAL_LINKDMA(&i2c, hdmatx, hdma_tx);

  res = HAL_DMA_Init(&hdma_rx);
  if (res != HAL_OK) {
    return Err(res);
  }

  /* Associate the initialized DMA handle to the the I2C handle */
  __HAL_LINKDMA(&i2c, hdmarx, hdma_rx);

  /*##-5- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (I2Cx_TX and
   * I2Cx_RX) */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

  /*##-6- Configure the NVIC for I2C ########################################*/
  /* NVIC for I2Cx */
  HAL_NVIC_SetPriority(I2C1_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(I2C1_IRQn);

  return Ok();
}

static Result<I2C_HandleTypeDef *, HAL_StatusTypeDef> init_I2C() {
  RCC_PeriphCLKInitTypeDef PeriphClkInit{
      RCC_PERIPHCLK_I2C1,
      0,                        // RTC
      0,                        // USART1
      RCC_I2C1CLKSOURCE_SYSCLK, // i2c1
      0,                        // HDMI CEC
      0,                        // USB
  };

  HAL_StatusTypeDef res;

  res = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
  if (res != HAL_OK) {
    return Err(res);
  }

  res = HAL_I2C_Init(&i2c1);
  if (res != HAL_OK) {
    return Err(res);
  }

  return Ok(&i2c1);
}

/*!
 * \brief fast-ampermeter entry point
 * \return Never
 */
int main(void) {
  InitBoard();

  CDC_ACM::init();

  INA219 ina219{*init_I2C().unwrap(), INA219::DEFAULT_ADDRESS, 10};
  ina219.start(INA219::MAX_16V, 0.4f, 0.3f);

  INA219DMA_Reader reader(std::move(ina219), init_DMA);

  auto res = reader.update(result_read_cb);
  assert(res == HAL_OK);

  auto waiter = [&reader]() {
    reader.pool();
    __asm__("wfi");
  };

  ru_sktbelpa_fast_freqmeter_Request req{};
  ru_sktbelpa_fast_freqmeter_Response resp{};

  RxMessageReader cmd_reader;
  TxMessageWriter resp_writer;
  while (true) {
    HistoryWriterConfig historyWriterConfig;

    cmd_reader.read(req, ru_sktbelpa_fast_freqmeter_Request_fields,
                    ru_sktbelpa_fast_freqmeter_INFO_MAGICK, waiter);
    process_message<ru_sktbelpa_fast_freqmeter_Request,
                    ru_sktbelpa_fast_freqmeter_Response>(req, resp,
                                                         historyWriterConfig);
    resp_writer.write(resp, ru_sktbelpa_fast_freqmeter_Response_fields,
                      ru_sktbelpa_fast_freqmeter_INFO_MAGICK, waiter);
  }
}
