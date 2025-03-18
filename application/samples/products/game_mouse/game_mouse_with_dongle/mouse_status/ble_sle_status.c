/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved.
 * Description: BT PUBLIC API module.
 */
#include "cmsis_os2.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "debug_print.h"
#include "securec.h"
#include "bts_le_gap.h"
#include "bts_device_manager.h"
#include "bts_gatt_server.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_ssap_server.h"
#include "bts_def.h"
#include "errcode.h"
#include "nv.h"
#include "sle_common.h"
#include "bts_le_gap.h"
#include "sle_errcode.h"
#include "osal_errno.h"
#include "osal_timer.h"
#include "ble_mouse_server_adv.h"
#include "sle_mouse_server_adv.h"
#include "mouse_mode_manage.h"
#include "mac_rules.h"
#include "ble_sle_status.h"

#define BSLE_PAIR_TIMEOUT 60000  // 60000ms = 60s
extern void app_mouse_pair_timer_cbk(unsigned long arg);
static osal_timer g_app_mouse_pair_timer = {0, app_mouse_pair_timer_cbk, 0xff, MOUSE_EVENT_INTERVAL_MS};

mouse_pair_info_t g_pair_info;
static uint16_t g_pair_timesec = 0;

__attribute__((section(".PMRAMCODE"))) void app_mouse_pair_timer_reset(void)
{
    g_pair_timesec = 0;
}

void printf_mac_info(uint8_t *mac)
{
    osal_printk("%02x:*:*:%02x:%02x:%02x \n", mac[0], mac[3], mac[4], mac[5]);  /* printf mac_addr 0 3 4 5 */
}

void mouse_pair_info_debug(void)
{
    osal_printk("mouse_paired_info:\n");
    osal_printk("ble_mac: ");
    printf_mac_info(g_pair_info.ble_mac);
    osal_printk("sle_mac: ");
    printf_mac_info(g_pair_info.sle_mac);
    osal_printk("curr_channel:%d, ble_loop_mac:%d, sle_loop_mac:%d.\n",
        g_pair_info.mouse_channel, g_pair_info.ble_loop_mac, g_pair_info.sle_loop_mac);

    for (uint8_t i = 0; i < CHANNEL_MAX_NUM; i++) {
        osal_printk("mouse_channel:%d.\n", i);
        osal_printk("ble_mac: ");
        printf_mac_info(g_pair_info.channel_info[i].ble_mac);
        osal_printk("sle_mac: ");
        printf_mac_info(g_pair_info.channel_info[i].sle_mac);
        osal_printk("ch:%d, ble_peer_mac: ", i);
        printf_mac_info(g_pair_info.channel_info[i].ble_peer_mac);
        osal_printk("ch:%d, sle_peer_mac: ", i);
        printf_mac_info(g_pair_info.channel_info[i].sle_peer_mac);
    }
}

static void set_mouse_adv_type(uint8_t channel, uint8_t adv_type)
{
    g_pair_info.channel_info[channel].adv_flag = adv_type;
}

static uint8_t get_mouse_adv_type(uint8_t channel)
{
    return g_pair_info.channel_info[channel].adv_flag;
}

uint8_t mouse_is_adving(uint8_t channel)
{
    if (g_pair_info.channel_info[channel].adv_flag != NONE_ADV) {
        return 1;
    }
    return 0;
}

uint32_t stop_mouse_adv_process(uint8_t channel)
{
    uint32_t ret = 0;
    if (get_mouse_adv_type(channel) == BLE_ADV) {
        ret = ble_mouse_stop_adv();
        if (ret != 0) {
            osal_printk("stop ble adv fail, ret = %d\n", ret);
        }
    } else if (get_mouse_adv_type(channel) == SLE_ADV) {
        ret = app_mouse_sle_stop_announce();
        if (ret != 0) {
            osal_printk("stop sle adv fail, ret = %d\n", ret);
        }
    }
    return ret;
}

int mouse_disconnect_ble(uint8_t channel)
{
    int ret = 0;
    if (channel >= CHANNEL_MAX_NUM) {
        return -1;
    }

    bd_addr_t ble_addr;
    ble_addr.type = g_pair_info.channel_info[channel].ble_central_type;
    if (memcpy_s(ble_addr.addr, BD_ADDR_LEN, g_pair_info.channel_info[channel].ble_peer_mac, BD_ADDR_LEN) != EOK) {
        osal_printk("%s copy fail\n", __func__);
        return -1;
    }
    ret = gap_ble_disconnect_remote_device((bd_addr_t *)&ble_addr);
    if (ret != ERRCODE_SUCC) {
        osal_printk("mouse_disconnect_ble type:%d ret:0x%x\n", ble_addr.type, ret);
    }
    return ret;
}

int mouse_disconnect_sle(uint8_t channel)
{
    int ret = 0;
    if (channel >= CHANNEL_MAX_NUM) {
        return -1;
    }

    sle_addr_t sle_addr;
    sle_addr.type = g_pair_info.channel_info[channel].sle_central_type;
    if (memcpy_s(sle_addr.addr, BD_ADDR_LEN, g_pair_info.channel_info[channel].sle_peer_mac, BD_ADDR_LEN) != EOK) {
        osal_printk("%s copy fail\n", __func__);
        return -1;
    }
    ret = sle_disconnect_remote_device((sle_addr_t *)&sle_addr);
    if (ret != ERRCODE_SUCC) {
        osal_printk("mouse_disconnect_sle type:%d ret:0x%x\n", sle_addr.type, ret);
    }
    return ret;
}

uint8_t mouse_is_connected(uint8_t channel)
{
    if (g_pair_info.channel_info[channel].connect_flag != NONE_CONN) {
        return 1;
    }
    return 0;
}

int32_t stop_mouse_conn_process(uint8_t channel)
{
    int32_t ret = 0;
    if (g_pair_info.channel_info[channel].connect_flag == BLE_CONN) {
        ret = mouse_disconnect_ble(channel);
    } else if (g_pair_info.channel_info[channel].connect_flag == SLE_CONN) {
        ret = mouse_disconnect_sle(channel);
    }
    return ret;
}

int32_t mouse_pair_timer_enable(void)
{
    int ret = -1;
    if (g_app_mouse_pair_timer.data == 0xff) {
        ret = osal_timer_init(&g_app_mouse_pair_timer);
        osal_printk("g_app_mouse_pair_timer create ret:%x!\n", ret);
    }
    ret = osal_timer_start(&g_app_mouse_pair_timer);
    osal_printk("g_app_mouse_pair_timer start ret:%x!\n", ret);
    return ret;
}

int32_t mouse_pair_timer_disable(void)
{
    int32_t ret = osal_timer_stop(&g_app_mouse_pair_timer);
    osal_printk("g_app_mouse_pair_timer stop ret:%x!\n", ret);
    return ret;
}

static void check_mouse_pair_status(uint16_t *timesec, uint8_t status)
{
    if (*timesec >= BSLE_PAIR_TIMEOUT) {
        *timesec = 0;
        mac_change_recover(g_pair_info.mouse_channel);
        save_all_pair_info();
        stop_mouse_adv_process(g_pair_info.mouse_channel);  // 关闭广播
        set_mouse_adv_type(g_pair_info.mouse_channel, NONE_ADV);
        g_pair_info.status = status;
    }
}

static void update_mouse_pair_status(uint16_t *timesec, uint8_t status)
{
    *timesec = 0;
    g_pair_info.status = status;
}

void app_mouse_pair_timer_cbk(unsigned long arg)
{
    UNUSED(arg);
    g_pair_timesec = g_pair_timesec + MOUSE_EVENT_INTERVAL_MS;

    switch (g_pair_info.status) {
        case BSLE_MOUSE_CHANNEL_CHANGE_START:
            check_mouse_pair_status(&g_pair_timesec, BSLE_MOUSE_CHANNEL_CHANGE_STOP);
            break;
        case BSLE_MOUSE_CHANNEL_CHANGE_RESTART:
            update_mouse_pair_status(&g_pair_timesec, BSLE_MOUSE_CHANNEL_CHANGE_START);
            break;
        case BSLE_MOUSE_PAIR_START:
            check_mouse_pair_status(&g_pair_timesec, BSLE_MOUSE_PAIR_STOP);
            break;
        case BSLE_MOUSE_PAIR_RESTART:
            update_mouse_pair_status(&g_pair_timesec, BSLE_MOUSE_PAIR_START);
            break;
        case BSLE_MOUSE_CHANNEL_CHANGE_STOP:
        case BSLE_MOUSE_PAIR_SUCC:
        case BSLE_MOUSE_PAIR_STOP:
        case BSLE_MOUSE_CONNECTED:
        case BSLE_MOUSE_DISCONNECTED:
            mouse_pair_timer_disable();
            return;
        default:
            osal_printk("app_mouse_pair_timer_cbk can not find msg id, status:%d\n", g_pair_info.status);
            break;
    }
    
    osal_timer_start(&g_app_mouse_pair_timer);
}

static int ble_set_local_addr(uint8_t *mac, uint8_t type)
{
    bd_addr_t local_addr;
    local_addr.type = type;
    if (memcpy_s(local_addr.addr, BD_ADDR_LEN, mac, BD_ADDR_LEN) != ERRCODE_BT_SUCCESS) {
        osal_printk("%s copy fail\n", __func__);
        return -1;
    }
    errcode_t ret = gap_ble_set_local_addr(&local_addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s fail 0x%x\n", __func__, ret);
        return ret;
    }
    return 0;
}

static int ble_is_pair(void)
{
    if (g_pair_info.channel_info[g_pair_info.mouse_channel].mode != MOUSE_MODE_BLE) {
        return 0;
    }
    if (g_pair_info.channel_info[g_pair_info.mouse_channel].ble_pair_flag != PAIR_BLE_SUCCESS) {
        return 0;
    }
    return 1;
}

static int set_sle_local_addr(uint8_t *mac, uint8_t type)
{
    sle_addr_t local_addr;
    local_addr.type = type;
    if (memcpy_s(local_addr.addr, BD_ADDR_LEN, mac, BD_ADDR_LEN) != ERRCODE_BT_SUCCESS) {
        osal_printk("%s copy fail\n", __func__);
        return -1;
    }
    errcode_t ret = sle_set_local_addr(&local_addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s fail 0x%x\n", __func__, ret);
        return ret;
    }
    return 0;
}

static int sle_is_pair(void)
{
    if (g_pair_info.channel_info[g_pair_info.mouse_channel].mode != MOUSE_MODE_SLE) {
        return 0;
    }
    if (g_pair_info.channel_info[g_pair_info.mouse_channel].sle_pair_flag != PAIR_SLE_SUCCESS) {
        return 0;
    }
    return 1;
}

uint32_t get_ble_pair_flag(void)
{
    return g_pair_info.channel_info[g_pair_info.mouse_channel].ble_pair_flag;
}

uint32_t ble_mouse_start_reconnect_adv(uint8_t adv_id, uint8_t channel)
{
    errcode_t n_ret = 0;
    gap_ble_adv_params_t adv_para = { 0 };

    if (channel >= CHANNEL_MAX_NUM) {
        return 0;
    }

    adv_para.min_interval = BLE_ADV_MIN_INTERVAL;
    adv_para.max_interval = BLE_ADV_MAX_INTERVAL;
    adv_para.channel_map = BLE_ADV_CHANNEL_MAP_CH_DEFAULT;
    adv_para.adv_filter_policy = BLE_ADV_FILTER_POLICY_SCAN_ANY_CONNECT_ANY;
    for (uint8_t i = 0; i < BD_ADDR_LEN; i++) {
        adv_para.peer_addr.addr[i] = g_pair_info.channel_info[channel].ble_peer_mac[BD_ADDR_LEN - 1 - i];
    }
    
    if (g_pair_info.channel_info[channel].ble_central_type == BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS) {
        osal_printk("===== send direct_adv === PUBLIC_DEVICE _ADDRESS =====\n");
        adv_para.peer_addr.type = BLE_PUBLIC_DEVICE_ADDRESS;
        adv_para.duration = BTH_GAP_BLE_DIRECT_ADV_FOREVER_DURATION;
        adv_para.adv_type = BLE_ADV_TYPE_CONNECTABLE_HIGH_DUTY_CYCLE_DIRECTED;
    } else if (g_pair_info.channel_info[channel].ble_central_type == BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS) {
        osal_printk("===== send direct_adv === RANDOM_DEVICE _ADDRESS =====\n");
        adv_para.peer_addr.type = BLE_RANDOM_DEVICE_ADDRESS;
        adv_para.duration = BTH_GAP_BLE_ADV_FOREVER_DURATION;
        adv_para.adv_type = BLE_ADV_TYPE_CONNECTABLE_UNDIRECTED;
    } else {
        osal_printk("central_addr_type error!\n");
    }

    n_ret = gap_ble_set_adv_param(adv_id, &adv_para);
    if (n_ret != 0) {
        osal_printk("reconnect set param error %x\n", n_ret);
        return n_ret;
    }

    n_ret = gap_ble_start_adv(adv_id);
    if (n_ret != 0) {
        osal_printk("reconnect set start error %x\n", n_ret);
        return n_ret;
    }
    return 0;
}

uint32_t ble_mouse_start_pair_adv(uint8_t adv_id, uint8_t channel)
{
    UNUSED(adv_id);
    UNUSED(channel);
    ble_mouse_start_adv();
    return 0;
}

#define SLE_ANNOUNCE_FLAG_DATA              0X01
#define SLE_ANNOUNCE_FLAG_DATA_LEN          0x03
#define SLE_ANNOUNCE_SUPPORT_SERVICE_LEN    0x06
#define SLE_MANUFACTURER_DATA_LEN           0x04
#define SLE_ANNOUNCE_LOCAL_NAME_DATA_TYPE   0X0B
#define SLE_ANNOUNCE_SUPPORT_SERVICE_TYPE   0X05
#define EXT_ANNOUNCE_OR_SCAN_RSP_DATA_LEN   64
uint32_t sle_set_mouse_annonuce_data(uint8_t adv_id, uint8_t channel)
{
    uint8_t announce_data_len = 0;
    uint8_t scan_rsp_data_len = 0;
    uint8_t set_announce_data[EXT_ANNOUNCE_OR_SCAN_RSP_DATA_LEN] = { 0 };
    uint8_t set_scan_rsp_data[EXT_ANNOUNCE_OR_SCAN_RSP_DATA_LEN] = { 0 };
    sle_announce_data_t cfg_announce_data = {0};

    uint8_t adv_data[] = {
        SLE_ANNOUNCE_FLAG_DATA,
        0x01, 0x01,
        SLE_ANNOUNCE_SUPPORT_SERVICE_TYPE,
        0x04, 0x0B, 0x06, 0x09, 0x06,
        SLE_ADV_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA,
        0x02, 0xAA, 0xAA,
        /* single service uuid + datas */
        0x03,
        0x16,                         /* length */
        0x09, 0x06,                   /* DIS service uuid */
        0x07, 0x03, 0x02, 0x05, 0x00, /* appearance */
        0x06, 0x0d, 'T', 'u', 'r', 'n', 'k', 'e', 'y', '_', 'm', 'o', 'u', 's', 'e' /* device name */
    };

    if (channel == 0) {
        // channel-0 用于和Dongle设备连接 不会被互联互通设备发现
        adv_data[SLE_ANNOUNCE_FLAG_DATA_LEN - 1] = 0;
    } else {
        // channel-1, channel-2 用于和互联互通设备连接 不被Dongle设备发现
        adv_data[SLE_ANNOUNCE_FLAG_DATA_LEN + SLE_ANNOUNCE_SUPPORT_SERVICE_LEN + SLE_MANUFACTURER_DATA_LEN - 1] = 0;
    }

    if (memcpy_s(&set_announce_data[announce_data_len], sizeof(adv_data), adv_data, sizeof(adv_data)) != EOK) {
        return -1;
    }
    announce_data_len += sizeof(adv_data);

    cfg_announce_data.announce_data = set_announce_data;
    cfg_announce_data.announce_data_len = announce_data_len;
    
    uint16_t name_len = strlen(TURNKEY_DEVICE_NAME);
    set_scan_rsp_data[scan_rsp_data_len] = SLE_ANNOUNCE_LOCAL_NAME_DATA_TYPE;
    scan_rsp_data_len++;
    set_scan_rsp_data[scan_rsp_data_len] = name_len;
    scan_rsp_data_len++;
    if (memcpy_s(&set_scan_rsp_data[scan_rsp_data_len], name_len, (uint8_t *)TURNKEY_DEVICE_NAME, name_len) != EOK) {
        return -1;
    }
    scan_rsp_data_len += name_len;

    cfg_announce_data.seek_rsp_data = set_scan_rsp_data;
    cfg_announce_data.seek_rsp_data_len = scan_rsp_data_len;

    if (sle_set_announce_data(adv_id, &cfg_announce_data)) {
        return 0;
    }
    return -1;
}

uint32_t sle_mouse_start_reconnect_adv(uint8_t adv_id, uint8_t channel)
{
    sle_announce_param_t param = {0};
    param.announce_mode         = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle       = adv_id;
    param.announce_gt_role      = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level        = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map  = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announce_interval_max = SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.conn_interval_min     = SLE_CONN_INTV_MIN_DEFAULT;
    param.conn_interval_max     = SLE_CONN_INTV_MAX_DEFAULT;
    param.conn_max_latency      = SLE_CONN_MAX_LATENCY;
    param.conn_supervision_timeout = SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT;
    if (memcpy_s(param.own_addr.addr, BD_ADDR_LEN, g_pair_info.channel_info[channel].sle_mac, BD_ADDR_LEN) != EOK) {
        osal_printk("set sle adv param memcpy addr fail\r\n");
        return ERRCODE_SLE_PARAM_ERR;
    }
    param.peer_addr.type = g_pair_info.channel_info[channel].sle_central_type;
    if (memcpy_s(param.peer_addr.addr, BD_ADDR_LEN,
        g_pair_info.channel_info[channel].sle_peer_mac, BD_ADDR_LEN) != EOK) {
        osal_printk("set sle adv param memcpy addr fail\r\n");
        return ERRCODE_SLE_PARAM_ERR;
    }

    sle_set_announce_param(param.announce_handle, &param);
    app_mouse_sle_start_announce();
    return 0;
}

uint32_t sle_mouse_start_pair_adv(uint8_t adv_id, uint8_t channel)
{
    sle_announce_param_t param = {0};
    param.announce_mode         = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle       = adv_id;
    param.announce_gt_role      = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level        = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map  = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announce_interval_max = SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.conn_interval_min     = SLE_CONN_INTV_MIN_DEFAULT;
    param.conn_interval_max     = SLE_CONN_INTV_MAX_DEFAULT;
    param.conn_max_latency      = SLE_CONN_MAX_LATENCY;
    param.conn_supervision_timeout = SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT;
    if (memcpy_s(param.own_addr.addr, BD_ADDR_LEN, g_pair_info.channel_info[channel].sle_mac, BD_ADDR_LEN) != EOK) {
        osal_printk("set sle adv param memcpy addr fail\r\n");
        return ERRCODE_SLE_PARAM_ERR;
    }
    sle_set_announce_param(param.announce_handle, &param);
    app_mouse_sle_start_announce();
    return 0;
}

int start_mouse_adv_process(uint8_t channel)
{
    osal_printk("start_mouse_adv_process enter.\n\n");
    uint8_t adv_id = 0;
    if (g_pair_info.channel_info[channel].mode == MOUSE_MODE_BLE) {
        adv_id = BTH_GAP_BLE_ADV_HANDLE_DEFAULT;
        ble_set_local_addr(g_pair_info.channel_info[channel].ble_mac, BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS);
        ble_mouse_set_adv_data(adv_id);

        if (ble_is_pair()) {
            set_mouse_adv_type(channel, BLE_ADV);
            osal_printk("ble hw mouse_channel:%d direct adv.\n", channel);
            if (ble_mouse_start_reconnect_adv(adv_id, channel) != 0) {
                osal_printk("ble hw mouse_channel:%d direct adv fail.\n", channel);
                return -1;
            }
            return 0;
        }

        if (ble_mouse_start_pair_adv(adv_id, channel) != 0) {
            osal_printk("ble hw mouse_channel:%d pair adv fail.\n", channel);
            return -1;
        }
        set_mouse_adv_type(channel, BLE_ADV);
    } else if (g_pair_info.channel_info[channel].mode == MOUSE_MODE_SLE) {
        adv_id = SLE_ADV_HANDLE_DEFAULT;
        sle_set_mouse_annonuce_data(adv_id, channel);
        set_sle_local_addr(g_pair_info.channel_info[channel].sle_mac, SLE_ADDRESS_TYPE_PUBLIC);

        if (sle_is_pair()) {
            set_mouse_adv_type(channel, SLE_ADV);
            osal_printk("sle hw mouse_channel:%d direct adv.\n", channel);
            if (sle_mouse_start_reconnect_adv(adv_id, channel) != 0) {
                osal_printk("sle hw mouse_channel:%d direct adv fail.\n", channel);
                return -1;
            }
            return 0;
        }

        if (sle_mouse_start_pair_adv(adv_id, channel) != 0) {
            osal_printk("sle hw mouse_channel:%d pair adv fail.\n", channel);
            return -1;
        }
        set_mouse_adv_type(channel, SLE_ADV);
    } else {
        osal_printk("mouse_channel:%d mode:%d error.\n", channel, g_pair_info.channel_info[channel].mode);
    }
    return 0;
}

void ble_pair_complete_process(uint16_t conn_id, const bd_addr_t *addr)
{
    uint8_t last_pair = g_pair_info.channel_info[g_pair_info.mouse_channel].ble_pair_flag;
    osal_printk("ble connect_id: %d central_addr_type: %x pair:%d\n", conn_id, addr->type, last_pair);

    g_pair_info.channel_info[g_pair_info.mouse_channel].ble_pair_flag = PAIR_BLE_SUCCESS;
    g_pair_info.channel_info[g_pair_info.mouse_channel].ble_central_type = addr->type;
    g_pair_info.channel_info[g_pair_info.mouse_channel].mode = MOUSE_MODE_BLE;
    if (memcpy_s(g_pair_info.channel_info[g_pair_info.mouse_channel].ble_peer_mac, BD_ADDR_LEN,
        addr->addr, BD_ADDR_LEN) != EOK) {
        osal_printk("%s copy fail\n", __func__);
        return;
    }

    save_all_pair_info();
    osal_printk("ble pair save data\n");
    g_pair_info.status = BSLE_MOUSE_PAIR_SUCC;
    return;
}

void sle_pair_complete_process(uint16_t conn_id, const sle_addr_t *addr)
{
    uint8_t last_pair = g_pair_info.channel_info[g_pair_info.mouse_channel].sle_pair_flag;
    osal_printk("sle connect_id: %d central_addr_type: %x pair:%d\n", conn_id, addr->type, last_pair);

    g_pair_info.channel_info[g_pair_info.mouse_channel].sle_pair_flag = PAIR_SLE_SUCCESS;
    g_pair_info.channel_info[g_pair_info.mouse_channel].sle_central_type = addr->type;
    g_pair_info.channel_info[g_pair_info.mouse_channel].mode = MOUSE_MODE_SLE;
    if (memcpy_s(g_pair_info.channel_info[g_pair_info.mouse_channel].sle_peer_mac, BD_ADDR_LEN,
        addr->addr, BD_ADDR_LEN) != EOK) {
        osal_printk("%s copy fail\n", __func__);
        return;
    }

    save_all_pair_info();
    osal_printk("sle pair save data\n");
    g_pair_info.status = BSLE_MOUSE_PAIR_SUCC;
    return;
}

void mouse_key_bsle_process(void)
{
    if ((g_pair_info.status == BSLE_MOUSE_PAIR_SUCC) ||
        (get_app_mouse_handle().current_mode == USB_MOUSE_MODE)) {
        return;
    }
    g_pair_info.status = BSLE_MOUSE_CHANNEL_CHANGE_RESTART;
    mouse_pair_timer_enable();
    start_mouse_adv_process(g_pair_info.mouse_channel);
    return;
}

void save_all_pair_info(void)
{
    uapi_nv_write(0x11, (uint8_t *)&(g_pair_info), sizeof(mouse_pair_info_t));
}