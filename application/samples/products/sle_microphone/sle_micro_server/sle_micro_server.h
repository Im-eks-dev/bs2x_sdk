/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE MICRO server Config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-23, Create file. \n
 */

#ifndef SLE_MICRO_SERVER_H
#define SLE_MICRO_SERVER_H

#include <stdint.h>
#include "sle_ssap_server.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */
#define CONFIG_SAMPLE_SUPPORT_LOW_LATENCY_TYPE
/* Service UUID */
#define SLE_UUID_SERVER_SERVICE        0x2222

/* Property UUID */
#define SLE_UUID_SERVER_NTF_REPORT     0x2323

/* Property Property */
#define SLE_UUID_TEST_PROPERTIES  (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

/* Operation indication */
#define SLE_UUID_TEST_OPERATION_INDICATION  (SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE)

/* Descriptor Property */
#define SLE_UUID_TEST_DESCRIPTOR   (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

errcode_t sle_micro_server_init(ssaps_read_request_callback ssaps_read_callback, ssaps_write_request_callback
    ssaps_write_callback);

errcode_t sle_micro_server_send_report_by_uuid(uint8_t *data, uint16_t len);

errcode_t sle_micro_server_send_report_by_handle(uint8_t *data, uint16_t len);

uint8_t get_g_sle_micro_server_conn_state(void);

uint16_t get_sle_micro_server_pair(void);

bool get_ssaps_ready(void);
uint8_t get_ssaps_server_id(void);
int get_conn_update(void);
int get_sle_conn_hdl(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif