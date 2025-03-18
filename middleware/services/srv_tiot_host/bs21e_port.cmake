#[[
Copyright (c) CompanyNameMagicTag. 2024-2024. All rights reserved.
Description: CMake tiot driver module.
Author: tiot driver
Create: 2024-05-13
]]

if(DEFINED CONFIG_TIOT_PORTING_AIR_MOUSE)
if(DEFINED CONFIG_AIR_MOUSE_HR_BOARD)
set(TIOT_PORTING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/air_mouse_hr)
set(TIOT_PORTING_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/air_mouse_hr)
else()
set(TIOT_PORTING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/air_mouse)
set(TIOT_PORTING_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/air_mouse)
endif()
endif()
