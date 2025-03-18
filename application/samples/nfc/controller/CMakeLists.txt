#===============================================================================
# @brief    sample nfc controller
# Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
#===============================================================================

if(DEFINED CONFIG_HAVE_NFC_POLL)
    set(PUBLIC_DEFINES "${PUBLIC_DEFINES}"
        HAVE_NFC_POLL
    )
endif()

if(DEFINED CONFIG_HAVE_NFC_LISTEN)
    set(PUBLIC_DEFINES "${PUBLIC_DEFINES}"
        HAVE_NFC_LISTEN
    )
endif()

add_subdirectory_if_exist(example_common)
if(DEFINED CONFIG_ENABLE_NFC_ONLY_SAMPLE)
    add_subdirectory_if_exist(sample_nfc_only)
endif()
if(DEFINED CONFIG_ENABLE_NFC_SLE_SAMPLE)
    add_subdirectory_if_exist(sample_nfc_sle)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
set(PUBLIC_DEFINES "${PUBLIC_DEFINES}" PARENT_SCOPE)
