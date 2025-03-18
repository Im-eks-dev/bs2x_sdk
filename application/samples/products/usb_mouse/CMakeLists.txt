#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_button/mouse_button.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_wheel/mouse_wheel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_paw3395.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_usb/usb_init_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/usb_mouse.c
)
set(HEADER_LIST
${CMAKE_CURRENT_SOURCE_DIR}/mouse_button
${CMAKE_CURRENT_SOURCE_DIR}/mouse_wheel
${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor
${CMAKE_CURRENT_SOURCE_DIR}/mouse_usb
)
if(DEFINED CONFIG_SAMPLE_USB_SUPPORT_SENSOR_3395)
    list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_paw3395.c)
elseif(DEFINED CONFIG_SAMPLE_USB_SUPPORT_SENSOR_3395SE)
    list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_paw3395se.c)
else()
    message(FATAL_ERROR "Please define sensor type!")
endif()
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)