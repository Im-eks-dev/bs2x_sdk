/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Mouse Manager  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#include "cmsis_os2.h"
#include "app_init.h"
#include "app_os_init.h"
#include "gadget/f_hid.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "common_def.h"
#include "securec.h"
#include "uart.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "pinctrl.h"
#include "los_memory.h"
#include "bts_le_gap.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "osal_timer.h"
#include "pm_porting.h"
#include "pm_clock.h"
#include "log_reg_dump.h"
#include "watchdog.h"
#include "preserve.h"
#include "hal_reboot.h"
#include "cpu_utils.h"
#include "bts_device_manager.h"
#include "sle_device_manager.h"
#include "bts_low_latency.h"
#include "nv.h"

#include "mouse_usb/usb_init_app.h"
#include "mouse_button/mouse_button.h"
#include "mouse_sensor/mouse_sensor.h"
#include "mouse_usb/usb_init_app.h"
#include "mouse_data_transfer_manage.h"
#include "mouse_status/ble_sle_status.h"
#include "ble_bas_and_dis_server.h"
#include "mouse_mac_rules/mac_rules.h"
#include "mouse_sensor_spi.h"
#include "mouse_mode_manage.h"

#if defined(CONFIG_SAMPLE_SUPPORT_GAME_MOUSE)
#include "mouse_power/mouse_battery_manager.h"
#include "mouse_power/mouse_lowpower_manager.h"
#include "sle_mouse_server/sle_mouse_server_adv.h"
#include "sle_mouse_server/sle_mouse_hid_server.h"
#include "sle_mouse_server/sle_mouse_server.h"
#include "ble_mouse_server/ble_mouse_server_adv.h"
#include "ble_mouse_server/ble_mouse_server.h"
#include "ble_mouse_server/ble_hid_mouse_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_GAME_MOUSE_DONGLE)
#include "sle_mouse_client/sle_mouse_client.h"
#endif
#include "log_common.h"
#include "adc_porting.h"
#include "mouse_light/mouse_light.h"
#ifdef CONFIG_STORAGE_BY_FLASH
#include "sfc_flash.h"
#include "app_public.h"
#endif
#include "pm_veto.h"

// 任务创建常量
#define APP_MOUSE_MANAGER_TASK_STACK_SIZE 0xA00
#define APP_MOUSE_MANGER_TASK_PRIO (osPriority_t)(18)
#define APP_MOUSE_MANGER_TASK_DELAY_MS 2000
// 日志标题
#define APP_MOUSE_MANGER_LOG "[app mouse manger]"
// 根据调试平台自定修改USB插入侦测配置，因为DEMO鼠标的电路设计默认用ChargeFull的IO来识别是否插入充电5V
#define APP_MOUSE_USB_DETECT_INT_USE_CHARGE_FULL_PIN   0
// USB引脚定义 5V-0V表示插入与拔出:H-ChargeIn，L-ChargeFull or Out
#define PIN_MOUSE_USB S_MGPIO5

// MOUSE Charge Full引脚满电指示脚位：H-ChargeFull,L-ChargeIn
#define PIN_CHARGE_FULL_IO S_MGPIO23 // S_AGPIO5

// 档位引脚定义
#ifdef CONFIG_BSLE_BUTTON_SWITCH
#define PIN_MOUSE_SLE_MODE_CONFIG S_MGPIO18
#define PIN_MOUSE_BLE_MODE_CONFIG S_MGPIO21
#endif

static gap_conn_param_update_t g_app_ble_mouse_work = {0, 0x6, 0x6, 0, 0x1F4};
static gap_conn_param_update_t g_app_ble_mouse_standby = {0, 0x9, 0x9, 0x20, 0x1F4};

// 消息队列最大条目数
#define APP_MOUSE_MSG_QUEUE_LEN     20
// 消息队列每条消息最大数据长度
#define APP_MOUSE_MSG_QUEUE_MAX_SIZE 32
// 队列等待超时时间设置
#define APP_MOUSE_QUEUE_DELAY 0xFFFFFFFF
#define APP_MOUSE_INTERVAL_MS 200

static void app_mouse_run_current_mode(uint8_t mouse_type);
extern void evt_task_gle_acb_low_latency_set_latency_en(uint8_t en);
static void app_mouse_stop_current_mode(uint8_t mouse_type);
 
static app_mouse_dpi_rate_t g_mouse_dpi_rate = {0};
static app_mouse_handle_t g_app_mouse_handle;
uint8_t g_app_mouse_battery_percentage = 0;
bool first_get_battery_percentage = false;
const uint16_t g_work_mode_report_rate_tab[REPORT_RATE_LENGTH] = {SLE_LOW_LATENCY_125HZ, SLE_LOW_LATENCY_250HZ,
    SLE_LOW_LATENCY_500HZ, SLE_LOW_LATENCY_1K, SLE_LOW_LATENCY_2K, SLE_LOW_LATENCY_4K, SLE_LOW_LATENCY_8K};
static void mouse_data_send_timer_handler(unsigned long arg);
static osal_timer g_mouse_data_send_timer = { 0, mouse_data_send_timer_handler, 0, 5 };
extern bool g_acb_low_latency_active_enable;
bool g_is_mouse_goto_work = false;
app_mouse_handle_t get_app_mouse_handle(void)
{
    return g_app_mouse_handle;
}

uint16_t get_app_mouse_curr_report_rate(void)
{
    return g_app_mouse_handle.curr_report_rate;
}

void set_app_mouse_curr_report_rate(uint16_t rate)
{
    g_app_mouse_handle.curr_report_rate = rate;
    g_mouse_dpi_rate.sle_dongle_rate = g_app_mouse_handle.curr_report_rate;
    uapi_nv_write(0x12, (uint8_t *)&(g_mouse_dpi_rate), sizeof(app_mouse_dpi_rate_t));
}

uint16_t get_app_mouse_work_report_rate(void)
{
    return g_app_mouse_handle.work_report_rate;
}

void set_app_mouse_work_report_rate(uint16_t rate)
{
    g_app_mouse_handle.work_report_rate = rate;
}


static uint8_t get_report_rate_level(uint16_t rate)
{
    switch (rate) {
        case SLE_LOW_LATENCY_125HZ:
            return REPORT_RATE_125HZ;
        case SLE_LOW_LATENCY_250HZ:
            return REPORT_RATE_250HZ;
        case SLE_LOW_LATENCY_500HZ:
            return REPORT_RATE_500HZ;
        case SLE_LOW_LATENCY_1K:
            return REPORT_RATE_1000HZ;
        case SLE_LOW_LATENCY_2K:
            return REPORT_RATE_2000HZ;
        case SLE_LOW_LATENCY_4K:
            return REPORT_RATE_4000HZ;
        case SLE_LOW_LATENCY_8K:
            return REPORT_RATE_8000HZ;
        default:
            return REPORT_RATE_1000HZ;
    }
}

static uint8_t get_curr_report_rate_level(void)
{
    uint8_t report_rate_level;
#if defined(CONFIG_MOUSE_8K_TURNKEY)
    report_rate_level = (get_report_rate_level(g_app_mouse_handle.curr_report_rate)) % REPORT_RATE_LENGTH;
#elif defined(CONFIG_MOUSE_2K_TURNKEY)
    report_rate_level = (get_report_rate_level(g_app_mouse_handle.curr_report_rate)) % (REPORT_RATE_LENGTH - 2);
#endif
    return report_rate_level;
}

static uint8_t get_next_report_rate_level(void)
{
    uint8_t report_rate_level;
#if defined(CONFIG_MOUSE_8K_TURNKEY)
    report_rate_level = (get_report_rate_level(g_app_mouse_handle.curr_report_rate) + 1) % REPORT_RATE_LENGTH;
#elif defined(CONFIG_MOUSE_2K_TURNKEY)
    report_rate_level = (get_report_rate_level(g_app_mouse_handle.curr_report_rate) + 1) % (REPORT_RATE_LENGTH - 2);
#endif
    return report_rate_level;
}

static void app_mouse_operate_handle_init(void)
{
    g_app_mouse_handle.slide_sw_mode = 0xFF;
    g_app_mouse_handle.current_mode  = INVALID_MOUSE_MODE;
    g_app_mouse_handle.sle_mode = SLE_MODE_INVALID;
    g_app_mouse_handle.curr_report_rate = SLE_LOW_LATENCY_1K;
    g_app_mouse_handle.work_report_rate = SLE_LOW_LATENCY_1K;
    g_app_mouse_handle.first_init_index = 0;
    g_app_mouse_handle.msg_id = 0xFF;
    g_app_mouse_handle.usb_hid_index = -1;
    g_app_mouse_handle.usb_detect_times = 0;
    g_app_mouse_handle.usb_is_plugin = 0;
    g_app_mouse_handle.usb_is_device_mode = 0;
    g_app_mouse_handle.mouse_is_sleep   = 0;
    g_app_mouse_handle.dpi_value        = 1600; // dpi value: 1600
    g_app_mouse_handle.channel_id = 0;
}

__attribute__((section(".PMRAMCODE"))) int app_mouse_get_usb_mode_hid_index(void)
{
    return g_app_mouse_handle.usb_hid_index;
}

/*
 * @description: 档位按键引脚，中断回调函数,波动开关硬件设计不掉电需写处理逻辑
 * @version: V0.0.0
 */
#ifdef CONFIG_BSLE_BUTTON_SWITCH
static void app_mouse_sle_mode_config_button_cbk(pin_t pin)
{
    uint8_t mode_pin_val;
    mode_pin_val = uapi_gpio_get_val(PIN_MOUSE_SLE_MODE_CONFIG);
    osal_printk("^app_mouse_sle_mode_config_button_cbk:%d\n", mode_pin_val);
    unused(pin);
    uint8_t usb_inited;
    usb_inited = get_usb_inited();
    // usb有线接入状态且usb inited时,保持有线usb模式,不切换sle
    if (!usb_inited && g_app_mouse_handle.first_init_index) {
        // usb鼠标未初始化,且执行完开机判断USB是否插入,避免上电直接重启
        cpu_utils_set_system_status_by_cause(REBOOT_CAUSE_UPG_COMPLETION);
        hal_reboot_chip();
    }
}

static void app_mouse_ble_mode_config_button_cbk(pin_t pin)
{
    uint8_t mode_pin_val;
    mode_pin_val = uapi_gpio_get_val(PIN_MOUSE_BLE_MODE_CONFIG);
    osal_printk("^app_mouse_ble_mode_config_button_cbk:%d\n", mode_pin_val);
    unused(pin);
    uint8_t usb_inited;
    usb_inited = get_usb_inited();
    // usb有线接入状态且usb inited时,保持有线usb模式,不切换ble
    if (!usb_inited && g_app_mouse_handle.first_init_index) {
        // usb鼠标未初始化,且执行完开机判断USB是否插入,避免上电直接重启
        cpu_utils_set_system_status_by_cause(REBOOT_CAUSE_UPG_COMPLETION);
        hal_reboot_chip();
    }
}

static void app_mouse_mode_pin_init_config(void)
{
    uapi_pin_set_mode(PIN_MOUSE_SLE_MODE_CONFIG, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(PIN_MOUSE_SLE_MODE_CONFIG, GPIO_DIRECTION_INPUT);
    uapi_pin_set_pull(PIN_MOUSE_SLE_MODE_CONFIG, PIN_PULL_UP);
    uapi_pin_set_mode(PIN_MOUSE_BLE_MODE_CONFIG, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(PIN_MOUSE_BLE_MODE_CONFIG, GPIO_DIRECTION_INPUT);
    uapi_pin_set_pull(PIN_MOUSE_BLE_MODE_CONFIG, PIN_PULL_UP);
    uapi_gpio_register_isr_func(PIN_MOUSE_SLE_MODE_CONFIG, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_sle_mode_config_button_cbk);
    uapi_gpio_register_isr_func(PIN_MOUSE_BLE_MODE_CONFIG, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_ble_mode_config_button_cbk);
}

/* 上电初始化检查 */
static void app_mouse_mode_pin_check(void)
{
    uint8_t sle_mode_pin_val = uapi_gpio_get_val(PIN_MOUSE_SLE_MODE_CONFIG);
    uint8_t ble_mode_pin_val = uapi_gpio_get_val(PIN_MOUSE_BLE_MODE_CONFIG);
    if (sle_mode_pin_val == GPIO_LEVEL_LOW) {
        g_app_mouse_handle.slide_sw_mode = SLIDE_SW_MODE_SLE;
    } else if (ble_mode_pin_val == GPIO_LEVEL_LOW) {
        g_app_mouse_handle.slide_sw_mode = SLIDE_SW_MODE_BLE;
    } else {
        g_app_mouse_handle.slide_sw_mode = SLIDE_SW_MODE_USB;
    }
}
#endif
/*
 * @description: 创建鼠标场景变换消息队列
 */
static void app_mouse_create_msgqueue(void)
{
    if (osal_msg_queue_create("app_mouse_msg_queue", APP_MOUSE_MSG_QUEUE_LEN,
                              (unsigned long *)&g_app_mouse_handle.msg_id, 0,
                              sizeof(app_mouse_msg_block_t)) != EOK) {
        osal_printk("^%s create_msgqueue message queue failed!\n", APP_MOUSE_MANGER_LOG);
    }
}

/*
 * @description: 删除鼠标场景变换消息队列
 */
static void app_mouse_delete_msgqueue(void)
{
    osal_msg_queue_delete(g_app_mouse_handle.msg_id);
}

void app_mouse_send_msg(uint8_t msg_id, uint8_t msg_type)
{
    app_mouse_msg_block_t msg;
    msg.msg_id = msg_id;
    msg.msg_type = msg_type;
    msg.buf_len = 0;
    msg.buf = NULL;
    osal_msg_queue_write_copy(g_app_mouse_handle.msg_id, &msg, sizeof(msg), 0);
}

/* buf由调用者malloc申请 */
void app_mouse_send_msg_buf(uint8_t msg_id, uint8_t msg_type, uint16_t buf_len, uint8_t *buf)
{
    app_mouse_msg_block_t msg;
    msg.msg_id = msg_id;
    msg.msg_type = msg_type;
    msg.buf_len = buf_len;
    msg.buf = buf;
    osal_msg_queue_write_head_copy(g_app_mouse_handle.msg_id, &msg, sizeof(msg), 0);
}

static int app_mouse_recive_msg(unsigned long queue_id, void *buffer_addr, unsigned int *buffer_size)
{
    int ret;
    ret = osal_msg_queue_read_copy(queue_id, buffer_addr, buffer_size, APP_MOUSE_QUEUE_DELAY);
    return ret;
}

/*
 * @description: 通过mouse_id关闭当前模式
 */
static void app_mouse_stop_current_mode(uint8_t mouse_type)
{
    // 此处可通过Kconfig预设不同电平状态代表哪一种模式，暂时保留不做定义
    osal_printk("app_suspend_current_mouse_mode:%d!\n", mouse_type);
    switch (mouse_type) {
        case SLE_MOUSE_MODE:
            app_mouse_standby_timer_stop();
            /* 关闭SLE流程(sle_mouse_server提供)
            1、有连接断连、关广播，无连接关广播 */
            app_mouse_disable_transfer();
            close_sle_mode();
            break;

        case BLE_MOUSE_MODE:
            app_mouse_standby_timer_stop();
            /* 关闭BLE流程(ble_mouse_server提供)
            1、有连接断连、关广播，无连接关广播 */
            app_mouse_disable_transfer();
            close_ble_mode();
            break;

        case USB_MOUSE_MODE:
            /* 关闭USB流程(mouse_usb提供)
            1、usb deinit */
            app_mouse_disable_transfer();
            app_mouse_usb_unregister_data_cbk();
            if (close_usb_mode() == 0) {
                cpu_utils_set_system_status_by_cause(REBOOT_CAUSE_UPG_COMPLETION);
                hal_reboot_chip();
            }
            break;

        default:
            osal_printk("app_suspend_current_mouse_mode can not find mode by id!\n");
            break;
    }
}

/*
 * @description: 通过mouse_id开启新模式
 */
static void app_mouse_run_current_mode(uint8_t mouse_type)
{
    osal_printk("app_mouse_run_current_mode:%d\r\n", mouse_type);
    switch (mouse_type) {
        case SLE_MOUSE_MODE:
            app_mouse_standby_timer_start();
            /* 开启SLE流程 low lantency回调注册，起广播，等待连接后数据可传输 开启数据传输 */
            osal_printk("start sle_mouse_enable_adv:%d!\n", mouse_type);
            mouse_srv_change_channel(0);
            break;

        case BLE_MOUSE_MODE:
            app_mouse_standby_timer_start();
            /* 开启BLE流程 等待用户按键配对后发起广播 等待连接后数据可传输 */
            osal_printk("start ble_mouse_enable_adv:%d!\n", mouse_type);
            mouse_srv_change_channel(0);
            break;

        case USB_MOUSE_MODE:

            /* 开启USB流程
            1、发送USB已连接消息 */
            app_mouse_enable_transfer();
            app_mouse_send_msg(MSG_ID_CONNECTED, MSG_TYPE_ENABLE_DATA_SEND);
            app_mouse_send_msg(MSG_ID_CONNECTED, MSG_TYPE_DATA_SYNC);
            break;

        default:
            osal_printk("app_mouse_run_current_mode can not find mode by id!\n");
            break;
    }
}

/*
 * @description: USB引脚，中断回调函数
 */
#if (APP_MOUSE_USB_DETECT_INT_USE_CHARGE_FULL_PIN == 0)
static void app_mouse_mode_usb_pin_cbk(pin_t pin)
{
    unused(pin);
    uint8_t usb_pin_val;
    // 判断USB引脚高低位（等待具体调试，是否有跳变导致多次中断，预留方案软件定时轮询）
    usb_pin_val = uapi_gpio_get_val(PIN_MOUSE_USB);
    osal_printk("usb_pin_cbk:%d!\n", usb_pin_val);
    // mask 当前硬件设计不能单独用这个PIN单独来判断应用逻辑
    if (usb_pin_val == 1) {
        g_app_mouse_handle.usb_is_plugin = 1;
        app_mouse_send_msg(MSG_ID_USB_MANAGE, MSG_TYPE_USB_PIN_PLUG_IN);
    } else {
        g_app_mouse_handle.usb_is_plugin = 0;
        app_mouse_send_msg(MSG_ID_USB_MANAGE, MSG_TYPE_USB_PIN_PULL_OUT);
    }
}
#else
static void app_mouse_charge_full_pin_cbk(pin_t pin)
{
    unused(pin);
    uint8_t usb_pin_val;
    // uint8_t usb_pin_chargein_val;
    // 判断USB引脚高低位（等待具体调试，是否有跳变导致多次中断，预留方案软件定时轮询）
    usb_pin_val = uapi_gpio_get_val(PIN_MOUSE_USB);
    osal_printk("--usb_pin_cbk--:%d\n", usb_pin_val);
    /* 当这两IO口处于1个高一个低状态就表示插入USB充电模式，当全部为高就认为没有插入,此方案是因为硬件没有单独
    IO设计侦测USB插入的VBUS PIN,是一个临时解决方案，当充满电后Charge Full的IO会变成高，在这种情形下可能无法
    触发中断实现拔插逻辑行为
    */
    if (usb_pin_val == 1) {
        g_app_mouse_handle.usb_is_plugin = 0;
        app_mouse_send_msg(MSG_ID_USB_MANAGE, MSG_TYPE_USB_PIN_PULL_OUT);
    } else {
        g_app_mouse_handle.usb_is_plugin = 1;
        app_mouse_send_msg(MSG_ID_USB_MANAGE, MSG_TYPE_USB_PIN_PLUG_IN);
    }
}
#endif
/*
 * @description: USB引脚拔出设备处理流程
 */
static void app_mouse_usb_pull_out_process(void)
{
    uint8_t usb_init;
    usb_init = get_usb_inited();
    if (usb_init) {
        // 为了区分充电和usb有线模式，此处判断usb是否有线模式
        if (g_app_mouse_handle.current_mode == USB_MOUSE_MODE) {
            app_mouse_stop_current_mode(USB_MOUSE_MODE);
        }
    }
}

/*
 * @description: USB引脚插入设备处理流程
 */
static void app_mouse_usb_plug_in_process(void)
{
    uint8_t usb_init;
    usb_init = get_usb_inited();
    if (!usb_init) {
        g_app_mouse_handle.usb_hid_index = usb_init_app(DEV_HID);
        if (g_app_mouse_handle.usb_hid_index < 0) {
            osal_printk("usb_init_app fail!\n");
        }
    }
}

/*
 * @description: 开机判断USB是否插入
 */
static void app_mouse_usb_plug_check(void)
{
    uint8_t usb_pin_val;
    uint8_t usb_init;

    // USB所在引脚进行中断注册
    uapi_pin_set_mode(PIN_MOUSE_USB, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(PIN_MOUSE_USB, GPIO_DIRECTION_INPUT);
    uapi_pin_set_pull(PIN_MOUSE_USB, PIN_PULL_NONE);
    uapi_gpio_register_isr_func(PIN_MOUSE_USB, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)app_mouse_mode_usb_pin_cbk);

    // 开机初始化后进行第一次判断(可预留读取NV直接起模式)
    usb_pin_val = uapi_gpio_get_val(PIN_MOUSE_USB);
    osal_printk("usb_pin_val :%d!\n", usb_pin_val);

    if  (usb_pin_val == 0) { // usb test
        osal_printk("usb_not plug in \n");
        g_app_mouse_handle.usb_is_plugin = 0;
    } else {    // USB插入
        usb_init = get_usb_inited();
        if (!usb_init) {
            g_app_mouse_handle.usb_hid_index = usb_init_app(DEV_HID);
            if (g_app_mouse_handle.usb_hid_index < 0) {
                osal_printk("usb_init_app fail!\n");
            }
        }
        g_app_mouse_handle.usb_is_plugin = 1;
    }
    g_app_mouse_handle.first_init_index = 1;
}

/*
 * @description: 连接成功消息处理流程
 */
static void app_mouse_connected_process(uint8_t sub_msg)
{
    switch (sub_msg) {
        case MSG_TYPE_DATA_SYNC:
            // 解析特定鼠标交互数据：SLE回报率变更，宏配置数据等
            mouse_srv_change_report_rate(get_curr_report_rate_level());
            break;
        case MSG_TYPE_ENABLE_DATA_SEND:
            app_mouse_open_data_transfer(g_app_mouse_handle.current_mode);
            break;
        default:
            break;
    }
}

/*
 * @description: 断链处理流程(只存在SLE与BLE，USB为单独引脚拔插)
 */
static void app_mouse_disconnect_process(uint8_t sub_msg)
{
    osal_printk("app_mouse_disconnect_process:curr_mode:%d, sub_msg:%d\n", g_app_mouse_handle.current_mode, sub_msg);
    switch (g_app_mouse_handle.current_mode) {
        case SLE_MOUSE_MODE:
            app_mouse_disable_transfer();
            if (mouse_lowpower_get_state() == MOUSE_IN_SLEEP) {
                break;
            }
            if (sub_msg == MSG_TYPE_DISCONN_BY_REMOTE) {
                mouse_key_bsle_process();
            } else {
                // 检测鼠标是否满足进入DeepSleep的条件
            }
            break;

        case BLE_MOUSE_MODE:
            app_mouse_disable_transfer();
            if (sub_msg == MSG_TYPE_ADV_TERMINATE) {
                ble_mouse_enable_adv();
            } else if (sub_msg == MSG_TYPE_DISCONN_BY_REMOTE) {
                mouse_key_bsle_process();
            } else {
                // 检测鼠标是否满足进入DeepSleep的条件
            }
            break;

        default:
            osal_printk("app_mouse_disconnect_process can not find mode by id!\n");
            break;
    }
}

static uint8_t get_mouse_channel_by_mode(uint8_t channel_mode)
{
    uint8_t channel = 0;
    if (channel_mode == BLE_MOUSE_MODE) {
        channel = g_pair_info.ble_mouse_channel;
    } else if (channel_mode == SLE_MOUSE_MODE) {
        channel = g_pair_info.sle_mouse_channel;
    }
    return channel;
}

static void set_mouse_channel_by_mode(uint8_t channel_mode, uint8_t channel_id)
{
    if (channel_mode == BLE_MOUSE_MODE) {
        g_pair_info.ble_mouse_channel = channel_id;
        printf("[test] ble_channel:%d\n", channel_id);
    } else if (channel_mode == SLE_MOUSE_MODE) {
        g_pair_info.sle_mouse_channel = channel_id;
        printf("[test] sle_channel:%d\n", channel_id);
    }
    g_pair_info.mouse_channel = channel_id;
    return;
}

static void app_mouse_services_init(void)
{
    uint16_t nv_len = 0;
    uapi_nv_read(0x11, sizeof(mouse_pair_info_t), &nv_len, (uint8_t *)&g_pair_info);
    g_app_mouse_handle.channel_id = get_mouse_channel_by_mode(g_app_mouse_handle.current_mode);
    g_pair_info.status = BSLE_MOUSE_INIT;

    osal_printk("app_mouse_protocol_stack_en_success:%d\n", g_app_mouse_handle.current_mode);
    if (g_app_mouse_handle.current_mode == BLE_MOUSE_MODE) {
        app_mouse_ble_register_low_lantency_cbk();
        ble_mouse_services_init();
    } else if (g_app_mouse_handle.current_mode == SLE_MOUSE_MODE) {
        app_mouse_sle_low_latency_cbk_reg();
        app_mouse_sle_register_low_lantency_cbk();
        sle_low_latency_tx_enable();
        sle_mouse_services_init();
    }
    app_mouse_run_current_mode(g_app_mouse_handle.current_mode);
}

#define SW_REPORT_LEN  6

void app_mouse_send_workOrSleep_to_dongle(bool state)
{
    osal_printk("app_mouse_send_workOrSleep_to_dongle state:%d\r\n", state);
    app_mouse_notify_data_t dongleSleepOrWorkReport = {0};
    dongleSleepOrWorkReport.head = REPORT_VALUE_HEAD;
    dongleSleepOrWorkReport.type = REPORT_VALUE_TYPE;
    dongleSleepOrWorkReport.cmd = SLE_CUSTOMER_WAKEUP_OR_SLEEP;
    dongleSleepOrWorkReport.length = sizeof(state);
    dongleSleepOrWorkReport.data.state.val = state;
    errcode_t ret = sle_mouse_send_channel_notify_indicate((uint8_t *)&dongleSleepOrWorkReport, SW_REPORT_LEN);
    if (ret != 0) {
        osal_printk("app_mouse_send_workOrSleep_to_dongle ret:%d\r\n", ret);
    }
}

void app_mouse_send_battery_percentage_to_dongle(void)
{
    if (get_sle_mouse_connected() != SLE_ACB_STATE_CONNECTED) {
        return;
    }
    app_mouse_notify_data_t mouseBatteryPercentageReport = {0};
    mouseBatteryPercentageReport.head = REPORT_VALUE_HEAD;
    mouseBatteryPercentageReport.type = REPORT_VALUE_TYPE;
    mouseBatteryPercentageReport.cmd = SLE_CUSTOMER_BATTERY_LAVEL_CHECK;
    mouseBatteryPercentageReport.length = sizeof(g_app_mouse_battery_percentage);
    mouseBatteryPercentageReport.data.state.val = g_app_mouse_battery_percentage;
    errcode_t ret = sle_mouse_send_channel_notify_indicate((uint8_t *)&mouseBatteryPercentageReport, SW_REPORT_LEN);
    if (ret != 0) {
        osal_printk("app_mouse_send_battery_percentage_to_dongle ret:%d\r\n", ret);
    }
}

static void ble_mouse_goto_standby(void)
{
    if (get_ble_connection_state() == GAP_BLE_STATE_CONNECTED) {
        ble_low_latency_set_em_data(get_ble_connection_conn_id(), false);
        g_app_ble_mouse_standby.conn_handle = get_ble_connection_conn_id();
        gap_ble_connect_param_update(&g_app_ble_mouse_standby);
    } else {
        ble_mouse_stop_adv();
    }
}

static void sle_mouse_goto_standby(void)
{
    if (get_sle_mouse_connected() == SLE_ACB_STATE_CONNECTED) {
        if (sle_mouse_get_device_type() == SLE_REMOTE_DEV_DONGLE) {
            osal_printk("dongle mode, connected\r\n");
#if defined(CONFIG_MOUSE_8K_TURNKEY)
            app_mouse_send_workOrSleep_to_dongle(false);    /* 通知 dongle 关闭 low latency */
#elif defined(CONFIG_MOUSE_2K_TURNKEY)
            app_mouse_report_rate_change(SLE_LOW_LATENCY_3HZ);                /* 通知 dongle 回报率设为 3HZ */
#endif
        } else {
            osal_printk("direct connection mode, connected\r\n");
            sle_low_latency_set_em_data(get_mouse_sle_conn_id(), false);
            sle_connection_param_update_t conn_params;
            conn_params.conn_id = get_mouse_sle_conn_id();
            conn_params.interval_min = 0x3C;
            conn_params.interval_max = 0x3C;
            conn_params.max_latency = 0x20;
            conn_params.supervision_timeout = 0x1F4;
            sle_update_connect_param(&conn_params);
        }
    } else {
        osal_printk("sle mode, disconnected\r\n");
        app_mouse_sle_stop_announce();
    }
}

void app_mouse_goto_standby_process(void)
{
    switch (g_app_mouse_handle.current_mode) {
        case BLE_MOUSE_MODE:
            ble_mouse_goto_standby();
            break;

        case SLE_MOUSE_MODE:
            sle_mouse_goto_standby();
            break;

        default:
            break;
    }
    app_mouse_perp_config_before_sleep();
    mouse_lowpower_set_state(MOUSE_IN_STANDBY);
    osal_printk("mouse goto standby\r\n");
}

static void ble_mouse_goto_sleep(void)
{
    if (mouse_is_connected(g_pair_info.mouse_channel)) {
        stop_mouse_conn_process(g_pair_info.mouse_channel);
    }
}

static void sle_mouse_goto_sleep(void)
{
    if (sle_mouse_get_device_type() == SLE_REMOTE_DEV_DONGLE) {
#if defined(CONFIG_MOUSE_8K_TURNKEY)
#elif defined(CONFIG_MOUSE_2K_TURNKEY)
        /* 通知 dongle 关闭 low latency, disconnect in low_latency_cb */
        app_mouse_send_workOrSleep_to_dongle(false);
        return;
#endif
    }
    if (mouse_is_connected(g_pair_info.mouse_channel)) {
        stop_mouse_conn_process(g_pair_info.mouse_channel);
    }
}

void app_mouse_goto_sleep_process(void)
{
    switch (g_app_mouse_handle.current_mode) {
        case BLE_MOUSE_MODE:
            ble_mouse_goto_sleep();
            break;

        case SLE_MOUSE_MODE:
            sle_mouse_goto_sleep();
            break;

        default:
            break;
    }
    mouse_lowpower_set_state(MOUSE_IN_SLEEP);
    osal_printk("mouse goto sleep\r\n");
}

static void ble_mouse_standby_to_work(void)
{
    if (get_ble_connection_state() == GAP_BLE_STATE_CONNECTED) {
        ble_low_latency_set_em_data(get_ble_connection_conn_id(), true);
        g_app_ble_mouse_work.conn_handle = get_ble_connection_conn_id();
        gap_ble_connect_param_update(&g_app_ble_mouse_work);
    } else if (get_ble_connection_state() == GAP_BLE_STATE_DISCONNECTED) {
        mouse_key_bsle_process();
    }
}

static void ble_mouse_sleep_to_work(void)
{
    mouse_key_bsle_process();
    osal_printk("sleep to work\r\n");
}

bool sle_mouse_get_status(void)
{
    return g_is_mouse_goto_work;
}

void sle_mouse_set_status(bool status)
{
    g_is_mouse_goto_work = status;
}

static void mouse_data_send_timer_handler(unsigned long arg)
{
    unused(arg);
    if (sle_mouse_get_status()) {
        app_mouse_sle_report_data();
        osal_timer_start(&g_mouse_data_send_timer);
    } else {
        osal_timer_stop(&g_mouse_data_send_timer);
    }
}

static void sle_mouse_standby_to_work(void)
{
    osal_printk("mouse standby goto work, connect state:%u\r\n", get_sle_mouse_connected());
    if (get_sle_mouse_connected() == SLE_ACB_STATE_CONNECTED) {
        if (sle_mouse_get_device_type() == SLE_REMOTE_DEV_DONGLE) {
#if defined(CONFIG_MOUSE_8K_TURNKEY)
            sle_update_local_latency(get_mouse_sle_conn_id(), 1, 0);
            app_mouse_send_workOrSleep_to_dongle(true);
            g_acb_low_latency_active_enable = false;
            sle_mouse_set_status(true);
            osal_timer_start(&g_mouse_data_send_timer);
#elif defined(CONFIG_MOUSE_2K_TURNKEY)
            sle_update_local_latency(get_mouse_sle_conn_id(), 1, 0xFFFF);
            app_mouse_report_rate_change(get_app_mouse_work_report_rate());    /* 通知 dongle 由3Hz切回之前回报率 */
#endif
        } else {
            sle_low_latency_set_em_data(get_mouse_sle_conn_id(), true);
        }
    } else {
        mouse_key_bsle_process();
    }
}

static void sle_mouse_sleep_to_work(void)
{
    app_mouse_services_init();
}

void app_mouse_goto_work_process(void)
{
    app_mouse_perp_config_after_wkup();
    switch (g_app_mouse_handle.current_mode) {
        case BLE_MOUSE_MODE:
            if (mouse_lowpower_get_state() == MOUSE_STANDBY_TO_WORK) {
                ble_mouse_standby_to_work();
            } else if (mouse_lowpower_get_state() == MOUSE_SLEEP_TO_WORK) {
                ble_mouse_sleep_to_work();
            }
            break;

        case SLE_MOUSE_MODE:
            if (mouse_lowpower_get_state() == MOUSE_STANDBY_TO_WORK) {
                sle_mouse_standby_to_work();
            } else if (mouse_lowpower_get_state() == MOUSE_SLEEP_TO_WORK) {
                sle_mouse_sleep_to_work();
            }
            break;

        default:
            break;
    }
    mouse_lowpower_set_state(MOUSE_IN_WORK);
    osal_printk("mouse goto work\r\n");
}

void app_mouse_lowpower_process(app_mouse_msg_block_t *msg)
{
    unused(msg);

    if (msg->msg_type == MSG_TYPE_GOTO_STANDBY) {
        // standby处理
        app_mouse_goto_standby_process();
        app_mouse_sleep_timer_start();
        uapi_pm_remove_sleep_veto(PM_VETO_ID_SYS);
    } else if (msg->msg_type == MSG_TYPE_GOTO_SLEEP) {
        uapi_pm_add_sleep_veto(PM_VETO_ID_SYS);
        // sleep处理
        app_mouse_goto_sleep_process();
        uapi_pm_remove_sleep_veto(PM_VETO_ID_SYS);
    } else {
        uapi_pm_add_sleep_veto(PM_VETO_ID_SYS);
        // work处理
        app_mouse_goto_work_process();
        app_mouse_standby_timer_start();
    }
}

void app_mouse_restore_work_seting(void)
{
    osal_printk("\n###:restore mouse seting when exit sleep mode###\n");
    if (g_app_mouse_handle.current_mode == SLE_MOUSE_MODE) {
        restore_sle_mode_seting();
    } else if (g_app_mouse_handle.current_mode == BLE_MOUSE_MODE) {
        restore_ble_mode_seting();
    }
    g_app_mouse_handle.mouse_is_sleep = 0;
    app_mouse_run_current_mode(g_app_mouse_handle.current_mode);
}

void mouse_msg_usb_manage(app_mouse_msg_block_t *msg)
{
    if (msg->msg_type == MSG_TYPE_USB_PIN_PULL_OUT) {
        app_mouse_red_led(255); // 255 rgb value
        app_mouse_green_led(0);
        app_mouse_usb_pull_out_process();
    } else if (msg->msg_type == MSG_TYPE_USB_PIN_PLUG_IN) {
        app_mouse_red_led(0);
        app_mouse_green_led(255); // 255 rgb value
        app_mouse_usb_plug_in_process();
    }
}

void mouse_msg_connect_manage(app_mouse_msg_block_t *msg)
{
    app_mouse_red_led(0);
    app_mouse_green_led(255); // 255 rgb value
    app_mouse_connected_process(msg->msg_type);
}

void mouse_msg_disconnect_manage(app_mouse_msg_block_t *msg)
{
    app_mouse_red_led(255); // 255 rgb value
    app_mouse_green_led(0);
    app_mouse_disconnect_process(msg->msg_type);
}

static void mouse_srv_change_dpi(bool key)
{
    // 鼠标DPI调整,调用Sensor的DPI接口设置:400, 800, 1600, 3200, 6400
    osal_printk("app_mouse_button_process:%s\n", "dpi key switch");
    if (key == true) {
        if (g_app_mouse_handle.dpi_value == DPI_VALUE_400) {
            g_app_mouse_handle.dpi_value = DPI_VALUE_800;
        } else if (g_app_mouse_handle.dpi_value == DPI_VALUE_800) {
            g_app_mouse_handle.dpi_value = DPI_VALUE_1600;
        } else if (g_app_mouse_handle.dpi_value == DPI_VALUE_1600) {
            g_app_mouse_handle.dpi_value = DPI_VALUE_3200;
        } else if (g_app_mouse_handle.dpi_value == DPI_VALUE_3200) {
            g_app_mouse_handle.dpi_value = DPI_VALUE_6400;
        } else {
            g_app_mouse_handle.dpi_value = DPI_VALUE_400;
        }
    } else {
        if (g_app_mouse_handle.dpi_value == DPI_VALUE_6400) {
            g_app_mouse_handle.dpi_value = DPI_VALUE_3200;
        } else if (g_app_mouse_handle.dpi_value == DPI_VALUE_3200) {
            g_app_mouse_handle.dpi_value = DPI_VALUE_1600;
        } else if (g_app_mouse_handle.dpi_value == DPI_VALUE_1600) {
            g_app_mouse_handle.dpi_value = DPI_VALUE_800;
        } else if (g_app_mouse_handle.dpi_value == DPI_VALUE_800) {
            g_app_mouse_handle.dpi_value = DPI_VALUE_400;
        } else {
            g_app_mouse_handle.dpi_value = DPI_VALUE_6400;
        }
    }
    app_mouse_dpi_change(g_app_mouse_handle.dpi_value);
    g_mouse_dpi_rate.mouse_dpi = g_app_mouse_handle.dpi_value;
    uapi_nv_write(0x12, (uint8_t *)&(g_mouse_dpi_rate), sizeof(app_mouse_dpi_rate_t));
}

void mouse_srv_change_report_rate(uint8_t temp_rate)
{
#if defined(CONFIG_MOUSE_2K_TURNKEY)
    if (temp_rate > REPORT_RATE_2000HZ) {
        osal_printk("report_rate over limit,Valid values: 125Hz - 2000Hz\r\n");
        return;
    }
#elif defined(CONFIG_MOUSE_8K_TURNKEY)
    if (temp_rate > REPORT_RATE_8000HZ) {
        osal_printk("report_rate over limit,Valid values: 125Hz - 8000Hz\r\n");
        return;
    }
#endif
    g_app_mouse_handle.curr_report_rate = g_work_mode_report_rate_tab[temp_rate];

    if (g_app_mouse_handle.current_mode == USB_MOUSE_MODE) {
        // USB MODE
        osal_printk("=====usb mode change report_rate=====\r\n");
        app_mouse_change_polling_rate(temp_rate);
        g_mouse_dpi_rate.usb_rate = g_app_mouse_handle.curr_report_rate;
        uapi_nv_write(0x12, (uint8_t *)&(g_mouse_dpi_rate), sizeof(app_mouse_dpi_rate_t));
    } else if (g_app_mouse_handle.current_mode == SLE_MOUSE_MODE &&
        get_sle_remote_device_type() == SLE_REMOTE_DEV_DONGLE) {
        // SLE-DONGLE MODE
        osal_printk("=====SLE-DONGLE MODE change report_rate=====\r\n");
        app_mouse_report_rate_change(g_app_mouse_handle.curr_report_rate);
    }

    osal_printk("mouse report rate:%dHz\r\n", g_app_mouse_handle.curr_report_rate);
}

void mouse_msg_button_manage(app_mouse_msg_block_t *msg)
{
    printf("app_mouse_button_process:%d\n", msg->msg_type);
    switch (msg->msg_type) {
        case MSG_TYPE_DPI_UP:
            mouse_srv_change_dpi(1); // 1表示DPI+
            break;
        case MSG_TYPE_DPI_DOWN:
            mouse_srv_change_dpi(0); // 0表示DPI-
            break;
        case MSG_TYPE_CHANNEL_CHANGE:
            mouse_srv_change_channel(1);
            break;
        case MSG_TYPE_REPORT_RATE_CHANGE: // 按键切回报率
            mouse_srv_change_report_rate(get_next_report_rate_level());
            break;
        case MSG_TYPE_PAIR_PROCESS:
            entry_pair_process();
        default:
            break;
    }
}

static bool g_hso_log_off_flag = false;
static bool g_hso_serial_off_flag = false;
void mouse_log_manage_init(void)
{
    // 需从flash中获取
}

/* osal_printk中进行判断决定是否打日志 */
bool osal_printk_control(void)
{
    if (g_hso_serial_off_flag) {
        return false;
    } else {
        return true;
    }
}

void mouse_msg_log_manage(app_mouse_msg_block_t *msg)
{
    switch (msg->msg_type) {
        case MSG_TYPE_HSO_LOG_OFF:
            log_set_local_log_level(LOG_LEVEL_NONE);
            g_hso_log_off_flag = true;
            // 需要记录在flash中
            break;
        case MSG_TYPE_HSO_LOG_ON:
            log_set_local_log_level(LOG_LEVEL_INFO);
            g_hso_log_off_flag = false;
            // 需要记录在flash中
            break;
        case MSG_TYPE_SERIAL_LOG_OFF:
            g_hso_serial_off_flag = true;
            // 需要记录在flash中
            break;
        case MSG_TYPE_SERIAL_LOG_ON:
            g_hso_serial_off_flag = false;
            // 需要记录在flash中
            break;
        default:
            break;
    }
}

void mouse_battery_percentage_report(uint8_t percentage)
{
    if (percentage <= 100 && percentage > 0) {  // 100: 最高电量
        if (g_app_mouse_handle.current_mode == BLE_MOUSE_MODE) {
            ble_bas_server_battery_level_notify(percentage);
        } else if (g_app_mouse_handle.current_mode == SLE_MOUSE_MODE) {
            app_mouse_send_battery_percentage_to_dongle();
        }
    }
}

void check_battery_level(uint8_t percentage)
{
    bool charge = g_app_mouse_handle.usb_is_plugin && (g_app_mouse_battery_percentage < percentage);
    bool no_charge = !g_app_mouse_handle.usb_is_plugin && (g_app_mouse_battery_percentage > percentage);
    if (charge || no_charge || !first_get_battery_percentage) {
        g_app_mouse_battery_percentage = percentage;
        first_get_battery_percentage = true;
        mouse_battery_percentage_report(percentage);
    }
}

#if defined(CONFIG_MOUSE_8K_TURNKEY)
#define MOUSE_ADC_SAMPLE_CHANNEL    7
#elif defined(CONFIG_MOUSE_2K_TURNKEY)
#define MOUSE_ADC_SAMPLE_CHANNEL    5
#endif
#define MOUSE_BATTERY_MAX_MV    4200
#define MOUSE_BATTERY_MIN_MV    3300
static void mouse_msg_adc_manage(app_mouse_msg_block_t *msg)
{
    int32_t adc_value, vbat_mv, percentage;
    switch (msg->msg_type) {
        case MSG_TYPE_ADC_OPEN_WITH_CALI:
            adc_port_gadc_entirely_open(MOUSE_ADC_SAMPLE_CHANNEL, true);
            break;
        case MSG_TYPE_ADC_OPEN_WITHOUT_CALI:
            adc_port_gadc_entirely_open(MOUSE_ADC_SAMPLE_CHANNEL, false);
            break;
        case MSG_TYPE_ADC_SAMPLE_AND_CLOSE:
            adc_value = adc_port_gadc_entirely_sample(MOUSE_ADC_SAMPLE_CHANNEL);
            adc_port_gadc_entirely_close(MOUSE_ADC_SAMPLE_CHANNEL);
            vbat_mv = adc_value * 0x3F / 0x14;  // 根据电阻分压计算
            // 当前按照线性计算，后续可按照电池放电曲线计算
            percentage = 0x64 * (vbat_mv - MOUSE_BATTERY_MIN_MV) / (MOUSE_BATTERY_MAX_MV - MOUSE_BATTERY_MIN_MV);
            if (percentage > 0x64) {
                percentage = 0x64;
            } else if (percentage < 0) {
                percentage = 0;
            }
            check_battery_level(percentage);
            osal_printk("ADC:%d, VBAT:%dmv, %d%%\n", adc_value, vbat_mv, percentage);
            break;
        default:
            break;
    }
}

void mouse_msg_charger_manage(app_mouse_msg_block_t *msg)
{
    switch (msg->msg_type) {
        case MSG_TYPE_BAT_INFO:
        case MSG_TYPE_BAT_LEVEL:
        case MSG_TYPE_START_CHARGE:
        case MSG_TYPE_STOP_CHARGE:
        case MSG_TYPE_FACTORY_MODE:
            break;
        default:
            break;
    }
}

void mouse_msg_mode_switch_manage(app_mouse_msg_block_t *msg)
{
    uint16_t nv_len = 0;
    uapi_nv_read(0x12, sizeof(app_mouse_dpi_rate_t), &nv_len, (uint8_t *)&g_mouse_dpi_rate);

    if (g_app_mouse_handle.current_mode != msg->msg_type) {
        if (msg->msg_type == USB_MOUSE_MODE) {
            g_app_mouse_handle.curr_report_rate = g_mouse_dpi_rate.usb_rate;
        } else if (msg->msg_type == SLE_MOUSE_MODE) {
            g_app_mouse_handle.curr_report_rate = g_mouse_dpi_rate.sle_dongle_rate;
        }
        app_mouse_stop_current_mode(g_app_mouse_handle.current_mode);
        g_app_mouse_handle.current_mode = msg->msg_type;
        app_mouse_run_current_mode(msg->msg_type);
    }
}

static void app_mouse_msg_process(app_mouse_msg_block_t *msg)
{
    switch (msg->msg_id) {
        case MSG_ID_PROTOCOL_ENABLED:
            app_mouse_services_init();
            break;
        case MSG_ID_USB_MANAGE:
            mouse_msg_usb_manage(msg);
            break;
        case MSG_ID_CONNECTED:
            mouse_msg_connect_manage(msg);
            break;
        case MSG_ID_DISCONNECTED:
            mouse_msg_disconnect_manage(msg);
            break;
        case MSG_ID_OTA_MODE:
            /* code */
            break;
        case MSG_ID_LED_CHANGE:
            /* code */
            break;
        case MSG_ID_BUTTON_MANAGE:
            mouse_msg_button_manage(msg);
            break;
        case MSG_ID_MODE_SWITCH:
            mouse_msg_mode_switch_manage(msg);
            break;
        case MSG_ID_LOW_POWER:
            app_mouse_lowpower_process(msg);
            break;
        case MSG_ID_LOG_MANAGE:
            mouse_msg_log_manage(msg);
            break;
        case MSG_ID_ADC_MANAGE:
            mouse_msg_adc_manage(msg);
            break;
        case MSG_ID_CHARGER_MANAGE:
            mouse_msg_charger_manage(msg);
            break;
        default:
            break;
    }
    if (msg->buf_len != 0 && msg->buf != NULL) {
        osal_kfree(msg->buf);
        msg->buf = NULL;
    }
}

void mouse_srv_change_channel(uint8_t init)
{
    if (g_app_mouse_handle.current_mode == USB_MOUSE_MODE) {
        return;
    }

    g_pair_info.mouse_channel = get_mouse_channel_by_mode(g_app_mouse_handle.current_mode);
    if (mouse_is_adving(g_pair_info.mouse_channel)) {
        stop_mouse_adv_process(g_pair_info.mouse_channel);
    }

    if (mouse_is_connected(g_pair_info.mouse_channel)) {
        stop_mouse_conn_process(g_pair_info.mouse_channel);
    }

    if (init != 0) {
        g_app_mouse_handle.channel_id = (g_app_mouse_handle.channel_id + 1) % CHANNEL_MAX_NUM;
        set_mouse_channel_by_mode(g_app_mouse_handle.current_mode, g_app_mouse_handle.channel_id);
    }

    if (g_app_mouse_handle.current_mode == BLE_MOUSE_MODE) {
        if (g_pair_info.channel_info[g_pair_info.mouse_channel].ble_pair_flag != PAIR_BLE_SUCCESS) {
            g_pair_info.channel_info[g_pair_info.mouse_channel].ble_pair_flag = NONE_PAIR;
        }
    } else if (g_app_mouse_handle.current_mode == SLE_MOUSE_MODE) {
        if (g_pair_info.channel_info[g_pair_info.mouse_channel].sle_pair_flag != PAIR_SLE_SUCCESS) {
            g_pair_info.channel_info[g_pair_info.mouse_channel].sle_pair_flag = NONE_PAIR;
        }
    }

    g_pair_info.status = BSLE_MOUSE_CHANNEL_CHANGE_RESTART;
    g_pair_info.channel_info[g_pair_info.mouse_channel].mode = g_app_mouse_handle.current_mode;
    g_pair_info.channel_info[g_pair_info.mouse_channel].connect_flag = 0;
    g_pair_info.channel_info[g_pair_info.mouse_channel].adv_flag = 0;

    check_mouse_channel_mac(g_pair_info.mouse_channel);

    mouse_pair_info_debug();
    save_all_pair_info();
    mouse_pair_timer_enable();
    start_mouse_adv_process(g_pair_info.mouse_channel);
    return;
}

void entry_pair_process(void)
{
    osal_printk("entry_pair_process enter.\n");
    if (g_app_mouse_handle.current_mode == USB_MOUSE_MODE) {
        return;
    }

    if (g_app_mouse_handle.current_mode == BLE_MOUSE_MODE) {
        g_pair_info.channel_info[g_pair_info.mouse_channel].ble_pair_flag = ENTRY_BLE_PAIR;
        g_pair_info.ble_loop_mac++;
    } else if (g_app_mouse_handle.current_mode == SLE_MOUSE_MODE) {
        g_pair_info.channel_info[g_pair_info.mouse_channel].sle_pair_flag = ENTRY_SLE_PAIR;
        g_pair_info.sle_loop_mac++;
    }

    channel_info_store();
    if (mouse_is_adving(g_pair_info.mouse_channel)) {
        stop_mouse_adv_process(g_pair_info.mouse_channel);
    }

    if (mouse_is_connected(g_pair_info.mouse_channel)) {
        stop_mouse_conn_process(g_pair_info.mouse_channel);
    }

    set_mouse_channel_mac(g_pair_info.mouse_channel, g_app_mouse_handle.current_mode);
    g_pair_info.channel_info[g_pair_info.mouse_channel].mode = g_app_mouse_handle.current_mode;
    g_pair_info.channel_info[g_pair_info.mouse_channel].connect_flag = 0;
    g_pair_info.channel_info[g_pair_info.mouse_channel].adv_flag = 0;
    g_pair_info.status = BSLE_MOUSE_PAIR_RESTART;

    mouse_pair_info_debug();
    save_all_pair_info();
    mouse_pair_timer_enable();
    start_mouse_adv_process(g_pair_info.mouse_channel);
    return;
}

static void app_mouse_mode_loading(void)
{
    uint16_t nv_len = 0;
#ifdef CONFIG_BSLE_BUTTON_SWITCH
    app_mouse_mode_pin_init_config();
    app_mouse_mode_pin_check();
#else
    mouse_pair_info_t pair_info = {0};
    uapi_nv_read(0x11, sizeof(mouse_pair_info_t), &nv_len, (uint8_t *)&pair_info);
    g_app_mouse_handle.slide_sw_mode = pair_info.channel_info[0].mode;
#endif
#if MOUSE_AUTO_TEST
    g_app_mouse_handle.slide_sw_mode = SLIDE_SW_MODE_BLE;
#endif
    uapi_nv_read(0x12, sizeof(app_mouse_dpi_rate_t), &nv_len, (uint8_t *)&g_mouse_dpi_rate);
    g_app_mouse_handle.dpi_value = g_mouse_dpi_rate.mouse_dpi;
    app_mouse_dpi_change(g_app_mouse_handle.dpi_value);

    osal_printk("app_mouse_mode load:%d\n", g_app_mouse_handle.slide_sw_mode);
    if (g_app_mouse_handle.slide_sw_mode == SLIDE_SW_MODE_SLE) {
        app_sle_mouse_server_init();
        g_app_mouse_handle.current_mode = SLE_MOUSE_MODE;
        g_app_mouse_handle.curr_report_rate = g_mouse_dpi_rate.sle_dongle_rate;
    } else if (g_app_mouse_handle.slide_sw_mode == SLIDE_SW_MODE_BLE) {
        app_ble_mouse_power_on_register();
        g_app_mouse_handle.current_mode = BLE_MOUSE_MODE;
    } else {
        g_app_mouse_handle.curr_report_rate = g_mouse_dpi_rate.usb_rate;
    }
}

#if (MOUSE_EN_CHANGE_CPU_CLK == 1)
static void app_mouse_change_cpu_mode(uint8_t mode)
{
    if (uapi_clock_control(CLOCK_CONTROL_FREQ_LEVEL_CONFIG, mode) == ERRCODE_SUCC) {
        osal_printk("Set CPU Work Mode:%s\r\n", (mode == 0) ? "32M" : "64M");
    } else {
        osal_printk("Config fail.\r\n");
    }
}
#endif

app_slide_sw_mode_t app_mouse_current_id(void)
{
    return g_app_mouse_handle.slide_sw_mode;
}

void app_main(void *unused)
{
    UNUSED(unused);

    hal_reboot_clear_history();
    system_boot_reason_print();
    system_boot_reason_process();
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    log_exception_dump_reg_check();
#endif

#if (MOUSE_EN_CHANGE_CPU_CLK == 1)
    app_mouse_change_cpu_mode(CLOCK_FREQ_LEVEL_HIGH);
#endif

#ifndef HADM_CARKEY
#if defined(CONFIG_SAMPLE_ENABLE)
    app_tasks_init();
#endif

#endif
#ifdef OS_DFX_SUPPORT
    print_os_task_id_and_name();
#endif

    while (1) {  //lint !e716 Main Loop
        (void)osDelay(5000); // 5000ms
        uapi_watchdog_kick();
    }
}
void app_mouse_customer_pm_init(void)
{
    app_mouse_customer_init(g_app_mouse_handle.current_mode);
}

static void mouse_usb_init_succ_cb(void)
{
    osal_printk("usb init ok\r\n");
    app_mouse_send_msg(MSG_ID_MODE_SWITCH, USB_MOUSE_MODE);
}
 
void usb_init_success_flag_register_callbacks(void *callback);
static void mouse_usb_init_succ_cb_register(void *callback)
{
    usb_init_success_flag_register_callbacks(callback);
}

void app_mouse_usb_init(void)
{
    mouse_usb_init_succ_cb_register(mouse_usb_init_succ_cb);
    app_mouse_usb_plug_check();
}

static void *app_mouse_task(const char *arg)
{
    unused(arg);
    app_mouse_msg_block_t msg_buffer;
    uint32_t buffer_size = sizeof(app_mouse_msg_block_t);

    // 初始化
    osal_printk("app_mouse_task init!\n");
    // flash
#ifdef CONFIG_STORAGE_BY_FLASH
    app_public_init();
    app_flash_read_workmide_info();
#endif
    app_mouse_operate_handle_init();
    app_mouse_create_msgqueue();
    app_mouse_mode_loading();
    app_mouse_customer_init(g_app_mouse_handle.current_mode);
    app_mouse_usb_init();
    app_mouse_red_led(255); // 255 rgb value
    uapi_nv_init();
    int ret = osal_timer_init(&g_mouse_data_send_timer);
    if (ret != OSAL_SUCCESS) {
        osal_printk("g_mouse_data_send_timer create failed!\n");
    }

    while (1) {
        if (app_mouse_recive_msg(g_app_mouse_handle.msg_id, &msg_buffer, &buffer_size) == 0) {
            app_mouse_msg_process(&msg_buffer);
        }
    }
    app_mouse_delete_msgqueue();
    return NULL;
}

#if defined(CONFIG_SAMPLE_SUPPORT_GAME_MOUSE)
osal_task *g_mouse_task_handle = NULL;
#endif

void app_mouse_manager_entry(void)
{
    uapi_pm_add_sleep_veto(PM_VETO_ID_SYS);
    uapi_pm_remove_sleep_veto(PM_VETO_ID_MCU);
#if defined(CONFIG_SAMPLE_SUPPORT_GAME_MOUSE)
    osal_kthread_lock();
    g_mouse_task_handle = osal_kthread_create((osal_kthread_handler)app_mouse_task, 0, "MouseManager",
                                              APP_MOUSE_MANAGER_TASK_STACK_SIZE);
    if (g_mouse_task_handle != NULL) {
        osal_kthread_set_priority(g_mouse_task_handle, APP_MOUSE_MANGER_TASK_PRIO);
    }
    osal_kthread_unlock();
#endif
}

/* Run the sle_mouse_with_dongle. */
app_run(app_mouse_manager_entry);
