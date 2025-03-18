if(DEFINED CONFIG_SAMPLE_SUPPORT_GAME_MOUSE_DONGLE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_usb/usb_dfu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_usb/usb_init_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_client/sle_mouse_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_low_latency_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_dongle.c
)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_GAME_MOUSE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_button/mouse_button.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_key/mouse_key.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_light/mouse_light.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_wheel/mouse_wheel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_power/mouse_battery_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_power/mouse_lowpower_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_flash/flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_usb/usb_dfu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_usb/usb_init_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_mac_rules/mac_rules.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_status/ble_sle_status.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mouse_server/ble_mouse_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mouse_server/ble_mouse_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mouse_server/ble_bas_and_dis_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mouse_server/ble_hid_mouse_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_server/sle_mouse_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_server/sle_mouse_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_server/sle_mouse_hid_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_server/sle_mouse_bas_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_server/sle_mouse_dis_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_server/sle_mouse_channel_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_data_transfer_manage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_mode_manage.c
)

if((DEFINED CONFIG_STORAGE_BY_FLASH) OR (DEFINED CONFIG_EXTENED_PROTOCOL))
list(APPEND SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/app_public/app_public.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sfc_flash/sfc_flash.c
)
endif()

list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_paw3395.c)
list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_paw3311.c)
list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_paw3805.c)
list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_pmw3325.c)
list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_pmw3950.c)
list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_pmw3398.c)

endif()

set(HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_button
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_key
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_light
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_wheel
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_power
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_usb
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_mac_rules
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_status
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mouse_server
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_server
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_client
    ${CMAKE_CURRENT_SOURCE_DIR}

)

if((DEFINED CONFIG_STORAGE_BY_FLASH) OR (DEFINED CONFIG_EXTENED_PROTOCOL))
list(APPEND HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sfc_flash
    ${CMAKE_CURRENT_SOURCE_DIR}/app_public
)
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)
