#===============================================================================
# @brief    sample nfc sle
# Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
#===============================================================================

if(DEFINED CONFIG_NFC_SLE_MASTER_SAMPLE)
    add_subdirectory_if_exist(master)
elseif(DEFINED CONFIG_NFC_SLE_SLAVE_SAMPLE)
    add_subdirectory_if_exist(slave)
endif()

set(SOURCES "${SOURCES}"
    ${CMAKE_CURRENT_SOURCE_DIR}/nfc_sle_queue.c
    PARENT_SCOPE
)

set(PUBLIC_HEADER "${PUBLIC_HEADER}"
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE
)

set(PUBLIC_DEFINES "${PUBLIC_DEFINES}" PARENT_SCOPE)