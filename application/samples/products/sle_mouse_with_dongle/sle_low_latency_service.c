/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Sle Low Latency Mouse Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include "soc_osal.h"
#include "qdec.h"
#include "pinctrl.h"
#include "gpio.h"
#include "tcxo.h"
#include "adc.h"
#include "adc_porting.h"
#include "timer.h"
#include "chip_core_irq.h"
#include "usb_porting.h"
#include "gadget/f_hid.h"
#include "mouse_button/mouse_button.h"
#include "sle_mouse_server/sle_mouse_server.h"
#include "sle_low_latency.h"
#include "sle_connection_manager.h"
#include "sle_errcode.h"
#include "sle_low_latency_service.h"

#define GAFE_SAMPLE_VALUE_SIGN_BIT          17
#define VBAT_SAMPLE_INTERVAL_MS             10000
#define SPI_RECV_DATA_LEN                   1
#define SPI_SEND_DATA_LEN                   2
#define MOUSE_TO_BT_DATA_LEN                5
#define DELAY_MS3                           3
#define MOUSE_DELAY_J_NUM                   40
#define MOUSE_DELAY_1US                     1
#define MOUSE_DELAY_2US                     2
#define MOUSE_DELAY_6US                     6
#define TURNOVER_SIGN                       (-1)
#define DELAY_US200                         200
#define SEND_MOUSE_MSG_TEST                 100
#define TEST_USB_TIMER_MS                   2
#define WHEEL_DATA                          4
#define MS_PER_S                            1000
#define USB_HID_MOUSE_SIM_SEND_DELAY_MS     (500UL)
#define USB_HID_MOUSE_SIM_SEND_DELAY_US     (500UL)
#define USB_HID_MOUSE_INIT_DELAY_MS         (500UL)
#define BLE_HID_MOUSE_HIGH_XY_BIT           12
#define USB_MOUSE_DRAW_QUADRATE_ANGLE       4
#define USB_MOUSE_DRAW_QUADRATE_TIMES       100
#define MOUSE_INPUT_MOVE_LEFT               200
#define MOUSE_INPUT_MOVE_RIGHT              100
#define MOUSE_INPUT_MOVE_UP                 200
#define MOUSE_INPUT_MOVE_DOWN               100
#define MOUSE_INPUT_ROLL_FORWARD            200
#define MOUSE_INPUT_ROLL_BACK               100
#define MOUSE_MOVE_STEP                     100
#define MOUSE_ROLL_STEP                     100
#define MOUSE_INPUT_NUM                     6
#define MOUSE_SIM_TIME                      1
#define MOUSE_INPUT_KEY                     0
#define MOUSE_INPUT_X                       1
#define MOUSE_INPUT_Y                       2
#define MOUSE_INPUT_WHEEL                   3
#define SIMULATE_TIMES                      (380)
#define DELAY_MS                            (1000)
#define DELAY_INPUT                         (20)
#define USB_MOUSE_POLLING_RATE_MAX_BIT      4
#define USB_MOUSE_POLLING_RATE_MAX_VALUE    8
#define MOUSE_KEY_UP_DELAY                  10
#define MOUSE_KEY_CLICK_DELAY               200
#define DATA_BIT2                           2
#define DATA_BIT3                           3
#define DATA_BIT4                           4
#define DATA_BIT8                           8
#define MOUSE_KIND                          0x2
#define SLE_MOUSE_TASK_DELAY_300_MS         300
#define SLE_MOUSE_TASK_DELAY_1700_MS        1700
#define SLE_MOUSE_TASK_DELAY_20_MS          20

typedef struct usb_hid_mouse_report {
    mouse_key_t key;
    int8_t wheel;
    int16_t x;
    int16_t y;
} usb_hid_mouse_report_t;

sle_mouse_data_t g_mouse_notify_data = { 0 };
#pragma pack (1)
typedef struct {
    int16_t x;
    int16_t y;
    int8_t  button_mask     : 5; // assign 5bit for buttons
    int8_t  wheel    : 3; // assign 3bit for wheel
} low_latency_mouse_t;
#pragma pack ()

static mouse_sensor_oprator_t g_usb_hid_hs_mouse_operator = { 0 };
static usb_hid_mouse_report_t g_send_mouse_msg = { 0 };
static low_latency_mouse_t g_low_latency_mouse_msg = { 0 };
static qdec_config_t g_usb_qdec_config = QDEC_DEFAULT_CONFIG;
static int g_usb_mouse_hid_index;
osal_timer g_vbat_timer = { 0 };
extern errcode_t sle_low_latency_dongle_get_em_data(uint8_t* em_data);

static bool sle_send_msg(void)
{
    uint8_t conn_state = SLE_ACB_STATE_NONE;
    uint32_t pair_status = ERRCODE_SLE_FAIL;
    bool ssap_able = false;
    get_g_sle_mouse_server_conn_state(&conn_state);
    get_g_sle_mouse_pair_state(&pair_status);
    if (conn_state != SLE_ACB_STATE_CONNECTED || pair_status != ERRCODE_SLE_SUCCESS) {
        return false;
    }
    ssap_able = sle_hid_get_ssap_cccd_state();
    if (ssap_able == true) {
        sle_mouse_send_notify_indicate((uint8_t *)&g_mouse_notify_data, sizeof(g_mouse_notify_data));
        osal_msleep(SLE_MOUSE_TASK_DELAY_20_MS);
    }
    return true;
}

static void mouse_left_button_func(pin_t pin)
{
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.left_key = !uapi_gpio_get_val(pin);
    g_mouse_notify_data.button = g_send_mouse_msg.key.d8;
    sle_send_msg();
    uapi_gpio_clear_interrupt(pin);
}

static void mouse_right_button_func(pin_t pin)
{
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.right_key = !uapi_gpio_get_val(pin);
    g_mouse_notify_data.button = g_send_mouse_msg.key.d8;
    sle_send_msg();
    uapi_gpio_clear_interrupt(pin);
}

static void mouse_mid_button_func(pin_t pin)
{
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.mid_key = !uapi_gpio_get_val(pin);
    g_mouse_notify_data.button = g_send_mouse_msg.key.d8;
    sle_send_msg();
    uapi_gpio_clear_interrupt(pin);
}

static int qdec_report_callback(int argc, char *argv[])
{
    UNUSED(argv);
    g_send_mouse_msg.wheel = -argc;
    g_mouse_notify_data.wheel += g_send_mouse_msg.wheel;
    if (sle_send_msg()) {
        g_mouse_notify_data.wheel = 0;
    }
    osal_irq_clear(QDEC_IRQN);
    return 0;
}

static void mouse_io_init(void)
{
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_MOUSE_PIN_LEFT, PIN_IE_1);
    uapi_pin_set_ie(CONFIG_MOUSE_PIN_RIGHT, PIN_IE_1);
    uapi_pin_set_ie(CONFIG_MOUSE_PIN_MID, PIN_IE_1);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_LEFT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_RIGHT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_MID, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    gpio_select_core(CONFIG_MOUSE_PIN_LEFT, CORES_APPS_CORE);
    gpio_select_core(CONFIG_MOUSE_PIN_RIGHT, CORES_APPS_CORE);
    gpio_select_core(CONFIG_MOUSE_PIN_MID, CORES_APPS_CORE);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_LEFT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_RIGHT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_MID, GPIO_DIRECTION_INPUT);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_LEFT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_left_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_RIGHT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_right_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_MID, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_mid_button_func);

    uapi_pin_set_mode(CONFIG_MOUSE_PIN_QDEC_COMMON, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    gpio_select_core(CONFIG_MOUSE_PIN_QDEC_COMMON, CORES_APPS_CORE);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_QDEC_COMMON, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_MOUSE_PIN_QDEC_COMMON, 0);
}

static void vbat_sample_cb(unsigned long data)
{
    UNUSED(data);
    adc_port_gadc_entirely_open(CONFIG_MOUSE_ADC_VBAT_CH, false);
    int adc_value = adc_port_gadc_entirely_sample(CONFIG_MOUSE_ADC_VBAT_CH);
    adc_port_gadc_entirely_close(CONFIG_MOUSE_ADC_VBAT_CH);
    osal_printk("VBAT: %dmv\n", adc_value);
    osal_timer_start(&g_vbat_timer);
}

void vbat_adc_init(void)
{
    adc_port_gadc_entirely_open(CONFIG_MOUSE_ADC_VBAT_CH, true);
    int adc_value = adc_port_gadc_entirely_sample(CONFIG_MOUSE_ADC_VBAT_CH);
    osal_printk("VBAT: %dmv\n", adc_value);
    adc_port_gadc_entirely_close(CONFIG_MOUSE_ADC_VBAT_CH);

    g_vbat_timer.handler = vbat_sample_cb;
    g_vbat_timer.data = 0;
    g_vbat_timer.interval = VBAT_SAMPLE_INTERVAL_MS;
    osal_timer_init(&g_vbat_timer);
    osal_timer_start(&g_vbat_timer);
}

mouse_freq_t mouse_init(uint32_t sensor_id)
{
    mouse_io_init();
    vbat_adc_init();
    uapi_qdec_init(&g_usb_qdec_config);
    qdec_port_pinmux_init(CONFIG_MOUSE_PIN_QDEC_A, CONFIG_MOUSE_PIN_QDEC_B);
    uapi_qdec_register_callback(qdec_report_callback);
    uapi_qdec_enable();
    osal_printk("sensor:%d\r\n", sensor_id);
    g_usb_hid_hs_mouse_operator = get_mouse_sensor_operator(sensor_id);
    mouse_freq_t freq = g_usb_hid_hs_mouse_operator.init();
    osal_printk("g_usb_hid_hs_mouse_operator set frequency :%d\r\n", freq);
    osal_printk("mouse init done\r\n");
    return freq;
}

static errcode_t sle_mouse_key_set(int8_t *button_mask, int16_t *x, int16_t *y, int8_t *wheel)
{
    g_usb_hid_hs_mouse_operator.get_xy(x, y);
    *button_mask = g_send_mouse_msg.key.d8;
    *wheel = g_send_mouse_msg.wheel;
    g_send_mouse_msg.wheel = 0;
    return ERRCODE_SUCC;
}

void sle_mouse_get_key(void)
{
    int8_t button_mask = 0;
    int16_t x = 0;
    int16_t y = 0;
    int8_t wheel = 0;
    sle_mouse_key_set(&button_mask, &x, &y, &wheel);
    g_mouse_notify_data.button = button_mask;
    g_mouse_notify_data.x = x;
    g_mouse_notify_data.y = y;
    g_mouse_notify_data.wheel = wheel;
    sle_mouse_send_notify_indicate((uint8_t *)&g_mouse_notify_data, sizeof(g_mouse_notify_data));
}

uint8_t *sle_generate_mouse_data_get(uint16_t *len)
{
    int16_t x = 0;
    int16_t y = 0;
    g_usb_hid_hs_mouse_operator.get_xy(&x, &y);
    g_low_latency_mouse_msg.button_mask = g_send_mouse_msg.key.d8;
    g_low_latency_mouse_msg.x = x;
    g_low_latency_mouse_msg.y = y;
    g_low_latency_mouse_msg.wheel = g_send_mouse_msg.wheel;
    *len = 5; // 鼠标数据为5个字节
    return (uint8_t *)&g_low_latency_mouse_msg;
}

void sle_low_latency_mouse_app_init(void)
{
    sle_low_latency_tx_callbacks_t mouse_cbk;
    mouse_cbk.low_latency_tx_cb = sle_generate_mouse_data_get;
    sle_low_latency_tx_register_callbacks(&mouse_cbk);
    return ;
}

// bs22和bs26 dongle侧使用USB中断驱动，取数据回调需要挂在usb中断上，其余芯片挂在toggle中断上
#if (defined CONFIG_SAMPLE_SLE_DONGLE_8K) || (defined CONFIG_SAMPLE_SLE_DONGLE_4K_USB) || \
    (defined CONFIG_SAMPLE_SLE_DONGLE_2K_USB) || (defined CONFIG_SAMPLE_SLE_DONGLE_1K_USB) || \
    (defined CONFIG_SAMPLE_SLE_DONGLE_500HZ_USB) || (defined CONFIG_SAMPLE_SLE_DONGLE_250HZ_USB) || \
    (defined CONFIG_SAMPLE_SLE_DONGLE_125HZ_USB)
void dongle_cbk(uint8_t **data, uint16_t *length, uint8_t *device_index)
{
    static usb_hid_mouse_report_t mouse_message = { 0 }; // must be static or global variabal
    low_latency_mouse_t key_base = { 0 };
    uint16_t buff_size = sizeof(low_latency_mouse_t);
    uint16_t data_len = 0;
    sle_low_latency_rx_get_data((uint8_t *)&key_base, buff_size, &data_len);
    if ((data_len == 0) || (data_len != sizeof(low_latency_mouse_t))) {
        return;
    }

    mouse_message.key.d8 = key_base.button_mask;
    mouse_message.x = key_base.x;
    mouse_message.y = key_base.y;
    mouse_message.wheel = key_base.wheel;

    *data = (uint8_t *)&mouse_message;
    *length = sizeof(usb_hid_mouse_report_t);
    *device_index = g_usb_mouse_hid_index;
}
#else
void usb_sle_high_mouse_report(uint8_t lenth, uint8_t *data)
{
    if (lenth > sizeof(g_send_mouse_msg)) {
        osal_printk("Invalide data, len:%d\r\n", lenth);
        return;
    }
    low_latency_mouse_t *mouse_data = (low_latency_mouse_t *)data;
    g_send_mouse_msg.key.d8 = mouse_data->button_mask;
    g_send_mouse_msg.x = mouse_data->x;
    g_send_mouse_msg.y = mouse_data->y;
    g_send_mouse_msg.wheel = mouse_data->wheel;
    fhid_send_data(g_usb_mouse_hid_index, (char *)&g_send_mouse_msg, sizeof(usb_hid_mouse_report_t));
}
#endif

void sle_low_latency_dongle_init(int usb_hid_index)
{
    g_usb_mouse_hid_index = usb_hid_index;
    // bs22和bs26 dongle侧使用USB中断驱动，取数据回调需要挂在usb中断上，其余芯片挂在toggle中断上
#if (defined CONFIG_SAMPLE_SLE_DONGLE_8K) || (defined CONFIG_SAMPLE_SLE_DONGLE_4K_USB) || \
    (defined CONFIG_SAMPLE_SLE_DONGLE_2K_USB) || (defined CONFIG_SAMPLE_SLE_DONGLE_1K_USB) || \
    (defined CONFIG_SAMPLE_SLE_DONGLE_500HZ_USB) || (defined CONFIG_SAMPLE_SLE_DONGLE_250HZ_USB) || \
    (defined CONFIG_SAMPLE_SLE_DONGLE_125HZ_USB)
    usb_register_callback(&dongle_cbk);
#else
    // 依赖device层起sample中断, 根据产品形态决定
    sle_low_latency_rx_callbacks_t cbk_func = {NULL};
    cbk_func.low_latency_rx_cb = (low_latency_general_rx_callback)usb_sle_high_mouse_report;
    sle_low_latency_rx_register_callbacks(&cbk_func);
#endif
}
