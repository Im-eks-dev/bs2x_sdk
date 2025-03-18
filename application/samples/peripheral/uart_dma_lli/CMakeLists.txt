#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_UART_DMA_LLI_MASTER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/uart_dma_lli_master_demo.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_UART_DMA_LLI_SLAVE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/uart_dma_lli_slave_demo.c
)
endif()

set(HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)