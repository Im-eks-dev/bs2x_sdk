/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse Server Manager  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#include <stdint.h>
#include "securec.h"
#include "errcode.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "osal_addr.h"
#include "sle_common.h"
#include "sle_errcode.h"
#include "bts_le_gap.h"
#include "app_os_init.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_ssap_server.h"
#include "sle_mouse_server_adv.h"
#include "sle_low_latency.h"
#include "mouse_mode_manage.h"
#include "ble_sle_status.h"
#include "sle_device_manager.h"
#include "mouse_data_transfer_manage.h"
#include "sle_mouse_bas_server.h"
#include "sle_mouse_dis_server.h"
#include "sle_mouse_hid_server.h"
#include "sle_mouse_channel_server.h"
#include "mouse_lowpower_manager.h"
#include "mac_rules.h"
#include "ble_mouse_server_adv.h"
#include "sle_mouse_server.h"

#define SLE_CONN_INTERVAL_PARAM 60 // 60 * 0.125 = 7.5ms
#define SLE_ADV_HANDLE_DEFAULT     1
#define USB_MOUSE_TASK_DELAY_MS    2000

#define SLE_MOUSE_DEFAULT_SERVER_ID  1
#define SLE_MOUSE_DEFAULT_CONNECT_ID 0

#define BT_INDEX_4                 4
#define BT_INDEX_5                 5
#define BT_INDEX_0                 0

static uint8_t g_sle_mouse_server_conn_state = SLE_ACB_STATE_NONE;
static uint8_t g_mouse_ssap_server_id = SLE_MOUSE_DEFAULT_SERVER_ID;
static uint8_t g_mouse_app_uuid[SLE_MOUSE_SAMPLE_APP_UUID_LEN] = { 0x0, 0x0 };

/************************** SLE 连接管理相关 **************************/
#define SLE_PRIVATE_FEATURE_2K     1
#define SLE_PRIVATE_FEATURE_8K     7

#define SLE_MOUSE_CONN_PARAM_INTERVAL_MIN     60
#define SLE_MOUSE_CONN_PARAM_INTERVAL_MIDDLE  100
#define SLE_MOUSE_CONN_PARAM_INTERVAL_MAX     400
#define SLE_MOUSE_CONN_PARAM_INTERVAL_DEF     32

static uint32_t g_mouse_sle_pair_status = 0;
static uint16_t g_mouse_sle_conn_id = 0xFFFF;
static uint8_t g_sle_remote_device_type = SLE_REMOTE_DEV_PC; // 默认为PC

/************************** Mouse 适配 Start **************************/
static bool g_sle_mouse_active_disconnect = true; /* 鼠标是否主动断开连接 */
static sle_addr_t g_sle_addr = {0};

typedef enum {
    SLE_DIS_INDEX0, // dis service
    SLE_DIS_INDEX1, // name
    SLE_DIS_INDEX2, // appearance
    SLE_DIS_INDEX3, // pnp id
} sle_dis_index_t;

sle_remote_device_type_t sle_mouse_get_device_type(void)
{
    return g_sle_remote_device_type;
}

void sle_mouse_set_device_type(sle_remote_device_type_t device_type)
{
    g_sle_remote_device_type = device_type;
}

errcode_t get_g_sle_mouse_pair_state(uint32_t *pair_state)
{
    *pair_state = g_mouse_sle_pair_status;
    return ERRCODE_SLE_SUCCESS;
}

uint8_t get_sle_mouse_connected(void)
{
    return g_sle_mouse_server_conn_state;
}

uint16_t get_mouse_sle_conn_id(void)
{
    return g_mouse_sle_conn_id;
}

uint16_t get_sle_remote_device_type(void)
{
    return g_sle_remote_device_type;
}


static uint16_t get_report_rate(uint8_t rate)
{
    switch (rate) {
#if ((CHIP_BS26 == 1) || (CHIP_BS22 == 1))
        case 5:         /* 5: MOUSE_8K_TURNKEY 8K report rate mapping */
            return SLE_LOW_LATENCY_8K;
        case 6:         /* 6: MOUSE_8K_TURNKEY 4K report rate mapping */
            return SLE_LOW_LATENCY_4K;
        case 7:         /* 7: MOUSE_8K_TURNKEY 2K report rate mapping */
            return SLE_LOW_LATENCY_2K;
        case 8:         /* 8: MOUSE_8K_TURNKEY 1K report rate mapping */
            return SLE_LOW_LATENCY_1K;
        case 9:         /* 9: MOUSE_8K_TURNKEY 500Hz report rate mapping */
            return SLE_LOW_LATENCY_500HZ;
        case 10:         /* 10: MOUSE_8K_TURNKEY 250Hz report rate mapping */
            return SLE_LOW_LATENCY_250HZ;
        case 11:         /* 11: MOUSE_8K_TURNKEY 125Hz report rate mapping */
            return SLE_LOW_LATENCY_125HZ;
#else
        case 0:         /* 9: MOUSE_2K_TURNKEY 4K report rate mapping */
            return SLE_LOW_LATENCY_4K;
        case 1:         /* 1: MOUSE_2K_TURNKEY 2K report rate mapping */
            return SLE_LOW_LATENCY_2K;
        case 2:         /* 2: MOUSE_2K_TURNKEY 1K report rate mapping */
            return SLE_LOW_LATENCY_1K;
        case 3:         /* 3: MOUSE_2K_TURNKEY 500Hz report rate mapping */
            return SLE_LOW_LATENCY_500HZ;
        case 4:         /* 4: MOUSE_2K_TURNKEY 1250Hz report rate mapping */
            return SLE_LOW_LATENCY_125HZ;
        case 12:         /* 12: MOUSE_2K_TURNKEY 10Hz report rate mapping */
            return SLE_LOW_LATENCY_10HZ;
        case 13:         /* 13: MOUSE_2K_TURNKEY 3Hz report rate mapping */
            return SLE_LOW_LATENCY_3HZ;
        case 14:         /* 14: MOUSE_2K_TURNKEY 250Hz report rate mapping */
            return SLE_LOW_LATENCY_250HZ;
#endif
        default:
            return SLE_LOW_LATENCY_INVALID;
    }
}

static void sle_connect_param_update_cbk(uint16_t conn_id, errcode_t status,
    const sle_connection_param_update_evt_t *param)
{
    osal_printk("%s connect up back conn_id:0x%02x, interval:0x%x, latency:0x%x, supervision:0x%x\r\n",
        SLE_MOUSE_DONGLE_SERVER_LOG, conn_id, param->interval, param->latency, param->supervision);
    osal_printk("[mouse server] sle_connect_param_update_cbk:0x%x\r\n", status);
}

static void sle_low_latency_cb(uint8_t status, sle_addr_t *addr, uint8_t rate)
{
    uint16_t report_rate = get_report_rate(rate);
#if ((CHIP_BS26 == 1) || (CHIP_BS22 == 1))
    if (status == 0) {
        osal_printk("sle low latency callback,status:%d update local latency 0,200\r\n", status);
        sle_update_local_latency(get_mouse_sle_conn_id(), 0, 200); // sle_update_local_latency 200
    }
#else
    if (status == 1 && report_rate == SLE_LOW_LATENCY_3HZ) {
        set_app_mouse_work_report_rate(get_app_mouse_curr_report_rate());
        osal_printk("sle low latency callback,status:%d update local latency 0,0xFFFF\r\n", status);
        sle_update_local_latency(get_mouse_sle_conn_id(), 0, 0xFFFF);
    }
    if (status == 0 && mouse_is_connected(g_pair_info.mouse_channel)) {
        stop_mouse_conn_process(g_pair_info.mouse_channel);
    }
#endif
    set_app_mouse_curr_report_rate(report_rate);
    osal_printk("sle low latency callback,addr:%02x:**:**:**:%02x:%02x,rate:%dHz,status:%d\r\n",
        addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5], report_rate, status);
}

void app_mouse_sle_disc_process(uint8_t disc_reason)
{
    switch (disc_reason) {
        case SLE_DISCONNECT_BY_REMOTE_USER:
        case SLE_ESTABLISH_CONNECT_FAIL:
        case SLE_DISCONNECT_BY_CONNECT_TIMEOUT:
            app_mouse_send_msg(MSG_ID_DISCONNECTED, MSG_TYPE_DISCONN_BY_REMOTE);
            break;
        default:
            osal_printk("sle disconnect,reason:0x%x.\n", disc_reason);
            break;
    }
}

static void sle_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                          sle_acb_state_t conn_state, sle_pair_state_t pair_state,
                                          sle_disc_reason_t disc_reason)
{
    osal_printk("%s connect state changed conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, disc_reason:0x%x\r\n",
                SLE_MOUSE_DONGLE_SERVER_LOG, conn_id, conn_state, pair_state, disc_reason);
    osal_printk("sle connect state changed callback,addr:%02x:**:**:**:%02x:%02x\r\n",
        addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);

    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        g_mouse_sle_conn_id = conn_id;
        if (memcpy_s(&g_sle_addr, sizeof(sle_addr_t), addr, sizeof(sle_addr_t)) != ERRCODE_SUCC) {
            osal_printk("memcpy_s sle addr fail!\r\n");
        }
        g_pair_info.channel_info[g_pair_info.mouse_channel].connect_flag = SLE_CONN;
        app_mouse_send_msg(MSG_ID_CONNECTED, MSG_TYPE_ENABLE_DATA_SEND);
        app_mouse_send_msg(MSG_ID_CONNECTED, MSG_TYPE_DATA_SYNC);
        osal_printk("sle connect success.\n");
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        g_sle_remote_device_type = SLE_REMOTE_DEV_PC;
        g_pair_info.status = BSLE_MOUSE_DISCONNECTED;
        g_pair_info.channel_info[g_pair_info.mouse_channel].connect_flag = NONE_CONN;
        app_mouse_sle_disc_process(disc_reason);
    }

    g_sle_mouse_server_conn_state = conn_state;
}

static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("[uuid server] pair complete conn_id:%02x, status:%x\r\n", conn_id, status);
    osal_printk("[uuid server] pair complete addr:%02x:**:**:**:%02x:%02x\r\n",
        addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    if (status == ERRCODE_SLE_SUCCESS) {
        sle_pair_complete_process(conn_id, addr);
        osal_printk("sle pair success.");
    }
    g_mouse_sle_pair_status = status;
}

static void sle_get_rmt_private_feature(uint16_t conn_id, errcode_t status, const sle_remote_private_feature_t *param)
{
    osal_printk("sle get rmt private feature, conn_id:%x status:%x\r\n", conn_id, status);
    if (status == ERRCODE_SLE_SUCCESS &&
        (((param->private_feature[0] >> SLE_PRIVATE_FEATURE_8K) & 1U) ||
        ((param->private_feature[1] >> SLE_PRIVATE_FEATURE_2K) & 1U))) {
        osal_printk("remote device support 2K or 8K\r\n");
        g_sle_remote_device_type = SLE_REMOTE_DEV_DONGLE;
    }
    if (g_sle_remote_device_type == SLE_REMOTE_DEV_PC) {
        sle_low_latency_set_em_data(conn_id, true);
    }
}

/*
 * SLE 默认首个通道只能连接Dongle，其余通道只能连接Pad/PC
*/
static void sle_mouse_set_nv_smp_keys(const sle_addr_t *addr, const sle_auth_info_evt_t* evt)
{
    sle_addr_t local_addr;
    local_addr.type = SLE_ADDRESS_TYPE_PUBLIC;
    if (memcpy_s(local_addr.addr, SLE_ADDR_LEN,
        g_pair_info.channel_info[g_pair_info.mouse_channel].sle_mac, SLE_ADDR_LEN) != EOK) {
        return;
    }
    
    sle_addr_t peer_addr;
    peer_addr.type = addr->type;
    if (memcpy_s(peer_addr.addr, SLE_ADDR_LEN, addr->addr, SLE_ADDR_LEN) != EOK) {
        return;
    }

    sle_auth_info_evt_t keys;
    if (memcpy_s(&keys, sizeof(sle_auth_info_evt_t), evt, sizeof(sle_auth_info_evt_t)) != EOK) {
        return;
    }
    if (g_sle_remote_device_type == SLE_REMOTE_DEV_DONGLE) {
        if (g_pair_info.mouse_channel == 0) {
            /* sle dongle smp keys save to nv idx 0 */
            sle_set_nv_smp_keys(&keys, &local_addr, &peer_addr, 0);
        } else {
            osal_printk("[Error]:This Channel Only For Pad/PC!!!\n");
            mac_change_recover(g_pair_info.mouse_channel);
            save_all_pair_info();
        }
    } else if (g_sle_remote_device_type == SLE_REMOTE_DEV_PC) {
        /* sle dev_pc smp keys save to nv idx 1 2 */
        if (g_pair_info.mouse_channel == 0) {
            osal_printk("[Error]:This Channel Only For Dongle!!!\n");
            mac_change_recover(g_pair_info.mouse_channel);
            save_all_pair_info();
        } else {
            sle_set_nv_smp_keys(&keys, &local_addr, &peer_addr, g_pair_info.mouse_channel);
        }
    }
}

static void sle_auth_complete_cbk(uint16_t conn_id, const sle_addr_t *addr,
    errcode_t status, const sle_auth_info_evt_t* evt)
{
    unused(conn_id);
    unused(status);
    osal_printk("auth_complete_cb enter\r\n");
    sle_mouse_set_nv_smp_keys(addr, evt);
}

static void sle_conn_register_cbks(void)
{
    sle_connection_callbacks_t conn_cbks = { 0 };
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb         = sle_pair_complete_cbk;
    conn_cbks.auth_complete_cb         = sle_auth_complete_cbk;
    conn_cbks.connect_param_update_cb  = sle_connect_param_update_cbk;
    conn_cbks.low_latency_cb           = sle_low_latency_cb;
    conn_cbks.remote_private_feature   = sle_get_rmt_private_feature;
    sle_connection_register_callbacks(&conn_cbks);
}

/************************** sle ssap 功能模块适配 Start **************************/
static uint8_t sle_get_ssap_server_id(void)
{
    return g_mouse_ssap_server_id;
}

errcode_t sle_mouse_init_ssap_server(void)
{
    /* 注册ssap server，应用id使用全局配置 */
    sle_uuid_t app_uuid = {
        .len = sizeof(g_mouse_app_uuid),
    };

    errno_t sec_ret = memcpy_s(app_uuid.uuid, sizeof(app_uuid.uuid), g_mouse_app_uuid, sizeof(g_mouse_app_uuid));
    if (sec_ret != EOK) {
        osal_printk("[Error]:mouse app,sle reg ssap server fail,memcpy ret:%d\n", sec_ret);
        return ERRCODE_SLE_MEMCPY_FAIL;
    }

    errcode_t ret = ssaps_register_server(&app_uuid, &g_mouse_ssap_server_id);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[Error]:mouse app,sle reg ssap server fail,reg ret:%x\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    return ERRCODE_SLE_SUCCESS;
}

/************************** service 功能模块适配 Start **************************/
/* 对外提供的发送接口 */
errcode_t sle_mouse_send_notify_indicate(uint8_t *report, uint16_t len)
{
    if (report == NULL) {
        return ERRCODE_SLE_FAIL;
    }

    return sle_hid_mouse_server_send_input_report(sle_get_ssap_server_id(), get_mouse_sle_conn_id(), report, len);
}

/* 对外提供channel服务的notify发送接口 */
errcode_t sle_mouse_send_channel_notify_indicate(uint8_t *report, uint16_t len)
{
    if (report == NULL) {
        return ERRCODE_SLE_FAIL;
    }

    return sle_mouse_channel_send_notification(sle_get_ssap_server_id(), get_mouse_sle_conn_id(), report, len);
}

/************************** service 功能模块适配 END **************************/

void close_sle_mode(void)
{
    if (g_sle_mouse_server_conn_state == SLE_ACB_STATE_CONNECTED) {
        sle_disconnect_remote_device(&g_sle_addr);
        g_sle_mouse_active_disconnect = true;
    } else if (mouse_is_adving(g_pair_info.mouse_channel)) {
        app_mouse_sle_stop_announce();
    }
}

void restore_sle_mode_seting(void)
{
    g_sle_mouse_active_disconnect = false;
}

/* 初始化鼠标依赖的服务 */
void sle_mouse_services_init(void)
{
    /* 初始化ssap server */
    errcode_t ret = sle_mouse_init_ssap_server();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[Error]:sle mouse init ssap server service failed:%d\n", ret);
        return;
    }

    /* 添加HID Server服务 */
    ret = sle_mouse_hid_server_init(sle_get_ssap_server_id());
    if (ret != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(sle_get_ssap_server_id());
        osal_printk("[Error]:sle mouse init hid server service failed:%d\n", ret);
        return;
    }

    /* 添加DIS Server服务 */
    ret = sle_mouse_dis_server_init(sle_get_ssap_server_id());
    if (ret != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(sle_get_ssap_server_id());
        osal_printk("[Error]:sle mouse init dis server service failed:%d\n", ret);
        return;
    }

    /* 添加BAS Server服务 */
    ret = sle_mouse_bas_server_init(sle_get_ssap_server_id());
    if (ret != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(sle_get_ssap_server_id());
        osal_printk("[Error]:sle mouse init bas server service failed:%d\n", ret);
        return;
    }

    /* 添加CHANNEL Server私有服务 */
    ret = sle_mouse_channel_server_init(sle_get_ssap_server_id());
    if (ret != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(sle_get_ssap_server_id());
        osal_printk("[Error]:sle mouse init channel server service failed:%d\n", ret);
    }
}

void mouse_inter_init(void)
{
    extern void mouse_inter_start(void);
    mouse_inter_start();
}

errcode_t app_sle_mouse_server_init(void)
{
    mouse_inter_init();
    app_mouse_sle_announce_register_cbks();
    sle_conn_register_cbks();
    osal_printk("%s init ok\r\n", SLE_MOUSE_DONGLE_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}
