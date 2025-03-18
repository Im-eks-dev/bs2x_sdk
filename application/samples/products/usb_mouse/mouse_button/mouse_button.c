/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test mouse GPIO source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-10, Create file. \n
 */
#include "gpio.h"
#include "pinctrl.h"
#include "tcxo.h"
#include "osal_debug.h"
#include "mouse_button.h"

#define DELAY_US200     200

static usb_hid_mouse_key_t *g_mouse_key = NULL;

static void mouse_left_button_func(pin_t pin)
{
    osal_printk("Left button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    if (g_mouse_key != NULL) {
        g_mouse_key->b.left_key = !uapi_gpio_get_val(pin);
    }
}

static void mouse_right_button_func(pin_t pin)
{
    osal_printk("Right button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    if (g_mouse_key != NULL) {
        g_mouse_key->b.right_key = !uapi_gpio_get_val(pin);
    }
}

static void mouse_mid_button_func(pin_t pin)
{
    osal_printk("MID button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    if (g_mouse_key != NULL) {
        g_mouse_key->b.mid_key = !uapi_gpio_get_val(pin);
    }
}

void mouse_button_init(usb_hid_mouse_key_t *mouse_key)
{
    g_mouse_key = mouse_key;

    uapi_gpio_init();
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_MOUSE_PIN_LEFT, PIN_IE_1);
    uapi_pin_set_ie(CONFIG_MOUSE_PIN_RIGHT, PIN_IE_1);
    uapi_pin_set_ie(CONFIG_MOUSE_PIN_MID, PIN_IE_1);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_LEFT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_RIGHT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_MID, (pin_mode_t)HAL_PIO_FUNC_GPIO);

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