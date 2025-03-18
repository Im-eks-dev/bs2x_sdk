/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE BAS server. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-09-05, Create file. \n
 */

#ifndef BTH_SLE_SERVICE_BAS_SERVER_H
#define BTH_SLE_SERVICE_BAS_SERVER_H

#include "sle_ssap_server.h"
#include "service_common.h"

/* <<电量管理服务>>服务相关的通用唯一标识 */
typedef enum {
    SLE_BAS_SERVICE_UUID                        = 0x060A, /* <<电量管理服务>>服务唯一标识 */
    SLE_BAS_REMAIN_BATTERY_CAPACITY_RATIO_UUID  = 0x1034, /* <<电池剩余容量占比信息>>属性唯一标识 */
    SLE_BAS_REMAIN_BATTERY_CAPACITY_INFO_UUID   = 0x1035, /* <<电池剩余容量信息>>属性唯一标识 */
    SLE_BAS_TOTAL_BATTERY_CAPACITY_UUID         = 0x1036, /* <<电池工作总容量信息>>属性唯一标识 */
    SLE_BAS_TOTAL_RATED_BATTERY_CAPACITY_UUID   = 0x1037, /* <<电池额定总容量信息>>属性唯一标识 */
    SLE_BAS_REMAIN_WORK_TIME_UUID               = 0x1038, /* <<设备剩余工作时长信息>>属性唯一标识 */
} sle_bas_uuid;

typedef enum {
    SLE_BAS_PROPERTY_REMAIN_BATTERY_CAPACITY_RATIO = 0, /* <<电池剩余容量占比信息>> 单位百分比 */
    SLE_BAS_PROPERTY_REMAIN_BATTERY_CAPACITY_INFO,      /* <<电池剩余容量信息>> 单位mAh */
    SLE_BAS_PROPERTY_TOTAL_BATTERY_CAPACITY,            /* <<电池工作总容量信息>> 单位mAh */
    SLE_BAS_PROPERTY_TOTAL_RATED_BATTERY_CAPACITY,      /* <<电池额定总容量信息>> 单位mAh */
    SLE_BAS_PROPERTY_REMAIN_WORK_TIME,                  /* <<设备剩余工作时长信息>> 单位秒 */
    SLE_BAS_PROPERTY_MAX_NUM,
} sle_bas_property_type;

/* BAS服务：设备公开信息消息类型（T） */
typedef enum {
    SLE_BAS_SERVICE_ADV_TYPE_REMAIN_BATTERY_CAPACITY_RATIO = 0x00,       /* 0x00: 电池剩余容量占比信息 */
    SLE_BAS_SERVICE_ADV_TYPE_REMAIN_BATTERY_CAPACITY_INFO,               /* 0x01: 电池剩余容量信息 */
    SLE_BAS_SERVICE_ADV_TYPE_TOTAL_BATTERY_CAPACITY,                     /* 0x02: 电池剩余容量信息 */
    SLE_BAS_SERVICE_ADV_TYPE_TOTAL_RATED_BATTERY_CAPACITY,               /* 0x03: 电池额定总容量信息 */
    SLE_BAS_SERVICE_ADV_TYPE_REMAIN_WORK_TIME,                           /* 0x04: 设备剩余工作时长信息 */
    SLE_BAS_SERVICE_ADV_TYPE_REMAIN_BATTERY_CAPACITY_RATIO_DESC = 0x05,  /* 0x05: 电池剩余容量占比信息属性说明描述符 */
} sle_bas_adv_msg_type;

/* bas服务初始化单个实例参数模板 */
typedef struct {
    uint16_t service_uuid;                                             /* 服务唯一标识 */
    sle_service_property_info *properties[SLE_BAS_PROPERTY_MAX_NUM];   /* 服务依赖属性列表 */
} sle_bas_service_template;

/* 客户端属性配置描述符 */
typedef struct {
    uint8_t notify : 1;  /* bit0 : 如果设置为1，服务端将属性值通过通知发送给客户端。只有属性值访问控制允许通知时才能生效 */
    uint8_t entry : 1;   /* bit1 : 如果设置为1，服务端将属性值通过指示发送给客户端。只有属性值访问控制允许指示时才能生效 */
    uint8_t rsv : 6;     /* bit2-bit15: 保留 */
    uint8_t rsv_byte;
} sle_bas_descriptor_client_att;

/* L3层BAS服务对外数据发送接口 */
typedef struct {
    uint8_t property_type;     /* 属性类型 { @ref ssap_property_type_t } */
    uint16_t data_len;         /* 数据长度 */
    uint16_t property_index;  /* 属性句柄 */
    uint8_t* data;             /* 数据指针 */
} bas_server_ntf_ind_param;

// 设备信息--数据内容
typedef struct {
    uint16_t service_uuid;                  /* 字节0～1：<<设备信息>>服务的服务标识（UUID） */
} sle_bas_adv_type;

/* @brief bas服务端添加回调 */
typedef void (*sle_bas_server_start_service_callback)(uint8_t server_id, uint16_t handle, errcode_t status);

/* @brief 指示确认回调 */
typedef void (*sle_bas_server_indicate_cfm_callback)(uint8_t server_id, uint16_t conn_id,
    sle_indication_cfm_result_t cfm_result, errcode_t status);

/* @brief 远端读请求回调函数 */
typedef void (*sle_bas_server_read_request_callback)(uint8_t server_id, uint16_t conn_id,
    ssaps_req_read_cb_t *read_cb_para, errcode_t status);

/* @brief 远端写请求回调函数 */
typedef void (*sle_bas_server_write_request_callback)(uint8_t server_id, uint16_t conn_id,
    ssaps_req_write_cb_t *write_cb_para, errcode_t status);

/* @brief MTU变化回调函数 */
typedef void (*sle_bas_server_mtu_changed_callback)(uint8_t server_id, uint16_t conn_id,
    ssap_exchange_info_t *mtu_size, errcode_t status);

typedef struct {
    sle_bas_server_start_service_callback start_service_cbk; /*!< @if Eng bas Server Service started callback.
                                                                   @else bas服务端启动服务回调函数。 @endif */
    sle_bas_server_indicate_cfm_callback indicate_cfm_cbk;   /*!< @if Eng Indicate cfm callback.
                                                                  @else bas服务端指示确认回调函数。 @endif */
    sle_bas_server_read_request_callback read_request_cbk;   /*!< @if Eng Read request received callback.
                                                                  @else bas服务端收到远端读请求回调函数。 @endif */
    sle_bas_server_write_request_callback write_request_cbk; /*!< @if Eng Write request received callback.
                                                                  @else bas服务端收到远端写请求回调函数。 @endif */
    sle_bas_server_mtu_changed_callback mtu_changed_cbk;     /*!< @if Eng mtu changed received callback.
                                                                  @else bas服务端收到mtu变化回调函数。 @endif */
} sle_bas_server_cbk;

typedef struct {
    struct osal_list_head node;
    uint8_t server_id;                                              /* 服务ID */
    uint16_t service_uuid;                                          /* BAS服务UUID */
    uint16_t service_handle;                                        /* BAS服务句柄 */
    sle_bas_server_cbk bas_server_cbk;                              /* BAS服务端对外回调接口 */
    sle_service_property_info properties[SLE_BAS_PROPERTY_MAX_NUM]; /* BAS服务依赖的属性列表 */
} sle_bas_server_service_instance;

/* @brief 初始化BAS服务 */
errcode_t sle_bas_server_instance_init(uint8_t server_id, sle_bas_server_cbk service_cbk);

/* BAS服务端发送数据 */
errcode_t sle_bas_server_trans_data(uint8_t server_id, uint16_t conn_id, bas_server_ntf_ind_param* data_block);

/* BAS设置属性数据 */
errcode_t sle_bas_server_set_property_by_type(uint8_t property_type, sle_service_property_info *property_info);

#endif