#[[
Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
Description: CMake tiot driver module.
Author: tiot driver
Create: 2023-11-07
]]

#===================================================================================
set(COMPONENT_NAME "tiot_driver")
return_if_not_build()

if(DEFINED CONFIG_TIOT_PORTING_AIR_MOUSE)
if(DEFINED CONFIG_AIR_MOUSE_HR_BOARD)
set(TIOT_PORTING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/air_mouse_hr)
set(TIOT_PORTING_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/air_mouse_hr)
else()
set(TIOT_PORTING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/air_mouse)
set(TIOT_PORTING_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/air_mouse)
endif()
elseif(DEFINED CONFIG_TIOT_PORTING_BSXX_BRIDGE)
if(${CHIP} MATCHES "bs25")
set(TIOT_PORTING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/bs25_pad_slave)
set(TIOT_PORTING_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/bs25_pad_slave)
else()
set(TIOT_PORTING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/bs21_pad_slave)
set(TIOT_PORTING_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/bs21_pad_slave)
endif()
else()
set(TIOT_PORTING_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/brandy_gnss_evb)
endif()
add_subdirectory(tiot_driver)

set(SOURCES
    ${TIOT_SRC_LIST}
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/common/interface/device_interface
)

set(PRIVATE_HEADER
    ${TIOT_HEADER_LIST}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
    ${TIOT_CFLAGS}
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

# SDK install
if(DEFINED CONFIG_TIOT_PORTING_AIR_MOUSE)
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/air_mouse "*")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/air_mouse_hr "*")
endif()
if(DEFINED CONFIG_TIOT_PORTING_BSXX_BRIDGE)
if(${CHIP} MATCHES "bs25")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/bs25_pad_slave "*")
else()
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/bs21_pad_slave "*")
endif()
endif()

if(DEFINED SDK_OUTPUT_PATH)
include(${CMAKE_CURRENT_SOURCE_DIR}/tiot_release.cmake)
endif()

if(DEFINED CONFIG_BUILD_USING_STATIC_LIBRARIES_SDK)
    if(EXISTS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
        set(SOURCES )
        set(LIBS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    endif()
endif()

build_component()
