#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_LOWPOWER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/lowpower.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_ble_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_ble_hid_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_ble_server_adv.c
)
set(HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/
)
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)
