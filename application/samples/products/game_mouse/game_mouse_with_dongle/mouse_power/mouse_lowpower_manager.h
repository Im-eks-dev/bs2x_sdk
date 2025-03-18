/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: Mouse Low-power Manager SAMPLE \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-10-27, Create file. \n
 */

#ifndef MOUSE_LOWPOWER_MANAGER
#define MOUSE_LOWPOWER_MANAGER

typedef enum {
    MOUSE_IN_WORK,              /* 工作状态 */
    MOUSE_IN_STANDBY,           /* 待机状态 */
    MOUSE_IN_SLEEP,             /* 睡眠状态 */
    MOUSE_WORK_TO_STANDBY,      /* 中间态：工作->待机 */
    MOUSE_STANDBY_TO_SLEEP,     /* 中间态：待机->睡眠 */
    MOUSE_STANDBY_TO_WORK,      /* 中间态：待机->工作 */
    MOUSE_SLEEP_TO_WORK,        /* 中间态：睡眠->工作 */
} app_mouse_lowpower_state_t;

void app_mouse_lowpower_init(void);
void app_mouse_sleep_manager(void);
void app_mouse_standby_timer_start(void);
void app_mouse_sleep_timer_start(void);
void app_mouse_standby_timer_stop(void);
void app_mouse_sleep_timer_stop(void);
void mouse_lowpower_set_state(app_mouse_lowpower_state_t state);
app_mouse_lowpower_state_t mouse_lowpower_get_state(void);
void app_mouse_perp_config_before_sleep(void);
void app_mouse_perp_config_after_wkup(void);
void app_mouse_idle_time_reset(void);
void app_mouse_send_standby_msg(void);
#endif
