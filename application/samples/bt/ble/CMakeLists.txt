#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(ble_hid_keyboard_server)
add_subdirectory_if_exist(ble_hid_mouse_server)
add_subdirectory_if_exist(ble_gatt_client)
add_subdirectory_if_exist(ble_uuid_server)
if(${CHIP} MATCHES "ws63")
add_subdirectory_if_exist(ble_wifi_cfg_client)
add_subdirectory_if_exist(ble_wifi_cfg_server)
endif()
set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)