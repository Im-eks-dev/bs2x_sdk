/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: APP Keyscan Source File. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-16, Create file. \n
 */

#include "securec.h"
#include "soc_osal.h"
#include "app_msg_queue.h"
#include "app_timer.h"
#include "pdm.h"
#include "hal_dma.h"
#include "keyscan.h"
#include "keyscan_porting.h"
#include "osal_mutex.h"
#include "adc.h"
#include "adc_porting.h"
#include "pinctrl.h"
#include "common_def.h"
#include "app_init.h"
#include "watchdog.h"
#include "gpio.h"
#include "pm_clock.h"
#include "hal_adc.h"
#include "ir_study.h"
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
#include "sle_rcu_server.h"
#include "sle_rcu_server_adv.h"
#include "sle_common.h"
#include "sle_connection_manager.h"
#include "sle_vdt_pdm.h"
#include "app_voice.h"
#endif
/* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
#include "ble_rcu_server.h"
#include "ble_rcu_server_adv.h"
#include "ble_hid_rcu_server.h"
#endif
/* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
#include "bts_le_gap.h"
#if defined(CONFIG_PM_SYS_SUPPORT)
#include "ulp_gpio.h"
#include "gpio.h"
#include "pm_sys.h"
#include "app_ulp.h"
#endif
#include "app_status.h"
#include "app_common.h"
#if defined(CONFIG_SAMPLE_SUPPORT_IR)
#include "ir_nec.h"
#include "ir_porting.h"
#include "chip_core_irq.h"
#endif
#if defined(CONFIG_RCU_MASS_PRODUCTION_TEST)
#include "rcu_mp_test.h"
#endif
#include "app_keyscan.h"

combine_key_e g_combine_key_flag = COMBINE_KEY_FLAG_NONE;
osal_mutex g_key_process;

static bool g_switch_mouse_and_keyboard = false;
static bool g_switch_consumer_and_ir = true;
static bool g_check_consumer_send = false;
static bool g_check_keyboard_send = false;
static bool g_keystate_down = 0;
static uint16_t g_conn_id = 0;

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER) || defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
static const uint16_t g_consumer_key_map[RCU_CONSUMER_KEY_NUM] = {0xE2, 0x223, 0x224, 0x221, 0xE9, 0xEA};
static const uint8_t g_consumer_key_index[RCU_CONSUMER_KEY_NUM] = {0x4, 0x5, 0x6, 0x7, 0x8, 0x9};
static uint32_t g_keyboard_send_count = 0;
static bool g_check_mouse_send = false;
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER) && !defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static bool g_switch_sle_and_ble = false;
#else
static bool g_switch_sle_and_ble = true;
#endif
/* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
#define RING_BUFFER_NUMBER 4
uint8_t g_sle_pdm_buffer[CONFIG_USB_UAC_MAX_RECORD_SIZE] = {0};
uint8_t g_write_buffer_node = 0;
uint8_t g_read_buffer_node = 0;
static uint32_t g_pdm_dma_data0[CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA] = {0};
static uint32_t g_pdm_dma_data1[CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA] = {0};
static uint32_t g_pdm_dma_data2[CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA] = {0};
static uint32_t g_pdm_dma_data3[CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA] = {0};
uint32_t *g_pdm_dma_data[RING_BUFFER_NUMBER] = {g_pdm_dma_data0, g_pdm_dma_data1, g_pdm_dma_data2, g_pdm_dma_data3};
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER) || defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
typedef struct usb_hid_mouse_report {
    mouse_key_t key;
    int8_t x;     /* A negative value indicates that the mouse moves left. */
    int8_t y;     /* A negative value indicates that the mouse moves up. */
    int8_t wheel; /* A negative value indicates that the wheel roll forward. */
} usb_hid_mouse_report_t;

typedef struct usb_hid_keyboard_report {
    uint8_t special_key; /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t reserve;
    uint8_t key[USB_HID_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_keyboard_report_t;

typedef struct usb_hid_consumer_report {
    uint8_t comsumer_key0;
    uint8_t comsumer_key1;
} usb_hid_consumer_report_t;
#endif
/* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static usb_hid_mouse_report_t g_hid_sle_mouse_report;
static usb_hid_keyboard_report_t g_hid_sle_keyboard_report;
static usb_hid_consumer_report_t g_hid_sle_consumer_report;
#endif
/* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
static usb_hid_mouse_report_t g_hid_ble_mouse_report;
static usb_hid_keyboard_report_t g_hid_ble_keyboard_report;
static usb_hid_consumer_report_t g_hid_ble_consumer_report;
#endif
/* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */

#if defined(CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE)
static const uint8_t g_key_map[CONFIG_KEYSCAN_ENABLE_ROW][CONFIG_KEYSCAN_ENABLE_COL] = {
    {0x29, 0x2B, 0x14, 0x35, 0x04, 0x1E, 0x1D, 0x00},
    {0x3D, 0x3C, 0x08, 0x3B, 0x07, 0x20, 0x06, 0x00},
    {0x00, 0x39, 0x1A, 0x3A, 0x16, 0x1F, 0x1B, 0x00},
    {0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xE4, 0x00},
    {0x0A, 0x17, 0x15, 0x22, 0x09, 0x21, 0x19, 0x05},
    {0x0B, 0x1C, 0x18, 0x23, 0x0D, 0x24, 0x10, 0x11},
    {0x3F, 0x30, 0x0C, 0x2E, 0x0E, 0x25, 0x36, 0x00},
    {0x00, 0x00, 0x12, 0x40, 0x0F, 0x26, 0x37, 0x00},
    {0x34, 0x2F, 0x13, 0x2D, 0x33, 0x27, 0x00, 0x38},
    {0x3E, 0x2A, 0x00, 0x41, 0x31, 0x42, 0x28, 0x2C},
    {0x00, 0x00, 0xE3, 0x00, 0x00, 0x43, 0x00, 0x51},
    {0xE2, 0x00, 0x00, 0x00, 0x00, 0x45, 0xE5, 0xE6},
    {0x00, 0x53, 0x00, 0x00, 0xE1, 0x44, 0x00, 0x4F},
    {0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4C, 0x50},
    {0x5F, 0x5C, 0x61, 0x5E, 0x59, 0x62, 0x55, 0x5B},
    {0x54, 0x60, 0x56, 0x57, 0x5D, 0x5A, 0x58, 0x63},
};
#endif /* CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE */

#if defined(CONFIG_KEYSCAN_USE_SIX_KEYS_TYPE)
static const uint8_t g_key_map[CONFIG_KEYSCAN_ENABLE_ROW][CONFIG_KEYSCAN_ENABLE_COL] = {
    {RCU_KEY_HOME,      RCU_KEY_BACK},
    {RCU_KEY_SEARCH,    RCU_KEY_VOLUP},
    {RCU_KEY_VOLDOWN,   RCU_KEY_VOLDOWN},
};
#endif /* CONFIG_KEYSCAN_USE_SIX_KEYS_TYPE */

#if defined(CONFIG_KEYSCAN_USER_CONFIG_TYPE)
static const uint8_t g_key_map[CONFIG_KEYSCAN_ENABLE_ROW][CONFIG_KEYSCAN_ENABLE_COL] = {
    {RCU_KEY_CONNECT_ADV, RCU_KEY_STANDBY, RCU_KEY_BACK, RCU_KEY_SWITCH_IR, RCU_KEY_VOLUP},
    {RCU_KEY_APPLIC, RCU_KEY_UP, RCU_KEY_ENTER, RCU_KEY_DOWN, RCU_KEY_HOME},
    {RCU_KEY_LEFT, RCU_KEY_RIGHT, RCU_KEY_MIC, RCU_KEY_DISCONNECT_DEVICE, RCU_KEY_WAKEUP_ADV},
    {RCU_KEY_VOLDOWN, RCU_KEY_SWITCH_MOUSE_AND_KEY, 0x0, 0x0, 0x0},
};
#endif /* CONFIG_KEYSCAN_USER_CONFIG_TYPE */

#define COMBINE_KEY_MAX 9
#define COMBINE_KEY_TYPE_INDEX 3

static const uint8_t combine_key[COMBINE_KEY_MAX][4] = {
    {RCU_KEY_LEFT,  RCU_KEY_RIGHT,  0x0,  COMBINE_KEY_FLAG_IR_LEARN},
    {RCU_KEY_UP, RCU_KEY_RIGHT, 0x0, COMBINE_KEY_FLAG_PAIR},
    {RCU_KEY_DOWN, RCU_KEY_RIGHT, 0x0, COMBINE_KEY_FLAG_UNPAIR},
#if defined(CONFIG_RCU_MASS_PRODUCTION_TEST)
    {RCU_KEY_APPLIC, RCU_KEY_UP, 0X0, COMBINE_KEY_FLAG_TEST_STATION_01},
    {RCU_KEY_APPLIC, RCU_KEY_DOWN, 0X0, COMBINE_KEY_FLAG_TEST_STATION_02},
    {RCU_KEY_APPLIC, RCU_KEY_RIGHT, 0X0, COMBINE_KEY_FLAG_TEST_STATION_03},
    {RCU_KEY_ENTER, RCU_KEY_UP, 0X0, COMBINE_KEY_FLAG_TEST_STATION_04},
    {RCU_KEY_ENTER, RCU_KEY_DOWN, 0X0, COMBINE_KEY_FLAG_TEST_STATION_05},
    {RCU_KEY_ENTER, RCU_KEY_RIGHT, 0X0, COMBINE_KEY_FLAG_TEST_STATION_06},
#endif
    };

static void set_combine_key_flag(combine_key_e flag)
{
    g_combine_key_flag = flag;
}

static combine_key_e get_combine_key_flag(void)
{
    return g_combine_key_flag;
}

static bool is_key_match(uint8_t template_key, uint8_t *key_buffer, uint8_t key_num)
{
    for (int i = 0; i < key_num; i++) {
        if (template_key == key_buffer[i]) {
            return true;
        }
    }

    return false;
}

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)

static void sle_rcu_consumer_send_report(uint8_t key_value)
{
    if (memset_s(&g_hid_sle_consumer_report, sizeof(g_hid_sle_consumer_report), 0, sizeof(g_hid_sle_consumer_report)) !=
        EOK) {
        return;
    }
    for (uint8_t i = 0; i < RCU_CONSUMER_KEY_NUM; i++) {
        if (key_value == g_consumer_key_index[i]) {
            g_hid_sle_consumer_report.comsumer_key0 = g_consumer_key_map[i] & 0xFF;
            g_hid_sle_consumer_report.comsumer_key1 = g_consumer_key_map[i] >> RCU_CONSUMER_KEY_OFFSET;
        }
    }
    sle_rcu_server_send_report_by_handle(
        (uint8_t *)(uintptr_t)&g_hid_sle_consumer_report, sizeof(usb_hid_consumer_report_t), g_conn_id);
}

static void sle_rcu_keyboard_send_report(uint8_t key_value, bool send_flag)
{
    if ((key_value != 0) && (g_keyboard_send_count < USB_HID_MAX_KEY_LENTH)) {
        g_hid_sle_keyboard_report.key[g_keyboard_send_count++] = key_value;
    }
    if (send_flag) {
        sle_rcu_server_send_report_by_handle(
            (uint8_t *)(uintptr_t)&g_hid_sle_keyboard_report, sizeof(usb_hid_keyboard_report_t), g_conn_id);

        (void)memset_s(&g_hid_sle_keyboard_report, sizeof(g_hid_sle_keyboard_report), 0,
                       sizeof(g_hid_sle_keyboard_report));
        g_keyboard_send_count = 0;
    }
}

static void sle_rcu_system_power_down_send_report(void)
{
    if (memset_s(&g_hid_sle_consumer_report, sizeof(g_hid_sle_consumer_report), 0, sizeof(g_hid_sle_consumer_report)) !=
        EOK) {
        return;
    }
    g_hid_sle_consumer_report.comsumer_key0 = 0x01;
    g_hid_sle_consumer_report.comsumer_key1 = 0x00;
    sle_rcu_server_send_report_by_handle(
        (uint8_t *)(uintptr_t)&g_hid_sle_consumer_report, sizeof(usb_hid_consumer_report_t), g_conn_id);
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
        sle_rcu_server_send_report_by_handle(
            (uint8_t *)(uintptr_t)&g_hid_sle_mouse_report, sizeof(usb_hid_mouse_report_t), g_conn_id);
        if (memset_s(&g_hid_sle_mouse_report, sizeof(g_hid_sle_mouse_report), 0,
                     sizeof(g_hid_sle_mouse_report)) != EOK) {
            return;
        }
    }
}

static void sle_rcu_send_end(void)
{
    if (g_check_consumer_send) {
        if (memset_s(&g_hid_sle_consumer_report, sizeof(g_hid_sle_consumer_report), 0,
                     sizeof(g_hid_sle_consumer_report)) != EOK) {
            return;
        }
        sle_rcu_server_send_report_by_handle(
            (uint8_t *)(uintptr_t)&g_hid_sle_consumer_report, sizeof(usb_hid_consumer_report_t), g_conn_id);
        g_check_consumer_send = false;
    }
    if (g_check_mouse_send) {
        if (memset_s(&g_hid_sle_mouse_report, sizeof(g_hid_sle_mouse_report), 0, sizeof(g_hid_sle_mouse_report)) !=
            EOK) {
            return;
        }
        sle_rcu_server_send_report_by_handle(
            (uint8_t *)(uintptr_t)&g_hid_sle_mouse_report, sizeof(usb_hid_mouse_report_t), g_conn_id);
        g_check_mouse_send = false;
    }
    if (g_check_keyboard_send) {
        if (memset_s(&g_hid_sle_keyboard_report, sizeof(g_hid_sle_keyboard_report), 0,
                     sizeof(g_hid_sle_keyboard_report)) != EOK) {
            return;
        }
        sle_rcu_server_send_report_by_handle(
            (uint8_t *)(uintptr_t)&g_hid_sle_keyboard_report, sizeof(usb_hid_keyboard_report_t), g_conn_id);
        g_check_keyboard_send = false;
    }
}

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

static void rcu_wakeup_adv(void)
{
    sle_addr_t addr[RCU_TARGET_ADDR_NUM];
    uint16_t number = 1;

    sle_get_bonded_devices(addr, &number);
    if (number > 0) {
        sle_rcu_server_wakeup_adv_init(addr);
    }
}
#endif
/* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
static void ble_rcu_consumer_send_report(uint8_t key_value)
{
    if (memset_s(&g_hid_ble_consumer_report, sizeof(g_hid_ble_consumer_report), 0, sizeof(g_hid_ble_consumer_report)) !=
        EOK) {
        return;
    }
    for (uint8_t i = 0; i < RCU_CONSUMER_KEY_NUM; i++) {
        if (key_value == g_consumer_key_index[i]) {
            g_hid_ble_consumer_report.comsumer_key0 = g_consumer_key_map[i] & 0xFF;
            g_hid_ble_consumer_report.comsumer_key1 = g_consumer_key_map[i] >> RCU_CONSUMER_KEY_OFFSET;
        }
    }
    ble_hid_rcu_server_send_consumer_input_report_by_uuid(
        (uint8_t *)(uintptr_t)&g_hid_ble_consumer_report, sizeof(usb_hid_consumer_report_t), g_conn_id);
}

static void ble_rcu_keyboard_send_report(uint8_t key_value, bool send_flag)
{
    if (key_value != 0) {
        g_hid_ble_keyboard_report.key[g_keyboard_send_count++] = key_value;
    }
    if (send_flag) {
        ble_hid_rcu_server_send_keyboard_input_report_by_uuid(
            (uint8_t *)(uintptr_t)&g_hid_ble_keyboard_report, sizeof(usb_hid_keyboard_report_t), g_conn_id);
        if (memset_s(&g_hid_ble_keyboard_report, sizeof(g_hid_ble_keyboard_report), 0,
                     sizeof(g_hid_ble_keyboard_report)) != EOK) {
            g_keyboard_send_count = 0;
            return;
        }
        g_keyboard_send_count = 0;
    }
}

static void ble_rcu_system_power_down_send_report(void)
{
    if (memset_s(&g_hid_ble_consumer_report, sizeof(g_hid_ble_consumer_report), 0, sizeof(g_hid_ble_consumer_report)) !=
        EOK) {
        return;
    }
    g_hid_ble_consumer_report.comsumer_key0 = 0x01;
    g_hid_ble_consumer_report.comsumer_key1 = 0x00;
    ble_hid_rcu_server_send_power_input_report_by_uuid(
        (uint8_t *)(uintptr_t)&g_hid_ble_consumer_report, sizeof(usb_hid_consumer_report_t), g_conn_id);
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
        ble_hid_rcu_server_send_mouse_input_report_by_uuid(
            (uint8_t *)(uintptr_t)&g_hid_ble_mouse_report, sizeof(usb_hid_mouse_report_t), g_conn_id);
        if (memset_s(&g_hid_ble_mouse_report, sizeof(g_hid_ble_mouse_report), 0, sizeof(g_hid_ble_mouse_report)) !=
            EOK) {
            return;
        }
    }
}

static void ble_rcu_send_end(void)
{
    if (g_check_consumer_send) {
        if (memset_s(&g_hid_ble_consumer_report, sizeof(g_hid_ble_consumer_report), 0,
                     sizeof(g_hid_ble_consumer_report)) != EOK) {
            return;
        }
        ble_hid_rcu_server_send_consumer_input_report_by_uuid(
            (uint8_t *)(uintptr_t)&g_hid_ble_consumer_report, sizeof(usb_hid_consumer_report_t), g_conn_id);
        g_check_consumer_send = false;
    }
    if (g_check_mouse_send) {
        if (memset_s(&g_hid_ble_mouse_report, sizeof(g_hid_ble_mouse_report), 0, sizeof(g_hid_ble_mouse_report)) !=
            EOK) {
            return;
        }
        ble_hid_rcu_server_send_mouse_input_report_by_uuid(
            (uint8_t *)(uintptr_t)&g_hid_ble_mouse_report, sizeof(usb_hid_mouse_report_t), g_conn_id);
        g_check_mouse_send = false;
    }
    if (g_check_keyboard_send) {
        if (memset_s(&g_hid_ble_keyboard_report, sizeof(g_hid_ble_keyboard_report), 0,
                     sizeof(g_hid_ble_keyboard_report)) != EOK) {
            return;
        }
        ble_hid_rcu_server_send_keyboard_input_report_by_uuid(
            (uint8_t *)(uintptr_t)&g_hid_ble_keyboard_report, sizeof(usb_hid_keyboard_report_t), g_conn_id);
        g_check_keyboard_send = false;
    }
}
#endif
/* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_IR)
static void sle_ir_function_switch(uint8_t key)
{
    switch (key) {
        case RCU_KEY_STANDBY:
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


static void rcu_consumer_send_report(uint8_t key_value)
{
    unused(key_value);
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        sle_rcu_consumer_send_report(key_value);
#endif
        /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        ble_rcu_consumer_send_report(key_value);
#endif
        /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
    g_check_consumer_send = true;
}

static void rcu_keyboard_send_report(uint8_t key_value)
{
    unused(key_value);
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        sle_rcu_keyboard_send_report(key_value, false);
#endif
        /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        ble_rcu_keyboard_send_report(key_value, false);
#endif
        /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
    g_check_keyboard_send = true;
}

static void rcu_mouse_and_keyboard_send_report(uint8_t key_value)
{
    unused(key_value);
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        if (g_switch_mouse_and_keyboard) {
            sle_rcu_mouse_send_report(key_value, false);
            g_check_mouse_send = true;
        } else {
            sle_rcu_keyboard_send_report(key_value, false);
            g_check_keyboard_send = true;
        }
#endif
        /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        if (g_switch_mouse_and_keyboard) {
            ble_rcu_mouse_send_report(key_value, false);
            g_check_mouse_send = true;
        } else {
            ble_rcu_keyboard_send_report(key_value, false);
            g_check_keyboard_send = true;
        }
#endif
        /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
}


static void rcu_consumer_and_ir_send_report(uint8_t key_value)
{
    if (key_value == RCU_KEY_VOLUP) {
        app_print("RCU_KEY_VOLUP is pressed!\r\n");
    }
    if (g_switch_consumer_and_ir) {
        rcu_consumer_send_report(key_value);
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_IR)
        sle_ir_function_switch(key_value);
#endif /* CONFIG_SAMPLE_SUPPORT_IR */
    }
    g_check_consumer_send = true;
}

static void rcu_system_power_down_send_report(void)
{
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        sle_rcu_system_power_down_send_report();
#endif
        /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        ble_rcu_system_power_down_send_report();
#endif
        /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
}

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
        case RCU_KEY_WAKEUP_ADV:
            rcu_wakeup_adv();
            break;
#endif
        default:
            break;
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
#endif
        /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        if (g_check_mouse_send) {
            ble_rcu_mouse_send_report(0, true);
        }
        if (g_check_keyboard_send) {
            ble_rcu_keyboard_send_report(0, true);
        }
#endif
        /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
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
#endif
        /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        ble_rcu_send_end();
#endif
        /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
    if (g_keystate_down) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        rcu_amic_deinit(g_conn_id);
#endif
        g_keystate_down = false;
    }
}

#if defined(CONFIG_SAMPLE_SUPPORT_IR) && defined(CONFIG_SAMPLE_SUPPORT_IR_STUDY)
void app_uapi_ir_study_start(uint8_t key_value)
{
    uapi_ir_study_start(key_value);
}
#else
void app_uapi_ir_study_start(uint8_t key_value)
{
    unused(key_value);
}
#endif
 
void app_ir_key_process(uint8_t key_value)
{
    if (get_rcu_status() == RCU_STATUS_IR_STUDY) {
        app_uapi_ir_study_start(key_value);
    } else {
        rcu_consumer_and_ir_send_report(key_value);
    }
}

/* 单键操作 */
static void one_key_process(uint8_t key_value)
{
    switch (key_value) {
        case RCU_KEY_HOME:
        case RCU_KEY_BACK:
        case RCU_KEY_SEARCH:
        case RCU_KEY_VOLUP:
        case RCU_KEY_VOLDOWN:
            app_ir_key_process(key_value);
            break;
        case RCU_KEY_POWER:
            rcu_system_power_down_send_report();
            break;
        case RCU_KEY_APPLIC:
        case RCU_KEY_PAGEUP:
        case RCU_KEY_PAGEDN:
        case RCU_KEY_STANDBY:
            rcu_keyboard_send_report(key_value);
            break;
        case RCU_KEY_RIGHT:
        case RCU_KEY_LEFT:
        case RCU_KEY_DOWN:
        case RCU_KEY_UP:
        case RCU_KEY_BACKOUT:
        case RCU_KEY_ENTER:
            rcu_mouse_and_keyboard_send_report(key_value);
            break;
        case RCU_KEY_SWITCH_MOUSE_AND_KEY:
        case RCU_KEY_SWITCH_SLE_AND_BLE:
        case RCU_KEY_SWITCH_CONN_ID:
        case RCU_KEY_DISCONNECT_DEVICE:
        case RCU_KEY_SWITCH_IR:
        case RCU_KEY_CONNECT_ADV:
        case RCU_KEY_WAKEUP_ADV:
            rcu_state_switch(key_value);
            break;
        case RCU_KEY_MIC:
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
            rcu_amic_init(g_conn_id);
#endif
            g_keystate_down = true;
            break;
        default:
            break;
    }
    rcu_mouse_and_keyboard_send_start();
}

#if defined(CONFIG_RCU_MASS_PRODUCTION_TEST)
static int rcu_mp_keyscan_callback(int key_nums, uint8_t key_values[])
{
    for (int i = 0; i < key_nums; i++) {
        sle_rcu_keyboard_send_report(key_values[i], true);
    }
    return 1;
}
#endif

/* 组合键判断 */
static void combine_key_detect_handle(key_t key)
{
    app_timer_process_stop(TIME_CMD_KEY_HOLD_LONG);

    if (key.num == PRESS_ONE_KEY) {
    }
    if (key.num == PRESS_TWO_KEY) {
        for (int i = 0; i < COMBINE_KEY_MAX; i++) {
            bool tag_one = is_key_match(combine_key[i][0], key.key_value, key.num);
            bool tag_two = is_key_match(combine_key[i][1], key.key_value, key.num);
            if (tag_one && tag_two) {
                set_combine_key_flag(combine_key[i][COMBINE_KEY_TYPE_INDEX]);
                app_timer_process_start(TIME_CMD_KEY_HOLD_LONG, APP_HOLD_LONG_TIME);
            }
        }
    }
}

static void key_up_process(void)
{
    rcu_send_end();
    app_timer_process_stop(TIME_CMD_KEY_HOLD_LONG);
}

static void key_down_process(key_t key)
{
    combine_key_detect_handle(key);
#if defined(CONFIG_RCU_MASS_PRODUCTION_TEST)
    rcu_mp_test_rcu_to_dongle_key_value(key.num, key.key_value);
#endif
    if (key.num == PRESS_NONE_KEY) {
        osal_printk("key down process key.num == 0 error!\r\n");
    } else if (key.num == PRESS_ONE_KEY) {
        one_key_process(key.key_value[0]);
    } else if (key.num == PRESS_TWO_KEY) {
        /* 配对组合键 */
        for (int i = 0; i < COMBINE_KEY_MAX; i++) {
            bool tag_one = is_key_match(combine_key[i][0], key.key_value, key.num);
            bool tag_two = is_key_match(combine_key[i][1], key.key_value, key.num);
            if (tag_one && tag_two) {
                osal_printk("%d\r\n", combine_key[i][COMBINE_KEY_TYPE_INDEX]);
            }
        }
    }
}

/**************************************************
 * 按键长按处理
 *
 **************************************************/

// 配对
static void key_handle_process_repairing_event(void)
{
    osal_printk("key_handle_process_repairing_event start!\r\n");
}

// 解配
static void key_handle_process_unpairing_event(void)
{
    osal_printk("key_handle_process_unpairing_event start\r\n");
}

// IR study
static void key_handle_process_ir_study_event(void)
{
    if (get_rcu_status() != RCU_STATUS_IR_STUDY) {
        set_rcu_status(RCU_STATUS_IR_STUDY);
        app_timer_process_start(TIME_CMD_IR_STUDY, APP_IR_STUDY_TIME);
        osal_printk("entry RCU_MODE_IR_STUDY\r\n");
    } else {
        app_print("entry exit\r\n");
        app_timer_process_stop(TIME_CMD_IR_STUDY);
        osal_printk("before clean\r\n");
        clear_rcu_status(RCU_STATUS_IR_STUDY);
        osal_printk("exit RCU_MODE_IR_STUDY\r\n");
    }
}

static void key_hold_process(void)
{
    combine_key_e combine_key_flag = get_combine_key_flag();

    switch (combine_key_flag) {
#if defined(CONFIG_RCU_MASS_PRODUCTION_TEST)
        case COMBINE_KEY_FLAG_TEST_STATION_01:
        case COMBINE_KEY_FLAG_TEST_STATION_02:
        case COMBINE_KEY_FLAG_TEST_STATION_03:
        case COMBINE_KEY_FLAG_TEST_STATION_04:
        case COMBINE_KEY_FLAG_TEST_STATION_05:
        case COMBINE_KEY_FLAG_TEST_STATION_06:
            rcu_mp_test_set_work_station(combine_key_flag);
            break;
#endif
        // 配对组合键
        case COMBINE_KEY_FLAG_PAIR:
            key_handle_process_repairing_event();
            break;

        // 解配组合键
        case COMBINE_KEY_FLAG_UNPAIR:
            key_handle_process_unpairing_event();
            break;

        // IR study组合键
        case COMBINE_KEY_FLAG_IR_LEARN:
            key_handle_process_ir_study_event();
            break;

        default:
            break;
    }

    set_combine_key_flag(COMBINE_KEY_FLAG_NONE);
}

void keyevent_process(uint8_t *key_value, uint8_t key_num, APP_MSG_DATA_TYPE event)
{
    if (osal_mutex_lock(&g_key_process) == OSAL_SUCCESS) {
#if defined(CONFIG_PM_SYS_SUPPORT)
        uapi_pm_work_state_reset();
#endif
        app_timer_process_start(TIME_CMD_SLEEP_CHECK, APP_SLEEP_CHECK_TIME);
        if (event == KEY_DOWN_EVENT_MSG) {
            key_t temp_key;
            temp_key.num = (key_num < KEY_MAX_NUM ? key_num : KEY_MAX_NUM);
            memcpy_s(temp_key.key_value, KEY_MAX_NUM, key_value, KEY_MAX_NUM);
            key_down_process(temp_key);
        } else if (event == KEY_UP_EVENT_MSG) {
            key_up_process();
        } else if (event == KEY_HOLD_LONG_EVENT_MSG) {
            key_hold_process();
        }
        osal_mutex_unlock(&g_key_process);
    }
}

static int app_keyscan_callback(int key_num, uint8_t key_value[])
{
    app_msg_data_t msg;
    if (key_num == 0) {
        msg.type = KEY_UP_EVENT_MSG;
        memcpy_s(msg.buffer, APP_MSG_BUFFER_LEN, NULL, KEY_MAX_NUM);
        msg.length = 0;
#if defined(CONFIG_PM_SYS_SUPPORT)
        uapi_pm_set_state_trans_duration(DURATION_MS_OF_WORK_TO_STANDBY, DURATION_MS_OF_STANDBY_TO_SLEEP);
#endif
    } else {
#if defined(CONFIG_PM_SYS_SUPPORT)
        uapi_pm_set_state_trans_duration(0xFFFFFFFF, 0xFFFFFFFF);
#endif
        msg.type = KEY_DOWN_EVENT_MSG;
        memcpy_s(msg.buffer, APP_MSG_BUFFER_LEN, key_value, KEY_MAX_NUM);
        msg.length = key_num;
    }
    app_write_msgqueue(msg);
    return 1;
}

void app_keyscan_init(void)
{
    /* keyscan init */
    osal_mutex_init(&g_key_process);
#if defined(CONFIG_KEYSCAN_USER_CONFIG_TYPE)
    uint8_t user_gpio_map[CONFIG_KEYSCAN_ENABLE_ROW + CONFIG_KEYSCAN_ENABLE_COL] = {12, 13, 14, 15, 16, 21, 28, 22, 23};
    if (keyscan_porting_set_gpio(user_gpio_map)) {
        return;
    }
#endif
    uapi_set_keyscan_value_map((uint8_t **)g_key_map, CONFIG_KEYSCAN_ENABLE_ROW, CONFIG_KEYSCAN_ENABLE_COL);
    uapi_keyscan_init(EVERY_ROW_PULSE_40_US, HAL_KEYSCAN_MODE_1, KEYSCAN_INT_VALUE_RDY);
    uapi_keyscan_register_callback(app_keyscan_callback);
#if defined(CONFIG_RCU_MASS_PRODUCTION_TEST)
    uapi_keyscan_register_callback(rcu_mp_keyscan_callback);
#endif
    uapi_keyscan_enable();
}