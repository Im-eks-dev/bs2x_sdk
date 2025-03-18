/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: QDEC Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-20, Create file. \n
 */
#include "qdec.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define QDEC_TASK_PRIO               24
#define QDEC_TASK_STACK_SIZE         0x1000

static qdec_config_t g_app_qdec_config = QDEC_DEFAULT_CONFIG;
static int32_t g_app_qdec_count = 0;

static int32_t app_qdec_report_callback(int argc, char *argv[])
{
    unused(argv);
    g_app_qdec_count += argc;
    osal_printk("current count is: %d.\r\n", g_app_qdec_count);
    return ERRCODE_SUCC;
}

static void *qdec_task(const char *arg)
{
    unused(arg);
    qdec_port_pinmux_init(CONFIG_MOUSE_PIN_QDEC_A, CONFIG_MOUSE_PIN_QDEC_B);

    /* QDEC Init Config. */
    uapi_qdec_init(&g_app_qdec_config);

    if (uapi_qdec_register_callback(app_qdec_report_callback) == ERRCODE_SUCC) {
        osal_printk("qdec register callback succ!\r\n");
    }

    if (uapi_qdec_enable() == ERRCODE_SUCC) {
        osal_printk("qdec enable succ!\r\n");
    }

    return NULL;
}

static void qdec_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)qdec_task, 0, "QdecTask", QDEC_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, QDEC_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the qdec_entry. */
app_run(qdec_entry);