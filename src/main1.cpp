
#include "BoardInit.h"

#include "usb_device.h"
#include "usbd_custom_hid_if.h"

#include "ina219.h"
#include "ina219dma_reader.h"

extern "C" int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

static void
result_read_cb(Result<INA219DMA_Reader::Values, HAL_StatusTypeDef> r,
               INA219DMA_Reader &reader) {
  struct res {
    res(float ShuntVoltage, float BusVoltage, float Power, float Current)
        : ShuntVoltage{ShuntVoltage},
          BusVoltage{BusVoltage}, Power{Power}, Current{Current} {}

    float ShuntVoltage, BusVoltage, Power, Current;
  };

  if (r.isOk()) {
    auto uwr = r.unwrap();
    res res{uwr.ShuntVoltage(), uwr.BusVoltage(), uwr.Power(), uwr.Current()};

    USBD_CUSTOM_HID_SendReport_FS(reinterpret_cast<uint8_t *>(&res),
                                  sizeof(res));
  }

  reader.update(result_read_cb);
}

/*!
 * \brief fast-ampermeter entry point
 * \return Never
 */
int main(void) {
  InitBoard();

  MX_USB_DEVICE_Init();

  INA219 ina219{*init_I2C().unwrap(), INA219::DEFAULT_ADDRESS, 10};
  ina219.start(INA219::MAX_16V, 0.4f, 0.3f);

  INA219DMA_Reader reader(std::move(ina219), init_DMA);

  reader.update(result_read_cb);

  while (1) {
    reader.pool();
    __asm__("wfi");
  }
}
