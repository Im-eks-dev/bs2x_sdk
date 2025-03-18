/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE MICRO sample of client. \n
 *
 * History: \n
 * 2023-09-23, Create file. \n
 */

#include "securec.h"
#include "errcode.h"
#include "osal_addr.h"
#include "osal_task.h"
#include "common_def.h"
#include "sle_common.h"
#include "sle_device_discovery.h"
#include "sle_device_manager.h"
#include "sle_errcode.h"

#include "sle_multi_conn_server.h"
#include "sle_multi_conn_server_adv.h"

/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MIN_DEFAULT 0x64
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MAX_DEFAULT 0x64
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MIN_DEFAULT 0xC8
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MAX_DEFAULT 0xC8
/* 超时时间5000ms，单位10ms */
#define SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT 0x1F4
/* 超时时间4990ms，单位10ms */
#define SLE_CONN_MAX_LATENCY 0x1F3
/* 广播发送功率 */
#define SLE_ADV_TX_POWER 10

/* 最大广播数据长度 */
#define SLE_ADV_DATA_LEN 20

#define SLE_MULTI_CONN_SERVER_LOG "[sle multi_conn server]"

#define MAC_ADDR_MOV_BIT_NUM_1 8
#define MAC_ADDR_MOV_BIT_NUM_2 16
#define MAC_ADDR_MOV_BIT_NUM_3 24
#define MAC_ADDR_MOV_BIT_NUM_4 32
#define MAC_ADDR_MOV_BIT_NUM_5 40

static void sle_power_on_cbk(uint8_t status)
{
    osal_printk("%s sle power on: %x\r\n", SLE_MULTI_CONN_SERVER_LOG, status);
    enable_sle();
}

static void sle_enable_cbk(uint8_t status)
{
    unused(status);
    errcode_t ret = 0;
    ret = sle_multi_conn_server_add();
    osal_printk("%s sle_enable_cbk,sle_multi_conn_server_add return: [%x]\r\n",
                SLE_MULTI_CONN_SERVER_LOG, ret);
    ret = sle_multi_conn_server_adv_init();
    osal_printk("%s sle_enable_cbk,sle_multi_conn_server_adv_init return: [%x]\r\n",
                SLE_MULTI_CONN_SERVER_LOG, ret);
}

errcode_t sle_multi_conn_server_sample_dev_cbk_register(void)
{
    errcode_t ret = 0;
    sle_dev_manager_callbacks_t dev_mgr_cbks = {0};
    dev_mgr_cbks.sle_power_on_cb = sle_power_on_cbk;
    dev_mgr_cbks.sle_enable_cb = sle_enable_cbk;
    ret = sle_dev_manager_register_callbacks(&dev_mgr_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_dev_register_cbks,register_callbacks fail :%x\r\n",
                    SLE_MULTI_CONN_SERVER_LOG, ret);
        return ret;
    }
#if (CORE_NUMS < 2)
    enable_sle();
#endif
    return ERRCODE_SLE_SUCCESS;
}

static uint16_t sle_set_adv_local_name(uint8_t *adv_data, uint16_t max_len)
{
    uint8_t index = 0;
    uint8_t *local_name = (uint8_t *)CONFIG_SLE_MULTI_CONN_SERVER_NAME;
    uint8_t local_name_len = (uint8_t)strlen((char *)CONFIG_SLE_MULTI_CONN_SERVER_NAME);

    osal_printk("%s local_name_len = %d\r\n", SLE_MULTI_CONN_SERVER_LOG, local_name_len);
    osal_printk("%s local_name: ", SLE_MULTI_CONN_SERVER_LOG);
    for (uint8_t i = 0; i < local_name_len; i++) {
        osal_printk("0x%02x ", local_name[i]);
    }
    osal_printk("\r\n");
    adv_data[index++] = local_name_len + 1;
    adv_data[index++] = SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
    if (memcpy_s(&adv_data[index], max_len - index, local_name, local_name_len) != EOK) {
        osal_printk("%s memcpy fail\r\n", SLE_MULTI_CONN_SERVER_LOG);
        return 0;
    }
    return (uint16_t)index + local_name_len;
}

static uint16_t sle_set_adv_data(uint8_t *adv_data)
{
    size_t len = 0;
    uint16_t idx = 0;

    len = sizeof(struct sle_adv_common_value);
    struct sle_adv_common_value adv_disc_level = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL,
        .value = SLE_ANNOUNCE_LEVEL_NORMAL,
    };
    if (memcpy_s(&adv_data[idx], SLE_ADV_DATA_LEN - idx, &adv_disc_level, len) != EOK) {
        return 0;
    }
    idx += len;

    len = sizeof(struct sle_adv_common_value);
    struct sle_adv_common_value adv_access_mode = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_ACCESS_MODE,
        .value = 0,
    };
    if (memcpy_s(&adv_data[idx], SLE_ADV_DATA_LEN - idx, &adv_access_mode, len) != EOK) {
        return 0;
    }
    idx += len;

    return idx;
}

static uint16_t sle_set_scan_response_data(uint8_t *scan_rsp_data)
{
    uint16_t idx = 0;
    size_t scan_rsp_data_len = sizeof(struct sle_adv_common_value);

    struct sle_adv_common_value tx_power_level = {
        .length = scan_rsp_data_len - 1,
        .type = SLE_ADV_DATA_TYPE_TX_POWER_LEVEL,
        .value = SLE_ADV_TX_POWER,
    };
    if (memcpy_s(scan_rsp_data, SLE_ADV_DATA_LEN, &tx_power_level, scan_rsp_data_len) != EOK) {
        return 0;
    }
    idx += scan_rsp_data_len;

    /* set local name */
    idx += sle_set_adv_local_name(&scan_rsp_data[idx], SLE_ADV_DATA_LEN - idx);
    return idx;
}

static int sle_set_default_announce_param(void)
{
    sle_announce_param_t param = {0};
    uint64_t config_addr = CONFIG_CURRENT_SERVER_ADDR;
    uint8_t local_addr[SLE_ADDR_LEN] = {
        (uint8_t)((config_addr >> MAC_ADDR_MOV_BIT_NUM_5) & 0xff),
        (uint8_t)((config_addr >> MAC_ADDR_MOV_BIT_NUM_4) & 0xff),
        (uint8_t)((config_addr >> MAC_ADDR_MOV_BIT_NUM_3) & 0xff),
        (uint8_t)((config_addr >> MAC_ADDR_MOV_BIT_NUM_2) & 0xff),
        (uint8_t)((config_addr >> MAC_ADDR_MOV_BIT_NUM_1) & 0xff),
        (uint8_t)(config_addr & 0xff)};

    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = SLE_ADV_HANDLE_DEFAULT;
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
        osal_printk("set sle adv param memcpy addr fail\r\n");
        return ERRCODE_SLE_PARAM_ERR;
    }
    sle_addr_t local_address = {0};
    local_address.type = 0;
    (void)memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    sle_set_local_addr(&local_address);
    osal_printk("%s sle_mult_conn_server_local addr: %02x:%02x:%02x:%02x:%02x:%02x \r\n",
                SLE_MULTI_CONN_SERVER_LOG, param.own_addr.addr[SLE_ADDR_INDEX0],
                param.own_addr.addr[SLE_ADDR_INDEX1], param.own_addr.addr[SLE_ADDR_INDEX2],
                param.own_addr.addr[SLE_ADDR_INDEX3], param.own_addr.addr[SLE_ADDR_INDEX4],
                param.own_addr.addr[SLE_ADDR_INDEX5]);
    return sle_set_announce_param(param.announce_handle, &param);
}

static errcode_t sle_set_default_announce_data(void)
{
    errcode_t ret;
    uint8_t announce_data_len = 0;
    uint8_t seek_data_len = 0;
    sle_announce_data_t data = {0};
    uint8_t adv_handle = SLE_ADV_HANDLE_DEFAULT;
    uint8_t announce_data[SLE_ADV_DATA_LEN] = {0};
    uint8_t seek_rsp_data[SLE_ADV_DATA_LEN] = {0};

    announce_data_len = sle_set_adv_data(announce_data);
    data.announce_data = announce_data;
    data.announce_data_len = announce_data_len;

    seek_data_len = sle_set_scan_response_data(seek_rsp_data);
    data.seek_rsp_data = seek_rsp_data;
    data.seek_rsp_data_len = seek_data_len;

    ret = sle_set_announce_data(adv_handle, &data);
    osal_printk("%s sle_set_default_announce_data, seek_rsp_data:%s, announce_data:%s\r\n",
                SLE_MULTI_CONN_SERVER_LOG, seek_rsp_data, announce_data);
    return ret;
}

static void sle_announce_enable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("%s sle announce enable callback id:%02x, state:%x\r\n",
                SLE_MULTI_CONN_SERVER_LOG, announce_id, status);
}

static void sle_announce_disable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("%s sle announce disable callback id:%02x, state:%x\r\n",
                SLE_MULTI_CONN_SERVER_LOG, announce_id, status);
}

static void sle_announce_terminal_cbk(uint32_t announce_id)
{
    osal_printk("%s sle announce terminal callback announce_id:%02x\r\n",
                SLE_MULTI_CONN_SERVER_LOG, announce_id);
}

/* 注册SLE设备公开回调函数 */
errcode_t sle_multi_conn_announce_register_cbks(void)
{
    errcode_t ret = 0;
    sle_announce_seek_callbacks_t seek_cbks = {0};
    seek_cbks.announce_enable_cb = sle_announce_enable_cbk;
    seek_cbks.announce_disable_cb = sle_announce_disable_cbk;
    seek_cbks.announce_terminal_cb = sle_announce_terminal_cbk;

    ret = sle_announce_seek_register_callbacks(&seek_cbks);
    osal_printk("%s sle_multi_conn_announce_register_cbks,register_callbacks return: [%x]\r\n",
                SLE_MULTI_CONN_SERVER_LOG, ret);
    return ret;
}

errcode_t sle_multi_conn_server_adv_init(void)
{
    errcode_t ret;
    sle_set_default_announce_param();
    sle_set_default_announce_data();
    ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    osal_printk("%s sle_multi_conn_server_adv_init,sle_start_announce return: [%x]\r\n",
                SLE_MULTI_CONN_SERVER_LOG, ret);
    return ret;
}
