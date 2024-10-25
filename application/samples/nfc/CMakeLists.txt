#===============================================================================
# @brief    nfc samples
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================

if(DEFINED CONFIG_ENABLE_NFC_CONTROLLER_SAMPLE)
    add_subdirectory_if_exist(controller)
elseif(DEFINED CONFIG_ENABLE_NFC_TAG_SAMPLE)
    add_subdirectory_if_exist(tag)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
set(PUBLIC_DEFINES "${PUBLIC_DEFINES}" PARENT_SCOPE)