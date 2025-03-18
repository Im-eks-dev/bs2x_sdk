/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 *
 * Description: adv config for ble uuid server. \n
 *
 * History: \n
 * 2024-04-21, Create file. \n
 */
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "osal_addr.h"
#include "securec.h"
#include "errcode.h"
#include "bts_def.h"
#include "bts_le_gap.h"
#include "ble_sle_status.h"
#include "ble_mouse_server_adv.h"

static uint8_t ble_set_adv_flag_data(uint8_t *set_adv_data_position, uint8_t max_len)
{
    errno_t n_ret;

    ble_adv_flag adv_flags = {
        .length = BLE_ADV_FLAG_LEN - BLE_GENERAL_BYTE_1,
        .adv_data_type = 1,
        .flags = BLE_ADV_FLAG_DATA,
    };
    if (get_ble_pair_flag() == NONE_PAIR) {
        adv_flags.flags = BLE_NO_SUPPORT_ADV_FLAG_DATA;
    }
    n_ret = memcpy_s(set_adv_data_position, max_len, &adv_flags, BLE_ADV_FLAG_LEN);
    if (n_ret != EOK) {
        return 0;
    }
    return BLE_ADV_FLAG_LEN;
}

static uint8_t ble_set_adv_appearance_mouse(uint8_t *set_adv_data_position, uint8_t max_len)
{
    errno_t n_ret;
    ble_appearance_t adv_appearance_data = {
        .length = BLE_ADV_APPEARANCE_LENGTH - BLE_GENERAL_BYTE_1,
        .adv_data_type = BLE_ADV_APPEARANCE_DATA_TYPE,
        .catogory_id = { u16_low_u8(APPEARANCE_MOUSE), u16_high_u8(APPEARANCE_MOUSE)},
    };
    n_ret = memcpy_s(set_adv_data_position, max_len, &adv_appearance_data, BLE_ADV_APPEARANCE_LENGTH);
    if (n_ret != EOK) {
        return 0;
    }
    return BLE_ADV_APPEARANCE_LENGTH;
}

static uint8_t ble_set_adv_manufac_data(uint8_t *set_adv_data_position, uint8_t max_len)
{
    errno_t n_ret;
    uint8_t adv_data[] = {
        0x13,
        BLE_ADV_MANUFACTURE_DATA_TYPE,
        0x06, 0x00, 0x03, 0x00, 0x80,
        'T', 'u', 'r', 'n', 'k', 'e', 'y', '_', 'm', 'o', 'u', 's', 'e'
    };  /* Microsoft ID Compatible & Mouse Name */
    n_ret = memcpy_s(set_adv_data_position, max_len, adv_data, sizeof(adv_data));
    if (n_ret != EOK) {
        return 0;
    }
    return sizeof(adv_data);
}

static uint8_t ble_set_adv_appearance_data(uint8_t *set_adv_data_position, uint8_t max_len)
{
    uint8_t idx = 0;
    idx += ble_set_adv_appearance_mouse(set_adv_data_position + idx, max_len - idx);
    idx += ble_set_adv_manufac_data(set_adv_data_position + idx, (max_len - idx));
    return idx;
}

static uint16_t ble_uuid_server_set_adv_data(uint8_t *set_adv_data, uint8_t adv_data_max_len)
{
    uint8_t idx = 0;

    if ((set_adv_data == NULL) || (adv_data_max_len == 0)) {
        return 0;
    }
    idx += ble_set_adv_flag_data(set_adv_data, adv_data_max_len);
    idx += ble_set_adv_appearance_data(&set_adv_data[idx], adv_data_max_len - idx);
    return idx;
}

static uint16_t ble_set_scan_response_data(uint8_t *scan_rsp_data, uint8_t scan_rsp_data_max_len)
{
    uint8_t idx = 0;

    if (scan_rsp_data == NULL) {
        return 0;
    }
    if (scan_rsp_data_max_len == 0) {
        return 0;
    }

    ble_appearance_t adv_appearance_data = {
        .length = BLE_ADV_APPEARANCE_LENGTH - BLE_GENERAL_BYTE_1,
        .adv_data_type = BLE_SERVICE_CLASS_UUID,
        .catogory_id = { u16_low_u8(HID_SERVICE), u16_high_u8(HID_SERVICE) },
    };

    errno_t n_ret = memcpy_s(scan_rsp_data, scan_rsp_data_max_len, &adv_appearance_data, sizeof(ble_appearance_t));
    if (n_ret != EOK) {
        return 0;
    }
    idx += BLE_ADV_APPEARANCE_LENGTH;

    /* set local name */
    uint16_t name_len = strlen(TURNKEY_DEVICE_NAME);
    scan_rsp_data[idx++] = name_len + BLE_GENERAL_BYTE_1;
    scan_rsp_data[idx++] = BLE_ADV_LOCAL_NAME_DATA_TYPE;
    if ((idx + name_len) > scan_rsp_data_max_len) {
        return 0;
    }
    n_ret = memcpy_s(&scan_rsp_data[idx], scan_rsp_data_max_len - idx, (uint8_t *)TURNKEY_DEVICE_NAME, name_len);
    if (n_ret != EOK) {
        return 0;
    }
    idx += name_len;
    return idx;
}

uint8_t ble_mouse_set_adv_data(uint8_t adv_id)
{
    uint16_t adv_data_len;
    uint16_t scan_rsp_data_len;
    uint8_t set_adv_data[EXT_ADV_OR_SCAN_RSP_DATA_LEN] = { 0 };
    uint8_t set_scan_rsp_data[EXT_ADV_OR_SCAN_RSP_DATA_LEN] = { 0 };
    gap_ble_config_adv_data_t cfg_adv_data;

    /* set adv data */
    adv_data_len = ble_uuid_server_set_adv_data(set_adv_data, EXT_ADV_OR_SCAN_RSP_DATA_LEN);
    if ((adv_data_len > EXT_ADV_OR_SCAN_RSP_DATA_LEN) || (adv_data_len == 0)) {
        return 0;
    }
    /* set scan response data */
    scan_rsp_data_len = ble_set_scan_response_data(set_scan_rsp_data, EXT_ADV_OR_SCAN_RSP_DATA_LEN);
    if ((scan_rsp_data_len > EXT_ADV_OR_SCAN_RSP_DATA_LEN) || (scan_rsp_data_len == 0)) {
        return 0;
    }
    cfg_adv_data.adv_data = set_adv_data;
    cfg_adv_data.adv_length = adv_data_len;

    cfg_adv_data.scan_rsp_data = set_scan_rsp_data;
    cfg_adv_data.scan_rsp_length = scan_rsp_data_len;
    if (gap_ble_set_adv_data(adv_id, &cfg_adv_data) != 0) {
        return 0;
    }
    return 0;
}

uint8_t ble_mouse_start_adv(void)
{
    errcode_t n_ret = 0;
    gap_ble_adv_params_t adv_para = { 0 };

    int adv_id = BTH_GAP_BLE_ADV_HANDLE_DEFAULT;

    adv_para.min_interval = BLE_ADV_MIN_INTERVAL;
    adv_para.max_interval = BLE_ADV_MAX_INTERVAL;
    adv_para.duration = BTH_GAP_BLE_ADV_FOREVER_DURATION;
    adv_para.peer_addr.type = BLE_PUBLIC_DEVICE_ADDRESS;
    /* 广播通道选择bitMap, 可参考BleAdvChannelMap */
    adv_para.channel_map = BLE_ADV_CHANNEL_MAP_CH_DEFAULT;
    adv_para.adv_type = BLE_ADV_TYPE_CONNECTABLE_UNDIRECTED;

    adv_para.adv_filter_policy = BLE_ADV_FILTER_POLICY_SCAN_ANY_CONNECT_ANY;
    (void)memset_s(&adv_para.peer_addr.addr, BD_ADDR_LEN, 0, BD_ADDR_LEN);
    n_ret = gap_ble_set_adv_param(adv_id, &adv_para);
    if (n_ret != 0) {
        return 0;
    }
    n_ret = gap_ble_start_adv(adv_id);
    if (n_ret != 0) {
        return 0;
    }
    return 0;
}

uint32_t ble_mouse_stop_adv(void)
{
    errcode_t n_ret = 0;
    int adv_id = BTH_GAP_BLE_ADV_HANDLE_DEFAULT;
    n_ret = gap_ble_stop_adv(adv_id);
    if (n_ret != 0) {
        return n_ret;
    }
    return 0;
}
