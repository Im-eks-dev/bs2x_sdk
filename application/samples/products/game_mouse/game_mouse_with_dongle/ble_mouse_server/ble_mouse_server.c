/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 *
 * Description: BLE Mouse Service Server SAMPLE. \n
 *
 * History: \n
 * 2024-04-21, Create file. \n
 */
#include "stdbool.h"
#include "stdint.h"
#include "common_def.h"
#include "osal_addr.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "osal_timer.h"
#include "securec.h"
#include "errcode.h"
#include "bts_def.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "bts_le_gap.h"
#include "bts_device_manager.h"
#include "bts_gatt_server.h"
#include "bts_low_latency.h"
#include "ble_mouse_server_adv.h"
#include "ble_hid_mouse_server.h"
#include "ble_sle_status.h"
#include "mouse_mode_manage.h"
#include "ble_mouse_server_adv.h"
#include "mouse_lowpower_manager.h"
#include "ble_mouse_server.h"

#define BLE_MOUSE_SERVICE_NUM 3
#define BLE_MOUSE_TASK_DELAY_MS 2000
#define UART16_LEN 2

#define BLE_MOUSE_SERVER_LOG "[ble mouse server]"

#if (CORE_NUMS > 1)
#define BLE_ENABLE_READY_FLAG   1
#define BLE_ENABLE_SUCCESS_FLAG 2
#else
#define BLE_ENABLE_READY_FLAG   0
#define BLE_ENABLE_SUCCESS_FLAG 1 /* 单核BT整体启动后APP线程才注册回调 */
#endif

uint16_t g_app_ble_mouse_connection = GAP_BLE_STATE_DISCONNECTED;
uint16_t g_app_ble_mouse_pair_state = GAP_BLE_PAIR_NONE;
static uint8_t g_ble_mouse_active_disconnect = true; // 鼠标是否主动断开连接
static uint16_t g_app_ble_mouse_conn_id = 0xFFFF;
static ble_connect_state_t g_app_ble_mouse_conn_state = BLE_CONNECT_NONE;
static bd_addr_t g_ble_addr; // = {0};
static void app_ble_mouse_connect_callback(unsigned long arg);
static osal_timer g_app_mouse_ble_conn_timer = {0, app_ble_mouse_connect_callback, 0, 1000};

static void ble_mouse_adv_enable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("%s adv enable cbk adv_id:%d status:%d\n", BLE_MOUSE_SERVER_LOG, adv_id, status);
}

static void ble_mouse_adv_disable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("%s adv disable adv_id: %d, status:%d\n", BLE_MOUSE_SERVER_LOG, adv_id, status);
}

void close_ble_mode(void)
{
    g_ble_mouse_active_disconnect = true;
    if (g_app_ble_mouse_connection == GAP_BLE_STATE_CONNECTED) {
        gap_ble_disconnect_remote_device(&g_ble_addr);
    } else if (mouse_is_adving(g_pair_info.mouse_channel)) {
        ble_mouse_stop_adv();
    }
}
void restore_ble_mode_seting(void)
{
    g_ble_mouse_active_disconnect = false;
}
uint16_t get_ble_pair_state(void)
{
    return g_app_ble_mouse_pair_state;
}

uint8_t get_ble_mouse_active_disconnect(void)
{
    return g_ble_mouse_active_disconnect;
}

uint16_t get_ble_connection_state(void)
{
    return g_app_ble_mouse_connection;
}

uint16_t get_ble_connection_conn_id(void)
{
    return g_app_ble_mouse_conn_id;
}

void ble_connect_ready_to_send_data(void)
{
    app_mouse_send_msg(MSG_ID_CONNECTED, MSG_TYPE_ENABLE_DATA_SEND);
}

static void app_ble_mouse_connect_callback(unsigned long arg)
{
    unused(arg);
    ble_connect_ready_to_send_data();
    osal_timer_stop(&g_app_mouse_ble_conn_timer);
}

static void ble_mouse_connect_state_process(void)
{
    // 连接后轮询检测是否需要发起连接间隔更新请求
    ble_mouse_disable_adv();
    g_app_ble_mouse_conn_state = BLE_CONNECT_CHECK_INTERVAL;
}

static void ble_mouse_disconnect_state_process(void)
{
    g_app_ble_mouse_pair_state = GAP_BLE_PAIR_NONE;
    g_app_ble_mouse_conn_state = BLE_CONNECT_NONE;
    g_app_ble_mouse_connection = GAP_BLE_STATE_DISCONNECTED;
    if (g_app_mouse_ble_conn_timer.timer != NULL) {
        osal_timer_stop(&g_app_mouse_ble_conn_timer);
    }
}

void app_mouse_ble_disc_process(uint8_t disc_reason)
{
    switch (disc_reason) {
        case BLE_DISCONNECT_BY_REMOTE_USER:
        case BLE_DISCONNECT_BY_REMOTE_DEVICE_POWER_OFF:
        case BLE_ESTABLISH_CONNECT_FAIL:  // 0x3E建链失败后 重启广播
        case BLE_DISCONNECT_BY_CONNECT_TIMEOUT:
            app_mouse_send_msg(MSG_ID_DISCONNECTED, MSG_TYPE_DISCONN_BY_REMOTE);
            break;
        default:
            osal_printk("ble disconnect,reason:0x%x.\n", disc_reason);
            break;
    }
}

static void ble_mouse_connect_change_cbk(uint16_t conn_id,
                                         bd_addr_t *addr,
                                         gap_ble_conn_state_t conn_state,
                                         gap_ble_pair_state_t pair_state,
                                         gap_ble_disc_reason_t disc_reason)
{
    osal_printk("%s connect state change conn_id: %d, status: %d, pair_status:%d, addr %x disc_reason %x\n",
                BLE_MOUSE_SERVER_LOG, conn_id, conn_state, pair_state, addr->type, disc_reason);
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
        if (memcpy_s(&g_ble_addr, sizeof(bd_addr_t), addr, sizeof(bd_addr_t)) != ERRCODE_SUCC) {
            osal_printk("memcpy_s ble addr fail!\r\n");
        }
        g_pair_info.channel_info[g_pair_info.mouse_channel].connect_flag = BLE_CONN;
        ble_mouse_connect_state_process();
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        ble_mouse_disconnect_state_process();
        g_pair_info.status = BSLE_MOUSE_DISCONNECTED;
        g_pair_info.channel_info[g_pair_info.mouse_channel].connect_flag = NONE_CONN;
        app_mouse_ble_disc_process(disc_reason);
    }
    g_app_ble_mouse_conn_id = conn_id;
    g_app_ble_mouse_connection = conn_state;
}

static void ble_pair_state_change_cbk(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    osal_printk("##pair status:%d, conn_id %x addr type %x\r\n", status, conn_id, addr->type);
    if (status == ERRCODE_SUCC) {
        ble_low_latency_set_em_data(conn_id, true);
        ble_pair_complete_process(conn_id, addr);

        g_app_ble_mouse_pair_state = GAP_BLE_PAIR_PAIRED;
        if (g_app_mouse_ble_conn_timer.timer != NULL) {
            osal_timer_start(&g_app_mouse_ble_conn_timer);
        }
    }
}

static void ble_mouse_set_nv_smp_keys(const bd_addr_t *addr, const ble_auth_info_evt_t* evt)
{
    bd_addr_t local_addr;
    local_addr.type = SLE_ADDRESS_TYPE_PUBLIC;
    if (memcpy_s(local_addr.addr, BD_ADDR_LEN,
        g_pair_info.channel_info[g_pair_info.mouse_channel].ble_mac, BD_ADDR_LEN) != EOK) {
        return;
    }
    
    bd_addr_t peer_addr;
    peer_addr.type = addr->type;
    if (memcpy_s(peer_addr.addr, BD_ADDR_LEN, addr->addr, BD_ADDR_LEN) != EOK) {
        return;
    }

    ble_auth_info_evt_t keys;
    if (memcpy_s(&keys, sizeof(ble_auth_info_evt_t), evt, sizeof(ble_auth_info_evt_t)) != EOK) {
        return;
    }
    /* ble dev_pc smp keys save to nv idx 3 4 5 */
    ble_set_nv_pair_keys(&keys, &local_addr, &peer_addr, g_pair_info.mouse_channel + 3);
}

static void ble_mouse_auth_compleate_cbk(uint16_t conn_id, const bd_addr_t *addr, errcode_t status,
    const ble_auth_info_evt_t* evt)
{
    osal_printk("###auth_compleate status:%d, conn_id %x addr type: %x\r\n", status, conn_id, addr->type);
    ble_mouse_set_nv_smp_keys(addr, evt);
    osal_printk("\r\n Link Key dump end.\r\n");
}

static void ble_mouse_conn_param_update_cbk(uint16_t conn_id, errcode_t status,
    const gap_ble_conn_param_update_t *param)
{
    osal_printk("###conn_param_update status:%d, conn_id: %x param->interval: %x, param->latency: %x\r\n",
        status, conn_id, param->interval, param->latency);
}

static void ble_mouse_adv_terminate_cb(uint8_t adv_id, adv_status_t status)
{
    osal_printk("[BLE]adv terminate cbk adv_id: %d, status:%d\n", adv_id, status);
    if (g_app_ble_mouse_connection == GAP_BLE_STATE_CONNECTED) {
        return;
    }
    if (mouse_is_adving(g_pair_info.mouse_channel)) {
        app_mouse_send_msg(MSG_ID_DISCONNECTED, MSG_TYPE_ADV_TERMINATE);
    }
}

static void ble_enable_cbk(uint8_t status)
{
    app_mouse_send_msg(MSG_ID_PROTOCOL_ENABLED, 0);
    /* 开启主动指定BLE配对信息存放NV索引功能 */
    gap_ble_set_save_smp_keys_mode(GAP_BLE_SAVE_SMP_KEYS_MANU);
    gap_ble_set_pair_info_available(GAP_BLE_PAIR_INFO_AVAILABLE);
    osal_printk("ble_enable_cbk: status = 0x%x \r\n", status);
}

static void bt_core_enable_cb_register(void)
{
    bts_dev_manager_callbacks_t dev_mgr_cb = { 0 };
    gap_ble_callbacks_t gap_cb = {0};
    dev_mgr_cb.ble_enable_cb = ble_enable_cbk;
    gap_cb.start_adv_cb = ble_mouse_adv_enable_cbk;
    gap_cb.stop_adv_cb = ble_mouse_adv_disable_cbk;
    gap_cb.conn_state_change_cb = ble_mouse_connect_change_cbk;
    gap_cb.pair_result_cb = ble_pair_state_change_cbk;
    gap_cb.auth_complete_cb = ble_mouse_auth_compleate_cbk;
    gap_cb.conn_param_update_cb = ble_mouse_conn_param_update_cbk;
    gap_cb.terminate_adv_cb = ble_mouse_adv_terminate_cb;

    if (gap_ble_register_callbacks(&gap_cb) != ERRCODE_BT_SUCCESS||
     bts_dev_manager_register_callbacks(&dev_mgr_cb) != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register ble_enable_cbk failed\r\n", BLE_MOUSE_SERVER_LOG);
    }
    #if (CORE_NUMS < 2)
    enable_ble();
    #endif
}

void ble_mouse_enable_adv(void)
{
    ble_mouse_start_adv();
}

void ble_mouse_disable_adv(void)
{
    osal_printk("ble mouse conncet active stop adv\n");
    ble_mouse_stop_adv();
}

void ble_mouse_services_init(void)
{
    ble_hiddev_mouse_server_init();
}

void app_ble_mouse_power_on_register(void)
{
    int ret;
    osal_printk("app_mouse_bt_core_register_init\r\n");
    bt_core_enable_cb_register();
    ble_hid_server_register_gatt_callbacks();
    ret = osal_timer_init(&g_app_mouse_ble_conn_timer);
    if (ret != 0) {
        osal_printk("g_app_mouse_ble_conn_timer create failed!\n");
    }
}

