/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: RCU Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-21, Create file. \n
 */
#include "securec.h"
#include "soc_osal.h"
#include "common_def.h"
#include "app_init.h"
#include "watchdog.h"
#include "keyscan.h"
#include "adc.h"
#include "pdm.h"
#include "gpio.h"
#include "pinctrl.h"
#include "pm_clock.h"
#include "hal_adc.h"
#include "hal_dma.h"
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
#include "sle_vdt_codec.h"
#include "sle_vdt_pdm.h"
#include "sle_errcode.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_rcu_server.h"
#include "sle_rcu_server_adv.h"
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
#include "ble_rcu_server.h"
#include "ble_rcu_server_adv.h"
#include "ble_hid_rcu_server.h"
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
#include "bts_le_gap.h"
#if defined(CONFIG_SAMPLE_SUPPORT_IR)
#include "ir_nec.h"
#endif
#if defined(CONFIG_PM_SYS_SUPPORT)
#include "ulp_gpio.h"
#include "gpio.h"
#include "pm_sys.h"
#endif
#ifdef RAM_PARTIAL_RETENTION
#include "pm_sleep_porting.h"
#endif

#define SLE_RCU_SERVER_TASK_PRIO           24
#define USB_HID_MAX_KEY_LENTH              6
#define KEYSCAN_REPORT_MAX_KEY_NUMS        6
#define CONVERT_DEC_TO_HEX                 16
#define MAX_NUM_OF_DEC                     10
#define LENGTH_OF_KEY_VALUE_STR            5
#define SLE_RCU_PARAM_ARGC_2               2
#define SLE_RCU_PARAM_ARGC_3               3
#define SLE_RCU_PARAM_ARGC_4               4

#define SLE_RCU_STATE_DISCONNECT           0
#define SLE_RCU_STATE_CONNECTED            1

#define RCU_TASK_STACK_SIZE                0xc00
#define RCU_TASK_DURATION_MS               200
#define SLE_RCU_WAIT_SSAPS_READY           500
#define SLE_RCU_SERVER_DELAY_COUNT         3
#define SLE_ADV_HANDLE_DEFAULT             1
#define SLE_RCU_SERVER_MSG_QUEUE_MAX_SIZE  32
#define SLE_RCU_SERVER_LOG                 "[sle rcu server]"
#define USB_RCU_TASK_DELAY_MS              10

#define RCU_KEY_MUTE                       0x4
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

#define SLE_VDT_SERVER_LOG                 "[sle vdt server]"
#define ADC_GADC_CHANNEL7                  7
#define ADC_GADC_CHANNEL6                  6
#define SLE_VDT_MIC_OFFSET_16              16
#define SLE_VDT_MIC_OFFSET_24              24
#define SLE_VDT_ENCODE_SIZE                136
#define RING_BUFFER_NUMBER                 4
#define PDM_DMA_TRANSFER_EVENT             1
#define RCU_TARGET_ADDR_NUM                2
#define RCU_CONSUMER_KEY_NUM               6
#define RCU_CONSUMER_KEY_OFFSET            8

#define DURATION_MS_OF_WORK_TO_STANDBY     20000
#define DURATION_MS_OF_STANDBY_TO_SLEEP    20000

static bool g_switch_mouse_and_keyboard = false;
static bool g_switch_consumer_and_ir = true;
static bool g_check_consumer_send = false;
static bool g_check_mouse_send = false;
static bool g_check_keyboard_send = false;

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER) && !defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static bool g_switch_sle_and_ble = false;
#else
static bool g_switch_sle_and_ble = true;
static uint8_t g_sle_pdm_buffer[CONFIG_USB_UAC_MAX_RECORD_SIZE] = { 0 };
static uint32_t g_pdm_dma_data0[CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA] = { 0 };
static uint32_t g_pdm_dma_data1[CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA] = { 0 };
static uint32_t g_pdm_dma_data2[CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA] = { 0 };
static uint32_t g_pdm_dma_data3[CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA] = { 0 };
static uint32_t *g_pdm_dma_data[RING_BUFFER_NUMBER] = {
    g_pdm_dma_data0, g_pdm_dma_data1, g_pdm_dma_data2, g_pdm_dma_data3};
static uint8_t g_write_buffer_state = 0;
static uint8_t g_read_buffer_state = 0;
static osal_event g_dma_event_id;
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

static uint32_t g_keyboard_send_count = 0;
static uint16_t g_conn_id = 0;

static uint32_t keystate = 0;
static uint32_t pressdown = 1;
static uint32_t pressup = 0;

#if defined(CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE)
static const uint8_t g_key_map[CONFIG_KEYSCAN_ENABLE_ROW][CONFIG_KEYSCAN_ENABLE_COL] = {
    { 0x29, 0x2B, 0x14, 0x35, 0x04, 0x1E, 0x1D, 0x00 },
    { 0x3D, 0x3C, 0x08, 0x3B, 0x07, 0x20, 0x06, 0x00 },
    { 0x00, 0x39, 0x1A, 0x3A, 0x16, 0x1F, 0x1B, 0x00 },
    { 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xE4, 0x00 },
    { 0x0A, 0x17, 0x15, 0x22, 0x09, 0x21, 0x19, 0x05 },
    { 0x0B, 0x1C, 0x18, 0x23, 0x0D, 0x24, 0x10, 0x11 },
    { 0x3F, 0x30, 0x0C, 0x2E, 0x0E, 0x25, 0x36, 0x00 },
    { 0x00, 0x00, 0x12, 0x40, 0x0F, 0x26, 0x37, 0x00 },
    { 0x34, 0x2F, 0x13, 0x2D, 0x33, 0x27, 0x00, 0x38 },
    { 0x3E, 0x2A, 0x00, 0x41, 0x31, 0x42, 0x28, 0x2C },
    { 0x00, 0x00, 0xE3, 0x00, 0x00, 0x43, 0x00, 0x51 },
    { 0xE2, 0x00, 0x00, 0x00, 0x00, 0x45, 0xE5, 0xE6 },
    { 0x00, 0x53, 0x00, 0x00, 0xE1, 0x44, 0x00, 0x4F },
    { 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4C, 0x50 },
    { 0x5F, 0x5C, 0x61, 0x5E, 0x59, 0x62, 0x55, 0x5B },
    { 0x54, 0x60, 0x56, 0x57, 0x5D, 0x5A, 0x58, 0x63 },
};
#endif /* CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE */

#if defined(CONFIG_KEYSCAN_USE_SIX_KEYS_TYPE)
static const uint8_t g_key_map[CONFIG_KEYSCAN_ENABLE_ROW][CONFIG_KEYSCAN_ENABLE_COL]  = {
    { 0x51, 0x52 },
    { 0x50, 0x29 },
    { 0x4F, 0x28 },
};
#endif /* CONFIG_KEYSCAN_USE_SIX_KEYS_TYPE */

#if defined(CONFIG_KEYSCAN_USER_CONFIG_TYPE)
static const uint8_t g_key_map[CONFIG_KEYSCAN_ENABLE_ROW][CONFIG_KEYSCAN_ENABLE_COL]  = {
    { RCU_KEY_CONNECT_ADV, RCU_KEY_MUTE,                 RCU_KEY_BACK,  RCU_KEY_SWITCH_IR,             RCU_KEY_VOLUP},
    { RCU_KEY_APPLIC,      RCU_KEY_UP,                   RCU_KEY_ENTER, RCU_KEY_DOWN,                  RCU_KEY_HOME},
    { RCU_KEY_LEFT,        RCU_KEY_RIGHT,                RCU_KEY_MIC,   RCU_KEY_DISCONNECT_DEVICE,     RCU_KEY_PAGEDN},
    { RCU_KEY_VOLDOWN,     RCU_KEY_SWITCH_MOUSE_AND_KEY, 0x0,           0x0,                           0x0},
};
#endif /* CONFIG_KEYSCAN_USER_CONFIG_TYPE */

static const uint16_t g_consumer_key_map[RCU_CONSUMER_KEY_NUM]  = {0xE2, 0x223, 0x224, 0x221, 0xE9, 0xEA};
static const uint8_t g_consumer_key_index[RCU_CONSUMER_KEY_NUM]  = {0x4, 0x5, 0x6, 0x7, 0x8, 0x9};

typedef union mouse_key {
    struct {
        uint8_t left_key   : 1;
        uint8_t right_key  : 1;
        uint8_t mid_key    : 1;
        uint8_t reserved   : 5;
    } b;
    uint8_t d8;
} mouse_key_t;

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
typedef struct usb_hid_sle_mouse_report {
    mouse_key_t key;
    int8_t x;                 /* A negative value indicates that the mouse moves left. */
    int8_t y;                 /* A negative value indicates that the mouse moves up. */
    int8_t wheel;             /* A negative value indicates that the wheel roll forward. */
} usb_hid_sle_mouse_report_t;

typedef struct usb_hid_sle_keyboard_report {
    uint8_t special_key;                         /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t reserve;
    uint8_t key[USB_HID_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_sle_keyboard_report_t;

typedef struct usb_hid_sle_consumer_report {
    uint8_t comsumer_key0;
    uint8_t comsumer_key1;
} usb_hid_sle_consumer_report_t;
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
typedef struct usb_hid_ble_mouse_report {
    mouse_key_t key;
    int8_t x;                 /* A negative value indicates that the mouse moves left. */
    int8_t y;                 /* A negative value indicates that the mouse moves up. */
    int8_t wheel;             /* A negative value indicates that the wheel roll forward. */
} usb_hid_ble_mouse_report_t;

typedef struct usb_hid_ble_keyboard_report {
    uint8_t special_key;                         /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t reserve;
    uint8_t key[USB_HID_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_ble_keyboard_report_t;

typedef struct usb_hid_ble_consumer_report {
    uint8_t comsumer_key0;
    uint8_t comsumer_key1;
} usb_hid_ble_consumer_report_t;
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static usb_hid_sle_keyboard_report_t g_hid_sle_keyboard_report;
static usb_hid_sle_mouse_report_t g_hid_sle_mouse_report;
static usb_hid_sle_consumer_report_t g_hid_sle_consumer_report;
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
static usb_hid_ble_keyboard_report_t g_hid_ble_keyboard_report;
static usb_hid_ble_mouse_report_t g_hid_ble_mouse_report;
static usb_hid_ble_consumer_report_t g_hid_ble_consumer_report;
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */

osal_task *g_rcu_task_handle = NULL;

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static uint32_t g_rcu_dma_channel = 0;
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static void sle_usb_vdt_dma_transfer_done_callback(uint8_t intr, uint8_t channel, uintptr_t arg);
static void rcu_amic_init(void);
static void rcu_amic_deinit(void);
void dma_port_release_handshaking_source(dma_channel_t ch);

static void ssaps_server_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
                                          errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    unused(read_cb_para);
    unused(status);
}

static void ssaps_server_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
                                           errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    unused(write_cb_para);
    unused(status);
}

static void sle_rcu_keyboard_send_report(uint8_t key_value, bool send_flag)
{
    if (key_value != 0) {
        g_hid_sle_keyboard_report.key[g_keyboard_send_count++] = key_value;
    }
    if (send_flag) {
        sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_keyboard_report,
                                             sizeof(usb_hid_sle_keyboard_report_t), g_conn_id);
        if (memset_s(&g_hid_sle_keyboard_report, sizeof(g_hid_sle_keyboard_report), 0,
                     sizeof(g_hid_sle_keyboard_report)) != EOK) {
            g_keyboard_send_count = 0;
            return;
        }
        g_keyboard_send_count = 0;
    }
}

static void sle_rcu_mouse_send_report(uint8_t key_value, bool send_flag)
{
    switch (key_value) {
        case RCU_KEY_RIGHT:
            g_hid_sle_mouse_report.x = 0x10;
            break;
        case RCU_KEY_LEFT:
            g_hid_sle_mouse_report.x = 0xF0;
            break;
        case RCU_KEY_DOWN:
            g_hid_sle_mouse_report.y = 0x10;
            break;
        case RCU_KEY_UP:
            g_hid_sle_mouse_report.y = 0xF0;
            break;
        default:
            break;
    }
    if (send_flag) {
        sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_mouse_report,
                                             sizeof(usb_hid_sle_mouse_report_t), g_conn_id);
        if (memset_s(&g_hid_sle_mouse_report, sizeof(g_hid_sle_mouse_report), 0,
                     sizeof(g_hid_sle_mouse_report)) != EOK) {
            return;
        }
    }
}

static void sle_rcu_consumer_send_report(uint8_t key_value)
{
    if (memset_s(&g_hid_sle_consumer_report, sizeof(g_hid_sle_consumer_report), 0,
                 sizeof(g_hid_sle_consumer_report)) != EOK) {
        return;
    }
    for (uint8_t i = 0; i < RCU_CONSUMER_KEY_NUM; i++) {
        if (key_value == g_consumer_key_index[i]) {
            g_hid_sle_consumer_report.comsumer_key0 = g_consumer_key_map[i] & 0xFF;
            g_hid_sle_consumer_report.comsumer_key1 = g_consumer_key_map[i] >> RCU_CONSUMER_KEY_OFFSET;
        }
    }
    sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_consumer_report,
                                         sizeof(usb_hid_sle_consumer_report_t), g_conn_id);
}

static void sle_rcu_system_power_down_send_report(void)
{
    if (memset_s(&g_hid_sle_consumer_report, sizeof(g_hid_sle_consumer_report), 0,
                 sizeof(g_hid_sle_consumer_report)) != EOK) {
        return;
    }
    g_hid_sle_consumer_report.comsumer_key0 = 0x01;
    g_hid_sle_consumer_report.comsumer_key1 = 0x00;
    sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_consumer_report,
                                         sizeof(usb_hid_sle_consumer_report_t), g_conn_id);
}

static void sle_rcu_send_end(void)
{
    if (g_check_consumer_send) {
        if (memset_s(&g_hid_sle_consumer_report, sizeof(g_hid_sle_consumer_report), 0,
                     sizeof(g_hid_sle_consumer_report)) != EOK) {
            return;
        }
        sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_consumer_report,
                                             sizeof(usb_hid_sle_consumer_report_t), g_conn_id);
        g_check_consumer_send = false;
    }
    if (g_check_mouse_send) {
        if (memset_s(&g_hid_sle_mouse_report, sizeof(g_hid_sle_mouse_report), 0,
                     sizeof(g_hid_sle_mouse_report)) != EOK) {
            return;
        }
        sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_mouse_report,
                                             sizeof(usb_hid_sle_mouse_report_t), g_conn_id);
        g_check_mouse_send = false;
    }
    if (g_check_keyboard_send) {
        if (memset_s(&g_hid_sle_keyboard_report, sizeof(g_hid_sle_keyboard_report), 0,
                     sizeof(g_hid_sle_keyboard_report)) != EOK) {
            return;
        }
        sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_keyboard_report,
                                             sizeof(usb_hid_sle_keyboard_report_t), g_conn_id);
        g_check_keyboard_send = false;
    }
}
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
static void ble_rcu_keyboard_send_report(uint8_t key_value, bool send_flag)
{
    if (key_value != 0) {
        g_hid_ble_keyboard_report.key[g_keyboard_send_count++] = key_value;
    }
    if (send_flag) {
        ble_hid_rcu_server_send_keyboard_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_keyboard_report,
                                                              sizeof(usb_hid_ble_keyboard_report_t), g_conn_id);
        if (memset_s(&g_hid_ble_keyboard_report, sizeof(g_hid_ble_keyboard_report), 0,
                     sizeof(g_hid_ble_keyboard_report)) != EOK) {
            g_keyboard_send_count = 0;
            return;
        }
        g_keyboard_send_count = 0;
    }
}

static void ble_rcu_mouse_send_report(uint8_t key_value, bool send_flag)
{
    switch (key_value) {
        case RCU_KEY_RIGHT:
            g_hid_ble_mouse_report.x = 0x10;
            break;
        case RCU_KEY_LEFT:
            g_hid_ble_mouse_report.x = 0xF0;
            break;
        case RCU_KEY_DOWN:
            g_hid_ble_mouse_report.y = 0x10;
            break;
        case RCU_KEY_UP:
            g_hid_ble_mouse_report.y = 0xF0;
            break;
        default:
            break;
    }
    if (send_flag) {
        ble_hid_rcu_server_send_mouse_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_mouse_report,
                                                           sizeof(usb_hid_ble_mouse_report_t), g_conn_id);
        if (memset_s(&g_hid_ble_mouse_report, sizeof(g_hid_ble_mouse_report), 0,
                     sizeof(g_hid_ble_mouse_report)) != EOK) {
            return;
        }
    }
}

static void ble_rcu_consumer_send_report(uint8_t key_value)
{
    if (memset_s(&g_hid_ble_consumer_report, sizeof(g_hid_ble_consumer_report), 0,
                 sizeof(g_hid_ble_consumer_report)) != EOK) {
        return;
    }
    for (uint8_t i = 0; i < RCU_CONSUMER_KEY_NUM; i++) {
        if (key_value == g_consumer_key_index[i]) {
            g_hid_ble_consumer_report.comsumer_key0 = g_consumer_key_map[i] & 0xFF;
            g_hid_ble_consumer_report.comsumer_key1 = g_consumer_key_map[i] >> RCU_CONSUMER_KEY_OFFSET;
        }
    }
    ble_hid_rcu_server_send_consumer_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_consumer_report,
                                                          sizeof(usb_hid_ble_consumer_report_t), g_conn_id);
}

static void ble_rcu_system_power_down_send_report(void)
{
    if (memset_s(&g_hid_ble_consumer_report, sizeof(g_hid_ble_consumer_report), 0,
                 sizeof(g_hid_ble_consumer_report)) != EOK) {
        return;
    }
    g_hid_ble_consumer_report.comsumer_key0 = 0x01;
    g_hid_ble_consumer_report.comsumer_key1 = 0x00;
    ble_hid_rcu_server_send_power_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_consumer_report,
                                                       sizeof(usb_hid_ble_consumer_report_t), g_conn_id);
}

static void ble_rcu_send_end(void)
{
    if (g_check_consumer_send) {
        if (memset_s(&g_hid_ble_consumer_report, sizeof(g_hid_ble_consumer_report), 0,
                     sizeof(g_hid_ble_consumer_report)) != EOK) {
            return;
        }
        ble_hid_rcu_server_send_consumer_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_consumer_report,
                                                              sizeof(usb_hid_ble_consumer_report_t), g_conn_id);
        g_check_consumer_send = false;
    }
    if (g_check_mouse_send) {
        if (memset_s(&g_hid_ble_mouse_report, sizeof(g_hid_ble_mouse_report), 0,
                     sizeof(g_hid_ble_mouse_report)) != EOK) {
            return;
        }
        ble_hid_rcu_server_send_mouse_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_mouse_report,
                                                           sizeof(usb_hid_ble_mouse_report_t), g_conn_id);
        g_check_mouse_send = false;
    }
    if (g_check_keyboard_send) {
        if (memset_s(&g_hid_ble_keyboard_report, sizeof(g_hid_ble_keyboard_report), 0,
                     sizeof(g_hid_ble_keyboard_report)) != EOK) {
            return;
        }
        ble_hid_rcu_server_send_keyboard_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_keyboard_report,
                                                              sizeof(usb_hid_ble_keyboard_report_t), g_conn_id);
        g_check_keyboard_send = false;
    }
}
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */

static void rcu_keyboard_send_report(uint8_t key_value)
{
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        sle_rcu_keyboard_send_report(key_value, false);
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        ble_rcu_keyboard_send_report(key_value, false);
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
    g_check_keyboard_send = true;
}

static void rcu_mouse_and_keyboard_send_report(uint8_t key_value)
{
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        if (g_switch_mouse_and_keyboard) {
            sle_rcu_mouse_send_report(key_value, false);
            g_check_mouse_send = true;
        } else {
            sle_rcu_keyboard_send_report(key_value, false);
            g_check_keyboard_send = true;
        }
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        if (g_switch_mouse_and_keyboard) {
            ble_rcu_mouse_send_report(key_value, false);
            g_check_mouse_send = true;
        } else {
            ble_rcu_keyboard_send_report(key_value, false);
            g_check_keyboard_send = true;
        }
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
}

static void rcu_system_power_down_send_report(void)
{
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        sle_rcu_system_power_down_send_report();
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        ble_rcu_system_power_down_send_report();
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
}

static void rcu_mouse_and_keyboard_send_start(void)
{
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        if (g_check_mouse_send) {
            sle_rcu_mouse_send_report(0, true);
        }
        if (g_check_keyboard_send) {
            sle_rcu_keyboard_send_report(0, true);
        }
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        if (g_check_mouse_send) {
            ble_rcu_mouse_send_report(0, true);
        }
        if (g_check_keyboard_send) {
            ble_rcu_keyboard_send_report(0, true);
        }
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
}

static void rcu_send_end(void)
{
#if defined(CONFIG_PM_SYS_SUPPORT)
    uapi_pm_work_state_reset();
#endif
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        sle_rcu_send_end();
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        ble_rcu_send_end();
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
    if (keystate == pressdown) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        rcu_amic_deinit();
#endif
        keystate = pressup;
    }
}

static void rcu_consumer_send_report(uint8_t key_value)
{
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        sle_rcu_consumer_send_report(key_value);
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        ble_rcu_consumer_send_report(key_value);
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
    g_check_consumer_send = true;
}

#if defined(CONFIG_SAMPLE_SUPPORT_IR)
static void sle_ir_function_switch(uint8_t key)
{
    switch (key) {
        case RCU_KEY_MUTE:
            ir_transmit_nec(IR_NEC_USER_CODE, IR_NEC_KEY_MUTE);
            break;
        case RCU_KEY_HOME:
            ir_transmit_nec(IR_NEC_USER_CODE, IR_NEC_KEY_HOME);
            break;
        case RCU_KEY_BACK:
            ir_transmit_nec(IR_NEC_USER_CODE, IR_NEC_KEY_BACK);
            break;
        case RCU_KEY_VOLUP:
            ir_transmit_nec(IR_NEC_USER_CODE, IR_NEC_KEY_VOLUMEUP);
            break;
        case RCU_KEY_VOLDOWN:
            ir_transmit_nec(IR_NEC_USER_CODE, IR_NEC_KEY_VOLUMEDOWN);
            break;
        default:
            break;
    }
}
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static void rcu_disconnec_remote_device(void)
{
    sle_addr_t addr[RCU_TARGET_ADDR_NUM];
    uint16_t number = 1;
    sle_get_paired_devices(addr, &number);
    if (number > 0) {
        sle_disconnect_remote_device(addr);
    }
}

static void rcu_start_adv(void)
{
    sle_addr_t addr[RCU_TARGET_ADDR_NUM];
    uint16_t number = 1;

    sle_get_bonded_devices(addr, &number);
    if (number > 0) {
        sle_rcu_server_directed_adv_init(addr);
    }
}
#endif

static void rcu_state_switch(uint8_t key_value)
{
    switch (key_value) {
        case RCU_KEY_SWITCH_MOUSE_AND_KEY:
            g_switch_mouse_and_keyboard = !g_switch_mouse_and_keyboard;
            break;
        case RCU_KEY_SWITCH_SLE_AND_BLE:
            g_switch_sle_and_ble = !g_switch_sle_and_ble;
            break;
        case RCU_KEY_SWITCH_IR:
            g_switch_consumer_and_ir = !g_switch_consumer_and_ir;
            break;
        case RCU_KEY_SWITCH_CONN_ID:
            g_conn_id = !g_conn_id;
            break;
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        case RCU_KEY_DISCONNECT_DEVICE:
            rcu_disconnec_remote_device();
            break;
        case RCU_KEY_CONNECT_ADV:
            rcu_start_adv();
            break;
#endif
        default:
            break;
    }
}

static void rcu_consumer_and_ir_send_report(uint8_t key_value)
{
    if (g_switch_consumer_and_ir) {
        rcu_consumer_send_report(key_value);
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_IR)
        sle_ir_function_switch(key_value);
#endif /* CONFIG_SAMPLE_SUPPORT_IR */
    }
    g_check_consumer_send = true;
}

static int rcu_keyscan_callback(int key_nums, uint8_t key_values[])
{
    rcu_send_end();
    for (uint8_t i = 0; i < key_nums; i++) {
        switch (key_values[i]) {
            case RCU_KEY_MUTE:
            case RCU_KEY_HOME:
            case RCU_KEY_BACK:
            case RCU_KEY_SEARCH:
            case RCU_KEY_VOLUP:
            case RCU_KEY_VOLDOWN:
                rcu_consumer_and_ir_send_report(key_values[i]);
                break;
            case RCU_KEY_POWER:
                rcu_system_power_down_send_report();
                break;
            case RCU_KEY_APPLIC:
            case RCU_KEY_PAGEUP:
            case RCU_KEY_PAGEDN:
                rcu_keyboard_send_report(key_values[i]);
                break;
            case RCU_KEY_RIGHT:
            case RCU_KEY_LEFT:
            case RCU_KEY_DOWN:
            case RCU_KEY_UP:
            case RCU_KEY_BACKOUT:
            case RCU_KEY_ENTER:
                rcu_mouse_and_keyboard_send_report(key_values[i]);
                break;
            case RCU_KEY_SWITCH_MOUSE_AND_KEY:
            case RCU_KEY_SWITCH_SLE_AND_BLE:
            case RCU_KEY_SWITCH_CONN_ID:
            case RCU_KEY_DISCONNECT_DEVICE:
            case RCU_KEY_SWITCH_IR:
            case RCU_KEY_CONNECT_ADV:
                rcu_state_switch(key_values[i]);
                break;
            case RCU_KEY_MIC:
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
                rcu_amic_init();
#endif
                keystate = pressdown;
                break;
            default:
                break;
        }
    }
    rcu_mouse_and_keyboard_send_start();
    return 1;
}

#if defined (CONFIG_RCU_MASS_PRODUCTION_TEST)
static int rcu_mp_keyscan_callback(int key_nums, uint8_t key_values[])
{
    for (int i = 0; i < key_nums; i++) {
        sle_rcu_keyboard_send_report(key_values[i], true);
    }
    return 1;
}
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static void sle_vdt_set_phy_param(void)
{
    sle_set_phy_t param = { 0 };
    param.tx_format = 1;         /* 无线帧类型2 */
    param.rx_format = 1;         /* 无线帧类型2 */
    param.tx_phy = 0;            /* 0 1M 1 2M 2 4M */
    param.rx_phy = 0;
    param.tx_pilot_density = 0;  /* 导频密度4:1 */
    param.rx_pilot_density = 0;  /* 导频密度4:1 */
    param.g_feedback = 0;
    param.t_feedback = 0;
    if (sle_set_phy_param(0, &param) != 0) {
        return;
    }
    osal_printk("sle_vdt_set_phy_param ok!\r\n");
}

static void sle_vdt_adc_set_io(pin_t pin)
{
    uapi_pin_set_mode(pin, 0);
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_INPUT);
    uapi_pin_set_pull(pin, PIN_PULL_NONE);
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(pin, PIN_IE_1);
#endif
}

static void sle_vdt_adc_init(void)
{
    uapi_pin_init();
    uapi_gpio_init();

    sle_vdt_adc_set_io(CONFIG_ADC_USE_PIN1);
    sle_vdt_adc_set_io(CONFIG_ADC_USE_PIN2);
    uapi_adc_init(ADC_CLOCK_NONE);
    uapi_adc_power_en(AFE_AMIC_MODE, true);
    uapi_adc_open_differential_channel(ADC_GADC_CHANNEL7, ADC_GADC_CHANNEL6);
    adc_calibration(AFE_AMIC_MODE, true, true, true);

    return;
}

static void sle_usb_vdt_dma_transfer_restart(void)
{
    g_write_buffer_state = (g_write_buffer_state + 1) % RING_BUFFER_NUMBER;
    if (g_write_buffer_state != g_read_buffer_state) {
        uint32_t ret = osal_event_write(&g_dma_event_id, PDM_DMA_TRANSFER_EVENT);
        if (ret != OSAL_SUCCESS) {
            return;
        }
    }
    if (sle_vdt_pdm_start_dma_transfer(g_pdm_dma_data[g_write_buffer_state],
        sle_usb_vdt_dma_transfer_done_callback) != 0) {
        return;
    }
}

static void sle_usb_vdt_dma_transfer_done_callback(uint8_t intr, uint8_t channel, uintptr_t arg)
{
    unused(channel);
    unused(arg);
    g_rcu_dma_channel = channel;
    switch (intr) {
        case HAL_DMA_INTERRUPT_TFR:
            sle_usb_vdt_dma_transfer_restart();
            break;
        case HAL_DMA_INTERRUPT_ERR:
            osal_printk("%s DMA transfer error.\r\n", SLE_VDT_SERVER_LOG);
            break;
        default:
            break;
    }
}

static void rcu_amic_init(void)
{
    sle_vdt_adc_init();
    if (sle_vdt_pdm_init() != 0) {
        osal_printk("%s Init the PDM fail.\r\n", SLE_VDT_SERVER_LOG);
    }
    if (uapi_pdm_start() != ERRCODE_SUCC) {
        osal_printk("%s Start the PDM fail.\r\n", SLE_VDT_SERVER_LOG);
    }

    if (sle_vdt_pdm_start_dma_transfer(g_pdm_dma_data[g_write_buffer_state],
        sle_usb_vdt_dma_transfer_done_callback) != 0) {
        return;
    }
}

static void rcu_amic_deinit(void)
{
    uapi_dma_end_transfer(g_rcu_dma_channel);
    uapi_dma_close();
    uapi_dma_deinit();
    dma_port_release_handshaking_source(g_rcu_dma_channel);
    uapi_adc_power_en(AFE_AMIC_MODE, false);
    uapi_adc_deinit();
    uapi_pdm_stop();
    uapi_pdm_deinit();
}
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

static void rcu_keyscan_init(void)
{
    /* keyscan init */
#if defined (CONFIG_KEYSCAN_USER_CONFIG_TYPE)
    uint8_t user_gpio_map[CONFIG_KEYSCAN_ENABLE_ROW + CONFIG_KEYSCAN_ENABLE_COL] = { 12, 13, 14, 15,
                                                                                     16, 21, 28, 22, 23 };
    if (keyscan_porting_set_gpio(user_gpio_map)) {
        return;
    }
#endif
    uapi_set_keyscan_value_map((uint8_t **)g_key_map, CONFIG_KEYSCAN_ENABLE_ROW, CONFIG_KEYSCAN_ENABLE_COL);
    uapi_keyscan_init(EVERY_ROW_PULSE_40_US, HAL_KEYSCAN_MODE_1, KEYSCAN_INT_VALUE_RDY);
    uapi_keyscan_register_callback(rcu_keyscan_callback);
#if defined (CONFIG_RCU_MASS_PRODUCTION_TEST)
    uapi_keyscan_register_callback(rcu_mp_keyscan_callback);
#endif
    uapi_keyscan_enable();
}

static void *rcu_task(const char *arg)
{
    unused(arg);

    osal_printk("enter rcu_task!\r\n");

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
    uint8_t *out_data1, *out_data2;
    while (get_g_ssaps_ready() == false) {
        osal_msleep(RCU_TASK_DURATION_MS);
    }
    while (get_g_conn_update() == 0) {
        osal_msleep(RCU_TASK_DURATION_MS);
    }

    uint32_t buffer_filled_count = 0;
    while (1) {
        uapi_watchdog_kick();
        uint32_t ret = osal_event_read(&g_dma_event_id, PDM_DMA_TRANSFER_EVENT, OSAL_WAIT_FOREVER,
                                       OSAL_WAITMODE_AND | OSAL_WAITMODE_CLR);
        if (ret & PDM_DMA_TRANSFER_EVENT) {
            for (uint32_t i = 0; i < CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA; i++) {
                g_sle_pdm_buffer[buffer_filled_count++] =
                    (uint8_t)(g_pdm_dma_data[g_read_buffer_state][i] >> SLE_VDT_MIC_OFFSET_16);
                g_sle_pdm_buffer[buffer_filled_count++] =
                    (uint8_t)(g_pdm_dma_data[g_read_buffer_state][i] >> SLE_VDT_MIC_OFFSET_24);
            }

            g_read_buffer_state = (g_read_buffer_state + 1) % RING_BUFFER_NUMBER;
            buffer_filled_count = 0;
            uint32_t encode_data_len = sle_vdt_codec_encode(g_sle_pdm_buffer, &out_data1);
            uint32_t encode_data_len2 = sle_vdt_codec_encode(g_sle_pdm_buffer + CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA,
                                                             &out_data2);
            sle_rcu_cpy_data_and_set_valid(out_data1, out_data2, encode_data_len, encode_data_len2);
        }
    }
    uapi_keyscan_deinit();
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    return NULL;
}

static void rcu_entry(void)
{
#ifdef RAM_PARTIAL_RETENTION
    pm_port_disable_partial_retention();
#endif
    if (uapi_clock_control(CLOCK_CLKEN_ID_MCU_CORE, CLOCK_FREQ_LEVEL_LOW_POWER) == ERRCODE_SUCC) {
        osal_printk("Config succ.\r\n");
    } else {
        osal_printk("Config fail.\r\n");
    }

    rcu_keyscan_init();
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
    ble_rcu_server_init();
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
    if (osal_event_init(&g_dma_event_id) != OSAL_SUCCESS) {
        osal_printk("%s touch osal_event_init fail! \r\n", SLE_VDT_SERVER_LOG);
    }
    /* sle server init */
    sle_rcu_server_init(ssaps_server_read_request_cbk, ssaps_server_write_request_cbk);
    sle_vdt_codec_init();
    sle_vdt_set_phy_param();
#endif

    osal_kthread_lock();
    g_rcu_task_handle = osal_kthread_create((osal_kthread_handler)rcu_task, 0, "SleRcuSrverTask",
                                            RCU_TASK_STACK_SIZE);
    if (g_rcu_task_handle != NULL) {
        osal_kthread_set_priority(g_rcu_task_handle, SLE_RCU_SERVER_TASK_PRIO);
    }
    osal_kthread_unlock();
}

#if defined(CONFIG_PM_SYS_SUPPORT)
static void ulp_gpio_wkup_handler(uint8_t ulp_gpio)
{
    uapi_pm_wkup_process(0);
    osal_printk("ulp_gpio%d wakeup\n", ulp_gpio);
}

static ulp_gpio_int_wkup_cfg_t g_wk_cfg[] = {
    { 1, 35, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler }, /* ulp_gpio1设置为下降沿中断，用于系统唤醒并
                                                                              在系统唤醒后进入回调处理。 */
};

static void rcu_enable_ulpgpio_wkup(void)
{
    uapi_gpio_deinit();
    ulp_gpio_init();
    ulp_gpio_int_wkup_config(g_wk_cfg, sizeof(g_wk_cfg) / sizeof(ulp_gpio_int_wkup_cfg_t));
}

static void rcu_disable_ulpgpio_wkup(void)
{
    ulp_gpio_deinit();
    uapi_gpio_init();
}

static int32_t rcu_state_work_to_standby(uintptr_t arg)
{
    unused(arg);
    uapi_keyscan_suspend(arg);
    rcu_enable_ulpgpio_wkup();
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
    uapi_pin_set_pull(S_MGPIO0, PIN_PULL_NONE);
    uapi_pin_set_pull(S_MGPIO1, PIN_PULL_NONE);
    /* 连接态：增大连接间隔；广播态：暂不操作。 */
    sle_rcu_work_to_standby();
#else
    /* 连接态：增大连接间隔；广播态：暂不操作。 */
    ble_rcu_work_to_standby();
#endif
    return 0;
}

static int32_t rcu_state_standby_to_sleep(uintptr_t arg)
{
    unused(arg);
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
    /* 断开连接、关闭广播。 */
    sle_rcu_standby_to_sleep();
#else
    /* 断开连接、关闭广播。 */
    ble_rcu_standby_to_sleep();
#endif
#ifdef RAM_PARTIAL_RETENTION
    pm_port_enable_partial_retention();
#endif
    return 0;
}

static int32_t rcu_state_standby_to_work(uintptr_t arg)
{
    unused(arg);
    rcu_disable_ulpgpio_wkup();
    uapi_keyscan_resume(arg);

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
    uapi_pin_set_pull(S_MGPIO0, PIN_PULL_DOWN);
    uapi_pin_set_pull(S_MGPIO1, PIN_PULL_DOWN);
    /* 连接态：减小连接间隔；广播态：不操作。 */
    sle_rcu_standby_to_work();
#else
    /* 连接态：减小连接间隔；广播态：不操作。 */
    ble_rcu_standby_to_work();
#endif
    return 0;
}

static int32_t rcu_state_sleep_to_work(uintptr_t arg)
{
    unused(arg);
    rcu_disable_ulpgpio_wkup();
    uapi_keyscan_resume(arg);

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
    uapi_pin_set_pull(S_MGPIO0, PIN_PULL_DOWN);
    uapi_pin_set_pull(S_MGPIO1, PIN_PULL_DOWN);
    /* 打开广播、蓝牙回连。 */
    sle_rcu_sleep_to_work();
#else
    /* 打开广播、蓝牙回连。 */
    ble_rcu_sleep_to_work();
#endif
    return 0;
}

void rcu_low_power_entry(void)
{
    pm_state_trans_handler_t handler = {
        .work_to_standby = rcu_state_work_to_standby,
        .standby_to_sleep = rcu_state_standby_to_sleep,
        .standby_to_work = rcu_state_standby_to_work,
        .sleep_to_work = rcu_state_sleep_to_work,
    };
    uapi_pm_state_trans_handler_register(&handler);

    uapi_pm_work_state_reset();
    rcu_disable_ulpgpio_wkup();
    uapi_pm_set_state_trans_duration(DURATION_MS_OF_WORK_TO_STANDBY, DURATION_MS_OF_STANDBY_TO_SLEEP);
}

app_run(rcu_low_power_entry);
#endif

/* Run the rcu_entry. */
app_run(rcu_entry);