/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE MICRO sample of client. \n
 *
 * History: \n
 * 2023-09-23, Create file. \n
 */

#ifndef SLE_MULTI_CONN_CLIENT_H
#define SLE_MULTI_CONN_CLIENT_H

#include "sle_ssap_client.h"
#include "sle_connection_manager.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MULTI_CONN_SEND_BUFF_MAX_LENGTH 20
#define SLE_MULTI_CONN_CLIENT_LOG "[sle multi_conn client]"

void sle_multi_conn_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb);
void sle_multi_conn_start_scan(void);
uint16_t get_sle_multi_conn_conn_id(uint8_t server_index);
uint16_t get_sle_multi_conn_client_id(void);
uint8_t get_sle_multi_conn_server_index(uint16_t conn_id);
uint16_t get_sle_multi_conn_conn_hdl(uint8_t server_index);
uint8_t get_sle_multi_conn_get_connect_state(void);
uint8_t get_ssap_connect_param_update_ready(void);
errcode_t sle_multi_conn_client_sample_dev_cbk_register(void);
void sle_multi_conn_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status);
void sle_multi_conn_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status);
void sle_multi_conn_write_req(uint8_t server_index);
void sle_multi_conn_read_req(uint8_t server_index);
uint8_t get_sle_multi_conn_param_update_ready(uint8_t server_index);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif