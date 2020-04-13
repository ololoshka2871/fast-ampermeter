/*!
 * \file
 * \brief Contains Special trap for HardFault_Handler interrupt of cpu
 */

/*! \ingroup Utils
 *  @{
 */

extern "C" {

#include <stdint.h>

/// The prototype shows it is a naked function - in effect this is just an
/// assembly function.
void HardFault_Handler(void) __attribute__((naked));

/// The fault handler implementation calls a function called
/// prvGetRegistersFromStack().
void HardFault_Handler(void) {
  for (;;)
    __asm__("BKPT");
}
}

/*! @} */
