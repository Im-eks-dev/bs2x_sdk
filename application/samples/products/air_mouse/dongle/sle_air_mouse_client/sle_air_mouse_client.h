/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
* Description: Sle Air Mouse with dongle Client header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#ifndef SLE_AIR_MOUSE_CLIENT_H
#define SLE_AIR_MOUSE_CLIENT_H

#include "sle_ssap_client.h"
#include "slp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void sle_air_mouse_client_init(void);

uint16_t get_g_sle_air_mouse_client_conn_id(void);

uint8_t get_g_sle_air_mouse_client_conn_state(void);

const SlpDeviceAddr *get_slp_air_mouse_dongl_addr(void);

sle_addr_t *get_sle_air_mouse_server_addr(void);

void sle_client_slp_command_register_cbks(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif