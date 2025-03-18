/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE MICRO sample of client. \n
 *
 * History: \n
 * 2023-09-23, Create file. \n
 */

#include "securec.h"
#include "string.h"
#include "common_def.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "sle_device_manager.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "sle_errcode.h"
#include "sle_multi_conn_client.h"

#define SLE_MICRO_MULTINUM_ONE 1
#define SLE_MICRO_MULTINUM_TWO 2
#define SLE_ADDR_INDEX0 0
#define SLE_ADDR_INDEX1 1
#define SLE_ADDR_INDEX2 2
#define SLE_ADDR_INDEX3 3
#define SLE_ADDR_INDEX4 4
#define SLE_ADDR_INDEX5 5

#define MAC_ADDR_MOV_BIT_NUM_1 8
#define MAC_ADDR_MOV_BIT_NUM_2 16
#define MAC_ADDR_MOV_BIT_NUM_3 24
#define MAC_ADDR_MOV_BIT_NUM_4 32
#define MAC_ADDR_MOV_BIT_NUM_5 40

#define SERVER_INDEX0 0
#define SERVER_INDEX1 1
#define SERVER_INDEX2 2
#define SERVER_INDEX3 3
#define SERVER_INDEX4 4
#define SERVER_INDEX5 5
#define SERVER_INDEX6 6
#define SERVER_INDEX7 7

#define SLE_SEEK_INTERVAL_DEFAULT 100
#define SLE_SEEK_WINDOW_DEFAULT 50
#define SLE_MULTI_CONN_WAIT_SLE_CORE_READY_MS 1000
#define SLE_PARAMS_INTERVAL_MIN 0X20 /* 4ms --- 0.125ms */
#define SLE_PARAMS_INTERVAL_MAX 0X20 /* 4ms --- 0.125ms */
#define SLE_PARAMS_SUPERVISION_TIMEOUT_DEFAULT 0x1F4

static sle_announce_seek_callbacks_t g_sle_multi_conn_seek_cbk = {0};
static sle_connection_callbacks_t g_sle_multi_conn_connect_cbk = {0};
static ssapc_callbacks_t g_sle_multi_conn_ssapc_cbk = {0};

static sle_uuid_t g_client_uuid;
static uint8_t g_client_id;

uint8_t g_current_conn_index = CONFIG_SLE_MULTI_CONN_NUM;
uint8_t g_connected_num;

typedef struct sle_multicon_stru {
    uint8_t is_connected[CONFIG_SLE_MULTI_CONN_NUM];
    uint16_t conn_id[CONFIG_SLE_MULTI_CONN_NUM];
    uint16_t ssap_connect_state[CONFIG_SLE_MULTI_CONN_NUM];
    sle_addr_t server_addr[CONFIG_SLE_MULTI_CONN_NUM];
    uint8_t param_update_ready[CONFIG_SLE_MULTI_CONN_NUM];
    uint16_t conn_hdl[CONFIG_SLE_MULTI_CONN_NUM];
    sle_uuid_t uuid[CONFIG_SLE_MULTI_CONN_NUM];
    ssapc_find_service_result_t service[CONFIG_SLE_MULTI_CONN_NUM];
} sle_multi_conn_stru_t;
static sle_multi_conn_stru_t g_sle_multi_conn_param = {0};


uint16_t get_sle_multi_conn_conn_hdl(uint8_t server_index)
{
    return g_sle_multi_conn_param.conn_hdl[server_index];
}

uint16_t get_sle_multi_conn_conn_id(uint8_t server_index)
{
    return g_sle_multi_conn_param.conn_id[server_index];
}

uint16_t get_sle_multi_conn_client_id(void)
{
    return g_client_id;
}

uint8_t get_sle_multi_conn_param_update_ready(uint8_t server_index)
{
    return g_sle_multi_conn_param.param_update_ready[server_index];
}

uint8_t get_sle_multi_conn_server_index(uint16_t conn_id)
{
    for (uint8_t i = 0; i < CONFIG_SLE_MULTI_CONN_NUM; i++) {
        if (g_sle_multi_conn_param.is_connected[i] == 0) {
            continue;
        }
        if (memcmp(&conn_id, &g_sle_multi_conn_param.conn_id[i], sizeof(uint16_t)) == 0) {
            return i;
        }
    }
    return CONFIG_SLE_MULTI_CONN_NUM;
}

void sle_multi_conn_start_scan(void)
{
    sle_seek_param_t param = {0};
    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 1;
    param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;
    errcode_t ret = 0;
    ret |= sle_set_seek_param(&param);
    ret |= sle_start_seek();
    osal_printk("%s sle_start_seek,status:[%x]\r\n", SLE_MULTI_CONN_CLIENT_LOG, ret);
}

static void sle_multi_conn_client_sample_seek_enable_cbk(errcode_t status)
{
    osal_printk("%s seek_enable_cbk,status: [%x]\r\n", SLE_MULTI_CONN_CLIENT_LOG, status);
}

static uint8_t sle_multi_conn_find_connected_server_by_addr(const uint8_t *server_addr)
{
    uint8_t i;
    for (i = 0; i < CONFIG_SLE_MULTI_CONN_NUM; i++) {
        if (g_sle_multi_conn_param.is_connected[i] == 0) {
            continue;
        }
        if (memcmp(server_addr, g_sle_multi_conn_param.server_addr[i].addr, SLE_ADDR_LEN) == 0) {
            return i;
        }
    }
    return CONFIG_SLE_MULTI_CONN_NUM;
}

static uint8_t sle_multi_conn_find_unconnect_server_by_addr(const uint8_t *server_addr)
{
    uint8_t i;
    for (i = 0; i < CONFIG_SLE_MULTI_CONN_NUM; i++) {
        if (g_sle_multi_conn_param.is_connected[i] == 1) {
            continue;
        }
        if (memcmp(server_addr, g_sle_multi_conn_param.server_addr[i].addr, SLE_ADDR_LEN) == 0) {
            return i;
        }
    }
    return CONFIG_SLE_MULTI_CONN_NUM;
}

static void sle_multi_conn_client_sample_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    if (seek_result_data == NULL || seek_result_data->data == NULL) {
        return;
    }
    
    uint8_t find_connect_server = sle_multi_conn_find_unconnect_server_by_addr(seek_result_data->addr.addr);
    if (find_connect_server < CONFIG_SLE_MULTI_CONN_NUM) {
        // 停止扫描,回调执行连接device/server,将current_connect_index置为当前扫描到的server
        g_current_conn_index = find_connect_server;
        sle_stop_seek();
    } else {
    }
}

static void sle_multi_conn_client_sample_seek_disable_cbk(errcode_t status)
{
    unused(status);
    // 停止扫描后,回调执行连接current_conn_index代表的device/server
    if (g_current_conn_index < CONFIG_SLE_MULTI_CONN_NUM) {
        sle_connect_remote_device(&g_sle_multi_conn_param.server_addr[g_current_conn_index]);
    }
}

static void sle_multi_conn_client_sample_seek_cbk_register(void)
{
    // 注册sle扫描回调
    g_sle_multi_conn_seek_cbk.seek_enable_cb = sle_multi_conn_client_sample_seek_enable_cbk;
    g_sle_multi_conn_seek_cbk.seek_result_cb = sle_multi_conn_client_sample_seek_result_info_cbk;
    g_sle_multi_conn_seek_cbk.seek_disable_cb = sle_multi_conn_client_sample_seek_disable_cbk;
    sle_announce_seek_register_callbacks(&g_sle_multi_conn_seek_cbk);
}

static void sle_multi_conn_client_sample_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                                                   sle_acb_state_t conn_state,
                                                                   sle_pair_state_t pair_state,
                                                                   sle_disc_reason_t disc_reason)
{
    osal_printk("%s conn state changed: conn_id:%x, connect_state:%x, pair_state:%x, disc_reason:%x,\r\n \
                server addr:[%02x:%02x:%02x:%02x:%02x:%02x]\r\n",
                SLE_MULTI_CONN_CLIENT_LOG, conn_id, conn_state, pair_state, disc_reason, addr->addr[SLE_ADDR_INDEX0],
                addr->addr[SLE_ADDR_INDEX1], addr->addr[SLE_ADDR_INDEX2], addr->addr[SLE_ADDR_INDEX3],
                addr->addr[SLE_ADDR_INDEX4], addr->addr[SLE_ADDR_INDEX5]);
    
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        uint8_t server_index;
        server_index = sle_multi_conn_find_unconnect_server_by_addr(addr->addr);
        sle_pair_remote_device(&g_sle_multi_conn_param.server_addr[server_index]);
        // 连接后，更新server连接状态
        g_sle_multi_conn_param.is_connected[server_index] = 1;
        // 连接后，更新server与client连接ID
        g_sle_multi_conn_param.conn_id[server_index] = conn_id;
        // 连接后，更新client连接的server数量
        g_connected_num++;
        g_current_conn_index = CONFIG_SLE_MULTI_CONN_NUM;
        if (g_connected_num < CONFIG_SLE_MULTI_CONN_NUM) {
            sle_multi_conn_start_scan();
        }
    } else if (conn_state == SLE_ACB_STATE_NONE) {
        osal_printk("%s SLE_ACB_STATE_NONE\r\n", SLE_MULTI_CONN_CLIENT_LOG);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        uint8_t server_index = 0;
        server_index = sle_multi_conn_find_connected_server_by_addr(addr->addr);
        if (server_index == CONFIG_SLE_MULTI_CONN_NUM) {
            sle_multi_conn_start_scan();
            return;
        }

        // 断连后,更新server连接状态
        g_sle_multi_conn_param.is_connected[server_index] = 0;
        // 断连后,更新server与client连接ID
        g_sle_multi_conn_param.conn_id[server_index] = 0;
        // 连接后，更新client连接的server数量
        g_connected_num--;
        g_current_conn_index = CONFIG_SLE_MULTI_CONN_NUM;
        g_sle_multi_conn_param.param_update_ready[server_index] = 0;

        sle_remove_paired_remote_device(&g_sle_multi_conn_param.server_addr[server_index]);
        sle_multi_conn_start_scan();
    } else {
    }
}

void sle_multi_conn_client_sample_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    unused(conn_id);
    unused(addr);
    unused(status);
    osal_printk("%s pair complete\n", SLE_MULTI_CONN_CLIENT_LOG);
    if (status == 0) {
        ssap_exchange_info_t info = {0};
        info.mtu_size = CONFIG_SLE_MTU_LENGTH;
        info.version = 1;
        ssapc_exchange_info_req(g_client_id, conn_id, &info);
    }
}

static void sle_multi_conn_client_param_update_req_cbk(uint16_t conn_id, errcode_t status,
                                                       const sle_connection_param_update_req_t *param)
{
    unused(conn_id);
    unused(status);
    unused(param);
    osal_printk("%s param_update_req_cbk conn_id:0x%02x, interval_min:0x%x, interval_max:0x%x\r\n",
                SLE_MULTI_CONN_CLIENT_LOG,
                conn_id,
                param->interval_min,
                param->interval_max);
}

static void sle_multi_conn_client_param_update_cbk(uint16_t conn_id, errcode_t status,
                                                   const sle_connection_param_update_evt_t *param)
{
    unused(conn_id);
    unused(status);
    unused(param);
    
    uint8_t server_index = get_sle_multi_conn_server_index(conn_id);
    if (server_index == CONFIG_SLE_MULTI_CONN_NUM) {
        return;
    }
    g_sle_multi_conn_param.param_update_ready[server_index] = 1;
}

static void sle_multi_conn_client_sample_connect_cbk_register(void)
{
    g_sle_multi_conn_connect_cbk.connect_state_changed_cb = sle_multi_conn_client_sample_connect_state_changed_cbk;
    g_sle_multi_conn_connect_cbk.connect_param_update_req_cb = sle_multi_conn_client_param_update_req_cbk;
    g_sle_multi_conn_connect_cbk.connect_param_update_cb = sle_multi_conn_client_param_update_cbk;
    g_sle_multi_conn_connect_cbk.pair_complete_cb = sle_multi_conn_client_sample_pair_complete_cbk;
    sle_connection_register_callbacks(&g_sle_multi_conn_connect_cbk);
}

static void sle_multi_conn_client_sample_exchange_info_cbk(uint8_t client_id, uint16_t conn_id,
                                                           ssap_exchange_info_t *param, errcode_t status)
{
    osal_printk("%s exchange_info_cbk, client id: %x status: %x mtu size: %x, version: %x.\r\n",
                SLE_MULTI_CONN_CLIENT_LOG,
                client_id,
                status,
                param->mtu_size,
                param->version);
    
    ssapc_find_structure_param_t find_param = {0};
    find_param.type = SSAP_FIND_TYPE_PROPERTY;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(client_id, conn_id, &find_param);
}

static void sle_multi_conn_client_sample_find_structure_cbk(uint8_t client_id, uint16_t conn_id,
                                                            ssapc_find_service_result_t *service, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(service);
    unused(status);
    osal_printk("%s find_structure_cbk cbk client: %x, conn_id: %x\r\n",
                SLE_MULTI_CONN_CLIENT_LOG,
                client_id,
                conn_id);
}

static void sle_multi_conn_client_sample_find_property_cbk(uint8_t client_id, uint16_t conn_id,
                                                           ssapc_find_property_result_t *property, errcode_t status)
{
    unused(status);
    unused(conn_id);
    unused(property);
    unused(status);
    osal_printk("%s find_property_cbk client_id: [%x], conn_id: [%x], status: [%x] handle: [%x]\r\n",
                SLE_MULTI_CONN_CLIENT_LOG,
                client_id,
                conn_id,
                status,
                property->handle);
    uint8_t server_index = 0;
    server_index = get_sle_multi_conn_server_index(conn_id);
    if (server_index == CONFIG_SLE_MULTI_CONN_NUM) {
        return;
    }
    // 更新server与client连接handle
    g_sle_multi_conn_param.conn_hdl[server_index] = property->handle;
}

static void sle_multi_conn_client_sample_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
                                                                ssapc_find_structure_result_t *structure_result,
                                                                errcode_t status)
{
    unused(conn_id);
    unused(client_id);
    unused(structure_result);
    unused(status);
    osal_printk("%s find_structure_cmp_cbk client_id: [%x], conn_id: [%x], status: [%x], type: [%x], uuid: [%x]\r\n",
                SLE_MULTI_CONN_CLIENT_LOG,
                client_id,
                conn_id,
                status,
                structure_result->type,
                structure_result->uuid.uuid);

    uint8_t server_index = 0;
    server_index = get_sle_multi_conn_server_index(conn_id);
    if (server_index == CONFIG_SLE_MULTI_CONN_NUM) {
        return;
    }
    // 更新server与client连接handle
    g_sle_multi_conn_param.uuid[server_index] = structure_result->uuid;

    sle_connection_param_update_t params = {0};
    params.conn_id = conn_id;
    params.interval_min = SLE_PARAMS_INTERVAL_MIN;
    params.interval_max = SLE_PARAMS_INTERVAL_MAX;
    params.max_latency = 0;
    params.supervision_timeout = SLE_PARAMS_SUPERVISION_TIMEOUT_DEFAULT;
    sle_update_connect_param(&params);
}

static void sle_multi_conn_client_sample_ssapc_write_cfm_cbk(uint8_t client_id,
                                                             uint16_t conn_id,
                                                             ssapc_write_result_t *write_result,
                                                             errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(write_result);
    unused(status);
    uint8_t server_index = 0;
    server_index = get_sle_multi_conn_server_index(conn_id);
    if (server_index == CONFIG_SLE_MULTI_CONN_NUM) {
        return;
    }
    osal_printk("%s write cfm cbk. server_index: %x conn_id: %x\r\n",
                SLE_MULTI_CONN_CLIENT_LOG,
                server_index,
                conn_id);
}

static void sle_multi_conn_client_sample_ssapc_read_cfm_cbk(uint8_t client_id,
                                                            uint16_t conn_id,
                                                            ssapc_handle_value_t *read_data,
                                                            errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(read_data);
    unused(status);
    uint8_t server_index = 0;
    server_index = get_sle_multi_conn_server_index(conn_id);
    if (server_index == CONFIG_SLE_MULTI_CONN_NUM) {
        return;
    }
    osal_printk("%s ssapc_read_cfm_cbk. server_index: %x,conn_id: %x\r\n",
                SLE_MULTI_CONN_CLIENT_LOG,
                server_index,
                conn_id);
}

static void sle_multi_conn_client_sample_ssapc_cbk_register(ssapc_notification_callback notification_cb,
                                                            ssapc_notification_callback indication_cb)
{
    g_sle_multi_conn_ssapc_cbk.exchange_info_cb = sle_multi_conn_client_sample_exchange_info_cbk;
    g_sle_multi_conn_ssapc_cbk.find_structure_cb = sle_multi_conn_client_sample_find_structure_cbk;
    g_sle_multi_conn_ssapc_cbk.ssapc_find_property_cbk = sle_multi_conn_client_sample_find_property_cbk;
    g_sle_multi_conn_ssapc_cbk.find_structure_cmp_cb = sle_multi_conn_client_sample_find_structure_cmp_cbk;
    g_sle_multi_conn_ssapc_cbk.write_cfm_cb = sle_multi_conn_client_sample_ssapc_write_cfm_cbk;
    g_sle_multi_conn_ssapc_cbk.read_cfm_cb = sle_multi_conn_client_sample_ssapc_read_cfm_cbk;
    g_sle_multi_conn_ssapc_cbk.notification_cb = notification_cb;
    g_sle_multi_conn_ssapc_cbk.indication_cb = indication_cb;
    ssapc_register_callbacks(&g_sle_multi_conn_ssapc_cbk);
}

static void sle_client_target_server_addr_init(uint8_t server_index, uint64_t mac_addr)
{
    g_sle_multi_conn_param.server_addr[server_index].addr[SLE_ADDR_INDEX0] =
        (uint8_t)(((mac_addr >> MAC_ADDR_MOV_BIT_NUM_5) & 0xff));
    g_sle_multi_conn_param.server_addr[server_index].addr[SLE_ADDR_INDEX1] =
        (uint8_t)(((mac_addr >> MAC_ADDR_MOV_BIT_NUM_4) & 0xff));
    g_sle_multi_conn_param.server_addr[server_index].addr[SLE_ADDR_INDEX2] =
        (uint8_t)(((mac_addr >> MAC_ADDR_MOV_BIT_NUM_3) & 0xff));
    g_sle_multi_conn_param.server_addr[server_index].addr[SLE_ADDR_INDEX3] =
        (uint8_t)(((mac_addr >> MAC_ADDR_MOV_BIT_NUM_2) & 0xff));
    g_sle_multi_conn_param.server_addr[server_index].addr[SLE_ADDR_INDEX4] =
        (uint8_t)(((mac_addr >> MAC_ADDR_MOV_BIT_NUM_1) & 0xff));
    g_sle_multi_conn_param.server_addr[server_index].addr[SLE_ADDR_INDEX5] =
        (uint8_t)(mac_addr & 0xff);
}

static void sle_client_target_server_init(void)
{
    uint64_t config_addr = 0x0;
#ifdef CONFIG_SLE_MULTI_CONN_SERVER0_ADDR
    config_addr = CONFIG_SLE_MULTI_CONN_SERVER0_ADDR;
    sle_client_target_server_addr_init(SERVER_INDEX0, config_addr);
#endif
#ifdef CONFIG_SLE_MULTI_CONN_SERVER1_ADDR
    config_addr = CONFIG_SLE_MULTI_CONN_SERVER1_ADDR;
    sle_client_target_server_addr_init(SERVER_INDEX1, config_addr);
#endif
#ifdef CONFIG_SLE_MULTI_CONN_SERVER2_ADDR
    config_addr = CONFIG_SLE_MULTI_CONN_SERVER2_ADDR;
    sle_client_target_server_addr_init(SERVER_INDEX2, config_addr);
#endif
#ifdef CONFIG_SLE_MULTI_CONN_SERVER3_ADDR
    config_addr = CONFIG_SLE_MULTI_CONN_SERVER3_ADDR;
    sle_client_target_server_addr_init(SERVER_INDEX3, config_addr);
#endif
#ifdef CONFIG_SLE_MULTI_CONN_SERVER4_ADDR
        config_addr = CONFIG_SLE_MULTI_CONN_SERVER4_ADDR;
    sle_client_target_server_addr_init(SERVER_INDEX4, config_addr);
#endif
#ifdef CONFIG_SLE_MULTI_CONN_SERVER5_ADDR
    config_addr = CONFIG_SLE_MULTI_CONN_SERVER5_ADDR;
    sle_client_target_server_addr_init(SERVER_INDEX5, config_addr);
#endif
#ifdef CONFIG_SLE_MULTI_CONN_SERVER6_ADDR
    config_addr = CONFIG_SLE_MULTI_CONN_SERVER6_ADDR;
    sle_client_target_server_addr_init(SERVER_INDEX6, config_addr);
#endif
#ifdef CONFIG_SLE_MULTI_CONN_SERVER7_ADDR
    config_addr = CONFIG_SLE_MULTI_CONN_SERVER7_ADDR;
    sle_client_target_server_addr_init(SERVER_INDEX7, config_addr);
#endif
    uint8_t i;
    for (i = 0; i < CONFIG_SLE_MULTI_CONN_NUM; i++) {
        osal_printk("%s server%x mac address: [%02x:%02x:%02x:%02x:%02x:%02x]\r\n",
                    SLE_MULTI_CONN_CLIENT_LOG,
                    i,
                    g_sle_multi_conn_param.server_addr[i].addr[SLE_ADDR_INDEX0],
                    g_sle_multi_conn_param.server_addr[i].addr[SLE_ADDR_INDEX1],
                    g_sle_multi_conn_param.server_addr[i].addr[SLE_ADDR_INDEX2],
                    g_sle_multi_conn_param.server_addr[i].addr[SLE_ADDR_INDEX3],
                    g_sle_multi_conn_param.server_addr[i].addr[SLE_ADDR_INDEX4],
                    g_sle_multi_conn_param.server_addr[i].addr[SLE_ADDR_INDEX5]);
    }
}

void sle_multi_conn_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    osal_printk("%s notification_cb status: [%x] data : %s, handle : %x, client_id : %x, conn_id : %x\r\n",
                SLE_MULTI_CONN_CLIENT_LOG,
                status,
                data->data,
                data->handle,
                client_id,
                conn_id);
}

void sle_multi_conn_write_req(uint8_t server_index)
{
    uint16_t conn_id = g_sle_multi_conn_param.conn_id[server_index];
    uint8_t req_data[MULTI_CONN_SEND_BUFF_MAX_LENGTH] = {'a', 'b', 'c', 'd'};
    uint8_t len = sizeof(req_data);
    ssapc_write_param_t param = {0};
    param.handle = g_sle_multi_conn_param.conn_hdl[server_index];
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.data_len = len;
    param.data = req_data;
    ssapc_write_req(g_client_id, conn_id, &param);
    osal_printk("%s write_req. server_index: %x, data: %s\r\n",
                SLE_MULTI_CONN_CLIENT_LOG, server_index, req_data);
}

void sle_multi_conn_read_req(uint8_t server_index)
{
    uint16_t conn_id = g_sle_multi_conn_param.conn_id[server_index];
    uint16_t handle = g_sle_multi_conn_param.conn_hdl[server_index];
    uint8_t type = SSAP_PROPERTY_TYPE_VALUE;
    ssapc_read_req(g_client_id, conn_id, handle, type);
    osal_printk("%s ssapc_read_req. server_index: %x\r\n",
                SLE_MULTI_CONN_CLIENT_LOG, server_index);
}

void sle_multi_conn_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    osal_printk("%s sle multi_conn client sle_multi_conn_indication_cb status: [%x]\r\n \
                recived data : %s, client_id : %x, conn_id : %x\r\n",
                SLE_MULTI_CONN_CLIENT_LOG,
                status,
                data->data,
                client_id,
                conn_id);
}

void sle_multi_conn_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb)
{
    uint64_t config_addr = CONFIG_SLE_MULTI_CONN_CLIENT_ADDR;
    uint8_t local_addr[SLE_ADDR_LEN] = {
        (uint8_t)((config_addr >> MAC_ADDR_MOV_BIT_NUM_5) & 0xff),
        (uint8_t)((config_addr >> MAC_ADDR_MOV_BIT_NUM_4) & 0xff),
        (uint8_t)((config_addr >> MAC_ADDR_MOV_BIT_NUM_3) & 0xff),
        (uint8_t)((config_addr >> MAC_ADDR_MOV_BIT_NUM_2) & 0xff),
        (uint8_t)((config_addr >> MAC_ADDR_MOV_BIT_NUM_1) & 0xff),
        (uint8_t)(config_addr & 0xff)};

    sle_addr_t local_address;
    local_address.type = 0;
    (void)memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    // client地址设置
    sle_set_local_addr(&local_address);
    osal_printk("%s sle_set_local_addr: [%02x:%02x:%02x:%02x:%02x:%02x]\n",
                SLE_MULTI_CONN_CLIENT_LOG,
                local_address.addr[SLE_ADDR_INDEX0],
                local_address.addr[SLE_ADDR_INDEX1],
                local_address.addr[SLE_ADDR_INDEX2],
                local_address.addr[SLE_ADDR_INDEX3],
                local_address.addr[SLE_ADDR_INDEX4],
                local_address.addr[SLE_ADDR_INDEX5]);

    // server地址库设置
    sle_client_target_server_init();
    osal_printk("%s sle_client_tartget_server_init:\n", SLE_MULTI_CONN_CLIENT_LOG);
    //
    sle_multi_conn_client_sample_seek_cbk_register();
    osal_printk("%s sle_multi_conn_client_sample_seek_cbk_register\n", SLE_MULTI_CONN_CLIENT_LOG);

    ssapc_register_client(&g_client_uuid, &g_client_id);

    sle_multi_conn_client_sample_connect_cbk_register();
    osal_printk("%s sle_multi_conn_client_sample_connect_cbk_register\n", SLE_MULTI_CONN_CLIENT_LOG);

    sle_multi_conn_client_sample_ssapc_cbk_register(notification_cb, indication_cb);
    osal_printk("%s sle_multi_conn_client_sample_ssapc_cbk_register\n", SLE_MULTI_CONN_CLIENT_LOG);
}

static void sle_multi_conn_client_sample_sle_power_on_cbk(uint8_t status)
{
    unused(status);
    enable_sle();
    sle_remove_all_pairs();
}

static void sle_multi_conn_client_sample_sle_enable_cbk(uint8_t status)
{
    if (status == 0) {
        // client 初始化,设置地址和回调
        sle_multi_conn_client_init(sle_multi_conn_notification_cb, sle_multi_conn_indication_cb);
        // 开始扫描任务
        sle_multi_conn_start_scan();
    }
}

errcode_t sle_multi_conn_client_sample_dev_cbk_register(void)
{
    errcode_t ret = 0;
    sle_dev_manager_callbacks_t client_dev_mgr_cbk = {0};
    // 设备管理，上电回调
    client_dev_mgr_cbk.sle_power_on_cb = sle_multi_conn_client_sample_sle_power_on_cbk;
    // 设备管理，使能sle回调
    client_dev_mgr_cbk.sle_enable_cb = sle_multi_conn_client_sample_sle_enable_cbk;
    // 设备管理，注册回调
    ret = sle_dev_manager_register_callbacks(&client_dev_mgr_cbk);
    osal_printk("%s sle_dev_register_cbks,register_callbacks return :[%x]\r\n",
                SLE_MULTI_CONN_CLIENT_LOG, ret);
#if (CORE_NUMS < 2)
    enable_sle();
#endif
    return ERRCODE_SLE_SUCCESS;
}