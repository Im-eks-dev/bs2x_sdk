/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved.
 * Description: BT PUBLIC API module.
 */
#include "cmsis_os2.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "osal_timer.h"
#include "common_def.h"
#include "securec.h"
#include "ble_sle_status.h"
#include "mouse_mode_manage.h"
#include "mac_rules.h"

typedef struct {
    uint8_t sle_mac;
    uint8_t ble_mac;
    uint8_t channel;
    uint8_t mode;
    uint8_t ble_last_pair;
    uint8_t sle_last_pair;
    uint8_t stored;
} channel_info_t;
static channel_info_t g_channel_info;

void mac_change_recover(uint8_t channel)
{
    if (channel >= CHANNEL_MAX_NUM) {
        return;
    }
    if (g_channel_info.stored == 0) {
        return;
    }
    g_pair_info.channel_info[channel].mode = g_channel_info.mode;
    g_pair_info.channel_info[channel].ble_mac[ROLLING_MAC_ADDR_POS] = g_channel_info.ble_mac;
    g_pair_info.channel_info[channel].sle_mac[ROLLING_MAC_ADDR_POS] = g_channel_info.sle_mac;
    g_channel_info.stored = 0;
}

void channel_info_store(void)
{
    g_channel_info.channel  = g_pair_info.mouse_channel;
    g_channel_info.ble_last_pair = g_pair_info.channel_info[g_pair_info.mouse_channel].ble_pair_flag;
    g_channel_info.sle_last_pair = g_pair_info.channel_info[g_pair_info.mouse_channel].sle_pair_flag;
    g_channel_info.mode     = g_pair_info.channel_info[g_pair_info.mouse_channel].mode;
    g_channel_info.ble_mac  = g_pair_info.channel_info[g_pair_info.mouse_channel].ble_mac[ROLLING_MAC_ADDR_POS];
    g_channel_info.sle_mac  = g_pair_info.channel_info[g_pair_info.mouse_channel].sle_mac[ROLLING_MAC_ADDR_POS];
    g_channel_info.stored   = 1;
}

static void mac_change_ruler(uint8_t type, uint8_t *mac)
{
    uint8_t *orig_mac = NULL;
    uint8_t loop_mac = 0;
    if (type == 0) {
        orig_mac = g_pair_info.ble_mac;
        loop_mac = g_pair_info.ble_loop_mac;
    } else if (type == 1) {
        orig_mac = g_pair_info.sle_mac;
        loop_mac = g_pair_info.sle_loop_mac;
    } else {
        osal_printk("invalid type:%d\n", type);
    }

    for (uint8_t i = 0; i < BD_ADDR_LEN; i++) {
        mac[i] = orig_mac[i];
    }
    loop_mac = loop_mac + 1;
    mac[ROLLING_MAC_ADDR_POS] = ((g_pair_info.mouse_channel + 1) << 6) | ((loop_mac) & 0x3f); /* 0x3f对应偏移6位 */
    return;
}

void check_mouse_channel_mac(uint8_t channel)
{
    if (channel >= CHANNEL_MAX_NUM) {
        return;
    }

    uint8_t zero_mac[BD_ADDR_LEN] = {0};
    if (memcmp(g_pair_info.channel_info[channel].ble_mac, zero_mac, BD_ADDR_LEN) == 0) {
        mac_change_ruler(0, g_pair_info.channel_info[g_pair_info.mouse_channel].ble_mac);
    }

    if (memcmp(g_pair_info.channel_info[channel].sle_mac, zero_mac, BD_ADDR_LEN) == 0) {
        mac_change_ruler(1, g_pair_info.channel_info[g_pair_info.mouse_channel].sle_mac);
    }
}

void set_mouse_channel_mac(uint8_t channel, uint8_t mode)
{
    if (channel >= CHANNEL_MAX_NUM) {
        return;
    }

    if (mode == BLE_MOUSE_MODE) {
        mac_change_ruler(0, g_pair_info.channel_info[g_pair_info.mouse_channel].ble_mac);
    } else if (mode == SLE_MOUSE_MODE) {
        mac_change_ruler(1, g_pair_info.channel_info[g_pair_info.mouse_channel].sle_mac);
    }
}