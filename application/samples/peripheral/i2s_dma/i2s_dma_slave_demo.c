/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: i2s Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-18, Create file. \n
 */
#include "i2s.h"
#include "watchdog.h"
#include "hal_sio.h"
#include "hal_dma.h"
#include "soc_osal.h"
#include "app_init.h"

#define I2S_DIV_NUMBER              32
#define I2S_CHANNEL_NUMBER          2
#define I2S_TX_INT_THRESHOLD        7
#define I2S_RX_INT_THRESHOLD        1
#define I2S_DMA_TRANSFER_EVENT      1
#define I2S_RING_BUFFER_NUMBER      4

#define I2S_TASK_PRIO               24
#define I2S_TASK_STACK_SIZE         0xc00

static uint32_t g_i2s_dma_data0[CONFIG_I2S_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t g_i2s_dma_data1[CONFIG_I2S_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t g_i2s_dma_data2[CONFIG_I2S_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t g_i2s_dma_data3[CONFIG_I2S_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t *g_i2s_dma_data[I2S_RING_BUFFER_NUMBER] = {
    g_i2s_dma_data0, g_i2s_dma_data1, g_i2s_dma_data2, g_i2s_dma_data3};
static uint8_t g_read_buffer_state = 0;
static osal_event g_i2s_dma_id;

static void i2s_dma_transfer_restart(void);

static int32_t i2s_start_dma_transfer(uint32_t *i2s_buffer, dma_transfer_cb_t trans_done)
{
    dma_ch_user_peripheral_config_t transfer_config;
    uint8_t channel = 0;

    transfer_config.src = i2s_porting_rx_merge_data_addr_get(SIO_BUS_0);
    transfer_config.dest = (uint32_t)(uintptr_t)i2s_buffer;
    transfer_config.transfer_num = (uint16_t)CONFIG_I2S_TRANSFER_LEN_OF_DMA;
    transfer_config.src_handshaking = HAL_DMA_HANDSHAKING_I2S_RX;
    transfer_config.dest_handshaking = 0;
    transfer_config.trans_type = HAL_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA;
    transfer_config.trans_dir = HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM;
    transfer_config.priority = 0;
    transfer_config.src_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.dest_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.burst_length = 0;
    transfer_config.src_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    transfer_config.dest_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    transfer_config.protection = HAL_DMA_PROTECTION_CONTROL_BUFFERABLE;

    errcode_t ret = uapi_dma_configure_peripheral_transfer_single(&transfer_config, &channel,
                                                                  trans_done, (uintptr_t)NULL);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Configure the DMA fail. %x\r\n", "i2s dma", ret);
        return 1;
    }
    ret = uapi_dma_start_transfer(channel);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Start the DMA fail. %x\r\n", "i2s dma", ret);
        return 1;
    }
    hal_sio_set_rx_enable(SIO_BUS_0, 1);
    return 0;
}

static void i2s_dma_trans_done_callback(uint8_t intr, uint8_t channel, uintptr_t arg)
{
    unused(channel);
    unused(arg);
    switch (intr) {
        case HAL_DMA_INTERRUPT_TFR:
            i2s_dma_transfer_restart();
            break;
        case HAL_DMA_INTERRUPT_ERR:
            osal_printk("i2s DMA transfer error.\r\n");
            break;
        default:
            break;
    }
}

static void i2s_dma_transfer_restart(void)
{
    g_read_buffer_state = (g_read_buffer_state + 1) % I2S_RING_BUFFER_NUMBER;
    if (osal_event_write(&g_i2s_dma_id, I2S_DMA_TRANSFER_EVENT) != OSAL_SUCCESS) {
        osal_printk("osal_event_write fail!\r\n");
        return;
    }
    if (i2s_start_dma_transfer(g_i2s_dma_data[g_read_buffer_state], i2s_dma_trans_done_callback) != 0) {
        return;
    }
}

static void *i2s_dma_slave_task(const char *arg)
{
    unused(arg);
    if (osal_event_init(&g_i2s_dma_id) != OSAL_SUCCESS) {
        return NULL;
    }
    uapi_i2s_deinit(SIO_BUS_0);
    uapi_dma_deinit();
    uapi_i2s_init(SIO_BUS_0, NULL);
    sio_porting_i2s_pinmux();
    i2s_config_t config = {
        .drive_mode= SLAVE,
        .transfer_mode = STD_MODE,
        .data_width = THIRTY_TWO_BIT,
        .channels_num = TWO_CH,
        .timing = NONE_TIMING_MODE,
        .clk_edge = RISING_EDGE,
        .div_number = I2S_DIV_NUMBER,
        .number_of_channels = I2S_CHANNEL_NUMBER,
    };
    i2s_dma_attr_t attr = {
        .tx_dma_enable = 0,
        .tx_int_threshold = I2S_TX_INT_THRESHOLD,
        .rx_dma_enable = 1,
        .rx_int_threshold = I2S_RX_INT_THRESHOLD,
    };

    uapi_i2s_set_config(SIO_BUS_0, &config);
    uapi_i2s_dma_config(SIO_BUS_0, &attr);

    /* DMA init. */
    uapi_dma_init();
    uapi_dma_open();
    if (i2s_start_dma_transfer(g_i2s_dma_data[g_read_buffer_state], i2s_dma_trans_done_callback) != 0) {
        return NULL;
    }
    osal_printk("DMA slave receive start.\r\n");
    while (1) {
        if (!(osal_event_read(&g_i2s_dma_id, I2S_DMA_TRANSFER_EVENT, OSAL_WAIT_FOREVER,
                              OSAL_WAITMODE_AND | OSAL_WAITMODE_CLR))) {
            uapi_watchdog_kick();
            continue;
        }
    }
    return NULL;
}

static void i2s_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)i2s_dma_slave_task, 0, "I2sDmaSlaveTask",
                                      I2S_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2S_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the i2s_entry. */
app_run(i2s_entry);