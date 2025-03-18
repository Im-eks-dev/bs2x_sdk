if(DEFINED CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_DONGLE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/air_mouse_usb/usb_init_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/sle_air_mouse_client/sle_air_mouse_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_air_mouse_with_dongle.c
)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_AIR_MOUSE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse/sle_air_mouse_server/sle_air_mouse_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse/sle_air_mouse_server/sle_air_mouse_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_air_mouse_with_dongle.c
)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_AIR_MOUSE AND DEFINED CONFIG_AIR_MOUSE_HR_BOARD)
set(SOURCES_LIST
    ${SOURCES_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse/air_mouse_button/air_mouse_button.c
)
endif()

if(DEFINED CONFIG_AIR_MOUSE_DONGLE_FACTORY_PHASE_CALI)
set(SOURCES_LIST
    ${SOURCES_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/air_mouse_usb/usb_serial.c
    )
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)