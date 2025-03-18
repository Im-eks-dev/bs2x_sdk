/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: sle adv config for sle uart server. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-17, Create file. \n
 */
#include "sle_measure_dis_server.h"
#include "sle_errcode.h"
#include "sle_common.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_ssap_client.h"
#include "sle_measure_dis_server_adv.h"

typedef struct {
    uint8_t length;
    uint8_t type;
    uint8_t value;
} sle_adv_common_value;

errcode_t measure_dis_stop_adv(uint8_t announce_id)
{
    return sle_stop_announce(announce_id);
}

errcode_t measure_dis_start_adv(uint8_t announce_id)
{
    return sle_start_announce(announce_id);
}

static uint16_t sle_set_adv_local_name(uint8_t *adv_data, uint16_t max_len)
{
    uint8_t index = 0;
    uint8_t local_name[SLE_NAME_MAX_LEN];
    uint8_t local_name_len;

    sle_get_local_name(local_name, &local_name_len);

    adv_data[index++] = local_name_len + 1;
    adv_data[index++] = SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
    if (memcpy_s(&adv_data[index], max_len - index, local_name, local_name_len) != EOK) {
        return 0xFFFF;
    }

    return (uint16_t)index + local_name_len;
}

static uint16_t sle_set_adv_data(uint8_t *adv_data, uint8_t adv_data_len)
{
    size_t len = 0;
    uint16_t idx = 0;

    len = sizeof(sle_adv_common_value);
    sle_adv_common_value adv_disc_level = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL,
        .value = SLE_ANNOUNCE_LEVEL_NORMAL,
    };
    if (memcpy_s(&adv_data[idx], adv_data_len - idx, &adv_disc_level, len) != EOK) {
        return 0xFFFF;
    }
    idx += len;

    len = sizeof(sle_adv_common_value);
    sle_adv_common_value adv_access_mode = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_ACCESS_MODE,
        .value = 0,
    };
    if (memcpy_s(&adv_data[idx], adv_data_len - idx, &adv_access_mode, len) != EOK) {
        return 0xFFFF;
    }
    idx += len;
    return idx;
}

static uint16_t sle_set_scan_response_data(uint8_t *scan_rsp_data, uint8_t adv_data_len)
{
    uint16_t idx = 0;
    size_t scan_rsp_data_len = sizeof(sle_adv_common_value);

    sle_adv_common_value tx_power_level = {
        .length = scan_rsp_data_len - 1,
        .type = SLE_ADV_DATA_TYPE_TX_POWER_LEVEL,
        .value = SLE_ADV_TX_POWER,
    };
    if (memcpy_s(scan_rsp_data, adv_data_len, &tx_power_level, scan_rsp_data_len) != EOK) {
        return 0xFFFF;
    }
    idx += scan_rsp_data_len;

    /* set local name */
    idx += sle_set_adv_local_name(&scan_rsp_data[idx], adv_data_len - idx);
    return idx;
}

static errcode_t measure_dis_set_default_announce_param(uint8_t announce_id, sle_addr_t *sle_addr)
{
    sle_announce_param_t param = {0};
    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = announce_id;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_NO_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_ADV_INTERVAL_SLAVE_DEFAULT;
    param.announce_interval_max = SLE_ADV_INTERVAL_SLAVE_DEFAULT;
    param.conn_interval_min = 0x64;
    param.conn_interval_max = 0x64;
    param.conn_max_latency = 0;
    param.conn_supervision_timeout = 0x1F4;
    param.own_addr = *sle_addr;

    return sle_set_announce_param(param.announce_handle, &param);
}

static errcode_t measure_dis_set_default_announce_data(uint8_t announce_id)
{
    uint8_t announce_data_len = 0;
    uint8_t seek_data_len = 0;
    sle_announce_data_t data = {0};
    uint8_t adv_handle = announce_id;
    uint8_t announce_data[SLE_ADV_DATA_LEN_MAX] = {0};
    uint8_t seek_rsp_data[SLE_ADV_DATA_LEN_MAX] = {0};

    announce_data_len = sle_set_adv_data(announce_data, SLE_ADV_DATA_LEN_MAX);
    data.announce_data = announce_data;
    data.announce_data_len = announce_data_len;

    seek_data_len = sle_set_scan_response_data(seek_rsp_data, SLE_ADV_DATA_LEN_MAX);
    data.seek_rsp_data = seek_rsp_data;
    data.seek_rsp_data_len = seek_data_len;

    sle_set_announce_data(adv_handle, &data);

    return ERRCODE_SLE_SUCCESS;
}

errcode_t measure_dis_sle_set_adv(uint8_t announce_id, uint8_t *addr)
{
    sle_addr_t sle_addr = {0};
    if (memcpy_s(sle_addr.addr, SLE_ADDR_LEN, addr, SLE_ADDR_LEN) != EOK) {
        return ERRCODE_MEMCPY;
    }

    measure_dis_set_default_announce_param(announce_id, &sle_addr);
    measure_dis_set_default_announce_data(announce_id);

    return ERRCODE_SLE_SUCCESS;
}
