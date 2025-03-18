/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: I2S Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-13, Create file. \n
 */
#include "pinctrl.h"
#include "i2s.h"
#include "tcxo.h"
#include "hal_sio.h"
#include "soc_osal.h"
#include "app_init.h"

#define I2S_DIV_NUM                24
#define I2S_NUMBER_OF_CHANNELS     2
#define TCXO_DELAY_MS              1000
#define I2S_TASK_DURATION_MS       500

#define I2S_TASK_PRIO              24
#define I2S_TASK_STACK_SIZE        0x1000

static uint32_t g_app_left_data[CONFIG_I2S_TRANSFER_LEN];
static uint32_t g_app_right_data[CONFIG_I2S_TRANSFER_LEN];
static i2s_tx_data_t g_app_write_data = {
    .left_buff = g_app_left_data,
    .right_buff = g_app_right_data,
    .length = CONFIG_I2S_TRANSFER_LEN,
};

void app_i2s_rx_callback(uint32_t *left_buff, uint32_t *right_buff, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++) {
        osal_printk("l: 0x%0x\r\n", left_buff[i]);
        osal_printk("r: 0x%0x\r\n", right_buff[i]);
    }
}

static void *i2s_master_task(const char *arg)
{
    unused(arg);
    i2s_config_t config = {
        .drive_mode= MASTER,
        .transfer_mode = STD_MODE,
        .data_width = TWENTY_FOUR_BIT,
        .channels_num = TWO_CH,
        .timing = NONE_TIMING_MODE,
        .clk_edge = RISING_EDGE,
        .div_number = I2S_DIV_NUM,
        .number_of_channels = I2S_NUMBER_OF_CHANNELS,
    };
    uint32_t i2s_first_data = 0x100000;
    for (uint32_t i = 0; i < CONFIG_I2S_TRANSFER_LEN; i++) {
        g_app_left_data[i] = i2s_first_data;
        g_app_right_data[i] = i2s_first_data;
        i2s_first_data++;
    }
    sio_porting_i2s_pinmux();
    /* I2S init. */
    uapi_i2s_deinit(CONFIG_I2S_MASTER_BUS_ID);
    uapi_i2s_init(CONFIG_I2S_MASTER_BUS_ID, app_i2s_rx_callback);
    uapi_i2s_set_config(CONFIG_I2S_MASTER_BUS_ID, &config);

    osal_printk("i2s master write start!\r\n");
    while (1) {
        osal_msleep(I2S_TASK_DURATION_MS);
        uapi_i2s_write_data(CONFIG_I2S_MASTER_BUS_ID, &g_app_write_data);
        uapi_tcxo_delay_ms(TCXO_DELAY_MS);
    }

    return NULL;
}

static void i2s_master_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)i2s_master_task, 0, "I2sMasterTask", I2S_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2S_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the i2s_master_entry. */
app_run(i2s_master_entry);