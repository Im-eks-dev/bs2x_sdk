/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: ADC Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-06, Create file. \n
 */
#include "pinctrl.h"
#include "gpio.h"
#include "adc.h"
#include "adc_porting.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#if defined (CONFIG_TIMING_SAMPLE)
#include "timer.h"
#include "chip_core_irq.h"
#endif

#define ADC_AUTO_SAMPLE_TEST_TIMES        100
#if defined (CONFIG_TIMING_SAMPLE)
#define SAMPLE_INTERVAL_MS                5000
#endif

#if defined(CONFIG_ADC_USING_V152)
#define PGA_GAIN_EIGHT                    3
#define IA_GAIN_SIXTEEN                   4
#define HAFE_SAMPLE_VALUE_SIGN_BIT        21
#define HADC_DEFAULT_CALI               \
{                                       \
    .hadc_os_code = 0x68,               \
    .hadc_wt_b22  = 0x75fdd,            \
    .hadc_wt_b21  = 0x3ef67,            \
    .hadc_wt_b20  = 0x1f7ae,            \
    .hadc_wt_b19  = 0xfc3d,             \
    .hadc_wt_b18  = 0x7e22,             \
    .hadc_wt_b17  = 0x7dc9,             \
    .hadc_wt_b16  = 0x3f01,             \
    .hadc_wt_b15  = 0x1f80,             \
    .hadc_wt_b14  = 0xfc5,              \
    .hadc_wt_b13  = 0x800,              \
    .hadc_wt_b12  = 0x800,              \
    .hadc_wt_b11  = 0x400,              \
    .hadc_wt_b10  = 0x200,              \
    .hadc_wt_b9   = 0x100,              \
    .dac_code1    = 0x200,              \
    .dac_code2    = 0x1d5,              \
}
#endif

#if defined (CONFIG_TIMING_SAMPLE)
osal_timer g_vbat_timer = { 0 };

static void timing_sample_cb(unsigned long data)
{
    UNUSED(data);
    int adc_value = 0;
#if defined(CONFIG_ADC_USING_V152)
    uint8_t gadc_channel1 = GADC_CHANNELP_1;
    uint8_t gadc_channel2 = GADC_CHANNELN_1;
#endif
#if defined(CONFIG_ADC_USING_V153)
    uint8_t gadc_channel1 = GADC_CHANNEL_0;
    uint8_t gadc_channel2 = GADC_CHANNEL_5;
#endif
    adc_port_gadc_entirely_open(gadc_channel1, false);  // False means use recorded calibration code.
    adc_value =  adc_port_gadc_entirely_sample(gadc_channel1);
    osal_printk("gadc_channel1: %dmv\n", adc_value);
    adc_value =  adc_port_gadc_entirely_sample(gadc_channel2);
    osal_printk("gadc_channel2: %dmv\n", adc_value);
    adc_port_gadc_entirely_close(gadc_channel2);
    osal_timer_start(&g_vbat_timer);
}
#endif

static void adc_entry(void)
{
    osal_printk("start adc sample test");
    uapi_adc_init(ADC_CLOCK_NONE);
    uint8_t gadc_channel = 0;
#if defined(CONFIG_ADC_USING_V152)
/* GADC */
    gadc_channel = GADC_CHANNELP_1;
#endif

#if defined(CONFIG_ADC_USING_V153)
/* GADC */
    gadc_channel = GADC_CHANNEL_0;
#endif
    adc_port_gadc_entirely_open(gadc_channel, true); // True means calibration currently.

#if defined(CONFIG_HADC_SAMPLE)
/* HADC */
    int hadc_value = 0;
#if defined (CONFIG_HADC_SELF_CALIBRATION)
    uapi_adc_power_en(AFE_HADC_MODE, true);
    adc_calibration(AFE_HADC_MODE, true, true, true);
    hadc_gain_set(PGA_GAIN_EIGHT, IA_GAIN_SIXTEEN);
#else
    afe_config_t afe_config = HADC_DEFAULT_CALI;
    adc_set_cali_code(AFE_HADC_MODE, &afe_config);
    hadc_gain_set(PGA_GAIN_EIGHT, IA_GAIN_SIXTEEN);
    uapi_adc_power_en(AFE_HADC_MODE, true);
#endif
#endif

    int gadc_value = 0;
    for (int i = 0; i < ADC_AUTO_SAMPLE_TEST_TIMES; i++) {
        gadc_value =  adc_port_gadc_entirely_sample(gadc_channel);
        osal_printk("gadc: %dmv\n", gadc_value);

#if defined(CONFIG_HADC_SAMPLE)
        hadc_value =  uapi_adc_auto_sample(HADC_CHANNEL_0);
        osal_printk("hadc: %dmv\n", (hadc_value * ADC_TICK2VOL_REF_VOLTAGE_MV) >> \
                    (HAFE_SAMPLE_VALUE_SIGN_BIT + PGA_GAIN_EIGHT + IA_GAIN_SIXTEEN));
#endif
    }
    adc_port_gadc_entirely_close(gadc_channel);
#if defined (CONFIG_TIMING_SAMPLE)
    g_vbat_timer.handler = timing_sample_cb;
    g_vbat_timer.data = 0;
    g_vbat_timer.interval = SAMPLE_INTERVAL_MS;
    osal_timer_init(&g_vbat_timer);
    osal_timer_start(&g_vbat_timer);
#endif
}

/* Run the adc_entry. */
app_run(adc_entry);