/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse DIS Server Inc. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#ifndef SLE_MOUSE_DIS_SERVER_H
#define SLE_MOUSE_DIS_SERVER_H

#include "sle_ssap_server.h"
#include "service_common.h"
#include "dis_server.h"

/**
 * @if Eng
 * @brief  Initializing the SLE DIS Service Instance.
 * @param [in] server_id server id.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @else
 * @brief  SLE DIS服务实例初始化。
 * @param [in] server_id 服务ID。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @endif
 */
errcode_t sle_mouse_dis_server_init(uint8_t server_id);

#endif /* END of SLE_MOUSE_DIS_SERVER_H */
