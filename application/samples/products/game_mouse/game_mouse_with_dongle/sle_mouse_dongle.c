/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse Dongle Manager  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
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
#include "los_memory.h"
#include "sle_errcode.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_low_latency.h"
#include "mouse_usb/usb_init_app.h"
#include "sle_low_latency_service.h"
#include "bts_le_gap.h"
#include "log_reg_dump.h"
#include "watchdog.h"
#include "preserve.h"
#include "hal_reboot.h"
#include "pm_porting.h"
#include "pm_clock.h"
#include "sle_device_manager.h"
#include "nv.h"
#if defined(CONFIG_SAMPLE_SUPPORT_GAME_MOUSE_DONGLE)
#include "sle_mouse_client/sle_mouse_client.h"
#include "mouse_usb/usb_init_app.h"

#define SLE_DONGLE_TASK_STACK_SIZE 0xc00
#define SLE_DONGLE_TASK_PRIO (osPriority_t)(17)
#define SLE_DONGLE_TASK_DELAY_MS 1000
#define SLE_DONGLE_TASK_DELAY_20_MS 20
#define SLE_DONGLE_APP_DELAY_MS 5000
#define SLE_DUAL_CORE_ENABLE (0)  // 1-双核BS25，0-单核BS21
// SLE DONGLE应用消息事件定义：使能协议栈、搜索设备、连接、配对、服务特征发现、断连等
#define APP_DONGLE_MSG_QUEUE_LEN 20
// 消息队列每条消息最大数据长度
#define APP_DONGLE_MSG_QUEUE_MAX_SIZE 32
// 队列等待超时时间设置
#define APP_DONGLE_QUEUE_DELAY 0xFFFFFFFF

typedef struct {
    app_dongle_event_t msg_event;
    uint8_t msg_sub_event;
} app_dongle_msg_block_t;

typedef struct app_dongle_handle {
    int8_t usb_hid_index;
    unsigned long msg_id;
    uint16_t report_rate;
    uint32_t dpi;
} app_dongle_handle_t;

static app_dongle_handle_t g_app_dongle_handle;
static uint16_t g_last_work_report_rate = SLE_LOW_LATENCY_1K;

static osal_timer g_sle_dongle_timer;

void set_sle_dongle_report_rate(uint16_t report_rate)
{
    osal_printk("set sle dongle report_rate: %uHz\r\n", report_rate);
    if (report_rate == SLE_MOUSE_3Hz && g_app_dongle_handle.report_rate != SLE_MOUSE_3Hz) {
        g_last_work_report_rate = g_app_dongle_handle.report_rate;
    }
    g_app_dongle_handle.report_rate = report_rate;
    uapi_nv_write(0x12, (uint8_t *)&(g_app_dongle_handle.report_rate), sizeof(uint16_t));
}

uint16_t get_sle_dongle_report_rate(void)
{
    return g_app_dongle_handle.report_rate;
}

void set_sle_dongle_dpi_data(uint32_t dpi)
{
    g_app_dongle_handle.dpi = dpi;
}

/*
 * @description: 创建Dongle场景变换消息队列
 * @version: V0.0.0
 */
static void app_dongle_create_msgqueue(void)
{
    if (osal_msg_queue_create("app_dongle_msg_queue", APP_DONGLE_MSG_QUEUE_LEN,
                              (unsigned long *)&g_app_dongle_handle.msg_id, 0,
                              sizeof(app_dongle_msg_block_t)) != EOK) {
        osal_printk("^create_msgqueue message queue failed!\n");
    }
}

/*
 * @description: 删除Dongle场景变换消息队列
 * @version: V0.0.0
 */
static void app_dongle_delete_msgqueue(void)
{
    osal_msg_queue_delete(g_app_dongle_handle.msg_id);
}

/*
 * @description: 向游戏鼠标Dongle的消息队列中写入消息
 * @param msg_id 一级消息类型，参考{app_dongle_msg_type_t}
 */
void app_dongle_send_msg(uint8_t msg_event, uint8_t msg_sub_event)
{
    app_dongle_msg_block_t msg;
    msg.msg_event = msg_event;
    msg.msg_sub_event = msg_sub_event;
    osal_msg_queue_write_head_copy(g_app_dongle_handle.msg_id, &msg, sizeof(msg), 0);
}

static int app_dongle_recive_msg(unsigned long queue_id, void *buffer_addr, unsigned int *buffer_size)
{
    int ret;
    ret = osal_msg_queue_read_copy(queue_id, buffer_addr, buffer_size, APP_DONGLE_QUEUE_DELAY);
    return ret;
}

void sle_dongle_module_init(void)
{
    g_app_dongle_handle.msg_id = 0xFFFFFFFF;
    g_app_dongle_handle.usb_hid_index = -1;
    uint16_t nv_len = 0;
    uapi_nv_read(0x12, sizeof(uint16_t), &nv_len, (uint8_t *)&g_app_dongle_handle.report_rate);
    app_dongle_create_msgqueue();
    sle_mouse_client_init();
}
#if (SLE_DUAL_CORE_ENABLE)
static void bt_core_power_on_cb(errcode_t status)  // bt service
{
    osal_printk("bt_core_enable_cb status:%d\n", status);
    if (status == 0) {
        app_dongle_send_msg(SLE_DONGLE_INIT_PROTOCOL_STACK, 0);
    }
}

void bt_core_cbk_register(void)
{
    gap_ble_callbacks_t gap_cb = {0};
    gap_cb.ble_enable_cb = bt_core_power_on_cb;
    if (gap_ble_register_callbacks(&gap_cb) != 0) {
        osal_printk("register failed\r\n");
    }
}
#endif

static uint8_t sle_dongle_connect_state(void)
{
    uint8_t dongle_state = sle_mouse_client_get_connect_state();
    return dongle_state;
}

static void sle_dongle_event_cbk(unsigned long arg)
{
    UNUSED(arg);
    if (sle_dongle_connect_state() == SLE_ACB_STATE_CONNECTED) {
        app_dongle_send_msg(SLE_DONGLE_DISCOVERY_SERVICES, 0);
    }
    osal_timer_stop(&g_sle_dongle_timer);
}

static void sle_dongle_timer_init(void)
{
    uint32_t ret;
    /* 创建单次软件定时器，时间为ms，启动到ms数时执行回调函数 */
    g_sle_dongle_timer.timer = NULL;
    g_sle_dongle_timer.data = 0;
    g_sle_dongle_timer.handler = sle_dongle_event_cbk; /* 回调函数 */
    g_sle_dongle_timer.interval = 3000;                /* 3000ms */
    ret = osal_timer_init(&g_sle_dongle_timer);
    if (ret != OSAL_SUCCESS) {
        osal_printk("g_sle_dongle_timer create failed!\n");
    }
    osal_printk("g_sle_dongle_timer success\n");
}

static void sle_dongle_change_report_rate(app_sle_report_rate_t rate)
{
    if (rate > SLE_MOUSE_8000Hz) {
        osal_printk("\nSet ReportRate OverLimit !!!\n");
    }
    sle_low_latency_set(sle_mouse_client_get_connect_id(), true, rate);
}

// Sle Dongle App Main Process
void sle_dongle_report_hid_data(uint8_t msg_sub_event)
{
    if (msg_sub_event == HID_DATA_DPI) {
        fhid_send_data(
            g_app_dongle_handle.usb_hid_index, (char *)&g_app_dongle_handle.dpi, sizeof(g_app_dongle_handle.dpi));
    } else if (msg_sub_event == HID_DATA_REPORT) {
        fhid_send_data(g_app_dongle_handle.usb_hid_index,
            (char *)(uint32_t)g_app_dongle_handle.report_rate,
            sizeof(g_app_dongle_handle.report_rate));
    }
}

static void sle_dongle_report_rate_change(void)
{
    sle_dongle_change_report_rate(g_app_dongle_handle.report_rate);
    sle_mouse_client_ssapc_write_req(COMMAND_SEND_REPORT,
        (uint8_t *)&(g_app_dongle_handle.report_rate),
        sizeof(g_app_dongle_handle.report_rate));
}

static void sle_dongle_enable_low_latency(void)
{
    sle_low_latency_dongle_init(g_app_dongle_handle.usb_hid_index);
    sle_low_latency_rx_enable();
    if (g_app_dongle_handle.report_rate == SLE_LOW_LATENCY_3HZ) {
        sle_low_latency_set(sle_mouse_client_get_connect_id(), true, g_last_work_report_rate);
        set_sle_dongle_report_rate(g_last_work_report_rate);
    } else {
        sle_low_latency_set(sle_mouse_client_get_connect_id(), true, g_app_dongle_handle.report_rate);
    }
    osal_timer_start(&g_sle_dongle_timer);
}

static void app_sle_dongle_msg_process(app_dongle_msg_block_t *msg)
{
    osal_printk("sle_dongle_msg_process:%d, %d\r\n", msg->msg_event, msg->msg_sub_event);
    switch (msg->msg_event) {
        case SLE_DONGLE_INIT_PROTOCOL_STACK:
            enable_sle();
            break;
        case SLE_DONGLE_SCAN_DEVICES:
            sle_mouse_client_start_scan();
            break;
        case SLE_DONGLE_CONNECTED:
            sle_dongle_enable_low_latency();
            break;
        case SLE_DONGLE_PAIR_DEVICES:
            sle_mouse_client_pair_device();
            break;
        case SLE_DONGLE_DISCONNECTED:
            sle_mouse_client_disconnect_process();
            break;
        case SLE_DONGLE_DISCOVERY_SERVICES:
            sle_mouse_client_discover_service();
            break;
        case SLE_DONGLE_WAKE_UP_HOST:
            usb_device_remote_wakeup();
            break;
        case SLE_DONGLE_CHANGE_MOUSE_REPORT_RATE:
            sle_dongle_report_rate_change();
            break;
        case SLE_DONGLE_ENABLE_LOW_LATENCY:
            sle_low_latency_set(sle_mouse_client_get_connect_id(), true, g_app_dongle_handle.report_rate);
            break;
        case SLE_DONGLE_DISABLE_LOW_LATENCY:
            sle_low_latency_set(sle_mouse_client_get_connect_id(), false, g_app_dongle_handle.report_rate);
            break;
        case SLE_DONGLE_HID_SEND_DATA:
            sle_dongle_report_hid_data(msg->msg_sub_event);
            break;
        case SLE_DONGLE_SET_DPI:
            sle_mouse_client_ssapc_write_req(
                COMMAND_SEND_DPI, (uint8_t *)&g_app_dongle_handle.dpi, sizeof(g_app_dongle_handle.dpi));
            break;
        default:
            break;
    }
}

static void *sle_dongle_task(const char *arg)
{
    app_dongle_msg_block_t msg_buffer;
    uint32_t buffer_size = sizeof(app_dongle_msg_block_t);

    unused(arg);
    uapi_nv_init();
    g_app_dongle_handle.usb_hid_index = usb_init_app(DEV_SER_HID);
    osal_printk("usb_hid_init: %d\n", g_app_dongle_handle.usb_hid_index);
    if (g_app_dongle_handle.usb_hid_index < 0) {
        return NULL;
    }
    // USB客制化通信功能注册回调
    usb_register_customer_cmd(sle_mouse_client_send_customer_data);
    // Dongle定时器注册
    sle_dongle_timer_init();
    while (1) {
        if (app_dongle_recive_msg(g_app_dongle_handle.msg_id, &msg_buffer, &buffer_size) == 0) {
            app_sle_dongle_msg_process(&msg_buffer);
        }
    }
    app_dongle_delete_msgqueue();
    return NULL;
}

static void app_dongle_usb_wakeup_handler(void)
{
    bool is_usb_recv_key = usb_dongle_is_recv_vaild_data();
    if (usb_device_is_host_suspended()) {
        if (is_usb_recv_key) {
            app_dongle_send_msg(SLE_DONGLE_WAKE_UP_HOST, 0);  // 发消息给donlge task wakeup pc
        }
    }
}

// 重定义app_main
void app_main(void *unused)
{
    UNUSED(unused);
    hal_reboot_clear_history();
    system_boot_reason_print();
    system_boot_reason_process();
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    log_exception_dump_reg_check();
#endif

    sle_dongle_module_init();  // 初始化sle client
    // 注册B核上电消息事件(BS25 Dual Core需要注册，BS21系列不需要直接使能协议栈即可)
#if (SLE_DUAL_CORE_ENABLE)
    bt_core_cbk_register();
#endif
    app_tasks_init();

    // 改变MCU的工作频率,BS25:96MHz,BS21:64Mhz
    if (uapi_clock_control(CLOCK_CONTROL_FREQ_LEVEL_CONFIG, CLOCK_FREQ_LEVEL_HIGH) == ERRCODE_SUCC) {
        osal_printk("Config succ.\r\n");
    } else {
        osal_printk("Config fail.\r\n");
    }

    while (1) {
        (void)osDelay(SLE_DONGLE_APP_DELAY_MS);
        app_dongle_usb_wakeup_handler();
        uapi_watchdog_kick();
    }
}

static void sle_dongle_main(void)
{
    osThreadAttr_t attr;

    attr.name = "sle_dongle_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = SLE_DONGLE_TASK_STACK_SIZE;
    attr.priority = SLE_DONGLE_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)sle_dongle_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}
/* Run the sle_mouse_with_dongle. */
app_run(sle_dongle_main);
#endif
