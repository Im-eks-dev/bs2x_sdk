/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: APP STATUS HEADER FILE. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-22, Create file. \n
 */
#ifndef APP_STATUS_H
#define APP_STATUS_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    APP_SYS_WORK,
    APP_SYS_STANDBY,
    APP_SYS_SLEEP,
} APP_SYS_STATUS;

typedef enum {
    APP_CONNECT_STATUS_IDLE = 0,
    APP_CONNECT_STATUS_ADVING,
    APP_CONNECT_STATUS_CONNECTED,
    APP_CONNECT_STATUS_DISCONNECT,
    APP_CONNECT_STATUS_PAIRED,
} APP_CONNECT_STATUS;

typedef enum {
    APP_ADV_TYPE_IDLE = 0,
    APP_ADV_TYPE_DIRECT,
    APP_ADV_TYPE_UNDIRECT,
} APP_ADV_TYPE;

typedef enum {
    APP_DISCONNECT_TYPE_IDLE = 0,
    APP_DISCONNECT_TYPE_DIRECT,
    APP_DISCONNECT_TYPE_UNDIRECT,
    APP_DISCONNECT_TYPE_UNPAIR,
} APP_DISCONNECT_TYPE;

typedef enum {
    APP_STOP_ADV_TYPE_IDLE = 0,
    APP_STOP_ADV_TYPE_DIRECT,
    APP_STOP_ADV_TYPE_UNDIRECT,
    APP_STOP_ADV_TYPE_TIMEOUT,
    APP_STOP_ADV_TYPE_UNPAIR,
} APP_STOP_ADV_TYPE;

enum {
    RCU_STATUS_IDLE = 0,
    RCU_STATUS_VOICE_WORK = 0x01,
    RCU_STATUS_KEY_OPERATION = 0x02,
    RCU_STATUS_IR_SEND = 0x04,
    RCU_STATUS_IR_STUDY = 0x08,
    RCU_STATUS_ADV_SEND = 0x10,
    RCU_STATUS_LED_OPEN = 0x20,
    RCU_STATUS_OTA_UPGRADE = 0x40,

    RCU_STATUS_TEST_NO_SLEPP = 0x80,
};

typedef struct app_globle_status {
    APP_SYS_STATUS app_sys_status;
    APP_CONNECT_STATUS app_connect_status;
    APP_ADV_TYPE app_adv_type;
    APP_STOP_ADV_TYPE app_adv_stop_reason;
    APP_DISCONNECT_TYPE app_disconnect_reason;
    uint8_t rcu_status;
} app_globle_status_t;

app_globle_status_t get_app_globle_status(void);
void set_app_sys_status(APP_SYS_STATUS status);
void set_app_connect_status(APP_CONNECT_STATUS status);
void set_app_adv_type(APP_ADV_TYPE type);
void set_app_adv_stop_reason(APP_STOP_ADV_TYPE app_adv_stop_reason);
void set_app_disconnect_reason(APP_DISCONNECT_TYPE app_disconnect_reason);
void set_rcu_status(uint8_t status);
void clear_rcu_status(uint8_t status);
uint8_t get_rcu_status(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif