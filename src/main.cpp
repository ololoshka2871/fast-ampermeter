/*!
 *  \file
 *  \brief fast-ampermeter main
 */

#include <cassert>

#include "hw_includes.h"

#include "result.h"

#include "BoardInit.h"
#include "Debug.h"

#include "ina219.h"
#include "ina219dma_reader.h"

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

  INA219 ina219{*init_I2C().unwrap(), INA219::DEFAULT_ADDRESS, 10};
  ina219.start(INA219::MAX_16V, 0.4f, 0.3f);

  INA219DMA_Reader reader(std::move(ina219), init_DMA);

  volatile float I, V;

  auto res = reader.update(
      [&I, &V](Result<INA219DMA_Reader::Values, HAL_StatusTypeDef> r) {
        auto _r = r.unwrap();
        I = _r.Current;
        V = _r.BusVoltage;
      });
  assert(res == HAL_OK);

  while (true) {
    reader.pool();
    //__asm__("wfi");
  }
}
