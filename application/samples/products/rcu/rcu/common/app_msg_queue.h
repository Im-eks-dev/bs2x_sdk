/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: MSG QUEUE. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-16, Create file. \n
 */

#ifndef APP_MSG_QUEUE_H
#define APP_MSG_QUEUE_H

#include "securec.h"
#include "cmsis_os2.h"
#include "common_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define APP_MAX_MSG_COUNT                   5
#define APP_MSG_BUFFER_LEN                  20
#define APP_MSG_WAIT_FOREVER                0xFFFFFFFF

typedef struct app_msg_data {
    uint16_t type;
    uint16_t length;
    uint8_t  buffer[APP_MSG_BUFFER_LEN];
} app_msg_data_t;

typedef enum {
    KEY_DOWN_EVENT_MSG = 1,
    KEY_UP_EVENT_MSG,
    KEY_HOLD_LONG_EVENT_MSG,
    BATTERY_EVENT_MSG,
    KEY_EVENT,
    SLEPP_CHECK_MSG,
    IR_STUDY_SUCCESS_MSG,
    KEY_PAIR_MSG,
    RCU_MSG_SLE_ADV_ENABLE,
    RCU_MSG_SLE_ADV_DISABLE,
    RCU_MSG_SLE_CONNECTED,
    RCU_MSG_SLE_DISCONNECTED,
    RCU_MSG_SLE_CONN_PARAM_UPDATAED
} APP_MSG_DATA_TYPE;

void app_create_msgqueue(void);
void app_delete_msgqueue(void);
void app_write_msgqueue(app_msg_data_t msg);
uint32_t app_receive_msgqueue(uint8_t* buf, uint32_t* len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif