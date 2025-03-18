/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE MICRO sample of client. \n
 *
 * History: \n
 * 2023-09-23, Create file. \n
 */

#ifndef SLE_MULTI_CONN_SERVER_H
#define SLE_MULTI_CONN_SERVER_H

#include <stdint.h>
#include "errcode.h"
#include "osal_debug.h"
#include "sle_ssap_server.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Service UUID */
#define SLE_UUID_SERVER_SERVICE 0x2222
#define SLE_UUID_AMIC_SERVER_SERVICE 0x2224

/* Property UUID */
#define SLE_UUID_SERVER_NTF_REPORT 0x2323
#define SLE_UUID_AMIC_SERVER_NTF_REPORT 0x2325

/* Operation indication */
#define SSAP_OPERATE_INDICATION_BIT_READ_WRITE (SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE)

/* Descriptor Property */
#define SSAP_PERMISSION_READ_WRITE (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

#define SLE_MULTI_CONN_SERVER_LOG "[sle multi_conn server]"
#define SLE_SERVER_INIT_DELAY_MS 1000

/* 广播ID */
#define SLE_ADV_HANDLE_DEFAULT (CONFIG_CURRENT_SERVER_INDEX + 1)

#define SLE_ADDR_INDEX0 0
#define SLE_ADDR_INDEX1 1
#define SLE_ADDR_INDEX2 2
#define SLE_ADDR_INDEX3 3
#define SLE_ADDR_INDEX4 4
#define SLE_ADDR_INDEX5 5

errcode_t sle_multi_conn_server_init(ssaps_read_request_callback ssaps_read_callback,
                                     ssaps_write_request_callback ssaps_write_callback);
errcode_t sle_multi_conn_server_add(void);
errcode_t sle_multi_conn_amic_server_add(void);
errcode_t sle_multi_conn_server_send_notify_indicate(void);
errcode_t sle_multi_conn_amic_server_send_report_by_handle(uint8_t *data, uint8_t len, uint16_t conn_id);
uint16_t sle_multi_conn_client_is_connected(void);
int get_g_conn_update(void);
uint16_t get_g_sle_conn_hdl(void);
uint16_t get_g_sle_conn_num(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif