/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 *
 * Description: Mouse Data Transfer SAMPLE. \n
 *
 * History: \n
 * 2024-04-21, Create file. \n
 */
#ifndef MOUSE_DATA_TRANSFER_MANAGE_H
#define MOUSE_DATA_TRANSFER_MANAGE_H
#include "sle_mouse_server/sle_mouse_hid_server.h"
#define REPORT_VALUE_HEAD 0x05
#define REPORT_VALUE_TYPE 0x5B

// bios下使用默认描述符，发送的数据长度
#define REPORT_LEN_BIOS 3
// usb mouse device id
#define USB_MOUSE_DEVICE_ID 0

#pragma pack (1)
typedef struct {
    uint8_t kind;
    mouse_key_t key;
    int8_t wheel;
    int16_t x;
    int16_t y;
} app_mouse_report_data_t;

typedef struct {
    mouse_key_t key;
    int8_t x;
    int8_t y;
} app_mouse_report_data_protocol_0_t;

typedef struct {
    uint16_t val;
    uint8_t reserved[2];
} notify_data_reportRate_t;

typedef struct {
    uint8_t val;
    uint8_t reserved[3];
} notify_data_state_t;

typedef struct {
    uint8_t head;
    uint8_t type;
    uint16_t length;
    uint8_t cmd;
    union {
        uint32_t dpi;
        notify_data_reportRate_t reportRate;
        notify_data_state_t state;
    } data;
} app_mouse_notify_data_t;
#pragma pack ()

typedef struct {
    uint8_t head;
    uint8_t type;
    uint16_t length;
    uint8_t cmd;
    sle_mouse_data_t data;
} app_mouse_data_report_t;

void app_mouse_customer_init(uint8_t mouse_mode);
void app_mouse_open_data_transfer(uint8_t mode_id);
void app_mouse_ble_register_low_lantency_cbk(void);
void app_mouse_sle_register_low_lantency_cbk(void);
void app_mouse_sle_data_tx_enable(void);
void app_mouse_disable_transfer(void);
void app_mouse_enable_transfer(void);
bool app_mouse_data_report_enable(void);
void app_mouse_usb_unregister_data_cbk(void);
void app_mouse_user_data_process(uint8_t *msg);
void app_mouse_dpi_change(uint32_t dpi);
void app_mouse_report_rate_change(uint16_t report_rate);
void app_mouse_lift_mode_check(void);
void app_mouse_sle_low_latency_cbk_reg(void);
void app_mouse_wheel_init(void);
void app_mouse_change_polling_rate(uint8_t reportRate);
void app_mouse_set_sensor_mode(uint8_t reportRate);
void app_mouse_sle_report_data(void);
#endif
