/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: BLE MOUSE Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-20, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "mouse_init.h"
#include "ble_mouse_server/ble_mouse_server.h"

#define BLE_MOUSE_TASK_PRIO                  28
#define BLE_MOUSE_TASK_STACK_SIZE            0x800

static void *ble_mouse_task(const char *arg)
{
    unused(arg);
    ble_mouse_server_init();
    ble_mouse_init();
    return NULL;
}

static void ble_mouse_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)ble_mouse_task, 0, "BLEmouseTask",
                                      BLE_MOUSE_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLE_MOUSE_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the ble_mouse_entry. */
app_run(ble_mouse_entry);