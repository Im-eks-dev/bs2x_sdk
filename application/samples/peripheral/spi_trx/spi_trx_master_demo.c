/**
 * Copyright (c) @CompanyNameMagicTag 2025-2025. All rights reserved. \n
 *
 * Description: SPI Full Duplex Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-01-07, Create file. \n
 */
#include "pinctrl.h"
#include "spi.h"
#include "soc_osal.h"
#include "app_init.h"
#if defined(CONFIG_SAMPLE_SUPPORT_SPI_TRX_PERFORMANCE_TEST)
#include "tcxo.h"
#endif

#define SPI_TRX_SLAVE_NUM                   1
#define SPI_TRX_FREQUENCY                   2
#define SPI_TRX_CLK_POLARITY                0
#define SPI_TRX_CLK_PHASE                   0
#define SPI_TRX_FRAME_FORMAT                0
#define SPI_TRX_FRAME_FORMAT_STANDARD       0
#define SPI_TRX_FRAME_SIZE                  0x1f
#define SPI_TRX_WAIT_CYCLES                 0x10
#define SPI_TRX_TASK_DURATION_MS            500
#define SPI_TRX_TASK_PRIO                   24
#define SPI_TRX_TASK_STACK_SIZE             0x1000

#if defined(CONFIG_SAMPLE_SUPPORT_SPI_TRX_PERFORMANCE_TEST)
uint8_t g_spi_rx_data[CONFIG_SPI_TRX_TEST_TOTAL_LEN] = { 0 };
uint16_t g_spi_test_index = 0;
#endif

static void app_spi_trx_init_pin(void)
{
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_SPI_TRX_MASTER_DI_PIN, PIN_IE_1);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
    uapi_pin_set_mode(CONFIG_SPI_TRX_MASTER_CLK_PIN, CONFIG_SPI_TRX_MASTER_CLK_PIN_MODE);
    uapi_pin_set_mode(CONFIG_SPI_TRX_MASTER_CS_PIN, CONFIG_SPI_TRX_MASTER_CS_PIN_MODE);
    uapi_pin_set_mode(CONFIG_SPI_TRX_MASTER_DI_PIN, CONFIG_SPI_TRX_MASTER_DI_PIN_MODE);
    uapi_pin_set_mode(CONFIG_SPI_TRX_MASTER_DO_PIN, CONFIG_SPI_TRX_MASTER_DO_PIN_MODE);
}

static void app_spi_trx_master_init_config(void)
{
    spi_attr_t config = { 0 };
    spi_extra_attr_t ext_config = { 0 };

    config.is_slave = false;
    config.slave_num = SPI_TRX_SLAVE_NUM;
    config.bus_clk = SPI_CLK_FREQ;
    config.freq_mhz = SPI_TRX_FREQUENCY;
    config.clk_polarity = SPI_TRX_CLK_POLARITY;
    config.clk_phase = SPI_TRX_CLK_PHASE;
    config.frame_format = SPI_TRX_FRAME_FORMAT;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    config.frame_size = SPI_TRX_FRAME_SIZE;
    config.tmod = HAL_SPI_TRANS_MODE_TXRX;
    config.sste = 0;

    ext_config.qspi_param.wait_cycles = SPI_TRX_WAIT_CYCLES;

    uapi_spi_init(CONFIG_SPI_TRX_MASTER_BUS_ID, &config, &ext_config);
}

static void *spi_trx_master_task(const char *arg)
{
    unused(arg);
    /* SPI pinmux. */
    app_spi_trx_init_pin();

    /* SPI master init config. */
    app_spi_trx_master_init_config();

    /* SPI data config. */
    uint8_t tx_data[CONFIG_SPI_TRX_TRANSFER_LEN] = { 0 };
    for (uint32_t loop = 0; loop < CONFIG_SPI_TRX_TRANSFER_LEN; loop++) {
        tx_data[loop] = (loop & 0xFF);
    }
    uint8_t rx_data[CONFIG_SPI_TRX_TRANSFER_LEN] = { 0 };
    spi_xfer_data_t data = {
        .tx_buff = tx_data,
        .tx_bytes = CONFIG_SPI_TRX_TRANSFER_LEN,
        .rx_buff = rx_data,
        .rx_bytes = CONFIG_SPI_TRX_TRANSFER_LEN,
    };
#if defined(CONFIG_SAMPLE_SUPPORT_SPI_TRX_PERFORMANCE_TEST)
    uint16_t g_spi_test_times = CONFIG_SPI_TRX_TEST_TOTAL_LEN / CONFIG_SPI_TRX_TRANSFER_LEN;
    while (g_spi_test_times--) {
        uapi_tcxo_delay_us(1);
        if (uapi_spi_master_txrx_transfer(CONFIG_SPI_TRX_MASTER_BUS_ID, &data, 0xFFFFFFFF) == ERRCODE_SUCC) {
            for (uint32_t i = 0; i < data.rx_bytes; i++) {
                g_spi_rx_data[g_spi_test_index++] = data.rx_buff[i];
            }
        }
    }
#else
    while (1) {
        osal_msleep(SPI_TRX_TASK_DURATION_MS);
        osal_printk("spi%d master trx writeread start!\r\n", CONFIG_SPI_TRX_MASTER_BUS_ID);
        if (uapi_spi_master_txrx_transfer(CONFIG_SPI_TRX_MASTER_BUS_ID, &data, 0xFFFFFFFF) == ERRCODE_SUCC) {
            for (uint32_t i = 0; i < data.rx_bytes; i++) {
                osal_printk("spi%d master trx writeread data is %x\r\n", CONFIG_SPI_TRX_MASTER_BUS_ID, data.rx_buff[i]);
            }
            osal_printk("spi%d master trx writeread succ!\r\n", CONFIG_SPI_TRX_MASTER_BUS_ID);
        }
    }
#endif
    return NULL;
}

static void spi_trx_master_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)spi_trx_master_task, 0, "SpiTrxMasterTask",
        SPI_TRX_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SPI_TRX_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the spi_trx_master_entry. */
app_run(spi_trx_master_entry);