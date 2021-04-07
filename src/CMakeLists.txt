
# bootloader sources
set(BOOTLOADER_SRC
    BoardInit.cpp
    BoardInit.h
    systick.c
    msp.cpp
#    main.cpp
    main1.cpp
    hardfault.cpp
    _init.c
    stubs.c
    stubs.cpp

    usb_device.h
    usb_device.c

#    SWO_debug.c

#    ina219.h
#    ina219.cpp

#    ina219dma_reader.cpp
#    ina219dma_reader.h

#    usb_cdc_interface.h
#    usb_cdc_interface.c

     usbd_desc.h
     usbd_desc.c
     usbd_conf.c
     usbd_custom_hid_if.h
     usbd_custom_hid_if.c

#    cdc_acm.h
#    cdc_acm.cpp

#    rxmessagereader.h
#    rxmessagereader.cpp

#    txmessagewriter.h
#    txmessagewriter.cpp

#    history.h
)

if(STM32_FAMILY STREQUAL "F0")
    list(APPEND BOOTLOADER_SRC
        interrupts_f0.c
        )
endif()

if(STM32_FAMILY STREQUAL "F3")
    list(APPEND BOOTLOADER_SRC
        interrupts_f3.c
        )
endif()

if(STM32_FAMILY STREQUAL "L4")
    list(APPEND BOOTLOADER_SRC
        interrupts_l4.c
        )
endif()

if(STM32_FAMILY STREQUAL "F4")
    list(APPEND BOOTLOADER_SRC
        interrupts_f4.c
        )
endif()

################################################################################

# Bootloader elf
ADD_EXECUTABLE(${PROJECT_NAME}
    ${BOOTLOADER_SRC}
    ${OD_SRC}
    ${CanOpenNode_app_SRC}
)

# if the library contains weak symbols in must be into -Wl,--whole-archive
# braces
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    -Wl,--whole-archive
        HAL
    -Wl,--no-whole-archive
    result
    USB
    nanopb
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    -DHISTORY_SIZE=${HISTORY_SIZE}
)

function(show_object_size target)
    string(REPLACE "objcopy" "size" CMAKE_OBJSIZE "${CMAKE_OBJCOPY}")
    add_custom_command(
        TARGET ${target} POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_OBJSIZE} ${target}
    )
endfunction()

################################################################################

STM32_GENERATE_MAP_FILE(${PROJECT_NAME})
STM32_SET_TARGET_PROPERTIES(${PROJECT_NAME})
STM32_SET_TARGET_LD_SPEC(${PROJECT_NAME} "nano.specs")
STM32_SET_TARGET_LD_SPEC(${PROJECT_NAME} "nosys.specs")

show_object_size(${PROJECT_NAME})
STM32_ADD_HEX_BIN_TARGETS(${PROJECT_NAME})
