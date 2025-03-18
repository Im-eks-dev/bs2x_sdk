#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "uart")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/uart.c
)

if(DEFINED CONFIG_UART_SUPPORT_DMA_LLI)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/uart_lli.c")
endif()

set(PUBLIC_HEADER
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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
