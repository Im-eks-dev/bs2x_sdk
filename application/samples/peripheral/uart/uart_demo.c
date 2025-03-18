/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: UART Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-06-29, Create file. \n
 */
#include "pinctrl.h"
#include "uart.h"
#include "watchdog.h"
#include "soc_osal.h"
#include "app_init.h"
#if defined(CONFIG_UART_SUPPORT_DMA)
#include "dma.h"
#include "hal_dma.h"
#endif

#define UART_BAUDRATE                      115200
#define CONFIG_UART_INT_WAIT_MS            5

#define UART_TASK_PRIO                     24
#define UART_TASK_STACK_SIZE               0x1000

static uint8_t g_app_uart_rx_buff[CONFIG_UART_TRANSFER_SIZE] = { 0 };
#if defined(CONFIG_UART_SUPPORT_INT_MODE)
static uint8_t g_app_uart_int_rx_flag = 0;
static volatile uint16_t g_app_uart_int_index = 0;
static uint8_t g_app_uart_int_rx_buff[CONFIG_UART_TRANSFER_SIZE] = { 0 };
static void app_uart_register_rx_callback(void);
#endif
static uart_buffer_config_t g_app_uart_buffer_config = {
    .rx_buffer = g_app_uart_rx_buff,
    .rx_buffer_size = CONFIG_UART_TRANSFER_SIZE
};

#if defined(CONFIG_UART_SUPPORT_DMA)
uart_write_dma_config_t g_app_dma_cfg = {
    .src_width = HAL_DMA_TRANSFER_WIDTH_8,
    .dest_width = HAL_DMA_TRANSFER_WIDTH_8,
    .burst_length = HAL_DMA_BURST_TRANSACTION_LENGTH_1,
    .priority = HAL_DMA_CH_PRIORITY_0
};
#endif

static void app_uart_init_pin(void)
{
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_UART_RXD_PIN, PIN_IE_1);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
    uapi_pin_set_mode(CONFIG_UART_TXD_PIN, CONFIG_UART_TXD_PIN_MODE);
    uapi_pin_set_mode(CONFIG_UART_RXD_PIN, CONFIG_UART_RXD_PIN_MODE);
}

static void app_uart_init_config(void)
{
    uart_attr_t attr = {
        .baud_rate = UART_BAUDRATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };

    uart_pin_config_t pin_config = {
        .tx_pin = CONFIG_UART_TXD_PIN,
        .rx_pin = CONFIG_UART_RXD_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };

#if defined(CONFIG_UART_SUPPORT_DMA)
    uart_extra_attr_t extra_attr = {
        .tx_dma_enable = true,
        .tx_int_threshold = UART_FIFO_INT_TX_LEVEL_EQ_0_CHARACTER,
        .rx_dma_enable = true,
        .rx_int_threshold = UART_FIFO_INT_RX_LEVEL_1_CHARACTER
    };
    uapi_dma_init();
    uapi_dma_open();
    uapi_uart_deinit(CONFIG_UART_BUS_ID);
    uapi_uart_init(CONFIG_UART_BUS_ID, &pin_config, &attr, &extra_attr, &g_app_uart_buffer_config);
#else
    uapi_uart_deinit(CONFIG_UART_BUS_ID);
    uapi_uart_init(CONFIG_UART_BUS_ID, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
#endif
#if defined(CONFIG_UART_SUPPORT_INT_MODE)
    app_uart_register_rx_callback();
#endif
}

#if defined(CONFIG_UART_SUPPORT_INT_MODE)
static void app_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    if (buffer == NULL || length == 0) {
        osal_printk("uart%d int mode transfer illegal data!\r\n", CONFIG_UART_BUS_ID);
        return;
    }

    uint8_t *buff = (uint8_t *)buffer;
    if (memcpy_s(g_app_uart_int_rx_buff + g_app_uart_int_index, length, buff, length) != EOK) {
        g_app_uart_int_index = 0;
        osal_printk("uart%d int mode data2 copy fail!\r\n", CONFIG_UART_BUS_ID);
    }
    g_app_uart_int_index += length;
    if (g_app_uart_int_index >= CONFIG_UART_TRANSFER_SIZE) {
        g_app_uart_int_rx_flag = 1;
        g_app_uart_int_index = 0;
    }
}

static void app_uart_write_int_handler(const void *buffer, uint32_t length, const void *params)
{
    unused(params);
    uint8_t *buff = (void *)buffer;
    for (uint16_t i = 0; i < length; i++) {
        osal_printk("uart%d write data[%d] = %d\r\n", CONFIG_UART_BUS_ID, i, buff[i]);
    }
}

static void app_uart_register_rx_callback(void)
{
    osal_printk("uart%d int mode register receive callback start!\r\n", CONFIG_UART_BUS_ID);
    if (uapi_uart_register_rx_callback(CONFIG_UART_BUS_ID, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                       1, app_uart_read_int_handler) == ERRCODE_SUCC) {
        osal_printk("uart%d int mode register receive callback succ!\r\n", CONFIG_UART_BUS_ID);
    }
}
#endif

static void *uart_task(const char *arg)
{
    unused(arg);
#if defined(CONFIG_UART_SUPPORT_DMA)
    int32_t ret = CONFIG_UART_TRANSFER_SIZE;
#if defined(CONFIG_UART_USING_V151)
    ret = ERRCODE_SUCC;
#endif
#endif
    /* UART pinmux. */
    app_uart_init_pin();

    /* UART init config. */
    app_uart_init_config();

    while (1) {
        (void)uapi_watchdog_kick();
#if defined(CONFIG_UART_SUPPORT_INT_MODE)
        while (g_app_uart_int_rx_flag != 1) {
            osal_msleep(CONFIG_UART_INT_WAIT_MS);
            (void)uapi_watchdog_kick();
        }
        g_app_uart_int_rx_flag = 0;
        osal_printk("uart%d int mode send back!\r\n", CONFIG_UART_BUS_ID);
        if (uapi_uart_write_int(CONFIG_UART_BUS_ID, g_app_uart_int_rx_buff, CONFIG_UART_TRANSFER_SIZE, 0,
                                app_uart_write_int_handler) == ERRCODE_SUCC) {
            osal_printk("uart%d int mode send back succ!\r\n", CONFIG_UART_BUS_ID);
        }
#elif defined(CONFIG_UART_SUPPORT_DMA)
        osal_printk("uart%d dma mode receive start!\r\n", CONFIG_UART_BUS_ID);
        if (uapi_uart_read_by_dma(CONFIG_UART_BUS_ID, g_app_uart_rx_buff, CONFIG_UART_TRANSFER_SIZE,
            &g_app_dma_cfg) == ret) {
            osal_printk("uart%d dma mode receive succ!\r\n", CONFIG_UART_BUS_ID);
        }
        osal_printk("uart%d dma mode send back!\r\n", CONFIG_UART_BUS_ID);
        if (uapi_uart_write_by_dma(CONFIG_UART_BUS_ID, g_app_uart_rx_buff, CONFIG_UART_TRANSFER_SIZE,
            &g_app_dma_cfg) == ret) {
            osal_printk("uart%d dma mode send back succ!\r\n", CONFIG_UART_BUS_ID);
        }
#else
        osal_printk("uart%d poll mode receive start!\r\n", CONFIG_UART_BUS_ID);
        if (uapi_uart_read(CONFIG_UART_BUS_ID, g_app_uart_rx_buff, CONFIG_UART_TRANSFER_SIZE,
            0) == CONFIG_UART_TRANSFER_SIZE) {
            osal_printk("uart%d poll mode receive succ!\r\n", CONFIG_UART_BUS_ID);
        }
        osal_printk("uart%d poll mode send back!\r\n", CONFIG_UART_BUS_ID);
        if (uapi_uart_write(CONFIG_UART_BUS_ID, g_app_uart_rx_buff, CONFIG_UART_TRANSFER_SIZE,
            0) == CONFIG_UART_TRANSFER_SIZE) {
            osal_printk("uart%d poll mode send back succ!\r\n", CONFIG_UART_BUS_ID);
        }
#endif
    }

    return NULL;
}

static void uart_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)uart_task, 0, "UartTask", UART_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, UART_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the uart_entry. */
app_run(uart_entry);