/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: APP Keyscan Header File. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-20, Create file. \n
 */

#ifndef APP_KEYSCAN_H
#define APP_KEYSCAN_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define KEY_MAX_NUM                        3
#define USB_HID_MAX_KEY_LENTH              6

#define PRESS_NONE_KEY                     0
#define PRESS_ONE_KEY                      1
#define PRESS_TWO_KEY                      2
#define PRESS_THREE_KEY                    3

#define RCU_KEY_STANDBY                    0x66
#define RCU_KEY_HOME                       0x5
#define RCU_KEY_BACK                       0x6
#define RCU_KEY_SEARCH                     0x7
#define RCU_KEY_VOLUP                      0x8
#define RCU_KEY_VOLDOWN                    0x9
#define RCU_KEY_SWITCH_MOUSE_AND_KEY       0xA
#define RCU_KEY_POWER                      0xB
#define RCU_KEY_SWITCH_SLE_AND_BLE         0xC
#define RCU_KEY_SWITCH_CONN_ID             0xD
#define RCU_KEY_MIC                        0xE
#define RCU_KEY_CONNECT_ADV                0xF
#define RCU_KEY_DISCONNECT_DEVICE          0x10
#define RCU_KEY_SWITCH_IR                  0x11
#define RCU_KEY_WAKEUP_ADV                 0x12

#define RCU_KEY_APPLIC                     0x65
#define RCU_KEY_ENTER                      0x28
#define RCU_KEY_BACKOUT                    0x29
#define RCU_KEY_PAGEUP                     0x4B
#define RCU_KEY_PAGEDN                     0x4E
#define RCU_KEY_RIGHT                      0x4F
#define RCU_KEY_LEFT                       0x50
#define RCU_KEY_DOWN                       0x51
#define RCU_KEY_UP                         0x52

#define IR_NEC_USER_CODE                   0x00
#define IR_NEC_KEY_UP                      0xCA
#define IR_NEC_KEY_DOWN                    0xD2
#define IR_NEC_KEY_RIGHT                   0xC1
#define IR_NEC_KEY_LEFT                    0x99
#define IR_NEC_KEY_SELECT                  0xCE
#define IR_NEC_KEY_BACK                    0x90
#define IR_NEC_KEY_MENU                    0x9D
#define IR_NEC_KEY_POWER                   0x9C
#define IR_NEC_KEY_HOME                    0xCB
#define IR_NEC_KEY_VOLUMEUP                0x80
#define IR_NEC_KEY_VOLUMEDOWN              0x81
#define IR_NEC_KEY_MUTE                    0xDD
#define IR_NEC_KEY_STANDBY                 0xDD
#define IR_NEC_KEY_SEARCH                  0xDE

#define PDM_DMA_TRANSFER_EVENT             1
#define RCU_TARGET_ADDR_NUM                2
#define RCU_CONSUMER_KEY_NUM               6
#define RCU_CONSUMER_KEY_OFFSET            8

#define DURATION_MS_OF_WORK_TO_STANDBY     2000
#define DURATION_MS_OF_STANDBY_TO_SLEEP    30000
#define DURATION_MS_OF_SLEEP_TO_UDS        30000

typedef union mouse_key {
    struct {
        uint8_t left_key   : 1;
        uint8_t right_key  : 1;
        uint8_t mid_key    : 1;
        uint8_t reserved   : 5;
    } b;
    uint8_t d8;
} mouse_key_t;

typedef enum {
    COMBINE_KEY_FLAG_NONE           = 0,
    COMBINE_KEY_FLAG_TEST_STATION_01,
    COMBINE_KEY_FLAG_TEST_STATION_02,
    COMBINE_KEY_FLAG_TEST_STATION_03,
    COMBINE_KEY_FLAG_TEST_STATION_04,
    COMBINE_KEY_FLAG_TEST_STATION_05,
    COMBINE_KEY_FLAG_TEST_STATION_06,
    COMBINE_KEY_FLAG_PAIR,
    COMBINE_KEY_FLAG_UNPAIR,
    COMBINE_KEY_FLAG_IR_LEARN,
    COMBINE_KEY_FLAG_IR_CHANGE,
    COMBINE_KEY_FLAG_IR_MODE_LOCAL,
    COMBINE_KEY_FLAG_IR_MODE_UPLOAD,
    COMBINE_KEY_FLAG_IR_LEARN_ERASE,
    COMBINE_KEY_FLAG_SINGLE_TONE_MODE
} combine_key_e;

typedef struct {
    uint8_t num;
    uint8_t key_value[KEY_MAX_NUM];
} key_t;

void keyevent_process(uint8_t *key_buf, uint8_t keylen, APP_MSG_DATA_TYPE event);
void app_keyscan_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif