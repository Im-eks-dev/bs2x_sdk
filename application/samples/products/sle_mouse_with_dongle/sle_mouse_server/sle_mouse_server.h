/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: SLE mouse sample header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 * 2024-09-12, Modify file with profiles. \n
 */

#ifndef SLE_MOUSE_SERVER_H
#define SLE_MOUSE_SERVER_H

#include "sle_ssap_server.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief Mouse data of sle HID service instance.
 * @note The data format is same with the property data of <<Type & Format Description>>.
 * @else
 * @brief SLE HID服务实例鼠标数据。
 * @note  数据格式和<<类型和格式描述>>属性数据一致。
 * @endif
 */
#pragma pack(push, 1)
typedef struct {
    int32_t button : 8; /* 按键掩码 */
    int32_t x : 12;     /* mouse的x坐标 */
    int32_t y : 12;     /* mouse的y坐标 */
    int8_t wheel;       /* 滚轮 */
} sle_mouse_data_t;
#pragma pack(pop)

/**
 * @if Eng
 * @brief  SLE read connect status.
 * @param [in,out] conn_state   get sle connect state.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li sle_ssap_server.h
 * @else
 * @brief  SLE读取连接状态。
 * @param [in,out] conn_state   SLE连接状态。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li sle_ssap_server.h
 * @endif
 */
errcode_t get_g_sle_mouse_server_conn_state(uint8_t *conn_state);

/**
 * @if Eng
 * @brief  SLE read pair status.
 * @param [in,out] pair_state   get sle pair state.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li sle_ssap_server.h
 * @else
 * @brief  SLE读取配对状态。
 * @param [in,out] pair_state   SLE配对状态.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li sle_ssap_server.h
 * @endif
 */
errcode_t get_g_sle_mouse_pair_state(uint32_t *pair_state);

/**
 * @if Eng
 * @brief  SLE send mouse by hid server input report.
 * @param [in] report   data buffer point.
 * @param [in] len      data buffer len.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li sle_ssap_server.h
 * @else
 * @brief  通过HID server输入报告发送数据。
 * @param [in] report   数据指针。
 * @param [in] len      数据长度。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li sle_ssap_server.h
 * @endif
 */
errcode_t sle_mouse_send_notify_indicate(uint8_t *report, uint16_t len);

/**
 * @if Eng
 * @brief  Get client attribute notify state for input report write request.
 * @attention  NULL
 * @retval Client attribute notify state.
 * @else
 * @brief  获取输入报告cccd指示标志回写状态。
 * @attention  NULL
 * @retval cccd通知状态
 * @endif
 */
bool sle_hid_get_ssap_cccd_state(void);

/**
 * @if Eng
 * @brief  Set client attribute notify state for input report write request.
 * @param [in] state client attribute notify state.
 * @attention  NULL
 * @retval input report handle.
 * @else
 * @brief  设置输入报告cccd指示标志回写状态。
 * @param [in] state cccd通知状态.
 * @attention  无
 * @retval 无
 * @endif
 */
void sle_hid_set_ssap_cccd_state(bool state);

/* @brief sle mouse device callback register */
void sle_dev_cb_register(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif