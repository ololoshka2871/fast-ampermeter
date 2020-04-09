
# setup arm embedded tool chain
SET(TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/gcc_stm32.cmake)
INCLUDE(halpers)

# path of the cmake files dirrectory after install
set(cmake_INSTALL_DIR   ${SHARE_INSTALL_DIR}/cmake)

# install application-related modules
install(FILES
        FindMono.cmake
        FindSTM32Cube.cmake
        CreateODSources.cmake
        gcc_stm32.cmake
        gcc_stm32f1.cmake
        gcc_stm32f3.cmake
        gcc_stm32f4.cmake
        gcc_stm32l1.cmake
        gcc_stm32l4.cmake
        stm32_linker.cmake
        stm32_linker_F4.cmake
        ApplicationHalpers.cmake
        HybridTargets.cmake
        Doxygen.cmake
    DESTINATION ${cmake_INSTALL_DIR}/Modules)

# CMAKE_TOOLCHAIN_FILE mast be set in Top level CMakeLists.txt
set(CMAKE_TOOLCHAIN_FILE ${TOOLCHAIN_FILE}    PARENT_SCOPE)
set(cmake_INSTALL_DIR    ${cmake_INSTALL_DIR} PARENT_SCOPE)

