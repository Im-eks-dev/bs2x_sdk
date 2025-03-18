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
volatile int16_t g_uart_dma_lli_buffer_count = CONFIG_UART_DMA_LLI_MAX_MEMBER;
void uart_dma_lli_write_cb(uint8_t channel, errcode_t result)
{
    unused(channel);
    unused(result);
    g_uart_dma_lli_buffer_count++;
}

static void *uart_dma_lli_master_task(const char *arg)
{
    unused(arg);
    // 防止收到异常数据;会导致倒灌风险，影响芯片下电
    uart_pin_config_t *pin_config_map;
    pin_config_map = uapi_uart_pin_cfg_get(CONFIG_SLE_UART_DMA_BUS_ID);
    uapi_pin_set_pull(pin_config_map->rx_pin, PIN_PULL_UP);
    uapi_pin_set_pull(pin_config_map->cts_pin, PIN_PULL_UP);
    uart_port_lli_init_config(CONFIG_SLE_UART_DMA_BUS_ID);
    sle_uart_recv_data_init();
    while (1) {
        uapi_watchdog_kick();
        if (!uart_dma_lli_buffer_check_empty() && (g_uart_dma_lli_buffer_count > 0)) {
            g_uart_dma_lli_buffer_count--;
            uint16_t len = 0;
            uint8_t *data = uart_dma_lli_buffer_get_data(&len);
            uapi_uart_dma_send(CONFIG_SLE_UART_DMA_BUS_ID, data, len, uart_dma_lli_write_cb);
        }
    }
    return NULL;
}

static void uart_dma_lli_master_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    sle_uart_client_sample_dev_cbk_register();
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
    osal_printk("uart_dam_lli_slave_sle_C start!\r\n");
}

/* Run the uart_dma_lli_master_entry. */
app_run(uart_dma_lli_master_entry);