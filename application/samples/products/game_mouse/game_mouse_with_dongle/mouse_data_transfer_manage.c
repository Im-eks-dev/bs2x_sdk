/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 *
 * Description: Mouse Data Transfer SAMPLE. \n
 *
 * History: \n
 * 2024-04-21, Create file. \n
 */
#include "cmsis_os2.h"
#include "app_init.h"
#include "app_os_init.h"
#include "gadget/f_hid.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "common_def.h"
#include "securec.h"
#include "bts_le_gap.h"
#include "usb_porting.h"
#include "sle_low_latency.h"
#include "osal_timer.h"
#include "mouse_usb/usb_init_app.h"
#include "mouse_button/mouse_button.h"
#include "mouse_key/mouse_key.h"
#include "mouse_sensor/mouse_sensor.h"
#include "mouse_wheel/mouse_wheel.h"
#include "mouse_power/mouse_battery_manager.h"
#include "sle_mouse_server/sle_mouse_server_adv.h"
#include "sle_mouse_server/sle_mouse_hid_server.h"
#include "sle_mouse_server/sle_mouse_server.h"
#include "ble_mouse_server/ble_mouse_server_adv.h"
#include "ble_mouse_server/ble_mouse_server.h"
#include "ble_mouse_server/ble_hid_mouse_server.h"
#include "mouse_mode_manage.h"
#include "sle_connection_manager.h"
#include "bts_low_latency.h"
#include "mouse_light/mouse_light.h"
#include "mouse_power/mouse_lowpower_manager.h"
#include "mouse_status/ble_sle_status.h"
#include "mouse_sensor_spi.h"
#include "mouse_data_transfer_manage.h"

// 鼠标建议改为01好做USB远程按键唤醒,但hid report map需要同步对应修改
#define USB_MOUSE_REPORT_ID 0x01
// 鼠标BLE默认用8ms单位做1次数据发送：8ms =125Hz
#define BLE_INTERVAL_MS     8 // 8 //单位：ms
#define SLE_INTERVAL_MS     20 // 8 //单位：ms
#define SSAP_NOTIFY_HEAD_LEN 5

// 鼠标通用接收数据结构
static app_mouse_report_data_t g_app_mouse_data = {0};
// sensor操作结构
static mouse_sensor_oprator_t g_app_mouse_sensor_operator = {0};

// Ble数据发送软件定时器结构
static ble_hid_high_mouse_event_st g_app_mouse_ble_data = {0};
static sle_mouse_data_t g_mouse_notify_data = { 0 };

#if defined(CONFIG_MOUSE_8K_TURNKEY)
static uint8_t g_app_mouse_usb_polling_rate = 1;
#elif defined(CONFIG_MOUSE_2K_TURNKEY)
static uint8_t g_app_mouse_usb_polling_rate = 2;
#endif
static bool g_app_mouse_data_report = false;

// USB模式下回报率切换，数据上报频率映射数组
#if defined(CONFIG_MOUSE_2K_TURNKEY)
    static uint8_t polling_arr_2K [5] = {32, 16, 8, 4, 2};  // 2K鼠标，默认4K采样频率，4000/32 = 125,对应回报率125Hz - 2KHz
#elif defined(CONFIG_MOUSE_8K_TURNKEY)
    static uint8_t polling_arr_8K [7] = {64, 32, 16, 8, 4, 2, 1}; // 8K鼠标，默认8K采样频率，8000/64 = 125,对应回报率125Hz - 8KHz
#endif
static mouse_sensor_t g_mouse_sensor;
extern bool g_acb_low_latency_active_enable;

#if (MOUSE_AUTO_TEST == 1)
#include "math.h"
#define CIR_NUM    100
#define CIR_RADIUS 150
void app_mouse_auto_score_data(int16_t *x_pos, int16_t *y_pos)
{
    int i;
    static int16_t circle[CIR_NUM+1][2] = {{0x8FFF}};
    static int idx = -1;

    if (idx < 0) {
        for (i = 0; i < CIR_NUM + 1; i++) {
            circle[i][0] = CIR_RADIUS * cos(2 * 3.1416 * (i % CIR_NUM) / CIR_NUM); // x_pos: 2 π 3.1416
            circle[i][1] = CIR_RADIUS * sin(2 * 3.1416 * (i % CIR_NUM) / CIR_NUM); // y_pos: 2 π 3.1416
        }

        for (i = 0; i < CIR_NUM; i++) {
            circle[i][0] = circle[i + 1][0] - circle[i][0];
            circle[i][1] = circle[i + 1][1] - circle[i][1];
        }
    }

    idx = (idx + 1) % CIR_NUM;

    if (idx == CIR_NUM) {
        idx = -1;
    }

    *x_pos = circle[idx][0];
    *y_pos = circle[idx][1];
}
#endif

void app_mouse_set_sensor_mode(uint8_t reportRate)
{
    g_app_mouse_sensor_operator.set_mode(reportRate);
}

void app_mouse_change_polling_rate(uint8_t reportRate)
{
#if defined(CONFIG_MOUSE_2K_TURNKEY)
    g_app_mouse_usb_polling_rate = polling_arr_2K[reportRate];
#elif defined(CONFIG_MOUSE_8K_TURNKEY)
    g_app_mouse_usb_polling_rate = polling_arr_8K[reportRate];
#endif
    app_mouse_set_sensor_mode(reportRate);
}

bool app_mouse_data_report_enable(void)
{
    return g_app_mouse_data_report;
}
void app_mouse_disable_transfer(void)
{
    g_app_mouse_data_report = false;
}

void app_mouse_enable_transfer(void)
{
    g_app_mouse_data_report = true;
}

static bool mouse_data_protocol_0_valid(app_mouse_report_data_protocol_0_t *mouse_data)
{
    if ((mouse_data->x != 0) || (mouse_data->y != 0) || (mouse_data->key.d8 != 0)) {
        return true;
    }
    return false;
}

static bool mouse_data_valid(app_mouse_report_data_t *mouse_data)
{
    static uint8_t mouse_prev_button = 0;
    if ((mouse_data->x != 0) || (mouse_data->y != 0)) {
        return true;
    }
    if (mouse_data->key.d8 != mouse_prev_button) {
        mouse_prev_button = mouse_data->key.d8;
        return true;
    }
    if (mouse_data->wheel != 0) {
        return true;
    }
    return false;
}
/* SLE EM Data Transmit */
uint8_t *sle_hid_mouse_low_latency_data_cbk(uint8_t *length, uint16_t *ssap_handle,
    uint8_t *data_type, uint16_t co_handle)
{
    UNUSED(co_handle);
    int16_t x = 0;
    int16_t y = 0;
    if (mouse_lowpower_get_state() != MOUSE_IN_WORK) {
        return NULL;
    }
#if (MOUSE_AUTO_TEST == 1)
    app_mouse_auto_score_data(&x, &y);
#else
    g_app_mouse_sensor_operator.get_xy(&x, &y);
#endif

    g_app_mouse_data.x = x;
    g_app_mouse_data.y = y;
    if (!mouse_data_valid(&g_app_mouse_data)) {
        return NULL;
    }
    app_mouse_idle_time_reset();
    app_mouse_pair_timer_reset();
    g_mouse_notify_data.button = g_app_mouse_data.key.d8;
    g_mouse_notify_data.x = x;
    g_mouse_notify_data.y = y;
    g_mouse_notify_data.wheel = g_app_mouse_data.wheel;
    g_app_mouse_data.wheel = 0;

    *length = sizeof(g_mouse_notify_data);
    *ssap_handle = sle_mouse_get_ssap_rpt_handle();
    *data_type = 0;
    return (uint8_t *)&g_mouse_notify_data;
}

void sle_set_em_data_cbk(uint16_t co_handle, uint8_t status)
{
    osal_printk("sle_set_em_data_cbk, co_handle: %d, status: %d \r\n", co_handle, status);
}

void app_mouse_sle_low_latency_cbk_reg(void)
{
    sle_low_latency_callbacks_t cbks = {0};
    cbks.hid_data_cb = sle_hid_mouse_low_latency_data_cbk;
    cbks.sle_set_em_data_cb = sle_set_em_data_cbk;
    sle_low_latency_register_callbacks(&cbks);
}

/* SLE Low Latency Data Transmit */
uint8_t *app_mouse_sle_data_report_cbk(uint16_t *len)
{
    if (sle_mouse_get_status()) {
        sle_mouse_set_status(false);
#if defined(CONFIG_MOUSE_8K_TURNKEY)
        g_acb_low_latency_active_enable = true;
#endif
    }
    int16_t x = 0;
    int16_t y = 0;
    if (app_mouse_data_report_enable() == false) {
        return NULL;
    }
    if (mouse_lowpower_get_state() != MOUSE_IN_WORK) {
        return NULL;
    }
#if (MOUSE_AUTO_TEST == 1)
    app_mouse_auto_score_data(&x, &y);
#else
    g_app_mouse_sensor_operator.get_xy(&x, &y);
#endif

    g_app_mouse_data.x = x;
    g_app_mouse_data.y = y;
    if (!mouse_data_valid(&g_app_mouse_data)) {
        return NULL;
    }
    app_mouse_idle_time_reset();
    app_mouse_pair_timer_reset();
    g_mouse_notify_data.button = g_app_mouse_data.key.d8;
    g_mouse_notify_data.x = x;
    g_mouse_notify_data.y = y;
    g_mouse_notify_data.wheel = g_app_mouse_data.wheel;
    g_app_mouse_data.wheel = 0;
    *len = sizeof(g_mouse_notify_data);

    return (uint8_t *)&g_mouse_notify_data;
}

void app_mouse_sle_report_data(void)
{
    int16_t x = 0;
    int16_t y = 0;
    if (app_mouse_data_report_enable() == false) {
        return;
    }
    if (mouse_lowpower_get_state() != MOUSE_IN_WORK) {
        return;
    }
#if (MOUSE_AUTO_TEST == 1)
    app_mouse_auto_score_data(&x, &y);
#else
    g_app_mouse_sensor_operator.get_xy(&x, &y);
#endif
    g_app_mouse_data.x = x;
    g_app_mouse_data.y = y;
    if (!mouse_data_valid(&g_app_mouse_data)) {
        return;
    }
    app_mouse_idle_time_reset();
    app_mouse_pair_timer_reset();
    g_mouse_notify_data.button = g_app_mouse_data.key.d8;
    g_mouse_notify_data.x = x;
    g_mouse_notify_data.y = y;
    g_mouse_notify_data.wheel = g_app_mouse_data.wheel;
    g_app_mouse_data.wheel = 0;
    app_mouse_data_report_t mouseDataReport = {0};
    mouseDataReport.head = REPORT_VALUE_HEAD;
    mouseDataReport.type = REPORT_VALUE_TYPE;
    mouseDataReport.cmd = SLE_CUSTOMER_MOUSE_DATA;
    mouseDataReport.length = sizeof(g_mouse_notify_data);
    mouseDataReport.data = g_mouse_notify_data;
    sle_mouse_send_channel_notify_indicate((uint8_t *)&mouseDataReport,
        sizeof(g_mouse_notify_data) + SSAP_NOTIFY_HEAD_LEN);
}

/*
 * @description: sle低时延注册回调
*/
void app_mouse_sle_register_low_lantency_cbk(void)
{
    sle_low_latency_tx_callbacks_t app_mouse_sle_cbk;
    app_mouse_sle_cbk.low_latency_tx_cb = app_mouse_sle_data_report_cbk;
    sle_low_latency_tx_register_callbacks(&app_mouse_sle_cbk);
    osal_printk("##sle_low_latency_tx_register_callbacks!\n");
    return;
}

/* BLE EM Data Transmit */
uint8_t *ble_hid_mouse_low_latency_data_cbk(uint8_t *length, uint16_t *att_handle, uint16_t co_handle)
{
    UNUSED(co_handle);
    int16_t x = 0;
    int16_t y = 0;
    if (mouse_lowpower_get_state() != MOUSE_IN_WORK) {
        return NULL;
    }
#if (MOUSE_AUTO_TEST == 1)
    app_mouse_auto_score_data(&x, &y);  // 自动产生鼠标画圆坐标测试数据
#else
    g_app_mouse_sensor_operator.get_xy(&x, &y);
#endif

    g_app_mouse_data.x = x;
    g_app_mouse_data.y = y;
    if (!mouse_data_valid(&g_app_mouse_data)) {
        return NULL;
    }
    app_mouse_idle_time_reset();
    app_mouse_pair_timer_reset();
    g_app_mouse_ble_data.button_mask = g_app_mouse_data.key.d8;
    g_app_mouse_ble_data.x = x;
    g_app_mouse_ble_data.y = y;
    g_app_mouse_ble_data.wheel = g_app_mouse_data.wheel;
    g_app_mouse_data.wheel = 0;

    *length = sizeof(ble_hid_high_mouse_event_st);
    *att_handle = ble_get_hid_report_att_hdl();
    return (uint8_t *)&g_app_mouse_ble_data;
}

void ble_hid_mouse_low_latency_em_data_cbk(uint16_t conn_handle, uint8_t status)
{
    osal_printk("ble_hid_mouse_low_latency_data_send handle:%d, status:%d.\n", conn_handle, status);
}

void app_mouse_ble_register_low_lantency_cbk(void)
{
    ble_low_latency_callbacks_t app_mouse_ble_cbk = {0};
    app_mouse_ble_cbk.hid_data_cb = ble_hid_mouse_low_latency_data_cbk;
    app_mouse_ble_cbk.ble_set_em_data_cb = ble_hid_mouse_low_latency_em_data_cbk;
    ble_low_latency_register_callbacks(&app_mouse_ble_cbk);
    osal_printk("##ble_low_latency_tx_register_callbacks!\n");
    return;
}

/*
 * @description: USB数据传输回调
*/
static void app_usb_mouse_report_data(uint8_t **data, uint16_t *length, uint8_t *device_index)
{
    static uint8_t usb_sof_cnt = 0;
    usb_sof_cnt = (usb_sof_cnt + 1) % g_app_mouse_usb_polling_rate;
    if (usb_sof_cnt != 0) {
        return;
    }
    if (app_mouse_data_report_enable() == false) {
        return;
    }

    int g_usb_mouse_hid_index = -1;
    g_usb_mouse_hid_index = app_mouse_get_usb_mode_hid_index();
    int16_t x = 0;
    int16_t y = 0;

#if (MOUSE_AUTO_TEST == 1)
    app_mouse_auto_score_data(&x, &y);
#else
    g_app_mouse_sensor_operator.get_xy(&x, &y);
#endif
    app_mouse_idle_time_reset();
    app_mouse_pair_timer_reset();
    uint8_t protocol = fhid_get_protocol(USB_MOUSE_DEVICE_ID);
    static app_mouse_report_data_t usb_mouse_message = {0};
    usb_mouse_message.kind = USB_MOUSE_REPORT_ID;
    usb_mouse_message.x = x;
    usb_mouse_message.y = y;
    usb_mouse_message.wheel = g_app_mouse_data.wheel;
    usb_mouse_message.key.d8 = g_app_mouse_data.key.d8;
    // 空包数据不发处理
    *length = 0;

    if (mouse_data_valid(&usb_mouse_message) && protocol != 0) {
        *length = sizeof(app_mouse_report_data_t);
        *data = (uint8_t *)&usb_mouse_message;
        *device_index = g_usb_mouse_hid_index;
        g_app_mouse_data.wheel = 0;
        return;
    }

    static app_mouse_report_data_protocol_0_t usb_mouse_message_protocol_0 = {0};
    usb_mouse_message_protocol_0.key.d8 = g_app_mouse_data.key.d8;
    usb_mouse_message_protocol_0.x = (int8_t)x;
    usb_mouse_message_protocol_0.y = (int8_t)y;
    if (mouse_data_protocol_0_valid(&usb_mouse_message_protocol_0) && protocol == 0) {
        *data = (uint8_t *)&usb_mouse_message_protocol_0;
        *length = REPORT_LEN_BIOS;
    }
    g_app_mouse_data.wheel = 0;
}

static void app_mouse_ble_data_report_enable(void)
{
    if (app_mouse_data_report_enable() == false) {
        app_mouse_enable_transfer();
    }
}

void app_mouse_sle_data_report_enable(void)
{
    if (app_mouse_data_report_enable() == false) {
        app_mouse_enable_transfer();
    }
}

void app_mouse_user_data_process(uint8_t *msg)
{
    osal_printk("app_mouse_user_data_process: %02x\n", msg[0]);
}

void app_mouse_usb_unregister_data_cbk(void)
{
    usb_unregister_callback();
}

/*
 * @description: 根据模式id开启数据传输
*/
void app_mouse_open_data_transfer(uint8_t mode_type)
{
    switch (mode_type) {
        case BLE_MOUSE_MODE:
            osal_printk("start ble game mode data transfer now\n");
            app_mouse_ble_data_report_enable();
            break;

        case SLE_MOUSE_MODE:
            osal_printk("start sle game mode data transfer now\n");
            app_mouse_sle_data_report_enable();
            break;

        case USB_MOUSE_MODE:
            osal_printk("start usb game mode data transfer now\n");
            usb_register_callback(app_usb_mouse_report_data);
            if (app_mouse_data_report_enable() == false) {
                app_mouse_enable_transfer();
            }
            break;

        default:
            osal_printk("app_mouse_open_data_transfer can not find mode by id!\n");
            break;
    }
}

void app_mouse_set_dpi_index(uint32_t dpi)
{
    UNUSED(dpi);
}

void app_mouse_save_report_rate(uint16_t report_rate)
{
    UNUSED(report_rate);
}

#define DPI_REPORT_LEN 9
#define REPORT_RATE_REPORT_VALUE_LEN 2
#define REPORT_RATE_REPORT_LEN 7
void app_mouse_dpi_change(uint32_t dpi)
{
    UNUSED(dpi);
#if (MOUSE_AUTO_TEST == 0)
    if (g_app_mouse_sensor_operator.set_sensor_dpi != NULL) {
        if (g_app_mouse_sensor_operator.set_sensor_dpi(dpi) != 0) {
            osal_printk("set_sensor_dpi failed!\n");
        }
    } else {
        osal_printk("app_mouse_dpi_change failed!, dpi:%u\n", dpi);
        return;
    }
    app_mouse_handle_t temp_handle  = get_app_mouse_handle();
    if (temp_handle.current_mode == SLE_MOUSE_MODE && get_sle_remote_device_type() == SLE_REMOTE_DEV_DONGLE) {
        app_mouse_notify_data_t dongleDpiReport = {0};
        app_mouse_set_dpi_index(dpi);  // dpi 闪存读写

        dongleDpiReport.head = REPORT_VALUE_HEAD;
        dongleDpiReport.type = REPORT_VALUE_TYPE;
        dongleDpiReport.cmd = SLE_CUSTOMER_REQ_CHANGE_DPI;
        dongleDpiReport.length = sizeof(dpi);
        dongleDpiReport.data.dpi = dpi;
        sle_mouse_send_channel_notify_indicate((uint8_t *)&dongleDpiReport, DPI_REPORT_LEN);
    }

#endif
}

void app_mouse_report_rate_change(uint16_t report_rate)
{
    app_mouse_save_report_rate(report_rate);  // reportRate 闪存读写
    app_mouse_notify_data_t dongleReportRateReport = {0};
    dongleReportRateReport.head = REPORT_VALUE_HEAD;
    dongleReportRateReport.type = REPORT_VALUE_TYPE;
    dongleReportRateReport.cmd = SLE_CUSTOMER_REQ_CHANGE_REPORT_RATE;
    dongleReportRateReport.length = REPORT_RATE_REPORT_VALUE_LEN;
    dongleReportRateReport.data.reportRate.val = report_rate;
    sle_mouse_send_channel_notify_indicate((uint8_t *)&dongleReportRateReport, REPORT_RATE_REPORT_LEN);
}

void app_mouse_set_reset_mode(void)
{
    g_app_mouse_sensor_operator.setrest(true);
    g_app_mouse_sensor_operator.set_mode(SENSOR_OFFICE_MODE);
}

void app_mouse_lift_mode_check(void)
{
#if (MOUSE_AUTO_TEST == 0)
    static mouse_status prev_liftstate = MOUSE_STATUS_INVALID_PARAMETER;
    mouse_status current_liftstate = g_app_mouse_sensor_operator.liftCut_check();
    if (current_liftstate != prev_liftstate) {
        prev_liftstate = current_liftstate;
            if (current_liftstate == MOUSE_STATUS_OK) {
            osal_printk("\n app mouse work ok");
            g_app_mouse_sensor_operator.setrest(false);
        } else {
            osal_printk("\n app mouse work error");
            g_app_mouse_sensor_operator.setrest(true);
        }
    }
#endif
}

static void app_mouse_sensor_init(uint8_t mouse_mode)
{
    unused(mouse_mode);
    g_mouse_sensor = mouse_sensor_seclect_type();
    g_app_mouse_sensor_operator = get_mouse_sensor_operator(g_mouse_sensor);
    g_app_mouse_sensor_operator.init();
}

void app_mouse_wheel_init(void)
{
    mouse_wheel_init(&(g_app_mouse_data.wheel));
}

/*
 * @description:鼠标Sensor外设、编码器、按键,电池ADC、低功耗模块等初始化
*/
void app_mouse_customer_init(uint8_t mouse_mode)
{
    UNUSED(mouse_mode);
#ifdef CONFIG_SINGLE_BUTTON_MODE
    app_mouse_button_init(&(g_app_mouse_data.key));
#else
    mouse_key_init(&(g_app_mouse_data.key));
#endif
    app_mouse_sensor_init(mouse_mode);
    app_mouse_wheel_init();
    app_mouse_battery_init();
    app_mouse_lowpower_init();
    app_mouse_red_led(255); // 255 rgb value
}
