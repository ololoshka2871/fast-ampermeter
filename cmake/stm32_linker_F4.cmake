
IF(STM32_FAMILY STREQUAL "F4")
  SET(STM32_CCRAM_DEF "  CCMRAM (rw) : ORIGIN = ${STM32_CCRAM_ORIGIN}, LENGTH = ${STM32_CCRAM_SIZE}\n")
  SET(STM32_CCRAM_SECTION
  "  _siccmram = LOADADDR(.ccmram)\;\n"
  "  .ccmram :\n"
  "  {"
  "    . = ALIGN(4)\;\n"
  "    _sccmram = .\;\n"
  "    *(.ccmram)\n"
  "    *(.ccmram*)\n"
  "    . = ALIGN(4)\;\n"
  "    _eccmram = .\;\n"
  "  } >CCMRAM AT> FLASH\n"
  )
ELSE()
  SET(STM32_CCRAM_DEF "")
  SET(STM32_CCRAM_SECTION "")
ENDIF()
