/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 *
 * Description: Mouse Button SAMPLE. \n
 *
 * History: \n
 * 2024-04-21, Create file. \n
 */
#ifndef MOUSE_BUTTON_H
#define MOUSE_BUTTON_H

#include <stdint.h>
#include "errcode.h"
#include "osal_debug.h"
#include "hal_gpio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


typedef enum {
    KEY_DEBOUNCE_IDLE,      // 按键去抖初始化状态
    KEY_HW_DEBOUNCE_STATE,  // 按键消除硬件抖状态
    KEY_SW_DEBOUNCE_STATE,  // 按键软件件去抖状态
} debounce_state_t;

typedef struct {
    debounce_state_t state;  // 按键去抖动状态
    uint8_t hw_debouce_time; // 按键硬件抖动时间范围：暂定1-5ms之间，以实测结论调定
    uint8_t sw_debouce_time; // 按键软件件抖时间范围：暂定5-15ms之间，以实测结论值定
} mouse_key_debounce_t;

typedef enum {
    BUTTON_IDLE,
    BUTTON_PRESS,
    BUTTON_RELEASE,
    BUTTON_CLICK,
    BUTTON_DOUBLE,
    BUTTON_LONG,
    BUTTON_LONG_LONG,
    BUTTON_EVENT_MAX
} button_event_t;

typedef struct {
    uint16_t event   : 4;
    uint16_t timecnt : 12;
} mouse_button_t;

typedef union mouse_key {
    struct {
        uint8_t left_key        : 1;
        uint8_t right_key       : 1;
        uint8_t mid_key         : 1;
        uint8_t backward_key    : 1;
        uint8_t forward_key     : 1;
        uint8_t report_rate     : 1;
        uint8_t reserved        : 2;
    } b;
    uint8_t d8;
} mouse_key_t;

typedef struct {
    gpio_level_t left_key       : 1;
    gpio_level_t right_key      : 1;
    gpio_level_t mid_key        : 1;
    gpio_level_t backward_key   : 1;
    gpio_level_t forward_key    : 1;
    gpio_level_t dpi_key        : 1;  // 单键dpi循环
    gpio_level_t dpi_add        : 1;  // 双键dpi_add
    gpio_level_t dpi_sub        : 1;  // 双键dpi_sub
    gpio_level_t report_rate    : 1;
    gpio_level_t reserved       : 2;
} mouse_gpio_t;

// 组合键键值
typedef enum {
    LEFT = 0x02,
    RIGHT = 0x04,
    MID = 0x08,
    FORWARD = 0x10,
    BACK = 0x20,
} mouse_combined_keys;

void app_mouse_button_init(mouse_key_t *mouse_key);
bool app_mouse_no_key_press(void);
void app_mouse_detect_combined_keys(void);
void app_mouse_combine_key_pair_timer_cbk(unsigned long arg);
void app_mouse_combine_key_change_mod_cbk(unsigned long arg);
void mouse_button_suspend(void);
void mouse_button_resume(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
