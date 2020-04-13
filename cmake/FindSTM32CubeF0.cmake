set ( STM32CubeF0_FOUND false )
if ( NOT STM32CubeF0_DIR )
	set ( STM32Cube_DIR "~/STM32Cube/Repository/" )
endif ()
file ( TO_CMAKE_PATH "${STM32CubeF0_DIR}" STM32CubeF0_DIR )
if ( IS_DIRECTORY "${STM32CubeF0_DIR}" )
	if ( NOT PACKAGE_FIND_VERSION )
		message ( STATUS "No STM32CubeF0 version specified, using the latest version." )
                file ( GLOB STM32CubeF0 "${STM32CubeF0_DIR}/STM32Cube_FW_${STM32_FAMILY}_V*" )
		list ( SORT STM32CubeF0 )
		list ( REVERSE STM32CubeF0 )
		list ( GET STM32CubeF0 0 STM32CubeF0 )
	else ()
                set ( STM32CubeF0 "${STM32CubeF0_DIR}/STM32Cube_FW_${STM32_FAMILY}_V${PACKAGE_FIND_VERSION}" )
	endif ()
	if ( IS_DIRECTORY "${STM32CubeF0}" )
		message ( STATUS "Select STM32CubeF0 in ${STM32CubeF0}" )
		set ( STM32CubeF0_FOUND true )
                string ( REPLACE "${STM32CubeF0_DIR}/STM32Cube_FW_${STM32_FAMILY}_V" "" STM32CubeF0_VERSION ${STM32CubeF0} )
	else ()
		message ( FATAL_ERROR "The path ${STM32CubeF0} is invaid." )
	endif ()

else ()
        message ( FATAL_ERROR "Unknown STM32CubeF0 path, please use -DSTM32CubeF0_DIR to set." )
endif ()

include ( FindPackageHandleStandardArgs )
find_package_handle_standard_args ( "STM32CubeF0"
    DEFAULT_MSG
    STM32CubeF0_FOUND
    STM32CubeF0
)
