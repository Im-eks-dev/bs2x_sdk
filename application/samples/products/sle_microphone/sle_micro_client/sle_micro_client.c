/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE MICRO sample of client. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-23, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "securec.h"
#include "bts_le_gap.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_device_manager.h"
#include "sle_errcode.h"
#include "sle_low_latency.h"
#include "sle_micro_client.h"

#define SLE_MICRO_MULTINUM_ONE 1
#define SLE_MICRO_MULTINUM_TWO 2
#define SLE_MICRO_MULTINUM_THREE 3
#define SLE_MICRO_MULTINUM_FOUR 4

#define SLE_ADDR_INDEX0 0
#define SLE_ADDR_INDEX1 1
#define SLE_ADDR_INDEX2 2
#define SLE_ADDR_INDEX3 3
#define SLE_ADDR_INDEX4 4
#define SLE_ADDR_INDEX5 5

#define SLE_MTU_SIZE_DEFAULT CONFIG_SLE_MTU_LENGTH
#define SLE_SEEK_INTERVAL_DEFAULT 160
#define SLE_SEEK_WINDOW_DEFAULT 160

#define SLE_MICRO_LOW_LATENCY_1K 1000

#define SLE_MICRO_CLIENT_LOG "[sle micro client]"

static ssapc_find_service_result_t g_sle_micro_find_service_result = {0};
static sle_dev_manager_callbacks_t g_sle_dev_mgr_cbk = {0};
static sle_announce_seek_callbacks_t g_sle_micro_seek_cbk = {0};
static sle_connection_callbacks_t g_sle_micro_connect_cbk = {0};
static ssapc_callbacks_t g_sle_micro_ssapc_cbk = {0};
static sle_addr_t g_sle_micro_remote_addr = {0};
static ssapc_write_param_t g_sle_micro_send_param = {0};
static uint16_t g_sle_micro_conn_id = 0;

// 已连接设备数量
static uint8_t connected_num;
typedef struct sle_multicon_stru {
    // 当前操作设备数组索引标识
    uint8_t current_connect;
    // 地址索引被占用标识
    uint8_t is_connected[CONFIG_SLE_MULTICON_NUM];
    // Server 端 Connect Id
    uint16_t conn_id[CONFIG_SLE_MULTICON_NUM];
    // Server 端 Address
    uint8_t addr[CONFIG_SLE_MULTICON_NUM][SLE_ADDR_LEN];
} sle_multicon_stru_t;
static sle_multicon_stru_t sle_multicon_param = {0};

uint16_t get_sle_micro_conn_id(void)
{
    return g_sle_micro_conn_id;
}

ssapc_write_param_t *get_sle_micro_send_param(void)
{
    return &g_sle_micro_send_param;
}

void sle_micro_start_scan(void)
{
    uint16_t number;
    sle_get_paired_devices_num(&number);
    osal_printk("%s sle_micro_start_scan, pair num = %d\r\n", SLE_MICRO_CLIENT_LOG, number);
    sle_seek_param_t param = {0};
    param.own_addr_type = 0;
    param.filter_duplicates = 1;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 0;
    param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;
    sle_set_seek_param(&param);
    sle_start_seek();
}

static void sle_micro_client_sample_sle_power_on_cbk(uint8_t status)
{
    osal_printk("sle power on cbk: %d.\r\n", status);
}

static void sle_micro_client_sample_sle_enable_cbk(uint8_t status)
{
    if (status != ERRCODE_SUCC) {
        osal_printk("%s sle_micro_client_sample_sle_enable_cbk,status error\r\n", SLE_MICRO_CLIENT_LOG);
    } else {
        uint8_t local_addr[SLE_ADDR_LEN] = {CONFIG_SLE_MULTICON_CLIENT_ADDR0, CONFIG_SLE_MULTICON_CLIENT_ADDR1,
                                            CONFIG_SLE_MULTICON_CLIENT_ADDR2, CONFIG_SLE_MULTICON_CLIENT_ADDR3,
                                            CONFIG_SLE_MULTICON_CLIENT_ADDR4, CONFIG_SLE_MULTICON_CLIENT_ADDR5};
        sle_addr_t local_address;
        local_address.type = 0;
        memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
        if (sle_set_local_addr(&local_address) != ERRCODE_SUCC) {
            osal_printk("%s set local addr error!\r\n", SLE_MICRO_CLIENT_LOG);
            return;
        }
        sle_micro_start_scan();
    }
}

static void sle_micro_client_sample_seek_enable_cbk(errcode_t status)
{
    if (status != ERRCODE_SUCC) {
        osal_printk("%s sle_micro_client_sample_seek_enable_cbk,status error\r\n", SLE_MICRO_CLIENT_LOG);
    }
}

static uint8_t sle_micro_find_unconnect_server_by_addr(uint8_t *server_addr)
{
    uint8_t i;
    for (i = 0; i < CONFIG_SLE_MULTICON_NUM; i++) {
        if (sle_multicon_param.is_connected[i] == 1) {
            continue;
        }

        if (memcmp(server_addr, sle_multicon_param.addr[i], SLE_ADDR_LEN) == 0) {
            return i;
        }
    }
    return i;
}

static uint8_t sle_micro_find_connected_server_by_addr(const uint8_t *server_addr)
{
    uint8_t i;
    for (i = 0; i < CONFIG_SLE_MULTICON_NUM; i++) {
        if (sle_multicon_param.is_connected[i] == 0) {
            continue;
        }

        if (memcmp(server_addr, sle_multicon_param.addr[i], SLE_ADDR_LEN) == 0) {
            return i;
        }
    }
    return i;
}

static void sle_micro_client_sample_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    uint8_t idx = 0;
    if (seek_result_data == NULL || seek_result_data->data == NULL) {
        osal_printk("seek_result_data error!\r\n");
    } else if ((idx = sle_micro_find_unconnect_server_by_addr(seek_result_data->addr.addr)) < CONFIG_SLE_MULTICON_NUM) {
        sle_multicon_param.current_connect = idx;
        (void)memcpy_s(&g_sle_micro_remote_addr, sizeof(sle_addr_t), &seek_result_data->addr, sizeof(sle_addr_t));
        sle_stop_seek();
    }
}

static void sle_micro_client_sample_seek_disable_cbk(errcode_t status)
{
    if (status != ERRCODE_SUCC) {
        osal_printk("%s sle_micro_client_sample_seek_disable_cbk,status error\r\n", SLE_MICRO_CLIENT_LOG);
    } else {
        sle_remove_paired_remote_device(&g_sle_micro_remote_addr);
        sle_connect_remote_device(&g_sle_micro_remote_addr);
    }
}

static void sle_micro_client_sample_seek_cbk_register(void)
{
    g_sle_dev_mgr_cbk.sle_power_on_cb = sle_micro_client_sample_sle_power_on_cbk;
    g_sle_dev_mgr_cbk.sle_enable_cb = sle_micro_client_sample_sle_enable_cbk;
    g_sle_micro_seek_cbk.seek_enable_cb = sle_micro_client_sample_seek_enable_cbk;
    g_sle_micro_seek_cbk.seek_result_cb = sle_micro_client_sample_seek_result_info_cbk;
    g_sle_micro_seek_cbk.seek_disable_cb = sle_micro_client_sample_seek_disable_cbk;
    if (sle_dev_manager_register_callbacks(&g_sle_dev_mgr_cbk) != ERRCODE_BT_SUCCESS ||
        sle_announce_seek_register_callbacks(&g_sle_micro_seek_cbk) != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register ble_client_enable_cb failed\r\n", SLE_MICRO_CLIENT_LOG);
    }
}

static void sle_micro_client_sample_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                                              sle_acb_state_t conn_state, sle_pair_state_t pair_state,
                                                              sle_disc_reason_t disc_reason)
{
    osal_printk("%s conn state changed conn_id: 0x%x, conn_state:0x%x,disc_reason:0x%x ,pair_state:0x%x\r\n",
                SLE_MICRO_CLIENT_LOG, conn_id, conn_state, disc_reason, pair_state);
    osal_printk("%s conn state changed addr:0x%02x:xx:xx:xx:0x%02x:0x%02x\r\n", SLE_MICRO_CLIENT_LOG,
                addr->addr[SLE_ADDR_INDEX0], addr->addr[SLE_ADDR_INDEX4], addr->addr[SLE_ADDR_INDEX5]);
    g_sle_micro_conn_id = conn_id;
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        sle_multicon_param.is_connected[sle_multicon_param.current_connect] = 1;
        sle_multicon_param.conn_id[sle_multicon_param.current_connect] = conn_id;
        
        if (pair_state == SLE_PAIR_NONE) {
            connected_num++;
            osal_printk("%s SLE_ACB_STATE_CONNECTED, connected_num = %d\r\n", SLE_MICRO_CLIENT_LOG, connected_num);
            sle_pair_remote_device(&g_sle_micro_remote_addr);
        }
    } else if (conn_state == SLE_ACB_STATE_NONE) {
        osal_printk("%s SLE_ACB_STATE_NONE\r\n", SLE_MICRO_CLIENT_LOG);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s SLE_ACB_STATE_DISCONNECTED\r\n", SLE_MICRO_CLIENT_LOG);
        uint8_t connected_server_id = 0;
        if ((connected_server_id = sle_micro_find_connected_server_by_addr(addr->addr)) < CONFIG_SLE_MULTICON_NUM) {
            osal_printk("%s disconneted addr:[0x%02x:0x%02x:xx:xx:xx:0x%02x],find connected server index =%d\r\n",
                        SLE_MICRO_CLIENT_LOG, sle_multicon_param.addr[connected_server_id][SLE_ADDR_INDEX0],
                        sle_multicon_param.addr[connected_server_id][SLE_ADDR_INDEX1],
                        sle_multicon_param.addr[connected_server_id][SLE_ADDR_INDEX5], connected_server_id);
            sle_multicon_param.is_connected[connected_server_id] = 0;
            connected_num--;
        }
        osal_printk("%s SLE_ACB_STATE_DISCONNECTED, connected_num = %d\r\n", SLE_MICRO_CLIENT_LOG, connected_num);
        if (connected_num < CONFIG_SLE_MULTICON_NUM) {
            sle_low_latency_set(conn_id, false, SLE_MICRO_LOW_LATENCY_1K);
            sle_micro_start_scan();
        }
    } else {
        osal_printk("%s status error\r\n", SLE_MICRO_CLIENT_LOG);
    }
}

static void sle_micro_client_param_update_req_cbk(uint16_t conn_id, errcode_t status,
                                                  const sle_connection_param_update_req_t *param)
{
    unused(conn_id);
    unused(param);
    osal_printk("%s sle_micro_client_param_update_req_cbk callback!, status:%d\r\n", SLE_MICRO_CLIENT_LOG, status);
}

static void sle_micro_client_param_update_cbk(uint16_t conn_id, errcode_t status,
                                              const sle_connection_param_update_evt_t *param)
{
    osal_printk("%s sle_micro_client_param_update_cbk, conn_id:%d status:%d supervision:0x%d \r\n",
                SLE_MICRO_CLIENT_LOG, conn_id, status, param->supervision);
}

static void sle_micro_client_auth_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status,
                                               const sle_auth_info_evt_t *evt)
{
    unused(addr);
    osal_printk("%s sle_multi_connection_auth_complete_cbk, conn_id:%d status:%d remote_id:%d\r\n",
                SLE_MICRO_CLIENT_LOG, conn_id, status, evt->key_deriv_algo);
}

static void sle_micro_client_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    unused(status);
    unused(addr);
    osal_printk("%s sle_multi_connection_pair_complete_cbk conn_id:%d \n", SLE_MICRO_CLIENT_LOG, conn_id);
    if (status == 0) {
        ssap_exchange_info_t info = {0};
        info.mtu_size = SLE_MTU_SIZE_DEFAULT;
        info.version = 1;
        ssapc_exchange_info_req(0, conn_id, &info);
    }
}

static void sle_micro_client_sample_connect_cbk_register(void)
{
    g_sle_micro_connect_cbk.connect_state_changed_cb = sle_micro_client_sample_connect_state_changed_cbk;
    g_sle_micro_connect_cbk.connect_param_update_req_cb = sle_micro_client_param_update_req_cbk;
    g_sle_micro_connect_cbk.connect_param_update_cb = sle_micro_client_param_update_cbk;
    g_sle_micro_connect_cbk.auth_complete_cb = sle_micro_client_auth_complete_cbk;
    g_sle_micro_connect_cbk.pair_complete_cb = sle_micro_client_pair_complete_cbk;
    sle_connection_register_callbacks(&g_sle_micro_connect_cbk);
}

static void sle_micro_client_sample_exchange_info_cbk(uint8_t client_id, uint16_t conn_id, ssap_exchange_info_t *param,
                                                      errcode_t status)
{
    osal_printk("%s exchange_info_cbk,pair complete client id:%d status:%d\r\n",
                SLE_MICRO_CLIENT_LOG, client_id, status);
    osal_printk("%s exchange mtu, mtu size: %d, version: %d.\r\n", SLE_MICRO_CLIENT_LOG,
                param->mtu_size, param->version);
    ssapc_find_structure_param_t find_param = {0};
    find_param.type = SSAP_FIND_TYPE_PROPERTY;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(0, conn_id, &find_param);
}

static void sle_micro_client_sample_find_structure_cbk(uint8_t client_id, uint16_t conn_id,
                                                       ssapc_find_service_result_t *service,
                                                       errcode_t status)
{
    osal_printk("%s find structure cbk client: %d conn_id:%d status: %d \r\n", SLE_MICRO_CLIENT_LOG,
                client_id, conn_id, status);
    osal_printk("%s find structure start_hdl:[0x%02x], end_hdl:[0x%02x], uuid len:%d\r\n", SLE_MICRO_CLIENT_LOG,
                service->start_hdl, service->end_hdl, service->uuid.len);
    g_sle_micro_find_service_result.start_hdl = service->start_hdl;
    g_sle_micro_find_service_result.end_hdl = service->end_hdl;
    (void)memcpy_s(&g_sle_micro_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t));
}

static void sle_micro_client_sample_find_property_cbk(uint8_t client_id, uint16_t conn_id,
                                                      ssapc_find_property_result_t *property, errcode_t status)
{
    osal_printk("%s sle_micro_client_sample_find_property_cbk, client id: %d, conn id: %d, operate ind: %d, "
                "descriptors count: %d status:%d property->handle %d\r\n",
                SLE_MICRO_CLIENT_LOG,
                client_id, conn_id, property->operate_indication,
                property->descriptors_count, status, property->handle);
    g_sle_micro_send_param.handle = property->handle;
    g_sle_micro_send_param.type = SSAP_PROPERTY_TYPE_VALUE;
    uint8_t data[] = {conn_id};
    uint8_t len = sizeof(data);
    ssapc_write_param_t param = {0};
    param.handle = property->handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.data_len = len;
    param.data = data;
    ssapc_write_req(0, conn_id, &param);
}

static void sle_micro_client_sample_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
                                                           ssapc_find_structure_result_t *structure_result,
                                                           errcode_t status)
{
    unused(conn_id);
    osal_printk("%s sle_micro_client_sample_find_structure_cmp_cbk,client id:%d status:%d type:%d uuid len:%d \r\n",
                SLE_MICRO_CLIENT_LOG, client_id, status, structure_result->type, structure_result->uuid.len);
    errcode_t ret;
    ret = sle_low_latency_set(get_sle_micro_conn_id(), true, SLE_MICRO_LOW_LATENCY_1K);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_low_latency_set fail ret:%d\r\n", SLE_MICRO_CLIENT_LOG, ret);
    }
    
    if (connected_num < CONFIG_SLE_MULTICON_NUM) {
        sle_micro_start_scan();
    }
}

static void sle_micro_client_sample_ssapc_cbk_register(ssapc_notification_callback notification_cb,
                                                       ssapc_notification_callback indication_cb)
{
    g_sle_micro_ssapc_cbk.exchange_info_cb = sle_micro_client_sample_exchange_info_cbk;
    g_sle_micro_ssapc_cbk.find_structure_cb = sle_micro_client_sample_find_structure_cbk;
    g_sle_micro_ssapc_cbk.ssapc_find_property_cbk = sle_micro_client_sample_find_property_cbk;
    g_sle_micro_ssapc_cbk.find_structure_cmp_cb = sle_micro_client_sample_find_structure_cmp_cbk;

    g_sle_micro_ssapc_cbk.notification_cb = notification_cb;
    g_sle_micro_ssapc_cbk.indication_cb = indication_cb;
    ssapc_register_callbacks(&g_sle_micro_ssapc_cbk);
}

static void sle_client_tartget_server_init(void)
{
#if (CONFIG_SLE_MULTICON_NUM == SLE_MICRO_MULTINUM_ONE)
        sle_multicon_param.addr[0][SLE_ADDR_INDEX0] = CONFIG_SLE_MULTICON_SERVER1_ADDR0;
        sle_multicon_param.addr[0][SLE_ADDR_INDEX1] = CONFIG_SLE_MULTICON_SERVER1_ADDR1;
        sle_multicon_param.addr[0][SLE_ADDR_INDEX2] = CONFIG_SLE_MULTICON_SERVER1_ADDR2;
        sle_multicon_param.addr[0][SLE_ADDR_INDEX3] = CONFIG_SLE_MULTICON_SERVER1_ADDR3;
        sle_multicon_param.addr[0][SLE_ADDR_INDEX4] = CONFIG_SLE_MULTICON_SERVER1_ADDR4;
        sle_multicon_param.addr[0][SLE_ADDR_INDEX5] = CONFIG_SLE_MULTICON_SERVER1_ADDR5;
#elif (CONFIG_SLE_MULTICON_NUM == SLE_MICRO_MULTINUM_TWO)
        sle_multicon_param.addr[0][SLE_ADDR_INDEX0] = CONFIG_SLE_MULTICON_SERVER1_ADDR0;
        sle_multicon_param.addr[0][SLE_ADDR_INDEX1] = CONFIG_SLE_MULTICON_SERVER1_ADDR1;
        sle_multicon_param.addr[0][SLE_ADDR_INDEX2] = CONFIG_SLE_MULTICON_SERVER1_ADDR2;
        sle_multicon_param.addr[0][SLE_ADDR_INDEX3] = CONFIG_SLE_MULTICON_SERVER1_ADDR3;
        sle_multicon_param.addr[0][SLE_ADDR_INDEX4] = CONFIG_SLE_MULTICON_SERVER1_ADDR4;
        sle_multicon_param.addr[0][SLE_ADDR_INDEX5] = CONFIG_SLE_MULTICON_SERVER1_ADDR5;
#ifdef CONFIG_SLE_EXSIT_TWO_MULTICON_SERVER
        sle_multicon_param.addr[1][SLE_ADDR_INDEX0] = CONFIG_SLE_MULTICON_SERVER2_ADDR0;
        sle_multicon_param.addr[1][SLE_ADDR_INDEX1] = CONFIG_SLE_MULTICON_SERVER2_ADDR1;
        sle_multicon_param.addr[1][SLE_ADDR_INDEX2] = CONFIG_SLE_MULTICON_SERVER2_ADDR2;
        sle_multicon_param.addr[1][SLE_ADDR_INDEX3] = CONFIG_SLE_MULTICON_SERVER2_ADDR3;
        sle_multicon_param.addr[1][SLE_ADDR_INDEX4] = CONFIG_SLE_MULTICON_SERVER2_ADDR4;
        sle_multicon_param.addr[1][SLE_ADDR_INDEX5] = CONFIG_SLE_MULTICON_SERVER2_ADDR5;
#endif
#endif
}

void sle_micro_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb)
{
    osal_printk("%s sle_micro_client_init!\r\n", SLE_MICRO_CLIENT_LOG);
    sle_client_tartget_server_init();
    sle_micro_client_sample_seek_cbk_register();
    sle_micro_client_sample_connect_cbk_register();
    sle_micro_client_sample_ssapc_cbk_register(notification_cb, indication_cb);

    enable_sle();
}