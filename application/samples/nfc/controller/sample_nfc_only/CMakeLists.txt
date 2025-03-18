#===============================================================================
# @brief    sample nfc only
# Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
#===============================================================================

if(DEFINED CONFIG_SAMPLE_SUPPORT_NFC_ONLY_RW)
    set(SOURCES "${SOURCES}" ${CMAKE_CURRENT_SOURCE_DIR}/nfc_sample.c PARENT_SCOPE)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_NFC_ONLY_CE)
    set(SOURCES "${SOURCES}" ${CMAKE_CURRENT_SOURCE_DIR}/nfc_sample.c PARENT_SCOPE)
endif()
