/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE HID server. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-09-05, Create file. \n
 */

#ifndef BTH_GLE_SERVICE_SLE_HID_SERVER_H
#define BTH_GLE_SERVICE_SLE_HID_SERVER_H

/* HID服务属性索引定义，必选属性在前面，可选属性在后面 */
typedef enum {
    SLE_HID_PROPERTY_TYPE_REPORT_MAP = 0,   /* 必选属性: <<类型和格式描述>> */
    SLE_HID_PROPERTY_TYPE_INPUT_REPORT,     /* 必选属性: <<输入报告信息>> */
    SLE_HID_PROPERTY_TYPE_REPORT_REFERENCE, /* 必选属性: <<报告索引信息>> */
    SLE_HID_PROPERTY_TYPE_WORK_STATUS,      /* 可选属性: <<工作状态指示>> */
    SLE_HID_PROPERTY_TYPE_OUTPUT_REPORT,    /* 可选属性: <<输出报告信息>> */
    SLE_HID_PROPERTY_TYPE_FEATURE_REPORT,   /* 可选属性: <<特性报告信息>> */
    SLE_HID_PROPERTY_TYPE_MAX_NUM,
} sle_hid_server_property_type;

/* @brief SLE HID广播服务数据类型。 */
typedef enum {
    SLE_MOUSE_HID_SERVER_REPORT_TYPE_INPUT = 1, /* 输入报告 */
    SLE_MOUSE_HID_SERVER_REPORT_TYPE_OUTPUT,    /* 输出报告 */
    SLE_MOUSE_HID_SERVER_REPORT_TYPE_FEATURE,   /* 特性报告 */
    SLE_MOUSE_HID_SERVER_REPORT_TYPE_MAX        /* 预留 */
} sle_mouse_hid_service_report_data_type;

/* HID服务属性：工作状态指示 */
typedef enum {
    SLE_HID_SERVICE_HID_STATUS_NORMAL = 0,     /* 0x00: 指示当前人机工作状态为正常 */
    SLE_HID_SERVICE_HID_STATUS_PENDING,        /* 0x01: 指示当前人机工作状态为挂起 */
    SLE_HID_SERVICE_HID_STATUS_MAX             /* 0x02~: 预留 */
} sle_hid_server_work_status;

/* @brief hid服务端添加回调 */
typedef void (*sle_hid_server_start_service_callback)(uint8_t server_id, uint16_t service_handle, errcode_t status);

/* @brief 指示确认回调 */
typedef void (*sle_hid_server_indicate_cfm_callback)(uint8_t server_id, uint16_t conn_id,
    sle_indication_cfm_result_t cfm_result, errcode_t status);

/* @brief 远端读请求回调函数 */
typedef void (*sle_hid_server_read_request_callback)(uint8_t server_id, uint16_t conn_id,
    ssaps_req_read_cb_t *read_cb_para, errcode_t status);

/* @brief 远端写请求回调函数 */
typedef void (*sle_hid_server_write_request_callback)(uint8_t server_id, uint16_t conn_id,
    ssaps_req_write_cb_t *write_cb_para, errcode_t status);

/* @brief MTU变化回调函数 */
typedef void (*sle_hid_server_mtu_changed_callback)(uint8_t server_id, uint16_t conn_id,
    ssap_exchange_info_t *mtu_size, errcode_t status);

/* @brief hid server定义的实例化回调接口类型 */
typedef struct {
    sle_hid_server_start_service_callback start_service_cbk; /*!< @if Eng HID Server Service started callback.
                                                                   @else HID服务端启动服务回调函数。 @endif */
    sle_hid_server_indicate_cfm_callback  indicate_cfm_cbk;  /*!< @if Eng Indicate cfm callback.
                                                                  @else HID服务端指示确认回调函数。 @endif */
    sle_hid_server_read_request_callback  read_request_cbk;  /*!< @if Eng Read request received callback.
                                                                  @else HID服务端收到远端读请求回调函数。 @endif */
    sle_hid_server_write_request_callback write_request_cbk; /*!< @if Eng Write request received callback.
                                                                   @else HID服务端收到远端写请求回调函数。 @endif */
    sle_hid_server_mtu_changed_callback mtu_changed_cbk;     /* MTU变化回调 */
} sle_hid_server_service_cbk;

/* HID服务初始化单个实例参数模板 */
typedef struct {
    uint16_t service_uuid;                                                /* 服务唯一标识 */
    sle_service_property_info* properties[SLE_HID_PROPERTY_TYPE_MAX_NUM]; /* 属性列表 */
} sle_hid_server_service_template;

/**
 * @brief SLE HID服务实例报告索引属性数据。
 * @note sec_port和dst_port取值：
 * @note   当源、目的端口任意为0时，不支持报告信息在数据面交互
 * @note   当源、目的端口任意为0xFFFF时，报告信息在基础服务层透传
 */
typedef struct {
    uint8_t report_id;        /*!< @if Eng  HID server report ID.
                                   @else HID服务报告ID。@endif */
    uint8_t report_type;      /*!< @if Eng  HID server report type { @ref sle_mouse_hid_service_report_data_type }.
                                   @else HID服务报告类型 { @ref sle_mouse_hid_service_report_data_type }。@endif */
    uint16_t property_handle; /*!< @if Eng  HID server property handle.
                                   @else HID服务属性句柄。@endif */
    uint16_t src_port;        /*!< @if Eng  HID server data plane source port.
                                   @else HID服务数据面源端口。@endif */
    uint16_t dst_port;        /*!< @if Eng  HID server data plane destination  port.
                                   @else HID服务数据面目的端口。@endif */
} sle_hid_server_report_reference;

/* HID：获取服务实例所有句柄 */
typedef struct {
    uint8_t service_handle;
    uint8_t properties_handle[SLE_HID_PROPERTY_TYPE_MAX_NUM];
} sle_hid_server_all_handle;

/*************************************** 对外接口声明 ***************************************/
/* @brief 初始化hid服务 */
errcode_t sle_hid_server_instance_init(uint8_t server_id, sle_hid_server_service_cbk service_cbk);

/* @brief HID服务根据属性类型获取句柄 */
errcode_t sle_hid_server_get_handle_by_type(uint8_t server_id, uint8_t property_type, uint16_t *property_handle);

/* @brief 设置属性信息 */
errcode_t sle_hid_server_set_property_by_type(uint8_t property_type, sle_service_property_info *property_info);
#endif