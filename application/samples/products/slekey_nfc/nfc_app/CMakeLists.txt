#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "nfc_app")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/nfc_app.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error=unused-parameter -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=strict-prototypes -Wno-error=discarded-qualifiers -Wno-error=pointer-sign
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
