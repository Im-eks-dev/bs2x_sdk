/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: RCU MP Test source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-25, Create file. \n
 */
#include <stdint.h>
#include <string.h>
#include "securec.h"
#include "product.h"
#include "common_def.h"
#include "sle_device_discovery.h"
#include "sle_ssap_server.h"
#include "sle_connection_manager.h"
#include "sle_errcode.h"
#include "sle_rcu_server_adv.h"
#include "osal_debug.h"
#include "app_common.h"
#include "sle_rcu_server.h"
#include "sle_rcu_server_adv.h"
#include "app_status.h"
#include "hal_reboot.h"
#include "rcu_mp_test.h"

sle_auth_info_evt_t g_key[MP_TEST_MAX_STATION_NUM] = {
    {{0xab, 0xa9, 0xef, 0x28, 0x88, 0xb4, 0xfd, 0x8, 0x45, 0xbd, 0xe2, 0x5a, 0xa8, 0xb8, 0x68, 0xc7}, 0x2, 0x2, 0x0},
    {{0xa7, 0xc7, 0xb6, 0xa, 0x5b, 0x8f, 0x14, 0x12, 0xff, 0x90, 0xd7, 0x4a, 0x7, 0x6f, 0xf1, 0xf1}, 0x2, 0x2, 0x0},
    {{0x4a, 0xbb, 0xb2, 0xc0, 0x78, 0x8, 0xcd, 0x1d, 0xf7, 0x9f, 0x8d, 0x8f, 0xc4, 0xbc, 0x60, 0x41}, 0x2, 0x2, 0x0},
    {{0x91, 0x9a, 0x86, 0x1b, 0xea, 0x9a, 0xeb, 0x78, 0x9e, 0x3d, 0x4a, 0xe7, 0xf3, 0x20, 0xd, 0x9f}, 0x2, 0x2, 0x0},
    {{0x94, 0x29, 0x90, 0xc2, 0x9d, 0x55, 0xe, 0xe, 0xd2, 0x53, 0x73, 0xd9, 0x6, 0xde, 0x60, 0x71}, 0x2, 0x2, 0x0},
    {{0x1e, 0xc2, 0xba, 0xbe, 0x69, 0xf0, 0x8e, 0x32, 0x24, 0x6, 0x62, 0x75, 0x6e, 0xad, 0xc, 0xbd}, 0x2, 0x2, 0x0},
};

sle_addr_t g_peer_addr[MP_TEST_MAX_STATION_NUM] = {
    {0, {00, 00, 00, 00, 00, 01}},
    {0, {00, 00, 00, 00, 00, 02}},
    {0, {00, 00, 00, 00, 00, 03}},
    {0, {00, 00, 00, 00, 00, 04}},
    {0, {00, 00, 00, 00, 00, 05}},
    {0, {00, 00, 00, 00, 00, 06}},
};
void rcu_out_of_production_testing(void)
{
    uapi_reg_write(0x5702C984, 0xFF);
    uapi_reg_write(0x5702C980, 0x0);
}

void rcu_mp_test_rcu_to_dongle_key_value(uint8_t key_nums, uint8_t *key_values)
{
    uint8_t version_buff[PRIVATE_FRAME_INFO_LEN] = { PRIVATE_FRAME_FLAG, 0x00, 0x00, 0x00, 0x00, 0x05,
        PRIVATE_FRAME_CMD_GET_KEY_VALUE, 0x0b, 0x00, 0x01};
    ssaps_ntf_ind_t param = { 0 };
    version_buff[BODYLEN_BYTE] = TLV_TYPE_AND_LEN + key_nums;

    encode2byte_little(version_buff + TLV_LEN_BYTE1, key_nums);
    param.handle = rcu_get_handle();
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = version_buff;
    param.value_len = PRIVATE_FRAME_INFO_LEN;
    (void)memcpy_s(param.value + PRIVATE_FRAME_TLV_DATA_BYTE, PRIVATE_FRAME_INFO_LEN - PRIVATE_FRAME_TLV_DATA_BYTE,
        key_values, key_nums);
    ssaps_notify_indicate(rcu_get_server_id(), get_g_connid(), &param);
}

void rcu_mp_test_get_version_handler(void)
{
    uint8_t version_buff[PRIVATE_FRAME_INFO_LEN] = { PRIVATE_FRAME_FLAG, 0x00, 0x00, 0x00, 0x00, 0x05,
        PRIVATE_FRAME_CMD_GET_VERSION, 0x07, 0x00, 0x01};
    ssaps_ntf_ind_t param = { 0 };
    param.handle = rcu_get_handle();
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = version_buff;
    param.value_len = PRIVATE_FRAME_INFO_LEN;
    encode2byte_little(version_buff + TLV_LEN_BYTE1, VERSION_LEN);
    (void)memcpy_s(param.value + PRIVATE_FRAME_TLV_DATA_BYTE, PRIVATE_FRAME_INFO_LEN - PRIVATE_FRAME_TLV_DATA_BYTE,
        APPLICATION_VERSION_STRING, strlen(APPLICATION_VERSION_STRING));

    ssaps_notify_indicate(rcu_get_server_id(), get_g_connid(), &param);
}

void rcu_mp_test_get_mac_addr_handler(void)
{
    sle_addr_t own_addr = {0};
    sle_get_local_addr(&own_addr);

    uint8_t version_buff[PRIVATE_FRAME_INFO_LEN] = { PRIVATE_FRAME_FLAG, 0x00, 0x00, 0x00, 0x00, 0x05,
        PRIVATE_FRAME_CMD_GET_MAC_ADDR, 0x09, 0x00, 0X01};
    ssaps_ntf_ind_t param = { 0 };
    param.handle = rcu_get_handle();
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = version_buff;
    param.value_len = PRIVATE_FRAME_INFO_LEN;
    encode2byte_little(version_buff + TLV_LEN_BYTE1, SLE_ADDR_LEN);

    (void)memcpy_s(param.value + PRIVATE_FRAME_TLV_DATA_BYTE, PRIVATE_FRAME_INFO_LEN - PRIVATE_FRAME_TLV_DATA_BYTE,
        own_addr.addr, SLE_ADDR_LEN);
    osal_printk("server_id value: %d \r\n", rcu_get_server_id());
    osal_printk("g_connid value: %d \r\n", get_g_connid());
    ssaps_notify_indicate(rcu_get_server_id(), get_g_connid(), &param);
}

void rcu_mp_test_set_work_station(uint8_t combine_key_flag)
{
    errcode_sle_t ret;
    osal_printk("dongle set work station:%d\r\n", combine_key_flag);
    set_rcu_status(RCU_STATUS_TEST_NO_SLEPP);
    uint8_t local_addr[SLE_ADDR_LEN] = {0x10, 0x00, 0x00, 0x00, 0x00, 0x00};
    sle_addr_t rcu_mp_test_address;
    rcu_mp_test_address.type = 0;
    (void)memcpy_s(rcu_mp_test_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    sle_addr_t pair_mp_test_address;
    (void)memcpy_s(&pair_mp_test_address, sizeof(sle_addr_t),  &g_peer_addr[combine_key_flag - 1], sizeof(sle_addr_t));
    sle_set_save_pair_keys_mode(SLE_SAVE_SMP_KEYS_MANU);
    ret = sle_set_nv_smp_keys(&g_key[combine_key_flag - 1], &rcu_mp_test_address, &pair_mp_test_address, 0);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("Set test station fail.  error_code: %x \r\n", ret);
    }
    rcu_mp_test_server_adv_init(&rcu_mp_test_address);
}

void rcu_mp_test_handler(private_frame_t *private_frame, uint8_t *cmd, uint16_t cmd_len)
{
    unused(private_frame);
    unused(cmd);
    unused(cmd_len);
    switch (private_frame->cmd_id) {
        case PRIVATE_FRAME_CMD_GET_MAC_ADDR:
            rcu_mp_test_get_mac_addr_handler();
            break;
        case PRIVATE_FRAME_CMD_GET_VERSION:
            rcu_mp_test_get_version_handler();
            break;
        default:
            break;
    }
}