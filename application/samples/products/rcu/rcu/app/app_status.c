/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: APP STATUS SOURCE \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-5-21, Create file. \n
 */
#include "securec.h"
#include "common_def.h"
#include "app_common.h"
#include "app_msg_queue.h"
#include "app_status.h"

static app_globle_status_t g_app_cur_status;

app_globle_status_t get_app_globle_status(void)
{
    return g_app_cur_status;
}

void set_app_sys_status(APP_SYS_STATUS status)
{
    g_app_cur_status.app_sys_status = status;
}

void set_app_connect_status(APP_CONNECT_STATUS status)
{
    g_app_cur_status.app_connect_status = status;
    if (status == APP_CONNECT_STATUS_ADVING) {
        set_rcu_status(RCU_STATUS_ADV_SEND);
    } else {
        clear_rcu_status(RCU_STATUS_ADV_SEND);
    }
    if (status == APP_CONNECT_STATUS_DISCONNECT) {
        if ((get_rcu_status() & RCU_STATUS_OTA_UPGRADE)) {
            clear_rcu_status(RCU_STATUS_OTA_UPGRADE);
        }
    }
}

void set_app_adv_type(APP_ADV_TYPE type)
{
    g_app_cur_status.app_adv_type = type;
}

void set_app_adv_stop_reason(APP_STOP_ADV_TYPE app_adv_stop_reason)
{
    g_app_cur_status.app_adv_stop_reason = app_adv_stop_reason;
}

void set_app_disconnect_reason(APP_DISCONNECT_TYPE app_disconnect_reason)
{
    g_app_cur_status.app_disconnect_reason = app_disconnect_reason;
}

void set_rcu_status(uint8_t status)
{
    g_app_cur_status.rcu_status |= status;
    app_print("[SYS]set: 0x%x, now: 0x%x\r\n", status, g_app_cur_status.rcu_status);
}

void clear_rcu_status(uint8_t status)
{
    g_app_cur_status.rcu_status &= ~status;
    app_print("[SYS]clear: 0x%x, now: 0x%x\r\n", status, g_app_cur_status.rcu_status);

    if (g_app_cur_status.rcu_status == RCU_STATUS_IDLE) {
        app_msg_data_t msg;
        msg.type = SLEPP_CHECK_MSG;
        msg.length = 0;
        app_write_msgqueue(msg);
    }
}

uint8_t get_rcu_status(void)
{
    return g_app_cur_status.rcu_status;
}