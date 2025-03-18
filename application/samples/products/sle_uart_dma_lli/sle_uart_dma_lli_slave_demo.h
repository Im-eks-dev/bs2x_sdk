/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: UART DMA LLI Slave Sample Source. \n
 *
 * History: \n
 * 2024-06-13, Create file. \n
 */
#ifndef SLE_UART_DMA_LLI_SLAVE_DEMO_H
#define SLE_UART_DMA_LLI_SLAVE_DEMO_H

#if defined(CONFIG_SAMPLE_SUPPORT_UART_DMA_RAW_DATA_MODE)
bool uart_dma_rx_cb(uint8_t *data, uint32_t length);
#endif

#endif