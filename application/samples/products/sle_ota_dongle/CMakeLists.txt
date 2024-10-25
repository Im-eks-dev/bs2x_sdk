#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
#===============================================================================
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_ota_dongle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_ota_hid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_ota_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_ota_cmd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_ota_cmd_handler.c
)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)