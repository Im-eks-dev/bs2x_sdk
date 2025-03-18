#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/keyboard_usb/keyscan_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/keyboard_usb/usb_init_keyboard_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/usb_keyboard.c
)
set(HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/keyboard_usb)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)