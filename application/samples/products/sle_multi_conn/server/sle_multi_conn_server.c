/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE MICRO sample of client. \n
 *
 * History: \n
 * 2023-09-23, Create file. \n
 */

#include "securec.h"
#include "common_def.h"
#include "osal_debug.h"
#include "osal_addr.h"
#include "osal_task.h"
#include "sle_errcode.h"
#include "sle_common.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_device_manager.h"
#include "sle_multi_conn_server_adv.h"
#include "sle_multi_conn_server.h"

#define OCTET_BIT_LEN 8
#define UUID_LEN_2 2
#define UUID_INDEX 14

#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16

#define SLE_MULTI_CONN_STATE_DISCONNECT 0
#define SLE_MULTI_CONN_STATE_CONNECTED 1
#define MULTI_CONN_SEND_BUFF_LENGTH 20


/* sle server app uuid */
static uint8_t g_sle_uuid_app_uuid[UUID_LEN_2] = {0x12, CONFIG_CURRENT_SERVER_INDEX};
/* server notify property uuid */
static uint8_t g_sle_property_value[OCTET_BIT_LEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
/* sle connect acb handle */
static uint16_t g_sle_conn_id = 0;
/* sle server handle */
static uint8_t g_server_id = CONFIG_CURRENT_SERVER_INDEX;
/* sle service handle */
static uint16_t g_service_handle = 0;
/* sle ntf property handle */
static uint16_t g_property_handle = 0;
/* sle pair acb handle */
static uint16_t g_sle_pair_handle;
/* sle ssp connected */
static bool g_sle_conned = false;
/* sle ssp connect paired */
static bool g_sle_paired = false;
/* sle ssp param updated */
static int g_conn_update = 0;

static uint8_t g_sle_multi_conn_base[] = {0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                          0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

int get_g_conn_update(void)
{
    return g_conn_update;
}

uint16_t get_g_sle_conn_hdl(void)
{
    return g_sle_conn_id;
}

uint16_t sle_multi_conn_client_is_connected(void)
{
    return g_sle_conned;
}

static void sle_uuid_set_base(sle_uuid_t *out)
{
    if (memcpy_s(out->uuid, SLE_UUID_LEN, g_sle_multi_conn_base, SLE_UUID_LEN) != EOK) {
        out->len = 0;
        return;
    }
    out->len = UUID_LEN_2;
}

static void encode2byte_little(uint8_t *_ptr, uint16_t data)
{
    *(uint8_t *)((_ptr) + 1) = (uint8_t)((data) >> 0x8);
    *(uint8_t *)(_ptr) = (uint8_t)(data);
}

static void sle_uuid_setu2(uint16_t u2, sle_uuid_t *out)
{
    sle_uuid_set_base(out);
    out->len = UUID_LEN_2;
    encode2byte_little(&out->uuid[UUID_INDEX], u2);
}

static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, ssap_exchange_info_t *mtu_size,
                                  errcode_t status)
{
    osal_printk("%s ssaps ssaps_mtu_changed_cbk callback server_id:%x, conn_id:%x, mtu_size:%x, status:%x\r\n",
                SLE_MULTI_CONN_SERVER_LOG, server_id, conn_id, mtu_size->mtu_size, status);
    g_sle_pair_handle = conn_id + 1;
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("%s start service cbk callback server_id:%x, handle:%x, status:%x\r\n",
                SLE_MULTI_CONN_SERVER_LOG, server_id, handle, status);
}

static void ssaps_add_service_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    unused(uuid);
    osal_printk("%s add service cbk callback server_id:%x, handle:%x, status:%x\r\n",
                SLE_MULTI_CONN_SERVER_LOG, server_id, handle, status);
}

static void ssaps_add_property_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t service_handle,
                                   uint16_t handle, errcode_t status)
{
    unused(uuid);
    osal_printk("%s add property cbk callback server_id:%x, service_handle:%x,handle:%x, status:%x\r\n",
                SLE_MULTI_CONN_SERVER_LOG, server_id, service_handle, handle, status);
}

static void ssaps_add_descriptor_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t service_handle,
                                     uint16_t property_handle, errcode_t status)
{
    unused(uuid);
    osal_printk("%s add descriptor cbk callback server_id:%x, service_handle:%x, property_handle:%x, \
                 status:%x\r\n",
                SLE_MULTI_CONN_SERVER_LOG, server_id, service_handle, property_handle, status);
}

static void ssaps_delete_all_service_cbk(uint8_t server_id, errcode_t status)
{
    osal_printk("%s delete all service callback server_id:%x, status:%x\r\n", SLE_MULTI_CONN_SERVER_LOG,
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
    osal_printk("%s sle_ssaps_register_cbks,ssaps_register_callbacks return: [%x]\r\n",
                SLE_MULTI_CONN_SERVER_LOG, ret);
    return ret;
}

/* 添加一个ssap服务 */
static errcode_t sle_uuid_server_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    sle_uuid_setu2(SLE_UUID_SERVER_SERVICE, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, true, &g_service_handle);
    osal_printk("%s sle_uuid_server_service_add,ssaps_add_service_sync return: [%x]\r\n",
                SLE_MULTI_CONN_SERVER_LOG, ret);
    return ret;
}

static errcode_t sle_uuid_server_property_add(void)
{
    errcode_t ret;
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};
    uint8_t ntf_value[] = {0x01, 0x00};
    property.permissions = SSAP_PERMISSION_READ_WRITE;
    property.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ_WRITE;
    property.value = (uint8_t *)osal_vmalloc(sizeof(g_sle_property_value));
    if (property.value == NULL) {
        osal_vfree(property.value);
        osal_printk("%s osal_vfree(property.value); \r\n", SLE_MULTI_CONN_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &property.uuid);
    if (memcpy_s(property.value, sizeof(g_sle_property_value), g_sle_property_value,
                 sizeof(g_sle_property_value)) != EOK) {
        osal_vfree(property.value);
        osal_printk("%s osal_vfree(property.value); \r\n", SLE_MULTI_CONN_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property, &g_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    descriptor.permissions = SSAP_PERMISSION_READ_WRITE;
    descriptor.type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ_WRITE;

    descriptor.value = (uint8_t *)osal_vmalloc(sizeof(ntf_value));
    if (descriptor.value == NULL) {
        osal_vfree(property.value);
        osal_printk("%s osal_vfree(property.value); \r\n", SLE_MULTI_CONN_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(descriptor.value, sizeof(ntf_value), ntf_value, sizeof(ntf_value)) != EOK) {
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        osal_printk("%s osal_vfree(escriptor.value property.value); \r\n", SLE_MULTI_CONN_SERVER_LOG);
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

/* 添加ssaps SLE Service Access Protocol SERVER */
errcode_t sle_multi_conn_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = {0};
    osal_printk("%s sle_multi_conn_server_add \r\n", SLE_MULTI_CONN_SERVER_LOG);
    app_uuid.len = sizeof(g_sle_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_uuid_app_uuid, sizeof(g_sle_uuid_app_uuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    if (ssaps_register_server(&app_uuid, &g_server_id) != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s ssaps_register_server failed. \r\n", SLE_MULTI_CONN_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }

    if (sle_uuid_server_service_add() != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_uuid_server_service_add failed. \r\n", SLE_MULTI_CONN_SERVER_LOG);
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_uuid_server_property_add() != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_uuid_server_property_add failed. \r\n", SLE_MULTI_CONN_SERVER_LOG);
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("%s sle multi_conn add service, server_id:%x, service_handle:%x, property_handle:%x\r\n",
                SLE_MULTI_CONN_SERVER_LOG, g_server_id, g_service_handle, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s ssaps_start_service failed. \r\n", SLE_MULTI_CONN_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_multi_conn_server_send_notify_indicate(void)
{
    ssaps_ntf_ind_t param = {0};
    // max receive length
    uint8_t receive_buf[MULTI_CONN_SEND_BUFF_LENGTH] = CONFIG_SLE_MULTI_CONN_SERVER_NAME;
    param.handle = g_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = receive_buf;
    param.value_len = MULTI_CONN_SEND_BUFF_LENGTH;

    errcode_t ret;
    ret = ssaps_notify_indicate(g_server_id, g_sle_conn_id, &param);
    osal_printk("%s sle_multi_conn_server_send_notify_indicate return: [%x]\r\n",
                SLE_MULTI_CONN_SERVER_LOG, ret);
    return ret;
}

static void sle_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr, sle_acb_state_t conn_state,
                                          sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    osal_printk("%s connect state changed callback conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, \
                disc_reason:0x%x\r\n, client mac addr:[%02x:%02x:%02x:%02x:%02x:%02x]\r\n", SLE_MULTI_CONN_SERVER_LOG,
                conn_id, conn_state, pair_state, disc_reason,
                addr->addr[SLE_ADDR_INDEX0], addr->addr[SLE_ADDR_INDEX1], addr->addr[SLE_ADDR_INDEX2],
                addr->addr[SLE_ADDR_INDEX3], addr->addr[SLE_ADDR_INDEX4], addr->addr[SLE_ADDR_INDEX5]);
    
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        g_sle_conn_id = conn_id;
        g_sle_conned = true;
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        g_sle_conn_id = 0;
        g_sle_conned = false;
        g_sle_paired = false;
        g_conn_update = 0;
        errcode_t ret;
        ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
        osal_printk("%s sle_connect_state_changed_cbk,sle_start_announce return: [%x]\r\n",
                    SLE_MULTI_CONN_SERVER_LOG, ret);
    }
}

static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    unused(conn_id);
    unused(addr);
    unused(status);
    osal_printk("%s sle_pair_complete_cbk pair complete\r\n", SLE_MULTI_CONN_SERVER_LOG);
    g_sle_paired = true;
}

void sle_connect_param_update_cb(uint16_t conn_id, errcode_t status, const sle_connection_param_update_evt_t *param)
{
    unused(conn_id);
    unused(status);
    unused(param);
    osal_printk("%s sle_connect_param_update_cb interval %x*0.125 ms\r\n", SLE_MULTI_CONN_SERVER_LOG, param->interval);
    g_conn_update = 1;
}

static errcode_t sle_conn_register_cbks(void)
{
    errcode_t ret;
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;
    conn_cbks.connect_param_update_cb = sle_connect_param_update_cb;
    ret = sle_connection_register_callbacks(&conn_cbks);
    osal_printk("%s sle_conn_register_cbks,sle_connection_register_callbacks return: [%x]\r\n",
                SLE_MULTI_CONN_SERVER_LOG, ret);
    return ret;
    return ERRCODE_SLE_SUCCESS;
}

/* 初始化uuid server */
errcode_t sle_multi_conn_server_init(ssaps_read_request_callback ssaps_read_callback,
                                     ssaps_write_request_callback ssaps_write_callback)
{
    errcode_t ret;

    ret = sle_multi_conn_announce_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_multi_conn_server_init,sle_multi_conn_announce_register_cbks fail :%x\r\n",
                    SLE_MULTI_CONN_SERVER_LOG, ret);
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

    osal_printk("%s init ok\r\n", SLE_MULTI_CONN_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}