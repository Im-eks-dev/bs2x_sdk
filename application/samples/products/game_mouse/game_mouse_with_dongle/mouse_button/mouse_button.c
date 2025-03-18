/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 *
 * Description: Mouse Button SAMPLE. \n
 *
 * History: \n
 * 2024-04-21, Create file. \n
 */
#include "osal_debug.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "pinctrl.h"
#include "tcxo.h"
#include "osal_timer.h"
#include "nv.h"
#include "cpu_utils.h"
#include "hal_reboot.h"
#include "ble_sle_status.h"
#include "mouse_mode_manage.h"
#include "mouse_battery_manager.h"
#include "mouse_log.h"
#include "mouse_button.h"

#define DELAY_US200 200
// 按键抖动阈值:1-30ms
#define GAME_MOUSE_KEY_SW_DEBOUNCE_TIME_MS  12 // 5ms-20ms
#define GMAE_MOUSE_KEY_HW_DEBOUNCE_TIMES    1 // 1ms ~5ms
#define GAME_MOUSE_BUTTON_TIME_MS           20
#define GAME_MOUSE_BSLE_PAIR_TIME             3000    // 3000ms
#define GAME_MOUSE_CHANGE_MODE_TIME            3000    // 3000ms

#define BUTTON_TIMER_COUNT_10 10
#define BUTTON_TIMER_COUNT_50 50

#ifndef MOUSE_DEBOUNCE_NEW_METHOD
#define MOUSE_DEBOUNCE_NEW_METHOD   1
#endif
#define MOUSE_SLE_PAIR (LEFT | RIGHT | MID)
#define MOUSE_SLE_BLE_CHANGE_MOD (FORWARD | BACK | MID)

static mouse_key_t *g_mouse_key = NULL;
static mouse_gpio_t g_debounce_key;
static mouse_gpio_t g_debounce_temp_key;
#if (MOUSE_DEBOUNCE_NEW_METHOD)
static mouse_key_debounce_t g_mouse_lkey_debounce;
static mouse_key_debounce_t g_mouse_rkey_debounce;
static mouse_key_debounce_t g_mouse_mkey_debounce;
static mouse_key_debounce_t g_mouse_fbkey_debounce; // backward key
static mouse_key_debounce_t g_mouse_ffkey_debounce; // forward key
static mouse_key_debounce_t g_mouse_rrkey_debounce;
static uint8_t g_key_debounce_time = 0;
#ifdef CONFIG_DOUBLE_DPI_KEY
static debounce_state_t g_mouse_dpi_add_state; // 双键，DPI增加键状态
static debounce_state_t g_mouse_dpi_sub_state; // 双键，DPI降低键状态
#else
static debounce_state_t g_mouse_dpi_state; // 单键，DPI切换键状态
#endif
static uint8_t g_combined_left = 0;
static uint8_t g_combined_right = 0;
static uint8_t g_combined_mid = 0;
static uint8_t g_combined_forward = 0;
static uint8_t g_combined_back = 0;

static void app_mouse_lkey_timer_cbk(unsigned long arg);
static void app_mouse_rkey_timer_cbk(unsigned long arg);
static void app_mouse_mkey_timer_cbk(unsigned long arg);
#ifdef CONFIG_DOUBLE_DPI_KEY
static void app_mouse_dpi_add_timer_cbk(unsigned long arg);
static void app_mouse_dpi_sub_timer_cbk(unsigned long arg);
#else
static void app_mouse_dpikey_timer_cbk(unsigned long arg);
#endif
static void app_mouse_fbkey_timer_cbk(unsigned long arg);
static void app_mouse_ffkey_timer_cbk(unsigned long arg);
#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
static void app_mouse_rrkey_timer_cbk(unsigned long arg);
#endif

static osal_timer g_mouse_lkey_irqs_timer    = {0, app_mouse_lkey_timer_cbk, 0, GAME_MOUSE_KEY_SW_DEBOUNCE_TIME_MS};
static osal_timer g_mouse_rkey_irqs_timer    = {0, app_mouse_rkey_timer_cbk, 0, GAME_MOUSE_KEY_SW_DEBOUNCE_TIME_MS};
static osal_timer g_mouse_mkey_irqs_timer    = {0, app_mouse_mkey_timer_cbk, 0, GAME_MOUSE_KEY_SW_DEBOUNCE_TIME_MS};
#ifdef CONFIG_DOUBLE_DPI_KEY
static osal_timer g_mouse_dpi_add_irqs_timer = {0, app_mouse_dpi_add_timer_cbk, 0, GAME_MOUSE_KEY_SW_DEBOUNCE_TIME_MS};
static osal_timer g_mouse_dpi_sub_irqs_timer = {0, app_mouse_dpi_sub_timer_cbk, 0, GAME_MOUSE_KEY_SW_DEBOUNCE_TIME_MS};
#else
static osal_timer g_mouse_dpikey_irqs_timer  = {0, app_mouse_dpikey_timer_cbk, 0, GAME_MOUSE_KEY_SW_DEBOUNCE_TIME_MS};
#endif
static osal_timer g_mouse_fbkey_irqs_timer   = {0, app_mouse_fbkey_timer_cbk, 0, GAME_MOUSE_KEY_SW_DEBOUNCE_TIME_MS};
static osal_timer g_mouse_ffkey_irqs_timer   = {0, app_mouse_ffkey_timer_cbk, 0, GAME_MOUSE_KEY_SW_DEBOUNCE_TIME_MS};
#endif
#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
static osal_timer g_mouse_rrkey_irqs_timer   = {0, app_mouse_rrkey_timer_cbk, 0, GAME_MOUSE_KEY_SW_DEBOUNCE_TIME_MS};
#endif
static osal_timer g_mouse_combine_key_pair_timer = {
    0, app_mouse_combine_key_pair_timer_cbk, 0, GAME_MOUSE_BSLE_PAIR_TIME
};
static osal_timer g_mouse_combine_key_change_mod_timer = {
    0, app_mouse_combine_key_change_mod_cbk, 0, GAME_MOUSE_CHANGE_MODE_TIME
};

static void app_set_mouse_key_debounce_time(uint8_t debounce_time)
{
    g_key_debounce_time = debounce_time;
    // 预留按键去抖动时间配置保存到NV中，参考范围0-30ms,默认12ms
}

static uint8_t app_get_mouse_key_debounce_time(void)
{
    // 按键去抖动时间从NV或云驱中读取，参考范围0-30ms,默认12ms
    return g_key_debounce_time;
}

static void app_mouse_lkey_timer_cbk(unsigned long arg)
{
    UNUSED(arg);
#if MOUSE_DEBOUNCE_NEW_METHOD
    gpio_level_t io_level;
    debounce_state_t lkey_state = g_mouse_lkey_debounce.state;
    switch (lkey_state) {
        case KEY_HW_DEBOUNCE_STATE:
            // 暂存第1个6ms周期的IO状态
            g_debounce_temp_key.left_key = uapi_gpio_get_val(CONFIG_MOUSE_PIN_LEFT);
            g_mouse_lkey_debounce.state = KEY_SW_DEBOUNCE_STATE;
            // 重新启动6ms定时器再检测
            osal_timer_start(&g_mouse_lkey_irqs_timer);
            break;
        case KEY_SW_DEBOUNCE_STATE:
            // 读IO状态与前一个6ms的IO状态比较开始处理
            io_level = uapi_gpio_get_val(CONFIG_MOUSE_PIN_LEFT);
            if (g_debounce_temp_key.left_key == io_level) {
                if (io_level == GPIO_LEVEL_LOW) {
                    g_mouse_key->b.left_key = 1;
                    g_combined_left = LEFT;
                    app_mouse_detect_combined_keys();
                } else {
                    g_mouse_key->b.left_key = 0;
                    g_combined_left = 0;
                    osal_timer_stop(&g_mouse_combine_key_pair_timer);
                }
                g_debounce_key.left_key = io_level;
                mouse_log(MOUSE_LOG_DEBUG, "left key:%d\n", g_mouse_key->b.left_key);
            }
             // 停止定时器，打开中断使能进入下一个抖动循环周期检测
            osal_timer_stop(&g_mouse_lkey_irqs_timer);
            g_mouse_lkey_debounce.state = KEY_DEBOUNCE_IDLE;
            uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_LEFT);
            break;
        default:
            break;
    }
#endif
}

static void app_mouse_rkey_timer_cbk(unsigned long arg)
{
    UNUSED(arg);
#if MOUSE_DEBOUNCE_NEW_METHOD
    gpio_level_t io_level;
    debounce_state_t rkey_state = g_mouse_rkey_debounce.state;
    switch (rkey_state) {
        case KEY_HW_DEBOUNCE_STATE:
             // 暂存第1个6ms周期的IO状态
            g_debounce_temp_key.right_key = uapi_gpio_get_val(CONFIG_MOUSE_PIN_RIGHT);
            g_mouse_rkey_debounce.state = KEY_SW_DEBOUNCE_STATE;
            osal_timer_start(&g_mouse_rkey_irqs_timer);
            break;
        case KEY_SW_DEBOUNCE_STATE:
             // 读IO状态与前一个6ms的IO状态比较开始处理
            io_level = uapi_gpio_get_val(CONFIG_MOUSE_PIN_RIGHT);
            if (g_debounce_temp_key.right_key == io_level) {
                if (io_level == GPIO_LEVEL_LOW) {
                    g_mouse_key->b.right_key = 1;
                    g_combined_right = RIGHT;
                    app_mouse_detect_combined_keys();
                } else {
                    g_mouse_key->b.right_key = 0;
                    g_combined_right = 0;
                    osal_timer_stop(&g_mouse_combine_key_pair_timer);
                }
                g_debounce_key.right_key = io_level;
                mouse_log(MOUSE_LOG_DEBUG, "right key:%d\n", g_mouse_key->b.right_key);
            }
            // 停止定时器，打开中断使能进入下一个抖动循环周期检测
            osal_timer_stop(&g_mouse_rkey_irqs_timer);
            g_mouse_rkey_debounce.state = KEY_DEBOUNCE_IDLE;
            uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_RIGHT);
            break;
        default:
            break;
    }
#endif
}

static void app_mouse_mkey_timer_cbk(unsigned long arg)
{
    UNUSED(arg);
#if MOUSE_DEBOUNCE_NEW_METHOD
    gpio_level_t io_level;
    debounce_state_t mkey_state = g_mouse_mkey_debounce.state;
    switch (mkey_state) {
        case KEY_HW_DEBOUNCE_STATE:
            // 暂存第1个6ms周期的IO状态
            g_debounce_temp_key.mid_key = uapi_gpio_get_val(CONFIG_MOUSE_PIN_MID);
            g_mouse_mkey_debounce.state = KEY_SW_DEBOUNCE_STATE;
            osal_timer_start(&g_mouse_mkey_irqs_timer);
            break;
        case KEY_SW_DEBOUNCE_STATE:
             // 读IO状态与前一个6ms的IO状态比较开始处理
            io_level = uapi_gpio_get_val(CONFIG_MOUSE_PIN_MID);
            if (g_debounce_temp_key.mid_key == io_level) {
                if (io_level == GPIO_LEVEL_LOW) {
                    g_mouse_key->b.mid_key = 1;
                    g_combined_mid = MID;
                    app_mouse_detect_combined_keys();
                } else {
                    g_mouse_key->b.mid_key = 0;
                    g_combined_mid = 0;
                    osal_timer_stop(&g_mouse_combine_key_pair_timer);
                    osal_timer_stop(&g_mouse_combine_key_change_mod_timer);
                }
                g_debounce_key.mid_key = io_level;
                mouse_log(MOUSE_LOG_DEBUG, "mid key:%d\n", g_mouse_key->b.mid_key);
            }
             // 停止定时器，打开中断使能进入下一个抖动循环周期检测
            osal_timer_stop(&g_mouse_mkey_irqs_timer);
            g_mouse_mkey_debounce.state = KEY_DEBOUNCE_IDLE;
            uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_MID);
            break;
        default:
            break;
    }
#endif
}

#ifdef CONFIG_DOUBLE_DPI_KEY
static void app_mouse_dpi_add_timer_cbk(unsigned long arg)
{
    UNUSED(arg);
    gpio_level_t io_level;
    debounce_state_t dpi_key_state = g_mouse_dpi_add_state;
    switch (dpi_key_state) {
        case KEY_HW_DEBOUNCE_STATE:
            g_debounce_temp_key.dpi_add = uapi_gpio_get_val(CONFIG_MOUSE_PIN_DPI_ADD);
            g_mouse_dpi_add_state = KEY_SW_DEBOUNCE_STATE;
            osal_timer_start(&g_mouse_dpi_add_irqs_timer);
            break;
        case KEY_SW_DEBOUNCE_STATE:
            io_level = uapi_gpio_get_val(CONFIG_MOUSE_PIN_DPI_ADD);
            if (g_debounce_temp_key.dpi_add == io_level) {
                if (io_level == GPIO_LEVEL_HIGH) {
                    mouse_log(MOUSE_LOG_DEBUG, "dpi add key:%d\n", g_mouse_dpi_add_state);
                    app_mouse_send_msg(MSG_ID_BUTTON_MANAGE, MSG_TYPE_DPI_UP);
                }
            }
            osal_timer_stop(&g_mouse_dpi_add_irqs_timer);
            g_mouse_dpi_add_state = KEY_DEBOUNCE_IDLE;
            uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_DPI_ADD);
            break;
        default:
            break;
    }
}

static void app_mouse_dpi_sub_timer_cbk(unsigned long arg)
{
    UNUSED(arg);
    gpio_level_t io_level ;
    debounce_state_t dpi_key_state = g_mouse_dpi_sub_state;
    switch (dpi_key_state) {
        case KEY_HW_DEBOUNCE_STATE:
            g_debounce_temp_key.dpi_sub = uapi_gpio_get_val(CONFIG_MOUSE_PIN_DPI_SUB);
            g_mouse_dpi_sub_state = KEY_SW_DEBOUNCE_STATE;
            osal_timer_start(&g_mouse_dpi_sub_irqs_timer);
            break;
        case KEY_SW_DEBOUNCE_STATE:
            io_level = uapi_gpio_get_val(CONFIG_MOUSE_PIN_DPI_SUB);
            if (g_debounce_temp_key.dpi_sub == io_level) {
                if (io_level == GPIO_LEVEL_HIGH) {
                    mouse_log(MOUSE_LOG_DEBUG, "dpi sub key:%d\n", g_mouse_dpi_sub_state);
                    app_mouse_send_msg(MSG_ID_BUTTON_MANAGE, MSG_TYPE_DPI_DOWN);
                }
            }
            osal_timer_stop(&g_mouse_dpi_sub_irqs_timer);
            g_mouse_dpi_sub_state = KEY_DEBOUNCE_IDLE;
            uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_DPI_SUB);
            break;
        default:
            break;
    }
}
#else
static void app_mouse_dpikey_timer_cbk(unsigned long arg)
{
    UNUSED(arg);
    gpio_level_t io_level ;
    debounce_state_t dpi_key_state = g_mouse_dpi_state;
    switch (dpi_key_state) {
        case KEY_HW_DEBOUNCE_STATE:
            g_debounce_temp_key.dpi_key = uapi_gpio_get_val(CONFIG_MOUSE_PIN_DPI);
            g_mouse_dpi_state = KEY_SW_DEBOUNCE_STATE;
            osal_timer_start(&g_mouse_dpikey_irqs_timer);
            break;
        case KEY_SW_DEBOUNCE_STATE:
            io_level = uapi_gpio_get_val(CONFIG_MOUSE_PIN_DPI);
            if (g_debounce_temp_key.dpi_key == io_level) {
                if (io_level == GPIO_LEVEL_HIGH) {
                    mouse_log(MOUSE_LOG_DEBUG, "dpi add key:%d\n", g_mouse_dpi_state);
                    app_mouse_send_msg(MSG_ID_BUTTON_MANAGE, MSG_TYPE_DPI_UP);
                }
            }
            osal_timer_stop(&g_mouse_dpikey_irqs_timer);
            g_mouse_dpi_state = KEY_DEBOUNCE_IDLE;
            uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_DPI);
            break;
        default:
            break;
    }
}
#endif

static void app_mouse_fbkey_timer_cbk(unsigned long arg)
{
    UNUSED(arg);
#if MOUSE_DEBOUNCE_NEW_METHOD
    gpio_level_t io_level;
    debounce_state_t fbkey_state = g_mouse_fbkey_debounce.state;
    switch (fbkey_state) {
        case KEY_HW_DEBOUNCE_STATE:
            // 暂存第1个6ms周期的IO状态
            g_debounce_temp_key.backward_key = uapi_gpio_get_val(CONFIG_MOUSE_PIN_BACKWARD);
            g_mouse_fbkey_debounce.state = KEY_SW_DEBOUNCE_STATE;
            osal_timer_start(&g_mouse_fbkey_irqs_timer);
            break;
        case KEY_SW_DEBOUNCE_STATE:
             // 读IO状态与前一个6ms的IO状态比较开始处理
            io_level = uapi_gpio_get_val(CONFIG_MOUSE_PIN_BACKWARD);
            if (g_debounce_temp_key.backward_key == io_level) {
                if (io_level == GPIO_LEVEL_LOW) {
                    g_mouse_key->b.backward_key = 1;
                    g_combined_back = BACK;
                    app_mouse_detect_combined_keys();
                } else {
                    g_mouse_key->b.backward_key = 0;
                    g_combined_back = 0;
                    osal_timer_stop(&g_mouse_combine_key_change_mod_timer);
                }
                g_debounce_key.backward_key = io_level;
                mouse_log(MOUSE_LOG_DEBUG, "backward key:%d\n", g_mouse_key->b.backward_key);
            }
            // 停止定时器，打开中断使能进入下一个抖动循环周期检测
            osal_timer_stop(&g_mouse_fbkey_irqs_timer);
            g_mouse_fbkey_debounce.state = KEY_DEBOUNCE_IDLE;
            uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_BACKWARD);
            break;
        default:
            break;
    }
#endif
}

static void app_mouse_ffkey_timer_cbk(unsigned long arg)
{
    UNUSED(arg);
#if MOUSE_DEBOUNCE_NEW_METHOD
    gpio_level_t io_level;
    debounce_state_t ffkey_state = g_mouse_ffkey_debounce.state;
    switch (ffkey_state) {
        case KEY_HW_DEBOUNCE_STATE:
             // 暂存第1个6ms周期的IO状态
            g_debounce_temp_key.forward_key = uapi_gpio_get_val(CONFIG_MOUSE_PIN_FORWARD);
            g_mouse_ffkey_debounce.state = KEY_SW_DEBOUNCE_STATE;
            osal_timer_start(&g_mouse_ffkey_irqs_timer);
            break;
        case KEY_SW_DEBOUNCE_STATE:
             // 读IO状态与前一个6ms的IO状态比较开始处理
            io_level = uapi_gpio_get_val(CONFIG_MOUSE_PIN_FORWARD);
            if (g_debounce_temp_key.forward_key == io_level) {
                if (io_level == GPIO_LEVEL_LOW) {
                    g_mouse_key->b.forward_key = 1;
                    g_combined_forward = FORWARD;
                    app_mouse_detect_combined_keys();
                } else {
                    g_mouse_key->b.forward_key = 0;
                    g_combined_forward = 0;
                    osal_timer_stop(&g_mouse_combine_key_change_mod_timer);
                }
                g_debounce_key.forward_key = io_level;
                mouse_log(MOUSE_LOG_DEBUG, "forward key:%d\n", g_mouse_key->b.forward_key);
            }
             // 停止定时器，打开中断使能进入下一个抖动循环周期检测
            osal_timer_stop(&g_mouse_ffkey_irqs_timer);
            g_mouse_ffkey_debounce.state = KEY_DEBOUNCE_IDLE;
            uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_FORWARD);
            break;
        default:
            break;
    }
#endif
}

#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
static void app_mouse_rrkey_timer_cbk(unsigned long arg)
{
    UNUSED(arg);
#if MOUSE_DEBOUNCE_NEW_METHOD
    gpio_level_t io_level;
    debounce_state_t rrkey_state = g_mouse_rrkey_debounce.state;
    switch (rrkey_state) {
        case KEY_HW_DEBOUNCE_STATE:
             // 暂存第1个6ms周期的IO状态
            g_debounce_temp_key.report_rate = uapi_gpio_get_val(CONFIG_MOUSE_PIN_REPORT_RATE);
            g_mouse_rrkey_debounce.state = KEY_SW_DEBOUNCE_STATE;
            osal_timer_start(&g_mouse_rrkey_irqs_timer);
            break;
        case KEY_SW_DEBOUNCE_STATE:
             // 读IO状态与前一个6ms的IO状态比较开始处理
            io_level = uapi_gpio_get_val(CONFIG_MOUSE_PIN_REPORT_RATE);
            if (g_debounce_temp_key.report_rate == io_level) {
                if (io_level == GPIO_LEVEL_LOW) {
                    g_mouse_key->b.report_rate = 1;
                } else {
                    g_mouse_key->b.report_rate = 0;
                    app_mouse_send_msg(MSG_ID_BUTTON_MANAGE, MSG_TYPE_REPORT_RATE_CHANGE);
                }
                g_debounce_key.report_rate = io_level;
                osal_printk("report rate key:%d\n", g_mouse_key->b.report_rate);
            }
             // 停止定时器，打开中断使能进入下一个抖动循环周期检测
            osal_timer_stop(&g_mouse_rrkey_irqs_timer);
            g_mouse_rrkey_debounce.state = KEY_DEBOUNCE_IDLE;
            uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_REPORT_RATE);
            break;
        default:
            break;
    }
#endif
}
#endif

void app_mouse_combine_key_pair_timer_cbk(unsigned long arg)
{
    unused(arg);
    if (get_app_mouse_handle().current_mode == SLIDE_SW_MODE_USB) {
        return;
    }
    // 发送配对消息
    app_mouse_send_msg(MSG_ID_BUTTON_MANAGE, MSG_TYPE_PAIR_PROCESS);
}

// 将要切换的模式写道nv中并重启
void app_mouse_write_nv_and_reboot(uint16_t nv_key_id, uint8_t kvalue)
{
    // 把当前BSLE模式写入nv中
    uint16_t nv_len = 0;
    mouse_pair_info_t pair_info = {0};
    uapi_nv_read(nv_key_id, sizeof(mouse_pair_info_t), &nv_len, (uint8_t *)&pair_info);
    // 8k鼠标模式下channel_info的第一个列表的mode成员用来存储下次要切换的模式
    pair_info.channel_info[0].mode = kvalue;
    uapi_nv_write(nv_key_id, (uint8_t *)&(pair_info), sizeof(mouse_pair_info_t));
    // 重启
    cpu_utils_set_system_status_by_cause(REBOOT_CAUSE_UPG_COMPLETION);
    hal_reboot_chip();
}

void app_mouse_combine_key_change_mod_cbk(unsigned long arg)
{
    unused(arg);
    if (get_app_mouse_handle().current_mode == SLIDE_SW_MODE_USB) {
        return;
    }

    if (get_app_mouse_handle().current_mode == SLIDE_SW_MODE_SLE) {
        app_mouse_write_nv_and_reboot(0x11, SLIDE_SW_MODE_BLE);
    } else if (get_app_mouse_handle().current_mode == SLIDE_SW_MODE_BLE) {
        app_mouse_write_nv_and_reboot(0x11, SLIDE_SW_MODE_SLE);
    }
}

static void app_mouse_key_debounce_timer_init(void)
{
#if MOUSE_DEBOUNCE_NEW_METHOD
    if (osal_timer_init(&g_mouse_lkey_irqs_timer) != 0) {
        osal_printk("g_mouse_lkey_irqs_timer create failed!\n");
    }

    if (osal_timer_init(&g_mouse_rkey_irqs_timer) != 0) {
        osal_printk("g_mouse_rkey_irqs_timer create failed!\n");
    }

    if (osal_timer_init(&g_mouse_mkey_irqs_timer) != 0) {
        osal_printk("g_mouse_mkey_irqs_timer create failed!\n");
    }
#ifdef CONFIG_DOUBLE_DPI_KEY
    if (osal_timer_init(&g_mouse_dpi_add_irqs_timer) != 0) {
        osal_printk("g_mouse_dpi_add_irqs_timer create failed!\n");
    }

    if (osal_timer_init(&g_mouse_dpi_sub_irqs_timer) != 0) {
        osal_printk("g_mouse_dpi_sub_irqs_timer create failed!\n");
    }
#else
    if (osal_timer_init(&g_mouse_dpikey_irqs_timer) != 0) {
        osal_printk("g_mouse_dpikey_irqs_timer create failed!\n");
    }
#endif

    if (osal_timer_init(&g_mouse_fbkey_irqs_timer) != 0) {
        osal_printk("g_mouse_fbkey_irqs_timer create failed!\n");
    }

    if (osal_timer_init(&g_mouse_ffkey_irqs_timer) != 0) {
        osal_printk("g_mouse_ffkey_irqs_timer create failed!\n");
    }

#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
    if (osal_timer_init(&g_mouse_rrkey_irqs_timer) != 0) {
        osal_printk("g_mouse_rrkey_irqs_timer create failed!\n");
    }
#endif

    if (osal_timer_init(&g_mouse_combine_key_pair_timer) != 0) {
        osal_printk("g_mouse_combine_key_pair_timer create failed!\n");
    }

    if (osal_timer_init(&g_mouse_combine_key_change_mod_timer) != 0) {
        osal_printk("g_mouse_combine_key_change_mod_timer create failed!\n");
    }
#endif
}

void app_mouse_key_debounce_timer_stop(void)
{
    osal_timer_stop(&g_mouse_lkey_irqs_timer);
    osal_timer_stop(&g_mouse_rkey_irqs_timer);
}

static void app_mouse_lkey_debounce_process(void)
{
#if MOUSE_DEBOUNCE_NEW_METHOD
    int ret;
    uapi_gpio_disable_interrupt(CONFIG_MOUSE_PIN_LEFT); // 关中断起定时器,默认6ms周期1次，2次12ms
    ret =osal_timer_start(&g_mouse_lkey_irqs_timer);
    if (ret == 0) {
        g_mouse_lkey_debounce.state = KEY_HW_DEBOUNCE_STATE;
    } else {
        g_mouse_lkey_debounce.state = KEY_DEBOUNCE_IDLE;
        uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_LEFT);
    }
#else
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.left_key = !uapi_gpio_get_val(CONFIG_MOUSE_PIN_LEFT);
    uapi_gpio_clear_interrupt(CONFIG_MOUSE_PIN_LEFT);
#endif
}

static void app_mouse_rkey_debounce_process(void)
{
#if MOUSE_DEBOUNCE_NEW_METHOD
    int ret;
    uapi_gpio_disable_interrupt(CONFIG_MOUSE_PIN_RIGHT); // 关中断起定时器,默认6ms周期1次，2次12ms
    ret =osal_timer_start(&g_mouse_rkey_irqs_timer);
    if (ret == 0) {
        g_mouse_rkey_debounce.state = KEY_HW_DEBOUNCE_STATE;
    } else {
        g_mouse_rkey_debounce.state = KEY_DEBOUNCE_IDLE;
        uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_RIGHT);
    }
#else
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.right_key = !uapi_gpio_get_val(CONFIG_MOUSE_PIN_RIGHT);
    uapi_gpio_clear_interrupt(CONFIG_MOUSE_PIN_RIGHT);
#endif
}

static void app_mouse_mkey_debounce_process(void)
{
#if MOUSE_DEBOUNCE_NEW_METHOD
    int ret;
    uapi_gpio_disable_interrupt(CONFIG_MOUSE_PIN_MID); // 关中断起定时器,默认6ms周期1次，2次12ms
    ret =osal_timer_start(&g_mouse_mkey_irqs_timer);
    if (ret == 0) {
        g_mouse_mkey_debounce.state = KEY_HW_DEBOUNCE_STATE;
    } else {
        g_mouse_mkey_debounce.state = KEY_DEBOUNCE_IDLE;
        uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_MID);
    }
#else
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.mid_key = !uapi_gpio_get_val(CONFIG_MOUSE_PIN_MID);
    uapi_gpio_clear_interrupt(CONFIG_MOUSE_PIN_MID);
#endif
}

#ifdef CONFIG_DOUBLE_DPI_KEY
// 双键，DPI增加
static void app_mouse_dpi_add_debounce_process(void)
{
#if MOUSE_DEBOUNCE_NEW_METHOD // 因系统设置默认定时器数量超最大阈值了，暂时先关闭
    int ret;
    uapi_gpio_disable_interrupt(CONFIG_MOUSE_PIN_DPI_ADD); // 关中断起定时器,默认6ms周期1次，2次12ms
    ret = osal_timer_start(&g_mouse_dpi_add_irqs_timer);
    if (ret == 0) {
        g_mouse_dpi_add_state = KEY_HW_DEBOUNCE_STATE;
    } else {
        g_mouse_dpi_add_state = KEY_DEBOUNCE_IDLE;
        uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_DPI_ADD);
    }
#else
    uapi_tcxo_delay_us(DELAY_US200);
    g_debounce_temp_key.dpi_add = !uapi_gpio_get_val(CONFIG_MOUSE_PIN_DPI_ADD);
    uapi_gpio_clear_interrupt(CONFIG_MOUSE_PIN_DPI_ADD);
#endif
}

// 双键，DPI降低
static void app_mouse_dpi_sub_debounce_process(void)
{
#if MOUSE_DEBOUNCE_NEW_METHOD // 因系统设置默认定时器数量超最大阈值了，暂时先关闭
    int ret;
    uapi_gpio_disable_interrupt(CONFIG_MOUSE_PIN_DPI_SUB); // 关中断起定时器,默认6ms周期1次，2次12ms
    ret = osal_timer_start(&g_mouse_dpi_sub_irqs_timer);
    if (ret == 0) {
        g_mouse_dpi_sub_state = KEY_HW_DEBOUNCE_STATE;
    } else {
        g_mouse_dpi_sub_state = KEY_DEBOUNCE_IDLE;
        uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_DPI_SUB);
    }
#else
    uapi_tcxo_delay_us(DELAY_US200);
    g_debounce_temp_key.dpi_sub = !uapi_gpio_get_val(CONFIG_MOUSE_PIN_DPI_SUB);
    uapi_gpio_clear_interrupt(CONFIG_MOUSE_PIN_DPI_SUB);
#endif
}

#else
// 单键，DPI循环增加
static void app_mouse_dpi_debounce_process(void)
{
#if MOUSE_DEBOUNCE_NEW_METHOD // 因系统设置默认定时器数量超最大阈值了，暂时先关闭
    int ret;
    uapi_gpio_disable_interrupt(CONFIG_MOUSE_PIN_DPI); // 关中断起定时器,默认6ms周期1次，2次12ms
    ret = osal_timer_start(&g_mouse_dpikey_irqs_timer);
    if (ret == 0) {
        g_mouse_dpi_state = KEY_HW_DEBOUNCE_STATE;
    } else {
        g_mouse_dpi_state = KEY_DEBOUNCE_IDLE;
        uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_DPI);
    }
#else
    uapi_tcxo_delay_us(DELAY_US200);
    g_debounce_temp_key.dpi_key = !uapi_gpio_get_val(CONFIG_MOUSE_PIN_DPI);
    uapi_gpio_clear_interrupt(CONFIG_MOUSE_PIN_DPI);
#endif
}
#endif

static void app_mouse_fbkey_debounce_process(void)
{
#if MOUSE_DEBOUNCE_NEW_METHOD // 因系统设置默认定时器数量超最大阈值了，暂时先关闭
    int ret;
    uapi_gpio_disable_interrupt(CONFIG_MOUSE_PIN_BACKWARD); // 关中断起定时器,默认6ms周期1次，2次12ms
    ret =osal_timer_start(&g_mouse_fbkey_irqs_timer);
    if (ret == 0) {
        g_mouse_fbkey_debounce.state = KEY_HW_DEBOUNCE_STATE;
    } else {
        g_mouse_fbkey_debounce.state = KEY_DEBOUNCE_IDLE;
        uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_BACKWARD);
    }
#else
    uapi_tcxo_delay_us(DELAY_US200);
    g_mouse_key->b.backward_key = !uapi_gpio_get_val(CONFIG_MOUSE_PIN_BACKWARD);
    uapi_gpio_clear_interrupt(CONFIG_MOUSE_PIN_BACKWARD);
#endif
}

static void app_mouse_ffkey_debounce_process(void)
{
#if MOUSE_DEBOUNCE_NEW_METHOD // 因系统设置默认定时器数量超最大阈值了，暂时先关闭
    int ret;
    uapi_gpio_disable_interrupt(CONFIG_MOUSE_PIN_FORWARD); // 关中断起定时器,默认6ms周期1次，2次12ms
    ret =osal_timer_start(&g_mouse_ffkey_irqs_timer);
    if (ret == 0) {
        g_mouse_ffkey_debounce.state = KEY_HW_DEBOUNCE_STATE;
    } else {
        g_mouse_ffkey_debounce.state = KEY_DEBOUNCE_IDLE;
        uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_FORWARD);
    }
#else
    uapi_tcxo_delay_us(DELAY_US200);
    g_mouse_key->b.forward_key = !uapi_gpio_get_val(CONFIG_MOUSE_PIN_FORWARD);
    uapi_gpio_clear_interrupt(CONFIG_MOUSE_PIN_FORWARD);
#endif
}

#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
static void app_mouse_report_rate_debounce_process(void)
{
#if MOUSE_DEBOUNCE_NEW_METHOD // 因系统设置默认定时器数量超最大阈值了，暂时先关闭
    int ret;
    uapi_gpio_disable_interrupt(CONFIG_MOUSE_PIN_REPORT_RATE); // 关中断起定时器,默认6ms周期1次，2次12ms
    ret =osal_timer_start(&g_mouse_rrkey_irqs_timer);
    if (ret == 0) {
        g_mouse_rrkey_debounce.state = KEY_HW_DEBOUNCE_STATE;
    } else {
        g_mouse_rrkey_debounce.state = KEY_DEBOUNCE_IDLE;
        uapi_gpio_enable_interrupt(CONFIG_MOUSE_PIN_REPORT_RATE);
    }
#else
    uapi_tcxo_delay_us(DELAY_US200);
    g_mouse_key->b.report_rate = !uapi_gpio_get_val(CONFIG_MOUSE_PIN_REPORT_RATE);
    uapi_gpio_clear_interrupt(CONFIG_MOUSE_PIN_REPORT_RATE);
#endif
}
#endif

#if (MOUSE_DEBOUNCE_NEW_METHOD)
static void app_set_mouse_key_debounce_para(uint8_t hw_debounce_time, uint8_t sw_debounce_time)
{
    g_mouse_lkey_debounce.state           = KEY_DEBOUNCE_IDLE;
    g_mouse_lkey_debounce.hw_debouce_time = hw_debounce_time;
    g_mouse_lkey_debounce.sw_debouce_time = sw_debounce_time;

    g_mouse_rkey_debounce.state           = KEY_DEBOUNCE_IDLE;
    g_mouse_rkey_debounce.hw_debouce_time = hw_debounce_time;
    g_mouse_rkey_debounce.sw_debouce_time = sw_debounce_time;

    g_mouse_mkey_debounce.state           = KEY_DEBOUNCE_IDLE;
    g_mouse_mkey_debounce.hw_debouce_time = hw_debounce_time;
    g_mouse_mkey_debounce.sw_debouce_time = sw_debounce_time;

    g_mouse_fbkey_debounce.state           = KEY_DEBOUNCE_IDLE;
    g_mouse_fbkey_debounce.hw_debouce_time = hw_debounce_time;
    g_mouse_fbkey_debounce.sw_debouce_time = sw_debounce_time;

    g_mouse_ffkey_debounce.state           = KEY_DEBOUNCE_IDLE;
    g_mouse_ffkey_debounce.hw_debouce_time = hw_debounce_time;
    g_mouse_ffkey_debounce.sw_debouce_time = sw_debounce_time;

    g_mouse_rrkey_debounce.state           = KEY_DEBOUNCE_IDLE;
    g_mouse_rrkey_debounce.hw_debouce_time = hw_debounce_time;
    g_mouse_rrkey_debounce.sw_debouce_time = sw_debounce_time;
}
#endif

static void app_mouse_key_init(void)
{
#if (MOUSE_DEBOUNCE_NEW_METHOD)
    g_debounce_temp_key.left_key    = uapi_gpio_get_val(CONFIG_MOUSE_PIN_LEFT);
    g_debounce_temp_key.right_key   = uapi_gpio_get_val(CONFIG_MOUSE_PIN_RIGHT);
    g_debounce_temp_key.mid_key     = uapi_gpio_get_val(CONFIG_MOUSE_PIN_MID);
    g_debounce_temp_key.backward_key= uapi_gpio_get_val(CONFIG_MOUSE_PIN_BACKWARD);
    g_debounce_temp_key.forward_key = uapi_gpio_get_val(CONFIG_MOUSE_PIN_FORWARD);
#ifdef CONFIG_DOUBLE_DPI_KEY
    g_debounce_temp_key.dpi_add     = uapi_gpio_get_val(CONFIG_MOUSE_PIN_DPI_ADD);
    g_debounce_temp_key.dpi_sub     = uapi_gpio_get_val(CONFIG_MOUSE_PIN_DPI_SUB);
#else
    g_debounce_temp_key.dpi_key     = uapi_gpio_get_val(CONFIG_MOUSE_PIN_DPI);
#endif
#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
    g_debounce_temp_key.report_rate = uapi_gpio_get_val(CONFIG_MOUSE_PIN_REPORT_RATE);
#endif
    g_debounce_temp_key.reserved    = 0;

    g_debounce_key.left_key       = g_debounce_temp_key.left_key;
    g_debounce_key.right_key      = g_debounce_temp_key.right_key;
    g_debounce_key.mid_key        = g_debounce_temp_key.mid_key;
    g_debounce_key.backward_key   = g_debounce_temp_key.backward_key;
    g_debounce_key.forward_key    = g_debounce_temp_key.forward_key;
    g_debounce_key.reserved       = g_debounce_temp_key.reserved;
    g_debounce_key.report_rate    = g_debounce_temp_key.report_rate;

    app_mouse_key_debounce_timer_init();
    app_set_mouse_key_debounce_time(GMAE_MOUSE_KEY_HW_DEBOUNCE_TIMES + GAME_MOUSE_KEY_SW_DEBOUNCE_TIME_MS);
    app_set_mouse_key_debounce_para(GMAE_MOUSE_KEY_HW_DEBOUNCE_TIMES, GAME_MOUSE_KEY_SW_DEBOUNCE_TIME_MS);
#endif
}

void app_mouse_detect_combined_keys(void)
{
    int mouse_sle_pair_combine_key = g_combined_left | g_combined_right | g_combined_mid;
    int mouse_sle_ble_change_mod_combine_key = g_combined_forward | g_combined_back | g_combined_mid;
    if (mouse_sle_pair_combine_key == MOUSE_SLE_PAIR) {
        osal_timer_start(&g_mouse_combine_key_pair_timer);
    }

    if (mouse_sle_ble_change_mod_combine_key == MOUSE_SLE_BLE_CHANGE_MOD) {
        osal_timer_start(&g_mouse_combine_key_change_mod_timer);
    }
}

static void app_mouse_left_button_func(pin_t pin)
{
    unused(pin);
    if (g_mouse_key != NULL) {
        if (app_get_mouse_key_debounce_time() != 0) {
            app_mouse_lkey_debounce_process();
        }
    }
}

static void app_mouse_right_button_func(pin_t pin)
{
    unused(pin);
    if (g_mouse_key != NULL) {
        if (app_get_mouse_key_debounce_time() != 0) {
            app_mouse_rkey_debounce_process();
        }
    }
}

static void app_mouse_mid_button_func(pin_t pin)
{
    unused(pin);
    if (g_mouse_key != NULL) {
        if (app_get_mouse_key_debounce_time() != 0) {
            app_mouse_mkey_debounce_process();
        }
    }
}

#ifdef CONFIG_DOUBLE_DPI_KEY
static void app_mouse_dpi_add_button_func(pin_t pin)
{
    unused(pin);
    if (g_mouse_key != NULL) {
        if (app_get_mouse_key_debounce_time() != 0) {
            app_mouse_dpi_add_debounce_process();
        }
    }
}

static void app_mouse_dpi_sub_button_func(pin_t pin)
{
    unused(pin);
    if (g_mouse_key != NULL) {
        if (app_get_mouse_key_debounce_time() != 0) {
            app_mouse_dpi_sub_debounce_process();
        }
    }
}
#else
static void app_mouse_dpi_button_func(pin_t pin)
{
    unused(pin);
    if (g_mouse_key != NULL) {
        if (app_get_mouse_key_debounce_time() != 0) {
            app_mouse_dpi_debounce_process();
        }
    }
}
#endif

static void app_mouse_backward_button_func(pin_t pin)
{
    unused(pin);
    if (g_mouse_key != NULL) {
        if (app_get_mouse_key_debounce_time() != 0) {
            app_mouse_fbkey_debounce_process();
        }
    }
}

static void app_mouse_forward_button_func(pin_t pin)
{
    unused(pin);
    if (g_mouse_key != NULL) {
        if (app_get_mouse_key_debounce_time() != 0) {
            app_mouse_ffkey_debounce_process();
        }
    }
}

#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
static void mouse_report_rate_button_func(pin_t pin)
{
    unused(pin);
    if (g_mouse_key != NULL) {
        if (app_get_mouse_key_debounce_time() != 0) {
            app_mouse_report_rate_debounce_process();
        }
    }
}
#endif

static void mouse_uapi_pin_set_mode(void)
{
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_LEFT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_RIGHT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_MID, (pin_mode_t)HAL_PIO_FUNC_GPIO);
#ifdef CONFIG_DOUBLE_DPI_KEY
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_DPI_ADD, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_DPI_SUB, (pin_mode_t)HAL_PIO_FUNC_GPIO);
#else
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_DPI, (pin_mode_t)HAL_PIO_FUNC_GPIO);
#endif
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_BACKWARD, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_FORWARD, (pin_mode_t)HAL_PIO_FUNC_GPIO);
#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_REPORT_RATE, (pin_mode_t)HAL_PIO_FUNC_GPIO);
#endif
}

static void mouse_uapi_gpio_set_dir(void)
{
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_LEFT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_RIGHT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_MID, GPIO_DIRECTION_INPUT);
#ifdef CONFIG_DOUBLE_DPI_KEY
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_DPI_ADD, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_DPI_SUB, GPIO_DIRECTION_INPUT);
#else
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_DPI, GPIO_DIRECTION_INPUT);
#endif
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_BACKWARD, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_FORWARD, GPIO_DIRECTION_INPUT);
#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_REPORT_RATE, GPIO_DIRECTION_INPUT);
#endif
}

static void mouse_uapi_pin_set_pull(void)
{
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_LEFT, PIN_PULL_UP);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_RIGHT, PIN_PULL_UP);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_MID, PIN_PULL_UP);
#ifdef CONFIG_DOUBLE_DPI_KEY
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_DPI_ADD, PIN_PULL_UP);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_DPI_SUB, PIN_PULL_UP);
#else
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_DPI, PIN_PULL_UP);
#endif
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_BACKWARD, PIN_PULL_UP);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_FORWARD, PIN_PULL_UP);
#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_REPORT_RATE, PIN_PULL_UP);
#endif
}

void app_mouse_button_init(mouse_key_t *mouse_key)
{
    g_mouse_key = mouse_key;

    uapi_gpio_init();
    mouse_uapi_pin_set_mode();
    mouse_uapi_gpio_set_dir();
    mouse_uapi_pin_set_pull();

    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_LEFT, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_left_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_RIGHT, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_right_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_MID, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_mid_button_func);
#ifdef CONFIG_DOUBLE_DPI_KEY
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_DPI_ADD, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_dpi_add_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_DPI_SUB, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_dpi_sub_button_func);
#else
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_DPI, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_dpi_button_func);
#endif
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_BACKWARD, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_backward_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_FORWARD, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_forward_button_func);
#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_REPORT_RATE, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)mouse_report_rate_button_func);
#endif
    app_mouse_key_init();
}

void mouse_button_suspend(void)
{
    uapi_gpio_unregister_isr_func(CONFIG_MOUSE_PIN_LEFT);
    uapi_gpio_unregister_isr_func(CONFIG_MOUSE_PIN_RIGHT);
    uapi_gpio_unregister_isr_func(CONFIG_MOUSE_PIN_MID);
#ifdef CONFIG_DOUBLE_DPI_KEY
    uapi_gpio_unregister_isr_func(CONFIG_MOUSE_PIN_DPI_ADD);
    uapi_gpio_unregister_isr_func(CONFIG_MOUSE_PIN_DPI_SUB);
#else
    uapi_gpio_unregister_isr_func(CONFIG_MOUSE_PIN_DPI);
#endif
    uapi_gpio_unregister_isr_func(CONFIG_MOUSE_PIN_BACKWARD);
    uapi_gpio_unregister_isr_func(CONFIG_MOUSE_PIN_FORWARD);
#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
    uapi_gpio_unregister_isr_func(CONFIG_MOUSE_PIN_REPORT_RATE);
#endif
    return;
}

void mouse_button_resume(void)
{
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_LEFT, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_left_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_RIGHT, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_right_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_MID, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_mid_button_func);
#ifdef CONFIG_DOUBLE_DPI_KEY
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_DPI_ADD, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_dpi_add_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_DPI_SUB, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_dpi_sub_button_func);
#else
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_DPI, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_dpi_button_func);
#endif
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_BACKWARD, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_backward_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_FORWARD, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)app_mouse_forward_button_func);
#ifdef CONFIG_MOUSE_REPORT_RATE_KEY
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_REPORT_RATE, GPIO_INTERRUPT_DEDGE,
        (gpio_callback_t)mouse_report_rate_button_func);
#endif
}

bool app_mouse_no_key_press(void)
{
    if ((g_mouse_lkey_debounce.state == KEY_DEBOUNCE_IDLE) && \
        (g_mouse_mkey_debounce.state == KEY_DEBOUNCE_IDLE) && \
        (g_mouse_rkey_debounce.state == KEY_DEBOUNCE_IDLE) && \
        (g_mouse_ffkey_debounce.state == KEY_DEBOUNCE_IDLE) && \
        (g_mouse_fbkey_debounce.state == KEY_DEBOUNCE_IDLE)) {
        return true;
    } else {
        return false;
    }
}
