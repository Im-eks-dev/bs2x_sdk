/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Button Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-03, Create file. \n
 */
#include "boards.h"
#include "osal_debug.h"
#include "app_init.h"
#include "tcxo.h"
#include "button.h"

static void gpio_callback_func(uint8_t pin_group, button_press_state_t state)
{
    unused(pin_group);

    if (state == 0) {
        osal_printk("Button PRESSED\r\n");
    } else {
        osal_printk("Button RELEASED\r\n");
    }
}

static void button_entry(void)
{
    osal_printk("Button start.\r\n");
    uapi_button_init();
    uapi_button_send_msg_register(gpio_callback_func);
    uapi_button_gpio_register(CONFIG_SAMPLE_BUTTON_PIN);
}

/* Run the button_entry. */
app_run(button_entry);