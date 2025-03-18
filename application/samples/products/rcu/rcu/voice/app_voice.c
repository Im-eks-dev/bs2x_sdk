/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: APP Voice Source File. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-28, Create file. \n
 */
 
#include "pinctrl.h"
#include "gpio.h"
#include "pm_sys.h"
#include "sle_rcu_server.h"
#include "sle_vdt_pdm.h"
#include "sle_connection_manager.h"
#include "pdm.h"
#include "dma.h"
#include "dma_porting.h"
#include "adc.h"
#include "adc_porting.h"
#include "hal_dma.h"
#include "keyscan.h"
#include "app_voice.h"
 
#define ADC_GADC_CHANNEL7                  7
#define ADC_GADC_CHANNEL6                  6
#define SLE_VDT_SERVER_LOG                 "[sle vdt server]"
 
static uint32_t g_rcu_dma_channel = 0;
void dma_port_release_handshaking_source(dma_channel_t ch);

static void sle_vdt_adc_set_io(pin_t pin)
{
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
/* ADC管脚无需配置IE使能且管脚默认IE为0，为防止用户修改IE，特在此将IE配置为0 */
    uapi_pin_set_ie(pin, PIN_IE_0);
#endif
    uapi_pin_set_mode(pin, 0);
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_INPUT);
    uapi_pin_set_pull(pin, PIN_PULL_NONE);
}

static void sle_vdt_adc_init(void)
{
    uapi_pin_init();
    uapi_gpio_init();

    sle_vdt_adc_set_io(CONFIG_ADC_USE_PIN1);
    sle_vdt_adc_set_io(CONFIG_ADC_USE_PIN2);
    uapi_adc_init(ADC_CLOCK_NONE);
    uapi_adc_power_en(AFE_AMIC_MODE, true);
    uapi_adc_open_differential_channel(ADC_GADC_CHANNEL7, ADC_GADC_CHANNEL6);
    adc_calibration(AFE_AMIC_MODE, true, true, true);

    return;
}

static void sle_usb_vdt_dma_transfer_done_callback(uint8_t intr, uint8_t channel, uintptr_t arg)
{
    unused(channel);
    unused(arg);
    uint8_t node = 0;
    g_rcu_dma_channel = channel;
    switch (intr) {
        case HAL_DMA_INTERRUPT_TFR:
            node = (g_write_buffer_node + 1) % RING_BUFFER_NUMBER;
            g_write_buffer_node = node;
            break;
        case HAL_DMA_INTERRUPT_ERR:
            osal_printk("%s DMA transfer error.\r\n", SLE_VDT_SERVER_LOG);
            break;
        default:
            break;
    }
}

void rcu_amic_deinit(uint16_t g_conn_id)
{
    sle_low_latency_set_em_data(g_conn_id, 0);
    uapi_dma_end_transfer(g_rcu_dma_channel);
    uapi_dma_close();
    uapi_dma_deinit();
    dma_port_release_handshaking_source(g_rcu_dma_channel);
    uapi_adc_power_en(AFE_AMIC_MODE, false);
    uapi_adc_deinit();
    uapi_pdm_stop();
    uapi_pdm_deinit();
}

void rcu_amic_init(uint16_t g_conn_id)
{
    sle_low_latency_set_em_data(g_conn_id, 1);

    sle_vdt_adc_init();
    if (sle_vdt_pdm_init() != 0) {
        osal_printk("%s Init the PDM fail.\r\n", SLE_VDT_SERVER_LOG);
    }
    if (uapi_pdm_start() != ERRCODE_SUCC) {
        osal_printk("%s Start the PDM fail.\r\n", SLE_VDT_SERVER_LOG);
    }

    dma_channel_t dma_channel = uapi_dma_get_lli_channel(0, HAL_DMA_HANDSHAKING_MAX_NUM);
    for (uint8_t i = 0; i < RING_BUFFER_NUMBER; i++) {
        if (rcu_add_dma_lli_node(i, dma_channel, sle_usb_vdt_dma_transfer_done_callback) != 0) {
            osal_printk("rcu_add_dma_lli_node fail!\r\n");
            return;
        }
    }

    if (uapi_dma_enable_lli(dma_channel, sle_usb_vdt_dma_transfer_done_callback, (uintptr_t)NULL) == ERRCODE_SUCC) {
        osal_printk("dma enable lli memory transfer succ!\r\n");
    }
}