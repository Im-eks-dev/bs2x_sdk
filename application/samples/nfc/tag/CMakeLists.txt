#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(SOURCES "${SOURCES}"
    ${CMAKE_CURRENT_SOURCE_DIR}/nfc_t2t_text.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nfc_ndef_msg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nfc_text_record.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nfc_uri_record.c
)

set(SOURCES "${SOURCES}" PARENT_SCOPE)