#===============================================================================
# @brief    example common
# Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
#===============================================================================

if(DEFINED CONFIG_HAVE_NFC_POLL)
    set(SOURCES "${SOURCES}"
        ${CMAKE_CURRENT_SOURCE_DIR}/example_rw.c
    )
endif()

if(DEFINED CONFIG_HAVE_NFC_LISTEN)
    set(SOURCES "${SOURCES}"
        ${CMAKE_CURRENT_SOURCE_DIR}/example_ce.c
    )
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)

set(PUBLIC_HEADER "${PUBLIC_HEADER}"
    ${CMAKE_CURRENT_SOURCE_DIR}/
    PARENT_SCOPE
)