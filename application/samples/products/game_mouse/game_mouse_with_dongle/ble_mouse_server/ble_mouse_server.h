/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 *
 * Description: BLE Mouse Service Server SAMPLE. \n
 *
 * History: \n
 * 2024-04-21, Create file. \n
 */
#ifndef BLE_UUID_SERVER_H
#define BLE_UUID_SERVER_H

#include "bts_def.h"

typedef enum {
    BLE_CONNECT_NONE,          // 未连接
    BLE_CONNECT_CHECK_INTERVAL, // 已连接空闲状态检测是否需要更新连接间隔
    BLE_CHANGE_INTERVAl_REQ, // 已连接请求更新连接间隔状态
    BLE_CHANGE_INTERVAl_IND, // 已连接更新连接成功
} ble_connect_state_t;

/* Service UUID */
#define BLE_UUID_UUID_SERVER_SERVICE                 0xABCD
/* Characteristic UUID */
#define BLE_UUID_UUID_SERVER_REPORT                  0xCDEF
/* Client Characteristic Configuration UUID */
#define BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION 0x2902
/* Server ID */
#define BLE_UUID_SERVER_ID 1
/* Characteristic Property */
#define UUID_SERVER_PROPERTIES   (GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_NOTIFY)

/**
 * @if Eng
 * @brief  BLE uuid server inir.
 * @attention  NULL
 * @retval BT_STATUS_SUCCESS    Excute successfully
 * @retval BT_STATUS_FAIL       Execute fail
 * @par Dependency:
 * @li bts_def.h
 * @else
 * @brief  BLE UUID服务器初始化。
 * @attention  NULL
 * @retval BT_STATUS_SUCCESS    执行成功
 * @retval BT_STATUS_FAIL       执行失败
 * @par 依赖:
 * @li bts_def.h
 * @endif
 */
void ble_mouse_enable_adv(void);
void ble_mouse_disable_adv(void);
void app_ble_mouse_power_on_register(void);
uint16_t get_ble_connection_state(void);
uint16_t get_ble_connection_conn_id(void);
uint16_t get_ble_pair_state(void);
void close_ble_mode(void);
void restore_ble_mode_seting(void);
void ble_mouse_services_init(void);
void ble_connect_ready_to_send_data(void);

#endif

