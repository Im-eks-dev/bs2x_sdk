/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: rapoo Keyscan Source File. \n
 * Author: @CompanyNameMagicTag \n
 * History: \n
 * 2024-09-25, Create file. \n
 */

#include "keyscan.h"
#include "common_def.h"
#include "osal_debug.h"
#include "osal_timer.h"
#include "ble_sle_status.h"
#include "mouse_mode_manage.h"
#include "mouse_key.h"

#define MOUSE_PAIR_TIMER_INTERVAL 30
#define MOUSE_PAIR_TIMER_LIMIT 100

static mouse_key_t *g_mouse_key = NULL;

static uint16_t pair_timer_cnt;
static bool g_pair_key_pressed;
static void app_mouse_pair_cbk(unsigned long arg);

static const uint8_t g_key_map[CONFIG_KEYSCAN_ENABLE_ROW][CONFIG_KEYSCAN_ENABLE_COL] = {
    {MOUSE_DPI_UP, MOUSE_CHANNEL_KEY, MOUSE_DPI_DOWN},
    {MOUSE_FORWARD_KEY, MOUSE_BACKWARD_KEY, MOUSE_RATE_KEY},
    {MOUSE_MID_KEY, MOUSE_RIGHT_KEY, MOUSE_LEFT_KEY},
};

static osal_timer g_mouse_pair_timer = {0, app_mouse_pair_cbk, 0, MOUSE_PAIR_TIMER_INTERVAL};

static void app_mouse_pair_cbk(unsigned long arg)
{
    unused(arg);
    if (g_pair_key_pressed) {
        pair_timer_cnt++;
        if (pair_timer_cnt > MOUSE_PAIR_TIMER_LIMIT) {
            pair_timer_cnt = 0;
            app_mouse_send_msg(MSG_ID_BUTTON_MANAGE, MSG_TYPE_PAIR_PROCESS);
            osal_timer_stop(&g_mouse_pair_timer);
        } else {
            osal_timer_start(&g_mouse_pair_timer);
        }
    } else {
        if (pair_timer_cnt < MOUSE_PAIR_TIMER_LIMIT) {
            pair_timer_cnt = 0;
            app_mouse_send_msg(MSG_ID_BUTTON_MANAGE, MSG_TYPE_CHANNEL_CHANGE);
            osal_timer_stop(&g_mouse_pair_timer);
        }
    }
}

static void app_mouse_pair_key_process(void)
{
    int ret;
    ret = osal_timer_start(&g_mouse_pair_timer);
    if (ret != 0) {
        osal_printk("pair_timer start faild! \r\n");
    }
}

uint8_t switch_one_key_value(uint8_t key_value)
{
    switch (key_value) {
        case MOUSE_DPI_UP:
            app_mouse_send_msg(MSG_ID_BUTTON_MANAGE, MSG_TYPE_DPI_UP);
            break;
        case MOUSE_DPI_DOWN:
            app_mouse_send_msg(MSG_ID_BUTTON_MANAGE, MSG_TYPE_DPI_DOWN);
            break;
        case MOUSE_CHANNEL_KEY:
            g_pair_key_pressed = true;
            app_mouse_pair_key_process();
            break;
        case MOUSE_RATE_KEY:
            app_mouse_send_msg(MSG_ID_BUTTON_MANAGE, MSG_TYPE_REPORT_RATE_CHANGE);
            break;
        case MOUSE_MID_KEY:
            g_mouse_key->b.mid_key = 1;
            mouse_key_bsle_process();
            break;
        case MOUSE_RIGHT_KEY:
            g_mouse_key->b.right_key = 1;
            mouse_key_bsle_process();
            break;
        case MOUSE_LEFT_KEY:
            g_mouse_key->b.left_key = 1;
            mouse_key_bsle_process();
            break;
        case MOUSE_FORWARD_KEY:
            g_mouse_key->b.forward_key = 1;
            break;
        case MOUSE_BACKWARD_KEY:
            g_mouse_key->b.backward_key = 1;
            break;
        default:
            break;
    }
    return g_mouse_key->d8;
}


static int rapoo_mouse_key_callback(int key_num, uint8_t key_value[])
{
    if (key_num == PRESS_NONE_KEY) {
        g_mouse_key->d8 = 0;
        g_pair_key_pressed = false;
    } else if (key_num == PRESS_ONE_KEY) {
        g_mouse_key->d8 = 0;
        switch_one_key_value(key_value[0]);
    } else if (key_num == PRESS_TWO_KEY) {
        g_mouse_key->d8 = switch_one_key_value(key_value[0]); // 记录第1个按键
        g_mouse_key->d8 |= switch_one_key_value(key_value[1]); // 记录第2个按键
    } else {
        // 不处理同时按下2个按键以上的情况
        osal_printk("key_num > 2\r\n");
        return 0;
    }
    return 1;
}

void mouse_key_init(mouse_key_t *mouse_key)
{
    /* keyscan init */
    g_mouse_key = mouse_key;
    uint8_t user_gpio_map[CONFIG_KEYSCAN_ENABLE_ROW + CONFIG_KEYSCAN_ENABLE_COL] = {22, 23, 24, 9, 10, 27};
    if (keyscan_porting_set_gpio(user_gpio_map)) {
        return;
    }

    pair_timer_cnt = 0;
    osal_timer_init(&g_mouse_pair_timer);

    uapi_set_keyscan_value_map((uint8_t **)g_key_map, CONFIG_KEYSCAN_ENABLE_ROW, CONFIG_KEYSCAN_ENABLE_COL);
    uapi_keyscan_init(EVERY_ROW_PULSE_40_US, HAL_KEYSCAN_MODE_1, KEYSCAN_INT_VALUE_RDY);
    uapi_keyscan_register_callback(rapoo_mouse_key_callback);
    uapi_keyscan_enable();
}