#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_MASTER_A)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_dma_lli_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_dma_lli_demo.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_SLAVE_D)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_dma_lli_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_dma_lli_demo.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_MASTER_B)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_dma_lli_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_dma_lli_slave.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_dma_lli_slave_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_dma_lli_slave_demo.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_SLAVE_C)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_dma_lli_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_dma_lli_master.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_dma_lli_master_demo.c
)
endif()

set(HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)