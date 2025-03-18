/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: UART DMA LLI Common Interface header. \n
 *
 * History: \n
 * 2024-06-13, Create file. \n
 */
#ifndef SLE_UART_DMA_LLI_COMMON_H
#define SLE_UART_DMA_LLI_COMMON_H

#include "pinctrl.h"
#include "uart.h"
#include "watchdog.h"
#include "dma.h"
#include "hal_dma.h"
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define DATA_CHECK_INDEX 0
#define DATA_CHECK_MAX 0xFF
#define SLE_UART_MTU_SIZE 500
#define SLE_UART_DMA_TRX_PILOT_DENSITY 2        // 导频密度16:1

// Optional SLE CONFIGS
#if defined(CONFIG_SAMPLE_SUPPORT_RATE_CONFIG_236B_25MS)
#define SLE_UART_RECV_CNT 500
#define UART_PAYLOAD_LEN 236                    // A端发包大小
#define SLE_UART_DMA_CONN_INV 200               // 单位：125us
#define SLE_UART_DMA_TRX_FORMAT 0               // 0 :无线帧类型1(GFSK); 1:无线帧类型2(QPSK)
#define SLE_UART_DMA_TRX_PHY 1                  // 0：1M; 1:2M; 2:4M;
#define SLE_UART_DATA_BUFFER_SIZE 6
#elif defined(CONFIG_SAMPLE_SUPPORT_RATE_CONFIG_236B_2MS)
#define SLE_UART_RECV_CNT 4000
#define UART_PAYLOAD_LEN 236                    // A端发包大小
#define SLE_UART_DMA_CONN_INV 16                // 单位：125us
#define SLE_UART_DMA_TRX_FORMAT 1               // 0 :无线帧类型1(GFSK); 1:无线帧类型2(QPSK)
#define SLE_UART_DMA_TRX_PHY 2                  // 0：1M; 1:2M; 2:4M;
#define SLE_UART_DATA_BUFFER_SIZE 6
#elif defined(CONFIG_SAMPLE_SUPPORT_RATE_CONFIG_250B_1K)
#define SLE_UART_RECV_CNT 10000
#define UART_PAYLOAD_LEN 250                    // A端发包大小
#define SLE_UART_LOW_LATENCY_PARAM 1000         // 单位：125us
#define SLE_UART_DMA_TRX_FORMAT 1               // 0 :无线帧类型1(GFSK); 1:无线帧类型2(QPSK)
#define SLE_UART_DMA_TRX_PHY 2                  // 0：1M; 1:2M; 2:4M;
#define SLE_UART_DMA_TRX_MCS 8                  // QPSK 1
#define SLE_UART_DATA_BUFFER_SIZE 6
#elif defined(CONFIG_SAMPLE_SUPPORT_RATE_CONFIG_36B_1K)
#define SLE_UART_RECV_CNT 10000
#define UART_PAYLOAD_LEN 36                    // A端发包大小
#define SLE_UART_LOW_LATENCY_PARAM 1000         // 单位：125us
#define SLE_UART_DMA_TRX_FORMAT 1               // 0 :无线帧类型1(GFSK); 1:无线帧类型2(QPSK)
#define SLE_UART_DMA_TRX_PHY 2                  // 0：1M; 1:2M; 2:4M;
#define SLE_UART_DMA_TRX_MCS 8                  // QPSK 1
#define SLE_UART_DATA_BUFFER_SIZE 6
#elif defined(CONFIG_SAMPLE_SUPPORT_RATE_CONFIG_36B_2K)
#define SLE_UART_RECV_CNT 20000
#define UART_PAYLOAD_LEN 36                     // A端发包大小
#define SLE_UART_LOW_LATENCY_PARAM 2000         // 单位：125us
#define SLE_UART_DMA_TRX_FORMAT 1               // 0 :无线帧类型1(GFSK); 1:无线帧类型2(QPSK)
#define SLE_UART_DMA_TRX_PHY 1                  // 0：1M; 1:2M; 2:4M;
#define SLE_UART_DMA_TRX_MCS 8                  // QPSK 1
#define SLE_UART_DATA_BUFFER_SIZE 20
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_MASTER_B) || defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_SLAVE_C)
bool uart_dma_lli_buffer_check_full(void);

bool uart_dma_lli_buffer_check_empty(void);

bool uart_dma_lli_buffer_add_data(uint8_t *data, uint16_t length);

uint8_t *uart_dma_lli_buffer_get_data(uint16_t *length);
#endif

// SLE FUNC PART
void sle_uart_dma_set_mcs(uint16_t conn_id, uint8_t mcs);

void sle_uart_performance_statistics(uint8_t *value, uint16_t len);

void sle_uart_send_data_init(uint8_t *send_data, uint16_t len);

void sle_uart_recv_data_init(void);

void sle_uart_send_wakeup_data(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif