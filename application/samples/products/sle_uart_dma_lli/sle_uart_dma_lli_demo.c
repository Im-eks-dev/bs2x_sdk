/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: UART DMA LLI Master Sample Source. \n
 *
 * History: \n
 * 2024-06-13, Create file. \n
 */
#include "app_init.h"
#include "osal_task.h"
#include "pm_clock.h"
#include "uart.h"
#include "tcxo.h"
#include "log_common.h"
#include "sle_uart_dma_lli_common.h"
#include "sle_uart_dma_lli_master.h"

#define UART_TASK_PRIO                     24
#define UART_TASK_DURATION_MS              500
#define UART_TASK_STACK_SIZE               0x1000
#define UART_2_LINE_DATA_DELAY             500

static uint8_t g_uart_send_buff[UART_PAYLOAD_LEN] = { 0 };
void uart_dma_lli_write_cb(uint8_t channel, errcode_t result)
{
    unused(channel);
    unused(result);
    g_uart_send_buff[DATA_CHECK_INDEX] = (g_uart_send_buff[DATA_CHECK_INDEX] + 1) % DATA_CHECK_MAX;
#if defined(CONFIG_SAMPLE_SUPPORT_UART_DMA_RAW_DATA_MODE) && !defined(CONFIG_UART_SUPPORT_FLOW_CTRL)
    uapi_tcxo_delay_us(UART_2_LINE_DATA_DELAY);
#endif
    uapi_uart_dma_send(CONFIG_SLE_UART_DMA_BUS_ID, g_uart_send_buff, UART_PAYLOAD_LEN, uart_dma_lli_write_cb);
}

#if defined(CONFIG_SAMPLE_SUPPORT_UART_DMA_RAW_DATA_MODE)
bool uart_dma_rx_cb(uint8_t *data, uint32_t length)
{
    sle_uart_performance_statistics(data, length);
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_MASTER_A)
    osal_printk("uart dma is ready!, len:%d\r\n", length);
    uapi_uart_dma_send(CONFIG_SLE_UART_DMA_BUS_ID, g_uart_send_buff, UART_PAYLOAD_LEN, uart_dma_lli_write_cb);
#endif
    return true;
}
#elif defined(CONFIG_SAMPLE_SUPPORT_UART_DMA_LLI_MODE)
static uint8_t g_uart_recv_buff[CONFIG_SLE_UART_DMA_MAX_MSG_LEN] = { 0 };
static bool uart_dma_lli_slave_rx_cb(uint8_t channel, uint16_t length, errcode_t result)
{
    unused(channel);
    unused(result);
    sle_uart_performance_statistics(g_uart_recv_buff, length);
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_MASTER_A)
    osal_printk("uart dma is ready!, len:%d\r\n", length);
    uapi_uart_dma_send(CONFIG_SLE_UART_DMA_BUS_ID, g_uart_send_buff, UART_PAYLOAD_LEN, uart_dma_lli_write_cb);
#endif
    return true;
}
#elif defined(CONFIG_SAMPLE_SUPPORT_UART_READ_BY_DMA_MODE)
static void test_uart_read_by_dma(void)
{
    uint32_t length;
    uint8_t g_test_uart_rx_buffer[UART_PAYLOAD_LEN] = { 0 };
    uart_write_dma_config_t dma_cfg = {
        .src_width = HAL_DMA_TRANSFER_WIDTH_8,              /* 0代表8bit */
        .dest_width = HAL_DMA_TRANSFER_WIDTH_8,             /* 0代表8bit */
        .burst_length = HAL_DMA_BURST_TRANSACTION_LENGTH_1, /* 代表1字节 */
        .priority = HAL_DMA_CH_PRIORITY_0                   /* 优先级0 */
    };
    length = uapi_uart_read_by_dma(CONFIG_SLE_UART_DMA_BUS_ID, g_test_uart_rx_buffer, UART_PAYLOAD_LEN, &dma_cfg);
    if (length == UART_PAYLOAD_LEN) {
        sle_uart_performance_statistics(g_test_uart_rx_buffer, length);
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_MASTER_A)
        osal_printk("uart dma is ready!, len:%d\r\n", length);
        uapi_uart_dma_send(CONFIG_SLE_UART_DMA_BUS_ID, g_uart_send_buff, UART_PAYLOAD_LEN, uart_dma_lli_write_cb);
#endif
    }
}
#endif

static void *uart_dma_lli_master_task(const char *arg)
{
    unused(arg);
    sle_uart_recv_data_init();
    sle_uart_send_data_init(g_uart_send_buff, UART_PAYLOAD_LEN);
    // 防止收到异常数据;会导致倒灌风险，影响芯片下电
    uart_pin_config_t *pin_config_map;
    pin_config_map = uapi_uart_pin_cfg_get(CONFIG_SLE_UART_DMA_BUS_ID);
    uapi_pin_set_pull(pin_config_map->rx_pin, PIN_PULL_UP);
    uapi_pin_set_pull(pin_config_map->cts_pin, PIN_PULL_UP);

    uart_port_lli_init_config(CONFIG_SLE_UART_DMA_BUS_ID);
#if defined(CONFIG_SAMPLE_SUPPORT_UART_DMA_RAW_DATA_MODE)
    uapi_uart_dma_recv_raw_data(CONFIG_SLE_UART_DMA_BUS_ID, uart_dma_rx_cb);
#if !defined(CONFIG_UART_SUPPORT_FLOW_CTRL)
    osal_irq_set_priority(UART_1_IRQN, 0x0);
#endif
#elif defined(CONFIG_SAMPLE_SUPPORT_UART_DMA_LLI_MODE)
    uapi_uart_dma_recv_register(CONFIG_SLE_UART_DMA_BUS_ID, g_uart_recv_buff,
        CONFIG_SLE_UART_DMA_MAX_MSG_LEN, uart_dma_lli_slave_rx_cb);
#elif defined(CONFIG_SAMPLE_SUPPORT_UART_READ_BY_DMA_MODE)
    while (1) {
        uapi_watchdog_kick();
        test_uart_read_by_dma();
    }
#endif

    return NULL;
}

static void uart_dma_lli_master_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)uart_dma_lli_master_task, 0, "UartDmaLLIMatserTask",
                                      UART_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, UART_TASK_PRIO);
    }
    osal_kthread_unlock();
    errcode_t ret = uapi_clock_control(CLOCK_CONTROL_FREQ_LEVEL_CONFIG, CLOCK_FREQ_LEVEL_HIGH);
    if (ret != ERRCODE_SUCC) {
        osal_printk("uapi_clock_control FAIL!\r\n");
    }
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_MASTER_A)
    osal_printk("sle_uart_dma_lli_master_A start!\r\n");
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_DMA_SLAVE_D)
    osal_printk("sle_uart_dma_lli_slave_D start!\r\n");
#endif
}

/* Run the uart_dma_lli_master_entry. */
app_run(uart_dma_lli_master_entry);