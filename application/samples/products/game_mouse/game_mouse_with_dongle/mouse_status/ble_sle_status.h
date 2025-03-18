/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved.
 * Description: BT PUBLIC API module.
 */

#ifndef _BLE_SLE_STATUS_H_
#define _BLE_SLE_STATUS_H_

#include "bts_def.h"
#include "sle_common.h"

#define MOUSE_EVENT_INTERVAL_MS (100)
#define CHANNEL_MAX_NUM     3

#define TURNKEY_DEVICE_NAME "Turnkey_mouse"

typedef enum {
    NONE_ADV = 0,
    BLE_ADV,
    BLE_DIRECT_ADV,
    SLE_ADV,
} mouse_adv_flag_t;

typedef enum {
    NONE_CONN = 0,
    BLE_CONN,
    SLE_CONN,
} mouse_conn_flag_t;

typedef enum {
    NONE_PAIR = 0,
    ENTRY_BLE_PAIR,
    ENTRY_SLE_PAIR,
    PAIR_BLE_SUCCESS,
    PAIR_SLE_SUCCESS
} mouse_pair_flag_t;

typedef enum {
    MOUSE_MODE_BLE = 0,
    MOUSE_MODE_SLE,
    MOUSE_MODE_NONE = 0xFF,
} mouse_mode_flag_t;

typedef enum {
    BSLE_MOUSE_INIT = 0,
    BSLE_MOUSE_CHANNEL_CHANGE_START,
    BSLE_MOUSE_CHANNEL_CHANGE_RESTART,
    BSLE_MOUSE_CHANNEL_CHANGE_STOP,
    BSLE_MOUSE_PAIR_START,
    BSLE_MOUSE_PAIR_RESTART,
    BSLE_MOUSE_PAIR_SUCC,
    BSLE_MOUSE_PAIR_STOP,
    BSLE_MOUSE_CONNECTED,
    BSLE_MOUSE_DISCONNECTED,
} mouse_pair_status_t;

typedef struct {
    uint8_t ble_mac[BD_ADDR_LEN];
    uint8_t sle_mac[BD_ADDR_LEN];
    uint8_t ble_peer_mac[BD_ADDR_LEN];
    uint8_t sle_peer_mac[BD_ADDR_LEN];
    uint8_t ble_central_type;
    uint8_t sle_central_type;
    uint8_t adv_flag;       // mouse_adv_flag
    uint8_t connect_flag;   // mouse_conn_flag
    uint8_t ble_pair_flag;  // mouse_pair_flag_t
    uint8_t sle_pair_flag;  // mouse_pair_flag_t
    uint8_t mode;           // mouse_mode_flag_t
} mouse_channel_info_t;

typedef struct {
    uint8_t ble_mac[BD_ADDR_LEN];
    uint8_t sle_mac[BD_ADDR_LEN];
    uint8_t mouse_channel;
    uint8_t ble_mouse_channel;
    uint8_t sle_mouse_channel;
    uint8_t ble_loop_mac;
    uint8_t sle_loop_mac;
    uint8_t status;         // mouse_pair_status_t
    mouse_channel_info_t channel_info[CHANNEL_MAX_NUM];
} mouse_pair_info_t;
extern mouse_pair_info_t g_pair_info;


uint8_t mouse_is_adving(uint8_t channel);
int mouse_disconnect_ble(uint8_t channel);
int mouse_disconnect_sle(uint8_t channel);
uint8_t mouse_is_connected(uint8_t channel);
int32_t stop_mouse_conn_process(uint8_t channel);
int32_t mouse_pair_timer_enable(void);
int32_t mouse_pair_timer_disable(void);
int start_mouse_adv_process(uint8_t channel);
uint32_t stop_mouse_adv_process(uint8_t channel);
void save_all_pair_info(void);
void app_mouse_pair_timer_reset(void);

void ble_pair_complete_process(uint16_t conn_id, const bd_addr_t *addr);
void sle_pair_complete_process(uint16_t conn_id, const sle_addr_t *addr);
void mouse_pair_info_debug(void);
void mouse_key_bsle_process(void);
uint32_t ble_set_mouse_adv_data(uint8_t adv_id);
uint32_t get_ble_pair_flag(void);

#endif