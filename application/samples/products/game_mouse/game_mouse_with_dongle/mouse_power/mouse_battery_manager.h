/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 *
 * Description: Mouse Battery Manager SAMPLE. \n
 *
 * History: \n
 * 2024-04-21, Create file. \n
 */
#ifndef MOUSE_BATTERY_MANAGER
#define MOUSE_BATTERY_MANAGER

#define BATTERY_LEVEL_REPORT_TIME 10000

typedef enum {
    MOUSE_BATTERY_IDLE,
    MOUSE_BATTERY_CHARGE_IN,   // 鼠标电池充电状态
    MOUSE_BATTERY_CHARGE_FULL, // 鼠标电池充电满状态
    MOUSE_BATTERY_CHARGE_OUT,  // 鼠标放电状态
    MOUSE_BATTERY_LEVEL_CHANGE, // 鼠标电池电量变更状态
    MOUSE_BATTERY_CHARGE_ERROR, // 鼠标电池错误状态：电池保护，短路等异常
} app_mouse_battery_state_t;

typedef enum {
    MOUSE_BATTERY_LOW,  // 鼠标电池电量低：ADC电压小于3.3V或更低，可自定义
    MOUSE_BATTERY_MID,  // 鼠标电池电量中：ADC电压大于3.45V~3.90V,可自定义
    MOUSE_BATTERY_HIGH, // 鼠标电池电量高：ADC电压大于3.95V~4.20V,可自定义
    MOUSE_BATTERY_ERROR, // 鼠标放电状态
} app_mouse_battery_level_t;

typedef struct {
    uint8_t battery_level;
    app_mouse_battery_state_t battery_state;
} app_mouse_battery_mgr_t;

void app_mouse_battery_init(void);
void app_mouse_battery_state_change(app_mouse_battery_state_t new_state);
void app_mouse_battery_manager(void);
void app_mouse_vbat_adc_suspend(void);
void app_mouse_vbat_adc_resume(void);
#endif
