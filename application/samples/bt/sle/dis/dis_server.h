/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE DIS server. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-09-05, Create file. \n
 */

#ifndef BTH_SLE_SERVICE_DIS_SERVER_H
#define BTH_SLE_SERVICE_DIS_SERVER_H

#include "service_common.h"

/* <<设备管理服务>>服务相关的通用唯一标识 */
typedef enum {
    SLE_DIS_SERVICE_UUID                              = 0x0609, /* <<设备管理服务>>服务唯一标识 */
    SLE_DIS_MANUFACTURE_INFO_UUID                     = 0x102E, /* <<厂商信息>>属性唯一标识 */
    SLE_DIS_DEVICE_MODEL_INFO_UUID                    = 0x102F, /* <<设备型号信息>>属性唯一标识 */
    SLE_DIS_DEVICE_SERIAL_NUMBER_UUID                 = 0x1030, /* <<设备序列号信息>>属性唯一标识 */
    SLE_DIS_HARDWARE_VERSION_INFO_UUID                = 0x1031, /* <<硬件版本信息>>属性唯一标识 */
    SLE_DIS_FIRMWARE_VERSION_INFO_UUID                = 0x1032, /* <<固件版本信息>>属性唯一标识 */
    SLE_DIS_SOFTWARE_VERSION_INFO_UUID                = 0x1033, /* <<软件版本信息>>属性唯一标识 */
    SLE_DIS_DEVICE_LOCAL_NAME_UUID                    = 0x103F, /* <<设备本地名称>>属性唯一标识 */
    SLE_DIS_DEVICE_APPEARANCE_INFO_UUID               = 0x1041, /* <<设备外观信息>>属性唯一标识 */
} sle_dis_uuid;

typedef enum {
    SLE_DIS_PROPERTY_MANUFACTURE_INFO = 0,     /* <<厂商信息>> */
    SLE_DIS_PROPERTY_DEVICE_MODEL_INFO,        /* <<设备型号信息>> */
    SLE_DIS_PROPERTY_DEVICE_SERIAL_NUMBER,     /* <<设备序列号信息>> */
    SLE_DIS_PROPERTY_HARDWARE_VERSION_INFO,    /* <<硬件版本信息>> */
    SLE_DIS_PROPERTY_FIRMWARE_VERSION_INFO,    /* <<固件版本信息>> */
    SLE_DIS_PROPERTY_SOFTWARE_VERSION_INFO,    /* <<软件版本信息>> */
    SLE_DIS_PROPERTY_DEVICE_LOCAL_NAME,        /* <<设备本地名称>> */
    SLE_DIS_PROPERTY_DEVICE_APPEARANCE_INFO,   /* <<设备外观信息>> */
    SLE_DIS_PROPERTY_MAX_NUM,
} sle_dis_property_type;

/* DIS服务：设备公开信息消息类型（T） */
typedef enum {
    SLE_DIS_SERVICE_ADV_TYPE_MANUFACTURE = 0x00,        /* 0x00: 广播数据类型--厂商信息 */
    SLE_DIS_SERVICE_ADV_TYPE_DEVICE_MODEL,              /* 0x01: 广播数据类型--设备型号信息 */
    SLE_DIS_SERVICE_ADV_TYPE_DEVICE_SERIAL_NUMBER,      /* 0x02: 广播数据类型--设备序列号信息 */
    SLE_DIS_SERVICE_ADV_TYPE_HARDWARE_VERSION,          /* 0x03: 广播数据类型--硬件版本信息 */
    SLE_DIS_SERVICE_ADV_TYPE_FIRMWARE_VERSION,          /* 0x04: 广播数据类型--固件版本信息 */
    SLE_DIS_SERVICE_ADV_TYPE_SOFTWARE_VERSION,          /* 0x05: 广播数据类型--软件版本信息 */
    SLE_DIS_SERVICE_ADV_TYPE_DEVICE_LOCAL_NAME,         /* 0x06: 广播数据类型--设备本地名称 */
    SLE_DIS_SERVICE_ADV_TYPE_DEVICE_APPEARANCE = 0x07,  /* 0x07: 广播数据类型--设备外观信息 */
} sle_dis_adv_msg_type;

/* @brief dis服务端添加回调 */
typedef void (*sle_dis_server_start_service_callback)(uint8_t server_id, uint16_t handle, errcode_t status);

/* @brief 指示确认回调 */
typedef void (*sle_dis_server_indicate_cfm_callback)(uint8_t server_id, uint16_t conn_id,
    sle_indication_cfm_result_t cfm_result, errcode_t status);

/* @brief 远端读请求回调函数 */
typedef void (*sle_dis_server_read_request_callback)(uint8_t server_id, uint16_t conn_id,
    ssaps_req_read_cb_t *read_cb_para, errcode_t status);

/* @brief 远端写请求回调函数 */
typedef void (*sle_dis_server_write_request_callback)(uint8_t server_id, uint16_t conn_id,
    ssaps_req_write_cb_t *write_cb_para, errcode_t status);

/* @brief MTU变化回调函数 */
typedef void (*sle_dis_server_mtu_changed_callback)(uint8_t server_id, uint16_t conn_id,
    ssap_exchange_info_t *mtu_size, errcode_t status);

typedef struct {
    sle_dis_server_start_service_callback start_service_cbk; /*!< @if Eng dis Server Service started callback.
                                                                   @else dis服务端启动服务回调函数。 @endif */
    sle_dis_server_indicate_cfm_callback indicate_cfm_cbk;   /*!< @if Eng Indicate cfm callback.
                                                                  @else dis服务端指示确认回调函数。 @endif */
    sle_dis_server_read_request_callback read_request_cbk;   /*!< @if Eng Read request received callback.
                                                                  @else dis服务端收到远端读请求回调函数。 @endif */
    sle_dis_server_write_request_callback write_request_cbk; /*!< @if Eng Write request received callback.
                                                                   @else dis服务端收到远端写请求回调函数。 @endif */
    sle_dis_server_mtu_changed_callback mtu_changed_cbk;     /*!< @if Eng mtu changed received callback.
                                                                  @else bas服务端收到mtu变化回调函数。 @endif */
} sle_dis_server_cbk;

typedef struct {
    struct osal_list_head node;
    uint8_t server_id;                                              /* 服务ID */
    uint16_t service_uuid;                                          /* DIS服务UUID */
    uint16_t service_handle;                                        /* DIS服务句柄 */
    sle_dis_server_cbk dis_server_cbk;                              /* DIS服务端对外回调接口 */
    sle_service_property_info properties[SLE_DIS_PROPERTY_MAX_NUM]; /* DIS服务依赖的属性列表 */
} sle_dis_service_instance;

/* dis服务初始化单个实例参数模板 */
typedef struct {
    uint16_t service_uuid;                                             /* 服务唯一标识 */
    sle_service_property_info *properties[SLE_DIS_PROPERTY_MAX_NUM];   /* 服务依赖属性列表 */
} sle_dis_service_template;

/* 客户端属性配置描述符 */
typedef struct {
    uint8_t notify : 1;  /* bit0 : 如果设置为1，服务端将属性值通过通知发送给客户端。只有属性值访问控制允许通知时才能生效 */
    uint8_t entry : 1;   /* bit1 : 如果设置为1，服务端将属性值通过指示发送给客户端。只有属性值访问控制允许指示时才能生效 */
    uint8_t rsv : 6;     /* bit2-bit15: 保留 */
    uint8_t rsv_byte;
} sle_dis_descriptor_client_att;

/* L3层DIS服务对外数据发送接口 */
typedef struct {
    uint8_t property_type;     /* 属性类型 { @ref ssap_property_type_t } */
    uint16_t data_len;         /* 数据长度 */
    uint16_t property_handle;  /* 属性句柄 */
    uint8_t* data;             /* 数据指针 */
} dis_server_ntf_ind_param;

// 设备信息--数据内容
typedef struct {
    uint16_t service_uuid;                  /* 字节0～1：<<设备信息>>服务的服务标识（UUID） */
} sle_dis_adv_type;

/*************************************** 对外接口声明 ***************************************/

/* @brief 设置DIS服务属性数据 */
errcode_t sle_dis_server_set_property_by_type(uint8_t property_type, sle_service_property_info *property_info);

/* @brief 初始化DIS服务 */
errcode_t sle_dis_server_instance_init(uint8_t server_id, sle_dis_server_cbk service_cbk);

/* DIS服务端广播数据编码 */
errcode_t sle_dis_server_trans_data(uint8_t server_id, uint16_t conn_id, dis_server_ntf_ind_param* data_block);

#endif