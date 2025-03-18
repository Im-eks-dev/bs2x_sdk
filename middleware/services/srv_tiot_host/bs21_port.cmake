#[[
Copyright (c) CompanyNameMagicTag. 2024-2024. All rights reserved.
Description: CMake tiot driver module.
Author: tiot driver
Create: 2024-05-13
]]

if(DEFINED CONFIG_TIOT_PORTING_BSXX_BRIDGE)
set(TIOT_PORTING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/bs21_pad_slave)
set(TIOT_PORTING_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/bs21_pad_slave)
endif()
