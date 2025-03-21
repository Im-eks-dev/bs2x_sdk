/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE KEYBOARD sample of client. \n
 *
 * History: \n
 * 2023-04-03, Create file. \n
 */
#ifndef SLE_KEYBOARD_CLIENT_H
#define SLE_KEYBOARD_CLIENT_H

#include "sle_ssap_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void sle_enable_cbk_register(void);
void sle_keyboard_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb);
void sle_keyboard_start_scan(void);
uint16_t get_sle_keyboard_conn_id(void);
ssapc_write_param_t get_sle_keyboard_send_param(void);
void sle_keyboard_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status);
void sle_keyboard_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif