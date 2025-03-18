/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse HID Server Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#include "securec.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "errcode.h"

#include "common_def.h"
#include "sle_errcode.h"
#include "sle_common.h"

#include "sle_ssap_stru.h"
#include "sle_ssap_server.h"
#include "sle_mouse_server.h"
#include "sle_mouse_hid_server.h"

#define REPORT_RATE_CHANGE_ACK_LEN   7
#define DONGLE_ACK_VALUE_INDEX       5

/************************** HID Server 数据适配 **************************/
/* HID server 属性数据定义 */
static uint8_t g_hid_mouse_report_map[] = {     /* 类型和格式描述 属性数据定义 */
    0x00,                       /* type indicate */
    0x05, 0x01,                 /* Usage Page (Generic Desktop)             */
    0x09, 0x02,                 /* Usage (Mouse)                            */
    0xA1, 0x01,                 /* Collection (Application)                 */
    0x09, 0x01,                 /*  Usage (Pointer)                         */
    0xA1, 0x00,                 /*  Collection (Physical)                   */
    0x85, 0x01,                 /*   Report ID  */
    0x05, 0x09,                 /*      Usage Page (Buttons)                */
    0x19, 0x01,                 /*      Usage Minimum (01)                  */
    0x29, 0x03,                 /*      Usage Maximum (03)                  */
    0x15, 0x00,                 /*      Logical Minimum (0)                 */
    0x25, 0x01,                 /*      Logical Maximum (1)                 */
    0x95, 0x03,                 /*      Report Count (3)                    */
    0x75, 0x01,                 /*      Report Size (1)                     */
    0x81, 0x02,                 /*      Input (Data, Variable, Absolute)    */
    0x95, 0x01,                 /*      Report Count (1)                    */
    0x75, 0x05,                 /*      Report Size (5)                     */
    0x81, 0x01,                 /*      Input (Constant)    ;5 bit padding  */
    0x05, 0x01,                 /*      Usage Page (Generic Desktop)        */
    0x09, 0x30,                 /*      Usage (X)                           */
    0x09, 0x31,                 /*      Usage (Y)                           */
    0x16, 0x01, 0xF8,           /*      Logical Minimum (-2047)              */
    0x26, 0xFF, 0x07,           /*      Logical Maximum (2047)               */
    0x75, 0x0C,                 /*      Report Size (12)                     */
    0x95, 0x02,                 /*      Report Count (2)                    */
    0x81, 0x06,                 /*      Input (Data, Variable, Relative)    */
    0x05, 0x01,                 /*      Usage Page (Generic Desktop)        */
    0x09, 0x38,                 /*      Usage (Wheel)                       */
    0x15, 0x81,                 /*      Logical Minimum (-127)              */
    0x25, 0x7F,                 /*      Logical Maximum (127)               */
    0x75, 0x08,                 /*      Report Size (8)                     */
    0x95, 0x01,                 /*      Report Count (1)                    */
    0x81, 0x06,                 /*      Input (Data, Variable, Relative)    */
    0xC0,                       /* End Collection,End Collection            */
    0xC0,                       /* End Collection,End Collection            */
};

/* 服务句柄，本文件内可见 */
static uint16_t g_sle_mouse_rpt_handle = 0;

/************************** HID Server 功能模块适配 Start **************************/
/* HID服务依赖的应用层回调，service_handle在hid多实例中区分 */
void sle_hid_server_start_service_cbk(uint8_t server_id, uint16_t service_handle, errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("hid start service cbk,server_id:%u,service handle:%u,status:%u\n",
        server_id, service_handle, status);
#else
    unused(server_id);
    unused(service_handle);
    unused(status);
#endif
}

/* HID服务应用层指示确认回调，server id在应用层多服务回调中区分 */
void sle_hid_server_indicate_cfm_cbk(uint8_t server_id, uint16_t conn_id,
    sle_indication_cfm_result_t cfm_result, errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("hid indicate cfm cbk,server_id:%u,conn id:%u,indicate res:%u,status:%u\n",
        server_id, conn_id, cfm_result, status);
#else
    unused(cfm_result);
    unused(server_id);
    unused(conn_id);
    unused(status);
#endif
}

void sle_hid_server_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
    errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("hid read req,server id:%u,conn id:0x%x,status:%u,porperty handlke:0x%04x\r\n",
        server_id, conn_id, status, read_cb_para->handle);
#else
    unused(read_cb_para);
    unused(server_id);
    unused(conn_id);
    unused(status);
#endif
}

void sle_hid_server_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
    errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("hid write req,server id:%u,conn id:%u,status:%u,property handle:0x%04x\n",
        server_id, conn_id, status, write_cb_para->handle);
#else
    unused(write_cb_para);
    unused(server_id);
    unused(conn_id);
    unused(status);
#endif
}

static void sle_hid_server_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id,
    ssap_exchange_info_t *mtu_size, errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("hid write request callback,server id:%u,conn id:%u,mtu:%u,status:%u\n",
        server_id, conn_id, mtu_size->mtu_size, status);
#else
    unused(mtu_size);
    unused(server_id);
    unused(conn_id);
    unused(status);
#endif
}

/* 设置必选属性数据 */
static errcode_t sle_mouse_set_mandatory_properties(void)
{
    /* <<类型和格式描述>> */
    sle_service_property_info property_info = {
        .property_data = g_hid_mouse_report_map,
        .data_len = sizeof(g_hid_mouse_report_map),
    };

    errcode_t init_ret = sle_hid_server_set_property_by_type(SLE_HID_PROPERTY_TYPE_REPORT_MAP, &property_info);
    if (init_ret != ERRCODE_SLE_SUCCESS) {
        return init_ret;
    }

    /* <<报告索引信息>> */
    sle_hid_server_report_reference hid_report_ref = {
        .report_id = SLE_MOUSE_SERVICE_HID_SERVER_REPORT_ID,
        .report_type = SLE_MOUSE_HID_SERVER_REPORT_TYPE_INPUT,
        .property_handle = 0, /* 关联report map分配的handle，由Profile完成填写 */
    };

    property_info.property_data = (uint8_t *)&hid_report_ref;
    property_info.data_len = sizeof(hid_report_ref);

    init_ret = sle_hid_server_set_property_by_type(SLE_HID_PROPERTY_TYPE_REPORT_REFERENCE, &property_info);
    if (init_ret != ERRCODE_SLE_SUCCESS) {
        return init_ret;
    }

    /* <<输入报告信息>>，由于需要设置客户端属性信息，放到最后 */
    sle_descriptor_client_att hid_client_desp = {
        .notify = 1,    /* 兼容处理，默认连接后不需要写cccd值即可发数据 */
    };
    sle_mouse_data_t hid_input_data = { 0 }; /* report map对应数据格式 */
    property_info.data_len = sizeof(hid_input_data);
    property_info.property_data = (uint8_t *)&hid_input_data;
    property_info.desp_data[SLE_SERVICE_PROPERTY_DESCRIPTOR_CLIENT_ATT].data = (uint8_t*)&hid_client_desp;
    property_info.desp_data[SLE_SERVICE_PROPERTY_DESCRIPTOR_CLIENT_ATT].data_len = sizeof(hid_client_desp);

    init_ret = sle_hid_server_set_property_by_type(SLE_HID_PROPERTY_TYPE_INPUT_REPORT, &property_info);
    if (init_ret != ERRCODE_SLE_SUCCESS) {
        return init_ret;
    }

    return ERRCODE_SLE_SUCCESS;
}

/* 设置可选属性数据，可选属性需要配置权限，必选免配置权限 */
static errcode_t sle_mouse_set_optional_properties(void)
{
    uint8_t hid_work_status = SLE_HID_SERVICE_HID_STATUS_PENDING;
    sle_descriptor_client_att client_desp = {
        .notify = 1,    /* 兼容处理，默认连接后不需要写cccd值即可发数据 */
    };

    sle_service_property_info work_status = {
        .is_support = SLE_SERVICE_PROPERTY_SUPPORT,
        .property_data = &hid_work_status,
        .data_len = 2, /* 写入单个属性值长度为2 */
        .access_ctrl = {
            .operate_indication =  SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP |
                SSAP_OPERATE_INDICATION_BIT_WRITE,
            .permission = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE,
        },
        .desp_data = {
            [SLE_SERVICE_PROPERTY_DESCRIPTOR_CLIENT_ATT] = {
                .is_support = SLE_SERVICE_DESCRIPTOR_SUPPORT,
                .desp_type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION,
                .data = (uint8_t *)&client_desp,
                .data_len = sizeof(client_desp),
            },
        },
        .desp_access_ctrl = {
            [SLE_SERVICE_PROPERTY_DESCRIPTOR_CLIENT_ATT] = {
                .operate_indication = SSAP_OPERATE_INDICATION_BIT_READ |
                    SSAP_OPERATE_INDICATION_BIT_DESCRIPTOR_CLIENT_CONFIGURATION_WRITE,
                .permission = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE,
            }
        },
    };

    errcode_t init_ret = sle_hid_server_set_property_by_type(SLE_HID_PROPERTY_TYPE_WORK_STATUS, &work_status);
    if (init_ret != ERRCODE_SLE_SUCCESS) {
        return init_ret;
    }

    return ERRCODE_SLE_SUCCESS;
}

/**********************************************
 * @brief 初始化HID server服务实例
 * @note 依赖全局数据配置 g_mouse_hid_server_template
 **********************************************/
errcode_t sle_mouse_hid_server_init(uint8_t server_id)
{
    /* 初始化 HID server 鼠标服务实例 */
    sle_hid_server_service_cbk mouse_hid_cbk = {
        .start_service_cbk = sle_hid_server_start_service_cbk,
        .indicate_cfm_cbk = sle_hid_server_indicate_cfm_cbk,
        .read_request_cbk = sle_hid_server_read_request_cbk,
        .write_request_cbk = sle_hid_server_write_request_cbk,
        .mtu_changed_cbk = sle_hid_server_mtu_changed_cbk,
    };

    /* 设置必选属性数据 */
    errcode_t init_ret = sle_mouse_set_mandatory_properties();
    if (init_ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("Sle init hid,set mandatory properties fail:%u\n", init_ret);
        return init_ret;
    }

    /* 设置可选属性数据 */
    init_ret = sle_mouse_set_optional_properties();
    if (init_ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("Sle init hid,set optional properties fail:%u\n", init_ret);
        return init_ret;
    }

    /* 初始化服务并启动 */
    init_ret = sle_hid_server_instance_init(server_id, mouse_hid_cbk);
    if (init_ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("Sle init hid,instance init fail:%u\n", init_ret);
        return init_ret;
    }

    /* 获取输入报告属性句柄 */
    init_ret = sle_hid_server_get_handle_by_type(server_id, SLE_HID_PROPERTY_TYPE_INPUT_REPORT,
        &g_sle_mouse_rpt_handle);
    if (init_ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("Sle init hid,get input report handle fail:%u\n", init_ret);
        return init_ret;
    }

    return ERRCODE_SLE_SUCCESS;
}

/* 获取输入报告句柄，各实例handle不一样 */
uint8_t sle_mouse_get_ssap_rpt_handle(void)
{
    return g_sle_mouse_rpt_handle;
}

/**********************************************
 * @brief 发送鼠标输入报告数据
 * @note 需要先初始化HID server，先调用sle_mouse_hid_server_init
 **********************************************/
errcode_t sle_hid_mouse_server_send_input_report(uint8_t server_id, uint16_t conn_id, uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0) {
        return ERRCODE_SLE_PARAM_ERR;
    }

    ssaps_ntf_ind_t param = {
        .type = SSAP_PROPERTY_TYPE_VALUE,
        .handle = sle_mouse_get_ssap_rpt_handle(),
        .value = data,
        .value_len = len,
    };

    errcode_t ret = ERRCODE_SLE_SUCCESS;
    ret = ssaps_notify_indicate(server_id, conn_id, &param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("hid trans data failed,server id:%u,ret:0x%x\n", server_id, ret);
        return ret;
    }

    return ERRCODE_SLE_SUCCESS;
}
