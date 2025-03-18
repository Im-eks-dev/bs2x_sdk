/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: USB Mouse Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-07, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "usb_init_keyboard_app.h"
#include "keyscan_init.h"

#define USB_KEYSCAN_TASK_PRIO           24
#define USB_KEYSCAN_TASK_STACK_SIZE     0x2000

static void *usb_keyscan_task(const char *arg)
{
    unused(arg);

    int index = usb_init_keyboard_app(DEV_HID);
    if (index < 0) {
        return NULL;
    }

    keyscan_init(index);

    return NULL;
}

static void usb_keyscan_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)usb_keyscan_task, 0, "USBKeyboardTask",
                                      USB_KEYSCAN_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, USB_KEYSCAN_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the usb_keyscan_entry. */
app_run(usb_keyscan_entry);