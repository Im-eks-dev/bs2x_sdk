/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Mouse Battery Manager SAMPLE. \n
 * Author: @CompanyNameMagicTag \n
 * History: \n
 * 2024-04-21, Create file. \n
 */

#include "soc_osal.h"
#include "osal_debug.h"
#include "osal_timer.h"
#include "adc.h"
#include "adc_porting.h"
#include "mouse_mode_manage.h"
#include "ble_bas_and_dis_server.h"
#include "mouse_lowpower_manager.h"
#include "mouse_battery_manager.h"

static app_mouse_battery_mgr_t g_app_mouse_battery_mgr;
static void mouse_adc_open_timer_handler(unsigned long arg);
static void mouse_adc_sample_timer_handler(unsigned long arg);
static osal_timer g_mouse_adc_open_timer = { 0, mouse_adc_open_timer_handler, 0, BATTERY_LEVEL_REPORT_TIME };
static osal_timer g_mouse_adc_sample_timer = { 0, mouse_adc_sample_timer_handler, 0, 100 };


static void mouse_adc_open_timer_handler(unsigned long arg)
{
    unused(arg);
    if (mouse_lowpower_get_state() != MOUSE_IN_WORK) {
        osal_printk("mouse_adc_open_timer_handler:failed\n");
    }
    static bool first_sample_flag = true;
    // Timer中断中仅发消息，防止ADC初始化及采样太长阻塞其他流程
    if (first_sample_flag) {
        app_mouse_send_msg(MSG_ID_ADC_MANAGE, MSG_TYPE_ADC_OPEN_WITH_CALI);
        first_sample_flag = false;
    } else {
        app_mouse_send_msg(MSG_ID_ADC_MANAGE, MSG_TYPE_ADC_OPEN_WITHOUT_CALI);
    }
    osal_timer_start(&g_mouse_adc_sample_timer);
}

static void mouse_adc_sample_timer_handler(unsigned long arg)
{
    unused(arg);
    if (mouse_lowpower_get_state() != MOUSE_IN_WORK) {
        osal_printk("mouse_adc_sample_timer_handler:failed\n");
    }
    app_mouse_send_msg(MSG_ID_ADC_MANAGE, MSG_TYPE_ADC_SAMPLE_AND_CLOSE);
    osal_timer_start(&g_mouse_adc_open_timer);
}

void app_mouse_battery_state_change(app_mouse_battery_state_t new_state)
{
    g_app_mouse_battery_mgr.battery_state = new_state;
}

void app_mouse_vbat_adc_init(void)
{
    int ret = osal_timer_init(&g_mouse_adc_open_timer);
    if (ret != OSAL_SUCCESS) {
        osal_printk("g_mouse_adc_open_timer create failed!\n");
    }
    ret = osal_timer_init(&g_mouse_adc_sample_timer);
    if (ret != OSAL_SUCCESS) {
        osal_printk("g_mouse_adc_sample_timer create failed!\n");
    }
    osal_timer_start(&g_mouse_adc_open_timer);
}

void app_mouse_battery_init(void)
{
    g_app_mouse_battery_mgr.battery_level = 0;
    g_app_mouse_battery_mgr.battery_state = MOUSE_BATTERY_IDLE;
    app_mouse_vbat_adc_init();
}

void app_mouse_vbat_adc_suspend(void)
{
    osal_timer_stop(&g_mouse_adc_sample_timer);
    osal_timer_stop(&g_mouse_adc_open_timer);
}

void app_mouse_vbat_adc_resume(void)
{
    osal_timer_start(&g_mouse_adc_open_timer);
}
