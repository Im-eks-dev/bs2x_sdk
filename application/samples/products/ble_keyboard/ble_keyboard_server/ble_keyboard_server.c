/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: BLE KEYBOARD Service Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-7-10, Create file. \n
 */
#include "stdbool.h"
#include "product.h"
#include "common_def.h"
#include "osal_debug.h"
#include "securec.h"
#include "bts_device_manager.h"
#include "bts_le_gap.h"
#include "ble_keyboard_server_adv.h"
#include "ble_hid_keyboard_server.h"
#include "ble_keyboard_server.h"

#define BLE_KEYBOARD_SERVICE_NUM 	3
#define UART16_LEN 					2
#define BLE_ENABLE_SUCCESS_FLAG 	2

static uint16_t g_ble_uart_conn_id;
uint16_t g_connection_state = GAP_BLE_STATE_DISCONNECTED;

static void ble_keyboard_adv_enable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("adv enable cbk adv_id:%d status:%d\n", adv_id, status);
}

static void ble_keyboard_adv_disable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("adv disable adv_id: %d, status:%d\n", adv_id, status);
}

uint16_t get_g_connection_state(void)
{
    return g_connection_state;
}

static void ble_keyboard_connect_change_cbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
                                            gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    g_ble_uart_conn_id = conn_id;
    g_connection_state = conn_state;
    osal_printk("connect state change conn_id: %d, status: %d, pair_status:%d, disc_reason %x\n",
                conn_id, conn_state, pair_state, disc_reason);
    osal_printk("addr:\n");
    for (uint8_t i = 0; i < BD_ADDR_LEN; i++) {
        osal_printk("%2x", addr->addr[i]);
    }
    osal_printk("\n");
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        ble_keyboard_set_adv_data();
        ble_keyboard_start_adv();
    }
}

static void ble_power_on_cbk(uint8_t status)
{
    osal_printk("power on status:%x\r\n", status);
    enable_ble();
}

static void ble_enable_cbk(uint8_t status)
{
    osal_printk("enable status:%x\r\n", status);
    /* ble enable callback will call twice, second is success */
    ble_hiddev_keyboard_server_init();
    ble_keyboard_set_adv_data();
    ble_keyboard_start_adv();
}

void bt_core_enable_cb_register(void)
{
    bts_dev_manager_callbacks_t dev_mgr_cb = { 0 };
    gap_ble_callbacks_t gap_cb = { 0 };
    dev_mgr_cb.power_on_cb = ble_power_on_cbk;
    dev_mgr_cb.ble_enable_cb = ble_enable_cbk;
    gap_cb.start_adv_cb = ble_keyboard_adv_enable_cbk;
    gap_cb.stop_adv_cb = ble_keyboard_adv_disable_cbk;
    gap_cb.conn_state_change_cb = ble_keyboard_connect_change_cbk;
    if (gap_ble_register_callbacks(&gap_cb) != ERRCODE_BT_SUCCESS ||
        bts_dev_manager_register_callbacks(&dev_mgr_cb) != ERRCODE_BT_SUCCESS) {
        osal_printk("register ble_enable_cbk failed\r\n");
    }
#if (CORE_NUMS < 2)
    enable_ble();
#endif
}

void ble_keyboard_server_init(void)
{
    bt_core_enable_cb_register();
    osal_printk("init ok\r\n");
}