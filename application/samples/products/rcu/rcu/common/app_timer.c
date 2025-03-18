/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: APP TIMER. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-16, Create file. \n
 */

#include "common_def.h"
#include "errcode.h"
#include "soc_osal.h"
#include "osal_timer.h"
#include "app_msg_queue.h"
#include "app_status.h"
#include "app_timer.h"

osal_timer app_timer[TIME_CMD_MAX];

void key_hold_long_start_fun(unsigned int time)
{
    osal_timer_stop(&app_timer[TIME_CMD_KEY_HOLD_LONG]);
    osal_timer_mod(&app_timer[TIME_CMD_KEY_HOLD_LONG], time);
}

void key_hold_long_stop_fun(void)
{
    osal_timer_stop(&app_timer[TIME_CMD_KEY_HOLD_LONG]);
}

void key_hold_long_timer_callback(unsigned long para)
{
    unused(para);
    app_msg_data_t msg;
    msg.type = KEY_HOLD_LONG_EVENT_MSG;
    msg.length = 0;
    app_write_msgqueue(msg);
    osal_timer_stop(&app_timer[TIME_CMD_KEY_HOLD_LONG]);
}

void pair_start_fun(unsigned int time)
{
    unused(time);
}

void pair_stop_fun(void)
{
}

void pair_timer_callback(unsigned long para)
{
    unused(para);
    pair_stop_fun();
}

void unpair_start_fun(unsigned int time)
{
    unused(time);
}

void unpair_stop_fun(void)
{
}

void unpair_timer_callback(unsigned long para)
{
    unused(para);
    unpair_stop_fun();
}

void sleep_check_start_fun(unsigned int time)
{
    unused(time);
    osal_timer_mod(&app_timer[TIME_CMD_SLEEP_CHECK], APP_SLEEP_CHECK_START_TIME);
}

void sleep_check_stop_fun(void)
{
    osal_timer_stop(&app_timer[TIME_CMD_SLEEP_CHECK]);
}

void sleep_check_timer_callback(unsigned long para)
{
    unused(para);
    app_msg_data_t msg;
    msg.type = SLEPP_CHECK_MSG;
    msg.length = 0;
    app_write_msgqueue(msg);
}

void ir_study_start_fun(unsigned int time)
{
    osal_timer_mod(&app_timer[TIME_CMD_IR_STUDY], time);
}
 
void ir_study_stop_fun(void)
{
    osal_timer_stop(&app_timer[TIME_CMD_IR_STUDY]);
}
 
void ir_study_timer_callback(unsigned long para)
{
    unused(para);
    ir_study_stop_fun();
    clear_rcu_status(RCU_STATUS_IR_STUDY);
    osal_printk("exit RCU_MODE_IR_STUDY\r\n");
}

app_timer_t app_timer_table[] = {
    {TIME_CMD_KEY_HOLD_LONG, key_hold_long_start_fun, key_hold_long_stop_fun,
     key_hold_long_timer_callback, APP_HOLD_LONG_TIME},
    {TIME_CMD_PAIR,          pair_start_fun,          pair_stop_fun,
     pair_timer_callback,          APP_PAIR_TIME},
    {TIME_CMD_SLEEP_CHECK,   sleep_check_start_fun,   sleep_check_stop_fun,
     sleep_check_timer_callback,   APP_SLEEP_CHECK_TIME},
    {TIME_CMD_UNPAIR,        unpair_start_fun,        unpair_stop_fun,
     unpair_timer_callback,        APP_UNPAIR_TIME},
    {TIME_CMD_IR_STUDY,      ir_study_start_fun,       ir_study_stop_fun,
     ir_study_timer_callback,      APP_IR_STUDY_TIME}
};

void app_timer_process_start(APP_TIMER_CMD cmd, unsigned int time)
{
    app_timer_table[cmd].start_fun(time);
}

void app_timer_process_stop(APP_TIMER_CMD cmd)
{
    app_timer_table[cmd].stop_fun();
}

/* 注册定时器 */
void app_timer_init(void)
{
    for (int i = 0; i < TIME_CMD_MAX; i++) {
        app_timer[i].timer = NULL;
        app_timer[i].data = 0;
        app_timer[i].handler = app_timer_table[i].handler;
        app_timer[i].interval = app_timer_table[i].interval;
        osal_timer_init(&app_timer[i]);
    }
    osal_printk("app_timer_init!\r\n");
}