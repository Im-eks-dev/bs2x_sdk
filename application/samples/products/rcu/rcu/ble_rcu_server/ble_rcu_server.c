/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: BLE RCU Server Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-9-10, Create file. \n
 */
#include "securec.h"
#include "stdbool.h"
#include "soc_osal.h"
#include "bts_le_gap.h"
#include "bts_device_manager.h"
#include "ble_rcu_server_adv.h"
#include "ble_hid_rcu_server.h"
#include "ble_rcu_server.h"

#define BLE_RCU_SERVICE_NUM 		3
#define BLE_RCU_TASK_DELAY_MS 		200
#define UART16_LEN 					2

static uint16_t g_ble_rcu_conn_id;
uint16_t g_connection_state = GAP_BLE_STATE_DISCONNECTED;
static uint16_t g_sle_enable = false;
static bd_addr_t g_ble_addr = { 0 };
/* 低功耗连接参数信息 */
static gap_conn_param_update_t g_worktostandby = { 0, 6, 9, 30, 3000 };
static gap_conn_param_update_t g_standbytowork = { 0, 9, 9, 2, 3000 };

static bool g_low_power_state = false; /* false:关闭, true:打开 */

static void ble_rcu_adv_enable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("adv enable cbk adv_id:%d status:%d\n", adv_id, status);
}

static void ble_rcu_adv_disable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("adv disable adv_id: %d, status:%d\n", adv_id, status);
}

uint16_t get_g_connection_state(void)
{
    return g_connection_state;
}

static void ble_rcu_connect_change_cbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
                                       gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    g_ble_rcu_conn_id = conn_id;
    g_connection_state = conn_state;
    osal_printk("connect state change conn_id: %d, status: %d, pair_status:%d, addr %x disc_reason %x\n",
                conn_id, conn_state, pair_state, addr[0], disc_reason);
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
        memcpy_s(&g_ble_addr, sizeof(bd_addr_t), addr, sizeof(bd_addr_t));
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        memset_s(&g_ble_addr, sizeof(bd_addr_t), 0, sizeof(bd_addr_t));
        if (!g_low_power_state) {
            ble_rcu_set_adv_data();
            ble_rcu_start_adv();
        }
    }
}

static void ble_enable_cbk(uint8_t status)
{
    osal_printk("enable status:%d\r\n", status);
    g_sle_enable = true;
}

static void bt_core_enable_cb_register(void)
{
    bts_dev_manager_callbacks_t dev_cb = { 0 };
    gap_ble_callbacks_t gap_cb = { 0 };
    dev_cb.ble_enable_cb = ble_enable_cbk;
    gap_cb.start_adv_cb = ble_rcu_adv_enable_cbk;
    gap_cb.stop_adv_cb = ble_rcu_adv_disable_cbk;
    gap_cb.conn_state_change_cb = ble_rcu_connect_change_cbk;
    if (gap_ble_register_callbacks(&gap_cb) != ERRCODE_BT_SUCCESS ||
        bts_dev_manager_register_callbacks(&dev_cb) != ERRCODE_BT_SUCCESS) {
        osal_printk("register ble_enable_cbk failed\r\n");
    }
}

void ble_rcu_work_to_standby(void)
{
    if (g_connection_state == GAP_BLE_STATE_CONNECTED) {
        g_worktostandby.conn_handle = g_ble_rcu_conn_id;
        gap_ble_connect_param_update(&g_worktostandby);
    }
}

void ble_rcu_standby_to_work(void)
{
    if (g_connection_state == GAP_BLE_STATE_CONNECTED) {
        g_standbytowork.conn_handle = g_ble_rcu_conn_id;
        gap_ble_connect_param_update(&g_standbytowork);
    }
    g_low_power_state = false;
}

void ble_rcu_standby_to_sleep(void)
{
    if (g_connection_state == GAP_BLE_STATE_CONNECTED) {
        gap_ble_disconnect_remote_device(&g_ble_addr);
        g_low_power_state = true;
    } else if (g_connection_state == GAP_BLE_STATE_DISCONNECTED) {
        gap_ble_stop_adv(BTH_GAP_BLE_ADV_HANDLE_DEFAULT);
    }
}

void ble_rcu_sleep_to_work(void)
{
    gap_ble_start_adv(BTH_GAP_BLE_ADV_HANDLE_DEFAULT);
    g_low_power_state = false;
}

void ble_rcu_server_init(void)
{
    bt_core_enable_cb_register();
    enable_ble();
    while (g_sle_enable == false) {
        osal_msleep(BLE_RCU_TASK_DELAY_MS);
    }
    ble_hid_rcu_server_init();
    ble_rcu_set_adv_data();
    ble_rcu_start_adv();
    osal_printk("init ok\r\n");
}