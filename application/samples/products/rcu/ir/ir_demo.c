/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: IR Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-10, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "keyscan.h"
#include "pm_clock.h"
#if defined(CONFIG_SAMPLE_SUPPORT_IR_STUDY)
#include "ir_study.h"
#endif /* CONFIG_SAMPLE_SUPPORT_IR_STUDY */
#include "ir_nec.h"

#define IR_TASK_STACK_SIZE          0x800
#define IR_TASK_PRIO                24
#define IR_TASK_DELAY_MS            10
#define IR_DELAY_MS                 2000
#define USER_CODE_H                 0xED
#define DATA_CODE                   0x66

#define IR_NEC_KEY_UP               0xCA
#define IR_NEC_KEY_DOWN             0xD2
#define IR_NEC_KEY_RIGHT            0xC1
#define IR_NEC_KEY_LEFT             0x99
#define IR_NEC_KEY_SELECT           0xCE
#define IR_NEC_KEY_BACK             0x90
#define IR_NEC_KEY_MENU             0x9D
#define IR_NEC_KEY_POWER            0x9C
#define IR_NEC_KEY_HOME             0xCB
#define IR_NEC_KEY_VOLUMEUP         0x80
#define IR_NEC_KEY_VOLUMEDOWN       0x81
#define IR_NEC_KEY_MUTE             0xDD
#define IR_SWITCH_SEND_AND_STUDY    0x6

#if defined(CONFIG_KEYSCAN_USE_SIX_KEYS_TYPE)
static const uint8_t g_key_map[CONFIG_KEYSCAN_ENABLE_ROW][CONFIG_KEYSCAN_ENABLE_COL]  = {
    { IR_NEC_KEY_UP, IR_NEC_KEY_DOWN },
    { IR_NEC_KEY_RIGHT, IR_NEC_KEY_LEFT },
    { IR_NEC_KEY_SELECT, IR_SWITCH_SEND_AND_STUDY },
};
#endif /* CONFIG_KEYSCAN_USE_SIX_KEYS_TYPE */

#if defined(CONFIG_SAMPLE_SUPPORT_IR_STUDY)
static bool g_rx_study_flag = false;
#endif /* CONFIG_SAMPLE_SUPPORT_IR_STUDY */

#if defined(CONFIG_KEYSCAN_USE_SIX_KEYS_TYPE)
static int ir_keyscan_callback(int key_nums, uint8_t key_values[])
{
    unused(key_nums);
    switch (key_values[0]) {
        case IR_NEC_KEY_LEFT:
#if defined(CONFIG_SAMPLE_SUPPORT_IR_STUDY)
            if (g_rx_study_flag) {
                uapi_ir_study_start(key_values[0]);
            } else {
                ir_nec_send(USER_CODE_H, key_values[0]);
            }
#else
            ir_nec_send(USER_CODE_H, key_values[0]);
#endif /* CONFIG_SAMPLE_SUPPORT_IR_STUDY */
            break;
        case IR_SWITCH_SEND_AND_STUDY:
#if defined(CONFIG_SAMPLE_SUPPORT_IR_STUDY)
            g_rx_study_flag = !g_rx_study_flag;
#endif /* CONFIG_SAMPLE_SUPPORT_IR_STUDY */
            break;
        default:
            break;
    }
    return 0;
}
#endif /* CONFIG_KEYSCAN_USE_SIX_KEYS_TYPE */

static void *ir_task(const char *arg)
{
    unused(arg);

    osal_printk("start ir sample\r\n");

    /* keyscan init */
#if defined(CONFIG_KEYSCAN_USE_SIX_KEYS_TYPE)
    uapi_set_keyscan_value_map((uint8_t **)g_key_map, CONFIG_KEYSCAN_ENABLE_ROW, CONFIG_KEYSCAN_ENABLE_COL);
    uapi_keyscan_init(EVERY_ROW_PULSE_40_US, HAL_KEYSCAN_MODE_0, KEYSCAN_INT_VALUE_RDY);
    uapi_keyscan_register_callback(ir_keyscan_callback);
    uapi_keyscan_enable();
#else
    while (1) {
        osal_msleep(IR_DELAY_MS);
        ir_transmit_nec(USER_CODE_H, DATA_CODE);
    }
#endif /* CONFIG_KEYSCAN_USE_SIX_KEYS_TYPE */
    return NULL;
}

static void ir_entry(void)
{
    if (uapi_clock_control(CLOCK_CLKEN_ID_MCU_CORE, CLOCK_FREQ_LEVEL_HIGH) == ERRCODE_SUCC) {
        osal_printk("Config succ.\r\n");
    } else {
        osal_printk("Config fail.\r\n");
    }
    osal_kthread_lock();
    osal_task *g_ir_task_handle = osal_kthread_create((osal_kthread_handler)ir_task, 0, "irTask", IR_TASK_STACK_SIZE);
    if (g_ir_task_handle != NULL) {
        osal_kthread_set_priority(g_ir_task_handle, IR_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the eflash_entry. */
app_run(ir_entry);
