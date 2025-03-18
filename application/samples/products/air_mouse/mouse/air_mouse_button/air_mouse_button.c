/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \r\n
 *
 * Description: Air Mouse RCU Button Source. \n
 * Author: @CompanyNameTag \r\n
 * History: \r\n
 * 2024-01-23, Create file. \r\n
 */
#include "../../dongle/air_mouse_usb/usb_init_app.h"
#include "../../mouse/sle_air_mouse_server/sle_air_mouse_server.h"
#include "securec.h"
#include "keyscan.h"
#include "keyscan_porting.h"
#include "osal_debug.h"

// 鼠标按键
#define MOUSE_LEFT_CODE             1 // 鼠标左键

// HID键盘扫描码
#define KEYBOARD_HID_CODE_ESC       0X29  // ESC
#define KEYBOARD_HID_CODE_BACK      0X2A  // 回退
#define KEYBOARD_HID_CODE_ENTER     0X28  // 回车
#define KEYBOARD_HID_CODE_SPACE     0X2C  // 空格
#define KEYBOARD_HID_CODE_PRINTSCR  0X46  // 截屏
#define KEYBOARD_HID_CODE_HOME      0X4a  // 首页
#define KEYBOARD_HID_CODE_END       0X4d  // 结尾
#define KEYBOARD_HID_CODE_UP        0X52  // 上
#define KEYBOARD_HID_CODE_DOWN      0X51  // 下
#define KEYBOARD_HID_CODE_LEFT      0X50  // 左
#define KEYBOARD_HID_CODE_RIGHT     0X4F  // 右

#define PRESS_BUTTON                1 // 按键按下
#define RELEASE_BUTTON              0 // 按键释放

#define KEYSCAN_ROW                 4 // 键值矩阵的行数
#define KEYSCAN_COL                 4 // 键值矩阵的列数

// 按键序号
#define K1                          1
#define K2                          2
#define K3                          3
#define K4                          4
#define K5                          5
#define K6                          6
#define K7                          7
#define K8                          8
#define K9                          9
#define K10                         10
#define K11                         11
#define K12                         12
#define K13                         13
#define T1                          14

#define GPIO_MAP_NUM                9   // keyscan使用的gpio管脚数量
#define GPIO_COL_1                  16  // COL1 gpio管脚号
#define GPIO_COL_2                  21  // COL2 gpio管脚号
#define GPIO_COL_3                  6   // COL3 gpio管脚号
#define GPIO_COL_4                  5   // COL4 gpio管脚号
#define GPIO_ROW_1                  24  // ROW1 gpio管脚号
#define GPIO_ROW_2                  23  // ROW2 gpio管脚号
#define GPIO_ROW_3                  22  // ROW3 gpio管脚号
#define GPIO_ROW_4                  2   // ROW3 gpio管脚号

// Keyscan的gpio矩阵，ROW在前，COL在后
static const uint8_t user_gpio_map[GPIO_MAP_NUM] = {
    GPIO_ROW_1, GPIO_ROW_2, GPIO_ROW_3, GPIO_ROW_4,
    GPIO_COL_1, GPIO_COL_2, GPIO_COL_3, GPIO_COL_4
};

// Keyscan的键值矩阵
static const uint8_t g_key_map[KEYSCAN_ROW][KEYSCAN_COL] = {
    { K1, K2, K3, K4 },
    { K5, K6, K7, K8 },
    { K9, K10, K11, K12 },
    { K13, T1 },
};

hid_kind_mode g_last_kind = 0;  // 上一次按下的按键类型

// 按键序号对应的值
static uint8_t g_value_map[T1 + 1] = {};

void init_value_map(void)
{
    g_value_map[K1] = MOUSE_LEFT_CODE;
    g_value_map[K2] = MOUSE_LEFT_CODE;
    g_value_map[K3] = KEYBOARD_HID_CODE_LEFT;
    g_value_map[K4] = KEYBOARD_HID_CODE_ESC;
    g_value_map[K5] = MOUSE_LEFT_CODE;
    g_value_map[K6] = KEYBOARD_HID_CODE_DOWN;
    g_value_map[K7] = MOUSE_LEFT_CODE;
    g_value_map[K8] = MOUSE_LEFT_CODE;
    g_value_map[K9] = KEYBOARD_HID_CODE_UP;
    g_value_map[K10] = MOUSE_LEFT_CODE;
    g_value_map[K11] = MOUSE_LEFT_CODE;
    g_value_map[K12] = KEYBOARD_HID_CODE_HOME;
    g_value_map[K13] = KEYBOARD_HID_CODE_RIGHT;
    g_value_map[T1] = MOUSE_LEFT_CODE;
}

// 按键释放处理
static void proc_release_button(uint8_t key)
{
    if (g_last_kind == HID_MOUSE_ABS_KIND) {
        sle_hid_mouse_server_send_mouse_key_report(key, RELEASE_BUTTON);
    } else if (g_last_kind == HID_KEYBOARD_KIND) {
        sle_hid_mouse_server_send_keyboard_report(key, RELEASE_BUTTON);
    }
}

// 按键按下处理
static void proc_press_button(uint8_t key, uint8_t value)
{
    if (value == MOUSE_LEFT_CODE) {
        g_last_kind = HID_MOUSE_ABS_KIND;
        sle_hid_mouse_server_send_mouse_key_report(key, PRESS_BUTTON);
    } else {
        g_last_kind = HID_KEYBOARD_KIND;
        sle_hid_mouse_server_send_keyboard_report(key, value);
    }
}

// keyscan回调函数
static int air_mouse_keyscan_callback(int key_num, uint8_t key_values[])
{
    osal_printk("key_values %d:", key_num);
    for (int i = 0; i < key_num; i++) {
        osal_printk("%u, %u|", key_values[i], g_value_map[key_values[i]]);
    }
    osal_printk("\r\n");

    // 不处理同时按下多个按键的情况
    if (key_num > 1) {
        osal_printk("key_num > 1\r\n");
        return 0;
    }

    if (key_num == 0) {
        proc_release_button(key_values[0]); // 按键释放处理
    } else if (key_num == 1) {
        proc_press_button(key_values[0], g_value_map[key_values[0]]); // 按下按键处理
    } else {
        osal_printk("[ERR] key_num < 0\r\n");
    }

    return 0;
}

void air_mouse_button_init(void)
{
    init_value_map();
    // 设置Keyscan的gpio矩阵
    keyscan_porting_set_gpio((uint8_t *)user_gpio_map);
    // 设置Keyscan的键值矩阵
    uapi_set_keyscan_value_map((uint8_t **)g_key_map, KEYSCAN_ROW, KEYSCAN_COL);
    // 扫描时长、扫描模式、中断类型
    uapi_keyscan_init(EVERY_ROW_PULSE_40_US, HAL_KEYSCAN_MODE_0, KEYSCAN_INT_VALUE_RDY);
    // 注册keyscan回调函数
    uapi_keyscan_register_callback(air_mouse_keyscan_callback);
    // 使能keyscan扫描
    uapi_keyscan_enable();
}
