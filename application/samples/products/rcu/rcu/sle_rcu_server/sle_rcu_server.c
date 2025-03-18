/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE RCU Server Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-21, Create file. \n
 */

#include "securec.h"
#include "common_def.h"
#include "osal_debug.h"
#include "osal_addr.h"
#include "osal_task.h"
#include "sle_errcode.h"
#include "sle_common.h"
#include "sle_connection_manager.h"
#include "sle_device_manager.h"
#include "sle_device_discovery.h"
#include "sle_rcu_server_adv.h"
#include "sle_ota.h"
#include "sle_vdt_codec.h"
#include "rcu.h"
#include "app_status.h"
#include "sle_rcu_server.h"
#if defined(CONFIG_RCU_MASS_PRODUCTION_TEST)
#include "rcu_mp_test.h"
#endif  /* CONFIG_RCU_MASS_PRODUCTION_TEST */

#define OCTET_BIT_LEN                   8
#define UUID_LEN_2                      2
#define UUID_INDEX                      14
#define BT_INDEX_4                      4
#define BT_INDEX_0                      0
#define UUID_16BIT_LEN                  2
#define UUID_128BIT_LEN                 16
#define SLE_ADV_HANDLE_DEFAULT          1
#define SLE_RCU_STATE_DISCONNECT        0
#define SLE_RCU_STATE_CONNECTED         1
#define UUID_BIT0                  0
#define UUID_BIT1                  1
#define UUID_BIT2                  2
#define UUID_BIT3                  3
#define UUID_BIT4                  4
#define UUID_BIT5                  5
#define UUID_BIT6                  6
#define UUID_BIT7                  7
#define UUID_BIT8                  8
#define UUID_BIT9                  9
#define UUID_BIT10                 10
#define UUID_BIT11                 11
#define UUID_BIT12                 12
#define UUID_BIT13                 13
#define UUID_BIT14                 14
#define UUID_BIT15                 15
#define RCU_SEND_BUFF_LENGTH       20
#define HID_ELEMENT_NUM            6
#define SLE_REMOTE_CONTROL_REPORT_LENGTH 4
#define SLE_HID_POINT 1

/* sle server app uuid */
static uint8_t g_sle_uuid_app_uuid[UUID_LEN_2] = { 0x12, 0x34 };
/* server notify property uuid */
static uint8_t g_sle_property_value[OCTET_BIT_LEN] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
/* sle connect acb handle */
static uint16_t g_sle_conn_handle[CONFIG_SLE_MULTICON_NUM] = { 0 };
/* sle server handle */
static uint8_t g_server_id = 0;
/* sle service handle */
static uint16_t g_service_handle = 0;
static uint16_t g_service_amic_handle = 0;
/* sle ntf property handle */
static uint16_t g_property_handle = 0;
static uint16_t g_amic_property_handle = 0;
/* sle pair acb handle */
static uint16_t g_sle_pair_handle;
bool g_ssaps_ready = false;
static int g_conn_update = 0;
static uint16_t g_sle_conn_num = 0;
static uint8_t g_sle_input_report[SLE_REMOTE_CONTROL_REPORT_LENGTH] = {0};
static uint8_t g_sle_hid_control_point = SLE_HID_POINT;
static uint16_t g_sle_conn_id;
static sle_addr_t g_sle_addr = { 0 };
/* 低功耗连接参数信息 */
static sle_connection_param_update_t g_work_to_standby = { 0, 100, 100, 48, 3000 };
static sle_connection_param_update_t g_standby_to_work = { 0, 100, 100, 3, 3000 };
static bool g_low_power_state = false; /* false:关闭, true:打开 */

/* Hid Information characteristic not defined */
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
    /* Report Map characteristic UUID. 类型和格式描述 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x39 },
    /* Hid Control Point characteristic UUID.  工作状态指示 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3A },
};

static sle_item_handle_t g_service_hdl[HID_ELEMENT_NUM] = {0};

static uint8_t g_hid_service_property[HID_ELEMENT_NUM] = {
    0,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP | SSAP_OPERATE_INDICATION_BIT_WRITE,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY,
    SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP,
};
#define SLE_SRV_ENCODED_REPORT_LEN 8
static uint8_t g_cccd[2] = {0x01, 0x0};

static uint8_t g_input_report_descriptor[SLE_SRV_ENCODED_REPORT_LEN] = {0};

typedef enum {
    SLE_UUID_INDEX0, // hid service
    SLE_UUID_INDEX1, // input report
    SLE_UUID_INDEX2, // CCCD
    SLE_UUID_INDEX3, // Report Reference
    SLE_UUID_INDEX4, // Report Map
    SLE_UUID_INDEX5, // Hid Control Point
} sle_uuid_index_t;

#define input(size)             (0x80 | (size))
#define output(size)            (0x90 | (size))
#define feature(size)           (0xb0 | (size))
#define collection(size)        (0xa0 | (size))
#define end_collection(size)    (0xc0 | (size))

/* Global items */
#define usage_page(size)        (0x04 | (size))
#define logical_minimum(size)   (0x14 | (size))
#define logical_maximum(size)   (0x24 | (size))
#define physical_minimum(size)  (0x34 | (size))
#define physical_maximum(size)  (0x44 | (size))
#define uint_exponent(size)     (0x54 | (size))
#define uint(size)              (0x64 | (size))
#define report_size(size)       (0x74 | (size))
#define report_id(size)         (0x84 | (size))
#define report_count(size)      (0x94 | (size))
#define push(size)              (0xa4 | (size))
#define pop(size)               (0xb4 | (size))

/* Local items */
#define usage(size)                 (0x08 | (size))
#define usage_minimum(size)         (0x18 | (size))
#define usage_maximum(size)         (0x28 | (size))
#define designator_index(size)      (0x38 | (size))
#define designator_minimum(size)    (0x48 | (size))
#define designator_maximum(size)    (0x58 | (size))
#define string_index(size)          (0x78 | (size))
#define string_minimum(size)        (0x88 | (size))
#define string_maximum(size)        (0x98 | (size))
#define delimiter(size)             (0xa8 | (size))

static uint8_t g_sle_report_map_datas[] = {
    usage_page(1),      0x01,
    usage(1),           0x06,
    collection(1),      0x01,
    report_id(1),       0x01,
    usage_page(1),      0x07,
    usage_minimum(1),   0xE0,
    usage_maximum(1),   0xE7,
    logical_minimum(1), 0x00,
    logical_maximum(1), 0x01,
    report_size(1),     0x01,
    report_count(1),    0x08,
    input(1),           0x02,
    report_count(1),    0x01,
    report_size(1),     0x08,
    input(1),           0x01,
    report_count(1),    0x05,
    report_size(1),     0x01,
    usage_page(1),      0x08,
    usage_minimum(1),   0x01,
    usage_maximum(1),   0x05,
    output(1),          0x02,
    report_count(1),    0x01,
    report_size(1),     0x03,
    output(1),          0x01,
    report_count(1),    0x06,
    report_size(1),     0x08,
    logical_minimum(1), 0x00,
    logical_maximum(1), 0xFF,
    usage_page(1),      0x07,
    usage_minimum(1),   0x00,
    usage_maximum(1),   0xFF,
    input(1),           0x00,
    end_collection(0),

    usage_page(1),      0x0C,
    usage(1),           0x01,
    collection(1),      0x01,
    report_id(1),       0x03,
    logical_minimum(1), 0x00,
    logical_maximum(2), 0xff, 0x1f,
    usage_minimum(1),   0x00,
    usage_maximum(2),   0xff, 0x1f,
    report_size(1),     0x10,
    report_count(1),    0x01,
    input(1),           0x00,
    end_collection(0),

    usage_page(1),      0x01,
    usage(1),           0x09,
    collection(1),      0x01,
    report_id(1),       0x02,
    usage(1),           0x81,
    logical_minimum(1), 0x00,
    logical_maximum(2), 0xff, 0x1f,
    usage_minimum(1),   0x00,
    usage_maximum(2),   0xff, 0x1f,
    report_size(1),     0x10,
    report_count(1),    0x01,
    input(1),           0x00,
    end_collection(0),

    usage_page(1),      0x01,
    usage(1),           0x02,
    collection(1),      0x01,
    report_id(1),       0x04,
    usage(1),           0x01,
    collection(1),      0x00,
    report_count(1),    0x03,
    report_size(1),     0x01,
    usage_page(1),      0x09,
    usage_minimum(1),   0x1,
    usage_maximum(1),   0x3,
    logical_minimum(1), 0x00,
    logical_maximum(1), 0x01,
    input(1),           0x02,
    report_count(1),    0x01,
    report_size(1),     0x05,
    input(1),           0x01,
    report_count(1),    0x03,
    report_size(1),     0x08,
    usage_page(1),      0x01,
    usage(1),           0x30,
    usage(1),           0x31,
    usage(1),           0x38,
    logical_minimum(1), 0x81,
    logical_maximum(1), 0x7f,
    input(1),           0x06,
    end_collection(0),
    end_collection(0),
};

uint8_t g_out_low_latency_data[LOW_LATENCY_DATA_MAX] = { 0 };

/* sle gamepad conn state */

static uint8_t g_sle_rcu_base[] = { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA, \
    0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static void encode2byte_little(uint8_t *_ptr, uint16_t data)
{
    *(uint8_t *)((_ptr) + 1) = (uint8_t)((data) >> 0x8);
    *(uint8_t *)(_ptr) = (uint8_t)(data);
}

uint16_t rcu_get_handle(void)
{
    return g_property_handle;
}
 
uint8_t get_g_connid(void)
{
    return g_sle_conn_id;
}
 
uint8_t rcu_get_server_id(void)
{
    return g_server_id;
}

bool get_g_ssaps_ready(void)
{
    return g_ssaps_ready;
}

int get_g_conn_update(void)
{
    return g_conn_update;
}

uint16_t get_g_sle_conn_hdl(uint32_t index)
{
    return g_sle_conn_handle[index];
}

uint16_t get_g_sle_conn_num(void)
{
    return g_sle_conn_num;
}

static void sle_uuid_set_base(sle_uuid_t *out)
{
    if (memcpy_s(out->uuid, SLE_UUID_LEN, g_sle_rcu_base, SLE_UUID_LEN) != EOK) {
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
    unused(server_id);
    unused(mtu_size);
    unused(status);
    app_print("%s ssaps ssaps_mtu_changed_cbk callback server_id:%x, conn_id:%x, mtu_size:%x, status:%x\r\n",
              SLE_RCU_SERVER_LOG, server_id, conn_id, mtu_size->mtu_size, status);
    g_ssaps_ready = true;
    if (g_sle_pair_handle == 0) {
        g_sle_pair_handle =  conn_id + 1;
    }
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    unused(server_id);
    unused(handle);
    unused(status);
# if !defined(CONFIG_RCU_MASS_PRODUCTION_TEST)
    if (sle_rcu_server_adv_init() != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_rcu_server_init,sle_rcu_server_adv_init fail\r\n",
                    SLE_RCU_SERVER_LOG);
        return;
    }
#endif
    app_print("%s start service cbk callback server_id:%d, handle:%x, status:%x\r\n", SLE_RCU_SERVER_LOG,
              server_id, handle, status);
}

static void ssaps_add_service_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    unused(uuid);
    unused(server_id);
    unused(handle);
    unused(status);
    app_print("%s add service cbk callback server_id:%x, handle:%x, status:%x\r\n", SLE_RCU_SERVER_LOG,
              server_id, handle, status);
}

static void ssaps_add_property_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t service_handle,
    uint16_t handle, errcode_t status)
{
    unused(uuid);
    unused(server_id);
    unused(service_handle);
    unused(handle);
    unused(status);
    app_print("%s add property cbk callback server_id:%x, service_handle:%x,handle:%x, status:%x\r\n",
              SLE_RCU_SERVER_LOG, server_id, service_handle, handle, status);
}

static void ssaps_add_descriptor_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t service_handle,
                                     uint16_t property_handle, errcode_t status)
{
    unused(uuid);
    unused(server_id);
    unused(service_handle);
    unused(property_handle);
    unused(status);
    app_print("%s add descriptor cbk callback server_id:%x, service_handle:%x, property_handle:%x, \
              status:%x\r\n", SLE_RCU_SERVER_LOG, server_id, service_handle, property_handle, status);
}

static void ssaps_delete_all_service_cbk(uint8_t server_id, errcode_t status)
{
    unused(server_id);
    unused(status);
    app_print("%s delete all service callback server_id:%x, status:%x\r\n", SLE_RCU_SERVER_LOG,
              server_id, status);
}

static errcode_t sle_ssaps_register_cbks(ssaps_read_request_callback ssaps_read_callback,
                                         ssaps_write_request_callback ssaps_write_callback)
{
    errcode_t ret;
    ssaps_callbacks_t ssaps_cbk = { 0 };
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
        osal_printk("%s sle_ssaps_register_cbks,ssaps_register_callbacks fail :%x\r\n", SLE_RCU_SERVER_LOG,
                    ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_server_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = { 0 };
    sle_uuid_setu2(SLE_UUID_SERVER_SERVICE, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_server_property_add(void)
{
    errcode_t ret;
    ssaps_property_info_t property = { 0 };
    ssaps_desc_info_t descriptor = { 0 };
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
    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property,  &g_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
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
    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    osal_vfree(descriptor.value);
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_amic_server_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = { 0 };
    sle_uuid_setu2(SLE_UUID_AMIC_SERVER_SERVICE, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_amic_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_amic_server_property_add(void)
{
    errcode_t ret;
    ssaps_property_info_t property = { 0 };
    ssaps_desc_info_t descriptor = { 0 };
    uint8_t ntf_value[] = { 0x01, 0x00 };

    property.permissions = SLE_UUID_TEST_PROPERTIES;
    property.operate_indication = SLE_UUID_TEST_OPERATION_INDICATION;
    sle_uuid_setu2(SLE_UUID_AMIC_SERVER_NTF_REPORT, &property.uuid);
    property.value = (uint8_t *)osal_vmalloc(sizeof(g_sle_property_value));
    if (property.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(property.value, sizeof(g_sle_property_value), g_sle_property_value,
        sizeof(g_sle_property_value)) != EOK) {
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_property_sync(g_server_id, g_service_amic_handle, &property,  &g_amic_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
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
    ret = ssaps_add_descriptor_sync(g_server_id, g_service_amic_handle, g_amic_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    osal_vfree(descriptor.value);
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_sample_set_uuid(uint8_t *uuid, sle_uuid_t *service_uuid)
{
    if (memcpy_s(service_uuid->uuid, SLE_UUID_LEN, uuid, SLE_UUID_LEN) != EOK) {
        osal_printk("sle mouse hid set uuid fail\r\n");
        return ERRCODE_SLE_MEMCPY_FAIL;
    }
    service_uuid->len = SLE_UUID_LEN;
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_remote_control_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    ret = sle_sample_set_uuid(g_sle_hid_group_uuid[SLE_UUID_INDEX0], &service_uuid);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse uuid set fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_hdl[SLE_UUID_INDEX0].handle_out);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse add service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    g_service_hdl[SLE_UUID_INDEX1].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    g_service_hdl[SLE_UUID_INDEX3].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    g_service_hdl[SLE_UUID_INDEX4].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    g_service_hdl[SLE_UUID_INDEX5].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_sample_add_descriptor_interface(uint32_t properties, uint16_t service_handle, uint16_t property_handle,
    uint16_t len, uint8_t *data)
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
    return ssaps_add_descriptor_sync(g_server_id, service_handle, property_handle, &descriptor);
}

errcode_t sle_sample_add_property_interface(uint32_t properties, uint8_t *uuid, uint16_t len, uint8_t *data,
    sle_item_handle_t* service_hdl)
{
    if ((data == NULL) || (service_hdl == NULL)) {
        osal_printk("sle sample add property interface param is NULL\r\n");
        return ERRCODE_SLE_FAIL;
    }
    ssaps_property_info_t property = {0};
    errcode_t ret = sle_sample_set_uuid(uuid, &property.uuid);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse uuid set fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    property.permissions = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
    property.operate_indication = properties;
    property.value_len = len;
    property.value = data;
    return ssaps_add_property_sync(g_server_id, service_hdl->handle_in, &property, &service_hdl->handle_out);
}

static errcode_t sle_remote_control_property_and_descriptor_add(void)
{
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX1],
        g_sle_hid_group_uuid[SLE_UUID_INDEX1], SLE_REMOTE_CONTROL_REPORT_LENGTH, g_sle_input_report,
        &g_service_hdl[SLE_UUID_INDEX1]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse add report fail, ret:%x, indet:%x\r\n", ret, SLE_UUID_INDEX1);
        return ERRCODE_SLE_FAIL;
    }
    app_print("[uuid server] sle mouse add report, proterty hdl:%x\r\n",
        g_service_hdl[SLE_UUID_INDEX1].handle_out);

    ret = sle_sample_add_descriptor_interface(g_hid_service_property[SLE_UUID_INDEX2],
        g_service_hdl[SLE_UUID_INDEX0].handle_out, g_service_hdl[SLE_UUID_INDEX1].handle_out, sizeof(g_cccd), g_cccd);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse add cccd fail, ret:%x, indet:%x\r\n", ret, SLE_UUID_INDEX2);
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
        osal_printk("[uuid server] sle mouse add report ref fail, ret:%x, indet:%x\r\n", ret, SLE_UUID_INDEX3);
        return ERRCODE_SLE_FAIL;
    }

    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX4],
        g_sle_hid_group_uuid[SLE_UUID_INDEX4], sizeof(g_sle_report_map_datas), g_sle_report_map_datas,
        &g_service_hdl[SLE_UUID_INDEX4]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse add report map ref fail, ret:%x, indet:%x\r\n", ret,
            SLE_UUID_INDEX4);
        return ERRCODE_SLE_FAIL;
    }

    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX5],
        g_sle_hid_group_uuid[SLE_UUID_INDEX5], sizeof(uint8_t), &g_sle_hid_control_point,
        &g_service_hdl[SLE_UUID_INDEX5]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse add hid ctrl point fail, ret:%x, indet:%x\r\n", ret,
            SLE_UUID_INDEX5);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_rcu_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = { 0 };

    app_uuid.len = sizeof(g_sle_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_uuid_app_uuid, sizeof(g_sle_uuid_app_uuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_server_id);
    sle_ota_service_init(g_server_id);
    if (sle_uuid_server_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_uuid_server_property_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    app_print("%s sle rcu add service, server_id:%x, service_handle:%x, property_handle:%x\r\n",
              SLE_RCU_SERVER_LOG, g_server_id, g_service_handle, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ERRCODE_SLE_FAIL;
    }
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[1]%s sle rcu add service fail, ret:%x\r\n", SLE_RCU_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    // add HID service
    if (sle_remote_control_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }

    if (sle_remote_control_property_and_descriptor_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }

    app_print("[2]%s sle rcu add service, server_id:%x, service_handle:%x, property_handle:%x\r\n",
              SLE_RCU_SERVER_LOG, g_server_id, g_service_hdl[SLE_UUID_INDEX0].handle_out, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_hdl[SLE_UUID_INDEX0].handle_out);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle rcu add HID service fail, ret:%x\r\n", SLE_RCU_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }

    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_rcu_amic_server_add(void)
{
    errcode_t ret;
    if (sle_uuid_amic_server_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_uuid_amic_server_property_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    app_print("%s sle rcu add service, server_id:%x, service_handle:%x, property_handle:%x\r\n",
              SLE_RCU_SERVER_LOG, g_server_id, g_service_amic_handle, g_amic_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_amic_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

/* device通过uuid向host发送数据：report */
errcode_t sle_rcu_server_send_report_by_uuid(const uint8_t *data, uint8_t len, uint16_t conn_id)
{
    errcode_t ret;
    ssaps_ntf_ind_by_uuid_t param = { 0 };
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.start_handle = g_service_handle;
    param.end_handle = g_property_handle;
    param.value_len = len;
    param.value = (uint8_t *)osal_vmalloc(len);
    if (param.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        osal_vfree(param.value);
        return ERRCODE_SLE_FAIL;
    }
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &param.uuid);
    ret = ssaps_notify_indicate_by_uuid(g_server_id, conn_id, &param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        app_print("%s sle_rcu_server_send_report_by_uuid,ssaps_notify_indicate_by_uuid fail :%x\r\n",
                  SLE_RCU_SERVER_LOG, ret);
        osal_vfree(param.value);
        return ret;
    }
    osal_vfree(param.value);
    return ERRCODE_SLE_SUCCESS;
}

/* device通过handle向host发送数据：report */
errcode_t sle_rcu_server_send_report_by_handle(const uint8_t *data, uint8_t len, uint16_t conn_id)
{
    ssaps_ntf_ind_t param = { 0 };
    uint8_t receive_buf[RCU_SEND_BUFF_LENGTH] = { 0 }; /* max receive length. */
    param.handle = g_service_hdl[SLE_UUID_INDEX1].handle_out;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = receive_buf;
    param.value_len = len;
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    return ssaps_notify_indicate(g_server_id, conn_id, &param);
}

errcode_t sle_rcu_amic_server_send_report_by_handle(uint8_t *data, uint8_t len, uint16_t conn_id)
{
    ssaps_ntf_ind_t param = { 0 };
    param.handle = g_amic_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = data;
    param.value_len = len;
    ssaps_notify_indicate(g_server_id, conn_id, &param);
    return ERRCODE_SLE_SUCCESS;
}

static void sle_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr, sle_acb_state_t conn_state,
                                          sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    unused(pair_state);
    unused(disc_reason);
    app_print("%s connect state changed callback conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, \
              disc_reason:0x%x\r\n", SLE_RCU_SERVER_LOG, conn_id, conn_state, pair_state, disc_reason);
    app_print("%s connect state changed callback addr:%02x:**:**:**:%02x:%02x\r\n", SLE_RCU_SERVER_LOG,
              addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4]);
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        g_sle_conn_id = conn_id;
        g_sle_conn_handle[conn_id] = 1;
        g_sle_conn_num++;
        if (g_sle_conn_num < CONFIG_SLE_MULTICON_NUM) {
            sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
        }
        memcpy_s(&g_sle_addr, sizeof(sle_addr_t), addr, sizeof(sle_addr_t));
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
#if defined(CONFIG_RCU_MASS_PRODUCTION_TEST)
        if (get_rcu_status() == RCU_STATUS_TEST_NO_SLEPP) {
            rcu_out_of_production_testing();
        }
#endif
        g_sle_conn_handle[conn_id] = 0;
        g_sle_pair_handle = 0;
        g_sle_conn_num--;
        g_ssaps_ready = false;
        memset_s(&g_sle_addr, sizeof(sle_addr_t), 0, sizeof(sle_addr_t));
        if (!g_low_power_state) {
            sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
        }
    }
}

uint8_t *sle_low_latency_get_data_cbk(uint8_t *length, uint16_t *ssap_handle, uint8_t *data_type, uint16_t co_handle)
{
    unused(data_type);
    unused(co_handle);

    // buffer空
    if (g_read_buffer_node == g_write_buffer_node) {
        *length = 0;
        return NULL;
    }

    uint8_t *out_data1, *out_data2;
    uint32_t buffer_filled_count = 0;
    for (uint32_t i = 0; i < CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA; i++) {
        g_sle_pdm_buffer[buffer_filled_count++] =
            (uint8_t)(g_pdm_dma_data[g_read_buffer_node][i] >> SLE_VDT_MIC_OFFSET_16);
        g_sle_pdm_buffer[buffer_filled_count++] =
            (uint8_t)(g_pdm_dma_data[g_read_buffer_node][i] >> SLE_VDT_MIC_OFFSET_24);
    }

    g_read_buffer_node = (g_read_buffer_node + 1) % RING_BUFFER_NUMBER;
    buffer_filled_count = 0;
    uint32_t encode_data_len1 = sle_vdt_codec_encode(g_sle_pdm_buffer, &out_data1);
    uint32_t encode_data_len2 = sle_vdt_codec_encode(g_sle_pdm_buffer + CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA, &out_data2);

    if (memcpy_s(g_out_low_latency_data, LOW_LATENCY_DATA_MAX, out_data1, encode_data_len1) != EOK) {
        osal_printk("memcpy first part data fail.\r\n");
    }
    if (memcpy_s(g_out_low_latency_data + encode_data_len1, LOW_LATENCY_DATA_MAX - encode_data_len1, out_data2,
        encode_data_len2) != EOK) {
        osal_printk("memcpy second part data fail.\r\n");
    }

    *ssap_handle = SLE_RCU_SSAP_RPT_HANDLE;
    *length = encode_data_len1 + encode_data_len2;
    return g_out_low_latency_data;
}

void sle_set_em_data_cbk(uint16_t co_handle, uint8_t status)
{
    unused(status);
    unused(co_handle);

    app_print("pair sle_set_em_data_cbk\r\n");
}

void sle_low_latency_cbk_reg(void)
{
    sle_low_latency_callbacks_t cbks = {0};
    cbks.hid_data_cb = sle_low_latency_get_data_cbk;
    cbks.sle_set_em_data_cb = sle_set_em_data_cbk;
    sle_low_latency_register_callbacks(&cbks);
}

static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    unused(addr);
    unused(status);
    app_print("pair complete\r\n");
    g_sle_pair_handle = conn_id + 1;

    sle_low_latency_cbk_reg();
}

void sle_connect_param_update_cb(uint16_t conn_id, errcode_t status, const sle_connection_param_update_evt_t *param)
{
    unused(conn_id);
    unused(status);
    unused(param);
    osal_printk("sle_connect_param_update_cb interval_min %d*0.125 ms\r\n", param->interval);
    g_conn_update = 1;
}

static errcode_t sle_conn_register_cbks(void)
{
    errcode_t ret;
    sle_connection_callbacks_t conn_cbks = { 0 };
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;
    conn_cbks.connect_param_update_cb = sle_connect_param_update_cb;
    ret = sle_connection_register_callbacks(&conn_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_conn_register_cbks,sle_connection_register_callbacks fail :%x\r\n",
                    SLE_RCU_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

uint16_t sle_rcu_client_is_connected(void)
{
    return g_sle_pair_handle;
}

void sle_rcu_work_to_standby(void)
{
    if (g_sle_conn_num > 0) {
        g_low_power_state = true;
        g_work_to_standby.conn_id = g_sle_conn_id;
        sle_update_connect_param(&g_work_to_standby);
    }
}

void sle_rcu_standby_to_work(void)
{
    if (g_sle_conn_num > 0) {
        g_standby_to_work.conn_id = g_sle_conn_id;
        sle_update_connect_param(&g_standby_to_work);
    }
    g_low_power_state = false;
}

void sle_rcu_standby_to_sleep(void)
{
    if (g_sle_conn_num > 0) {
        g_low_power_state = true;
        sle_disconnect_remote_device(&g_sle_addr);
    } else if (g_sle_conn_num == 0) {
        sle_stop_announce(SLE_ADV_HANDLE_DEFAULT);
    }
}

void sle_rcu_sleep_to_work(void)
{
    sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    g_low_power_state = false;
}

/* 初始化uuid server */
errcode_t sle_rcu_server_init(ssaps_read_request_callback ssaps_read_callback,
                              ssaps_write_request_callback ssaps_write_callback)
{
    errcode_t ret;
    ret = sle_rcu_announce_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }
    ret = sle_conn_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }
    ret = sle_ssaps_register_cbks(ssaps_read_callback, ssaps_write_callback);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }
    ret = enable_sle();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_rcu_server_init,enable_sle fail :%x\r\n", SLE_RCU_SERVER_LOG, ret);
        return ret;
    }
    osal_printk("%s init ok\r\n", SLE_RCU_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}