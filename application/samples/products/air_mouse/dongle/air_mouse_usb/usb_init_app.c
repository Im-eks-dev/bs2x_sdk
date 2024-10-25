/*
 * Copyright (c) @CompanyNameMagicTag 2022-2024. All rights reserved.
 * Description: usb init
 */
#include "usb_init_app.h"
#include <stdbool.h>
#include "securec.h"
#include "gadget/f_hid.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "slp.h"
#include "air_mouse_common.h"

#define USB_INIT_APP_MANUFACTURER  { 'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 }
#define USB_INIT_APP_MANUFACTURER_LEN   20
#define USB_INIT_APP_PRODUCT  { 'S', 0, 'L', 0, 'P', 0, ' ', 0, 'T', 0, 'V', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0 }
#define USB_INIT_APP_PRODUCT_LEN        22
#define USB_INIT_APP_SERIAL   { '2', 0, '0', 0, '2', 0, '0', 0, '0', 0, '6', 0, '2', 0, '4', 0 }
#define USB_INIT_APP_SERIAL_LEN         16
#define input(size)                 (0x80 | (size))
#define output(size)                (0x90 | (size))
#define feature(size)               (0xb0 | (size))
#define collection(size)            (0xa0 | (size))
#define end_collection(size)        (0xc0 | (size))
#define usage_page(size)            (0x04 | (size))
#define logical_minimum(size)       (0x14 | (size))
#define logical_maximum(size)       (0x24 | (size))
#define physical_minimum(size)      (0x34 | (size))
#define physical_maximum(size)      (0x44 | (size))
#define uint_exponent(size)         (0x54 | (size))
#define uint(size)                  (0x64 | (size))
#define report_size(size)           (0x74 | (size))
#define report_id(size)             (0x84 | (size))
#define report_count(size)          (0x94 | (size))
#define push(size)                  (0xa4 | (size))
#define pop(size)                   (0xb4 | (size))
#define usage(size)                 (0x08 | (size))
#define usage_minimum(size)         (0x18 | (size))
#define usage_maximum(size)         (0x28 | (size))
#define designator_index(size)      (0x38 | (size))
#define designator_minimum(size)    (0x48 | (size))
#define designator_maximum(size)    (0x58 | (size))
#define string_index(size)          (0x78 | (size))
#define string_minimum(size)        (0x88 | (size))
#define string_maximum(size)        (0x98 | (size))
#define delimiter(size)             (0xa8 | (size))
#define SCREEN_RESOLUTION_NUM       4  // 可选分辨率总数
#define SCREEN_RESOLUTION_PARAM_NUM 2  // 分辨率参数数量

#define MP_TEST_INFO_ID 0x1f
#define MP_TEST_SEND_AIR_MOUSE_SERVICE 0x01
#define MP_TEST_SEND_AIR_MOUSE_CMD 0x0c

static bool g_usb_inited = false;
static const uint8_t g_report_desc_hid[ ] = {
    usage_page(1),      0x01,
    usage(1),           0x02,
    /* 相对坐标 */
    collection(1),      0x01,
    report_id(1),       HID_MOUSE_REL_KIND,
    usage(1),           0x01,
    collection(1),      0x00,
    report_count(1),    0x03,
    report_size(1),     0x01,
    usage_page(1),      0x09,
    usage_minimum(1),   0x1,
    usage_maximum(1),   0x3,
    logical_minimum(1), 0x00,
    logical_maximum(1), 0x01,
    input(1),           0x02,
    report_count(1),    0x01,
    report_size(1),     0x05,
    input(1),           0x01,
    report_count(1),    0x02,
    report_size(1),     0x10,
    usage_page(1),      0x01,
    usage(1),           0x30,
    usage(1),           0x31,
    logical_minimum(2), 0x00, 0x80, // -32768
    logical_maximum(2), 0xFF, 0x7F, //  32767
    input(1),           0x06,
    end_collection(0),
    end_collection(0),
    /* 绝对坐标 */
    usage_page(1),       0x01,
    usage(1),            0x02,
    collection(1),       0x01,
    report_id(1),        HID_MOUSE_ABS_KIND,
    usage(1),            0x01,
    collection(1),       0x00,
    report_count(1),     0x03,
    report_size(1),      0x01,
    usage_page(1),       0x09,
    usage_minimum(1),    0x1,
    usage_maximum(1),    0x3,
    logical_minimum(1),  0x00,
    logical_maximum(1),  0x01,
    input(1),            0x02,
    report_count(1),     0x01,
    report_size(1),      0x05,
    input(1),            0x01,
    report_count(1),     0x02,
    report_size(1),      0x10,
    usage_page(1),       0x01,
    usage(1),            0x30,
    usage(1),            0x31,
    logical_minimum(2),  0x00, 0x00, // 0
    logical_maximum(2),  0x10, 0x27, // 10000
    physical_minimum(2), 0x00, 0x00, // 0
    physical_maximum(2), 0x10, 0x27, // 10000
    input(1),            0x62,
    end_collection(0),
    end_collection(0), // END_COLLECTION
    /* 键盘输入 */
    usage_page(1),      0x01,
    usage(1),           0x06,
    collection(1),      0x01,
    report_id(1),       HID_KEYBOARD_KIND,
    usage_page(1),      0x07,
    usage_minimum(1),   0xE0,
    usage_maximum(1),   0xE7,
    logical_minimum(1), 0x00,
    logical_maximum(1), 0x01,
    report_size(1),     0x01,
    report_count(1),    0x08,
    input(1),           0x02,
    report_count(1),    0x01,
    report_size(1),     0x08,
    input(1),           0x01,
    report_count(1),    0x05,
    report_size(1),     0x01,
    usage_page(1),      0x08,
    usage_minimum(1),   0x01,
    usage_maximum(1),   0x05,
    output(1),          0x02,
    report_count(1),    0x01,
    report_size(1),     0x03,
    output(1),          0x01,
    report_count(1),    0x06,
    report_size(1),     0x08,
    logical_minimum(1), 0x00,
    logical_maximum(1), 0x65,
    usage_page(1),      0x07,
    usage_minimum(1),   0x00,
    usage_maximum(1),   0x65,
    input(1),           0x00,
    end_collection(0),
    /* Near Link usb */
    usage_page(2), 0xB2, 0xFF,
    usage(1),           0x1,
    collection(1),      0x01,
    report_id(1),       0x1f,
    collection(1),      0x00,
    report_count(1),    0x3f,
    report_size(1),     0x8,
    usage_minimum(1),   0x0,
    usage_maximum(1),   0xFF,
    output(1),          0x2,
    usage(1),           0x2,
    report_count(1),    0x3f,
    report_size(1),     0x8,
    usage_minimum(1),   0x0,
    usage_maximum(1),   0xFF,
    input(1),           0,
    end_collection(0),
    end_collection(0)
};


static usb_hid_mouse_report_t g_mouse_report = { 0 }; // 相对坐标报文数组
static hid_kind_mode g_mouse_move_mode = 0; // 光标移动类型，绝对|相对
static uint16_t g_screen_resolution_x = 0; // 屏幕水平分辨率
static uint16_t g_screen_resolution_y = 0; // 屏幕垂直分辨率
static const uint16_t g_screen_resolution[SCREEN_RESOLUTION_NUM][SCREEN_RESOLUTION_PARAM_NUM] = {
    { 1280, 720 },  // 720P
    { 1920, 1080 }, // 1080P
    { 3840, 2160 }, // 4K
    { 7680, 4320 }, // 8K
}; // 屏幕分辨率参数：分辨率宽，分辨率高；单位：像素
static int32_t g_usb_mouse_hid_index = 0;
uint16_t g_cursor_coordinate_x = 0; // 光标x轴坐标，单位：像素
uint16_t g_cursor_coordinate_y = 0; // 光标y轴坐标，单位：像素
// 用于判定是否进睡眠的变量
static uint16_t g_last_x = 0;
static uint16_t g_last_y = 0;
static uint16_t g_static_count = 0;

// 设置屏幕分辨率
void set_screen_resolution(SlpScreenResolution resolution)
{
    g_screen_resolution_x = g_screen_resolution[resolution][0]; // 0:索引
    g_screen_resolution_y = g_screen_resolution[resolution][1];  // 1:索引
    osal_printk("resolution:%u, %u\r\n", g_screen_resolution_x, g_screen_resolution_y);
}

void set_mouse_move_mode(hid_kind_mode mode)
{
    g_mouse_move_mode = mode;
    osal_printk("mouse mode, %u\r\n", g_mouse_move_mode);
}

// 变量初始化
static void init_hid_variable(void)
{
    (void)memset_s(&g_mouse_report, sizeof(usb_hid_mouse_report_t), 0, sizeof(usb_hid_mouse_report_t));
    set_screen_resolution(SLP_SCREEN_1080P);
    set_mouse_move_mode(HID_MOUSE_REL_KIND);
}

// 使用相对坐标HID移动光标到屏幕中心
void move_cursor_to_center_rel(void)
{
    g_mouse_report.kind = HID_MOUSE_REL_KIND;

    // 移动到右下角
    g_mouse_report.x = g_screen_resolution_x;
    g_mouse_report.y = g_screen_resolution_y;
    fhid_send_data(g_usb_mouse_hid_index, (char *)(&g_mouse_report), sizeof(usb_hid_mouse_report_t));

    uint16_t center_x = g_screen_resolution_x / 2;
    uint16_t center_y = g_screen_resolution_y / 2;

    osal_msleep(10); // 10:ms 两次HID下发留一点间隔，
    // 移动到中心
    g_mouse_report.x = -center_x;
    g_mouse_report.y = -center_y;
    fhid_send_data(g_usb_mouse_hid_index, (char *)(&g_mouse_report), sizeof(usb_hid_mouse_report_t));

    g_cursor_coordinate_x = center_x;
    g_cursor_coordinate_y = center_y;
    osal_printk("move_cursor_to_center_rel, x, %u, y, %u\r\n", g_cursor_coordinate_x, g_cursor_coordinate_y);
}

// 以相对坐标方法更新屏幕光标位置
static void update_mouse_coordinate_by_rel(usb_hid_mouse_report_t *mouse_report)
{
    int16_t delta_x = mouse_report->x - g_cursor_coordinate_x;
    int16_t delta_y = mouse_report->y - g_cursor_coordinate_y;
    g_cursor_coordinate_x = mouse_report->x;
    g_cursor_coordinate_y = mouse_report->y;
    g_mouse_report.x = delta_x;
    g_mouse_report.y = delta_y;
    g_mouse_report.kind = HID_MOUSE_REL_KIND;
    fhid_send_data(g_usb_mouse_hid_index, (char *)(&g_mouse_report), sizeof(usb_hid_mouse_report_t));
}

// 以绝对坐标方法更新屏幕光标位置
static void update_mouse_coordinate_by_abs(usb_hid_mouse_report_t *mouse_report)
{
    g_mouse_report.x = mouse_report->x;
    g_mouse_report.y = mouse_report->y;
    g_mouse_report.kind = HID_MOUSE_ABS_KIND;
    g_cursor_coordinate_x = mouse_report->x;
    g_cursor_coordinate_y = mouse_report->y;
    fhid_send_data(g_usb_mouse_hid_index, (char *)(&g_mouse_report), sizeof(usb_hid_mouse_report_t));
}

void update_mouse_coordinate_by_nearlink(usb_hid_mouse_report_t *rpt)
{
    sle_usb_frame_header_t header = {0x0};
    header.flag = MP_TEST_INFO_ID;
    header.service_id = MP_TEST_SEND_AIR_MOUSE_SERVICE;
    header.command_id = MP_TEST_SEND_AIR_MOUSE_CMD;
    header.body_len = sizeof(header.tlv0) + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(usb_hid_mouse_report_t);
    // tlv0
    header.tlv0.type = 0x1;  // 0x1: 第一个tlv
    header.tlv0.len = 0x1;
    header.tlv0.value = 0x0;
    // tlv1
    header.tlv1.type = 0x2;  // 0x2: 第二个tlv
    header.tlv1.len = sizeof(usb_hid_mouse_report_t);
    memcpy_s(&header.tlv1.value, sizeof(usb_hid_mouse_report_t), rpt, sizeof(usb_hid_mouse_report_t));

    int32_t ret = fhid_send_data(g_usb_mouse_hid_index, (char *)&header, sizeof(sle_usb_frame_header_t));
    if (ret == -1) {
        osal_printk("nearlink send falied! ret: %d\n", ret);
        return;
    }
}

static void check_into_sleep(usb_hid_mouse_report_t *mouse_report)
{
    if ((g_last_x == mouse_report->x) && (g_last_y == mouse_report->y)) {
        g_static_count++;
    } else {
        g_static_count = 0;
    }

    g_last_x = mouse_report->x;
    g_last_y = mouse_report->y;

    if (g_static_count >= SLEEP_COUNT_THRESHOLD) {
        g_static_count = 0;
        ErrcodeSlpClient ret = SlpSleepCommand();
        if (ret != ERRCODE_SLPC_SUCCESS) {
            osal_printk("SlpSleepCommand Error 0x%x\r\n", ret);
        }
    }
}

void update_mouse_coordinate(usb_hid_mouse_report_t *mouse_report)
{
    // 如果需要睡眠，此处调用
    if (CONFIG_LOW_POWER_MODE == 1) {
        check_into_sleep(mouse_report);
    }
    switch (g_mouse_move_mode) {
        case HID_MOUSE_REL_KIND:
            update_mouse_coordinate_by_rel(mouse_report);
            break;
        case HID_MOUSE_ABS_KIND:
            update_mouse_coordinate_by_abs(mouse_report);
            break;
        case HID_NEARLINK_KIND:
            update_mouse_coordinate_by_nearlink(mouse_report);
            break;
        default:
            osal_printk("[ERR] wrong mouse move mode, %u\r\n", g_mouse_move_mode);
            break;
    }
}

// 更新鼠标按键
void update_mouse_key(usb_hid_mouse_report_t *mouse_report)
{
    g_mouse_report.x = 0;
    g_mouse_report.y = 0;
    g_mouse_report.mouse_key.d8 = mouse_report->mouse_key.d8;
    g_mouse_report.kind = HID_MOUSE_REL_KIND;
    fhid_send_data(g_usb_mouse_hid_index, (char *)(&g_mouse_report), sizeof(usb_hid_mouse_report_t));
}

int32_t get_usb_mouse_hid_index(void)
{
    return g_usb_mouse_hid_index;
}

int usb_init_app(device_type dtype)
{
    int32_t usb_hid_index = -1;
    if (g_usb_inited == true) {
        return -1;
    }

    // 制造商
    const char manufacturer[USB_INIT_APP_MANUFACTURER_LEN] = USB_INIT_APP_MANUFACTURER;
    struct device_string str_manufacturer = {
        .str = manufacturer,
        .len = USB_INIT_APP_MANUFACTURER_LEN
    };

    // 产品
    const char product[USB_INIT_APP_PRODUCT_LEN] = USB_INIT_APP_PRODUCT;
    struct device_string str_product = {
        .str = product,
        .len = USB_INIT_APP_PRODUCT_LEN
    };

    // 版本号
    const char serial[USB_INIT_APP_SERIAL_LEN] = USB_INIT_APP_SERIAL;
    struct device_string str_serial_number = {
        .str = serial,
        .len = USB_INIT_APP_SERIAL_LEN
    };

    // 设备版本
    struct device_id dev_id = {
        .vendor_id = 0x1111,
        .product_id = 0x0009,
        .release_num = 0x0800
    };

    // 设置描述符
    if (dtype == DEV_HID) {
        usb_hid_index = hid_add_report_descriptor(g_report_desc_hid, sizeof(g_report_desc_hid), 0);
    }

    // 设置版本设备id
    if (usbd_set_device_info(dtype, &str_manufacturer, &str_product, &str_serial_number, dev_id) != 0) {
        return -1;
    }

    // 切换usb为全速模式
    if (usb_device_set_speed(USB_FULL_SPEED) != 0) {
        return -1;
    }

    if (usb_init(DEVICE, dtype) != 0) {
        return -1;
    }

    g_usb_inited = true;
    return usb_hid_index;
}

int32_t usb_deinit_app(void)
{
    if (g_usb_inited == false) {
        return 0;
    }
    (void)usb_deinit();
    g_usb_inited = false;
    return 0;
}

/* HID设备初始化 */
void dongle_hid_usb_init(void)
{
    g_usb_mouse_hid_index = usb_init_app(DEV_HID); // 设置描述符、设备ID、版本号等，返回usb设备HID
    osal_printk("air mouse usb hid init, %d\n", g_usb_mouse_hid_index);
    if (g_usb_mouse_hid_index < 0) {
        osal_printk("usb_hid_init_fail\n");
    }
    init_hid_variable();
}
