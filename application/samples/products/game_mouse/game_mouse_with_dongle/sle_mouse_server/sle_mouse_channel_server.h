/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse CHANNEL Server Inc. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-10-23, Create file. \n
 */

#ifndef SLE_MOUSE_CHANNEL_SERVER_H
#define SLE_MOUSE_CHANNEL_SERVER_H

#include "sle_ssap_server.h"

#define CHANNEL_MAX_DATA_SIZE   10
/**
 * @if Eng
 * @brief  Initializing the SLE CHANNEL Service Instance.
 * @param [in] server_id server id.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @else
 * @brief  SLE CHANNEL服务实例初始化。
 * @param [in] server_id 服务ID。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @endif
 */
errcode_t sle_mouse_channel_server_init(uint8_t server_id);

/**
 * @if Eng
 * @brief  SLE CHANNEL Service Data Sending.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @else
 * @brief  SLE CHANNEL服务数据发送。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @endif
 */
errcode_t sle_mouse_channel_send_notification(uint8_t server_id, uint16_t conn_id, uint8_t *data, uint16_t len);

#endif /* END of SLE_MOUSE_CHANNEL_SERVER_H */
