/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: USB App Manager  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#include <stdbool.h>
#include "common_def.h"
#include "gadget/f_hid.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "securec.h"
#include "usb_dfu.h"
#include "sle_low_latency.h"
#include "sle_mouse_client.h"
#include "sle_low_latency_service.h"
#include "sle_errcode.h"
#include "mouse_button.h"
#include "mouse_data_transfer_manage.h"
#include "mouse_mode_manage.h"
#ifdef CONFIG_EXTENED_PROTOCOL
#include "app_public.h"
#endif
#include "usb_init_app.h"


#define USB_INIT_APP_MANUFACTURER                                              \
    {                                                                          \
        'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 \
    }
#define USB_INIT_APP_MANUFACTURER_LEN 20

#define USB_INIT_APP_PRODUCT                                                   \
    {                                                                          \
        'S', 0, 'L', 0, 'E', 0, '_', 0, 'M', 0, 'O', 0, 'U', 0, 'S', 0, 'E', 0 \
    }

#define USB_INIT_APP_PRODUCT_LEN 20
#define USB_INIT_APP_SERIAL                                            \
    {                                                                  \
        '2', 0, '0', 0, '2', 0, '0', 0, '0', 0, '6', 0, '2', 0, '4', 0 \
    }
#define USB_INIT_APP_SERIAL_LEN 16

#define WORD_LENGTH                 4
#define SWITCH_TO_DFU_FLAG          0x1e
#define SWITCH_TO_ACM_FLAG          0x1b
#define CUSTUMER_PAGE_REPORT_ID     0x8
#define CUSTOM_RW_PAGE_REPORT_ID    0x9
#define RECV_MAX_LENGTH             64
#define SEND_MAX_LENGTH             128
#define USB_RECV_STACK_SIZE         0x800
#define USB_RECV_FAIL_DELAY         50
#define USB_COMMAND_LENTH           13
#define USB_DEINIT_DELAY            50
#define PROTOCOL_MOUSE              2
#define DUMP_HID_USE_DATA           0
#define HEAD_OFFSET 0
#define TYPE_OFFSET 1
#define LENGTH_OFFSET 2
#define CMD_OFFSET 4
#define DATA_OFFSET 5
#define MOVE_8BIT 8
#define MOVE_24BIT 24
#define USB_HID_PACKAGE_HEAD 0x05
#define USB_HID_PACKAGE_TYPE 0x5A

// bInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
#define USB_PROTOCOL_NONE 0
#define USB_PROTOCOL_KEYBOARD 1
#define USB_PROTOCOL_MOUSE 2

#define input(size) (0x80 | (size))
#define output(size) (0x90 | (size))
#define feature(size) (0xb0 | (size))
#define collection(size) (0xa0 | (size))
#define end_collection(size) (0xc0 | (size))
#define usage_page(size) (0x04 | (size))
#define logical_minimum(size) (0x14 | (size))
#define logical_maximum(size) (0x24 | (size))
#define physical_minimum(size) (0x34 | (size))
#define physical_maximum(size) (0x44 | (size))
#define uint_exponent(size) (0x54 | (size))
#define uint(size) (0x64 | (size))
#define report_size(size) (0x74 | (size))
#define report_id(size) (0x84 | (size))
#define report_count(size) (0x94 | (size))
#define push(size) (0xa4 | (size))
#define pop(size) (0xb4 | (size))
#define usage(size) (0x08 | (size))
#define usage_minimum(size) (0x18 | (size))
#define usage_maximum(size) (0x28 | (size))
#define designator_index(size) (0x38 | (size))
#define designator_minimum(size) (0x48 | (size))
#define designator_maximum(size) (0x58 | (size))
#define string_index(size) (0x78 | (size))
#define string_minimum(size) (0x88 | (size))
#define string_maximum(size) (0x98 | (size))
#define delimiter(size) (0xa8 | (size))

// USB是否初始化标识
static bool g_usb_inited = false;
/* HID大多采用短项目的编码形式如下：
Bits 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4  3     2    1     0
     [data                  ][data               ] [bTag ]  [bType]    [bSize]
例如：
0x05 = 0000 01 01 解析说明如下
bTag=0000:Usage Page
bType= 01:主项目描述(bType = 0：主项目；bType = 1：全局项目；bType = 2：区域项目)
bSize= 01:单位1字节-----表示Usage Page内容字节数据1个字节.

0x09 = 0000 10 01 解析说明如下
bTag=0000:Usage Page
bType= 10:区域项目
bSize= 01:单位1字节
*/
static uint8_t g_report_desc_hid[] = {
    usage_page(1),      0x01, // 0x05,0x01,//全局项目描述，表示HID设备，桌面控制
    usage(1),           0x06, // 0x09,0x06,//区域项描述，表示HID设备是键盘
    collection(1),      0x01, // 0xA1,0x01,//应用集合开始
    report_id(1),       0x02, // 0x85,0x02,//卷标Report ID，必须从1起算，不可使用0
    usage_page(1),      0x07, // 0x05,0x07,//全局项目描述，表示Keypad
    usage_minimum(1),   0xE0, // 0x19,0xE0,//区域描述Logical_minnum
    usage_maximum(1),   0xE7, // 0x29,0xE7,//区域描述Logical_maxnum//E0-E7 8个按键
    logical_minimum(1), 0x00, // 0x15,0x00,//全局描述Logical_minnum
    logical_maximum(1), 0x01, // 0x25,0x01,//全局描述Logical_maxnum//0和1分别表示按下和松开
    report_size(1),     0x01, // 0x75,0x01,//表示上报数据格式1bit数据
    report_count(1),    0x08, // 0x95,0x08,//表示上报数据数量8个
    input(1),           0x02, // 0x81,0x02,//表示输入描述，表示将8个bit的数据加到report中去
    report_count(1),    0x01, // 0x95,0x01,//表示上报数据数量1个
    report_size(1),     0x08, // 0x85,0x01,//表示上报数据格式8bit数据
    input(1),           0x01, // 0x85,0x01,//表示输入描述，预留1个8bit备用，暂时没用
    report_count(1),    0x05, // 0x95,0x05,//表示上报数据数量5个
    report_size(1),     0x01, // 0x75,0x01,//表示上报数据格式1bit
    usage_page(1),      0x08, // 0x05,0x08,//全局项目描述，表示LED控制
    usage_minimum(1),   0x01, // 0x19,0x01,//区域描述Logical_minnum
    usage_maximum(1),   0x05, // 0x29,0x05,//区域描述Logical_maxnum，对应5个LED灯
    output(1),          0x02, // 0x91,0x02,//输出描述(Data,Var,Abs)
    report_count(1),    0x01, // 0x95,0x01,//表示上报数据数量1个
    report_size(1),     0x03, // 0x75,0x03,//表示上报数据格式3bit
    output(1),          0x01, // 0x91,0x01,//表示输出描述，添加3bit,与上面5个bit刚好组成1字节
    report_count(1),    0x06, // 0x95,0x06,//表示上报数据数量6个
    report_size(1),     0x08, // 0x75,0x08,//表示上报数据格式8bit
    logical_minimum(1), 0x00, // 0x15,0x00,//全局描述Logical_minnum
    logical_maximum(1), 0x65, // 0x25,0x65,//全局描述Logical_maxnum
    usage_page(1),      0x07, // 0x05,0x07,//全局项目描述，表示Keypad
    usage_minimum(1),   0x00, // 0x19,0x00,//区域描述Logical_minnum
    usage_maximum(1),   0x65, // 0x29,0x00,//区域描述Logical_maxnum
    input(1),           0x00, // 0x81,0x00,//表示输入 (Data,Var,Abs)，将这6个字节添加到本报告中，表示可以产生6个键值，对应按下哪个按键
    end_collection(0), // 0xC0,     //应用集合描述结束符合，与0xA1要对应

    usage_page(1),      0x01, // 0x05,0x01,//全局项目描述，表示HID设备，桌面控制
    usage(1),           0x02, // 0x09,0x02,//区域项描述，表示HID设备是鼠标
    collection(1),      0x01, // 0xA1,0x01,//应用集合开始
    report_id(1),       0x01, // 0x85,0x02,//卷标Report ID，必须从1起算，不可使用0
    usage(1),           0x01, // 0x09,0x01,//区域项描述，表示HID设备指针
    collection(1),      0x00, // 0xA1,0x01,//应用集合开始
    report_count(1),    0x05, // 0x95,0x05,//表示上报数据数量5个
    report_size(1),     0x01, // 0x75,0x01,//表示上报数据格式1bit
    usage_page(1),      0x09, // 0x05,0x09,//全局项目描述,按键描述
    usage_minimum(1),   0x01, // 0x19,0x01,//区域描述Logical_minnum
    usage_maximum(1),   0x05, // 0x29,0x05,//区域描述Logical_maxnum
    logical_minimum(1), 0x00, // 0x15,0x00,//全局描述Logical_minnum
    logical_maximum(1), 0x01, // 0x25,0x01,//全局描述Logical_maxnum
    input(1),           0x02, // 0x81,0x02,//表示输入 (Data,Var,Abs)
    report_count(1),    0x01, // 0x95,0x01,//表示上报数据数量1个
    report_size(1),     0x03, // 0x75,0x03,//表示上报数据格式3bit
    input(1),           0x01, // 0x81,0x01,//表示输入 (Data,Var,Abs)
    usage_page(1),      0x01, // 0x05,0x01,//全局项目描述，表示HID设备，桌面控制
    usage(1),           0x38, // 0x09,0x38,//区域项描述，表示HID设备是鼠标Wheel
    report_count(1),    0x01, // 0x95,0x01,//表示上报数据数量1个
    report_size(1),     0x08, // 0x75,0x08,//表示上报数据格式8bit
    logical_minimum(1), 0x81, // 0x15,0x81,//全局描述Logical_minnum(-129)
    logical_maximum(1), 0x7f, // 0x25,0x7f,//全局描述Logical_maxnum(127)
    input(1),           0x06, // 0x81,0x06,//表示输入 (Data,Var,Abs)
    usage(1),           0x30, // 0x09,0x30,//区域项描述，表示HID设备是鼠标X
    usage(1),           0x31, // 0x09,0x31,//区域项描述，表示HID设备是鼠标Y
    report_count(1),    0x02, // 0x95,0x02,//表示上报数据数量2个
    report_size(1),     0x10, // 0x75,0x10,//表示上报数据格式16bit
    logical_minimum(2), 0x01, 0x80, // 0x16,0x8001,//全局描述Logical_minnum
    logical_maximum(2), 0xff, 0x7f, // 0x126,0x7fff,//全局描述Logical_minnum
    input(1),           0x06, // 0x81,0x06,//表示输入 (Data,Var,Abs)
    end_collection(0),
    end_collection(0),

    // For dfu update.
    // Burntool need choose the device which usage_page is 0xFFB1 to switch to dfu mode and upgrade
    usage_page(2), 0xB1, 0xFF,
    usage(1),           0x1,
    collection(1),      0x01,
    report_id(1),       0x08,
    collection(1),      0x00,
    report_count(1),    0xc,
    report_size(1),     0x8,
    usage_minimum(1),   0x0,
    usage_maximum(1),   0xFF,
    logical_minimum(1), 0,
    logical_maximum(1), 0xff,
    output(1),          2,
    end_collection(0),
    end_collection(0),

    // For customer data update. usage_page is 0xFFB2,Report_ID =09
    usage_page(2), 0xB2, 0xFF,
    usage(1),           0x1,
    collection(1),      0x01,
    report_id(1),       0x09,
    collection(1),      0x00,
    report_count(1),    0x3e,
    report_size(1),     0x8,
    usage_minimum(1),   0x0,
    usage_maximum(1),   0xFF,
    logical_minimum(1), 0,
    logical_maximum(1), 0xff,
    output(1),           2,
    usage(1),           0x2,
    report_count(1),    0x3e,
    report_size(1),     0x8,
    usage_minimum(1),   0x0,
    usage_maximum(1),   0xFF,
    logical_minimum(1), 0,
    logical_maximum(1), 0xff,
    input(1),           0,
    end_collection(0),
    end_collection(0)

};

typedef struct {
    uint32_t start_flag;
    uint16_t packet_size;
    uint8_t frame_type;
    uint8_t frame_type_reserve;
    uint16_t flag;
    uint16_t check_sum;
} seboot_switch_dfu_t;

typedef enum {
    USB_RECV_HID,
    USB_RECV_SUSPEND,
} usb_recv_state_t;

static usb_recv_state_t g_usb_recv_state = USB_RECV_HID;
static osal_task *g_recv_task = NULL;
static int g_hid_mouse_index = -1;
static usb_customer_cmd_cb g_usb_customer_cmd_cb = NULL;
const uint16_t g_report_rate_idx_tab[7] = {SLE_LOW_LATENCY_125HZ,
    SLE_LOW_LATENCY_250HZ,
    SLE_LOW_LATENCY_500HZ,
    SLE_LOW_LATENCY_1K,
    SLE_LOW_LATENCY_2K,
    SLE_LOW_LATENCY_4K,
    SLE_LOW_LATENCY_8K};

enum mouse_dpi {
    DPI_400 = 400,
    DPI_800 = 800,
    DPI_1600 = 1600,
    DPI_3200 = 3200,
    DPI_6400 = 6400,
};

const uint32_t g_dpi_idx_tab[5] = {DPI_400, DPI_800, DPI_1600, DPI_3200, DPI_6400};

int get_hid_mouse_index(void)
{
    return g_hid_mouse_index;
}

bool get_usb_inited(void)
{
    return g_usb_inited;
}

void start_usb_mode(void)
{
    // 预留操作
    return;
}

uint32_t swap_endian32(uint32_t val)
{
    return (val >> MOVE_24BIT) | ((val << MOVE_8BIT) & 0x00FF0000) | ((val >> MOVE_8BIT) & 0x0000FF00) |
           (val << MOVE_24BIT);
}

// 数据帧格式：Head(1Byte)+Type(1Byte)+Length(2Byte不含Head和Type)+Cmd(1Byte)+Data(NByte)
// 例如：05 5a 00 05 01 00 00 0b b8
#if defined(CONFIG_SAMPLE_SUPPORT_GAME_MOUSE_DONGLE)
static int usb_hid_parser_recv_data(uint8_t *buf)
{
    uint8_t package_head = buf[HEAD_OFFSET];
    uint8_t package_type = buf[TYPE_OFFSET];
    uint16_t package_length = (uint16_t)((buf[LENGTH_OFFSET + 1] << MOVE_8BIT) | (buf[LENGTH_OFFSET]));
    if (package_head != USB_HID_PACKAGE_HEAD || package_type != USB_HID_PACKAGE_TYPE || package_length < 1) {
        osal_printk("\r\n***** usb_hid_recv_data format error ******\r\n");
        return ERRCODE_SLE_FAIL;
    }
    usb_app_customer_cmd_t package_command = buf[CMD_OFFSET];
    uint32_t package_data = swap_endian32(*((uint32_t *)&buf[DATA_OFFSET]));

    switch (package_command) {
        case CMD_VERSION_READ:
            break;
        case CMD_CHANGE_MOUSE_DPI:
            set_sle_dongle_dpi_data(package_data);
            app_dongle_send_msg(SLE_DONGLE_SET_DPI, 0);
            break;
        case CMD_SET_MOUSE_WORK_MODE:
            set_sle_dongle_report_rate(g_report_rate_idx_tab[package_data]);
            app_dongle_send_msg(SLE_DONGLE_CHANGE_MOUSE_REPORT_RATE, 0);
            break;
        case CMD_DPI_CHECK:
            sle_mouse_client_ssapc_write_req(COMMAND_DPI_CHECK, NULL, 0);
            break;
        case CMD_REPORT_CHECK:
            sle_mouse_client_ssapc_write_req(COMMAND_REPORT_CHECK, NULL, 0);
            break;
        case CMD_BATTERY_LEVEL_CHECK:
            sle_mouse_client_ssapc_write_req(COMMAND_BATTERY_LAVEL_CHECK, NULL, 0);
            break;
        case CMD_START_SEEK:
            sle_mouse_client_set_is_pair_complete(false);
            app_dongle_send_msg(SLE_DONGLE_SCAN_DEVICES, 0);
        default:
            break;
    }
    return 0;
}
#endif

// 有线模式下接收上位机命令
#if defined(CONFIG_SAMPLE_SUPPORT_GAME_MOUSE)
static int usb_hid_parser_recv_data_server(uint8_t *buf)
{
    uint8_t package_head = buf[HEAD_OFFSET];
    uint8_t package_type = buf[TYPE_OFFSET];
    uint16_t package_length = (uint16_t)((buf[LENGTH_OFFSET + 1] << MOVE_8BIT) | (buf[LENGTH_OFFSET]));
    if (package_head != USB_HID_PACKAGE_HEAD || package_type != USB_HID_PACKAGE_TYPE || package_length < 1) {
        osal_printk("\r\n***** usb_hid_recv_data format error ******\r\n");
        return ERRCODE_SLE_FAIL;
    }
    usb_app_customer_cmd_t package_command = buf[CMD_OFFSET];
    uint32_t package_data = swap_endian32(*((uint32_t *)&buf[DATA_OFFSET]));

    switch (package_command) {
        case CMD_VERSION_READ:
            break;
        case CMD_CHANGE_MOUSE_DPI:
            app_mouse_dpi_change(package_data);
            break;
        case CMD_SET_MOUSE_WORK_MODE:
            mouse_srv_change_report_rate(package_data);
            break;
        case CMD_DPI_CHECK:
            break;
        case CMD_REPORT_CHECK:
            break;
        case CMD_BATTERY_LEVEL_CHECK:
            break;
        case CMD_START_SEEK:
        default:
            break;
    }
    return 0;
}
#endif

#ifndef CONFIG_EXTENED_PROTOCOL
static uint8_t usb_hid_recv_data(void)
{
    static uint32_t recv_count = 0;
    uint8_t recv_data[RECV_MAX_LENGTH];
    for (;;) {
        int32_t ret = fhid_recv_data(g_hid_mouse_index, (char*)recv_data, RECV_MAX_LENGTH);
        if (ret <= 0) {
            osal_msleep(USB_RECV_FAIL_DELAY);
            continue;
        }

        if (ret == USB_COMMAND_LENTH && recv_data[0] == CUSTUMER_PAGE_REPORT_ID) {
            seboot_switch_dfu_t command;
            if (memcpy_s(&command, sizeof(seboot_switch_dfu_t), &recv_data[1], USB_COMMAND_LENTH - 1) != EOK) {
                continue;
            }
            if (command.frame_type == SWITCH_TO_DFU_FLAG) {
                osal_printk("start dfu\n");
                usb_deinit();
                osal_msleep(USB_DEINIT_DELAY);
                sample_usb_dfu_init();
                sample_usb_dfu_wait_ugrade_done_and_reset();
                break;
            }
        }

        if (ret <= RECV_MAX_LENGTH && recv_data[0] == CUSTOM_RW_PAGE_REPORT_ID) {
#if defined(CONFIG_SAMPLE_SUPPORT_GAME_MOUSE_DONGLE)
            usb_hid_parser_recv_data(&recv_data[1]);
#elif defined(CONFIG_SAMPLE_SUPPORT_GAME_MOUSE)
            usb_hid_parser_recv_data_server(&recv_data[1]);
#endif
            recv_count++;
            osal_printk("\nrecv_count:%d\r\n", recv_count);
        }
    }
    return 0;
}
#endif

static int usb_recv_task(uint32_t *para)
{
    unused(para);
    for (;;) {
        switch (g_usb_recv_state) {
            case USB_RECV_HID:
#ifdef CONFIG_EXTENED_PROTOCOL
                app_public_pc_command_pro();
#else
                usb_hid_recv_data();
#endif
                break;
            case USB_RECV_SUSPEND:
                osal_kthread_suspend(g_recv_task);
                break;
        }
    }
    osal_printk("usb recv task over\n");
    return 0;
}

int usb_init_app(device_type dtype)
{
    if (g_usb_inited == true) {
        return -1;
    }

    const char manufacturer[USB_INIT_APP_MANUFACTURER_LEN] = USB_INIT_APP_MANUFACTURER;
    struct device_string str_manufacturer = {
        .str = manufacturer,
        .len = USB_INIT_APP_MANUFACTURER_LEN
    };

    const char product[USB_INIT_APP_PRODUCT_LEN] = USB_INIT_APP_PRODUCT;
    struct device_string str_product = {
        .str = product,
        .len = USB_INIT_APP_PRODUCT_LEN
    };

    const char serial[USB_INIT_APP_SERIAL_LEN] = USB_INIT_APP_SERIAL;
    struct device_string str_serial_number = {
        .str = serial,
        .len = USB_INIT_APP_SERIAL_LEN
    };
// 鼠标支持USB模式的情况下建议Dongle的PID与鼠标的Pid做区分配置
#if defined(CONFIG_SAMPLE_SUPPORT_GAME_MOUSE_DONGLE)
    struct device_id dev_id = {
        .vendor_id = 0x1111,
        .product_id = 0x000f,
        .release_num = 0x0800
    };
#else
    struct device_id dev_id = {
        .vendor_id = 0x1111,
        .product_id = 0x0009,
        .release_num = 0x0800
    };
#endif

    if (dtype == DEV_HID || dtype == DEV_SER_HID) {
        // bInterfaceSubClass : 1=BOOT, 0=no boot
        uint8_t boot_protocol = 1;
        g_hid_mouse_index = hid_add_report_descriptor(g_report_desc_hid, sizeof(g_report_desc_hid), USB_PROTOCOL_MOUSE);
        fhid_set_interface_boot_protocol(USB_MOUSE_DEVICE_ID, boot_protocol);
    }

    if (usbd_set_device_info(dtype, &str_manufacturer, &str_product, &str_serial_number, dev_id) != 0) {
        osal_printk("set device info fail!\n");
        return -1;
    }

    if (usb_init(DEVICE, dtype) != 0) {
        osal_printk("usb_init failed!\n");
        return -1;
    }
    if (g_recv_task == NULL) {
        g_recv_task = osal_kthread_create((osal_kthread_handler)usb_recv_task, NULL, "usb_recv_task",
            USB_RECV_STACK_SIZE);
    }

    g_usb_inited = true;
    g_usb_recv_state = USB_RECV_HID;
    return g_hid_mouse_index;
}

void usb_register_customer_cmd(usb_customer_cmd_cb cus_cmd_cb)
{
    g_usb_customer_cmd_cb = cus_cmd_cb;
}

int usb_deinit_app(void)
{
    if (g_usb_inited == false) {
        return 0;
    }
    (void)usb_deinit();
    g_usb_inited = false;
    g_usb_recv_state = USB_RECV_SUSPEND;
    return 0;
}

int close_usb_mode(void)
{
    return usb_deinit_app();
}