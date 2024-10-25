/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Provides ble server demo API \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-12-29， Create file. \n
 */

#include <stdint.h>
#include "errcode.h"
#include "osal_debug.h"
#include "bts_le_gap.h"
#if defined(CONFIG_PM_SYS_SUPPORT)
#include "pm_sys.h"
#endif
#include "pm_ble_hid_server.h"
#include "pm_ble_server_adv.h"
#include "pm_ble_server.h"

uint16_t g_pm_connect_handle = 0;
uint16_t g_pm_connection_state = GAP_BLE_STATE_DISCONNECTED;
bd_addr_t g_pm_ble_addr = { 0 };

void pm_ble_server_init(void)
{
    pm_ble_hid_server_init();
    pm_ble_set_adv_data();
    pm_ble_start_adv();
}

errcode_t pm_ble_connect_param_update(uint32_t min_interval, uint32_t max_interval,
                                      uint32_t latency, uint32_t timeout)
{
    gap_conn_param_update_t conn_params;
    conn_params.conn_handle = g_pm_connect_handle;
    conn_params.interval_min = min_interval;
    conn_params.interval_max = max_interval;
    conn_params.slave_latency = latency;
    conn_params.timeout_multiplier = timeout;
    return gap_ble_connect_param_update(&conn_params);
}

void pm_ble_work_to_standby(void)
{
    if (g_pm_connection_state == GAP_BLE_STATE_CONNECTED) {
        errcode_t ret = pm_ble_connect_param_update(PM_STANDBY_BLE_CON_MIN_INTERVAL, PM_STANDBY_BLE_CON_MAX_INTERVAL,
                                                    PM_STANDBY_BLE_CON_LATENCY, PM_STANDBY_BLE_CON_TIMEOUT);
        if (ret != ERRCODE_BT_SUCCESS) {
            osal_printk("[work2standby]Ble connect param update fail, ret = 0x%x", ret);
        }
    }
}

void pm_ble_standby_to_work(void)
{
    if (g_pm_connection_state == GAP_BLE_STATE_CONNECTED) {
        errcode_t ret = pm_ble_connect_param_update(PM_WORK_BLE_CON_MIN_INTERVAL, PM_WORK_BLE_CON_MAX_INTERVAL,
                                                    PM_WORK_BLE_CON_LATENCY, PM_WORK_BLE_CON_TIMEOUT);
        if (ret != ERRCODE_BT_SUCCESS) {
            osal_printk("[standby2work]Ble connect param update fail, ret = 0x%x", ret);
        }
    }
}

void pm_ble_standby_to_sleep(void)
{
    errcode_t ret;
    if (g_pm_connection_state == GAP_BLE_STATE_CONNECTED) {
        ret = gap_ble_disconnect_remote_device(&g_pm_ble_addr);
        if (ret != ERRCODE_BT_SUCCESS) {
            osal_printk("[standby2sleep]Ble disconnect remote device fail, ret = 0x%x", ret);
        }
    } else if (g_pm_connection_state == GAP_BLE_STATE_DISCONNECTED) {
        ret = gap_ble_stop_adv(BTH_GAP_BLE_ADV_HANDLE_DEFAULT);
        if (ret != ERRCODE_BT_SUCCESS) {
            osal_printk("[standby2sleep]Ble stop adv fail, ret = 0x%x", ret);
        }
    }
}

void pm_ble_sleep_to_work(void)
{
    errcode_t ret = gap_ble_start_adv(BTH_GAP_BLE_ADV_HANDLE_DEFAULT);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("[sleep2work]Ble start adv fail, ret = 0x%x", ret);
    }
}

#if defined(CONFIG_PM_SYS_SUPPORT)
void pm_ble_set_state_trans_duration(void)
{
    uapi_pm_work_state_reset();
    uapi_pm_set_state_trans_duration(DURATION_MS_OF_WORK_TO_STANDBY, DURATION_MS_OF_STANDBY_TO_SLEEP);
}
#endif
