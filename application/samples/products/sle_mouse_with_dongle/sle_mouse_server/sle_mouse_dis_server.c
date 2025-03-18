/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 *
 * Description: SLE dis server profile, support multi service instance.
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
#include "sle_mouse_dis_server.h"

/* DIS服务依赖的数据列表汇总 */
typedef struct {
    char *property_name;
    uint8_t property_index;
    uint8_t *property_data;
    uint8_t data_len;
} sle_mouse_dis_data_list;

/********************* DIS服务回调 *********************/
static void sle_dis_ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("dis start service cbk,server_id:%u,service handle:0x%04x,status:%u\n",
        server_id, handle, status);
#else
    unused(server_id);
    unused(handle);
    unused(status);
#endif
}

static void sle_dis_ssaps_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
    errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("dis read req cbk,server_id:%u, conn_id:%x,property handle:0x%04x,status:%u\n",
        server_id, conn_id, read_cb_para->handle, status);
#else
    unused(server_id);
    unused(conn_id);
    unused(read_cb_para);
    unused(status);
#endif
}

static void sle_dis_ssaps_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
    errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("dis write req cbk,server_id:%u,property handle:0x%04x,status:%u\n",
        server_id, conn_id, write_cb_para->handle, status);
#else
    unused(server_id);
    unused(conn_id);
    unused(write_cb_para);
    unused(status);
#endif
}

static void sle_dis_ssaps_indicate_cfm_cbk(uint8_t server_id, uint16_t conn_id,
    sle_indication_cfm_result_t cfm_result, errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("dis indicate cfm cbk,server_id:%u,conn id:%u,res:%u,status:%u",
        server_id, conn_id, cfm_result, status);
#else
    unused(server_id);
    unused(conn_id);
    unused(cfm_result);
    unused(status);
#endif
}

static void sle_dis_ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id,
    ssap_exchange_info_t *mtu_size, errcode_t status)
{
#if (MOUSE_AUTO_TEST == 1)
    osal_printk("dis mtu change cbk,server_id:%u,conn id:%u,mtu:%u,status:%u\n",
        server_id, conn_id, mtu_size->mtu_size, status);
#else
    unused(server_id);
    unused(conn_id);
    unused(mtu_size);
    unused(status);
#endif
}

static errcode_t sle_dis_server_set_properties_data(void)
{
    char pnp_id[] = "HXXXXX-1000-1000-0001";     /* 设备厂商 */
    char dev_mode_info[] = "Sle Mouse";          /* 设备名称，最长31字节 */
    char dev_serial_num[] = "202410251532";      /* 设备序列号 */
    char hardware_version_info[] = "HXXXXX-1.0"; /* 设备硬件版本 */
    char firmware_version_info[] = "HXXXXX-1.0"; /* 设备固件版本 */
    char software_version_info[] = "HXXXXX-1.0"; /* 设备软件版本 */
    uint8_t appearance[] = {0x02, 0x05, 0x00};   /* 设备外观参数，鼠标0x000502小端序----鼠标 */

    /* 属性数据列表 */
    sle_mouse_dis_data_list dis_server_list[] = {
        {"Facture Info",  SLE_DIS_PROPERTY_MANUFACTURE_INFO, (uint8_t *)pnp_id, strlen(pnp_id)},
        {"Device Model",  SLE_DIS_PROPERTY_DEVICE_MODEL_INFO, (uint8_t *)dev_mode_info, strlen(dev_mode_info)},
        {"Serial Number", SLE_DIS_PROPERTY_DEVICE_SERIAL_NUMBER, (uint8_t *)dev_serial_num, strlen(dev_serial_num)},
        {"Hardware Version", SLE_DIS_PROPERTY_HARDWARE_VERSION_INFO,
            (uint8_t *)hardware_version_info, strlen(hardware_version_info)},
        {"Firmware Version", SLE_DIS_PROPERTY_FIRMWARE_VERSION_INFO,
            (uint8_t *)firmware_version_info, strlen(firmware_version_info)},
        {"Software Version", SLE_DIS_PROPERTY_SOFTWARE_VERSION_INFO,
            (uint8_t *)software_version_info, strlen(software_version_info)},
        {"Device Name", SLE_DIS_PROPERTY_DEVICE_LOCAL_NAME,
            (uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME)},
        {"Device Appearance", SLE_DIS_PROPERTY_DEVICE_APPEARANCE_INFO, appearance, sizeof(appearance)}
    };

    sle_service_property_info property_info = { 0 };

    uint8_t property_cnt = sizeof(dis_server_list) / sizeof(sle_mouse_dis_data_list);
    for (uint8_t idx = 0; idx < property_cnt; idx++) {
        property_info.property_data = dis_server_list[idx].property_data;
        property_info.data_len = dis_server_list[idx].data_len;

        errcode_t ret = sle_dis_server_set_property_by_type(dis_server_list[idx].property_index, &property_info);
        if (ret != ERRCODE_SLE_SUCCESS) {
            osal_printk("dis service,set property data failed:0x%x,property name:%s\r\n",
                ret, dis_server_list[idx].property_name);
            return ret;
        }
    }

    return ERRCODE_SLE_SUCCESS;
}

/**********************************************
 * @brief 初始化dis server服务实例
 * @note 依赖全局数据配置 g_mouse_dis_server_template
 **********************************************/
errcode_t sle_mouse_dis_server_init(uint8_t server_id)
{
    /* 初始化 dis server 鼠标服务实例 */
    sle_dis_server_cbk mouse_dis_cbk = {
        .start_service_cbk = sle_dis_ssaps_start_service_cbk,
        .indicate_cfm_cbk = sle_dis_ssaps_indicate_cfm_cbk,
        .read_request_cbk = sle_dis_ssaps_read_request_cbk,
        .write_request_cbk = sle_dis_ssaps_write_request_cbk,
        .mtu_changed_cbk = sle_dis_ssaps_mtu_changed_cbk,
    };

    errcode_t init_ret = sle_dis_server_set_properties_data();
    if (init_ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("init dis,set property data fail:%u\n", init_ret);
        return init_ret;
    }

    /* 初始化服务并启动 */
    init_ret = sle_dis_server_instance_init(server_id, mouse_dis_cbk);
    if (init_ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("init dis service instance fail:%u\n", init_ret);
        return init_ret;
    }

    return ERRCODE_SLE_SUCCESS;
}
