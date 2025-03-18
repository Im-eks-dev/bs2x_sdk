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
#include "usb_porting.h"
#include "mouse_sensor.h"
#include "mouse_button.h"
#include "mouse_wheel.h"
#include "usb_init_app.h"

#define USB_MOUSE_REPORT_ID             0x2

#define USB_MOUSE_TASK_PRIO             24
#define USB_MOUSE_TASK_STACK_SIZE       0x2000

typedef struct usb_hid_mouse_report {
    uint8_t kind;
    usb_hid_mouse_key_t key;
    int8_t x;                 /* A negative value indicates that the mouse moves left. */
    int8_t y;                 /* A negative value indicates that the mouse moves up. */
    int8_t wheel;             /* A negative value indicates that the wheel roll forward. */
} usb_hid_mouse_report_t;

static int g_usb_mouse_hid_index = -1;
static uint8_t g_usb_mouse_polling_rate = 1;
static usb_hid_mouse_report_t g_send_mouse_msg;
static mouse_sensor_oprator_t g_usb_hid_hs_mouse_operator;

static void mouse_cb(uint8_t **data, uint16_t *length, uint8_t *device_index)
{
    static uint8_t usb_sof_cnt = 0;
    usb_sof_cnt = (usb_sof_cnt + 1) % g_usb_mouse_polling_rate;
    if (usb_sof_cnt != 0) {
        return;
    }
    static usb_hid_mouse_report_t mouse_message = { 0 };
    mouse_message.kind = USB_MOUSE_REPORT_ID;
    int16_t x = 0;
    int16_t y = 0;

    g_usb_hid_hs_mouse_operator.get_xy(&x, &y);
    mouse_message.x = x;
    mouse_message.y = y;
    mouse_message.wheel = g_send_mouse_msg.wheel;
    mouse_message.key.d8 = g_send_mouse_msg.key.d8;
    g_send_mouse_msg.wheel = 0;
    *data = (uint8_t *)&mouse_message;
    *length = sizeof(usb_hid_mouse_report_t);
    *device_index = g_usb_mouse_hid_index;
}

static void *usb_mouse_task(const char *arg)
{
    unused(arg);
    g_usb_mouse_hid_index = usb_init_app(DEV_HID);
    if (g_usb_mouse_hid_index < 0) {
        return NULL;
    }

    g_send_mouse_msg.kind = USB_MOUSE_REPORT_ID;

    g_usb_hid_hs_mouse_operator = get_mouse_sensor_operator(PWM3395DM);
    g_usb_hid_hs_mouse_operator.init();

    mouse_button_init(&(g_send_mouse_msg.key));
    mouse_wheel_init(&(g_send_mouse_msg.wheel));
    usb_register_callback(mouse_cb);

    return NULL;
}

static void usb_mouse_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)usb_mouse_task, 0, "USBMouseTask",
                                      USB_MOUSE_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, USB_MOUSE_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the usb_mouse_entry. */
app_run(usb_mouse_entry);