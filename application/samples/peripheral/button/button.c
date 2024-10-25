/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Button Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-03, Create file. \n
 */
#include "boards.h"
#include "pinctrl.h"
#include "gpio.h"
#include "test_suite_log.h"
#include "cmsis_os2.h"
#include "app_init.h"

#include "osal_debug.h"

#if defined(CONFIG_BUTTON_PIN)
#   define BUTTON_0     CONFIG_BUTTON_PIN
#else
#   define BUTTON_0     6
#endif

#define BUTTON_TASK_STACK_SIZE    0x1000
#define BUTTON_TASK_PRIO          (osPriority_t)(17)

static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    UNUSED(pin);
    UNUSED(param);
    osal_printk("Button pressed.\r\n");
}

static void *button_task(const char *arg)
{
    unused(arg);

    uapi_pin_set_mode(BUTTON_0, HAL_PIO_FUNC_GPIO);

    gpio_select_core(BUTTON_0, CORES_APPS_CORE);

    uapi_gpio_set_dir(BUTTON_0, GPIO_DIRECTION_INPUT);
    uapi_gpio_register_isr_func(BUTTON_0, GPIO_INTERRUPT_DEDGE, gpio_callback_func);
    
    return NULL;
}

static void button_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "BlinkyTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = BUTTON_TASK_STACK_SIZE;
    attr.priority = BUTTON_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)button_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the button_entry. */
app_run(button_entry);