/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: i2s Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-18, Create file. \n
 */

#include "osal_debug.h"
#include "tcxo.h"
#include "i2s.h"
#include "hal_sio.h"
#include "sio_porting.h"
#include "securec.h"
#include "dma.h"
#include "hal_dma.h"
#include "watchdog.h"
#include "app_init.h"
#include "soc_osal.h"

#define I2S_TASK_STACK_SIZE         0xc00
#define I2S_TASK_PRIO               24
#define DATA_WIDTH                  24
#define NUMBER_OF_CHANNELS          2
#define I2S_DMA_TRANSFER_EVENT      1

#define I2S_TRANS_SRC_ADDR          0x520300c0

static uint32_t i2s_first_data = 0x100000;
static uint32_t g_i2s_send_dma_data[CONFIG_I2S_TRANSFER_LEN_OF_DMA] = { 0 };
static osal_event g_i2s_dma_id;

static int32_t i2s_start_dma_transfer(uint32_t *i2s_buffer, dma_transfer_cb_t trans_done)
{
    dma_ch_user_peripheral_config_t transfer_config;
    uint8_t channel = 0;

    unused(transfer_config);

    transfer_config.src = (uint32_t)(uintptr_t)i2s_buffer;
    transfer_config.dest = I2S_TRANS_SRC_ADDR;
    transfer_config.transfer_num = (uint16_t)CONFIG_I2S_TRANSFER_LEN_OF_DMA;
    transfer_config.src_handshaking = 0;
    transfer_config.dest_handshaking = HAL_DMA_HANDSHAKING_I2S_TX;
    transfer_config.trans_type = HAL_DMA_TRANS_MEMORY_TO_PERIPHERAL_DMA;
    transfer_config.trans_dir = HAL_DMA_TRANSFER_DIR_MEM_TO_PERIPHERAL;
    transfer_config.priority = 0;
    transfer_config.src_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.dest_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.burst_length = 0;
    transfer_config.src_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    transfer_config.dest_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
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

    return 0;
}

static void i2s_dma_trans_done_callback(uint8_t intr, uint8_t channel, uintptr_t arg)
{
    unused(channel);
    unused(arg);
    switch (intr) {
        case HAL_DMA_INTERRUPT_TFR:
            sio_porting_bclk_clock_enable(0);
            if (osal_event_write(&g_i2s_dma_id, I2S_DMA_TRANSFER_EVENT) != OSAL_SUCCESS) {
                break;
            }
            break;
        case HAL_DMA_INTERRUPT_ERR:
            osal_printk("i2s DMA transfer error.\r\n");
            break;
        default:
            break;
    }
}

static void *i2s_dma_master_task(const char *arg)
{
    unused(arg);

    if (osal_event_init(&g_i2s_dma_id) != OSAL_SUCCESS) {
        return NULL;
    }
    uapi_i2s_init(SIO_BUS_0, NULL);
    i2s_config_t config = {
        .drive_mode= MASTER,
        .transfer_mode = STD_MODE,
        .data_width = TWENTY_FOUR_BIT,
        .channels_num = TWO_CH,
        .timing = NONE_MODE,
        .clk_edge = NONE_EDGE,
        .div_number = DATA_WIDTH,
        .number_of_channels = NUMBER_OF_CHANNELS,
    };
    uapi_i2s_set_config(SIO_BUS_0, &config);
    sio_porting_i2s_pinmux();

    i2s_dma_attr_t attr = {
        .tx_dma_enable = 1,
        .tx_int_threshold = 8,
        .rx_dma_enable = 1,
        .rx_int_threshold = 8,
    };
    uapi_i2s_dma_config(SIO_BUS_0, &attr);

    /* DMA init. */
    uapi_dma_init();
    uapi_dma_open();
    for (uint32_t i = 0; i < CONFIG_I2S_TRANSFER_LEN_OF_DMA; i++) {
        g_i2s_send_dma_data[i] = i2s_first_data;
        i2s_first_data++;
    }
    osal_printk("DMA transfer start.\r\n");
    while (1) {
        if (i2s_start_dma_transfer(g_i2s_send_dma_data, i2s_dma_trans_done_callback) != 0) {
                return NULL;
        }
        while (!(osal_event_read(&g_i2s_dma_id, I2S_DMA_TRANSFER_EVENT, OSAL_WAIT_FOREVER,
                                 OSAL_WAITMODE_AND | OSAL_WAITMODE_CLR) & I2S_DMA_TRANSFER_EVENT)) {
            uapi_watchdog_kick();
        }
        sio_porting_bclk_clock_enable(1);
    }
    return NULL;
}

static void i2s_entry(void)
{
    osal_task *task_handle;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)i2s_dma_master_task, 0, "I2sDmaMasterTask",
                                      I2S_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2S_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the i2s_entry. */
app_run(i2s_entry);