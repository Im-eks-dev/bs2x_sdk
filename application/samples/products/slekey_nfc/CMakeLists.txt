#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
#==============================================================================
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/nfc_app/nfc_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/slekey_nfc.c
)
set(LIB
    ${CMAKE_CURRENT_SOURCE_DIR}/st_nfc/libst_nfc.a
)
set(PUBLIC_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/nfc_app
    ${CMAKE_CURRENT_SOURCE_DIR}/st_nfc
)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(LIBS "${LIBS}" ${LIB} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${PUBLIC_HEADER_LIST} PARENT_SCOPE)