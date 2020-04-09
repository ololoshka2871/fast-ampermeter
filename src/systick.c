#include "hw_includes.h"

/*!
 * \ingroup Utils
 * \brief System timer tick handler
 *
 * Interrupt header, must be defined, Can init uses uwTick,
 * else wwatchdog triggered
 */
void SysTick_Handler(void) {
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}
