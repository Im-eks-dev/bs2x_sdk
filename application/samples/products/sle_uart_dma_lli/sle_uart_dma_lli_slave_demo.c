/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: UART DMA LLI Slave Sample Source. \n
 *
 * History: \n
 * 2024-06-13, Create file. \n
 */
#include "app_init.h"
#include "osal_task.h"
#include "pm_clock.h"
#include "uart.h"
#include "gpio.h"
#include "pinctrl.h"
#include "sle_low_latency.h"
#include "chip_core_irq.h"
#include "soc_osal.h"
#include "sle_uart_dma_lli_slave.h"
#include "sle_uart_dma_lli_slave_adv.h"
#include "sle_uart_dma_lli_common.h"

extern uint8_t gle_tx_acb_data_num_get(void);

#define UART_SEND_DATA_LEN                 1
#define UART_TASK_PRIO                     24
#define UART_TASK_DURATION_MS              5000
#define UART_TASK_STACK_SIZE               0x1000

// uart_dma 回调注册

#if defined(CONFIG_SAMPLE_SUPPORT_UART_DMA_RAW_DATA_MODE)
bool uart_dma_rx_cb(uint8_t *data, uint32_t length)
{
    sle_uart_performance_statistics(data, length);
    return uart_dma_lli_buffer_add_data(data, length);
}
#elif defined(CONFIG_SAMPLE_SUPPORT_UART_DMA_LLI_MODE)
static uint8_t g_uart_recv_buff[CONFIG_SLE_UART_DMA_MAX_MSG_LEN] = { 0 };
static bool uart_dma_lli_rx_cb(uint8_t channel, uint16_t length, errcode_t result)
{
    unused(channel);
    unused(result);
    uapi_watchdog_kick();
    sle_uart_performance_statistics(g_uart_recv_buff, length);
    return uart_dma_lli_buffer_add_data(g_uart_recv_buff, length);
}
#elif defined(CONFIG_SAMPLE_SUPPORT_UART_READ_BY_DMA_MODE)
static void test_uart_read_by_dma(void)
{
    if (uart_dma_lli_buffer_check_full()) {
        return;
    }
    int length = 0;
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
        uart_dma_lli_buffer_add_data(g_test_uart_rx_buffer, length);
    } else {
        osal_printk("uapi_uart_read_by_dma, read fail!, ret:0x%x\r\n", length);
    }
}
#endif

static void *uart_dma_lli_slave_task(const char *arg)
{
    unused(arg);
    sle_uart_recv_data_init();
    // 防止收到异常数据，会导致倒灌风险，影响芯片下电
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
        CONFIG_SLE_UART_DMA_MAX_MSG_LEN, uart_dma_lli_rx_cb);
#endif
    while (1) {
        uapi_watchdog_kick();
#if defined(CONFIG_SAMPLE_SUPPORT_UART_READ_BY_DMA_MODE)
        test_uart_read_by_dma();
#endif
#if defined(CONFIG_SAMPLE_SLE_SUPPORT_NORMAL_TYPE)
        if (sle_uart_client_is_connected() && !uart_dma_lli_buffer_check_empty() &&
            (gle_tx_acb_data_num_get() > 0)) {
            uint16_t len = 0;
            uint8_t *data = uart_dma_lli_buffer_get_data(&len);
            sle_uart_server_send_report_by_handle(data, len);
        }
#endif
    }
    return NULL;
}

static void uart_dma_lli_slave_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    sle_dev_register_cbks();
    task_handle = osal_kthread_create((osal_kthread_handler)uart_dma_lli_slave_task, 0, "UartDmaLLISlaveTask",
                                      UART_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, UART_TASK_PRIO);
    }
    osal_kthread_unlock();
    errcode_t ret = uapi_clock_control(CLOCK_CONTROL_FREQ_LEVEL_CONFIG, CLOCK_FREQ_LEVEL_HIGH);
    if (ret != ERRCODE_SUCC) {
        osal_printk("uapi_clock_control FAIL!\r\n");
    }
    osal_printk("uart_dam_lli_master_sle_B start!\r\n");
}

/* Run the uart_dma_lli_slave_entry. */
app_run(uart_dma_lli_slave_entry);