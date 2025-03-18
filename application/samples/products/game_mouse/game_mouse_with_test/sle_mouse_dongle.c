/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: Mouse PCBA TEST SAMPLE. \n
 * Author: @CompanyNameMagicTag \n
 * History: \n
 * 2022-07-27, Create file. \n
 */

#include "cmsis_os2.h"
#include "app_init.h"
#include "gadget/f_hid.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "common_def.h"
#include "securec.h"
#include "uart.h"
#include "app_os_init.h"
#include "usb_init_app.h"

#define MOUSE_TEST_TASK_STACK_SIZE 0xc00
#define MOUSE_TEST_TASK_PRIO (osPriority_t)(17)
#define SLE_DONGLE_TASK_DELAY_MS 1000
#define SLE_DONGLE_TASK_DELAY_20_MS 20
#define SLE_DONGLE_APP_DELAY_MS 5000
#define SLE_DUAL_CORE_ENABLE        (0) // 1-双核BS25，0-单核BS21
// SLE DONGLE应用消息事件定义：使能协议栈、搜索设备、连接、配对、服务特征发现、断连等
#define APP_DONGLE_MSG_QUEUE_LEN     20
// 消息队列每条消息最大数据长度
#define APP_DONGLE_MSG_QUEUE_MAX_SIZE 32
// 队列等待超时时间设置
#define APP_DONGLE_QUEUE_DELAY 0xFFFFFFFF

static void *mouse_test_task(const char *arg)
{
    unused(arg);
#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
    int usb_hid_index = usb_init_app(DEV_SER_HID);
#else
    int usb_hid_index = usb_init_app(DEV_HID);
#endif
    osal_printk("usb_hid_init: %d\n", usb_hid_index);

    return NULL;
}

static void sle_dongle_main(void)
{
    osThreadAttr_t attr;

    attr.name = "mouse_test_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = MOUSE_TEST_TASK_STACK_SIZE;
    attr.priority = MOUSE_TEST_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)mouse_test_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}
/* Run the sle_mouse_with_dongle. */
app_run(sle_dongle_main);
