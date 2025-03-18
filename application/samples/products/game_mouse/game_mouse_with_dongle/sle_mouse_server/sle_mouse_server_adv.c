/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse Server Adv Manager  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#include "securec.h"
#include "errcode.h"
#include "osal_addr.h"
#include "osal_debug.h"
#include "sle_errcode.h"
#include "sle_common.h"
#include "sle_device_discovery.h"
#include "mouse_mode_manage.h"
#include "sle_device_manager.h"
#include "sle_connection_manager.h"
#include "sle_mouse_server_adv.h"

static void sle_adv_announce_enable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("%s sle announce enable id:%02x, state:%02x\r\n", SLE_MOUSE_DONGLE_SERVER_LOG, announce_id, status);
}

static void sle_adv_announce_disable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("%s sle announce disable id:%02x, state:%02x\r\n", SLE_MOUSE_DONGLE_SERVER_LOG, announce_id, status);
}

static void sle_adv_announce_terminal_cbk(uint32_t announce_id)
{
    osal_printk("%s sle announce terminal id:%02x\r\n", SLE_MOUSE_DONGLE_SERVER_LOG, announce_id);
}

static void sle_mouse_server_enable_cbk(uint8_t status)
{
    osal_printk("%s sle enable status:%02x\r\n", SLE_MOUSE_DONGLE_SERVER_LOG, status);
    if (status == ERRCODE_SUCC) {
        app_mouse_send_msg(MSG_ID_PROTOCOL_ENABLED, 0);
    }
    sle_set_read_remote_private_feature(1);
    /* 开启主动指定SLE配对信息存放NV索引功能 */
    sle_set_save_pair_keys_mode(SLE_SAVE_SMP_KEYS_MANU);
}

void app_mouse_sle_announce_register_cbks(void)
{
    sle_dev_manager_callbacks_t dev_mgr_cbk = {0};
    sle_announce_seek_callbacks_t seek_cbks = {0};
    seek_cbks.announce_enable_cb = sle_adv_announce_enable_cbk;
    seek_cbks.announce_disable_cb = sle_adv_announce_disable_cbk;
    seek_cbks.announce_terminal_cb = sle_adv_announce_terminal_cbk;
    dev_mgr_cbk.sle_enable_cb = sle_mouse_server_enable_cbk;
    sle_dev_manager_register_callbacks(&dev_mgr_cbk);
    sle_announce_seek_register_callbacks(&seek_cbks);

    #if (CORE_NUMS < 2)
    enable_sle();
    #endif
}

void app_mouse_sle_start_announce(void)
{
    sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
}

errcode_t app_mouse_sle_stop_announce(void)
{
    errcode_t ret = sle_stop_announce(SLE_ADV_HANDLE_DEFAULT);
    if (ret != 0) {
        osal_printk("app_mouse_sle_stop_announce, sle stop adv fail, ret:%x\r\n", ret);
        return ret;
    }
    return 0;
}

