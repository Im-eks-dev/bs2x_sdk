/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: I2C Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-25, Create file. \n
 */
#include "pinctrl.h"
#include "i2c.h"
#include "soc_osal.h"
#include "app_init.h"
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#include "dma.h"
#endif

#define I2C_MASTER_ADDR                   0x0
#define I2C_SLAVE_ADDR                    0x8
#define I2C_SET_BAUDRATE                  500000
#define I2C_TASK_DURATION_MS              500
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
#define I2C_INT_TRANSFER_DELAY_MS         800
#endif

#define I2C_TASK_PRIO                     24
#define I2C_TASK_STACK_SIZE               0x1000

static i2c_data_t data = { 0 };
static uint8_t tx_buff[CONFIG_I2C_TRANSFER_LEN] = { 0 };
static uint8_t rx_buff[CONFIG_I2C_TRANSFER_LEN] = { 0 };

static void app_i2c_init_pin(void)
{
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_I2C_MASTER_SDA_PIN, PIN_IE_1);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
    /* I2C pinmux. */
    uapi_pin_set_mode(CONFIG_I2C_MASTER_SCL_PIN, CONFIG_I2C_MASTER_SCL_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_MASTER_SDA_PIN, CONFIG_I2C_MASTER_SDA_PIN_MODE);
}

static void app_i2c_data_config(void)
{
    /* I2C data config. */
    for (uint32_t loop = 0; loop < CONFIG_I2C_TRANSFER_LEN; loop++) {
        tx_buff[loop] = (loop & 0xFF);
    }
    data.send_buf = tx_buff;
    data.send_len = CONFIG_I2C_TRANSFER_LEN;
    data.receive_buf = rx_buff;
    data.receive_len = CONFIG_I2C_TRANSFER_LEN;
}

static void *i2c_master_task(const char *arg)
{
    unused(arg);

    uint32_t baudrate = I2C_SET_BAUDRATE;
    uint8_t hscode = I2C_MASTER_ADDR;
    uint16_t dev_addr = I2C_SLAVE_ADDR;

#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
    uapi_dma_init();
    uapi_dma_open();
#ifndef CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH
    uapi_i2c_set_dma_mode(CONFIG_I2C_MASTER_BUS_ID, true);
#endif
#endif  /* CONFIG_I2C_SUPPORT_DMA */

    /* I2C master init config. */
    app_i2c_init_pin();
    uapi_i2c_master_init(CONFIG_I2C_MASTER_BUS_ID, baudrate, hscode);

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    uapi_i2c_set_irq_mode(CONFIG_I2C_MASTER_BUS_ID, true);
#endif  /* CONFIG_I2C_SUPPORT_INT */

    app_i2c_data_config();

    while (1) {
        osal_msleep(I2C_TASK_DURATION_MS);
#ifndef CONFIG_I2C_MASTER_SUPPORT_WRITEREAD
        osal_printk("i2c%d master send start!\r\n", CONFIG_I2C_MASTER_BUS_ID);
        if (uapi_i2c_master_write(CONFIG_I2C_MASTER_BUS_ID, dev_addr, &data) == ERRCODE_SUCC) {
            osal_printk("i2c%d master send succ!\r\n", CONFIG_I2C_MASTER_BUS_ID);
        } else {
            continue;
        }
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
        osal_msleep(I2C_INT_TRANSFER_DELAY_MS);
#endif
        osal_printk("i2c%d master receive start!\r\n", CONFIG_I2C_MASTER_BUS_ID);
        if (uapi_i2c_master_read(CONFIG_I2C_MASTER_BUS_ID, dev_addr, &data) == ERRCODE_SUCC) {
            for (uint32_t i = 0; i < data.receive_len; i++) {
                osal_printk("i2c%d master receive data is %x\r\n", CONFIG_I2C_MASTER_BUS_ID, data.receive_buf[i]);
            }
            osal_printk("i2c%d master receive succ!\r\n", CONFIG_I2C_MASTER_BUS_ID);
        }
#else
        osal_printk("i2c%d master writeread start!\r\n", CONFIG_I2C_MASTER_BUS_ID);
        if (uapi_i2c_master_writeread(CONFIG_I2C_MASTER_BUS_ID, dev_addr, &data) == ERRCODE_SUCC) {
            for (uint32_t i = 0; i < data.receive_len; i++) {
                osal_printk("i2c%d master writeread data is %x\r\n", CONFIG_I2C_MASTER_BUS_ID, data.receive_buf[i]);
            }
            osal_printk("i2c%d master writeread succ!\r\n", CONFIG_I2C_MASTER_BUS_ID);
        }
#endif
    }

    return NULL;
}

static void i2c_master_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)i2c_master_task, 0, "I2cMasterTask", I2C_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2C_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the i2c_master_entry. */
app_run(i2c_master_entry);