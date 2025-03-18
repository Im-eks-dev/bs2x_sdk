/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE KEYBOARD Server Source. \n
 *
 * History: \n
 * 2023-07-29, Create file. \n
 */
#include "securec.h"
#include "common_def.h"
#include "soc_osal.h"
#include "sle_errcode.h"
#include "sle_device_manager.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_keyboard_server_adv.h"
#include "sle_keyboard_server.h"

#define OCTET_BIT_LEN                   8
#define UUID_LEN_2                      2
#define UUID_INDEX                      14
#define BT_INDEX_4                      4
#define BT_INDEX_0                      0
#define UUID_16BIT_LEN                  2
#define UUID_128BIT_LEN                 16
#define SLE_CONNECTION_INTERVAL         40

#define HID_ELEMENT_NUM            11
#define SLE_REMOTE_REPORT_OUTPUT_LENGTH 8
#define SLE_SRV_ENCODED_REPORT_LEN 8
#define SLE_REMOTE_CONTROL_REPORT_LENGTH 4
#define SLE_HID_POINT 1

/* sle server app uuid */
static uint8_t g_sle_uuid_app_uuid[UUID_LEN_2] = { 0x12, 0x34 };
/* server notify property uuid */
static uint8_t g_sle_property_value[OCTET_BIT_LEN] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
/* sle connect acb handle */
static uint16_t g_sle_conn_handle = 0;
/* sle server handle */
static uint8_t g_server_id = 0;
/* sle ntf service handle */
static uint16_t g_ntf_service_handle = 0;
/* sle ntf property handle */
static uint16_t g_ntf_property_handle = 0;

/* sle hid service handle */
static uint16_t g_hid_service_handle = 0;
/* sle hid property handle */
static uint16_t g_hid_property_handle = 0;
/* sle pair acb handle */
static uint16_t g_sle_pair_handle;
static uint8_t g_sle_hid_control_point = SLE_HID_POINT;
static sle_item_handle_t g_service_hdl[HID_ELEMENT_NUM] = {0};
static uint8_t g_cccd[2] = {0x01, 0x0};

static uint8_t g_input_report_descriptor[SLE_SRV_ENCODED_REPORT_LEN] = {0};
static uint8_t g_output_report_descriptor[SLE_SRV_ENCODED_REPORT_LEN] = {0};

static uint8_t g_sle_input_report[SLE_REMOTE_CONTROL_REPORT_LENGTH] = {0};
static uint8_t g_sle_output_report[SLE_REMOTE_REPORT_OUTPUT_LENGTH] = {0};

static sle_keyboard_server_msg_queue g_sle_keyboard_server_msg_queue = NULL;
static uint8_t g_sle_keyboard_base[] = { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA, \
    0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// uuid
static uint8_t g_sle_hid_group_uuid[HID_ELEMENT_NUM][SLE_UUID_LEN] = {
    /* Human Interface Device service UUID. */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0B },
    /* Report characteristic UUID. 输入报告信息 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3C },
    /* CCCD */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x02, 0x29, 0x00, 0x00 },
    /* Report Reference characteristic UUID. 报告索引信息 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3B },
    /* Report characteristic UUID. 输出报告信息 key data output */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x10 },
    /* CCCD */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x02, 0x29, 0x00, 0x00 },
    /* Report Reference characteristic UUID. 报告索引信息 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x10 },
    /* Report Map characteristic UUID. 类型和格式描述 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x39 },
    /* Hid Control Point characteristic UUID.  工作状态指示 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3A },
};

static uint8_t g_sle_report_map_datas[] = {
    0x00,                       /* type indicate */
    0x05, 0x01,       /* Usage Page (Generic Desktop) */
    0x09, 0x06,       /* Usage (Keyboard) */
    0xA1, 0x01,       /* Collection (Application) */
    0x85, 0x01,       /* Report Id (1) */
    0x05, 0x07,       /* Usage Page (Keyboard) */
    0x19, 0xE0,       /* Usage Minimum (Keyboard Left Control) */
    0x29, 0xE7,       /* Usage Maximum (Keyboard Right GUI) */
    0x15, 0x00,       /* Logical minimum (0) */
    0x25, 0x01,       /* Logical maximum (1) */
    0x75, 0x01,       /* Report Size (1) */
    0x95, 0x08,       /* Report Count (8) */
    0x81, 0x02,       /* Input (data,Value,Absolute,Bit Field) */
    0x95, 0x05,       /* Report Count (5) */
    0x05, 0x08,       /* Usage Page (LEDs) */
    0x19, 0x01,       /* Usage Minimum (Num Lock) */
    0x29, 0x05,       /* Usage Maximum (Kana) */
    0x91, 0x02,       /* Output (data,Value,Absolute,Non-volatile,Bit Field) */
    0x95, 0x01,       /* Report Count (1) */
    0x75, 0x03,       /* Report Size (3) */
    0x91, 0x01,       /* Output (Constant,Array,Absolute,Non-volatile,Bit Field) */
    0x95, 0x06,       /* Report Count (6) */
    0x75, 0x08,       /* Report Size (8) */
    0x15, 0x00,       /* Logical minimum (0) */
    0x26, 0xA4, 0x00, /* Logical maximum (164) */
    0x05, 0x07,       /* Usage Page (Keyboard) */
    0x19, 0x00,       /* Usage Minimum (No event indicated) */
    0x2A, 0xA4, 0x00, /* Usage Maximum (Keyboard ExSel) */
    0x81, 0x00,       /* Input (data,Array,Absolute,Bit Field) */
    0xC0,             /* End Collection */
};

// permissions
static uint8_t g_hid_service_property[HID_ELEMENT_NUM] = {
    0,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP | SSAP_OPERATE_INDICATION_BIT_WRITE,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY,
    SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP,
    // report id0
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP | SSAP_OPERATE_INDICATION_BIT_WRITE,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE,
    // report notify
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE,
};

typedef enum {
    SLE_UUID_INDEX0, // hid service
    SLE_UUID_INDEX1, // input report
    SLE_UUID_INDEX2, // input report CCCD
    SLE_UUID_INDEX3, // Report input Reference
    SLE_UUID_INDEX4, // report output
    SLE_UUID_INDEX5, // report output CCCD
    SLE_UUID_INDEX6, // report output reference
    SLE_UUID_INDEX7, // Report Map
    SLE_UUID_INDEX8, // Hid Control Point
} sle_uuid_index_t;

static void encode2byte_little(uint8_t *_ptr, uint16_t data)
{
    *(uint8_t *)((_ptr) + 1) = (uint8_t)((data) >> 0x8);
    *(uint8_t *)(_ptr) = (uint8_t)(data);
}

static void sle_uuid_set_base(sle_uuid_t *out)
{
    if (memcpy_s(out->uuid, SLE_UUID_LEN, g_sle_keyboard_base, SLE_UUID_LEN) != EOK) {
        sample_print("%s sle_uuid_set_base memcpy fail\n", SLE_KEYBOARD_SERVER_LOG);
        out->len = 0;
        return ;
    }
    out->len = UUID_LEN_2;
}

static void sle_uuid_setu2(uint16_t u2, sle_uuid_t *out)
{
    sle_uuid_set_base(out);
    out->len = UUID_LEN_2;
    encode2byte_little(&out->uuid[UUID_INDEX], u2);
}

static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id,  ssap_exchange_info_t *mtu_size,
                                  errcode_t status)
{
    sample_print(
        "%s ssaps ssaps_mtu_changed_cbk callback server_id:0x%x, conn_id:0x%x, mtu_size:0x%x, status:0x%x\r\n",
        SLE_KEYBOARD_SERVER_LOG, server_id, conn_id, mtu_size->mtu_size, status);
    if (g_sle_pair_handle == 0) {
        g_sle_pair_handle =  1;
    }
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    sample_print("%s start service cbk callback server_id:%d, handle:0x%x, status:0x%x\r\n", SLE_KEYBOARD_SERVER_LOG,
                 server_id, handle, status);
}
static void ssaps_add_service_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    unused(uuid);
    sample_print(
        "%s add service cbk callback server_id:0x%x, handle:0x%x, status:0x%x\r\n", SLE_KEYBOARD_SERVER_LOG,
        server_id, handle, status);
}
static void ssaps_add_property_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t service_handle,
    uint16_t handle, errcode_t status)
{
    unused(uuid);
    sample_print("%s add property cbk callback server_id:0x%x, service_handle:0x%x,handle:0x%x, status:0x%x\r\n",
                 SLE_KEYBOARD_SERVER_LOG, server_id, service_handle, handle, status);
}
static void ssaps_add_descriptor_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t service_handle,
                                     uint16_t property_handle, errcode_t status)
{
    unused(uuid);
    sample_print("%s add descriptor cbk callback server_id:0x%x, service_handle:0x%x, property_handle:0x%x, \
                 status:0x%x\r\n", SLE_KEYBOARD_SERVER_LOG, server_id, service_handle, property_handle, status);
}
static void ssaps_delete_all_service_cbk(uint8_t server_id, errcode_t status)
{
    sample_print("%s delete all service callback server_id:0x%x, status:0x%x\r\n", SLE_KEYBOARD_SERVER_LOG,
                 server_id, status);
}
static errcode_t sle_ssaps_register_cbks(ssaps_read_request_callback ssaps_read_callback,
                                         ssaps_write_request_callback ssaps_write_callback)
{
    errcode_t ret;
    ssaps_callbacks_t ssaps_cbk = {0};
    ssaps_cbk.add_service_cb = ssaps_add_service_cbk;
    ssaps_cbk.add_property_cb = ssaps_add_property_cbk;
    ssaps_cbk.add_descriptor_cb = ssaps_add_descriptor_cbk;
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;
    ssaps_cbk.delete_all_service_cb = ssaps_delete_all_service_cbk;
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    ssaps_cbk.read_request_cb = ssaps_read_callback;
    ssaps_cbk.write_request_cb = ssaps_write_callback;
    ret = ssaps_register_callbacks(&ssaps_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_ssaps_register_cbks,ssaps_register_callbacks fail :0x%x\r\n", SLE_KEYBOARD_SERVER_LOG,
                     ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_ntf_server_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    sle_uuid_setu2(SLE_UUID_SERVER_SERVICE, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_ntf_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle uuid add service fail, ret:0x%x\r\n", SLE_KEYBOARD_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_sample_set_uuid(uint8_t *uuid, sle_uuid_t *service_uuid)
{
    if (memcpy_s(service_uuid->uuid, SLE_UUID_LEN, uuid, SLE_UUID_LEN) != EOK) {
        sample_print("sle hid hid set uuid fail\r\n");
        return ERRCODE_SLE_MEMCPY_FAIL;
    }
    service_uuid->len = SLE_UUID_LEN;
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_sample_add_descriptor_interface(uint32_t properties, uint16_t service_handle, uint16_t property_handle,
    uint16_t len, uint8_t *data)
{
    if (data == NULL) {
        sample_print("sle sample add descriptor interface param is NULL\r\n");
        return ERRCODE_SLE_FAIL;
    }
    ssaps_desc_info_t descriptor = {0};
    descriptor.permissions = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
    descriptor.operate_indication = properties;
    descriptor.type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
    descriptor.value_len = len;
    descriptor.value = data;
    return ssaps_add_descriptor_sync(g_server_id, service_handle, property_handle, &descriptor);
}

errcode_t sle_sample_add_property_interface(uint32_t properties, uint8_t *uuid, uint16_t len, uint8_t *data,
    sle_item_handle_t* service_hdl)
{
    if ((data == NULL) || (service_hdl == NULL)) {
        sample_print("sle sample add property interface param is NULL\r\n");
        return ERRCODE_SLE_FAIL;
    }
    ssaps_property_info_t property = {0};
    errcode_t ret = sle_sample_set_uuid(uuid, &property.uuid);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("[hid server] sle hid uuid set fail, ret:0x%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    property.permissions = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
    property.operate_indication = properties;
    property.value_len = len;
    property.value = data;
    return ssaps_add_property_sync(g_server_id, service_hdl->handle_in, &property, &service_hdl->handle_out);
}

static errcode_t sle_ntf_server_property_add(void)
{
    errcode_t ret;
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};
    uint8_t ntf_value[] = { 0x01, 0x00 };

    property.permissions = SLE_UUID_TEST_PROPERTIES;
    property.operate_indication = SLE_UUID_TEST_OPERATION_INDICATION;
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &property.uuid);
    property.value = (uint8_t *)osal_vmalloc(sizeof(g_sle_property_value));
    if (property.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(property.value, sizeof(g_sle_property_value), g_sle_property_value,
        sizeof(g_sle_property_value)) != EOK) {
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_property_sync(g_server_id, g_ntf_service_handle, &property,  &g_ntf_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle keyboard add property fail, ret:0x%x\r\n", SLE_KEYBOARD_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    descriptor.permissions = SLE_UUID_TEST_DESCRIPTOR;
    descriptor.type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
    descriptor.operate_indication = SLE_UUID_TEST_OPERATION_INDICATION;
    descriptor.value = (uint8_t *)osal_vmalloc(sizeof(ntf_value));
    if (descriptor.value == NULL) {
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(descriptor.value, sizeof(ntf_value), ntf_value, sizeof(ntf_value)) != EOK) {
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_descriptor_sync(g_server_id, g_ntf_service_handle, g_ntf_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle keyboard add descriptor fail, ret:0x%x\r\n", SLE_KEYBOARD_SERVER_LOG, ret);
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    osal_vfree(descriptor.value);
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_hid_server_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    ret = sle_sample_set_uuid(g_sle_hid_group_uuid[SLE_UUID_INDEX0], &service_uuid);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[hid server] sle hid uuid set fail, ret:0x%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    ret = ssaps_add_service_sync(g_server_id, &service_uuid, true, &g_service_hdl[SLE_UUID_INDEX0].handle_out);
    g_hid_service_handle = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[hid server] sle hid add service fail, ret:0x%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    g_service_hdl[SLE_UUID_INDEX1].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    g_service_hdl[SLE_UUID_INDEX3].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    g_service_hdl[SLE_UUID_INDEX4].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    g_service_hdl[SLE_UUID_INDEX6].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    g_service_hdl[SLE_UUID_INDEX7].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    g_service_hdl[SLE_UUID_INDEX8].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_hid_server_property_and_descriptor_add(void)
{
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX1],
        g_sle_hid_group_uuid[SLE_UUID_INDEX1], SLE_REMOTE_CONTROL_REPORT_LENGTH, g_sle_input_report,
        &g_service_hdl[SLE_UUID_INDEX1]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[hid server] sle hid add report fail, ret:0x%x, indet:0x%x\r\n", ret, SLE_UUID_INDEX1);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("[hid server] sle hid add report, proterty hdl:0x%x\r\n",
        g_service_hdl[SLE_UUID_INDEX1].handle_out);
    g_hid_property_handle = g_service_hdl[SLE_UUID_INDEX1].handle_out;
    ret = sle_sample_add_descriptor_interface(g_hid_service_property[SLE_UUID_INDEX2],
        g_service_hdl[SLE_UUID_INDEX0].handle_out, g_service_hdl[SLE_UUID_INDEX1].handle_out, sizeof(g_cccd), g_cccd);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[hid server] sle hid add cccd fail, ret:0x%x, indet:0x%x\r\n", ret, SLE_UUID_INDEX2);
        return ERRCODE_SLE_FAIL;
    }

    g_input_report_descriptor[0] = 0x1;   // [1] : report id
    g_input_report_descriptor[1] = 0x1;   // [1] : input
    g_input_report_descriptor[2] = g_service_hdl[SLE_UUID_INDEX1].handle_out; // [2] rpt handle low
    g_input_report_descriptor[3] = 0;     // [3] rpt handle high
    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX3],
        g_sle_hid_group_uuid[SLE_UUID_INDEX3], SLE_SRV_ENCODED_REPORT_LEN, g_input_report_descriptor,
        &g_service_hdl[SLE_UUID_INDEX3]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[hid server] sle hid add report ref fail, ret:0x%x, indet:0x%x\r\n", ret, SLE_UUID_INDEX3);
        return ERRCODE_SLE_FAIL;
    }

    // 添加输出报告信息属性
    ret = sle_sample_add_output_report();

    /* Report Map. 类型和格式描述 */
    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX7],
        g_sle_hid_group_uuid[SLE_UUID_INDEX7], sizeof(g_sle_report_map_datas), g_sle_report_map_datas,
        &g_service_hdl[SLE_UUID_INDEX7]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[hid server] sle hid add report map ref fail, ret:0x%x, indet:0x%x\r\n", ret,
            SLE_UUID_INDEX7);
        return ERRCODE_SLE_FAIL;
    }

    /* Hid Control Point.  工作状态指示 */
    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX8],
        g_sle_hid_group_uuid[SLE_UUID_INDEX8], sizeof(uint8_t), &g_sle_hid_control_point,
        &g_service_hdl[SLE_UUID_INDEX8]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[hid server] sle hid add hid ctrl point fail, ret:0x%x, indet:0x%x\r\n", ret,
            SLE_UUID_INDEX8);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_sample_add_output_report(void)
{
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    // output report
    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX4],
        g_sle_hid_group_uuid[SLE_UUID_INDEX4], SLE_REMOTE_REPORT_OUTPUT_LENGTH, g_sle_output_report,
        &g_service_hdl[SLE_UUID_INDEX4]);
    osal_printk("[hid server] report output handle :0x%x \r\n", g_service_hdl[SLE_UUID_INDEX4].handle_out);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[hid server] report output add fail, ret:0x%x, indet:0x%x\r\n", ret, SLE_UUID_INDEX4);
        return ERRCODE_SLE_FAIL;
    }

    // report output cccd
    ret = sle_sample_add_descriptor_interface(g_hid_service_property[SLE_UUID_INDEX5],
        g_service_hdl[SLE_UUID_INDEX0].handle_out, g_service_hdl[SLE_UUID_INDEX4].handle_out, sizeof(g_cccd), g_cccd);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[hid server] report output cccd add fail, ret:0x%x, indet:0x%x\r\n", ret, SLE_UUID_INDEX5);
        return ERRCODE_SLE_FAIL;
    }

    // output report reference. 索引信息
    g_output_report_descriptor[0] = 0;
    g_output_report_descriptor[1] = 0x2;
    g_output_report_descriptor[2] = g_service_hdl[SLE_UUID_INDEX4].handle_out; // [2] rpt handle low
    g_output_report_descriptor[3] = 0; // [3] rpt handle high
    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX6],
        g_sle_hid_group_uuid[SLE_UUID_INDEX6], SLE_SRV_ENCODED_REPORT_LEN, g_output_report_descriptor,
        &g_service_hdl[SLE_UUID_INDEX6]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[hid server] output report reference add fail, ret:0x%x, indet:0x%x\r\n", ret, SLE_UUID_INDEX6);
        return ERRCODE_SLE_FAIL;
    }
    return ret;
}

errcode_t sle_keyboard_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = {0};

    sample_print("%s sle keyboard add service in\r\n", SLE_KEYBOARD_SERVER_LOG);
    app_uuid.len = sizeof(g_sle_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_uuid_app_uuid, sizeof(g_sle_uuid_app_uuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_server_id);

    // ntf service
    if (sle_ntf_server_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_ntf_server_property_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    sample_print(
        "%s sle keyboard add ntf service, server_id:0x%x, ntf_service_handle:0x%x, ntf_property_handle:0x%x\r\n",
        SLE_KEYBOARD_SERVER_LOG, g_server_id, g_ntf_service_handle, g_ntf_property_handle);
    ret = ssaps_start_service(g_server_id, g_ntf_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle keyboard add ntf service fail, ret:0x%x\r\n", SLE_KEYBOARD_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }

    // hid service
    if (sle_hid_server_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_hid_server_property_and_descriptor_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    sample_print(
        "%s sle keyboard add hid service, server_id:0x%x, hid_service_handle:0x%x, hid_property_handle:0x%x\r\n",
        SLE_KEYBOARD_SERVER_LOG, g_server_id, g_hid_service_handle, g_hid_property_handle);
    ret = ssaps_start_service(g_server_id, g_hid_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle keyboard add hid service fail, ret:0x%x\r\n", SLE_KEYBOARD_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    sample_print("%s sle keyboard add service out\r\n", SLE_KEYBOARD_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

/* device通过uuid向host发送数据：ntf report */
errcode_t sle_keyboard_server_send_report_by_uuid(const uint8_t *data, uint8_t len)
{
    errcode_t ret;
    ssaps_ntf_ind_by_uuid_t param = {0};
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.start_handle = g_ntf_service_handle;
    param.end_handle = g_ntf_property_handle;
    param.value_len = len;
    param.value = (uint8_t *)osal_vmalloc(len);
    if (param.value == NULL) {
        sample_print("%s send report new fail\r\n", SLE_KEYBOARD_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        sample_print("%s send input report memcpy fail\r\n", SLE_KEYBOARD_SERVER_LOG);
        osal_vfree(param.value);
        return ERRCODE_SLE_FAIL;
    }
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &param.uuid);
    ret = ssaps_notify_indicate_by_uuid(g_server_id, g_sle_conn_handle, &param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_keyboard_server_send_report_by_uuid,ssaps_notify_indicate_by_uuid fail :0x%x\r\n",
                     SLE_KEYBOARD_SERVER_LOG, ret);
        osal_vfree(param.value);
        return ret;
    }
    osal_vfree(param.value);
    return ERRCODE_SLE_SUCCESS;
}

/* device通过handle向host发送数据：ntf report */
errcode_t sle_keyboard_server_send_report_by_handle(const uint8_t *data, uint8_t len)
{
    ssaps_ntf_ind_t param = {0};
    errcode_t ret;
    param.handle = g_ntf_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = (uint8_t *)osal_vmalloc(len);
    param.value_len = len;
    if (param.value == NULL) {
        sample_print("%s send report fail,osal_vmalloc fail! \r\n", SLE_KEYBOARD_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        sample_print("%s send input report memcpy fail\r\n", SLE_KEYBOARD_SERVER_LOG);
        osal_vfree(param.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_notify_indicate(g_server_id, g_sle_conn_handle, &param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_keyboard_server_send_report_by_handle,ssaps_notify_indicate fail :0x%x\r\n",
                     SLE_KEYBOARD_SERVER_LOG, ret);
        osal_vfree(param.value);
        return ret;
    }
    osal_vfree(param.value);
    return ERRCODE_SLE_SUCCESS;
}

/* device通过uuid向host发送数据：hid report */
errcode_t sle_hid_keyboard_server_send_report_by_uuid(const uint8_t *data, uint8_t len)
{
    errcode_t ret;
    ssaps_ntf_ind_by_uuid_t param = {0};
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.start_handle = g_hid_service_handle;
    param.end_handle = g_hid_property_handle;
    param.value_len = len;
    param.value = (uint8_t *)osal_vmalloc(len);
    if (param.value == NULL) {
        sample_print("%s send report new fail\r\n", SLE_KEYBOARD_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        sample_print("%s send input report memcpy fail\r\n", SLE_KEYBOARD_SERVER_LOG);
        osal_vfree(param.value);
        return ERRCODE_SLE_FAIL;
    }
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &param.uuid);
    ret = ssaps_notify_indicate_by_uuid(g_server_id, g_sle_conn_handle, &param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_keyboard_server_send_report_by_uuid,ssaps_notify_indicate_by_uuid fail:0x%x\r\n",
                     SLE_KEYBOARD_SERVER_LOG, ret);
        osal_vfree(param.value);
        return ret;
    }
    osal_vfree(param.value);
    return ERRCODE_SLE_SUCCESS;
}

/* device通过handle向host发送数据：hid report */
errcode_t sle_hid_keyboard_server_send_report_by_handle(const uint8_t *data, uint8_t len)
{
    ssaps_ntf_ind_t param = {0};
    errcode_t ret;
    param.handle = g_hid_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = (uint8_t *)osal_vmalloc(len);
    param.value_len = len;
    if (param.value == NULL) {
        sample_print("%s send report fail,osal_vmalloc fail! \r\n", SLE_KEYBOARD_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        sample_print("%s send input report memcpy fail\r\n", SLE_KEYBOARD_SERVER_LOG);
        osal_vfree(param.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_notify_indicate(g_server_id, g_sle_conn_handle, &param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_hid_keyboard_server_send_report_by_handle, ssaps_notify_indicate fail :0x%x\r\n",
            SLE_KEYBOARD_SERVER_LOG, ret);
        osal_vfree(param.value);
        return ret;
    }
    osal_vfree(param.value);
    return ERRCODE_SLE_SUCCESS;
}

static void sle_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr, sle_acb_state_t conn_state,
                                          sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    uint8_t sle_connect_state[] = "sle_dis_connect";
    sample_print("%s connect state changed callback conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, \
                 disc_reason:0x%x\r\n", SLE_KEYBOARD_SERVER_LOG,conn_id, conn_state, pair_state, disc_reason);
    sample_print("%s connect state changed callback addr:%02x:**:**:**:%02x:%02x\r\n", SLE_KEYBOARD_SERVER_LOG,
                 addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4]);

    sle_connection_param_update_t con_param = {0};
    con_param.conn_id = conn_id;
    con_param.interval_max = SLE_CONNECTION_INTERVAL; // 100个slot  每个0.125ms
    con_param.interval_min = SLE_CONNECTION_INTERVAL; // 100个slot  每个0.125ms
    con_param.max_latency = 0;
    con_param.supervision_timeout = 500; // 设置连接延迟500ms
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        g_sle_conn_handle = conn_id;
        sle_update_connect_param(&con_param);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        g_sle_conn_handle = 0;
        g_sle_pair_handle = 0;
        if (g_sle_keyboard_server_msg_queue != NULL) {
            g_sle_keyboard_server_msg_queue(sle_connect_state, sizeof(sle_connect_state));
        }
    }
}

static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    sample_print("%s pair complete conn_id:0x%02x, status:0x%x\r\n", SLE_KEYBOARD_SERVER_LOG,
                 conn_id, status);
    sample_print("%s pair complete addr:0x%02x:**:**:**:0x%02x:0x%02x\r\n", SLE_KEYBOARD_SERVER_LOG,
                 addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4]);
    g_sle_pair_handle = 1;
}

static errcode_t sle_conn_register_cbks(void)
{
    errcode_t ret;
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;
    ret = sle_connection_register_callbacks(&conn_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_conn_register_cbks,sle_connection_register_callbacks fail :0x%x\r\n",
                     SLE_KEYBOARD_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

uint16_t sle_keyboard_client_is_connected(void)
{
    return g_sle_pair_handle;
}

/* 初始化hid server */
errcode_t sle_keyboard_server_init(ssaps_read_request_callback ssaps_read_callback,
                                   ssaps_write_request_callback ssaps_write_callback)
{
    errcode_t ret;
    ret = sle_keyboard_announce_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_keyboard_server_init,sle_keyboard_announce_register_cbks fail :0x%x\r\n",
                     SLE_KEYBOARD_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_conn_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_keyboard_server_init,sle_conn_register_cbks fail :0x%x\r\n",
            SLE_KEYBOARD_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_ssaps_register_cbks(ssaps_read_callback, ssaps_write_callback);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_keyboard_server_init,sle_ssaps_register_cbks fail :0x%x\r\n",
            SLE_KEYBOARD_SERVER_LOG, ret);
        return ret;
    }
    sample_print("%s init ok\r\n", SLE_KEYBOARD_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

void sle_keyboard_server_register_msg(sle_keyboard_server_msg_queue sle_keyboard_server_msg)
{
    g_sle_keyboard_server_msg_queue = sle_keyboard_server_msg;
}