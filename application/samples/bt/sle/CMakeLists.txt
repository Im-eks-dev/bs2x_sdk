set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER OR DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT)
    add_subdirectory_if_exist(sle_uart)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_HID_SERVER)
    add_subdirectory_if_exist(hid)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_DIS_SERVER)
    add_subdirectory_if_exist(dis)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_BAS_SERVER)
    add_subdirectory_if_exist(bas)
endif()

# 公共源码编译
set(SOURCES_LIST
    ${SOURCES_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/service_common.c
)
set(HEADER_LIST 
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)
