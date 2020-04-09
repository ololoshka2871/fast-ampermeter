#include "softreset.h"

#include "hw_includes.h"

void SoftReset() {
  SCB->AIRCR = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) |
                          (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                          SCB_AIRCR_SYSRESETREQ_Msk);
  NVIC_SystemReset();
  while (1)
    ;
}
