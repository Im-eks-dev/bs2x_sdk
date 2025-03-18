if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_KEYBOARD)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_keyboard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_keyboard_server/ble_keyboard_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_keyboard_server/ble_keyboard_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_keyboard_server/ble_hid_keyboard_server.c
)
endif()
set(HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/ble_keyboard_server)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)