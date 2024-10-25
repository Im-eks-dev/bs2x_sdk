/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Provides ble server demo API \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-12-29， Create file. \n
 */

#ifndef PM_BLE_SERVER_H
#define PM_BLE_SERVER_H

#include "bts_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_PM_SYS_SUPPORT)
#define DURATION_MS_OF_WORK_TO_STANDBY    60000
#define DURATION_MS_OF_STANDBY_TO_SLEEP   60000
#endif

#define PM_WORK_BLE_CON_MIN_INTERVAL      6
#define PM_WORK_BLE_CON_MAX_INTERVAL      9
#define PM_WORK_BLE_CON_LATENCY           2
#define PM_WORK_BLE_CON_TIMEOUT           300

#define PM_STANDBY_BLE_CON_MIN_INTERVAL   6
#define PM_STANDBY_BLE_CON_MAX_INTERVAL   9
#define PM_STANDBY_BLE_CON_LATENCY        30
#define PM_STANDBY_BLE_CON_TIMEOUT        300

extern uint16_t g_pm_connect_handle;
extern uint16_t g_pm_connection_state;
extern bd_addr_t g_pm_ble_addr;

void pm_ble_server_init(void);

errcode_t pm_ble_connect_param_update(uint32_t min_interval, uint32_t max_interval,
                                      uint32_t latency, uint32_t timeout);

void pm_ble_work_to_standby(void);

void pm_ble_standby_to_work(void);

void pm_ble_standby_to_sleep(void);

void pm_ble_sleep_to_work(void);

void pm_ble_set_state_trans_duration(void);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif
