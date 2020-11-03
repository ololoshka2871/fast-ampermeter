CMAKE_MINIMUM_REQUIRED(VERSION 3.4) 

################################################################################
# platform diffenitions

set(STM32_CHIP                          "STM32F042C6"
    CACHE STRING "Target STM32 chip name")

set(CLOCK_F_CPU                         48000000
    CACHE STRING "Target CPU Frequency")

set(CLOCK_F_XTAL                        -1
    CACHE STRING "CPU Quartz resonator fequency")

set(BUILD_USE_LTO                       FALSE
    CACHE BOOL "Use link-time optimisztion")

set(HISTORY_SIZE                        32
    CACHE STRING "Use link-time optimisztion")

set(DISABLE_SWO_OUTPUT                  TRUE
    CACHE BOOL "Disable SWO debug messages")


################################################################################

set(CPP_STD 17)

# Load the toolchain file that uses vars above
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
add_subdirectory(cmake)

# Always build in release as it uses -Os
SET(CMAKE_BUILD_TYPE Release)

################################################################################

project(Fast-ampermeter)

ENABLE_LANGUAGE(ASM)

# Find HAL library by chip family, if fails specify -DSTM32Cube_DIR=
# cmake in command line. Target tirrectory mast contain out-of-package STM32Cube_FW_*_V*/
# subdirrectory
find_package(STM32Cube REQUIRED)

STM32_GET_FAMITY_X(${STM32_CHIP} STM32_FAMILY_X)
SET(STM32_FAMILY_LONG STM32${STM32_FAMILY})
STRING(TOLOWER ${STM32_FAMILY_LONG} STM32_FAMILY_LONG_l)
SET(STM32_FAMILY_LONG_lx ${STM32_FAMILY_LONG_l}xx)
STRING(TOLOWER ${STM32_FAMILY_X} STM32_FAMILY_Xl)
STRING(TOLOWER ${STM32_FAMILY_X_nosizecode} STM32_FAMILY_Xxl)

# Project-wide definitions
ADD_DEFINITIONS(
    -D${STM32_FAMILY_LONG}
    -D${STM32_CHIP}
    -D${STM32_FAMILY_X}
    -D${STM32_FAMILY_X_nosizecode}
    -DMCU_${STM32_CHIP}
    -DF_CPU=${CLOCK_F_CPU}L
    -DHSE_VALUE=${CLOCK_F_XTAL}L
)

if (DISABLE_SWO_OUTPUT)
    ADD_DEFINITIONS(
        -DDISABLE_SWO_OUTPUT
    )
endif()

###############################################################################

add_subdirectory(Libs)
add_subdirectory(src)
add_subdirectory(tests)

###############################################################################

include(Doxygen)

# generate doc
SET(DOXY_USE_MDFILE_AS_MAINPAGE ${PROJECT_SOURCE_DIR}/README.md)
SET(DOXY_INPUT              "${PROJECT_SOURCE_DIR}/README.md ${PROJECT_SOURCE_DIR}/src")
SET(DOXY_EXTRACT_ALL        YES)

SET(DOXY_PROJECT_NAME       Fast-ampermeter)
SET(DOXY_OUTPUT_LANGUAGE    "English")
SET(DOXY_ENABLED_SECTIONS   "developer_sec")
SET(DOXY_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_BINARY_DIR}/doc")
SET(DOXY_PROJECT_NUMBER     0.1)
ADD_DOCUMENTATION(doc Doxyfile)
