## Not pushing this file down to libraries in order to keep source tree as is (not populating with extra files, such as CMakeList.txt)
##
## Below each section represents a library with its own settings
#

###################
# HW includes generation
###################

configure_file(STM32/hw_includes.h.in ${CMAKE_CURRENT_BINARY_DIR}/hw_includes.h @ONLY)

###################
# CMSIS and HAL
#
# Note: In order not to make yet another copy of CMSIS and HAL in the source tree I'll use
#       stm32generic's one for now. It seems it is pretty fresh and match the original
###################

SET(STM32_DRIVERS_BASE        ${STM32Cube}/Drivers)
SET(HAL_BASE                  ${STM32_DRIVERS_BASE}/${STM32_FAMILY_LONG}xx_HAL_Driver)
SET(HAL_BASE_SRC_DIR          ${HAL_BASE}/Src)
SET(CMSIS_BASE                ${STM32_DRIVERS_BASE}/CMSIS)

SET(HAL_SRC
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_adc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_adc_ex.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_can.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_cec.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_cortex.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_crc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_dac.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_dac_ex.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_dma.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_eth.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_flash.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_flash_ex.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_gpio.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_hcd.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_i2c.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_i2c_ex.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_i2s.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_irda.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_iwdg.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_mmc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_msp_template.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_nand.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_nor.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_pccard.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_pcd.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_pcd_ex.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_pwr.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_pwr_ex.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_rcc.c
    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_rcc_ex.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_rtc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_rtc_ex.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_sd.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_smartcard.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_spi.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_spi_ex.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_sram.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_tim.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_timebase_rtc_alarm_template.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_timebase_tim_template.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_tim_ex.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_uart.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_usart.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_hal_wwdg.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_adc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_crc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_dac.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_dma.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_exti.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_fsmc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_gpio.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_i2c.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_pwr.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_rcc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_rtc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_sdmmc.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_spi.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_tim.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_usart.c
##    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_usb.c
#    ${HAL_BASE_SRC_DIR}/${STM32_FAMILY_LONG_lx}_ll_utils.c

    ${CMAKE_CURRENT_SOURCE_DIR}/STM32/system_${STM32_FAMILY_LONG_lx}.c

    ${CMSIS_BASE}/Device/ST/${STM32_FAMILY_LONG}xx/Source/Templates/gcc/startup_${STM32_FAMILY_Xl}.s
)

ADD_LIBRARY(HAL STATIC ${HAL_SRC})
target_include_directories(HAL PUBLIC
    ${HAL_INCLUDE_SPECIAL}
    ${HAL_BASE}/Inc
    ${CMSIS_BASE}/Include
    ${CMSIS_BASE}/Device/ST/${STM32_FAMILY_LONG}xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/STM32
    ${CMAKE_CURRENT_BINARY_DIR}
)

if (HAL_DEFINES_SPECIAL)
    target_compile_definitions(HAL PUBLIC
        ${HAL_DEFINES_SPECIAL}
    )
endif()

target_compile_definitions(HAL PUBLIC
    -DUSE_HAL_I2C_REGISTER_CALLBACKS=1
    #-DUSE_HAL_PCD_REGISTER_CALLBACKS=1
)

###################
# USB
###################

set(USB_BASE_DIR ${STM32Cube}/Middlewares/ST/STM32_USB_Device_Library)
set(USB_CORE_DIR ${USB_BASE_DIR}/Core)
set(USB_CRC_ACM_DIR ${USB_BASE_DIR}/Class/CDC)

set(USB_SRC
    # ${USB_CORE_DIR}/Src/usbd_conf_template.c
    ${USB_CORE_DIR}/Src/usbd_core.c
    ${USB_CORE_DIR}/Src/usbd_ctlreq.c
    # ${USB_CORE_DIR}/Src/usbd_desc_template.c
    ${USB_CORE_DIR}/Src/usbd_ioreq.c

    ${USB_CRC_ACM_DIR}/Src/usbd_cdc.c
)

ADD_LIBRARY(USB STATIC ${USB_SRC})
target_include_directories(USB PUBLIC
    ${USB_CORE_DIR}/Inc
    ${USB_CRC_ACM_DIR}/Inc
)

target_link_libraries(USB PUBLIC
    HAL
)

###################
# Result
###################

add_library(result INTERFACE)
target_include_directories(result INTERFACE result)

###################
# nanopb
###################

add_subdirectory(nanopb)
set(PROTOBUF_PROTO_FILE ${PROTOBUF_PROTO_FILE} PARENT_SCOPE)


SET(LIBS_DIR            ${CMAKE_CURRENT_SOURCE_DIR}             PARENT_SCOPE)
