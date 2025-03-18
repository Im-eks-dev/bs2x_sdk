/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SPI Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-06-25, Create file. \n
 */
#include "pinctrl.h"
#include "spi.h"
#include "soc_osal.h"
#include "app_init.h"

#define SPI_SLAVE_NUM                   1
#define SPI_FREQUENCY                   2
#define SPI_CLK_POLARITY                0
#define SPI_CLK_PHASE                   0
#define SPI_FRAME_FORMAT                0
#define SPI_FRAME_FORMAT_STANDARD       0
#define SPI_FRAME_SIZE                  0x1f
#define SPI_TMOD                        0
#define SPI_WAIT_CYCLES                 0x10
#if defined(CONFIG_SPI_SUPPORT_DMA) && !(defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH))
#define SPI_DMA_WIDTH                   2
#endif

#define SPI_TASK_DURATION_MS            500
#define SPI_TASK_PRIO                   24
#define SPI_TASK_STACK_SIZE             0x1000

static void app_spi_init_pin(void)
{
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_SPI_SLAVE_CLK_PIN, PIN_IE_1);
    uapi_pin_set_ie(CONFIG_SPI_SLAVE_CS_PIN, PIN_IE_1);
    uapi_pin_set_ie(CONFIG_SPI_SLAVE_DI_PIN, PIN_IE_1);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
    uapi_pin_set_mode(CONFIG_SPI_SLAVE_CLK_PIN, CONFIG_SPI_SLAVE_CLK_PIN_MODE);
    uapi_pin_set_mode(CONFIG_SPI_SLAVE_CS_PIN, CONFIG_SPI_SLAVE_CS_PIN_MODE);
    uapi_pin_set_mode(CONFIG_SPI_SLAVE_DI_PIN, CONFIG_SPI_SLAVE_DI_PIN_MODE);
    uapi_pin_set_mode(CONFIG_SPI_SLAVE_DO_PIN, CONFIG_SPI_SLAVE_DO_PIN_MODE);
}

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
static void app_spi_slave_write_int_handler(const void *buffer, uint32_t length)
{
    unused(buffer);
    unused(length);
    osal_printk("spi slave write interrupt start!\r\n");
}

static void app_spi_slave_rx_callback(const void *buffer, uint32_t length, bool error)
{
    if (buffer == NULL || length == 0) {
        osal_printk("spi slave transfer illegal data!\r\n");
        return;
    }
    if (error) {
        osal_printk("app_spi_slave_read_int error!\r\n");
        return;
    }

    uint8_t *buff = (uint8_t *)buffer;
    for (uint32_t i = 0; i < length; i++) {
        osal_printk("buff[%d] = %x\r\n", i, buff[i]);
    }
    osal_printk("app_spi_slave_read_int success!\r\n");
}
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */

static void app_spi_slave_init_config(void)
{
    spi_attr_t config = { 0 };
    spi_extra_attr_t ext_config = { 0 };

    config.is_slave = true;
    config.slave_num = SPI_SLAVE_NUM;
    config.bus_clk = SPI_CLK_FREQ;
    config.freq_mhz = SPI_FREQUENCY;
    config.clk_polarity = SPI_CLK_POLARITY;
    config.clk_phase = SPI_CLK_PHASE;
    config.frame_format = SPI_FRAME_FORMAT;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    config.frame_size = SPI_FRAME_SIZE;
    config.tmod = SPI_TMOD;
    config.sste = 0;

    ext_config.qspi_param.wait_cycles = SPI_WAIT_CYCLES;

    uapi_spi_init(CONFIG_SPI_SLAVE_BUS_ID, &config, &ext_config);
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    uapi_dma_init();
    uapi_dma_open();
#ifndef CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH
    spi_dma_config_t dma_cfg = {
        .src_width = SPI_DMA_WIDTH,
        .dest_width = SPI_DMA_WIDTH,
        .burst_length = 0,
        .priority = 0
    };
    if (uapi_spi_set_dma_mode(CONFIG_SPI_SLAVE_BUS_ID, true, &dma_cfg) != ERRCODE_SUCC) {
        osal_printk("spi%d slave set dma mode fail!\r\n");
    }
#endif
#endif  /* CONFIG_SPI_SUPPORT_DMA */

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    if (uapi_spi_set_irq_mode(CONFIG_SPI_SLAVE_BUS_ID, true, app_spi_slave_rx_callback,
        app_spi_slave_write_int_handler) == ERRCODE_SUCC) {
        osal_printk("spi%d slave set irq mode succ!\r\n", CONFIG_SPI_SLAVE_BUS_ID);
    }
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
}

static void *spi_slave_task(const char *arg)
{
    unused(arg);
    /* SPI pinmux. */
    app_spi_init_pin();

    /* SPI slave init config. */
    app_spi_slave_init_config();

    /* SPI data config. */
    uint8_t tx_data[CONFIG_SPI_TRANSFER_LEN] = { 0 };
    for (uint32_t loop = 0; loop < CONFIG_SPI_TRANSFER_LEN; loop++) {
        tx_data[loop] = (loop & 0xFF);
    }
    uint8_t rx_data[CONFIG_SPI_TRANSFER_LEN] = { 0 };
    spi_xfer_data_t data = {
        .tx_buff = tx_data,
        .tx_bytes = CONFIG_SPI_TRANSFER_LEN,
        .rx_buff = rx_data,
        .rx_bytes = CONFIG_SPI_TRANSFER_LEN,
    };

    while (1) {
        osal_msleep(SPI_TASK_DURATION_MS);
#if !defined(CONFIG_SPI_SUPPORT_WRITEREAD) && !defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH)
        osal_printk("spi%d slave receive start!\r\n", CONFIG_SPI_SLAVE_BUS_ID);
        if (uapi_spi_slave_read(CONFIG_SPI_SLAVE_BUS_ID, &data, 0xFFFFFFFF) == ERRCODE_SUCC) {
#ifndef CONFIG_SPI_SUPPORT_INTERRUPT
            for (uint32_t i = 0; i < data.rx_bytes; i++) {
                osal_printk("spi%d slave receive data is %x\r\n", CONFIG_SPI_SLAVE_BUS_ID, data.rx_buff[i]);
            }
#endif
            osal_printk("spi%d slave receive succ!\r\n", CONFIG_SPI_SLAVE_BUS_ID);
        } else {
            continue;
        }
        osal_printk("spi%d slave send start!\r\n", CONFIG_SPI_SLAVE_BUS_ID);
        if (uapi_spi_slave_write(CONFIG_SPI_SLAVE_BUS_ID, &data, 0xFFFFFFFF) == ERRCODE_SUCC) {
            osal_printk("spi%d slave send succ!\r\n", CONFIG_SPI_SLAVE_BUS_ID);
        }
#else
        osal_printk("spi%d slave writeread start!\r\n", CONFIG_SPI_SLAVE_BUS_ID);
        if (uapi_spi_slave_writeread(CONFIG_SPI_SLAVE_BUS_ID, &data, 0xFFFFFFFF) == ERRCODE_SUCC) {
            for (uint32_t i = 0; i < data.rx_bytes; i++) {
                osal_printk("spi%d slave writeread data is %x\r\n", CONFIG_SPI_SLAVE_BUS_ID, data.rx_buff[i]);
            }
            osal_printk("spi%d slave writeread succ!\r\n", CONFIG_SPI_SLAVE_BUS_ID);
        }
#endif
    }

    return NULL;
}

static void spi_slave_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)spi_slave_task, 0, "SpiSlaveTask", SPI_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SPI_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the spi_slave_entry. */
app_run(spi_slave_entry);