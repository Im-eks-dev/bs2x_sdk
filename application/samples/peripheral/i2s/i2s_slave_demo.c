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
#include "hal_sio.h"
#include "soc_osal.h"
#include "app_init.h"

#define I2S_DIV_NUM                24
#define I2S_NUMBER_OF_CHANNELS     2

#define I2S_TASK_PRIO              24
#define I2S_TASK_STACK_SIZE        0x1000


void app_i2s_rx_callback(uint32_t *left_buff, uint32_t *right_buff, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++) {
        osal_printk("l: 0x%0x\r\n", left_buff[i]);
        osal_printk("r: 0x%0x\r\n", right_buff[i]);
    }
}

static void *i2s_slave_task(const char *arg)
{
    unused(arg);
    i2s_config_t config = {
        .drive_mode= SLAVE,
        .transfer_mode = STD_MODE,
        .data_width = TWENTY_FOUR_BIT,
        .channels_num = TWO_CH,
        .timing = NONE_TIMING_MODE,
        .clk_edge = RISING_EDGE,
        .div_number = I2S_DIV_NUM,
        .number_of_channels = I2S_NUMBER_OF_CHANNELS,
    };
    sio_porting_i2s_pinmux();
    /* I2S init. */
    uapi_i2s_deinit(CONFIG_I2S_SLAVE_BUS_ID);
    uapi_i2s_init(CONFIG_I2S_SLAVE_BUS_ID, app_i2s_rx_callback);
    uapi_i2s_set_config(CONFIG_I2S_SLAVE_BUS_ID, &config);

    osal_printk("i2s slave read start!\r\n");
    uapi_i2s_read_start(CONFIG_I2S_SLAVE_BUS_ID);

    return NULL;
}

static void i2s_slave_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)i2s_slave_task, 0, "I2sSlaveTask", I2S_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2S_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the i2s_slave_entry. */
app_run(i2s_slave_entry);