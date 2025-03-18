/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: SLE OTA sample of client. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-02-01, Create file. \n
 */
#include "securec.h"
#include "string.h"
#include "common_def.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "bts_le_gap.h"
#include "bts_device_manager.h"
#include "gadget/f_hid.h"
#include "sle_device_manager.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "sle_ota_cmd.h"
#include "sle_ota_cmd_handler.h"
#include "sle_ota_client.h"

#define SLE_MICRO_MULTINUM_ONE                      1
#define SLE_MICRO_MULTINUM_TWO                      2
#define SLE_ADDR_INDEX0                             0
#define SLE_ADDR_INDEX1                             1
#define SLE_ADDR_INDEX2                             2
#define SLE_ADDR_INDEX3                             3
#define SLE_ADDR_INDEX4                             4
#define SLE_ADDR_INDEX5                             5

#define SLE_MTU_SIZE_DEFAULT                        CONFIG_SLE_MTU_LENGTH
#define SLE_SEEK_INTERVAL_DEFAULT                   100
#define SLE_SEEK_WINDOW_DEFAULT                     100
#define SLE_WAIT_SLE_CORE_READY_MS                  1000
#define SLE_SERVER_ADDR_RPT_LEN                     20
#define SLE_SERVER_ADDR_INDEX                       12
#define SLE_SERVER_DATA_RPT_LEN                     64
#define SLE_SERVER_DATA_INDEX                       12
#define SLE_SERVER_RPT_LEN_INDEX                    7
#define SLE_SERVER_DATA_LEN_INDEX                   10
#define SLE_SERVER_8_BIT_SHIFT                      8
#define SLE_OTA_DONGLE_LOG                          "[sle ota dongle]"

static uint16_t g_sle_ota_conn_id = 0;
static uint8_t g_ssap_find_ready = 0;
static uint8_t g_ssap_connect_param_update_ready = 0;
static uint8_t g_ssap_connect_state = 0;
static uint8_t g_sle_enable_status = 0;

static ssapc_find_service_result_t g_sle_ota_find_service_result = { 0 };
static sle_dev_manager_callbacks_t g_sle_dev_mgr_cbk = { 0 };
static sle_announce_seek_callbacks_t g_sle_ota_seek_cbk = { 0 };
static sle_connection_callbacks_t g_sle_ota_connect_cbk = { 0 };
static ssapc_callbacks_t g_sle_ota_ssapc_cbk = { 0 };
static sle_addr_t g_sle_ota_remote_addr = { 0 };
static ssapc_write_param_t g_sle_ota_send_param = { 0 };
static sle_acb_state_t g_sle_ota_conn_state = SLE_ACB_STATE_NONE;
static sle_pair_state_t g_sle_ota_pair_state = SLE_PAIR_NONE;

static bool g_addr_is_set = false;
static bool g_scan_state = false;
static sle_multicon_stru_t g_sle_multicon_param = { 0 };
static uint8_t g_sever_addr_rpt[SLE_SERVER_ADDR_RPT_LEN] = { SLE_LINK_FRAME_HEAD_FLAG, 0x0, 0x0, 0x0, 0x0,
                                                             0x01, 0x0C, 0x09, 0x00, 0x03, 0x06, 0x00 };

const sle_uuid_t g_sle_ota_property_uuid = {
    .len = 2,
    .uuid = {0x37, 0xbe, 0xa8, 0x80, 0xfc, 0x70, 0x11, 0xea, 0xb7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff},
};

uint16_t get_sle_ota_conn_id(void)
{
    return g_sle_ota_conn_id;
}
void set_sle_ota_set_address_state(bool state)
{
    g_addr_is_set = state;
}

bool get_sle_ota_set_address_state(void)
{
    return g_addr_is_set;
}

void set_sle_ota_scan_state(bool state)
{
    g_scan_state = state;
}

bool get_sle_ota_scan_state(void)
{
    return g_scan_state;
}

uint8_t get_sle_ota_conn_state(void)
{
    return g_sle_ota_conn_state;
}

sle_multicon_stru_t *get_sle_multicon_param(void)
{
    return &g_sle_multicon_param;
}

ssapc_write_param_t *get_sle_ota_send_param(void)
{
    return &g_sle_ota_send_param;
}

uint8_t get_g_sle_enable_status(void)
{
    return g_sle_enable_status;
}

uint8_t get_ssap_find_ready(void)
{
    return g_ssap_find_ready;
}
uint8_t get_ssap_connect_param_update_ready(void)
{
    return g_ssap_connect_param_update_ready;
}

void set_sle_ota_remote_addr(uint8_t *addr, uint16_t len)
{
    if (len != sizeof(sle_addr_t)) {
        return;
    }
    memcpy_s((uint8_t *)&g_sle_ota_remote_addr, sizeof(sle_addr_t), addr, sizeof(sle_addr_t));
}
 
void get_sle_remote_addr(sle_addr_t *addr)
{
    addr->type = g_sle_ota_remote_addr.type;
    memcpy_s(addr->addr, SLE_ADDR_LEN, g_sle_ota_remote_addr.addr, SLE_ADDR_LEN);
}

void sle_ota_start_scan(void)
{
    sle_seek_param_t param = { 0 };
    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 1;
    param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;
    sle_set_seek_param(&param);
    sle_start_seek();
}

static void sle_ota_client_sample_sle_enable_cbk(uint8_t status)
{
    osal_msleep(SLE_WAIT_SLE_CORE_READY_MS);
    if (status != 0) {
        osal_printk("%s sle_ota_client_sample_sle_enable_cbk,status error\r\n", SLE_OTA_DONGLE_LOG);
    } else {
        uint8_t local_addr[SLE_ADDR_LEN] = {CONFIG_SLE_MULTICON_CLIENT_ADDR0, CONFIG_SLE_MULTICON_CLIENT_ADDR1,
                                            CONFIG_SLE_MULTICON_CLIENT_ADDR2, CONFIG_SLE_MULTICON_CLIENT_ADDR3,
                                            CONFIG_SLE_MULTICON_CLIENT_ADDR4, CONFIG_SLE_MULTICON_CLIENT_ADDR5};
        sle_addr_t local_address;
        local_address.type = 0;
        (void)memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
        if (sle_set_local_addr(&local_address) != ERRCODE_SUCC) {
            osal_printk("%s set local addr error!\r\n", SLE_OTA_DONGLE_LOG);
            return;
        }
        osal_printk("%s enter callback of sle enable,start scan!\r\n", SLE_OTA_DONGLE_LOG);
        sle_ota_start_scan();
        g_sle_enable_status = 1;
    }
}

static void sle_ota_client_sample_seek_enable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_ota_client_sample_seek_enable_cbk,status error\r\n", SLE_OTA_DONGLE_LOG);
    }
}

static void sle_ota_client_sample_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    if (seek_result_data == NULL || seek_result_data->data == NULL) {
        osal_printk("seek_result_data error!\r\n");
        return;
    }
    if (memcmp(g_sle_ota_remote_addr.addr, seek_result_data->addr.addr, SLE_ADDR_LEN) == 0) {
        sle_connect_remote_device(&g_sle_ota_remote_addr);
    }
    if (get_sle_ota_scan_state() == true) {
        for (uint8_t i = 0; i < SLE_ADDR_LEN; i++) {
            g_sever_addr_rpt[SLE_SERVER_ADDR_INDEX + i] = (uint8_t)seek_result_data->addr.addr[i];
        }
        fhid_send_data(sle_ota_get_hid_index(), (char *)g_sever_addr_rpt, SLE_SERVER_ADDR_RPT_LEN);
    }
}

static void sle_ota_client_sample_seek_disable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_ota_client_sample_seek_disable_cbk,status error\r\n", SLE_OTA_DONGLE_LOG);
    }
}

static void sle_ota_client_sample_seek_cbk_register(void)
{
    g_sle_dev_mgr_cbk.sle_enable_cb = sle_ota_client_sample_sle_enable_cbk;
    g_sle_ota_seek_cbk.seek_enable_cb = sle_ota_client_sample_seek_enable_cbk;
    g_sle_ota_seek_cbk.seek_result_cb = sle_ota_client_sample_seek_result_info_cbk;
    g_sle_ota_seek_cbk.seek_disable_cb = sle_ota_client_sample_seek_disable_cbk;
    sle_dev_manager_register_callbacks(&g_sle_dev_mgr_cbk);
    sle_announce_seek_register_callbacks(&g_sle_ota_seek_cbk);
}

static void sle_ota_client_sample_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                                            sle_acb_state_t conn_state,
                                                            sle_pair_state_t pair_state,
                                                            sle_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);
    osal_printk("%s conn state changed,connect_state:%d, disc_reason:%d\r\n", SLE_OTA_DONGLE_LOG,
                conn_state, disc_reason);
    osal_printk("%s conn state changed addr:0x%02x:xx:xx:xx:0x%02x:0x%02x\r\n", SLE_OTA_DONGLE_LOG,
                addr->addr[SLE_ADDR_INDEX0], addr->addr[SLE_ADDR_INDEX4], addr->addr[SLE_ADDR_INDEX5]);
    g_sle_ota_conn_id = conn_id;
    g_sle_ota_conn_state = conn_state;
    g_sle_ota_pair_state = pair_state;
    sle_ota_get_conn_status(SERVICE_ID_SERVICE_CONNECT, COMMAND_ID_GET_CONN_STATUS, NULL, 0);
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("%s SLE_ACB_STATE_CONNECTED\r\n", SLE_OTA_DONGLE_LOG);
        g_ssap_connect_state = 1;
        sle_stop_seek();
        uint8_t role = 0;
        sle_get_connect_role(conn_id, &role);
        if (role == SLE_LINK_ROLE_G) {
            sle_pair_remote_device(addr);
            sle_connection_param_update_t params = { 0 };
            params.conn_id = conn_id;
            params.interval_min = 0x64;  /* 4 --- 0.5ms */
            params.interval_max = 0x64;
            params.max_latency = 0;
            params.supervision_timeout = 0x1F4;
            sle_update_connect_param(&params);
            osal_printk("sle_update_connect_param\r\n");
        } else {
            g_ssap_connect_param_update_ready = 1;
            ssap_exchange_info_t info = {0};
            info.mtu_size = SLE_MTU_SIZE_DEFAULT;
            info.version = 1;
            ssapc_exchange_info_req(1, g_sle_ota_conn_id, &info);
        }
    } else if (conn_state == SLE_ACB_STATE_NONE) {
        osal_printk("%s SLE_ACB_STATE_NONE\r\n", SLE_OTA_DONGLE_LOG);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s SLE_ACB_STATE_DISCONNECTED\r\n", SLE_OTA_DONGLE_LOG);
        g_ssap_connect_state = 0;
        (void)memcpy_s(&g_sle_ota_remote_addr, sizeof(sle_addr_t), addr, sizeof(sle_addr_t));
        set_sle_ota_scan_state(true);
        sle_ota_start_scan();
        g_ssap_find_ready = 0;
    } else {
    }
}

static void sle_ota_client_param_update_req_cbk(uint16_t conn_id, errcode_t status,
                                                const sle_connection_param_update_req_t *param)
{
    unused(conn_id);
    unused(status);
    unused(param);
    g_ssap_connect_param_update_ready = 1;
}

void  sle_ota_client_sample_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    unused(conn_id);
    unused(addr);
    unused(status);
    osal_printk("pair status %d\r\n", status);
    if (status == 0) {
        g_ssap_connect_param_update_ready = 1;
        ssap_exchange_info_t info = {0};
        info.mtu_size = SLE_MTU_SIZE_DEFAULT;
        info.version = 1;
        ssapc_exchange_info_req(1, conn_id, &info);
    }
}

static void sle_ota_client_sample_connect_cbk_register(void)
{
    g_sle_ota_connect_cbk.connect_state_changed_cb = sle_ota_client_sample_connect_state_changed_cbk;
    g_sle_ota_connect_cbk.connect_param_update_req_cb =  sle_ota_client_param_update_req_cbk;
    g_sle_ota_connect_cbk.pair_complete_cb =  sle_ota_client_sample_pair_complete_cbk;
    sle_connection_register_callbacks(&g_sle_ota_connect_cbk);
}

static void sle_ota_client_sample_exchange_info_cbk(uint8_t client_id, uint16_t conn_id,
                                                    ssap_exchange_info_t *param, errcode_t status)
{
    unused(conn_id);
    unused(client_id);
    unused(param);
    unused(status);
    osal_printk("exchange info status %d\r\n", status);
    ssapc_find_structure_param_t find_param = { 0 };
    find_param.type = SSAP_FIND_TYPE_PROPERTY;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(0, conn_id, &find_param);
}

static void sle_ota_client_sample_find_structure_cbk(uint8_t client_id, uint16_t conn_id,
                                                     ssapc_find_service_result_t *service, errcode_t status)
{
    unused(conn_id);
    unused(client_id);
    unused(service);
    unused(status);
    g_sle_ota_find_service_result.start_hdl = service->start_hdl;
    g_sle_ota_find_service_result.end_hdl = service->end_hdl;
    (void)memcpy_s(&g_sle_ota_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t));
}

static void sle_ota_client_sample_find_property_cbk(uint8_t client_id, uint16_t conn_id,
                                                    ssapc_find_property_result_t *property, errcode_t status)
{
    unused(conn_id);
    unused(client_id);
    unused(property);
    unused(status);
    if (memcmp(&g_sle_ota_property_uuid, &property->uuid, sizeof(sle_uuid_t)) == 0) {
        g_sle_ota_send_param.handle = property->handle;
        g_sle_ota_send_param.type = SSAP_PROPERTY_TYPE_VALUE;
        g_ssap_find_ready = 1;
        osal_printk("ota prop handle %02x\r\n", g_sle_ota_send_param.handle);
    }
}

static void sle_ota_client_sample_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
                                                         ssapc_find_structure_result_t *structure_result,
                                                         errcode_t status)
{
    unused(conn_id);
    unused(client_id);
    unused(structure_result);
    unused(status);
}

static void sle_ota_client_sample_ssapc_cbk_register(ssapc_notification_callback notification_cb,
                                                     ssapc_notification_callback indication_cb)
{
    g_sle_ota_ssapc_cbk.exchange_info_cb = sle_ota_client_sample_exchange_info_cbk;
    g_sle_ota_ssapc_cbk.find_structure_cb = sle_ota_client_sample_find_structure_cbk;
    g_sle_ota_ssapc_cbk.ssapc_find_property_cbk = sle_ota_client_sample_find_property_cbk;
    g_sle_ota_ssapc_cbk.find_structure_cmp_cb = sle_ota_client_sample_find_structure_cmp_cbk;
    g_sle_ota_ssapc_cbk.notification_cb = notification_cb;
    g_sle_ota_ssapc_cbk.indication_cb = indication_cb;
    ssapc_register_callbacks(&g_sle_ota_ssapc_cbk);
}

void ble_enable_cb(uint8_t status)
{
    osal_printk("%s enable status: %d\n", SLE_OTA_DONGLE_LOG, status);
}

void bt_core_enable_cbk_register(void)
{
    errcode_t ret;
    bts_dev_manager_callbacks_t dev_cb = {0};
    dev_cb.ble_enable_cb = ble_enable_cb;
    ret = bts_dev_manager_register_callbacks(&dev_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register ble_enable_cb failed\r\n", SLE_OTA_DONGLE_LOG);
    }
}

static void sle_client_tartget_server_init(void)
{
    switch (CONFIG_SLE_MULTICON_NUM) {
        case SLE_MICRO_MULTINUM_ONE:
            g_sle_multicon_param.addr[0][SLE_ADDR_INDEX0] = CONFIG_SLE_MULTICON_SERVER1_ADDR0;
            g_sle_multicon_param.addr[0][SLE_ADDR_INDEX1] = CONFIG_SLE_MULTICON_SERVER1_ADDR1;
            g_sle_multicon_param.addr[0][SLE_ADDR_INDEX2] = CONFIG_SLE_MULTICON_SERVER1_ADDR2;
            g_sle_multicon_param.addr[0][SLE_ADDR_INDEX3] = CONFIG_SLE_MULTICON_SERVER1_ADDR3;
            g_sle_multicon_param.addr[0][SLE_ADDR_INDEX4] = CONFIG_SLE_MULTICON_SERVER1_ADDR4;
            g_sle_multicon_param.addr[0][SLE_ADDR_INDEX5] = CONFIG_SLE_MULTICON_SERVER1_ADDR5;
            break;
        case SLE_MICRO_MULTINUM_TWO:
            g_sle_multicon_param.addr[0][SLE_ADDR_INDEX0] = CONFIG_SLE_MULTICON_SERVER1_ADDR0;
            g_sle_multicon_param.addr[0][SLE_ADDR_INDEX1] = CONFIG_SLE_MULTICON_SERVER1_ADDR1;
            g_sle_multicon_param.addr[0][SLE_ADDR_INDEX2] = CONFIG_SLE_MULTICON_SERVER1_ADDR2;
            g_sle_multicon_param.addr[0][SLE_ADDR_INDEX3] = CONFIG_SLE_MULTICON_SERVER1_ADDR3;
            g_sle_multicon_param.addr[0][SLE_ADDR_INDEX4] = CONFIG_SLE_MULTICON_SERVER1_ADDR4;
            g_sle_multicon_param.addr[0][SLE_ADDR_INDEX5] = CONFIG_SLE_MULTICON_SERVER1_ADDR5;
#if defined(CONFIG_SLE_EXSIT_TWO_MULTICON_SERVER)
            g_sle_multicon_param.addr[1][SLE_ADDR_INDEX0] = CONFIG_SLE_MULTICON_SERVER2_ADDR0;
            g_sle_multicon_param.addr[1][SLE_ADDR_INDEX1] = CONFIG_SLE_MULTICON_SERVER2_ADDR1;
            g_sle_multicon_param.addr[1][SLE_ADDR_INDEX2] = CONFIG_SLE_MULTICON_SERVER2_ADDR2;
            g_sle_multicon_param.addr[1][SLE_ADDR_INDEX3] = CONFIG_SLE_MULTICON_SERVER2_ADDR3;
            g_sle_multicon_param.addr[1][SLE_ADDR_INDEX4] = CONFIG_SLE_MULTICON_SERVER2_ADDR4;
            g_sle_multicon_param.addr[1][SLE_ADDR_INDEX5] = CONFIG_SLE_MULTICON_SERVER2_ADDR5;
#endif
            break;
        default:
            break;
    }
}

void sle_ota_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb)
{
    sle_client_tartget_server_init();
    bt_core_enable_cbk_register();
    sle_ota_client_sample_seek_cbk_register();
    sle_ota_client_sample_connect_cbk_register();
    sle_ota_client_sample_ssapc_cbk_register(notification_cb, indication_cb);
    enable_sle();
}
