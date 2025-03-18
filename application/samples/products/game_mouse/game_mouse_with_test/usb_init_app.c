/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: Mouse PCBA TEST SAMPLE. \n
 * Author: @CompanyNameMagicTag \n
 * History: \n
 * 2022-07-27, Create file. \n
 */

#include <stdbool.h>
#include "common_def.h"
#include "gadget/f_hid.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "securec.h"
#include "sle_device_manager.h"
#include "at_bt_cmd_table.h"
#include "bts_factory.h"
#include "at_btc_product.h"
#include "at_bt_cmd_table.h"
#include "app_mp_test_mode.h"
#include "usb_init_app.h"

#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
#include "console.h"
#include "gadget/usbd_acm.h"
#include "at_product.h"
#define TEST_BREAK_LEN           (5)
#define SLP_SERIAL_TASK_PRIO     (26)
#define SERIAL_TASK_STACK_SIZE      0x400
#define SERIAL_RECV_DATA_MAX_LEN    64
static char g_usb_serial_recv_data[SERIAL_RECV_DATA_MAX_LEN];
#endif

#define USB_INIT_APP_MANUFACTURER                                              \
    {                                                                          \
        'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 \
    }
#define USB_INIT_APP_MANUFACTURER_LEN 20
#define USB_INIT_APP_PRODUCT                                                   \
    {                                                                          \
        'S', 0, 'L', 0, 'E', 0, '_', 0, 'D', 0, 'O', 0, 'N', 0, 'G', 0, 'L', 0, 'E', 0 \
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
#define MOUSE_TEST_REPORT_ID        0x1F
#define RECV_MAX_LENGTH             32
#define SEND_MAX_LENGTH             128
#define USB_RECV_STACK_SIZE         0x500
#define USB_RECV_FAIL_DELAY         50
#define USB_COMMAND_LENTH           13
#define USB_DEINIT_DELAY            50
#define PROTOCOL_MOUSE              2
#define DUMP_HID_USE_DATA           1
#define COMMADN_INDEX               2

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
    report_count(1),    0x3f,
    report_size(1),     0x8,
    usage_minimum(1),   0x0,
    usage_maximum(1),   0xFF,
    logical_minimum(1), 0,
    logical_maximum(1), 0xff,
    output(1),           2,
    usage(1),           0x2,
    report_count(1),    0x3f,
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

bool get_usb_inited(void)
{
    return g_usb_inited;
}

void start_usb_mode(void)
{
    // 预留操作
    return;
}

static void security_test_single_carrier(uint8_t *buf)
{
    rf_single_tone_param_args_t param = { 0 };
    uint8_t index = COMMADN_INDEX;
    param.rf_freq = buf[index++];
    param.rf_mode = buf[index++];
    bt_at_rf_single_tone_cmd(&param);
}

// USB-HID自定义数据解析功能函数
static int usb_hid_parser_recv_data(uint8_t *buf)
{
    usb_app_customer_cmd_t package_command = buf[1];

    switch (package_command) {
        case CMD_MP_TEST:
            pcba_test_usb_command_pro(buf);
            break;
        case CMD_SINGLE_CARRIER_TEST:
            security_test_single_carrier(buf);
            break;
        case CMD_MODULATED_WAVE_TEST:
            {
                sle_rf_tx_param_args_t param = { 0 };
                uint8_t index = COMMADN_INDEX;
                param.tx_freq = buf[index++];
                param.tx_power = buf[index++];
                param.test_data_len = buf[index++];
                param.pk_payload_type = buf[index++];
                param.tx_phy = buf[index++];
                param.tx_format = buf[index++];
                param.tx_rate = buf[index++];
                param.tx_pilot_ratio = buf[index++];
                param.tx_polar_r = buf[index++];
                param.tx_interval = buf[index++];
                bt_at_sle_rf_tx_cmd(&param);
            }
            break;
        case CMD_RECEIVE_TEST:
            {
                sle_rf_rx_param_args_t param = { 0 };
                uint8_t index = COMMADN_INDEX;
                param.rx_freq = buf[index++];
                param.rx_phy = buf[index++];
                param.rx_format = buf[index++];
                param.rx_pilot_ratio = buf[index++];
                param.rx_interval = buf[index++];
                bt_at_sle_rf_rx_cmd(&param);
            }
            break;
        case CMD_TXRX_END_TEST:
            bt_at_sle_rf_trxend_cmd();
            break;
        default:
            break;
    }
    return 0;
}

static uint8_t usb_hid_recv_data(void)
{
    static uint32_t recv_count = 0;
    enable_sle();
    bt_at_sle_register_callback_cmd();
    uint8_t recv_data[RECV_MAX_LENGTH];
    for (;;) {
        int32_t ret = fhid_recv_data(1, (char*)recv_data, RECV_MAX_LENGTH);
        if (ret <= 0) {
            osal_msleep(USB_RECV_FAIL_DELAY);
            continue;
        }

        if (ret <= RECV_MAX_LENGTH && recv_data[0] == MOUSE_TEST_REPORT_ID) {
            usb_hid_parser_recv_data(&recv_data[0]);
            recv_count++;
            osal_printk("\nrecv_count:%d\r\n", recv_count);
        }
    }
    return 0;
}

static int usb_recv_task(uint32_t *para)
{
    unused(para);
    for (;;) {
        switch (g_usb_recv_state) {
            case USB_RECV_HID:
                usb_hid_recv_data();
                break;
            default :
                break;
        }
    }
    osal_printk("usb recv task over\n");
    return 0;
}

#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
static int usb_serial_recv_data(void *data)
{
    unused(data);
    usb_serial_ioctl(0, CONSOLE_CMD_RD_BLOCK_SERIAL, 1);
    for (;;) {
        ssize_t recv_len = usb_serial_read(0, g_usb_serial_recv_data, SERIAL_RECV_DATA_MAX_LEN);
        if (recv_len <= 0) {
            osal_msleep(USB_RECV_FAIL_DELAY);
            continue;
        }

        osal_printk("recv_len: %u\r\n", recv_len);
        usb_serial_write(0, g_usb_serial_recv_data, recv_len);
        if (memcmp(g_usb_serial_recv_data, "AT", 0x2) == 0) {
            uapi_at_channel_data_recv(0, (uint8_t *)g_usb_serial_recv_data, recv_len);
        }
        if (memcmp(g_usb_serial_recv_data, "break", TEST_BREAK_LEN) == 0) {
            break;
        }
    }
    return -1;
}
#endif

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

    struct device_id dev_id = {
        .vendor_id = 0x1111,
        .product_id = 0x0009,
        .release_num = 0x0800
    };

    if (dtype == DEV_HID || dtype == DEV_SER_HID) {
        g_hid_mouse_index = hid_add_report_descriptor(g_report_desc_hid, sizeof(g_report_desc_hid), 0);
    }

    if (usbd_set_device_info(dtype, &str_manufacturer, &str_product, &str_serial_number, dev_id) != 0) {
        return -1;
    }

    if (usb_init(DEVICE, dtype) != 0) {
        return -1;
    }

    if (g_recv_task == NULL) {
        g_recv_task = osal_kthread_create((osal_kthread_handler)usb_recv_task, NULL, "usb_recv_task",
                                          USB_RECV_STACK_SIZE);
    }
#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
    osal_task *task_handle = NULL;
    task_handle = osal_kthread_create(
        (osal_kthread_handler)usb_serial_recv_data, 0, "SerialTask", SERIAL_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLP_SERIAL_TASK_PRIO - 1);  // 设置任务优先级
    }
#endif
    g_usb_inited = true;
    g_usb_recv_state = USB_RECV_HID;
    return g_hid_mouse_index;
}