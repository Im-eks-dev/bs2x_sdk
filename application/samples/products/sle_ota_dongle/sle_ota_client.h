/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: SLE OTA sample of client. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-02-01, Create file. \n
 */
#ifndef SLE_OTA_CLIENT_H
#define SLE_OTA_CLIENT_H

#include "stdbool.h"
#include "sle_ssap_client.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct sle_multicon_stru {
    uint8_t current_connect;
    uint8_t connected_num;
    uint8_t is_connected[CONFIG_SLE_MULTICON_NUM];
    uint16_t conn_id[CONFIG_SLE_MULTICON_NUM];
    uint8_t addr[CONFIG_SLE_MULTICON_NUM][SLE_ADDR_LEN];
} sle_multicon_stru_t;

void sle_ota_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb);
void sle_ota_start_scan(void);
uint8_t get_g_sle_enable_status(void);
uint16_t get_sle_ota_conn_id(void);
ssapc_write_param_t *get_sle_ota_send_param(void);
uint8_t get_ssap_find_ready(void);
uint8_t get_ssap_connect_param_update_ready(void);
sle_multicon_stru_t *get_sle_multicon_param(void);
uint16_t get_sle_ota_conn_state(void);
void set_sle_ota_set_address_state(bool state);
bool get_sle_ota_set_address_state(void);
void set_sle_ota_scan_state(bool state);
bool get_sle_ota_scan_state(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif