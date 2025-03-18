/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE uart sample of client. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-03, Create file. \n
 */
#include "sle_measure_dis_client.h"
#include "sle_errcode.h"
#include "sle_common.h"
#include "sle_device_manager.h"
#include "sle_connection_manager.h"
#include "sle_device_manager.h"
#include "sle_device_discovery.h"
#include "sle_ssap_client.h"
#include "sle_hadm_manager.h"
#include "cmsis_os2.h"
#include "sle_measure_dis_client_slem.h"

uint8_t iq_msg_buff[sizeof(measure_ids_msg_t) + sizeof(measure_dis_stored_iq_data_t)];

#define BLE_SLE_TAG_TASK_DURATION_MS 10
#define ADV_TO_SERVER 2
#define SLEM_UUID_LEN 16
#define KEY_CONNECT_DELAY_TIME 500
#define SLEM_IQ_DATALEN 512

static uint8_t g_stack_enable_status = 0;

uint8_t g_measure_dis_server_addr[SLE_ADDR_LEN] = { 1, 1, 1, 1, 1, 1 };
uint8_t g_measure_dis_client_addr[SLE_ADDR_LEN] = { 2, 2, 2, 2, 2, 2 };
uint8_t g_measure_dis_conn_id = SLEM_CONNET_INVAILD;

measure_dis_server_data_t g_measure_dis_server_data = {
    .server_uuid = {0x11, 0x22, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .service_uuid = {0x11, 0x33, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    .property_uuid = {0x11, 0x44, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
};

errcode_t measure_dis_slem_set_param(uint16_t conn_id)
{
    sle_set_channel_sounding_param_ex_t param = {
        .acb_interval = 0,
        .con_anchor_num = 1,
        .cs_interval = 0,
        .freq_space = 0,
        .is_cs_param_chg = 0,
        .refresh_rate = 1,
    };
    errcode_t ret = sle_set_channel_sounding_param_ex(conn_id, &param);
    if ((ret != ERRCODE_SUCC) || (conn_id == SLEM_CONNET_INVAILD)) {
        osal_printk("slem set param failed, ret = %d.\n", ret);
    }
    osal_printk("slem set param finish. refresh_rate:%d \n", param.refresh_rate);
    return ERRCODE_SUCC;
}

errcode_t measure_dis_slem_set_enable(uint16_t conn_id)
{
    osal_printk("SLEM SET ENABLE conn_id:%x.\r\n", conn_id);
    errcode_t ret = sle_set_channel_sounding_enable(conn_id);
    /* 解决测距使能之后，马上断链导致没有底层回调 */
    if ((ret != ERRCODE_SUCC) || (conn_id == SLEM_CONNET_INVAILD)) {
        osal_printk("slem set enable failed, ret = %d.\n", ret);
    }
    return ERRCODE_SUCC;
}

static int measure_dis_client_discover_key(uint8_t type)
{
    uint8_t conn_id = g_measure_dis_conn_id;

    ssapc_find_structure_param_t param = {
        .type = type,
        .start_hdl = g_measure_dis_server_data.begin_hdl,
        .end_hdl = g_measure_dis_server_data.end_hdl,
        .reserve = 0
    };

    param.uuid.len = SLEM_UUID_LEN;
    if (type == SSAP_FIND_TYPE_PRIMARY_SERVICE) {
        if (memcpy_s(param.uuid.uuid, SLEM_UUID_LEN,
            g_measure_dis_server_data.service_uuid, SLEM_UUID_LEN) != EOK) {
            return ERRCODE_MEMCPY;
        }
    } else if (type == SSAP_FIND_TYPE_PROPERTY) {
        if (memcpy_s(param.uuid.uuid, SLEM_UUID_LEN,
            g_measure_dis_server_data.property_uuid, SLEM_UUID_LEN) != EOK) {
            return ERRCODE_MEMCPY;
        }
    }

    osal_printk("SLE START CLIENT DISCOVER SERVER. conn_id:%d \r\n", conn_id);
    return ssapc_find_structure(0, conn_id, &param);
}

static void measure_dis_cm_conn_state_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                          sle_acb_state_t conn_state,
                                          sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    unused(pair_state);
    unused(addr);
    unused(disc_reason);
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("Connected. conn_id:%d\r\n", conn_id);
        g_measure_dis_conn_id = conn_id;
        measure_dis_client_discover_key(SSAP_FIND_TYPE_PROPERTY);
        ssap_exchange_info_t info = { 0 };
        info.mtu_size = SLEM_IQ_DATALEN;
        info.version = 1;
        ssapc_exchange_info_req(0, conn_id, &info);

        (void)osDelay(KEY_CONNECT_DELAY_TIME);
        measure_dis_slem_set_param(conn_id);
        (void)osDelay(KEY_CONNECT_DELAY_TIME);
        measure_dis_slem_set_enable(conn_id);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("Disconnected disc_reason:0x%x .\r\n", disc_reason);
        g_measure_dis_conn_id = SLEM_CONNET_INVAILD;
        measure_dis_start_scan();
    }
}

errcode_t measure_dis_cm_register_cbks(void)
{
    sle_connection_callbacks_t cm_cbks;
    cm_cbks.connect_state_changed_cb = measure_dis_cm_conn_state_cbk;
    cm_cbks.connect_param_update_req_cb = NULL;
    cm_cbks.connect_param_update_cb = NULL;
    cm_cbks.auth_complete_cb = NULL;
    cm_cbks.pair_complete_cb = NULL;
    cm_cbks.read_rssi_cb = NULL;

    return sle_connection_register_callbacks(&cm_cbks);
}

static void measure_dis_dd_enable_cbk(uint8_t status)
{
    osal_printk("sle enable result statue:0x%x .\r\n", status);
    g_stack_enable_status = 1;
}

static void measure_dis_dd_disenable_cbk(uint8_t status)
{
    osal_printk("sle disable result statue:0x%x .\r\n", status);
    g_stack_enable_status = 0;
}

static void measure_dis_dd_seek_result_cbk(sle_seek_result_info_t *seek_result_data)
{
    if (seek_result_data == NULL) {
        return;
    }

    if ((memcmp(g_measure_dis_server_addr, seek_result_data->addr.addr, SLE_ADDR_LEN) == 0)) {
        sle_stop_seek();
        sle_connect_remote_device(&(seek_result_data->addr));
    }
}

errcode_t measure_dis_dm_register_cbks(void)
{
    sle_dev_manager_callbacks_t dm_cbks = {
        .sle_power_on_cb = NULL,
        .sle_enable_cb = measure_dis_dd_enable_cbk,
        .sle_disable_cb = measure_dis_dd_disenable_cbk,
    };
    return sle_dev_manager_register_callbacks(&dm_cbks);
}

errcode_t measure_dis_dd_register_cbks(void)
{
    sle_announce_seek_callbacks_t dd_cbks  = {
        .announce_enable_cb = NULL,
        .announce_disable_cb = NULL,
        .announce_terminal_cb = NULL,
        .seek_enable_cb = NULL,
        .seek_disable_cb = NULL,
        .seek_result_cb = measure_dis_dd_seek_result_cbk,
    };
    return sle_announce_seek_register_callbacks(&dd_cbks);
}

STATIC void measure_dis_ssapc_find_structure_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_service_result_t *service, errcode_t status)
{
    osal_printk("[ssap client] find structure cbk client: %d conn_id:%d status: %d \n",
        client_id, conn_id, status);
    osal_printk("[ssap client] find structure cbk start_hdl: %d end_hdl:%d \n",
        service->start_hdl, service->end_hdl);

    g_measure_dis_server_data.begin_hdl= service->start_hdl;
    g_measure_dis_server_data.end_hdl = service->end_hdl;
}

STATIC void measure_dis_ssapc_find_property_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_property_result_t *property, errcode_t status)
{
    osal_printk("[ssap client] find property cbk client: %d conn_id:%d status: %d \n",
        client_id, conn_id, status);
    osal_printk("[ssap client] find property cbk handle: %d \n", property->handle);

    g_measure_dis_server_data.property_handle = property->handle;
}

STATIC void measure_dis_ssapc_find_structure_complete_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_structure_result_t *structure_result, errcode_t status)
{
    osal_printk("[ssap client] find structure complete cbk client: %d conn_id:%d status: %d \n",
                client_id, conn_id, status);
    if (structure_result == NULL) {
        return;
    }

    osal_printk("SERVICES DISCOVER COMPLETE. \r\n");
}

void measure_dis_client_msg_proc(uint8_t *data, uint16_t data_len)
{
    unused(data_len);
    uint32_t ret = ERRCODE_SLE_FAIL;
    measure_ids_msg_t *slem_profile_msg = (measure_ids_msg_t *)(data);

    switch (slem_profile_msg->type) {
        case SLEM_PROFILE_MSG_IQ:
            break;
        default:
            for (int i = 0; i < data_len; i++) {
                osal_printk("[%d]:%d\r\n", i, data[i]);
            }
            break;
    }

    if (unlikely(ret != ERRCODE_SLE_SUCCESS)) {
        osal_printk("client proc msg failed MSG_TYPE:%x ret:0x%x \r\n", slem_profile_msg->type, ret);
    }
}

/**
 * @brief  数据通知处理函数
 */
STATIC void measure_dis_ssapc_notification_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(status);
    osal_printk("[ssap client] notification info cbk client handle:%d, conn_id:%x, data_len:%x\n",
        data->handle, conn_id, data->data_len);

    measure_dis_client_msg_proc(data->data, data->data_len);
}

/**
 * @brief  数据指示处理函数
 */
STATIC void measure_dis_ssapc_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    osal_printk("[ssap client] indication info cbk client %d,mtu_size:%d, version:%x, status:%x\n",
        conn_id, data->handle, data->type, status);
}

/**
 * @brief  MTU信息交换响应处理函数
 */
STATIC void measure_dis_ssapc_exchange_info_cbk(uint8_t client_id, uint16_t conn_id, ssap_exchange_info_t *param,
    errcode_t status)
{
    unused(client_id);
    osal_printk("[ssap client] exchange info cbk client %d,mtu_size:%d, version:%x, status:%x\n",
        conn_id, param->mtu_size, param->version, status);
}

errcode_t measure_dis_ssapc_register_cbks(void)
{
    ssapc_callbacks_t ssapc_cbks;

    osal_printk("[ssap client] client register cbk\r\n");
    /* 服务发现模块回调函数 */
    ssapc_cbks.find_structure_cb = measure_dis_ssapc_find_structure_cbk;
    ssapc_cbks.ssapc_find_property_cbk = measure_dis_ssapc_find_property_cbk;
    ssapc_cbks.find_structure_cmp_cb = measure_dis_ssapc_find_structure_complete_cbk;
    ssapc_cbks.read_cfm_cb = NULL;
    ssapc_cbks.read_by_uuid_cmp_cb = NULL;
    ssapc_cbks.write_cfm_cb = NULL;
    ssapc_cbks.exchange_info_cb = measure_dis_ssapc_exchange_info_cbk;
    ssapc_cbks.notification_cb = measure_dis_ssapc_notification_cbk;
    ssapc_cbks.indication_cb = measure_dis_ssapc_indication_cb;
    return ssapc_register_callbacks(&ssapc_cbks);
}

errcode_t measure_dis_set_local_addr(uint8_t *addr)
{
    sle_addr_t sle_addr = {0};
    if (memcpy_s(sle_addr.addr, SLE_ADDR_LEN, addr, SLE_ADDR_LEN) != EOK) {
        return ERRCODE_MEMCPY;
    };
    return sle_set_local_addr(&sle_addr);
}

errcode_t measure_dis_set_local_name(uint8_t *name, uint8_t len)
{
    return sle_set_local_name(name, len);
}

void measure_dis_protocol_stack_init(uint8_t *addr, uint8_t *name)
{
    uint32_t ret = ERRCODE_SLE_SUCCESS;

    ret |= measure_dis_dm_register_cbks();
    ret |= measure_dis_cm_register_cbks();
    ret |= measure_dis_dd_register_cbks();
    ret = enable_sle();
    while (g_stack_enable_status != 1) {
        osal_msleep(BLE_SLE_TAG_TASK_DURATION_MS);
    }
    measure_dis_set_local_addr(addr);
    measure_dis_set_local_name(name, strlen((char *)name));
}

static int measure_dis_client_write_proc(uint8_t type, uint16_t len, uint8_t *data)
{
    errcode_t ret = ERRCODE_FAIL;
    ssapc_write_param_t param = {0};
    uint8_t conn_id = g_measure_dis_conn_id;

    param.handle = g_measure_dis_server_data.property_handle;
    param.type = type;
    param.data_len = len;
    param.data = data;

    ret = ssapc_write_cmd(0, conn_id, &param);
    if (ret != ERRCODE_SUCC) {
        osal_printk("CLIENT SEND (KEY) FAIL. cid:%d, handle:%d \r\n", conn_id, param.handle);
    }

    return ret;
}

int measure_dis_client_write_server(uint32_t type, uint8_t *data, uint32_t data_len)
{
    uint32_t ret = ERRCODE_SUCC;
    uint16_t len = sizeof(measure_ids_msg_t) + data_len;
    osal_printk("client send to server, len:0x%x.\r\n", len);
    measure_ids_msg_t *slem_msg = (measure_ids_msg_t *)osal_kmalloc(len, 0);
    if (slem_msg == NULL) {
        return ERRCODE_MALLOC;
    }
    slem_msg->type = type;
    slem_msg->len = data_len;
    if (memcpy_s(slem_msg->data, data_len, data, data_len) != EOK) {
        osal_kfree(slem_msg);
        return ERRCODE_MEMCPY;
    }
    ret = measure_dis_client_write_proc(0, len, (uint8_t *)(slem_msg));
    if (ret != ERRCODE_SUCC) {
        osal_printk("client send to server failed.\n");
    }
    osal_kfree(slem_msg);

    return ret;
}

errcode_t measure_dis_set_scan(void)
{
    errcode_t ret;
    sle_seek_param_t param = {0};
    param.own_addr_type = 0;
    param.filter_duplicates = 1;
    param.seek_filter_policy = 1;
    param.seek_phys = 1;
    param.seek_type[0] = 1;
    param.seek_interval[0] = 0x12c0;
    param.seek_window[0] = 0x500;
    ret = sle_set_seek_param(&param);

    return ret;
}

int measure_dis_start_scan(void)
{
    errcode_t ret;
    ret = measure_dis_set_scan();
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }
    ret = sle_start_seek();
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

int measure_dis_client_init(void)
{
    uint32_t ret = ERRCODE_SLE_SUCCESS;
    uint8_t measure_dis_name[SLE_NAME_MAX_LEN] = {'s', 'l', 'e', 'm', '-', 'c', '\0'};
    osal_printk("START CLIENT INIT .\r\n");
    measure_dis_protocol_stack_init(g_measure_dis_client_addr, measure_dis_name);
    measure_dis_reg_callbacks();
    ret = measure_dis_ssapc_register_cbks();
    measure_dis_start_scan();
    return ret;
}
/* 钥匙 */