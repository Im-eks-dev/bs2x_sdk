/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 *
 * Description: SLE channel server profile, support multi service instance.
 * Author: @CompanyNameTag
 * History: add new file
 * 2024-10-23, Create file.
 */
#include "securec.h"
#include "osal_debug.h"
#include "sle_errcode.h"
#include "sle_connection_manager.h"
#include "sle_ssap_stru.h"
#include "sle_ssap_server.h"
#include "mouse_mode_manage.h"
#include "mouse_data_transfer_manage.h"
#include "sle_low_latency_service.h"
#include "sle_mouse_server.h"
#include "sle_mouse_channel_server.h"

typedef enum {
    SLE_CHANNEL_SERVICE_INDEX,
    SLE_CHANNEL_CHAR_REV_INDEX,
    SLE_CHANNEL_CHAR_SEND_INDEX,
    SLE_CHANNEL_CCCD_INDEX,
    SLE_CHANNEL_MAX_INDEX
} channelUuidEnum;

static uint8_t g_channel_group_uuid[SLE_CHANNEL_MAX_INDEX][SLE_UUID_LEN] = {
    /* channel service UUID. */
    { 0x46, 0x64, 0x35, 0x63, 0x19, 0x96, 0x95, 0x04,
      0x08, 0x29, 0x16, 0x13, 0x30, 0xff, 0x00, 0x00 },
    /* characteristic UUID for receiving data. */
    { 0x46, 0x64, 0x35, 0x63, 0x19, 0x96, 0x95, 0x04,
      0x08, 0x29, 0x16, 0x13, 0x31, 0xff, 0x00, 0x00 },
    /* characteristic UUID for sendig data. */
    { 0x46, 0x64, 0x35, 0x63, 0x19, 0x96, 0x95, 0x04,
      0x08, 0x29, 0x16, 0x13, 0x32, 0xff, 0x00, 0x00 },
    /* CCC descriptor, 0x2902 */
    { 0x46, 0x64, 0x35, 0x63, 0x19, 0x96, 0x95, 0x04,
      0x08, 0x29, 0x16, 0x13, 0x02, 0x29, 0x00, 0x00 },
};

static sle_item_handle_t g_channel_service_hdl[SLE_CHANNEL_MAX_INDEX] = {0};

/********************* channel服务回调 *********************/
static void sle_channel_ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("channel server start service callback,server_id:%u,service handle:%u,status:%u\n",
        server_id, handle, status);
}

static void sle_channel_ssaps_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    osal_printk("channel server read request callback,server_id:%u, conn_id:%x,"
        "property handle:%u,property type:%u,status:%u\n",
        server_id, conn_id, read_cb_para->handle, read_cb_para->type, status);
}

static uint8_t get_report_rate_level(uint16_t report_rate)
{
    switch (report_rate) {
        case SLE_MOUSE_3Hz:
        case SLE_MOUSE_10Hz:
        case SLE_MOUSE_125Hz:
            return RATE_LEVEL_0;
        case SLE_MOUSE_250Hz:
            return RATE_LEVEL_1;
        case SLE_MOUSE_500Hz:
            return RATE_LEVEL_2;
        case SLE_MOUSE_1000Hz:
            return RATE_LEVEL_3;
        case SLE_MOUSE_2000Hz:
            return RATE_LEVEL_4;
        case SLE_MOUSE_4000Hz:
            return RATE_LEVEL_5;
        case SLE_MOUSE_8000Hz:
            return RATE_LEVEL_6;
        default:
            return 0;
    }
}

// 数据帧格式：Head(1Byte)+Type(1Byte)+Length(2Byte不含Head和Type)+Cmd(1Byte)+Data(NByte)
// 例如：2c 34 04 00 00 00 00 00
#define SLE_MOUSE_WRITE_REQUEST_CUSTOMER_DATA_MIN_LEN    5
static errcode_t sle_mouse_server_parser_customer_data(uint8_t *buf, uint16_t len)
{
    if (len < SLE_MOUSE_WRITE_REQUEST_CUSTOMER_DATA_MIN_LEN) {
        osal_printk("[Error]:SLE dongle recv customer data len invalid:%u\n", len);
        return ERRCODE_SLE_FAIL;
    }

    uint8_t package_head = buf[0]; // 0字节head
    uint8_t package_type = buf[1]; // 1字节type
    uint16_t package_len = (uint16_t)((buf[3] << 8) | (buf[2])); // 2和3字节长度

    osal_printk("\r\n***** sle_mouse_recv_dongle_data_dump start******\n");
    for (uint8_t i = 0; i < len; i++) {
        osal_printk("0x%02x ", buf[i]);
    }
    osal_printk("\r\n");

    if (package_head != SLE_MOUSE_RCV_HEAD ||
        package_type != SLE_MOUSE_RCV_TYPE) {
        osal_printk("[Error]:SLE dongle recv data invalid,msg head:0x%02x,type:0x%02x\n",
            package_head, package_type);
        return ERRCODE_SLE_FAIL;
    }

    app_mouse_customer_cmd_t package_command = buf[4]; // 4字节cmd
    switch (package_command) {
        case SLE_MOUSE_GET_DPI_CMD:
            osal_printk("***** SLE_MOUSE_GET_DPI_CMD:len=%d ******\r\n", package_len);
            break;
        case SLE_MOUSE_SET_DPI_CMD:
            osal_printk("***** SLE_MOUSE_SET_DPI_CMD:len=%d ******\r\n", package_len);
            app_mouse_dpi_change(*((uint32_t *)&buf[5])); // 5字节起为dpi_value
            break;
        case SLE_MOUSE_GET_REPORT_CMD:
            osal_printk("***** SLE_MOUSE_GET_REPORT_CMD:len=%d ******\r\n", package_len);
            break;
        case SLE_MOUSE_SET_REPORT_CMD:
            app_mouse_set_sensor_mode(get_report_rate_level(*((uint16_t *)&buf[5])));  // 5字节起为report_rate
            osal_printk("***** receive dongle msg, set rate(%uHz) ******\r\n", *((uint16_t *)&buf[5]));
            break;
        case SLE_MOUSE_GET_BATTERY_LAVEL:
            app_mouse_send_battery_percentage_to_dongle();
            break;
        default:
            osal_printk("***** SLE_DONGLE_UNKNOW_CMD ******\r\n");
            break;
    }

    return ERRCODE_SLE_SUCCESS;
}

static void sle_channel_ssaps_write_request_cbk(
    uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para, errcode_t status)
{
    if (write_cb_para == NULL) {
        return;
    }
    osal_printk("channel server write request callback,server_id:%u, conn_id:%x,"
        "property handle:%u,property type:%u,status:%u\n",
        server_id, conn_id, write_cb_para->handle, write_cb_para->type, status);
        
    sle_mouse_server_parser_customer_data(write_cb_para->value, write_cb_para->length);
}

static void sle_channel_ssaps_indicate_cfm_cbk(uint8_t server_id, uint16_t conn_id,
    sle_indication_cfm_result_t cfm_result, errcode_t status)
{
    osal_printk("channel server indicate cmf callback,server_id:%u,conn id:%u,indicate res:%u,status:%u",
        server_id, conn_id, cfm_result, status);
}

static void sle_channel_ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id,
    ssap_exchange_info_t *mtu_size, errcode_t status)
{
    osal_printk("channel server mtu change callback,server_id:%u,conn id:%u,mtu:%u,status:%u\n",
        server_id, conn_id, mtu_size->mtu_size, status);
}

static errcode_t sle_mouse_set_uuid(uint8_t *uuid, sle_uuid_t *service_uuid)
{
    if (memcpy_s(service_uuid->uuid, SLE_UUID_LEN, uuid, SLE_UUID_LEN) != EOK) {
        osal_printk("sle mouse hid set uuid fail\r\n");
        return ERRCODE_SLE_MEMCPY_FAIL;
    }
    service_uuid->len = SLE_UUID_LEN;
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_mouse_channel_add_descriptor_interface(uint8_t server_id, uint32_t properties, uint16_t service_handle,
    uint16_t property_handle, uint16_t len, uint8_t *data)
{
    if (data == NULL) {
        osal_printk("sle sample add descriptor interface param is NULL\r\n");
        return ERRCODE_SLE_FAIL;
    }
    ssaps_desc_info_t descriptor = {0};
    descriptor.permissions = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
    descriptor.operate_indication = properties;
    descriptor.type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
    descriptor.value_len = len;
    descriptor.value = data;
    return ssaps_add_descriptor_sync(server_id, service_handle, property_handle, &descriptor);
}

errcode_t sle_mouse_channel_add_property_interface(uint8_t server_id, uint32_t properties, uint8_t *uuid, uint16_t len,
    uint8_t *data, sle_item_handle_t* service_hdl)
{
    if ((data == NULL) || (service_hdl == NULL)) {
        osal_printk("sle sample add property interface param is NULL\r\n");
        return ERRCODE_SLE_FAIL;
    }
    ssaps_property_info_t property = {0};
    errcode_t ret = sle_mouse_set_uuid(uuid, &property.uuid);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse uuid set fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    property.permissions = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
    property.operate_indication = properties;
    property.value_len = len;
    property.value = data;
    return ssaps_add_property_sync(server_id, service_hdl->handle_in, &property, &service_hdl->handle_out);
}

static errcode_t sle_channel_property_and_descriptor_add(uint8_t server_id)
{
    /* 私有接收 */
    uint8_t rcv_data[CHANNEL_MAX_DATA_SIZE] = {0};
    uint32_t rcv_properties = SSAP_OPERATE_INDICATION_BIT_WRITE | SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP;
    errcode_t ret = sle_mouse_channel_add_property_interface(server_id,
        rcv_properties,
        g_channel_group_uuid[SLE_CHANNEL_CHAR_REV_INDEX],
        CHANNEL_MAX_DATA_SIZE,
        rcv_data,
        &g_channel_service_hdl[SLE_CHANNEL_CHAR_REV_INDEX]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[Error]:sle mouse add channel server fail,add local dev name ret:0x%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    /* 私有发送 */
    uint8_t snd_data[CHANNEL_MAX_DATA_SIZE] = {0};
    uint32_t snd_properties = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY;
    ret = sle_mouse_channel_add_property_interface(server_id,
        snd_properties,
        g_channel_group_uuid[SLE_CHANNEL_CHAR_SEND_INDEX],
        CHANNEL_MAX_DATA_SIZE,
        snd_data,
        &g_channel_service_hdl[SLE_CHANNEL_CHAR_SEND_INDEX]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[Error]:sle mouse add channel server fail,add appearance ret:0x%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    /* cccd */
    uint8_t cccd_data[2] = {0x01, 0x00};
    uint32_t cccd_properties = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE |
                               SSAP_OPERATE_INDICATION_BIT_DESCRITOR_WRITE |
                               SSAP_OPERATE_INDICATION_BIT_DESCRIPTOR_CLIENT_CONFIGURATION_WRITE;
    ret = sle_mouse_channel_add_descriptor_interface(server_id,
        cccd_properties,
        g_channel_service_hdl[SLE_CHANNEL_SERVICE_INDEX].handle_out,
        g_channel_service_hdl[SLE_CHANNEL_CHAR_SEND_INDEX].handle_out,
        sizeof(cccd_data),
        cccd_data);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[Error]:sle mouse add channel server fail,add manu facture ret:0x%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_channel_service_add(uint8_t server_id)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    ret = sle_mouse_set_uuid(g_channel_group_uuid[SLE_CHANNEL_SERVICE_INDEX], &service_uuid);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("sle mouse uuid set fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    ret = ssaps_add_service_sync(server_id, &service_uuid, 1,
        &g_channel_service_hdl[SLE_CHANNEL_SERVICE_INDEX].handle_out);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("sle mouse add service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    g_channel_service_hdl[SLE_CHANNEL_CHAR_REV_INDEX].handle_in =
        g_channel_service_hdl[SLE_CHANNEL_SERVICE_INDEX].handle_out;
    g_channel_service_hdl[SLE_CHANNEL_CHAR_SEND_INDEX].handle_in =
        g_channel_service_hdl[SLE_CHANNEL_SERVICE_INDEX].handle_out;
    g_channel_service_hdl[SLE_CHANNEL_CCCD_INDEX].handle_in =
        g_channel_service_hdl[SLE_CHANNEL_SERVICE_INDEX].handle_out;
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_mouse_channel_server_add(uint8_t server_id)
{
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    ret = sle_channel_service_add(server_id);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[Error]:sle mouse add channel server fail,add dis service ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    ret = sle_channel_property_and_descriptor_add(server_id);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[Error]:sle mouse add channel server fail,add property & desp ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    osal_printk("[Info]:sle mouse add channel server,server_id:%x,service_handle:%x\r\n",
        server_id, g_channel_service_hdl[SLE_CHANNEL_SERVICE_INDEX].handle_out);

    ret = ssaps_start_service(server_id, g_channel_service_hdl[SLE_CHANNEL_SERVICE_INDEX].handle_out);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[Error]:sle mouse add channel server fail,start service ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    osal_printk("[Info]:sle mouse add channel server completed.\r\n");
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_mouse_channel_server_init(uint8_t server_id)
{
    ssaps_callbacks_t channel_srv_cbk = {
        .start_service_cb      = sle_channel_ssaps_start_service_cbk,
        .read_request_cb       = sle_channel_ssaps_read_request_cbk,
        .write_request_cb      = sle_channel_ssaps_write_request_cbk,
        .indicate_cfm_cb       = sle_channel_ssaps_indicate_cfm_cbk,
        .mtu_changed_cb        = sle_channel_ssaps_mtu_changed_cbk,
    };

    /* 注册服务 */
    sle_uuid_t channel_uuid = { 0 };
    errcode_t ret = sle_mouse_set_uuid(g_channel_group_uuid[SLE_CHANNEL_SERVICE_INDEX], &channel_uuid);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("sle mouse uuid set fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    ret = ssaps_register_multi_callbacks(server_id, &channel_uuid, &channel_srv_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("channel server init failed,reg callback ret:%u,server id:%u\n", ret, server_id);
        return ret;
    }

    ret = sle_mouse_channel_server_add(server_id);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("Sle init channel service failed,instance init ret:%u\n", ret);
        return ret;
    }

    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_mouse_channel_send_notification(uint8_t server_id, uint16_t conn_id, uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0) {
        return ERRCODE_SLE_PARAM_ERR;
    }

    ssaps_ntf_ind_t param = {
        .type = SSAP_PROPERTY_TYPE_VALUE,
        .handle = g_channel_service_hdl[SLE_CHANNEL_CHAR_SEND_INDEX].handle_out,
        .value = data,
        .value_len = len,
    };

    errcode_t ret = ERRCODE_SLE_SUCCESS;
    ret = ssaps_notify_indicate(server_id, conn_id, &param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("hid server trans data failed,server id:%u,ret:0x%x\n", server_id, ret);
        return ret;
    }

    return ERRCODE_SLE_SUCCESS;
}

