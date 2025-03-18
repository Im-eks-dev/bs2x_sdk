/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: Mouse Low-power Manager SAMPLE \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-10-27, Create file. \n
 */

#include "soc_osal.h"
#include "osal_debug.h"
#include "osal_timer.h"
#include "pm_veto.h"
#include "pinctrl.h"
#include "gpio.h"
#include "ulp_gpio.h"
#include "qdec.h"
#include "keyscan.h"
#include "mouse_wheel.h"
#include "pm_sys.h"
#include "mouse_sensor_spi.h"
#include "mouse_mode_manage.h"
#include "mouse_data_transfer_manage.h"
#include "mouse_battery_manager.h"
#include "usb_init_app.h"
#include "mouse_lowpower_manager.h"


#define DURATION_S_OF_WORK2STANDBY    60
#define DURATION_S_OF_STANDBY2SLEEP   60

static void app_mouse_standby_timer_handler(unsigned long arg);
static osal_timer g_app_mouse_standby_timer = { 0, app_mouse_standby_timer_handler, 0, 1000 };
static void app_mouse_sleep_timer_handler(unsigned long arg);
static osal_timer g_app_mouse_sleep_timer = { 0, app_mouse_sleep_timer_handler, 0, 1000 * DURATION_S_OF_STANDBY2SLEEP };

static app_mouse_lowpower_state_t g_app_mouse_lowpower_state = MOUSE_IN_WORK;
static uint32_t g_app_mouse_idle_time = 0;

void mouse_lowpower_set_state(app_mouse_lowpower_state_t state)
{
    g_app_mouse_lowpower_state = state;
}

__attribute__((section(".PMRAMCODE"))) app_mouse_lowpower_state_t mouse_lowpower_get_state(void)
{
    return g_app_mouse_lowpower_state;
}

void app_mouse_lowpower_init(void)
{
    int ret;
    g_app_mouse_lowpower_state = MOUSE_IN_WORK;
    g_app_mouse_idle_time = 0; // 单位：秒

    ret = osal_timer_init(&g_app_mouse_standby_timer);
    if (ret != OSAL_SUCCESS) {
        osal_printk("g_app_mouse_standby_timer create failed!\n");
    } else {
        osal_printk("g_app_mouse_standby_timer create success\n");
    }

    ret = osal_timer_init(&g_app_mouse_sleep_timer);
    if (ret != OSAL_SUCCESS) {
        osal_printk("g_app_mouse_sleep_timer create failed!\n");
    }
}

__attribute__((section(".PMRAMCODE"))) void app_mouse_idle_time_reset(void)
{
    g_app_mouse_idle_time = 0;
}

void app_mouse_standby_timer_start(void)
{
    osal_timer_start(&g_app_mouse_standby_timer);
}

void app_mouse_standby_timer_stop(void)
{
    osal_timer_stop(&g_app_mouse_standby_timer);
}

void app_mouse_sleep_timer_start(void)
{
    osal_timer_start(&g_app_mouse_sleep_timer);
}

void app_mouse_sleep_timer_stop(void)
{
    osal_timer_stop(&g_app_mouse_sleep_timer);
}

void mouse_disable_ulpgpio_wkup(void)
{
    ulp_gpio_deinit();
    uapi_gpio_init();
}

static void ulp_gpio_wkup_handler(uint8_t ulp_gpio)
{
    mouse_disable_ulpgpio_wkup();
    if (g_app_mouse_lowpower_state == MOUSE_IN_STANDBY || g_app_mouse_lowpower_state == MOUSE_WORK_TO_STANDBY) {
        g_app_mouse_lowpower_state = MOUSE_STANDBY_TO_WORK;
        app_mouse_send_msg(MSG_ID_LOW_POWER, MSG_TYPE_GOTO_WORK);
    } else if (g_app_mouse_lowpower_state == MOUSE_IN_SLEEP || g_app_mouse_lowpower_state == MOUSE_STANDBY_TO_SLEEP) {
        g_app_mouse_lowpower_state = MOUSE_SLEEP_TO_WORK;
        app_mouse_send_msg(MSG_ID_LOW_POWER, MSG_TYPE_GOTO_WORK);
    }
    osal_printk("ulp_gpio%d wakeup\n", ulp_gpio);
}

static ulp_gpio_int_wkup_cfg_t g_wk_cfg[] = {
    { 0, CONFIG_MOUSE_PIN_QDEC_A, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },   // qdec唤醒
    { 1, CONFIG_MOUSE_PIN_QDEC_B, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },
    { 2, CONFIG_MOUSE_PIN_MOTION, true, ULP_GPIO_INTERRUPT_DEDGE, ulp_gpio_wkup_handler },  // Sensor唤醒
#if defined(CONFIG_MOUSE_8K_TURNKEY)
    { 3, CONFIG_MOUSE_PIN_LEFT, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },
    { 4, CONFIG_MOUSE_PIN_RIGHT, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },
    { 5, CONFIG_MOUSE_PIN_MID, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },
    { 6, CONFIG_MOUSE_PIN_BACKWARD, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },
    { 7, CONFIG_MOUSE_PIN_FORWARD, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },
#elif defined(CONFIG_MOUSE_2K_TURNKEY)
    { 7, 35, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },    // keyscan唤醒
#endif
};

static void mouse_enable_ulpgpio_wkup(void)
{
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_QDEC_A, 0);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_QDEC_B, 0);
    if (uapi_gpio_get_val(CONFIG_MOUSE_PIN_QDEC_A) == GPIO_LEVEL_LOW) {
        g_wk_cfg[0].trigger = ULP_GPIO_INTERRUPT_HIGH;
    } else {
        g_wk_cfg[0].trigger = ULP_GPIO_INTERRUPT_LOW;
    }

    if (uapi_gpio_get_val(CONFIG_MOUSE_PIN_QDEC_B) == GPIO_LEVEL_LOW) {
        g_wk_cfg[0x1].trigger = ULP_GPIO_INTERRUPT_HIGH;
    } else {
        g_wk_cfg[0x1].trigger = ULP_GPIO_INTERRUPT_LOW;
    }
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_QDEC_A, PIN_PULL_NONE);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_QDEC_B, PIN_PULL_NONE);
    uapi_gpio_deinit();
    ulp_gpio_init();
    ulp_gpio_int_wkup_config(g_wk_cfg, sizeof(g_wk_cfg) / sizeof(ulp_gpio_int_wkup_cfg_t));
}

/* 检查系统是否空闲，低功耗状态机将要进睡眠时判断 */
bool app_mouse_idle_state_check(void)
{
    if (app_mouse_no_key_press() == true && uapi_gpio_get_val(CONFIG_MOUSE_PIN_MOTION)) {
        return true;
    }
    return false;
}

void app_mouse_perp_config_before_sleep(void)
{
    app_mouse_vbat_adc_suspend();
    mouse_wheel_suspend();
    mouse_sensor_spi_suspend();
#if defined(CONFIG_MOUSE_8K_TURNKEY)
    mouse_button_suspend();
#elif defined(CONFIG_MOUSE_2K_TURNKEY)
    uapi_keyscan_suspend(0);
#endif
    mouse_enable_ulpgpio_wkup();
    if (get_usb_inited()) {
        close_usb_mode();
    }
}

void app_mouse_perp_config_after_wkup(void)
{
    app_mouse_sleep_timer_stop();
    app_mouse_usb_init();
#if defined(CONFIG_MOUSE_8K_TURNKEY)
    mouse_button_resume();
#elif defined(CONFIG_MOUSE_2K_TURNKEY)
    uapi_keyscan_resume(0);
#endif
    mouse_sensor_spi_resume();
    mouse_wheel_resume();
    app_mouse_vbat_adc_resume();
}

void app_mouse_send_standby_msg(void)
{
    g_app_mouse_lowpower_state = MOUSE_WORK_TO_STANDBY;
    app_mouse_send_msg(MSG_ID_LOW_POWER, MSG_TYPE_GOTO_STANDBY);  // 发送消息通知睡眠
}

static void app_mouse_sleep_process(void)
{
    if (g_app_mouse_lowpower_state == MOUSE_IN_WORK) {
        g_app_mouse_idle_time++;
        if (g_app_mouse_idle_time < DURATION_S_OF_WORK2STANDBY) {
            return;
        }
        g_app_mouse_idle_time = 0;
        if (!app_mouse_idle_state_check()) {
            return;
        }
        app_mouse_send_standby_msg();
        osal_timer_stop(&g_app_mouse_standby_timer);
    } else if (g_app_mouse_lowpower_state == MOUSE_WORK_TO_STANDBY) {
        osal_timer_stop(&g_app_mouse_standby_timer);
    }
}

void app_mouse_sleep_manager(void)
{
    app_mouse_sleep_process();
}

void app_mouse_standby_timer_handler(unsigned long arg)
{
    UNUSED(arg);
    osal_timer_start(&g_app_mouse_standby_timer);
    if (app_mouse_current_id() != SLIDE_SW_MODE_USB) {
        app_mouse_sleep_manager();
    } else {
        return;
    }
}

void app_mouse_sleep_timer_handler(unsigned long arg)
{
    UNUSED(arg);
    if (g_app_mouse_lowpower_state != MOUSE_IN_STANDBY) {
        return;
    }
    g_app_mouse_lowpower_state = MOUSE_STANDBY_TO_SLEEP;
    app_mouse_send_msg(MSG_ID_LOW_POWER, MSG_TYPE_GOTO_SLEEP);
}
