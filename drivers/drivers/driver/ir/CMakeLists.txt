#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ir")

if(DEFINED CONFIG_SAMPLE_SUPPORT_IR_STUDY)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ir_study.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ir_tx.c
)
endif()

if(NOT DEFINED CONFIG_SAMPLE_SUPPORT_IR_STUDY)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ir_tx.c
)
endif()

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)
build_component()
