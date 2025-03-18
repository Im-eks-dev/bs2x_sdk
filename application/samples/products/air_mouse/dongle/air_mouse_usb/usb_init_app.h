/*
 * Copyright (c) @CompanyNameMagicTag 2022-2024. All rights reserved.
 * Description: usb 接口定义
 */
#ifndef USB_INIT_APP_H
#define USB_INIT_APP_H

#include "implementation/usb_init.h"
#include "gadget/f_hid.h"
#include "slp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define USB_HID_MAX_KEY_LENTH 6

typedef enum {
    HID_MOUSE_REL_KIND = 0x01, // 光标相对位置
    HID_MOUSE_ABS_KIND,        // 光标绝对位置
    HID_KEYBOARD_KIND,         // 键盘输入
    HID_NEARLINK_KIND,         // 星闪usb协议
} hid_kind_mode;

#pragma pack(1)
typedef union mouse_key {
    struct {
        uint8_t left_key   : 1;
        uint8_t right_key  : 1;
        uint8_t mid_key    : 1;
        uint8_t reserved   : 5;
    } b;
    uint8_t d8;
} mouse_key_t;

typedef struct {
    uint8_t kind;
    mouse_key_t mouse_key;
    int16_t x;
    int16_t y;
} usb_hid_mouse_report_t;

typedef struct {
    uint8_t kind;
    uint8_t special_key;                /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t reserve;
    uint8_t key[USB_HID_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_keyboard_report_t;

typedef struct {
    uint32_t sequence_no;  // 发送的消息号
    uint8_t key;           // 按键序号
    usb_hid_mouse_report_t report;
} mouse_report_t;  // 用于空口传坐标、鼠标左键

typedef struct {
    uint32_t sequence_no;  // 发送的消息号
    uint8_t key;           // 按键序号
    usb_hid_keyboard_report_t report;
} keyboard_report_t;  // 用于空口传输键盘

typedef struct {
    uint8_t type;
    uint16_t len;
    uint8_t value;
} sle_tlv0_t;  // 第一个tlv，默认0x1 0x1 0x0 0x0就可以

typedef struct {
    uint8_t type;
    uint16_t len;
    uint8_t value[46];
} sle_tlv1_t;

typedef struct {
    uint8_t flag;
    uint8_t version;
    uint8_t total_frame;
    uint8_t frame_seq;
    uint8_t rsv;
    uint8_t service_id;
    uint8_t command_id;
    uint16_t body_len;
    sle_tlv0_t tlv0;
    sle_tlv1_t tlv1;
    uint8_t mic[2];
} sle_usb_frame_header_t;

#pragma pack()

int32_t usb_init_app(device_type dtype);
int32_t usb_deinit_app(void);
void dongle_hid_usb_init(void);
int32_t get_usb_mouse_hid_index(void);
void update_mouse_coordinate(usb_hid_mouse_report_t *mouse_report);
void update_mouse_key(usb_hid_mouse_report_t *mouse_report);
void set_screen_resolution(SlpScreenResolution resolution);
void set_mouse_move_mode(hid_kind_mode mode);
void move_cursor_to_center_rel(void);
void set_usb_mouse_hid_index(int32_t index);
void sle_ota_mouse_dongle_send_data_test(usb_hid_mouse_report_t *rpt);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
