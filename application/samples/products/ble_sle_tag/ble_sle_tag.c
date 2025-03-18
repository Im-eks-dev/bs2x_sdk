/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: BLE SLE TAG Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-10, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "sle_uuid_server.h"
#include "ble_uuid_server.h"

#define BLE_SLE_TAG_TASK_DURATION_MS    15

#define BLE_SLE_TAG_TASK_PRIO           24
#define BLE_SLE_TAG_TASK_STACK_SIZE     0x800

static void *ble_sle_tag_task(const char *arg)
{
    unused(arg);

    ble_uuid_server_init();
    sle_uuid_server_init();

    return NULL;
}

static void ble_sle_tag_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)ble_sle_tag_task, 0, "BLESLETagTask",
                                      BLE_SLE_TAG_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLE_SLE_TAG_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the ble_mouse_entry. */
app_run(ble_sle_tag_entry);