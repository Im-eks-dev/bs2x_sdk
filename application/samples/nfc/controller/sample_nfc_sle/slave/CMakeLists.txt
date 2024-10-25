#===============================================================================
# @brief    sample nfc sle slave
# Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
#===============================================================================

set(SOURCES "${SOURCES}"
    ${CMAKE_CURRENT_SOURCE_DIR}/nfc_sle_slave.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nfc_sle_slave_main.c
    PARENT_SCOPE
)
set(PUBLIC_HEADER "${PRIVATE_HEADER}"
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE
)
set(PUBLIC_DEFINES "${PUBLIC_DEFINES}"
    NFC_SLE_SLAVE_SAMPLE
    PARENT_SCOPE
)