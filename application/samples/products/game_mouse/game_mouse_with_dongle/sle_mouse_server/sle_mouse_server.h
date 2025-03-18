/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse Server Manager  Inc. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#ifndef SLE_MOUSE_SERVER_H
#define SLE_MOUSE_SERVER_H

#include "sle_ssap_server.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define DONGLE_SLEEP_LATENCY  200
#define SLE_MOUSE_SAMPLE_APP_UUID_LEN               2       /* 鼠标样例默认应用uuid长度 */

enum {
    SLE_MOUSE_RCV_HEAD = 0x05,
    SLE_MOUSE_RCV_TYPE = 0x5B,
};

typedef enum {
    SLE_MOUSE_GET_DPI_CMD = 0x01,
    SLE_MOUSE_SET_DPI_CMD = 0x00,
    SLE_MOUSE_GET_REPORT_CMD = 0x03,
    SLE_MOUSE_SET_REPORT_CMD = 0x02,
    SLE_MOUSE_GET_BATTERY_LAVEL = 0x04,
} app_mouse_customer_cmd_t;

typedef struct {
    uint16_t handle_in;
    uint16_t handle_out;
} sle_item_handle_t;

typedef struct {
    uint8_t     mouse_dpi;     /* 数据包内容:0-100,2:400,3:800 */
    uint8_t     mouse_key;     /* 数据包内容 */
    uint8_t     debounce_time; /* 数据包内容 */
    uint8_t     sensor_mode;   /* 数据包内容:125,1KHz,2KHz,4KHz,8Khz */
} app_mouse_package_t;

typedef struct {
    uint8_t     data_head;    /* 数据包头 */
    uint8_t     data_type;    /* 数据类型：01-Cmd，02-Req，other:保留待定 */
    uint16_t    data_length;  /* 数据长度 */
    app_mouse_package_t mouse_seting;
} app_mouse_ssap_t;

typedef enum {
    SLE_REMOTE_DEV_PC,     // 远端设备为PC或平板
    SLE_REMOTE_DEV_DONGLE, // 远端设备为Donlge
} sle_remote_device_type_t;

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
errcode_t app_sle_mouse_server_init(void);

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
errcode_t get_g_sle_mouse_server_conn_state(uint8_t *conn_state);

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
errcode_t get_g_sle_mouse_pair_state(uint32_t *pair_state);

errcode_t sle_mouse_send_notify_indicate(uint8_t *report, uint16_t len);
errcode_t sle_mouse_send_channel_notify_indicate(uint8_t *report, uint16_t len);

void close_sle_mode(void);
void restore_sle_mode_seting(void);
void sle_mouse_services_init(void);
uint8_t get_sle_mouse_connected(void);

uint16_t get_mouse_sle_conn_id(void);
uint16_t get_sle_remote_device_type(void);

sle_remote_device_type_t sle_mouse_get_device_type(void);
void sle_mouse_set_device_type(sle_remote_device_type_t device_type);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
