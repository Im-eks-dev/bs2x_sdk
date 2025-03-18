/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Mouse Manager. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */
#ifndef MOUSE_MODE_MANAGE
#define MOUSE_MODE_MANAGE
#include "mouse_button/mouse_button.h"

#define MOUSE_AUTO_TEST         0 // 0 -鼠标正常模式，1 -鼠标自动上报坐标测试模式
#define MOUSE_EN_CHANGE_CPU_CLK 1 // BS25:0 -鼠标工作在64MHz模式，1 -鼠标工作在96Mhz模式-BS21:1-鼠标工作在64MHz模式

typedef enum {
    SLIDE_SW_MODE_BLE,   // 拨码开关模式选择：BLE
    SLIDE_SW_MODE_SLE,   // 拨码开关模式选择：SLE
    SLIDE_SW_MODE_USB,   // 拨码开关模式选择：USB
} app_slide_sw_mode_t;

typedef enum {
    BLE_MOUSE_MODE = 0x00, // 单BLE鼠标模式
    SLE_MOUSE_MODE = 0x01, // 单SLE鼠标模式
    USB_MOUSE_MODE = 0x02, // 单USB鼠标模式
    MIX_MOUSE_MODE = 0x03, // 多模混合鼠标模式
    INVALID_MOUSE_MODE = 0xFF,
} app_mouse_type_t;

typedef enum {
    SLE_MODE_INVALID,
    SLE_MODE_DIRECTLY_CONNECT,
    SLE_MODE_DONGLE_COPNNECT,
} app_sle_mode_t;

typedef enum {
    MSG_ID_PROTOCOL_ENABLED,    // BLE,SLE协议栈使能成功回调消息
    MSG_ID_MODE_PIN_CHANGE,     // 鼠标档位按键引脚变换消息
    MSG_ID_USB_MANAGE,          // USB管理
    MSG_ID_CONNECTED,           // 鼠标建立连接(连接类型：SLE、BLE成功Connect，USB成功识别PC),开启数据传输
    MSG_ID_DISCONNECTED,        // 鼠标断开连接或无连接
    MSG_ID_OTA_MODE,            // 鼠标OTA模式
    MSG_ID_LED_CHANGE,          // LED灯效变换
    MSG_ID_BUTTON_MANAGE,       // 按键管理
    MSG_ID_MODE_SWITCH,         // 鼠标模式改变(BLE-USB-SLE)
    MSG_ID_LOW_POWER,           // 鼠标准备进入Sleep模式处理
    MSG_ID_LOG_MANAGE,          // 日志管理
    MSG_ID_ADC_MANAGE,          // ADC采样管理
    MSG_ID_CHARGER_MANAGE,      // Charger管理
} msg_id_t;

typedef enum {
    MSG_TYPE_USB_PIN_PULL_OUT,       // USB引脚拔出设备
    MSG_TYPE_USB_PIN_PLUG_IN,        // USB引脚插入设备
} msg_type_usb_t;

typedef enum {
    MSG_TYPE_DISCONN_BY_REMOTE,   // 对端用户断链
    MSG_TYPE_DISCONN_BY_LOCAL, // 本端断链
    MSG_TYPE_ENABLE_DATA_SEND,    // 连接成功使能数据发送
    MSG_TYPE_DATA_SYNC,       // 连接后自定义消息交互
    MSG_TYPE_ADV_TERMINATE    // 广播被动中止，补发广播
} msg_type_protocol_t;

typedef enum {
    MSG_TYPE_DPI_CHANGE,
    MSG_TYPE_DPI_UP,             // 鼠标DPI速率调整
    MSG_TYPE_DPI_DOWN,
    MSG_TYPE_REPORT_RATE_CHANGE,     // 鼠标回报率切换调整
    MSG_TYPE_CHANNEL_CHANGE,
    MSG_TYPE_PAIR_PROCESS
} msg_type_button_t;

typedef enum {
    MSG_TYPE_GOTO_STANDBY,
    MSG_TYPE_GOTO_SLEEP,
    MSG_TYPE_GOTO_WORK,
} msg_type_lowpower_t;

typedef enum {
    MSG_TYPE_HSO_LOG_OFF,       // HSO日志关闭
    MSG_TYPE_HSO_LOG_ON,        // HSO日志打开
    MSG_TYPE_SERIAL_LOG_OFF,    // 串口日志关闭
    MSG_TYPE_SERIAL_LOG_ON      // 串口日志打开
} msg_type_log_t;

typedef enum {
    MSG_TYPE_ADC_OPEN_WITH_CALI,        /* 建议：第一次上电调用，耗时长 */
    MSG_TYPE_ADC_OPEN_WITHOUT_CALI,     /* 建议：非第一次上电调用 */
    MSG_TYPE_ADC_SAMPLE_AND_CLOSE,
} msg_type_sdc_t;

typedef enum {
    MSG_TYPE_BAT_INFO,
    MSG_TYPE_BAT_LEVEL,
    MSG_TYPE_START_CHARGE,
    MSG_TYPE_STOP_CHARGE,
    MSG_TYPE_FACTORY_MODE,
} msg_type_charger_t;

typedef enum {
    SLE_CUSTOMER_REQ_CHANGE_REPORT_RATE,
    SLE_CUSTOMER_REQ_CHANGE_DPI,
    SLE_CUSTOMER_REQ_CHECK_REPORT_RATE,
    SLE_CUSTOMER_REQ_CHECK_DPI,
    SLE_CUSTOMER_RESPONSE_DATA,
    SLE_CUSTOMER_REQ_OTHER_DATA,
    SLE_CUSTOMER_WAKEUP_OR_SLEEP,
    SLE_CUSTOMER_BATTERY_LAVEL_CHECK,
    SLE_CUSTOMER_MOUSE_DATA,
    SLE_CUSTOMER_INVAILD_CMD = 0xFF,
} app_customer_cmd_t;

typedef struct {
    uint8_t  msg_id;
    uint8_t  msg_type;
    uint16_t buf_len;
    uint8_t  *buf;          /* 该指针由调用者申请，在消息队列结束时释放。 */
} app_mouse_msg_block_t;

/*
 * @description: 通用鼠标数据结构
 */
typedef struct app_mouse_hid_report {
    uint8_t kind;
    mouse_key_t key;
    int8_t x;
    int8_t y;
    int8_t wheel;
} app_mouse_hid_report_t;

typedef struct app_mouse_dpi_rate {
    uint16_t sle_dongle_rate;
    uint16_t usb_rate;
    uint16_t mouse_dpi;
} app_mouse_dpi_rate_t;

/*
 * @description: 通用鼠标行为操作数据结构
 * @version: V0.0.0
 */
typedef struct app_mouse_handle {
    uint8_t slide_sw_mode;
    uint8_t current_mode;
    uint8_t sle_mode;
    uint16_t curr_report_rate;
    uint16_t work_report_rate;
    uint8_t mouse_is_sleep : 1;
    uint8_t first_init_index : 1;
    uint8_t usb_is_plugin : 1;
    uint8_t usb_is_device_mode : 1; // 区分USB插入的是USB主机设备还是USB-DC充电
    uint8_t usb_detect_times : 4;
    int8_t usb_hid_index;
    uint8_t channel_id;
    unsigned long msg_id;
    uint32_t dpi_value;
} app_mouse_handle_t;

typedef enum {
    REPORT_RATE_125HZ,
    REPORT_RATE_250HZ,
    REPORT_RATE_500HZ,
    REPORT_RATE_1000HZ,
    REPORT_RATE_2000HZ,
    REPORT_RATE_4000HZ,
    REPORT_RATE_8000HZ,
    REPORT_RATE_LENGTH,
} mouse_report_rate_t;

enum mouse_dpi_value {
    DPI_VALUE_400 = 400,
    DPI_VALUE_800 = 800,
    DPI_VALUE_1600 = 1600,
    DPI_VALUE_3200 = 3200,
    DPI_VALUE_6400 = 6400,
};

int app_mouse_get_usb_mode_hid_index(void);
void set_app_mouse_curr_report_rate(uint16_t rate);
uint16_t get_app_mouse_curr_report_rate(void);
void set_app_mouse_work_report_rate(uint16_t rate);
uint16_t get_app_mouse_work_report_rate(void);
app_slide_sw_mode_t app_mouse_current_id(void);
void app_mouse_send_msg(uint8_t msg_id, uint8_t msg_type);
void app_mouse_sleep_function_process(void);
void app_mouse_restore_work_seting(void);
void app_mouse_customer_pm_init(void);
void entry_pair_process(void);
void mouse_srv_change_channel(uint8_t init);
void set_app_mouse_report_rate(uint16_t rate);
app_mouse_handle_t get_app_mouse_handle(void);
void app_mouse_send_battery_percentage_to_dongle(void);
void app_mouse_usb_init(void);
void mouse_srv_change_report_rate(uint8_t temp_rate);
bool sle_mouse_get_status(void);
void sle_mouse_set_status(bool status);
void sle_set_read_remote_private_feature(uint8_t is_read);
#endif
