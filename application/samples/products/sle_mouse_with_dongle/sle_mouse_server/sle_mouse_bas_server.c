/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 *
 * Description: SLE bas server profile, support multi service instance.
 * Author: @CompanyNameTag
 * History: add new file
 * 2024-09-13, Create file.
 */
#include "securec.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "sle_errcode.h"
#include "common_def.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_stru.h"
#include "sle_ssap_server.h"
#include "service_common.h"
#include "sle_mouse_bas_server.h"

/********************* BAS服务回调 *********************/
static void sle_bas_ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("bas start service cbk,server_id:%u,service handle:0x%04x,status:%u\n",
        server_id, handle, status);
#else
    unused(server_id);
    unused(handle);
    unused(status);
#endif
}

static void sle_bas_ssaps_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
    errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("bas read req cbk,server_id:%u,conn_id:%x,property handle:0x%04x,status:%u\n",
        server_id, conn_id, read_cb_para->handle, status);
#else
    unused(server_id);
    unused(conn_id);
    unused(read_cb_para);
    unused(status);
#endif
}

static void sle_bas_ssaps_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
    errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("bas write req cbk,server_id:%u,conn_id:%x,property handle:0x%04x,status:%u\n",
        server_id, conn_id, write_cb_para->handle, status);
#else
    unused(server_id);
    unused(conn_id);
    unused(write_cb_para);
    unused(status);
#endif
}

static void sle_bas_ssaps_indicate_cfm_cbk(uint8_t server_id, uint16_t conn_id,
    sle_indication_cfm_result_t cfm_result, errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("bas indicate cfm cbk,server_id:%u,conn id:%u,indicate res:%u,status:%u",
        server_id, conn_id, cfm_result, status);
#else
    unused(server_id);
    unused(conn_id);
    unused(cfm_result);
    unused(status);
#endif
}

static void sle_bas_ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id,
    ssap_exchange_info_t *mtu_size, errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("bas server mtu change cbk,server_id:%u,conn id:%u,mtu:%u,status:%u\n",
        server_id, conn_id, mtu_size->mtu_size, status);
#else
    unused(server_id);
    unused(conn_id);
    unused(mtu_size);
    unused(status);
#endif
}

/**
 * @brief 设置BAS服务的必选属性数据
 */
static errcode_t sle_mouse_bas_set_properties_data(void)
{
    uint8_t sle_bas_default_battery = 100;
    sle_bas_descriptor_client_att bas_client_desp = {
        .notify = 1,    /* 兼容处理：不需要写入cccd值即可读取数据 */
    };

    sle_service_property_info battter_info = {
        .property_data = &sle_bas_default_battery,
        .data_len = sizeof(sle_bas_default_battery),
        .desp_data = {
            [SLE_SERVICE_PROPERTY_DESCRIPTOR_CLIENT_ATT] = {
                .data = (uint8_t *)&bas_client_desp,
                .data_len = sizeof(bas_client_desp),
            }
        }
    };

    return sle_bas_server_set_property_by_type(SLE_BAS_PROPERTY_REMAIN_BATTERY_CAPACITY_RATIO, &battter_info);
}

/**********************************************
 * @brief 初始化bas server服务实例
 * @note 依赖全局数据配置 g_mouse_bas_server_template
 **********************************************/
errcode_t sle_mouse_bas_server_init(uint8_t server_id)
{
    /* 初始化 bas server 鼠标服务实例 */
    sle_bas_server_cbk mouse_bas_cbk = {
        .start_service_cbk = sle_bas_ssaps_start_service_cbk,
        .indicate_cfm_cbk = sle_bas_ssaps_indicate_cfm_cbk,
        .read_request_cbk = sle_bas_ssaps_read_request_cbk,
        .write_request_cbk = sle_bas_ssaps_write_request_cbk,
        .mtu_changed_cbk = sle_bas_ssaps_mtu_changed_cbk,
    };

    errcode_t init_ret = sle_mouse_bas_set_properties_data();
    if (init_ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("init bas service,set properties fail:%u\n", init_ret);
        return init_ret;
    }

    /* 初始化服务并启动 */
    init_ret = sle_bas_server_instance_init(server_id, mouse_bas_cbk);
    if (init_ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("init bas service,instance init fail:%u\n", init_ret);
        return init_ret;
    }

    return ERRCODE_SLE_SUCCESS;
}

/* BAS服务主动发送人机交互数据 */
void sle_bas_send_remain_capacity_ratio(uint8_t server_id, uint16_t conn_id, uint8_t remain_capacity_ratio)
{
    bas_server_ntf_ind_param mouse_data = {
        .data = (uint8_t*)&remain_capacity_ratio,
        .data_len = sizeof(remain_capacity_ratio),
        .property_type = SSAP_PROPERTY_TYPE_VALUE,
        .property_index = SLE_BAS_PROPERTY_REMAIN_BATTERY_CAPACITY_RATIO, /* UUID */
    };

    /* 调用HID接口发送数据 */
    errcode_t ret = sle_bas_server_trans_data(server_id, conn_id, &mouse_data);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("trans bas data failed:%u\r\n", ret);
    }
}