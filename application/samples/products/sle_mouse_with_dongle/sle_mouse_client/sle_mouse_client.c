/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE Uart Client Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "product.h"
#include "securec.h"
#include "bts_le_gap.h"
#include "sle_device_manager.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_low_latency.h"
#include "usb_init_app.h"
#include "sle_mouse_client.h"

// 占空比计算公式：SLE_SEEK_WINDOW_DEFAULT/SLE_SEEK_INTERVAL_DEFAULT, 当前占空比为：50/100=50%
#define SLE_SEEK_INTERVAL_DEFAULT 		100
#define SLE_SEEK_WINDOW_DEFAULT 		50
#define SLE_UART_TASK_DELAY_MS 			1000

#define SLE_MOUSE_DONGLE_CLIENT_LOG     "[sle mouse dongle client]"

uint16_t sle_get_report_rate(void);

static sle_dev_manager_callbacks_t g_sle_dev_mgr_cbk = { 0 };
static sle_announce_seek_callbacks_t g_sle_mouse_client_seek_cbk = { 0 };
static sle_connection_callbacks_t g_sle_mouse_client_connect_cbk = { 0 };
static sle_addr_t g_sle_mouse_server_addr = { 0 };
static uint16_t g_sle_mouse_client_conn_id = 0;
static uint8_t g_sle_mouse_client_conn_state = SLE_ACB_STATE_NONE;

uint8_t get_g_sle_mouse_client_conn_state(void)
{
    return g_sle_mouse_client_conn_state;
}

uint16_t get_g_sle_mouse_client_conn_id(void)
{
    return g_sle_mouse_client_conn_id;
}

static void sle_mouse_client_start_scan(void)
{
    sle_seek_param_t param = { 0 };
    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 1;
    param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;
    sle_set_seek_param(&param);
    sle_start_seek();
}

void sle_mouse_client_pair_device(const sle_addr_t *addr)
{
    // 没有配对过，发起设备配对
    osal_printk("%s start device pairing\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    sle_pair_remote_device(addr);
}

static void sle_mouse_client_pair_complete_cb(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    unused(conn_id);
    unused(addr);
    unused(status);
    osal_printk("%s pair completed.\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    sle_low_latency_set(get_g_sle_mouse_client_conn_id(), true, sle_get_report_rate()); /* report_rate_idx表示4k鼠标 */
}

static void sle_mouse_client_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                                       sle_acb_state_t conn_state, sle_pair_state_t pair_state,
                                                       sle_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);
    osal_printk("%s conn state changed disc_reason:0x%x\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, disc_reason);
    g_sle_mouse_client_conn_id = conn_id;
    g_sle_mouse_client_conn_state = conn_state;
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("%s SLE_ACB_STATE_CONNECTED\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
        sle_mouse_client_pair_device(addr);
    } else if (conn_state == SLE_ACB_STATE_NONE) {
        osal_printk("%s SLE_ACB_STATE_NONE\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED && !get_isDfuRunning()) {
        osal_printk("%s SLE_ACB_STATE_DISCONNECTED\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
        sle_low_latency_set(get_g_sle_mouse_client_conn_id(), false, SLE_LOW_LATENCY_4K);
        sle_mouse_client_start_scan();
    } else {
        osal_printk("%s status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void sle_mouse_client_connect_cbk_register(void)
{
    g_sle_mouse_client_connect_cbk.connect_state_changed_cb = sle_mouse_client_connect_state_changed_cbk;
    g_sle_mouse_client_connect_cbk.pair_complete_cb = sle_mouse_client_pair_complete_cb;
    sle_connection_register_callbacks(&g_sle_mouse_client_connect_cbk);
}

static void sle_mouse_client_sle_power_on_cbk(uint8_t status)
{
    osal_printk("sle power on cbk: %d.\r\n", status);
    enable_sle();
    sle_low_latency_rx_enable();
}

static void sle_mouse_client_sle_enable_cbk(uint8_t status)
{
    osal_printk("sle enable cbk: %d.\r\n", status);
    sle_mouse_client_connect_cbk_register();
    sle_mouse_client_start_scan();
}

static void sle_mouse_client_seek_enable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_mouse_client_seek_enable_cbk, status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void sle_mouse_client_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    osal_printk("sle mouse pattern:%s\r\n", "sle_mouse");
    osal_printk("%s sle mouse scan data:%s\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, seek_result_data->data);
    if (seek_result_data == NULL) {
        osal_printk("%s status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    // 广播数据中存在数据0,使用strstr查找子字符串会提前结束导致无法匹配到设备名称,因此从第18字节开始查找
    } else if (strstr((const char *)&seek_result_data->data[1], (const char *)"sle_mouse") != NULL) {
        if (memcpy_s(&g_sle_mouse_server_addr, sizeof(sle_addr_t),
                     &seek_result_data->addr, sizeof(sle_addr_t)) != EOK) {
            osal_printk("%s sle seek result data addr memcpy fail\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
        }

        sle_stop_seek();
    }
}

static void sle_mouse_client_seek_disable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_mouse_client_seek_disable_cbk, status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    } else {
        sle_connect_remote_device(&g_sle_mouse_server_addr);
        osal_msleep(SLE_UART_TASK_DELAY_MS);
    }
}

void sle_mouse_client_seek_cbk_register(void)
{
    g_sle_dev_mgr_cbk.sle_power_on_cb = sle_mouse_client_sle_power_on_cbk;
    g_sle_dev_mgr_cbk.sle_enable_cb = sle_mouse_client_sle_enable_cbk;
    g_sle_mouse_client_seek_cbk.seek_enable_cb = sle_mouse_client_seek_enable_cbk;
    g_sle_mouse_client_seek_cbk.seek_result_cb = sle_mouse_client_seek_result_info_cbk;
    g_sle_mouse_client_seek_cbk.seek_disable_cb = sle_mouse_client_seek_disable_cbk;
    if (sle_dev_manager_register_callbacks(&g_sle_dev_mgr_cbk) != ERRCODE_BT_SUCCESS ||
        sle_announce_seek_register_callbacks(&g_sle_mouse_client_seek_cbk) != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register ble_client_enable_cb failed\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    }
#if (CORE_NUMS < 2)
    enable_sle();
    sle_low_latency_rx_enable();
#endif
    sle_mouse_client_connect_cbk_register();
}
