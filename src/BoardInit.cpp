/*!
 * \file
 * \brief Initialization of base MCU peripherals
 */

#include <array>
#include <cstring>

#include "hw_includes.h"

/*!
 * \ingroup BootloaderHW
 *  @{
 */

#define USB_FREQ 48000000L

// Need c++14! (not working as planned!)

/// Class-container of multiplier and divider for PLL
struct sDiv_Mul {
  /// Coded to MCU-specific values divider and multiplier
  uint32_t mul, div;

  /// Empty method to raise compile-time error then divider or multiplier
  /// incorrect
  void error(bool) {}

  /*! Constructor. Set and check coded to MCU-specific values divider and
   * multiplier
   *
   * Raise compile-time error if values out of range
   */
  constexpr sDiv_Mul(int32_t m = -1, int32_t d = -1)
      : mul(static_cast<uint32_t>(m)), div(static_cast<uint32_t>(d)) {
    if (
#if defined(STM32F0)
        (m < RCC_PLL_MUL2) || (d < RCC_PREDIV_DIV1) || (m > RCC_PLL_MUL16) ||
        (d > RCC_PREDIV_DIV16)
#elif defined(STM32F1) || defined(STM32F3)
        (m < static_cast<int32_t>(RCC_PLL_MUL2)) ||
        (d < static_cast<int32_t>(RCC_HSE_PREDIV_DIV1)) ||
        (m > static_cast<int32_t>(RCC_PLL_MUL16)) ||
#if defined(STM32F105xC) || defined(STM32F107xC) || defined(STM32F100xB) ||    \
    defined(STM32F100xE) || defined(STM32F3)
        (d > static_cast<int32_t>(RCC_HSE_PREDIV_DIV16))
#else
        (d > static_cast<int32_t>(RCC_HSE_PREDIV_DIV2))
#endif

#elif defined(STM32L4)
        // no macro for 8 and 86 defined
        (m < 8) || (d < static_cast<int32_t>(RCC_PLLR_DIV2)) || (m > 86) ||
        (d > static_cast<int32_t>(RCC_PLLR_DIV8)
#else
        false
#endif
    ) {
      error(false);
    }
  }
};

struct sPllconfigF4 {
  /// Coded to MCU-specific values divider and multiplier
  uint32_t p_div, mul, P, Q;

  /// Empty method to raise compile-time error then divider or multiplier
  /// incorrect
  void error(bool) {}

  constexpr sPllconfigF4(uint32_t p_div, uint32_t mul = 0, uint32_t P = 0,
                         uint32_t Q = 0)
      : p_div(p_div), mul(mul), P(P), Q(Q) {
    if (p_div == 0) {
      error(false);
    }
  }
};

/*!
 * \brief Compile-time calculation of PLL divider and
 * multiplier
 *
 * Calculates proper value of PLL divider and multiplier to achieve CPU clock
 * from HSE value
 * \param div_mul value of Target_cpu_clock/HSE_value
 * \return struct sDiv_Mul, what contains codded PLL divider and
 * multiplier or compile-time error
 */
constexpr sDiv_Mul calc_pll_div_mul(const uint32_t div_mul) {
#if defined(STM32F0)
  const uint32_t muls[] = {RCC_PLL_MUL2,  RCC_PLL_MUL3,  RCC_PLL_MUL4,
                           RCC_PLL_MUL5,  RCC_PLL_MUL6,  RCC_PLL_MUL7,
                           RCC_PLL_MUL8,  RCC_PLL_MUL9,  RCC_PLL_MUL10,
                           RCC_PLL_MUL11, RCC_PLL_MUL12, RCC_PLL_MUL13,
                           RCC_PLL_MUL14, RCC_PLL_MUL15, RCC_PLL_MUL16};
  const uint32_t divs[] = {
      RCC_PREDIV_DIV1,  RCC_PREDIV_DIV2,  RCC_PREDIV_DIV3,  RCC_PREDIV_DIV4,
      RCC_PREDIV_DIV5,  RCC_PREDIV_DIV6,  RCC_PREDIV_DIV7,  RCC_PREDIV_DIV8,
      RCC_PREDIV_DIV9,  RCC_PREDIV_DIV10, RCC_PREDIV_DIV11, RCC_PREDIV_DIV12,
      RCC_PREDIV_DIV13, RCC_PREDIV_DIV14, RCC_PREDIV_DIV15, RCC_PREDIV_DIV16};
  for (uint32_t mul = 2; mul < sizeof(muls) / sizeof(uint32_t) + 2; ++mul) {
    for (uint32_t div = 1; div < sizeof(divs) / sizeof(uint32_t) + 1; ++div) {
      if (mul / div == div_mul) {
        return sDiv_Mul(muls[mul - 2], divs[div - 1]);
      }
    }
  }
#elif defined(STM32F1) || defined(STM32F3)
  const uint32_t muls[] = {RCC_PLL_MUL2,  RCC_PLL_MUL3,  RCC_PLL_MUL4,
                           RCC_PLL_MUL5,  RCC_PLL_MUL6,  RCC_PLL_MUL7,
                           RCC_PLL_MUL8,  RCC_PLL_MUL9,  RCC_PLL_MUL10,
                           RCC_PLL_MUL11, RCC_PLL_MUL12, RCC_PLL_MUL13,
                           RCC_PLL_MUL14, RCC_PLL_MUL15, RCC_PLL_MUL16};
  const uint32_t divs[] = {
    RCC_HSE_PREDIV_DIV1,
    RCC_HSE_PREDIV_DIV2,
#if defined(STM32F105xC) || defined(STM32F107xC) || defined(STM32F100xB) ||    \
    defined(STM32F100xE) || defined(STM32F3)
    RCC_HSE_PREDIV_DIV3,
    RCC_HSE_PREDIV_DIV4,
    RCC_HSE_PREDIV_DIV5,
    RCC_HSE_PREDIV_DIV6,
    RCC_HSE_PREDIV_DIV7,
    RCC_HSE_PREDIV_DIV8,
    RCC_HSE_PREDIV_DIV9,
    RCC_HSE_PREDIV_DIV10,
    RCC_HSE_PREDIV_DIV11,
    RCC_HSE_PREDIV_DIV12,
    RCC_HSE_PREDIV_DIV13,
    RCC_HSE_PREDIV_DIV14,
    RCC_HSE_PREDIV_DIV15,
    RCC_HSE_PREDIV_DIV16
  };
#endif
  for (uint32_t mul = 2; mul < sizeof(muls) / sizeof(uint32_t) + 2; ++mul) {
    for (uint32_t div = 1; div < sizeof(divs) / sizeof(uint32_t) + 1; ++div) {
      if (mul / div == div_mul) {
        return sDiv_Mul(static_cast<int32_t>(muls[mul - 2]),
                        static_cast<int32_t>(divs[div - 1]));
      }
    }
  }
#elif defined(STM32L4)
  const uint32_t divs[] = {
      RCC_PLLR_DIV2,
      RCC_PLLR_DIV4,
      RCC_PLLR_DIV6,
      RCC_PLLR_DIV8,
  };
  /* PLLN: Multiplication factor for PLL VCO output
    clock. This parameter must be a number between
    Min_Data = 8 and Max_Data = 86 (no macro defined)
  */
  for (uint32_t mul = 8; mul <= 86; ++mul) {
    for (uint32_t div = 0; div < sizeof(divs) / sizeof(uint32_t); ++div) {
      if (mul / divs[div] == div_mul) {
        return sDiv_Mul(mul, static_cast<int32_t>(divs[div]));
      }
    }
  }
#endif
  return sDiv_Mul();
}

#ifdef STM32F4
constexpr sPllconfigF4 calc_pll_div_mul_F4(int32_t source_freq,
                                           uint32_t targetCPUFreq,
                                           bool is_USB_used = false) {

  constexpr uint32_t Pv[] = {RCC_PLLP_DIV2, RCC_PLLP_DIV4, RCC_PLLP_DIV6,
                             RCC_PLLP_DIV8};

  for (uint32_t pdiv = 2; pdiv < 64; ++pdiv) {
    if ((source_freq / pdiv < 950000L) || (source_freq / pdiv > 2100000L)) {
      continue;
    }
    for (uint32_t mul = 50; mul < 433; ++mul) {
      auto PLL_OUT = source_freq / pdiv * mul;
      if ((PLL_OUT < 100000000L) || (PLL_OUT > 432000000L)) {
        continue;
      }

      uint32_t P = 0, Q = 0;

      bool Pok = false, Qok = false;

      for (size_t ip = 0; ip < std::size(Pv); ++ip) {
        P = Pv[ip];
        if (PLL_OUT / P == targetCPUFreq) {
          Pok = true;
          break;
        }
      }

      if (is_USB_used) {
        for (int32_t iq = 2; iq < 16; ++iq) {
          if (PLL_OUT / iq == USB_FREQ) {
            Q = iq;
            Qok = true;
          }
        }
      } else {
        Qok = true;
        Q = 15;
      }

      if (Pok && Qok) {
        return sPllconfigF4{pdiv, mul, P, Q};
      }
    }
  }

  return sPllconfigF4(0);
}
#endif

/*!
 * \brief Initialize CPU clicking by setup PLL correct values
 *
 * Uses MACROs HSE_VALUE and F_CPU to calculate and set PLL register values in
 * compile-time.
 * MCU family specific realization
 */
void InitOSC() {
#if defined(STM32F0)

#if (F_CPU != USB_FREQ) || (HSE_VALUE > 0)
  constexpr auto clk_src = HSE_VALUE < 0 ? HSI48_VALUE : HSE_VALUE;
  constexpr auto div_mul = calc_pll_div_mul(F_CPU / clk_src);
  constexpr RCC_OscInitTypeDef RCC_OscInitStruct = {
      // HSE_VALUE -> PREDIV -> PLLMUL => F_CPU
      // PREDIV / PLLMUL = F_CPU / HSE_VALUE

      .OscillatorType = RCC_OSCILLATORTYPE_HSE,
      .HSEState = RCC_HSE_ON,
      .LSEState = RCC_LSE_OFF,
      .HSIState = RCC_HSI_OFF,
      .HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
      .HSI14State = RCC_HSI14_OFF,
      .HSI14CalibrationValue = RCC_HSI14CALIBRATION_DEFAULT,
      .LSIState = RCC_LSI_OFF,
      .HSI48State = RCC_HSI48_OFF,
      .PLL = {
          .PLLState = RCC_PLL_ON,
          .PLLSource = RCC_PLLSOURCE_HSE,
          .PLLMUL = div_mul.mul,
          .PREDIV = div_mul.div,
      }};
#else
  constexpr RCC_OscInitTypeDef RCC_OscInitStruct = {
      // HSI48 -> F_CPU
      // HSI48 -> USB

      .OscillatorType = RCC_OSCILLATORTYPE_HSI48,
      .HSEState = RCC_HSE_OFF,
      .LSEState = RCC_LSE_OFF,
      .HSIState = RCC_HSI_OFF,
      .HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
      .HSI14State = RCC_HSI14_OFF,
      .HSI14CalibrationValue = RCC_HSI14CALIBRATION_DEFAULT,
      .LSIState = RCC_LSI_OFF,
      .HSI48State = RCC_HSI48_ON,
      .PLL = {.PLLState = RCC_PLL_OFF}};
#endif

#elif defined(STM32F1)
  constexpr auto div_mul = calc_pll_div_mul(F_CPU / HSE_VALUE);
  constexpr RCC_OscInitTypeDef RCC_OscInitStruct = {
      // HSE_VALUE -> PREDIV1 -> PLLMUL => F_CPU
      // PREDIV1 / PLLMUL = F_CPU / HSE_VALUE

      .OscillatorType = RCC_OSCILLATORTYPE_HSE,
      .HSEState = RCC_HSE_ON,
      .HSEPredivValue = div_mul.div,
      .LSEState = RCC_LSE_OFF,
      .HSIState = RCC_HSI_OFF,
      .PLL = {.PLLState = RCC_PLL_ON,
              .PLLSource = RCC_PLLSOURCE_HSE,
              .PLLMUL = div_mul.mul}};

#elif defined(STM32F3)
  constexpr auto div_mul = calc_pll_div_mul(F_CPU / HSE_VALUE);
  constexpr RCC_OscInitTypeDef RCC_OscInitStruct{
      // HSE_VALUE -> PREDIV1 -> PLLMUL => F_CPU
      // PREDIV1 / PLLMUL = F_CPU / HSE_VALUE

      RCC_OSCILLATORTYPE_HSE,
      RCC_HSE_ON,
      div_mul.div,
      RCC_LSE_OFF,
      RCC_HSI_OFF,
      0,
      RCC_LSI_OFF,
      RCC_PLLInitTypeDef{RCC_PLL_ON, RCC_PLLSOURCE_HSE, div_mul.mul}};
#elif defined(STM32L4)
  constexpr auto div_mul = calc_pll_div_mul(F_CPU / HSE_VALUE);
  constexpr RCC_OscInitTypeDef RCC_OscInitStruct{
      // HSE_VALUE -> PLLM (/1) -> PLLN -> PLLR => F_CPU
      // PLLR / PLLN = F_CPU / HSE_VALUE

      RCC_OSCILLATORTYPE_HSE,
      RCC_HSE_ON,
      RCC_LSE_OFF,
      RCC_HSI_OFF,
      0,
      RCC_LSI_OFF,
      RCC_MSI_OFF,
      0,
      RCC_MSIRANGE_0,
      RCC_HSI48_OFF,
      RCC_PLLInitTypeDef{RCC_PLL_ON, RCC_PLLSOURCE_HSE, 1, div_mul.mul,
#if defined(RCC_PLLP_SUPPORT)
                         RCC_PLLP_DIV17,
#endif
                         RCC_PLLQ_DIV8, div_mul.div}};

#elif defined(STM32F4)
  constexpr auto auto_pll_config = calc_pll_div_mul_F4(HSE_VALUE, F_CPU, true);
  constexpr RCC_OscInitTypeDef RCC_OscInitStruct{
      // HSE_VALUE -> /M -> *N -> /P => F_CPU
      //                       -> /Q => USB
      RCC_OSCILLATORTYPE_HSE,
      RCC_HSE_ON,
      RCC_LSE_OFF,
      RCC_HSI_OFF,
      0,
      RCC_LSI_OFF,
      RCC_PLLInitTypeDef{RCC_PLL_ON, RCC_PLLSOURCE_HSE, auto_pll_config.p_div,
                         auto_pll_config.mul, auto_pll_config.P,
                         auto_pll_config.Q}};
#else
#error "Unsupported CPU family"
#endif
  RCC_OscInitTypeDef tmp;
  memcpy(&tmp, &RCC_OscInitStruct, sizeof(RCC_OscInitTypeDef));

  HAL_RCC_OscConfig(&tmp);
}

/*!
 * \brief Setup MCU peripheral buses clocks
 *
 * MCU-family specific realization
 */
void Configure_AHB_Clocks() {
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1
#if defined(STM32F1) || defined(STM32F3) || defined(STM32L4) || defined(STM32F4)
                                | RCC_CLOCKTYPE_PCLK2
#endif
      ;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;

#if defined(STM32F0)
  RCC_ClkInitStruct.APB1CLKDivider =
      F_CPU > 36000000U ? RCC_HCLK_DIV2 : RCC_HCLK_DIV1;
  auto latency = F_CPU < 24000000U ? FLASH_LATENCY_0 : FLASH_LATENCY_1;
#elif defined(STM32F1) || defined(STM32F3)
  RCC_ClkInitStruct.APB1CLKDivider =
      F_CPU > 36000000U ? RCC_HCLK_DIV2 : RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  auto latency = F_CPU < 24000000U
                     ? FLASH_LATENCY_0
                     : F_CPU < 48000000U ? FLASH_LATENCY_1 : FLASH_LATENCY_2;
#elif defined(STM32L4)
  RCC_ClkInitStruct.APB1CLKDivider =
      F_CPU > 80000000U ? RCC_HCLK_DIV2 : RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  auto latency =
      F_CPU < 16000000U
          ? FLASH_LATENCY_0
          : F_CPU < 32000000U
                ? FLASH_LATENCY_1
                : F_CPU < 48000000U
                      ? FLASH_LATENCY_2
                      : F_CPU < 64000000U ? FLASH_LATENCY_3 : FLASH_LATENCY_4;
#elif defined(STM32F4)
  RCC_ClkInitStruct.APB1CLKDivider =
      F_CPU > 50000000U ? RCC_HCLK_DIV2 : RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider =
      F_CPU > 100000000U ? RCC_HCLK_DIV2 : RCC_HCLK_DIV1;

  auto latency =
      F_CPU < 30000000U
          ? FLASH_LATENCY_0
          : F_CPU < 64000000U
                ? FLASH_LATENCY_1
                : F_CPU < 90000000U
                      ? FLASH_LATENCY_2
                      : F_CPU < 100000000U ? FLASH_LATENCY_3 : FLASH_LATENCY_4;
#else
#error "Unsupported CPU family"
#endif
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, latency);
}

/*!
 * \brief Configures MCU base clocking and setup system timer
 */
void SystemClock_Config(void) {
  InitOSC();
  Configure_AHB_Clocks();

  // Set up SysTTick to 1 ms
  // HAL
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  // SysTick_IRQn interrupt configuration - setting SysTick as lower priority
  // to satisfy FreeRTOS requirements
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/*!
 * \brief Enable flash protection if release
 */
void flashProtect(void) {
#if defined(APP_FLASH_PROTECT)
  FLASH_OBProgramInitTypeDef OBInit;
  HAL_FLASHEx_OBGetConfig(&OBInit);

  if (OBInit.RDPLevel != OB_RDP_LEVEL_1) {
    OBInit.OptionType = OPTIONBYTE_RDP;
    OBInit.RDPLevel = OB_RDP_LEVEL_1;
    HAL_FLASH_Unlock();
    HAL_FLASH_OB_Unlock();
    HAL_FLASHEx_OBProgram(&OBInit);
    HAL_FLASH_OB_Launch();
    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();
  }
#endif
}

/*!
 * \brief Main initialization function
 */
void InitBoard() {
  // Initialize board and HAL
  HAL_Init();

  // defined in HAL_Init()
  // HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  SystemClock_Config();

  flashProtect();
}

/*! @} */
