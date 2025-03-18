/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: DONGLE MP Test source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-25, Create file. \n
 */

#include "common_def.h"
#include "securec.h"
#include "osal_debug.h"
#include "sle_errcode.h"
#include "sle_common.h"
#include "../sle_rcu_client.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "gadget/f_hid.h"
#include "rcu_dongle_mp_test.h"

static bool g_send_voice = false;
static uint8_t g_current_test_station = 0;
static uint8_t g_now_command_id = 0;
static sle_addr_t g_own_addr[MP_TEST_MAX_STATION_NUM] = {
    {0, {00, 00, 00, 00, 00, 01}},
    {0, {00, 00, 00, 00, 00, 02}},
    {0, {00, 00, 00, 00, 00, 03}},
    {0, {00, 00, 00, 00, 00, 04}},
    {0, {00, 00, 00, 00, 00, 05}},
    {0, {00, 00, 00, 00, 00, 06}},
};

sle_auth_info_evt_t g_key[MP_TEST_MAX_STATION_NUM] = {
    {{0xab, 0xa9, 0xef, 0x28, 0x88, 0xb4, 0xfd, 0x8, 0x45, 0xbd, 0xe2, 0x5a, 0xa8, 0xb8, 0x68, 0xc7}, 0x2, 0x2, 0x0},
    {{0xa7, 0xc7, 0xb6, 0xa, 0x5b, 0x8f, 0x14, 0x12, 0xff, 0x90, 0xd7, 0x4a, 0x7, 0x6f, 0xf1, 0xf1}, 0x2, 0x2, 0x0},
    {{0x4a, 0xbb, 0xb2, 0xc0, 0x78, 0x8, 0xcd, 0x1d, 0xf7, 0x9f, 0x8d, 0x8f, 0xc4, 0xbc, 0x60, 0x41}, 0x2, 0x2, 0x0},
    {{0x91, 0x9a, 0x86, 0x1b, 0xea, 0x9a, 0xeb, 0x78, 0x9e, 0x3d, 0x4a, 0xe7, 0xf3, 0x20, 0xd, 0x9f}, 0x2, 0x2, 0x0},
    {{0x94, 0x29, 0x90, 0xc2, 0x9d, 0x55, 0xe, 0xe, 0xd2, 0x53, 0x73, 0xd9, 0x6, 0xde, 0x60, 0x71}, 0x2, 0x2, 0x0},
    {{0x1e, 0xc2, 0xba, 0xbe, 0x69, 0xf0, 0x8e, 0x32, 0x24, 0x6, 0x62, 0x75, 0x6e, 0xad, 0xc, 0xbd}, 0x2, 0x2, 0x0},
};

static void rcu_dongle_to_usb_data(uint8_t *dongle_buff)
{
    if (dongle_buff == NULL) {
        osal_printk("dongle_buff is null\r\n");
        return;
    }
    osal_printk("rcu_dongle_to_usb_data \r\n");
    fhid_send_data(MP_TEST_SEND_ENDPOINT, (char *)dongle_buff, MP_TEST_INFO_LEN);
}

static void rcu_mp_test_dongle_to_server(uint8_t *cmd, int32_t cmd_len)
{
    ssapc_write_param_t *rcu_send_param_ptr = get_sle_rcu_send_param();
    rcu_send_param_ptr->data_len = cmd_len;
    uint8_t data[MP_TEST_INFO_LEN] = {0};
    if (memcpy_s(data, MP_TEST_INFO_LEN, cmd, cmd_len) != EOK) {
        osal_printk("error:memcpy_s failed\r\n");
    }
    rcu_send_param_ptr->data = data;

    ssapc_write_req(0, 0, rcu_send_param_ptr);
}

static void rcu_dongle_mp_test_get_voice(void)
{
    g_send_voice = true;
}

static void rcu_dongle_mp_test_stop_voice(void)
{
    osal_msleep(1);
    uint8_t test_send[MP_TEST_INFO_LEN] = {PRIVATE_SLE_FRAME_FLAG, 0x00, 0x00, 0x00, 0x00, 0x01, MP_TEST_GET_VOICE_CMD,
        0x0d, 0x00, 0x01, 0x01, 0x00, 0x00, 0x02, 0x1, 0x00, 0xff};
    fhid_send_data(MP_TEST_SEND_ENDPOINT, (char *)test_send, MP_TEST_INFO_LEN);
    g_send_voice = false;
}

static void rcu_dongle_mp_test_set_rssi(uint8_t *tlv)
{
    uint8_t tlv_type = tlv[TLV_TYPE_TYPE];
    uint16_t tlv_len = tlv[TLV_TYPE_LEN];
    if (tlv_type != 1 || tlv_len != 1) {
        return;
    }
    if (sle_set_connect_rssi((int8_t)tlv[TLV_TYPE_VALUE]) != ERRCODE_SLE_SUCCESS) {
        osal_printk("Set RSSI fail.\r\n");
    }
}

void rcu_dongle_mp_test_set_test_station(uint8_t *tlv)
{
    uint8_t tlv_type = tlv[TLV_TYPE_TYPE];
    uint8_t tlv_len = tlv[TLV_TYPE_LEN];

    if (tlv_type != 1 || tlv_len != 1) {
        return;
    }
    g_current_test_station = tlv[TLV_TYPE_VALUE];
    osal_printk("dongle need set work station:%d\r\n", g_current_test_station);
    sle_addr_t peer_addr = {0, {0x10, 0x00, 0x00, 0x00, 0x00, 0x00}};
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_TYPE)
    sle_set_save_pair_keys_mode(SLE_SAVE_SMP_KEYS_MANU);
    errcode_sle_t ret = sle_set_nv_smp_keys(&g_key[g_current_test_station - 1], &g_own_addr[g_current_test_station - 1],
        &peer_addr, 0);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("Set test station fail.  error_code: %x \r\n", ret);
    }
#endif
    sle_set_local_addr(&g_own_addr[g_current_test_station - 1]);
    sle_rcu_start_scan();
}

static void rcu_dongle_mp_test_clear_connect_info(void)
{
    uint8_t test_send[MP_TEST_INFO_LEN] = {PRIVATE_SLE_FRAME_FLAG, 0x00, 0x00, 0x00, 0x00, 0x01, MP_TEST_CLR_CONN_CMD,
        0x0d, 0x00, 0x01, 0x01, 0x00, 0x00};
    errcode_t ret = sle_remove_all_pairs();
    test_send[MP_TEST_TLV_LEN_BIT] = 0x1;
    if (!ret) {
        test_send[MP_TEST_TLV_DATA_BIT] = MP_TEST_SUCC_RET;
    } else {
        test_send[MP_TEST_TLV_DATA_BIT] = MP_TEST_FAIL_RET;
    }
    fhid_send_data(MP_TEST_SEND_ENDPOINT, (char *)test_send, MP_TEST_INFO_LEN);
}
void rcu_dongle_mp_test_submit(ssapc_handle_value_t *data)
{
    uint8_t dongle_buff[MP_TEST_INFO_LEN] = {0};
    memcpy_s(dongle_buff, MP_TEST_INFO_LEN, data->data, MP_TEST_INFO_LEN);
    if (g_now_command_id == dongle_buff[COMMAND_ID_BYTE]) {
        osal_printk("rcu_dongle_mp_test_submit \r\n");
        rcu_dongle_to_usb_data(dongle_buff);
    }
}
void *rcu_dongle_mp_test_task(const char *arg)
{
    unused(arg);
    uint8_t test_recv[MP_TEST_INFO_LEN] = { 0 };
    while (1) {
        int32_t ret = fhid_recv_data(1, (char*)test_recv, MP_TEST_INFO_LEN);
        if (ret < 0) {
            osal_printk("recv err");
            osal_msleep(MP_TEST_DELAY_US);
            continue;
        }
        g_now_command_id = test_recv[MP_TEST_CMD_BIT];
        if (test_recv[0] == PRIVATE_SLE_FRAME_FLAG) {
            switch (test_recv[MP_TEST_CMD_BIT]) {
                case MP_TEST_GET_MAC_CMD:
                case MP_TEST_GET_VER_CMD:
                    rcu_mp_test_dongle_to_server(test_recv, ret);
                    break;
                case MP_TEST_SET_RSSI_CMD:
                    rcu_dongle_mp_test_set_rssi(test_recv + MP_TEST_CMD_BIT + BODY_LEN);
                    break;
                case MP_TEST_CLR_CONN_CMD:
                    rcu_dongle_mp_test_clear_connect_info();
                    break;
                case MP_TEST_SET_TEST_STATION_CMD:
                    rcu_dongle_mp_test_set_test_station(test_recv + MP_TEST_CMD_BIT + BODY_LEN);
                    break;
                case MP_TEST_GET_VOICE_CMD:
                    rcu_dongle_mp_test_get_voice();
                    break;
                case MP_TEST_STOP_VOICE_CMD:
                    rcu_dongle_mp_test_stop_voice();
                    break;
                default:
                    break;
        }
    }
    }
    return NULL;
}

bool get_send_voice(void)
{
    return g_send_voice;
}