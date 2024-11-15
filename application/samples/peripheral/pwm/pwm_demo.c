/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: PWM Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-06-27, Create file. \n
 */
#include "pinctrl.h"
#include "pwm.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "tcxo.h"

#define PWM_CHANNEL                0
#define PWM_GROUP_ID               0
#define TEST_MAX_TIMES             5
#define DALAY_MS                   100
#define TEST_TCXO_DELAY_1000MS     1000
#define PWM_TASK_STACK_SIZE        0x1000
#define PWM_TASK_PRIO              (osPriority_t)(17)

static errcode_t pwm_sample_callback(uint8_t channel)
{
    osal_printk("PWM %d, cycle done. \r\n", channel);
    return ERRCODE_SUCC;
}

static void *pwm_task(const char *arg)
{
    UNUSED(arg);
    pwm_config_t cfg_no_repeat = {
        .low_time       = 100,
        .high_time      = 200,
        .offset_time    = 0,
        .cycles         = 0,
        .repeat         = true,
    };

    uapi_pin_set_mode(CONFIG_PWM_PIN, HAL_PIO_PWM0);
    uapi_pwm_init();
    uapi_pwm_open(PWM_CHANNEL, &cfg_no_repeat);

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
    uapi_pwm_unregister_interrupt(PWM_GROUP_ID);
    uapi_pwm_register_interrupt(PWM_GROUP_ID, pwm_sample_callback);
#ifdef CONFIG_PWM_USING_V151
    uint8_t channel_set[1] = {PWM_CHANNEL};
    uapi_pwm_set_group(PWM_GROUP_ID, channel_set, sizeof(channel_set));
    uapi_pwm_start(PWM_GROUP_ID);
#else
    uapi_pwm_open(PWM_CHANNEL, &cfg_no_repeat);
    uapi_pwm_start(PWM_CHANNEL);
#endif

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
#ifdef CONFIG_PWM_USING_V151
    uapi_pwm_close(PWM_CHANNEL);
#else
    uapi_pwm_close(PWM_GROUP_ID);
#endif

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
    uapi_pwm_deinit();
    return NULL;
}

static void pwm_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "PWMTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = PWM_TASK_STACK_SIZE;
    attr.priority = PWM_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)pwm_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the pwm_entry. */
app_run(pwm_entry);