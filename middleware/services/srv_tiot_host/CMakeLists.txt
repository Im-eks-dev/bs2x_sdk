#[[
Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
Description: CMake tiot driver module.
Author: tiot driver
Create: 2023-11-07
]]

#===================================================================================
set(COMPONENT_NAME "tiot_driver")
return_if_not_build()

# 默认为none
set(TIOT_PORTING_DIR "none")

# select port
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP}_port.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/${CHIP}_port.cmake)
endif()

if(${TIOT_PORTING_DIR} STREQUAL "none")
message(STATUS "[tiot driver]no porting dir, skip build.")
else()
add_subdirectory(tiot_driver)
endif()

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

build_component()
