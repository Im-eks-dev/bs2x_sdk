/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: Provides sle Car Key NFC sample \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-02-18, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "nfc_app.h"

#define TASK_PRIORITY_NFC                 20
#define TASK_PRIORITY_NFC_IRQ             19
#define NFC_STACK_SIZE      (4*1024)
#define NFC_IRQ_STACK_SIZE      (2*1024)

static void slekey_nfc_entry(void)
{
    osal_task *task_handle0 = NULL;
    osal_task *task_handle1 = NULL;
    osal_kthread_lock();
    task_handle0 = osal_kthread_create((osal_kthread_handler)nfc_task, 0, "nfc_task",
                                      NFC_STACK_SIZE);
    task_handle1 = osal_kthread_create((osal_kthread_handler)nfc_irq_task, 0, "nfc_irq_task",
                                      NFC_IRQ_STACK_SIZE);
    if (task_handle0 != NULL && task_handle1 != NULL) {
        osal_kthread_set_priority(task_handle0, TASK_PRIORITY_NFC);
        osal_kthread_set_priority(task_handle1, TASK_PRIORITY_NFC_IRQ);
    }
    osal_kthread_unlock();
}
/* Run the usb_gamepad_entry. */
app_run(slekey_nfc_entry);