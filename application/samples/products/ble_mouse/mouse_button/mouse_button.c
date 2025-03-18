/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse Button source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include "osal_debug.h"
#include "gpio.h"
#include "pinctrl.h"
#include "tcxo.h"
#include "bts_le_gap.h"
#if defined(CONFIG_PM_SYS_SUPPORT)
#include "pm_sys.h"
#endif
#include "mouse_button.h"

#define DELAY_US200     200

static mouse_key_t g_mouse_key = { 0 };

void mouse_left_button_func(pin_t pin)
{
#if defined(CONFIG_PM_SYS_SUPPORT)
    uapi_pm_work_state_reset();
#endif
    osal_printk("Left button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    g_mouse_key.b.left_key = !uapi_gpio_get_val(pin);
    uapi_gpio_clear_interrupt(pin);
}

void mouse_right_button_func(pin_t pin)
{
#if defined(CONFIG_PM_SYS_SUPPORT)
    uapi_pm_work_state_reset();
#endif
    osal_printk("Right button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    g_mouse_key.b.right_key = !uapi_gpio_get_val(pin);
    uapi_gpio_clear_interrupt(pin);
}

void mouse_mid_button_func(pin_t pin)
{
#if defined(CONFIG_PM_SYS_SUPPORT)
    uapi_pm_work_state_reset();
#endif
    osal_printk("MID button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    g_mouse_key.b.mid_key = !uapi_gpio_get_val(pin);
    uapi_gpio_clear_interrupt(pin);
}

uint16_t mouse_get_button(void)
{
    return g_mouse_key.d8;
}

void mouse_button_init(void)
{
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_MOUSE_PIN_LEFT, PIN_IE_1);
    uapi_pin_set_ie(CONFIG_MOUSE_PIN_RIGHT, PIN_IE_1);
    uapi_pin_set_ie(CONFIG_MOUSE_PIN_MID, PIN_IE_1);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_LEFT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_RIGHT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_MID, (pin_mode_t)HAL_PIO_FUNC_GPIO);

    uapi_pin_set_pull(CONFIG_MOUSE_PIN_LEFT, PIN_PULL_UP);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_RIGHT, PIN_PULL_UP);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_MID, PIN_PULL_UP);

    gpio_select_core(CONFIG_MOUSE_PIN_LEFT, CORES_APPS_CORE);
    gpio_select_core(CONFIG_MOUSE_PIN_RIGHT, CORES_APPS_CORE);
    gpio_select_core(CONFIG_MOUSE_PIN_MID, CORES_APPS_CORE);

    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_LEFT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_RIGHT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_MID, GPIO_DIRECTION_INPUT);

    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_LEFT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_left_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_RIGHT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_right_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_MID, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_mid_button_func);
}