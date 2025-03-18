/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: UART DMA LLI Common Interface Source. \n
 *
 * History: \n
 * 2024-06-13, Create file. \n
 */
#include "tcxo.h"
#include "sle_connection_manager.h"
#include "sle_uart_dma_lli_slave_demo.h"
#include "sle_uart_dma_lli_common.h"

static uint32_t g_recv_count = 0;
static uint64_t g_recv_data_count = 0;
static uint64_t g_recv_start_time = 0;
static uint64_t g_recv_end_time = 0;
static uint32_t g_recv_loss_count = 0;
static uint32_t g_buffer_loss_count = 0;
static uint32_t g_recv_err_count = 0;
static uint8_t g_recv_data[CONFIG_SLE_UART_DMA_MAX_MSG_LEN];

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_MASTER_B) || defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_SLAVE_C)
typedef struct {
    uint8_t data[CONFIG_SLE_UART_DMA_MAX_MSG_LEN];
    uint16_t length;
} uart_data;

uart_data g_data_buffer[SLE_UART_DATA_BUFFER_SIZE];
volatile uint8_t g_data_buffer_head = 0;
volatile uint8_t g_data_buffer_tail = 0;

bool uart_dma_lli_buffer_check_empty(void)
{
    return (g_data_buffer_head == g_data_buffer_tail);
}

bool uart_dma_lli_buffer_check_full(void)
{
    return ((g_data_buffer_tail + 1) % SLE_UART_DATA_BUFFER_SIZE == g_data_buffer_head);
}

bool uart_dma_lli_buffer_add_data(uint8_t *data, uint16_t length)
{
    uint32_t lock = osal_irq_lock();
    if (!uart_dma_lli_buffer_check_full()) {
        (void)memcpy_s(g_data_buffer[g_data_buffer_tail].data,
            CONFIG_SLE_UART_DMA_MAX_MSG_LEN, data, length);
        g_data_buffer[g_data_buffer_tail].length = length;
        g_data_buffer_tail = (g_data_buffer_tail + 1) % SLE_UART_DATA_BUFFER_SIZE;
    } else {
        g_buffer_loss_count++;
    }
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_MASTER_B) && defined(CONFIG_UART_SUPPORT_FLOW_CTRL)
    if (uart_dma_lli_buffer_check_full()) {
        osal_irq_restore(lock);
        return false;
    }
#endif
    osal_irq_restore(lock);
    return true;
}

uint8_t *uart_dma_lli_buffer_get_data(uint16_t *length)
{
    uint32_t lock = osal_irq_lock();
    uint8_t *temp = g_data_buffer[g_data_buffer_head].data;
    *length = g_data_buffer[g_data_buffer_head].length;
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_MASTER_B) && defined(CONFIG_UART_SUPPORT_FLOW_CTRL)
    if (uart_dma_lli_buffer_check_full()) {
#if defined(CONFIG_SAMPLE_SUPPORT_UART_DMA_RAW_DATA_MODE)
        uapi_uart_dma_recv_raw_data(CONFIG_SLE_UART_DMA_BUS_ID, uart_dma_rx_cb);
#elif defined(CONFIG_SAMPLE_SUPPORT_UART_DMA_LLI_MODE)
        uapi_uart_dma_lli_continue_recv(CONFIG_SLE_UART_DMA_BUS_ID);
#endif
    }
#endif

    g_data_buffer_head = (g_data_buffer_head + 1) % SLE_UART_DATA_BUFFER_SIZE;
    osal_irq_restore(lock);
    return temp;
}
#endif

void sle_uart_recv_data_init(void)
{
    for (uint16_t i = 0; i < CONFIG_SLE_UART_DMA_MAX_MSG_LEN; i++) {
        g_recv_data[i] = (i + 1) & 0xff;
    }
    g_recv_data[DATA_CHECK_INDEX] = DATA_CHECK_MAX;
}

void sle_uart_send_data_init(uint8_t *send_data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        send_data[i] = (i + 1) & 0xff;
    }
    send_data[DATA_CHECK_INDEX] = DATA_CHECK_MAX;
}

void sle_uart_err_data_check(uint8_t *value, uint16_t len)
{
    if (memcmp(g_recv_data, value, len) != 0) {
        for (int i = 0; i < len; i++) {
            osal_printk("%d-%d ", g_recv_data[i], value[i]);
        }
        osal_printk("\r\n");
        g_recv_err_count++;
    }
}

bool sle_uart_loss_data_check(uint8_t *value, uint16_t len)
{
    unused(len);
    if (value[DATA_CHECK_INDEX] != g_recv_data[DATA_CHECK_INDEX]) {
        if (g_recv_data[DATA_CHECK_INDEX] < value[DATA_CHECK_INDEX]) {
            g_recv_loss_count += value[DATA_CHECK_INDEX] - g_recv_data[DATA_CHECK_INDEX];
        } else {
            g_recv_loss_count += DATA_CHECK_MAX - g_recv_data[DATA_CHECK_INDEX] + value[DATA_CHECK_INDEX];
        }
        osal_printk("sle_uart_loss_data_check, cur:%d, recv:%d\r\n",
            g_recv_data[DATA_CHECK_INDEX], value[DATA_CHECK_INDEX]);
        return true;
    }
    return false;
}

void sle_uart_data_check(uint8_t *value, uint16_t len)
{
    if (sle_uart_loss_data_check(value, len) != true) {
        sle_uart_err_data_check(value, len);
    }
    g_recv_data[DATA_CHECK_INDEX] = (value[DATA_CHECK_INDEX] + 1) % DATA_CHECK_MAX;
}

void sle_uart_performance_statistics(uint8_t *value, uint16_t len)
{
    if (value == NULL || len == 0) {
        return;
    }
#if defined(UART_DMA_LLI_DATA_CHECK)
    sle_uart_data_check(value, len);    // 检查丢包错包
#endif
    g_recv_count++;
    g_recv_data_count += len;
    if (g_recv_count == 1) {
        g_recv_start_time = uapi_tcxo_get_us();
    } else if (g_recv_count == SLE_UART_RECV_CNT) {
        g_recv_end_time = uapi_tcxo_get_us();
        uint64_t sle_recv_diff_time = g_recv_end_time - g_recv_start_time;
        uint64_t sle_recv_diff_time_in_ms = sle_recv_diff_time / 1000; // 1000 代表ms转化成s
        uint64_t sle_throughput = g_recv_data_count * 8 / sle_recv_diff_time_in_ms; // 8 代表1byte = 8bit
        osal_printk("recv_data_count = %llu, recv_count = %llu\r\n", g_recv_data_count, g_recv_count);
        osal_printk("diff time:%lluus, throughput:%llukbps\r\n", sle_recv_diff_time, sle_throughput);
        osal_printk("buffer_loss_cnt = %d, loss_cnt = %d, err_cnt = %d\r\n",
            g_buffer_loss_count, g_recv_loss_count, g_recv_err_count);
        g_recv_count = 0;
        g_recv_end_time = 0;
        g_recv_start_time = 0;
        g_recv_loss_count = 0;
        g_buffer_loss_count = 0;
        g_recv_err_count = 0;
        g_recv_data_count = 0;
    }
}

void sle_uart_dma_set_mcs(uint16_t conn_id, uint8_t mcs)
{
    if (sle_set_mcs(conn_id, mcs) != 0) {
        osal_printk("sle_set_mcs fail\r\n");
        return;
    }
    osal_printk("sle_set_mcs success, value:%d\r\n", mcs);
}

void sle_uart_send_wakeup_data(void)
{
    uint8_t data[UART_PAYLOAD_LEN];
    for (int i = 0; i < UART_PAYLOAD_LEN; i++) {
        data[i] = (i + 1) % 0xff;
    }
    uapi_uart_dma_send(CONFIG_SLE_UART_DMA_BUS_ID, data, UART_PAYLOAD_LEN, NULL);
    osal_printk("sle_uart_send_wakeup_data!\r\n");
}