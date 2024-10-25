/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: SLE OTA Recv Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-02-01, Create file. \n
 */
#include "securec.h"
#include "common_def.h"
#include "bts_le_gap.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_device_manager.h"
#include "sle_ota_client.h"
#include "gadget/f_hid.h"
#include "sle_ota_cmd.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "sle_ota_cmd_handler.h"

#define WRITE_REQ_DATA_LEN_INDEX    33
#define WRITE_REQ_DATA_INDEX        35
#define WRITE_REQ_BUFFER_LEN        200
#define DEVICE_ADDR_RPT_LEN         24
#define RPT_TYPE_INDEX              12
#define RPT_ADDR_INDEX              16
#define SLE_OTA_CONNECT_DELAY       500

static uint32_t g_sle_ota_hid_index = 0;
static uint8_t g_sle_ota_send_buffer[WRITE_REQ_BUFFER_LEN] = { 0 };
static uint8_t g_sle_ota_rpt[DEVICE_ADDR_RPT_LEN] = { 0x1F, 0x0, 0x0, 0x0, 0x0, 0x1, 0x4, 0xd, 0x0, 0x1, 0x1,
                                                      0x0, 0x0, 0x2, 0x6, 0x0, 0x0 };

void sle_ota_set_hid_index(uint32_t index)
{
    g_sle_ota_hid_index = index;
}

uint32_t sle_ota_get_hid_index(void)
{
    return g_sle_ota_hid_index;
}

void sle_ota_send_common_response(uint8_t service_id, uint8_t command_id, uint8_t ret)
{
    sle_ota_response_frame_t response = { 0 };
    response.flag = 0x1F;
    response.service_id = service_id;
    response.command_id = command_id;
    response.body_len[0] = 0x04;
    response.body_len[1] = 0x00;
    response.type = 0x7F;
    response.len[0] = 0x01;
    response.len[1] = 0x00;
    response.errorcode = ret;
    fhid_send_data(g_sle_ota_hid_index, (char *)&response, sizeof(sle_ota_response_frame_t));
}

static errcode_t sle_ota_enable_sle(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

static errcode_t sle_ota_disable_sle(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = disable_sle();
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

static errcode_t sle_ota_set_device_addr(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(length);
    unused(buffer);
    errcode_t ret = ERRCODE_SUCC;
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

static errcode_t sle_ota_get_device_addr(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(service_id);
    unused(command_id);
    unused(buffer);
    unused(length);
    sle_addr_t local_address = { 0 };
    sle_get_local_addr(&local_address);
    g_sle_ota_rpt[RPT_TYPE_INDEX] = local_address.type;
    for (uint8_t i = 0; i < SLE_ADDR_LEN; i++) {
        g_sle_ota_rpt[RPT_ADDR_INDEX + i] = (uint8_t)local_address.addr[i];
    }
    fhid_send_data(g_sle_ota_hid_index, (char *)g_sle_ota_rpt, DEVICE_ADDR_RPT_LEN);
    return ERRCODE_SUCC;
}

static errcode_t sle_ota_set_device_name(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

static errcode_t sle_ota_get_device_name(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

static errcode_t sle_ota_set_announce_data(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

static errcode_t sle_ota_start_announce(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

static errcode_t sle_ota_end_announce(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

static errcode_t sle_ota_set_scan_param(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

static errcode_t sle_ota_upload_scan_result(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

static errcode_t sle_ota_device_start_scan(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    set_sle_ota_scan_state(true);
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

static errcode_t sle_ota_stop_scan(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    set_sle_ota_scan_state(false);
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

static errcode_t sle_ota_send_data(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    uint16_t len = (buffer[WRITE_REQ_DATA_LEN_INDEX]) + (buffer[WRITE_REQ_DATA_LEN_INDEX + 1] << 8);
    for (uint32_t i = 0; i < len; i++) {
        g_sle_ota_send_buffer[i] = buffer[WRITE_REQ_DATA_INDEX + i];
    }
    ssapc_write_param_t *sle_micro_send_param = get_sle_ota_send_param();
    sle_micro_send_param->handle = 0x2;
    sle_micro_send_param->data_len = len;
    sle_micro_send_param->data = g_sle_ota_send_buffer;
    ssapc_write_req(0, get_sle_ota_conn_id(), sle_micro_send_param);
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

errcode_t sle_ota_discover_device_service(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    errcode_t ret = ERRCODE_SUCC;
    switch (command_id) {
        case COMMAND_ID_ENABLE_SLE:
            ret = sle_ota_enable_sle(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_DISABLE:
            ret = sle_ota_disable_sle(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_SET_DEVICE_ADDR:
            ret = sle_ota_set_device_addr(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_GET_DEVICE_ADDR:
            ret = sle_ota_get_device_addr(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_SET_DEVICE_NAME:
            ret = sle_ota_set_device_name(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_GET_DEVICE_NAME:
            ret = sle_ota_get_device_name(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_SET_ANNOUNCE_DATA:
            ret = sle_ota_set_announce_data(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_SET_START_ANNOUNCE:
            ret = sle_ota_start_announce(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_SET_END_ANNOUNCE:
            ret = sle_ota_end_announce(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_SET_SCAN_PARAM:
            ret = sle_ota_set_scan_param(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_UPLOAD_SCAN_RESULT:
            ret = sle_ota_upload_scan_result(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_START_SCAN:
            ret = sle_ota_device_start_scan(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_STOP_SCAN:
            ret = sle_ota_stop_scan(service_id, command_id, buffer, length);
            break;
        default:
            break;
    }
    return ret;
}

static errcode_t sle_ota_connect(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    set_sle_ota_set_address_state(true);
    while (get_ssap_find_ready() == 0) {
        osal_msleep(SLE_OTA_CONNECT_DELAY);
    }
    sle_ota_send_common_response(service_id, command_id, (uint8_t)ret);
    return ret;
}

errcode_t sle_ota_manage_connection_service(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    switch (command_id) {
        case COMMAND_ID_SEND_LINK_REQ:
            ret = sle_ota_connect(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_BREAK_LINK_REQ:
        case COMMAND_ID_UPG_LINK_PARAM:
        case COMMAND_ID_SEND_BOUND_REQ:
        case COMMAND_ID_DELETE_BOUND:
        case COMMAND_ID_DELETE_ALL_BOUND:
        case COMMAND_ID_GET_BOUND_DEVICE_NUM:
        case COMMAND_ID_GET_BOUND_DEVICE:
        case COMMAND_ID_GET_BOUND_STATUS:
        case COMMAND_ID_GET_DEVICE_RSSI:
        case COMMAND_ID_GET_ACB_PARAM:
        case COMMAND_ID_SET_PHY_PARAM:
        case COMMAND_ID_SET_POWRER_MAXIMUM:
        case COMMAND_ID_SEND_LINK_MANAGEMEND_CHECK:
            break;
        default:
            break;
    }
    return ret;
}

errcode_t sle_ota_manage_ssap_client(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length)
{
    unused(buffer);
    unused(length);
    errcode_t ret = ERRCODE_SUCC;
    switch (command_id) {
        case COMMAND_ID_REGISTER_CLIENT:
        case COMMAND_ID_UNREGISTER_CLIENT:
        case COMMAND_ID_SEARCH_SERVER_DESCRIPTION:
        case COMMAND_ID_UUID_READ_REQ:
        case COMMAND_ID_HANDLE_READ_REQ:
        case COMMAND_ID_WRITE_REQ:
        case COMMAND_ID_WRITE_CMD:
            ret = sle_ota_send_data(service_id, command_id, buffer, length);
            break;
        case COMMAND_ID_EXCHANGE_INFO_REQ:
            break;
        default:
            break;
    }
    return ret;
}
