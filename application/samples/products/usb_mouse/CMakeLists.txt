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

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)