/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: UART DMA LLI Slave Sample Source. \n
 *
 * History: \n
 * 2024-06-13, Create file. \n
 */
#include "app_init.h"
#include "uart.h"
#include "osal_debug.h"
#include "pinctrl.h"

#define UART_TASK_PRIO                     24
#define UART_TASK_DURATION_MS              500
#define UART_TASK_STACK_SIZE               0x1000
#define UART_PAYLOAD_LEN                   255

static uint8_t g_app_uart_rx_payload_buff[UART_PAYLOAD_LEN] = { 0 };
static uint8_t g_app_uart_rx_payload_buff1[UART_PAYLOAD_LEN] = { 0 };
static bool g_double_rx_buffer = true;
volatile uint32_t g_recv_count = 0;

static void uart_dma_lli_write_cb(uint8_t channel, errcode_t result)
{
    unused(result & channel);
}

static bool uart_dma_lli_read_cb(uint8_t channel, uint16_t length, errcode_t result)
{
    unused(channel);
    unused(length);
    unused(result);
    if (g_recv_count++ % 0x200 == 0) {
        osal_printk("g_recv_count:%d\r\n", g_recv_count);
    }
    if (g_double_rx_buffer) {
        uapi_uart_dma_send(CONFIG_UART_BUS_ID, g_app_uart_rx_payload_buff, UART_PAYLOAD_LEN, uart_dma_lli_write_cb);
        uapi_uart_update_dma_recv_buff(CONFIG_UART_BUS_ID, g_app_uart_rx_payload_buff1, UART_PAYLOAD_LEN);
        g_double_rx_buffer = false;
    } else {
        uapi_uart_dma_send(CONFIG_UART_BUS_ID, g_app_uart_rx_payload_buff1, UART_PAYLOAD_LEN, uart_dma_lli_write_cb);
        uapi_uart_update_dma_recv_buff(CONFIG_UART_BUS_ID, g_app_uart_rx_payload_buff, UART_PAYLOAD_LEN);
        g_double_rx_buffer = true;
    }
    return true;
}

static void uart_dma_lli_slave_entry(void)
{
    uart_pin_config_t *pin_config_map;
    pin_config_map = uapi_uart_pin_cfg_get(CONFIG_UART_BUS_ID);
    uapi_pin_set_pull(pin_config_map->rx_pin, PIN_PULL_UP);
    uapi_pin_set_pull(pin_config_map->cts_pin, PIN_PULL_UP);
    uart_port_lli_init_config(CONFIG_UART_BUS_ID);

    uapi_uart_dma_recv_register(CONFIG_UART_BUS_ID, g_app_uart_rx_payload_buff, UART_PAYLOAD_LEN,
                                uart_dma_lli_read_cb);
    return;
}

/* Run the uart_dma_lli_slave_entry. */
app_run(uart_dma_lli_slave_entry);