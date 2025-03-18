/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: UART DMA LLI Master Sample Source. \n
 *
 * History: \n
 * 2024-06-13, Create file. \n
 */
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "app_init.h"
#include "pinctrl.h"
#include "uart.h"
#include "osal_debug.h"

#define UART_TASK_PRIO                     24
#define UART_TASK_DURATION_MS              500
#define UART_TASK_STACK_SIZE               0x1000
#define UART_PAYLOAD_LEN                   255

static uint8_t g_app_uart_tx_payload_buff[UART_PAYLOAD_LEN] = { 0 };
static uint8_t g_app_uart_rx_payload_buff[UART_PAYLOAD_LEN] = { 0 };
volatile uint32_t g_success = 0;
volatile uint32_t g_fail = 0;

static void uart_dma_lli_write_cb(uint8_t channel, errcode_t result)
{
    unused(result);
    unused(channel);
}

static bool uart_dma_lli_read_cb(uint8_t channel, uint16_t length, errcode_t result)
{
    unused(channel);
    unused(length);
    unused(result);
    if (memcmp(g_app_uart_rx_payload_buff, g_app_uart_tx_payload_buff, UART_PAYLOAD_LEN) == 0) {
        g_success++;
    } else {
        g_fail++;
        for (uint32_t i = 0; i < UART_PAYLOAD_LEN; i++) {
            osal_printk("[%d] tx:0x%x,rx:0x%x\r\n", i, g_app_uart_tx_payload_buff[i], g_app_uart_rx_payload_buff[i]);
        }
    }
    if ((g_success + g_fail) % 0x400 == 0) {
        osal_printk("g_success:%d\r\n", g_success);
        osal_printk("g_fail:%d\r\n", g_fail);
    }
    for (uint16_t i = 0; i < UART_PAYLOAD_LEN; i++) {
        g_app_uart_tx_payload_buff[i] = rand() % 0xff;
    }
    uapi_uart_dma_send(CONFIG_UART_BUS_ID, g_app_uart_tx_payload_buff, UART_PAYLOAD_LEN, uart_dma_lli_write_cb);
    return true;
}

static void uart_dma_lli_master_entry(void)
{
    for (uint16_t i = 0; i < UART_PAYLOAD_LEN; i++) {
        g_app_uart_tx_payload_buff[i] = rand() % 0xff;
    }
    uart_pin_config_t *pin_config_map;
    pin_config_map = uapi_uart_pin_cfg_get(CONFIG_UART_BUS_ID);
    uapi_pin_set_pull(pin_config_map->rx_pin, PIN_PULL_UP);
    uapi_pin_set_pull(pin_config_map->cts_pin, PIN_PULL_UP);
    uart_port_lli_init_config(CONFIG_UART_BUS_ID);
    uapi_uart_dma_recv_register(CONFIG_UART_BUS_ID, g_app_uart_rx_payload_buff, UART_PAYLOAD_LEN, uart_dma_lli_read_cb);
    uapi_uart_dma_send(CONFIG_UART_BUS_ID, g_app_uart_tx_payload_buff, UART_PAYLOAD_LEN, uart_dma_lli_write_cb);
    return;
}

/* Run the uart_dma_lli_master_entry. */
app_run(uart_dma_lli_master_entry);