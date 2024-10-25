/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Air Mouse with dongle server Header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-11-16, Create file. \n
 */

#ifndef SLE_AIR_MOUSE_SERVER_H
#define SLE_AIR_MOUSE_SERVER_H

#include "sle_ssap_server.h"
#include "../../dongle/air_mouse_usb/usb_init_app.h"
#include "slp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    uint16_t handle_in;
    uint16_t handle_out;
} sle_item_handle_t;

typedef enum {
    HANDLE_IN,
    HANDLE_OUT,
    HANDLE_INVALID,
} sle_handle_t;

typedef struct {
    int32_t button_mask : 8; /* 按键掩码 */
    int32_t x : 12; /* mouse的x坐标 */
    int32_t y : 12; /* mouse的y坐标 */
    int8_t wheel; /* 滚轮 */
} ssap_mouse_key_t;
/**
 * @if Eng
 * @brief  SLE mouse server inir.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li sle_ssap_server.h
 * @else
 * @brief  SLE MOUSE服务器初始化。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li sle_ssap_server.h
 * @endif
 */
errcode_t sle_air_mouse_server_init(void);

/**
 * @if Eng
 * @brief  SLE read ssap passage support.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li sle_ssap_server.h
 * @else
 * @brief  SLE MOUSE读取ssap通道能力。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li sle_ssap_server.h
 * @endif
 */
errcode_t get_g_read_ssap_support(bool *param);

/**
 * @if Eng
 * @brief  SLE read connect status.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li sle_ssap_server.h
 * @else
 * @brief  SLE读取连接状态。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li sle_ssap_server.h
 * @endif
 */
errcode_t get_g_sle_air_mouse_server_conn_state(uint8_t *conn_state);

/**
 * @if Eng
 * @brief  SLE read pair status.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li sle_ssap_server.h
 * @else
 * @brief  SLE读取连接状态。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li sle_ssap_server.h
 * @endif
 */
errcode_t get_g_sle_air_mouse_pair_state(uint32_t *pair_state);

/**
 * @if Eng
 * @brief  SLE send mouse key data.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li sle_ssap_server.h
 * @else
 * @brief  SLE发送键盘HID报告数据
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li sle_ssap_server.h
 * @endif
 */
errcode_t sle_hid_mouse_server_send_mouse_key_report(uint8_t key, uint8_t left_key);

/**
 * @if Eng
 * @brief  SLE send keyboard data.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li sle_ssap_server.h
 * @else
 * @brief  SLE发送键盘HID报告数据
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li sle_ssap_server.h
 * @endif
 */
errcode_t sle_hid_mouse_server_send_keyboard_report(uint8_t key, uint8_t value);

const SlpDeviceAddr *get_slp_air_mouse_addr(void);
void imu_wakeup_callback(uint8_t ulp_gpio);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif