/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse HID Server Inc. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#ifndef SLE_MOUSE_HID_SERVER_H
#define SLE_MOUSE_HID_SERVER_H

#include "sle_ssap_server.h"
#include "service_common.h"
#include "hid_server.h"

#define SLE_MOUSE_SSAP_RPT_HANDLE    2

typedef enum {
    SLE_MOUSE_SERVICE_HID_SERVER_REPORT_ID = 0x01,          /* report map中引用的report id */
} sle_mouse_service_report_id_t;

/**
 * @if Eng
 * @brief  SLE send mouse data.
 * @param [in] mouse_data mouse data
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @else
 * @brief  SLE发送鼠标数据。
 * @param [in] mouse_data 鼠标数据
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @endif
 */
errcode_t sle_hid_mouse_server_send_input_report(uint8_t server_id, uint16_t conn_id, uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  Initializing the SLE HID Service Instance.
 * @param [in] server_id server id.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @else
 * @brief  SLE HID服务实例初始化。
 * @param [in] server_id 服务ID。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @endif
 */
errcode_t sle_mouse_hid_server_init(uint8_t server_id);

/**
 * @if Eng
 * @brief  Get the input report attribute handle of the SLE HID service instance..
 * @attention  NULL
 * @retval input report handle.
 * @else
 * @brief  获取SLE HID服务实例输入报告属性句柄。
 * @attention  NULL
 * @retval 输入报告句柄。
 * @endif
 */
uint8_t sle_mouse_get_ssap_rpt_handle(void);

#endif /* END of SLE_MOUSE_HID_SERVER_H */
