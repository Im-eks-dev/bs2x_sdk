/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: BLE Mouse BAS and DIS Service Server SAMPLE. \n
 * Author: @CompanyNameMagicTag \n
 * History: \n
 * 2024-04-20, Create file. \n
 */
#ifndef APP_PUBLIC_H
#define APP_PUBLIC_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define U32_MAX ((uint32_t)0xffffffff)
#define U16_MAX ((uint16_t)0xffff)
#define U8_MAX ((uint8_t)0xff)
#define U31_MAX ((uint32_t)0x7fffffff)
#define U15_MAX ((uint16_t)0x7fff)
#define U7_MAX ((uint8_t)0x7f)

#define CLOCK_CONTROL_DEBUG 1
#define TEST_AUTO_CYCLE 0
#define APP_COMMAND_DEBUG 1
#define APP_FLASH_DEBUG 1
#define APP_TIMER_DEBUG   1
#define USB_TEST_SENSOR_REMOVE_ZERO     1
#define SLE_TEST_SENSOR_REMOVE_ZERO     1
#define USB_REPORT_RATE_CHANGE 1
#define SLE_REPORT_RATE_CHANGE 1
#define MOUSE_SENSOR_TYPE_SECLECT  1
#define MOUSE_SENSOR_MODE_INIT   1
#define MOUSE_SENSOR_MODE_UPDATA 1
#define MOUSE_SENSOR_DPI_UPDATA  1
#define MOUSE_INDEPENDENT_IO_BUTTON_EN 1
#define MOUSE_MATRIX_IO_BUTTON_IO 0
#define MOUSE_MODE_CHANGE 1

/*----------------------------RAPOO------------------------------*/
// 命令头标志
#define COMMAND_FALG 0xBA

// 当前收到的PC命令的来源是USB还是无线
#define CONN_TYPE_ENTRY_PCBA 0xA0  // 进入PCBA测试
#define CONN_TYPE_RLINKER 0xA5
#define CONN_TYPE_USB 0xFF

// PC写EEPROM/配置数据命令的最大有效数据长度
#define MAX_PC_CMD_WR_DATA_LEN 24

// PC读EEPROM/配置数据命令的最大有效数据长度
#define MAX_PC_CMD_RD_DATA_LEN 28

// PCBA测试命令
#define COMMAND_PCBA 0xE1
#define COMMAND_PCBA_SUB_START 0x01      // 数据字节1低半字节=频道与RF地址，高半字字=自检项目选择
#define COMMAND_PCBA_SUB_RD_RESULT 0x02  // 读取自检的结果
#define COMMAND_PCBA_SUB_SLEEP 0x03      // 进入睡眠模式由测架去测睡眠电流

#define PCBA_FUC_ENABLE 0

#ifndef MOUSE_FW_REVISION
#define MOUSE_FW_REVISION 0x00111111
#endif

// 字节0，表示是2.4G/蓝牙，字节1表示是否进入2.4/蓝牙的前一次是否为USB
#define FLASH_PROTOCOL_SLE_FLAG 0x000000FF
#define FLASH_PROTOCOL_USB_FLAG 0x0000FF00
#define FLASH_PROTOCOL_BLE_FLAG 0x00000000

// 字母
#define kA 0x04  // A
#define kB 0x05  // B
#define kC 0x06  // C
#define kD 0x07  // D
#define kE 0x08  // E
#define kF 0x09  // F
#define kG 0x0A  // G
#define kH 0x0B  // H
#define kI 0x0C  // I
#define kJ 0x0D  // J
#define kK 0x0E  // K
#define kL 0x0F  // L
#define kM 0x10  // M
#define kN 0x11  // N
#define kO 0x12  // O
#define kP 0x13  // P
#define kQ 0x14  // Q
#define kR 0x15  // R
#define kS 0x16  // S
#define kT 0x17  // T
#define kU 0x18  // U
#define kV 0x19  // V
#define kW 0x1A  // W
#define kX 0x1B  // X
#define kY 0x1C  // Y
#define kZ 0x1D  // Z

// F1 -- F12
#define kF1 0x3A   // F1
#define kF2 0x3B   // F2
#define kF3 0x3C   // F3
#define kF4 0x3D   // F4
#define kF5 0x3E   // F5
#define kF6 0x3F   // F6
#define kF7 0x40   // F7
#define kF8 0x41   // F8
#define kF9 0x42   // F9
#define kF10 0x43  // F10
#define kF11 0x44  // F11
#define kF12 0x45  // F12

// 上数字
#define k0 0x27   // ) 0
#define k1 0x1E   // (! 1)
#define k2 0x1F   //@ 2
#define k3 0x20   //# 3
#define k4 0x21   //$ 4
#define k5 0x22   //%5
#define k6 0x23   //^6
#define k7 0x24   //& 7
#define k8 0x25   //* 8
#define k9 0x26   // ( 9
#define kMi 0x2D  //—— _
#define kEq 0x2E  //+=

// num pad
#define kNLock 0x53   // Num lock
#define kNMulti 0x55  //*
#define kNDivi 0x54   // /
#define kNPlus 0x57   //+
#define kNMinus 0x56  //-
#define kN9 0x61      // 9
#define kN8 0x60      // 8
#define kN7 0x5F      // 7
#define kN6 0x5E      // 6
#define kN5 0x5D      // 5
#define kN4 0x5C      // 4
#define kN3 0x5B      // 3
#define kN2 0x5A      // 2
#define kN1 0x59      // 1
#define kN0 0x62      // 0
#define kNP 0x63      //.
#define kNEnter 0x58  //（小Enter）

// 控制
#define kHome 0x4A   // Home
#define kIns 0x49    // insert
#define kDel 0x4C    // delete
#define kEnd 0x4D    // end
#define kPaUp 0x4B   // page up
#define kPaDo 0x4E   // page down
#define kEsc 0x29    // ESC
#define kCaps 0x39   // Caps lock
#define kTab 0x2B    // tab
#define kScrol 0x47  // Scroll lock
#define kPrcsc 0x46  // prcsc sysrq
#define kPause 0x48  // Pause break
#define kBack 0x2A   // Back space
#define kEnter 0x28  // Enter
#define kSpace 0x2C  // space

#define kLCtrl 0xe0   // 左ctrl
#define kLShift 0xe1  // 左Shift
#define kLAlt 0xe2    // 左Alt
#define kLWin 0xe3    // 左windows
#define kRCtrl 0xe4   // 右Ctrl
#define kRShift 0xe5  // 右Shift
#define kRAlt 0xe6    // 右Alt
#define kRWin 0xe7    // 右windows

// 控制键位掩码定义
#define KEY_LCtrl 0x01   // 左ctrl
#define KEY_LShift 0x02  // 左Shift
#define KEY_LAlt 0x04    // 左Alt
#define KEY_LWin 0x08    // 左windows
#define KEY_RCtrl 0x10   // 右Ctrl
#define KEY_RShift 0x20  // 右Shift
#define KEY_RAlt 0x40    // 右Alt
#define KEY_RWin 0x80    // 右windows

#define kApp 0x65     // Application
#define kVBar 0x31    //|"\"
#define kDQuote 0x34  //" '
#define kColon 0x33   //: ;
#define kThrow 0x35   //~ `
#define kLTS 0x36     //<,
#define kGTS 0x37     //>.
#define kQM 0x38      // (?/)
#define kBR 0x30      // }]
#define kBL 0x2F      // {[
#define kUp 0x52      // Up arrow
#define kLeft 0x50    // Left arrow
#define kRight 0x4F   // Right arrow
#define kDown 0x51    // Down arrow
#define kK14 0x89     // international key
#define kKR_L 0x91    //
#define kK107 0x85    //
#define kK45 0x64     //
#define kK42 0x32     //
#define kK131 0x8b    //
#define kK132 0x8a    //
#define kK133 0x88    //
#define kK56 0x87     //
#define kKR_R 0x90    //
#define kr2 0xe8
#define kr3 0xe9
#define kr4 0xea

/*----------------------------RAPOO------------------------------*/

// REPORT ID定义
typedef enum {
    INTERFACE_INDEX_MOUSE = 0,  // 鼠标的接口定义

    INTERFACE_INDEX_KB = 1,  // 键盘的接口定义

    INTERFACE_INDEX_OTA = 2,  // USB OTA 升级的接口定义
} USB_Interface_Index_t;

// PC发送过来的命令
typedef enum {
    PC_CMD_SET_MODE = 0xA1,  // 进入指定模式

    PC_CMD_GET_MODE = 0xA2,  // 获取当前模式

    PC_CMD_GET_FW_VER = 0xA3,  // 获取 firmware 版本号及型号

    PC_CMD_READ_EEPROM = 0xA4,  // 读 eeprom

    PC_CMD_WRITER_EEPROM = 0xA5,  // 写 eeprom

    PC_CMD_RELOAD_CONFIG = 0xA6,  // 驱动加载指定的配置数据到指定的模式的配置

    PC_CMD_RESET_DEVICE = 0xA7,  // 复位设备

    PC_CMD_FW_UPDATE = 0xA8,  // 进入固件更新，跑bootloader

    PC_CMD_GET_CPI = 0xA9,  // 读取当前的CPI

    PC_CMD_READ_BAT_VOL = 0xAA,  // 读取当前电量

    PC_CMD_SET_POWER_MODE = 0xAB,  // 设置电源模式

    PC_CMD_UPDATE_KEYXYMOVE = 0xAC,  // 更新OLED显示屏上按键统计及移动距离

    PC_CMD_INIT_FLASH_DEF_PAR = 0xAD,  // 初始化为flash出厂默认参数

    PC_CMD_ADD_CPI = 0xAE,  // set cpi+

    PC_CMD_SUB_CPI = 0xAF,  // set cpi-

    PC_CMD_READ_SENSOR_MODE = 0xB1,  // read sensor mode

    PC_CMD_WRITE_SENSOR_MODE = 0xB2,  // wtite sensor mode

    PC_CMD_SET_CPI = 0xB3,  //@TEST
} PC_CMD_t;

// 回复给PC的结果ACK
typedef enum {
    ACK_PC_FAIL = 0x00,  // 失败

    ACK_PC_SUCCESS = 0x01,  // 成功

    ACK_PC_BUSYING = 0x02,  // 正忙
} ACK_PC_t;

typedef enum {
    USB_STATUS_POWER_REMOVED = 0,

    USB_STATUS_POWER_DETECTED = 1,

    USB_STATUS_POWER_READY = 2,

    USB_STATUS_RESET = 3,

    USB_STATUS_STARTED = 4,

    USB_STATUS_STOPPED = 5,

    USB_STATUS_SUSPEND = 6,

    USB_STATUS_RESUME = 7,
} USB_STATUS_t;

typedef enum {
    APP_USBD_HID_SET_BOOT_PROTO,   /**< Event SET_BOOT_PROTOCOL.       */
    APP_USBD_HID_SET_REPORT_PROTO, /**< Event SET_REPORT_PROTOCOL.     */
} app_usbd_protocal_t;

// 当前使用的连接协议
typedef enum {
    DEEP_ANA_REG1_SW_NO = 0x00,

    DEEP_ANA_REG1_SW_USB = 0x08,  // 切到USB

    DEEP_ANA_REG1_SW_RLINKER = 0x10,  // 切到无线

    DEEP_ANA_REG1_SW_USB_TO_BLE = 0x18,  // USB切到蓝牙

    DEEP_ANA_REG1_SW_RLINKER_TO_BLE = 0x20,  // USB切到蓝牙

    DEEP_ANA_REG1_SW_NO_POWER_ON = 0x28,  // 非上电标志

    DEEP_ANA_REG1_SW_MASK = 0x38,  // bit3~5: 用于切换用途

    DEEP_ANA_REG1_SW_DPI_MASK = 0x07,  // bit0~2:表示蓝牙模式的DPI档位，进入USB或2.4G模式
    // 时需将它与办公模式的DPI档位比较，不现的话需更新办公模式DPI档位
} nrf_gpregret2_switch_t;

// 当前使用的连接协议
typedef enum {
    PROTOCOL_MODE_USB = 0,  // USB连接方式

    PROTOCOL_MODE_SLE = 1,  // SLE连接方式

    PROTOCOL_MODE_BLE = 2,  // BT5.2连接方式
} nrf_protocol_mode_t;

// 当前工作模式
typedef enum {
    MOUSE_MODE_POWER_OFF = 0,  // 休眠模式

    MOUSE_MODE_NORMAL = 1,  // 正常工作模式

    MOUSE_MODE_PAIR = 2,  // 2.4G配对模式

    MOUSE_MODE_AUTO_PAKET = 3,  // 自动发包测试模式

    MOUSE_MODE_FAST_CONN = 4,  // 2.4G快连模式

    MOUSE_MODE_FCC = 5,  // FCC测试模式

    MOUSE_MODE_PCBA = 6,  // PCBA生产测试模式
} nrf_mode_t;

// 最大的包缓冲区数量
#define PACKET_BUFFER_SIZE 16

// 鼠标的报告ID
#define REPORT_ID_MOUSE 1

// REPORT ID定义
typedef enum {
    REPORT_ID_KB_STAND = 1,  // 标准健

    REPORT_ID_KB_POWER = 2,  // POWER键

    REPORT_ID_KB_CUSUMER = 3,  // 多媒体键

    REPORT_ID_KB_MCE = 4,  // MCE键

    REPORT_ID_OTA = 5,  // OTA升级的报告ID

    REPORT_ID_REPORT = 0xBA,  // SET REPORT/GET REPORT的报告ID

    REPORT_ID_KB_USER = 0xBB,  // 用户MODE + CPI + 电量
} KB_REPORT_ID_t;

// 上传给USB的标准键盘包格式
typedef struct {
    uint8_t ReportID;
    uint8_t Modifier_keys;
    uint8_t Save0;
    uint8_t Keys[6];
} m_keyboard_data_stand_t;

// 上传给USB的Power键盘包格式
typedef struct {
    uint8_t ReportID;
    uint8_t PowerKey;
} m_keyboard_data_power_t;

// 上传给USB的多媒体键盘包格式
typedef struct {
    uint8_t ReportID;
    uint8_t CusumerKey[2];
} m_keyboard_data_cusumer_t;

// 上传给USB的MCE键盘包格式
typedef struct {
    uint8_t ReportID;
    uint8_t MceKey;
} m_keyboard_data_mce_t;

typedef struct {
    uint8_t ReportID;  // 0xbb
    uint8_t cmd;       // 0xb0
    uint8_t mode;
    uint8_t cpi[2];
    uint8_t bat_valid;
    uint8_t bat_percent;
} m_keyboard_data_user_t;

// 上传给USB的鼠标包格式
typedef struct {
    uint8_t ReportID;
    uint8_t Key;
    int16_t x;
    int16_t y;
    int8_t Wheel;
    int8_t Title;
} m_mouse_data_t;

// 上传给USB的键盘包格式
typedef struct {
    // 当前按键
    uint8_t Upload_flag;  // 有键需要上传标志，键的类型由‘ReportID'决定

    union {
        uint8_t buffer[9];
        m_keyboard_data_stand_t Stand;
        m_keyboard_data_power_t Power;
        m_keyboard_data_cusumer_t Cusumer;
        m_keyboard_data_mce_t mce;
        m_keyboard_data_user_t user;
    } kb;
} m_upload_kb_data_t;

// 缓冲区中包的类型
typedef enum {
    BUFFER_PAKET_TYPE_MOUSE = 1,

    BUFFER_PAKET_TYPE_KB_STAND,

    BUFFER_PAKET_TYPE_POWER,

    BUFFER_PAKET_TYPE_CUSUMER,

    BUFFER_PAKET_TYPE_MCE,

    BUFFER_PAKET_TYPE_USER,
} BUFFER_PAKET_TYPE_t;

// 上传USB或2.4G鼠标与键盘包联合体
typedef struct {
    uint8_t type;

    union {
        m_keyboard_data_stand_t Stand;
        m_keyboard_data_power_t Power;
        m_keyboard_data_cusumer_t Cusumer;
        m_keyboard_data_mce_t mce;
        m_keyboard_data_user_t user;
        m_mouse_data_t Mouse;
    } paket;
} m_packet_t;

// 缓冲区数据结构
// 所要需要发送的鼠标与键盘数据先填到此缓冲区中，
// 然后根据是USB还是2.4G分别进行提取发送
typedef struct {
    m_packet_t buffer[PACKET_BUFFER_SIZE];
    uint8_t start_idx;
    uint8_t end_idx;
} packet_buffer_t;

// PC发给鼠标的命令结构
typedef union {
    uint8_t buffer8[36];
    struct {
        uint8_t CommandFlag;
        uint8_t ConnType;
        uint8_t Command;
        uint8_t DataLenth;

        union {
            uint8_t Addr8[4];
            uint32_t Addr32;
        } ADDR;

        union {
            uint8_t Data8[24];
            uint32_t Data32[6];
        } DATA;
    } BUFFER;
} PC_CMD_UNION;

typedef struct {
    uint8_t usb_status;

    uint8_t motion_entry_status;
} m_AppUsb_t;

typedef struct {
    uint8_t ReadyFlag;  // ACK包就绪标志

    uint8_t PaketLen;  // 当前有有效的配置包未处理

    uint8_t PCData[32];  // 当前配置包的数据  [40]
} PC_Data_t;

// 电池ADC状态
typedef enum {
    ADC_STABLE = 0x00,  // 稳定态

    ADC_BATTERY_CHARGE = 0x01,  // 充电中

    ADC_BATTERY_FULL = 0x02,  // 充满电

    ADC_UNSTABLE = 0x03,  // 不稳定
} ADC_STATE;

typedef enum {
    SWITCH_TASK_NO = 0,

    SWITCH_TASK_WORKMODE = 1,  // 切换工作模式

    SWITCH_TASK_PROTOCAL = 2,  // 切换连接方式

    SWITCH_TASK_RESET_DEIVCE = 3,  // 工作模式改变延时复位后重新连接

    SWITCH_TASK_ENTRY_FW_UPDATA = 4,  // 进入固件升级模式

    SWITCH_TASK_RESUME_DEFAULT = 5,  // 恢复出厂默认参数
} SWITCH_TASK_t;

// 配对地址结构
typedef struct {
    union {
        uint32_t pair_addr;
        uint8_t pair_addr8[4];
    } addr;
} RF_PAIR_INFO_STRUCT;

// 鼠标对码与固件信息及基本参数储存位置表:
// (注:所有数据类型存储采用小端格式)
typedef struct {
    volatile uint32_t usbVID;               // VID
    volatile uint32_t usbPID;               // PID
    volatile uint32_t pair_flag1;           // 地址1对码标志1,0x5A,表示已对码
    volatile uint32_t flash_init_flag;      // Flash 初始化标志 = 0xa059c2de 表示已初始化
    volatile uint32_t firmware_version;     // 固件版本
    volatile uint32_t usb_mouse_workmode;   // 鼠标模式 0x00:模式1;0x01: 模式2;0x02: 模式3;0x03: 模式3;
    volatile uint32_t sle_mouse_workmode;   // 鼠标模式 0x00:模式1;0x01: 模式2;0x02: 模式3;0x03: 模式3;
    volatile uint32_t ble_mouse_workmode;   // 鼠标模式 0x00:模式1;0x01: 模式2;0x02: 模式3;0x03: 模式3;
    volatile uint32_t mouse_move_distance;  // 鼠标移动距离地址
    volatile uint32_t mouse_key_count;      // 鼠标按键次数统记.

    volatile RF_PAIR_INFO_STRUCT pair_addr1;  // 对码地址1
    volatile uint32_t pair_flag2;                  // 地址2对码标志1,0x5A,表示已对码
    volatile RF_PAIR_INFO_STRUCT pair_addr2;  // 对码地址2
    volatile uint32_t pair_flag3;                  // 地址3对码标志1,0x5A,表示已对码
    volatile RF_PAIR_INFO_STRUCT pair_addr3;  // 对码地址3

    volatile uint32_t pair_default_addr;     // 对码模式默认地址,进入对码模式时采用的 RF 通信地址=0xFDA93652
    volatile uint32_t pair_default_channel;  // 对码模式频道,地址 0x38=进入对码模式时采用的频道 = 80(2480MHz)

    volatile uint32_t save2;
    volatile uint32_t save3;
    volatile uint32_t save4;
    volatile uint32_t BLEOr24GFlag;  // 2.4G(0xxxxxxxFF)或BLE(0xxxxxxx00)标志
    volatile uint32_t save5;
    volatile uint32_t save6;
    volatile uint32_t save7;
    volatile uint32_t save8;
    volatile uint32_t save9;

    volatile uint8_t usb_string[32];  // 自定义 USB 显示名称字符 1~32,地址+0x60对应显示的第 1 个字符,默认=0x20202020(‘ ‘)
} MOUSE_PAIR_FW_INFO_FLASH_STRUCT;

//+0x0000 ~ 027f鼠标模式配置信息结构
// 实际使用+0x0000 ~ +0x002B
typedef struct {
    volatile uint32_t LeftKey;      // 原本左键对应定义的功能
    volatile uint32_t MiddKey;      // 原本中键对应定义的功能
    volatile uint32_t RightKey;     // 原本右键对应定义的功能
    volatile uint32_t CPI_ADD_Key;  // 原本CPI+键对应定义的功能
    volatile uint32_t CPI_DEC_Key;  // 原本CPI-键对应定义的功能
    volatile uint32_t LF_Key;       // 原本左前进键对应定义的功能
    volatile uint32_t LB_Key;       // 原本左后进键对应定义的功能
    volatile uint32_t RF_Key;       // 原本右前进键对应定义的功能
    volatile uint32_t RB_Key;       // 原本右后进键对应定义的功能
    volatile uint32_t WHEEL_F_Key;  // 原本滚轮前进键对应定义的功能
    volatile uint32_t WHEEL_B_Key;  // 原本滚轮后退键对应定义的功能
} MOUSE_KEY_DEFINE_STRUCT;

//+0x0280 ~ 03ff鼠标模式配置信息结构
// 实际使用+0x0280 ~ +0x02BF
typedef union {
    struct {
        volatile uint32_t usb_rate;  // USB 回报率 0x08=125Hz 0x04=250Hz 0x02=500Hz 0x01=1000Hz, 默认=0x01
        union {
            volatile uint32_t sensor32;  // 传感器相关参数设置
            struct {
                volatile uint8_t lift_thr;  // 静默高度 可填值1 2 或 3
                volatile uint8_t Mode24G;   // 传感器工作模式选择,
                                   // 0:有线模式；1：高性能模式；2：低功耗模式；3：办公模式，
                // 其它：默认,有线时=有线模式，2.4G时=低功耗模式,蓝牙时=办公模式
                volatile uint8_t ModeUSB;  // 传感器工作模式选择,
                                  // 0:有线模式；1：高性能模式；2：低功耗模式；3：办公模式，
                // 其它：默认,有线时=有线模式，2.4G时=低功耗模式,蓝牙时=办公模式
                volatile uint8_t ModeBluetooth;  // 0:有线模式；1：高性能模式；2：低功耗模式；3：办公模式，
                    // 其它：默认,有线时=有线模式，2.4G时=低功耗模式,蓝牙时=办公模式
            } sensor8;
        } MOUSE_SENSOR;

        union {
            volatile uint32_t cpi_value[4];  // 每个模式的dpi，分7档，需要4个地址存储
            struct {
                volatile uint16_t cpi_x_value16[7];  // cpi_x_value16[7] 为 CPI_X 1 ~ 7档对应的CPI值

                volatile uint8_t cpi_total8;  // CPI 可设总档位字节(0~6)
                volatile uint8_t reserve1;
            } cpi_val_x;
        } CPI_X;

        union {
            volatile uint32_t cpi_led32;
            struct {
                volatile uint8_t cpi_Level8;   // CPI 当前选择的档位(0~6), CPI 可切换数量 1 到 7,默认=3
                volatile uint8_t led_mode8;    // CPI 当前选择的档位(0~6), CPI 可切换数量 1 到 7,默认=2，LED设置
                volatile uint8_t BreathFre8;   // LED 背光呼吸灯频率（0~4）,总共支持 5 档可选
                volatile uint8_t brightness8;  // LED 灯光亮度等级（0~9）,总共支持 10 档可选
            } mix_pra;
        } cpi_led;

        union {
            volatile uint32_t oled_MacroGun;  // OLED及压枪宏选择及已定义的压枪宏总数
            struct {
                volatile uint8_t Oled_brightness;  // OLED 背光亮度(5~100),支持 5%~100%亮度可调，每步 1%
                volatile uint8_t CurrentGunSel;    // 压枪宏识别码 0~3
                volatile uint8_t gun_total;        // 已定义压枪宏总数 0~3（0，表示没有定义）
                volatile uint8_t reserve3;
            } Oled_gun;
        } oled_MacroGun;

        volatile uint32_t OLED_PIC_Total;  // OLED 图片显示总数（0~7），最多 8 张
                                  // 若显示张数为 0，则只显示图片 1 索引地址上的图片，
        // 若显示张数 1，则会交递显示图片 1 索引地址与图片 2 索引地址上的图片，以此类退
        union {
            volatile uint32_t Oled_Pic_index[2];
            volatile uint8_t Oled_Pic_index8[8];  // 图片 1~8 索引地址,对应存在flash的图片存储区的0~15个地址中的一个，每个占用4K
        } PIC_LOC_ARRARY;

        union {
            volatile uint32_t As_Pic_index;  // 开机图片索引及当前显示的图片索引
            struct {
                volatile uint8_t PowerOn_Pic_index;  // 开机图片索引地址
                volatile uint8_t Current_Pic_index;  // 当前显示图片索引地址
                volatile uint8_t Oled_OffOn;         // 屏幕开关 0：开；1：关
                volatile uint8_t AutoPowerSave;      // 开动有动作时自动省电开关: 0: 关; 其它: 开
            } Pic_index;
        } PIC_INDEX;

        volatile uint32_t Oled_Dis_Type;  // 图片显示方式 0 正常, 1 右移，2 左移，3 右下移，4 左上移

        volatile uint32_t OledIntervalTime;  // 图片切换间隔时间 默认3, 单位为s

        union {
            volatile uint32_t RGB_value32;  // LED0 标志，0xff 表示此LED自定义过颜色值，是默认值
            struct {
                volatile uint8_t Red;    // LED0 红色颜色分量值 0~255;
                volatile uint8_t Green;  // LED0 绿色颜色分量值 0~255;
                volatile uint8_t Blue;   // LED0 蓝色颜色分量值 0~255;
                volatile uint8_t Flag;   // LED0 标志，0xff 表示此LED自定义过颜色值，是默认值
            } FlagAndRGB;
        } LED_RGB0;

        union {
            volatile uint32_t RGB_value32;  // LED1 标志，0xff 表示此LED自定义过颜色值，是默认值
            struct {
                volatile uint8_t Red;    // LED1 红色颜色分量值 0~255;
                volatile uint8_t Green;  // LED1 绿色颜色分量值 0~255;
                volatile uint8_t Blue;   // LED1 蓝色颜色分量值 0~255;
                volatile uint8_t Flag;   // LED1 标志，0xff 表示此LED自定义过颜色值，是默认值
            } FlagAndRGB;
        } LED_RGB1;

        union {
            volatile uint32_t RGB_value32;  // LED2 标志，0xff 表示此LED自定义过颜色值，是默认值
            struct {
                volatile uint8_t Red;    // LED2 红色颜色分量值 0~255;
                volatile uint8_t Green;  // LED2 绿色颜色分量值 0~255;
                volatile uint8_t Blue;   // LED2 蓝色颜色分量值 0~255;
                volatile uint8_t Flag;   // LED2 标志，0xff 表示此LED自定义过颜色值，是默认值
            } FlagAndRGB;
        } LED_RGB2;

        union {
            struct {
                volatile uint16_t cpi_y_value16[7];  // cpi_y_value16[7] 为 CPI_Y 1 ~ 7档对应的CPI值

                volatile uint8_t reserve1;  // 保留1
                volatile uint8_t reserve2;  // 保留2
            } cpi_val_y;
        } CPI_Y;
    } Config;

    volatile uint32_t ConfigArrary[16];
} MOUSE_MODE_CONFIG;

// 公共参数定义
typedef struct {
    MOUSE_PAIR_FW_INFO_FLASH_STRUCT UsePairFWInfo;  // 当前正在使用的鼠标配对与固件信息

    MOUSE_KEY_DEFINE_STRUCT UseKeyDefine;  // 当前正在使用的当前模式的按键定义

    MOUSE_MODE_CONFIG UseModeConfig;  // 当前正在使用的模式参数

    m_upload_kb_data_t upload_kb_data;  // 按键数据

    uint32_t USB_Switch_Counter;

    nrf_mode_t Mode;  // 当前鼠标的工作模式

    nrf_protocol_mode_t protocal_mode;  // 当前使用的通信方式

    uint8_t wait_usb_ready_flag;  // 等待USB就绪后再切到USB模式

    uint8_t RxPcPaketFlag;

    PC_CMD_UNION RxPCPaket;  // 接收到待处理的PC数据包

    PC_Data_t AckPCPaket;  // 收到PC命令后待发送回去的ACK确认包

    uint8_t RelaseStep;

    uint32_t RelaseDelay;

    uint16_t AdcTimesCnt;

    uint16_t BatVolmv;  // 电池当前电压值(mv)

    uint16_t BatPercent;  // 电池当前百分比%

    uint8_t LoBatFlag;  // 低电标志

    uint8_t SoftCutOffFlag;  // 软关机标志

    uint8_t PowerOnCnt;  // 上电N次后再更新电量显示

    uint32_t LoVatFlashActTimer;  // 有动作时闪低电计时

    uint8_t temp_workmode;  // 鼠标模式 0x00:模式1;0x01: 模式2;0x02: 模式3;

    nrf_protocol_mode_t temp_protocal;  // 暂存要切换的协议

    SWITCH_TASK_t switch_task;

    uint32_t BatFullTimer;

    uint32_t EntryIdleTimer;  // 在无线模式下,超过30s无动作后OLED与LED灯降低亮度

    uint32_t EntrySleepTimer;  // 无任何动作进入深片倒计时

#if ACTIVE_OFF_OLDE_LED_ENABLE
    uint32_t NoActiveTimeCounter;  // 没有动作计时
    uint32_t PauseCloseTimer;      // 暂时不关屏定时器，在设备LED及OLED时
#endif

    uint8_t worck_mode_change_flag;  // 工作模式改变标志

    uint8_t WriteFlashModeFlag;  // 更新当前模式的flash数据标志

    uint32_t IdleWriteFlashCounter;  // 空闲一定时间后才去写flash计时

#if OLED_ENABLE
    uint8_t mouse_pic_select_flag;  // 图片选择状态标志

    uint8_t mouse_OLed_scantimes;

    uint8_t mouse_OLed_scrollflag;

    uint32_t UpdataPIC_LogicalStartAddr;

    uint16_t UpdataPIC_TotolLen;
#endif

    uint32_t UpdataMacro_LogicalStartAddr;

    uint16_t UpdataMacro_TotolLen;

    uint32_t DelayUpdataTimer;

    uint32_t DelyFlashRLinkerModeTimer;

    uint16_t work_mode_change_dis_timer;

    uint8_t mouse_old_mode;

    uint8_t mouse_old_current_pic;

    uint8_t mouse_old_pic_total;

    uint32_t mouse_key_icnt;

    uint32_t mouse_cursor_dis;

    uint32_t mouse_key_amp;

#if (APP_FLASH_DEBUG)
    uint8_t local_mac_addr[6];

    uint8_t usb_rate;
    uint8_t usb_report_rate_level;
    uint8_t sle_rate;
    uint8_t sle_report_rate_level;
    uint8_t dpi_level;
#endif

    uint32_t mouse_Oled_bri;

    uint16_t mNewKeyApm;  // 当前按键APM，根据当前3次且大于2s的按键次，换算成1分钟的按键数量,APM——每分钟操作的次数，又称“手速”。

    uint16_t mOldKeyApm;  // 上一次按键APM

    uint8_t PauseSensorFlag;

    uint8_t UpdataDpiFlag;

    uint8_t UpdataZuJiDpiFlag;

    uint8_t AutoTxPaketFlag;  // 自动发包标志

    uint8_t timebase50msFlag;

#if OLED_ENABLE
    uint8_t OffTempDisFlag;  // 关屏临时显示标志
#endif

#if ACTIVE_OFF_OLDE_LED_ENABLE
    uint8_t ActiveDisOnOffFlag;
    uint8_t ActiveLEDOnOffFlag;
#endif
} m_public_var_t;

extern m_public_var_t m_PublicVar;

// 更新数据类型
typedef enum {
    CLEAR_FLAG = 0X00,  // 清除标志
    DEV_INFO_UPDATA = 0X01,  // 设备信息
    SLE_PAIR_UPDATA = 0X02,  // SLE配对数据
    BLE_PAIR_UPDATA = 0X04,  // BLE配对数据
    REO_BOOT = 0X08,  // BLE配对数据
} flash_updata_t;

typedef struct led_event {
    uint8_t led_event_effect;
    uint8_t led_event_flag;
} led_event;

#define EVENT_NUM 3
typedef struct device_flag {
    uint8_t WriteFlashModeFlag;
    uint8_t dpi_event_flag;
    led_event event_flag[EVENT_NUM];
} device_flag;

// LED事件类型
typedef enum {
    CLEAR_LED_EVENT_FLAG = 0X00,  // 清除灯事件
    PAIR_EVENT = 0X01,  // 配对
    RECONNECT_EVENT = 0X02,  // 回连
    CONNECT_OK = 0X04,  // 连接成功
    POWER_ON = 0X08,  // 上电灯
    DPI_CHANGE = 0X10  // DPI切换
} led_event_t;

// LED灯效
typedef enum {
    CLEAR_LED_FLAG = 0X00,  // 清除标志
    SLOW_LED_FLAG = 0X01,  // 慢闪
    QUICK_LED_FLAG = 0X02,  // 快闪
    DOUBLE_LED_FLAG = 0X04,  // 双闪
    HOLD_LED_FLAG = 0X08,  // 常亮灯
    PWM_LED_FLAG = 0X10,  // PWM
} led_effect_t;

// 上电后初始化处理
void app_public_init(void);
void app_public_reset_device(void);
void app_public_pc_command_pro(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
