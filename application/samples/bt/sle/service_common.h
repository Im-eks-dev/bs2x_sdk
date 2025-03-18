/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE service common. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-09-05, Create file. \n
 */

#ifndef BTH_GLE_SERVICE_SERVICE_COMMON_H
#define BTH_GLE_SERVICE_SERVICE_COMMON_H

#define SLE_STACK_UUID_LEN                          16      /* SLE协议栈默认UUID长度 */
#define SLE_STACK_STANDARD_SERVICE_UUID_LEN         2       /* SLE协议栈标准服务uuid长度 */

/* 属性注册及数据管理相关结构体和接口 */

/* 服务是否支持当前属性 */
typedef enum {
    SLE_SERVICE_PROPERTY_NOT_SUPPORT = 0,  /* 0: 不支持该属性 */
    SLE_SERVICE_PROPERTY_SUPPORT,          /* 1: 支持该属性 */
} sle_property_is_support_t;

/* 服务属性是否支持当前属性描述符 */
typedef enum {
    SLE_SERVICE_DESCRIPTOR_NOT_SUPPORT = 0,  /* 0: 不支持该属性描述符 */
    SLE_SERVICE_DESCRIPTOR_SUPPORT,          /* 1: 支持该属性描述符 */
} sle_descriptor_is_support_t;

typedef enum {
    SLE_SERVICE_PROPERTY_DESCRIPTOR_ATTRIBUTE = 0,   /* 属性说明描述符 */
    SLE_SERVICE_PROPERTY_DESCRIPTOR_CLIENT_ATT,      /* 客户端属性值配置描述符 */
    SLE_SERVICE_PROPERTY_DESCRIPTOR_SERVER_ATT,      /* 服务端属性值配置描述符 */
    SLE_SERVICE_PROPERTY_DESCRIPTOR_ATT_FORMAT,      /* 属性值格式描述符 */
    SLE_SERVICE_PROPERTY_DESCRIPTOR_MAX_NUM,
} sle_service_property_descriptor_type;

/* 属性描述符数据内容 */
typedef struct {
    uint8_t is_support;         /* 属性描述符是否支持 */
    uint8_t desp_type;          /* 描述符类型值，@ref {ssap_property_type_t} */
    uint8_t data_len;           /* 描述符数据长度 */
    uint8_t *data;              /* 描述符数据 */
} sle_service_descriptor_data;

/* 服务管理属性描述符的访问控制和权限控制(Access Control List) */
typedef struct {
    uint32_t operate_indication;   /* 访问控制，@ref {ssap_operate_indication_t} */
    uint16_t permission;           /* 权限控制，@ref {ssap_permission_t} */
} sle_service_access_ctrl;

#define SLE_STANDARD_SERVICE_ADV_DATA_TYPE      0x03    /* 标准服务在广播数据的数据标识 */

/* 服务属性本地存储数据结构体 */
typedef struct {
    uint16_t property_uuid;                   /* 服务属性UUID唯一标识（基于Base UUID） */
    uint16_t property_handle;                 /* 服务属性注册后分配的句柄 */
    uint8_t is_support;                       /* 当前实例是否支持该属性 */
    uint8_t data_len;                         /* 属性数据长度 */
    uint8_t *property_data;                   /* 属性数据 */
    sle_service_access_ctrl access_ctrl;     /* 服务属性的访问、权限控制列表 */
    sle_service_descriptor_data desp_data[SLE_SERVICE_PROPERTY_DESCRIPTOR_MAX_NUM]; /* 服务管理描述符数据矩阵 */
    sle_service_access_ctrl desp_access_ctrl[SLE_SERVICE_PROPERTY_DESCRIPTOR_MAX_NUM];      /* 服务管理描述符访问控制、权限控制矩阵 */
} sle_service_property_info;

/**
 * @if Eng
 * @brief SLE HID service instance client attribute descriptor data.
 * @else
 * @brief SLE HID服务实例客户端属性描述符数据。
 * @endif
 */
typedef struct {
    uint16_t notify : 1;   /* bit0 : 如果设置为1，服务端将属性值通过通知发送给客户端。只有属性值访问控制允许通知时才能生效 */
    uint16_t indicate : 1; /* bit1 : 如果设置为1，服务端将属性值通过指示发送给客户端。只有属性值访问控制允许指示时才能生效 */
    uint16_t rsv : 14;     /* bit2-bit15: 保留 */
} sle_service_descriptor_client_att;

/**
 * @if Eng
 * @brief SLE HID service instance client attribute descriptor data.
 * @else
 * @brief SLE HID服务实例服务端属性描述符数据。
 * @endif
 */
typedef struct {
    uint16_t broadcast : 1; /* bit0 : 如果设置为1，服务端将属性值通过广播方式发送给客户端。只有属性值访问控制允许广播时才能生效 */
    uint16_t no_def : 1;    /* bit1 : 未定义，协议未使用、未描述 */
    uint16_t rsv : 14;      /* bit2-bit15: 保留 */
} sle_service_descriptor_server_att;

/* 各服务广播数据编码：设备公开信息数据结构（TLV） */
typedef struct {
    uint8_t data_type;
    uint8_t data_len;
    uint8_t data[0];
} sle_service_adv_encode_data;

/* 服务广播数据编码时依赖的属性配置信息 */
typedef struct {
    uint8_t service_data_type;           /* 各服务广播数据中，服务数据内容部分对应的类型 */
    sle_service_property_info* property; /* 服务属性数据，各服务需要在广播段发送的属性信息 */
} sle_service_adv_encode_property;

/* 服务广播数据编码输入参数 */
typedef struct {
    uint8_t *mandatory_field;            /* 各服务的必选字段，即：必须携带的数据内容 */
    uint8_t mandatory_len;               /* 各服务必选字段的长度，例如：HID服务最少3字节 */
    uint8_t property_num;                /* 各服务需要编码到广播里的属性数量 */
    sle_service_adv_encode_property *properties; /* 各服务待编码到广播的属性信息 */
} sle_service_adv_encode;

/**
 * @if Eng
 * @brief SLE service instance client attribute descriptor data.
 * @else
 * @brief SLE 服务实例客户端属性描述符数据。
 * @endif
 */
typedef struct {
    uint16_t notify : 1;   /* bit0 : 如果设置为1，服务端将属性值通过通知发送给客户端。只有属性值访问控制允许通知时才能生效 */
    uint16_t indicate : 1; /* bit1 : 如果设置为1，服务端将属性值通过指示发送给客户端。只有属性值访问控制允许指示时才能生效 */
    uint16_t rsv : 14;     /* bit2-bit15: 保留 */
} sle_descriptor_client_att;

/**
 * @if Eng
 * @brief SLE service instance client attribute descriptor data.
 * @else
 * @brief SLE 服务实例服务端属性描述符数据。
 * @endif
 */
typedef struct {
    uint16_t broadcast : 1; /* bit0 : 如果设置为1，服务端将属性值通过广播方式发送给客户端。只有属性值访问控制允许广播时才能生效 */
    uint16_t no_def : 1;    /* bit1 : 未定义，协议未使用、未描述 */
    uint16_t rsv : 14;      /* bit2-bit15: 保留 */
} sle_descriptor_server_att;

/****************************
 * @brief 添加属性过程中需要进行的数据操作
 * @example 例如：HID server在添加完report map之后，
 *          需要在report reference数据中携带report map的handler
 * @param properties [in]   待添加的服务属性列表
 * @param property_idx [in] 当前正在处理的服务属性索引
 * @note 调用时机：添加指定服务属性之前
 */
typedef void(sle_service_add_property_cbk)(sle_service_property_info *properties, uint16_t property_idx);

static inline void sle_service_set_uuid(sle_uuid_t *uuid_buf, uint16_t service_uuid)
{
    uint8_t uuid_offset = SLE_STACK_UUID_LEN - SLE_STACK_STANDARD_SERVICE_UUID_LEN;
    uint16_t *uuid_end = (uint16_t *)(uuid_buf->uuid + uuid_offset); /* 只修改最后两个字节 */
    *uuid_end = service_uuid;
}

/* 服务属性注册对外接口 */
/* 向服务添加属性 */
errcode_t sle_ssaps_service_add_property(uint8_t server_id, uint16_t service_handle,
    sle_service_property_info *properties, uint16_t property_cnt, sle_service_add_property_cbk data_proc);

/* 向属性添加属性描述符 */
errcode_t sle_ssaps_service_add_descriptor(uint16_t server_id, uint16_t service_handle,
    sle_service_property_info *property);

/* 编码各服务广播数据接口 */
uint8_t sle_service_encode_adv_data(sle_service_adv_encode *adv_data, uint8_t *out_data, uint8_t left_len);

/* 申请属性数据 */
errcode_t sle_service_alloc_property_data(sle_service_property_info *src_info,
    sle_service_property_info **out_property);

/* 释放属性数据 */
void sle_service_free_property_data(sle_service_property_info** properties, uint8_t property_cnt);

#endif