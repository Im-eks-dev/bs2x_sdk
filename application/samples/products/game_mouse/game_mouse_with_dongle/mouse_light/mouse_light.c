/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Mouse Manager  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */
#include "gpio.h"
#include "hal_gpio.h"
#include "pinctrl.h"
#include "pwm.h"
#include "tcxo.h"

#define PWM_CHANNEL_0                0
#define PWM_CHANNEL_1                1
#define PWM_CHANNEL_2                2
#define PWM_CHANNEL_3                3
#define PWM_CHANNEL_4                4
#define PWM_CHANNEL_5                5
#define PWM_GROUP_ID               0
#define TEST_TCXO_DELAY_1000MS     1000

void app_mouse_all_led(uint8_t high_time)
{
    unused(high_time);
}
void app_mouse_red_led(uint8_t high_time)
{
    unused(high_time);
}

void app_mouse_green_led(uint8_t high_time)
{
    unused(high_time);
}
void app_mouse_blue_led(uint8_t high_time)
{
    pwm_config_t cfg_no_repeat0 = {
        255,
        high_time,
        0,
        0xFF,
        true,
    };

    uint8_t channel_set[] = {2};
    uint32_t channel_set_len = sizeof(channel_set) / sizeof(channel_set[0]);
    uapi_pin_set_mode(S_MGPIO2, PIN_MODE_42);
    uapi_pwm_init();
    uapi_pwm_open(PWM_CHANNEL_2, &cfg_no_repeat0);

    uapi_pwm_unregister_interrupt(PWM_GROUP_ID);
    uapi_pwm_register_interrupt(PWM_GROUP_ID, NULL);
    uapi_pwm_set_group(PWM_GROUP_ID, channel_set, channel_set_len);
    uapi_pwm_start_group(PWM_GROUP_ID);
}

void app_mouse_blue_dpi_led(uint8_t high_time)
{
    pwm_config_t cfg_no_repeat0 = {
        255,
        high_time,
        0,
        0xFF,
        true,
    };

    uint8_t channel_set[] = {3};
    uint32_t channel_set_len = sizeof(channel_set) / sizeof(channel_set[0]);
    uapi_pin_set_mode(S_MGPIO3, PIN_MODE_43);
    uapi_pwm_init();
    uapi_pwm_open(PWM_CHANNEL_3, &cfg_no_repeat0);

    uapi_pwm_unregister_interrupt(PWM_GROUP_ID);
    uapi_pwm_register_interrupt(PWM_GROUP_ID, NULL);
    uapi_pwm_set_group(PWM_GROUP_ID, channel_set, channel_set_len);
    uapi_pwm_start_group(PWM_GROUP_ID);
}
void app_mouse_green_dpi_led(uint8_t high_time)
{
    pwm_config_t cfg_no_repeat0 = {
        255,
        high_time,
        0,
        0xFF,
        true,
    };

    uint8_t channel_set[] = {4};
    uint32_t channel_set_len = sizeof(channel_set) / sizeof(channel_set[0]);
    uapi_pin_set_mode(S_MGPIO4, PIN_MODE_44);
    uapi_pwm_init();
    uapi_pwm_open(PWM_CHANNEL_4, &cfg_no_repeat0);

    uapi_pwm_unregister_interrupt(PWM_GROUP_ID);
    uapi_pwm_register_interrupt(PWM_GROUP_ID, NULL);
    uapi_pwm_set_group(PWM_GROUP_ID, channel_set, channel_set_len);
    uapi_pwm_start_group(PWM_GROUP_ID);
}

void app_mouse_breathing_light(void)
{
    for (int brightness = 0; brightness <= 255; brightness++) { // 最高亮度255
        app_mouse_all_led(brightness);
        uapi_tcxo_delay_ms(10); // 10ms
    }

    for (int brightness = 255; brightness >= 0; brightness--) { // 最高亮度255
        app_mouse_all_led(brightness);
        uapi_tcxo_delay_ms(10); // 10ms
    }
}


void app_mouse_light_close(void)
{
    uapi_pwm_close(PWM_CHANNEL_0);
    uapi_pwm_close(PWM_CHANNEL_1);
    uapi_pwm_deinit();
}