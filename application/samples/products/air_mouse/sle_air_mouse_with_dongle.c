/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Air Mouse with dongle Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-11-16, Create file. \n
 */
#include "app_init.h"
#include "gadget/f_hid.h"
#include "soc_osal.h"
#include "securec.h"
#include "uart.h"
#include "sle_errcode.h"
#include "slp_errcode.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "dongle/air_mouse_usb/usb_init_app.h"
#if defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE)
#include "mouse/sle_air_mouse_server/sle_air_mouse_server_adv.h"
#include "mouse/sle_air_mouse_server/sle_air_mouse_server.h"
#ifdef CONFIG_AIR_MOUSE_HR_BOARD
#include "mouse/air_mouse_button/air_mouse_button.h"
#endif
#elif defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_DONGLE) || defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_GOLDEN_DONGLE)
#include "dongle/sle_air_mouse_client/sle_air_mouse_client.h"
#include "dongle/air_mouse_usb/usb_init_app.h"
#include "tcxo.h"
#endif
#include "slp.h"
#include "slp_factory.h"
#include "pm_veto.h"
#ifdef CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_GOLDEN_DONGLE
#include "dongle/air_mouse_usb/usb_serial.h"
#include "gadget/usbd_acm.h"
#endif

#define AIR_MOUSE_TASK_STACK_SIZE       0xa00
#define AIR_MOUSE_TASK_PRIO             27

#define SLP_TASK_STACK_SIZE             0xa00
#define SLP_TASK_PRIO                   27

static void *slp_task(const char *arg)
{
    osal_printk("input slp_task\r\n");
    unused(arg);

    // 启动Slp任务，并阻塞等待消息
    SlpTask(NULL);

    return NULL;
}

#if defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE)
static void *sle_air_mouse_task(const char *arg)
{
    osal_printk("input sle_air_mouse_task\r\n");
    unused(arg);
    uapi_pm_add_sleep_veto(PM_VETO_ID_SLP);
    // GLE广播
    sle_air_mouse_server_init();

#ifdef CONFIG_AIR_MOUSE_HR_BOARD
    /* 按键功能初始化 */
    air_mouse_button_init();
#endif

    // 设置Slp本机属性
    SlpLocalAtt att = {0};
    (void)memcpy_s(&att.localAddr, sizeof(SlpDeviceAddr), get_slp_air_mouse_addr(), sizeof(SlpDeviceAddr));
    ErrcodeSlpClient ret = SlpSetLocalAttCommand(&att);
    if (ret != ERRCODE_SLPC_SUCCESS) {
        osal_printk("SlpSetLocalAttCommand Error 0x%x\r\n", ret);
        return NULL;
    }
    return NULL;
}

#elif defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_DONGLE) || defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_GOLDEN_DONGLE)
static void *sle_air_mouse_dongle_task(const char *arg)
{
    osal_printk("input sle_air_mouse_dongle_task\r\n");
    unused(arg);
    uapi_pm_add_sleep_veto(PM_VETO_ID_SLP);

    // GLE 扫描连接
    sle_air_mouse_client_init();

#ifdef CONFIG_AIR_MOUSE_HR_BOARD
#ifdef CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_GOLDEN_DONGLE
    dongle_usb_serial_init();  // USB 串口设备初始化
#else
    dongle_hid_usb_init();  // USB HID设备初始化
#endif
#endif

    // 设置Slp本机属性
    SlpLocalAtt att = {0};
    (void)memcpy_s(&att.localAddr, sizeof(SlpDeviceAddr), get_slp_air_mouse_dongl_addr(), sizeof(SlpDeviceAddr));
#ifdef CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_SCREEN_TEST
    set_mouse_move_mode(HID_NEARLINK_KIND);
#else
    set_mouse_move_mode(HID_MOUSE_REL_KIND);
#endif
    set_screen_resolution(SLP_SCREEN_1080P);
    att.screenParam.resolution = SLP_SCREEN_1080P;
    att.screenParam.cursorSpeed = SLP_CURSOR_SPEED_MEDIUM;
    ErrcodeSlpClient ret = SlpSetLocalAttCommand(&att);
    if (ret != ERRCODE_SLPC_SUCCESS) {
        osal_printk("SlpSetLocalAttCommand Error 0x%x\r\n", ret);
        return NULL;
    }

#ifdef CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_GOLDEN_DONGLE
    SlpSetFactoryTestMode(SLP_FACTORY_TEST_AOX_CALI); // 相位校准模式
#else
    osal_msleep(1500); // 1500:ms,让电视识别到USB设备
    move_cursor_to_center_rel();
#endif
    return NULL;
}
#endif

static void sle_air_mouse_with_dongle_entry(void)
{
    osal_printk("input sle_air_mouse_with_dongle_entry\r\n");
    osal_task *task_cb = NULL;
    osal_task *slp_task_cb = NULL;
    osal_kthread_lock();

    slp_task_cb = osal_kthread_create((osal_kthread_handler)slp_task, NULL,
                                      "SLPTask", SLP_TASK_STACK_SIZE);
#if defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_DONGLE) || defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_GOLDEN_DONGLE)
    task_cb = osal_kthread_create((osal_kthread_handler)sle_air_mouse_dongle_task, NULL,
                                  "SLEAirMouseDongleTask", AIR_MOUSE_TASK_STACK_SIZE);
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE)
    task_cb = osal_kthread_create((osal_kthread_handler)sle_air_mouse_task, NULL,
                                  "SLEAirMouseTask", AIR_MOUSE_TASK_STACK_SIZE);
#endif

    if (slp_task_cb != NULL) {
        osal_kthread_set_priority(slp_task_cb, SLP_TASK_PRIO);
    }
    if (task_cb != NULL) {
        osal_kthread_set_priority(task_cb, AIR_MOUSE_TASK_PRIO);
    }

    osal_kthread_unlock();
}

/* Run the sle_air_mouse_with_dongle_entry. */
app_run(sle_air_mouse_with_dongle_entry);