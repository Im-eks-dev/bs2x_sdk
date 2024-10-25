/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: sle adv config for sle rcu server. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-21, Create file. \n
 */
#include "securec.h"
#include "errcode.h"
#include "osal_addr.h"
#include "osal_task.h"
#include "common_def.h"
#include "sle_common.h"
#include "sle_device_manager.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#include "sle_rcu_server.h"
#include "sle_rcu_server_adv.h"

/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MIN_DEFAULT                 0x64
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MAX_DEFAULT                 0x64
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MIN_DEFAULT              (0xC8 * 2)
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MAX_DEFAULT              (0xC8 * 2)
/* 超时时间5000ms，单位10ms */
#define SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT      0x1F4
/* 超时时间4990ms，单位10ms */
#define SLE_CONN_MAX_LATENCY                      0x1F3
/* 广播发送功率 */
#define SLE_ADV_TX_POWER                          10
/* 广播ID */
#define SLE_ADV_HANDLE_DEFAULT                    1
/* 定向广播ID */
#define SLE_ADV_HANDLE_DIRECTED                   2
/* 最大广播数据长度 */
#define SLE_ADV_DATA_LEN_MAX                      251
#define SLE_UART_TASK_DELAY_MS                    1000

static uint16_t sle_set_adv_local_name(uint8_t *adv_data, uint16_t max_len)
{
    uint8_t index = 0;
    uint8_t *local_name = (uint8_t *)CONFIG_SLE_MULTICON_SERVER_NAME;
    uint8_t local_name_len = (uint8_t)strlen((char *)CONFIG_SLE_MULTICON_SERVER_NAME);

    adv_data[index++] = local_name_len + 1;
    adv_data[index++] = SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
    if (memcpy_s(&adv_data[index], max_len - index, local_name, local_name_len) != EOK) {
        return 0;
    }
    return (uint16_t)index + local_name_len;
}

static uint16_t sle_set_adv_data(uint8_t *adv_data, uint16_t length)
{
    size_t len = 0;
    uint16_t idx = 0;

    len = sizeof(struct sle_adv_common_value);
    struct sle_adv_common_value adv_disc_level = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL,
        .value = SLE_ANNOUNCE_LEVEL_NORMAL,
    };
    if (memcpy_s(&adv_data[idx], length - idx, &adv_disc_level, len) != EOK) {
        return 0;
    }
    idx += len;

    len = sizeof(struct sle_adv_common_value);
    struct sle_adv_common_value adv_access_mode = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_ACCESS_MODE,
        .value = 0,
    };
    if (memcpy_s(&adv_data[idx], length - idx, &adv_access_mode, len) != EOK) {
        return 0;
    }
    idx += len;

    return idx;
}

static uint16_t sle_set_scan_response_data(uint8_t *scan_rsp_data, uint16_t length)
{
    uint16_t idx = 0;
    size_t scan_rsp_data_len = sizeof(struct sle_adv_common_value);

    struct sle_adv_common_value tx_power_level = {
        .length = scan_rsp_data_len - 1,
        .type = SLE_ADV_DATA_TYPE_TX_POWER_LEVEL,
        .value = SLE_ADV_TX_POWER,
    };
    if (memcpy_s(scan_rsp_data, length, &tx_power_level, scan_rsp_data_len) != EOK) {
        return 0;
    }
    idx += scan_rsp_data_len;

    /* set local name */
    idx += sle_set_adv_local_name(&scan_rsp_data[idx], length - idx);
    return idx;
}

static int sle_set_default_announce_param(void)
{
    sle_announce_param_t param = { 0 };
    uint8_t local_addr[SLE_ADDR_LEN] = { CONFIG_SLE_MULTICON_SERVER_ADDR0, CONFIG_SLE_MULTICON_SERVER_ADDR1,
                                         CONFIG_SLE_MULTICON_SERVER_ADDR2, CONFIG_SLE_MULTICON_SERVER_ADDR3,
                                         CONFIG_SLE_MULTICON_SERVER_ADDR4, CONFIG_SLE_MULTICON_SERVER_ADDR5 };
    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = SLE_ADV_HANDLE_DEFAULT;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_channel_map = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announce_interval_max = SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.conn_interval_min = SLE_CONN_INTV_MIN_DEFAULT;
    param.conn_interval_max = SLE_CONN_INTV_MAX_DEFAULT;
    param.conn_max_latency = SLE_CONN_MAX_LATENCY;
    param.conn_supervision_timeout = SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT;
    param.own_addr.type = 0;
    if (memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN) != EOK) {
        return 0;
    }
    sle_addr_t local_address;
    local_address.type = 0;
    (void)memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    sle_set_local_addr(&local_address);
    return sle_set_announce_param(param.announce_handle, &param);
}

static int sle_set_default_announce_data(void)
{
    errcode_t ret;
    uint8_t announce_data_len = 0;
    uint8_t seek_data_len = 0;
    sle_announce_data_t data = { 0 };
    uint8_t adv_handle = SLE_ADV_HANDLE_DEFAULT;
    uint8_t announce_data[SLE_ADV_DATA_LEN_MAX] = { 0 };
    uint8_t seek_rsp_data[SLE_ADV_DATA_LEN_MAX] = { 0 };

    announce_data_len = sle_set_adv_data(announce_data, SLE_ADV_DATA_LEN_MAX);
    data.announce_data = announce_data;
    data.announce_data_len = announce_data_len;

    seek_data_len = sle_set_scan_response_data(seek_rsp_data, SLE_ADV_DATA_LEN_MAX);
    data.seek_rsp_data = seek_rsp_data;
    data.seek_rsp_data_len = seek_data_len;

    ret = sle_set_announce_data(adv_handle, &data);
    if (ret == ERRCODE_SLE_SUCCESS) {
        sample_print("%s set announce data success.\r\n", SLE_RCU_SERVER_LOG);
    } else {
        sample_print("%s set adv param fail.\r\n", SLE_RCU_SERVER_LOG);
    }
    return ERRCODE_SLE_SUCCESS;
}

static int sle_set_directed_announce_param(sle_addr_t *addr)
{
    sle_announce_param_t param = { 0 };
    uint8_t local_addr[SLE_ADDR_LEN] = { CONFIG_SLE_MULTICON_SERVER_ADDR0, CONFIG_SLE_MULTICON_SERVER_ADDR1,
                                         CONFIG_SLE_MULTICON_SERVER_ADDR2, CONFIG_SLE_MULTICON_SERVER_ADDR3,
                                         CONFIG_SLE_MULTICON_SERVER_ADDR4, CONFIG_SLE_MULTICON_SERVER_ADDR5 };
    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_DIRECTED;
    param.announce_handle = SLE_ADV_HANDLE_DIRECTED;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announce_interval_max = SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.conn_interval_min = SLE_CONN_INTV_MIN_DEFAULT;
    param.conn_interval_max = SLE_CONN_INTV_MAX_DEFAULT;
    param.conn_max_latency = SLE_CONN_MAX_LATENCY;
    param.conn_supervision_timeout = SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT;
    param.own_addr.type = 0;
    if (memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN) != EOK) {
        return 0;
    }
    param.peer_addr.type = 0;
    if (memcpy_s(param.peer_addr.addr, SLE_ADDR_LEN, addr->addr, SLE_ADDR_LEN) != EOK) {
        return 0;
    }
    sle_addr_t local_address;
    local_address.type = 0;
    (void)memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    sle_set_local_addr(&local_address);
    return sle_set_announce_param(param.announce_handle, &param);
}

static void sle_set_directed_announce_data(void)
{
    errcode_t ret;
    uint8_t announce_data_len = 0;
    uint8_t seek_data_len = 0;
    sle_announce_data_t data = { 0 };
    uint8_t adv_handle = SLE_ADV_HANDLE_DIRECTED;
    uint8_t announce_data[SLE_ADV_DATA_LEN_MAX] = { 0 };
    uint8_t seek_rsp_data[SLE_ADV_DATA_LEN_MAX] = { 0 };

    announce_data_len = sle_set_adv_data(announce_data, SLE_ADV_DATA_LEN_MAX);
    data.announce_data = announce_data;
    data.announce_data_len = announce_data_len;

    seek_data_len = sle_set_scan_response_data(seek_rsp_data, SLE_ADV_DATA_LEN_MAX);
    data.seek_rsp_data = seek_rsp_data;
    data.seek_rsp_data_len = seek_data_len;

    ret = sle_set_announce_data(adv_handle, &data);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s set adv param fail.\r\n", SLE_RCU_SERVER_LOG);
    }
}

static void sle_announce_enable_cbk(uint32_t announce_id, errcode_t status)
{
    sample_print("%s sle announce enable callback id:%02x, state:%x\r\n", SLE_RCU_SERVER_LOG, announce_id,
                 status);
}

static void sle_announce_disable_cbk(uint32_t announce_id, errcode_t status)
{
    sample_print("%s sle announce disable callback id:%02x, state:%x\r\n", SLE_RCU_SERVER_LOG, announce_id,
                 status);
}

static void sle_announce_terminal_cbk(uint32_t announce_id)
{
    sample_print("%s sle announce terminal callback id:%02x\r\n", SLE_RCU_SERVER_LOG, announce_id);
}

static void sle_enable_cbk(uint8_t status)
{
    if (sle_rcu_server_add() != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_rcu_server_init,sle_rcu_server_add fail\r\n", SLE_RCU_SERVER_LOG);
        return;
    }
    if (sle_rcu_amic_server_add() != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_rcu_amic_server_init,sle_rcu_server_add fail\r\n", SLE_RCU_SERVER_LOG);
        return;
    }
    sample_print("%s sle enable callback status:%x\r\n", SLE_RCU_SERVER_LOG, status);
}

errcode_t sle_rcu_announce_register_cbks(void)
{
    errcode_t ret = 0;
    sle_dev_manager_callbacks_t dev_cbks = { 0 };
    sle_announce_seek_callbacks_t seek_cbks = { 0 };
    seek_cbks.announce_enable_cb = sle_announce_enable_cbk;
    seek_cbks.announce_disable_cb = sle_announce_disable_cbk;
    seek_cbks.announce_terminal_cb = sle_announce_terminal_cbk;
    dev_cbks.sle_enable_cb = sle_enable_cbk;
    ret |= sle_dev_manager_register_callbacks(&dev_cbks);
    ret |= sle_announce_seek_register_callbacks(&seek_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_rcu_announce_register_cbks,register_callbacks fail :%x\r\n",
                     SLE_RCU_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_rcu_server_adv_init(void)
{
    errcode_t ret;
    sle_set_default_announce_param();
    sle_set_default_announce_data();
    ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_rcu_server_adv_init,sle_start_announce fail :%x\r\n", SLE_RCU_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_rcu_server_directed_adv_init(sle_addr_t *addr)
{
    errcode_t ret;
    sle_set_directed_announce_param(addr);
    sle_set_directed_announce_data();
    ret = sle_start_announce(SLE_ADV_HANDLE_DIRECTED);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_rcu_server_adv_init,sle_start_announce fail :%x\r\n", SLE_RCU_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}