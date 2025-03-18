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
#include "at_btc_product.h"
#include "sle_factory_manager.h"
#include "chip_core_irq.h"
#include "watchdog.h"
#include "timer.h"
#include "gpio.h"
#include "adc.h"
#include "adc_porting.h"
#include "pinctrl.h"
#include "app_mp_test_mode.h"

#define MOUSE_MODE_PCBA                     0
#define MOUSE_MODE_NORMAL                   1
#define ADC_AUTO_SAMPLE_TEST_TIMES          100
#define MP_TEST_IO_COUNT                    30
#define MP_TEST_IO_VALID_COUNT              20
#define MP_TEST_TIMER_2MS                   2
#define COMMAND_PCBA_STUB_INDEX             2
#define PCBA_TEST_SELCECT_INDEX             5
#define PCBA_TEST_CH_INDEX                  3
#define PCBA_TEST_ADDRESS_INDEX             4
#define PCBA_TEST_POWER_INDEX               6
#define PCBA_TEST_LOW_BAT_VOLMV             3000
#define PCBA_TEST_HIGH_BAT_VOLMV            4800
#define PCBA_TEST_DELAY_SLEEP_COUNT         100
#define PCBA_TEST_TIMER_PRIORITY            2
#define PCBA_TEST_TIMER1_2MS                2000
#define PCBA_TEST_TIMER0_500MS              500000
#define PCBA_TEST_TIMER0_30S                30000
#define PCBA_TEST_TX_PARAM_DATA_LEN         32
#define PCBA_TEST_TX_FORMAT                 2
#define PCBA_TEST_TX_RATE                   2
#define PCBA_TEST_TX_INTERVAL               50
#define PCBA_TEST_PARAM_100                 100
#define PCBA_TEST_SHIFT_8BITS               8
#define PCBA_TEST_SHIFT_16BITS              16
#define PCBA_TEST_SHIFT_24BITS              24

uint8_t	g_pcba_test_selcect = MOUSE_SELF_SEL_RF_IO | MOUSE_SELF_SEL_LOWBAT | MOUSE_SELF_SEL_FRE;
uint8_t	g_pcba_test_index = 0;
uint8_t	g_pcba_test_ch = 0;
uint8_t	g_pcba_test_address = 0;
sle_rf_power_t g_pcba_test_power = SLE_RF_POWER_LEVEL_0;
int16_t g_total_tx_rssi = 0;
uint8_t g_total_tx_rssi_times = 0;
int16_t g_total_ack_rssi = 0;
uint8_t g_total_ack_rssi_times = 0;
uint16_t g_io1_valid_count = 0;
uint16_t g_io2_valid_count = 0;
uint16_t g_io_high_count[32] = {0};
uint16_t g_io_low_count[32] = {0};
uint8_t	g_pcba_result = 0;
uint8_t g_tx_paket_counter = 0;
uint8_t	g_tx_paket_success_counter = 0;
uint16_t g_delay_sleep_count = 0;
uint32_t g_last_io_status = 0;
uint8_t g_ack_paket_index = 0;

bool g_pcba_timer_running = false;

bool g_time_balse_2ms_Flag = false;
static uint32_t g_pcba_timeout_timer = 0;

const sle_rf_power_t test_power_table[] = {
    SLE_RF_POWER_LEVEL_0,       /* 索引0 GFSK : -4dBm, QPSK/8PSK : -8dBm */
    SLE_RF_POWER_LEVEL_1,       /* 索引1 GFSK : -2dBm, QPSK/8PSK : -6dBm */
    SLE_RF_POWER_LEVEL_2,       /* 索引2 GFSK : 0dBm, QPSK/8PSK : -4dBm */
    SLE_RF_POWER_LEVEL_3,       /* 索引3 GFSK : 2dBm, QPSK/8PSK : -2dBm */
    SLE_RF_POWER_LEVEL_4,       /* 索引4 GFSK : 4dBm, QPSK/8PSK : 0dBm */
    SLE_RF_POWER_LEVEL_5,       /* 索引5 GFSK : 6dBm, QPSK/8PSK : 2dBm */
};

const uint8_t test_channel_table[16] = { 0, 8, 16, 24,
                                         32, 40, 48, 56,
                                         64, 72, 80, 4,
                                         12, 20, 28, 36
};

typedef struct {
    uint8_t pc_data[16];
    uint8_t paketlen;
    uint8_t readyflag;
} ack_pc_paket_t;

typedef struct {
    uint8_t mode;
    uint8_t* rx_buffer;
    ack_pc_paket_t ack_pc_paket;
    uint16_t bat_volmv;
    uint16_t adc_times_cnt;
    uint16_t bat_avs_adc;
} public_var_t;

public_var_t g_public_var = { 0 };
uint8_t g_pcba_test_step = 0;

// 2Hz中断事件回调处理
static void pcba_2hz_irq_handler(timer_index_t timer)
{
    unused(timer);
    uapi_gpio_toggle(IO_PCBA_PULSE_2HZ);
}

static void pcba_test_timer_handler(timer_index_t timer)
{
    unused(timer);
    if (g_pcba_timeout_timer >= MP_TEST_TIMER_2MS) {
        g_pcba_timeout_timer -= MP_TEST_TIMER_2MS;
    } else {
        g_public_var.mode = MOUSE_MODE_NORMAL;
    }

    g_time_balse_2ms_Flag = true;

    if (g_delay_sleep_count >= MP_TEST_TIMER_2MS) {
        g_delay_sleep_count -= MP_TEST_TIMER_2MS;
    } else {
        g_delay_sleep_count = 0;
    }
}

static gpio_level_t pcba_get_last_time_pin_level(uint32_t last_io_status, pin_t pin)
{
    return (gpio_level_t)(last_io_status & (1ULL << pin));
}

void pcba_test_command_start(void)
{
    uint8_t index = 0;
    g_pcba_test_selcect = g_public_var.rx_buffer[PCBA_TEST_SELCECT_INDEX];
    g_pcba_test_index = g_pcba_test_selcect & 0x0f;
    g_pcba_test_ch = g_public_var.rx_buffer[PCBA_TEST_CH_INDEX];     // TestChannelTable[PCBA_test_index];
    g_pcba_test_address = g_public_var.rx_buffer[PCBA_TEST_ADDRESS_INDEX]; // TestAddrTable[PCBA_test_index];

    index = g_public_var.rx_buffer[PCBA_TEST_POWER_INDEX];
    if (index > PCBA_TEST_SELCECT_INDEX) {
        index = PCBA_TEST_SELCECT_INDEX;
    }
    g_pcba_test_power = test_power_table[index];

    g_pcba_test_step = PCBA_TEST_STEP_START;

    g_public_var.ack_pc_paket.pc_data[0] = 1;  //  ACK_PC_SUCCESS
    g_public_var.ack_pc_paket.paketlen = 1;
    g_public_var.ack_pc_paket.readyflag = 1;
}

void pcba_test_command_rd_result(void)
{
    g_pcba_result = 0;
    int8_t mouse_rssi = -100;
    int8_t ack_rssi = -100;
    uint32_t tempio = 0;
    /* 比较IO自检测与RF结果 */
    if (g_pcba_test_selcect & MOUSE_SELF_SEL_RF_IO) {
        mouse_rssi = g_total_tx_rssi / g_total_tx_rssi_times;
        ack_rssi = g_total_ack_rssi / g_total_ack_rssi_times;
        if ((g_io1_valid_count > MP_TEST_IO_VALID_COUNT) && (g_io2_valid_count > MP_TEST_IO_VALID_COUNT)) {
            g_pcba_result |= MOUSE_SELF_SEL_RF_IO;
        }
    }
    g_public_var.bat_volmv = g_public_var.bat_volmv / g_public_var.adc_times_cnt;
    /* 比较低电范围 */
    if (g_pcba_test_selcect & MOUSE_SELF_SEL_LOWBAT) {
        if ((g_public_var.bat_volmv >= PCBA_TEST_LOW_BAT_VOLMV) &&
            (g_public_var.bat_volmv < PCBA_TEST_HIGH_BAT_VOLMV)) {
            g_pcba_result |= MOUSE_SELF_SEL_LOWBAT;
        }
    }
    /* 频偏由测架测，直接返回1 */
    if (g_pcba_test_selcect & MOUSE_SELF_SEL_FRE) {
        g_pcba_result |= MOUSE_SELF_SEL_FRE;
    }

    g_ack_paket_index = 0;
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = 1;  //  ACK_PC_SUCCESS
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = g_pcba_result | g_pcba_test_index;
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = g_tx_paket_success_counter; // 返回发包成功计数
    /* 由特殊接收器返回给被测试的接器的发包功率，可用来测发射功率 */
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = mouse_rssi;
    /* 由被测的接收器测到的ACK包的RSSI值，可用来测接收灵敏度 */
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = ack_rssi;
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = g_io1_valid_count & 0x00ff;
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = (g_io1_valid_count >> PCBA_TEST_SHIFT_8BITS) &
                                                            0x00ff;
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = g_io2_valid_count & 0x00ff;
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = (g_io2_valid_count >> PCBA_TEST_SHIFT_8BITS) &
                                                            0x00ff;
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = g_public_var.bat_volmv & 0x00ff;
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = (g_public_var.bat_volmv >> PCBA_TEST_SHIFT_8BITS) &
                                                            0x00ff;
    tempio = 0;
    for (uint8_t i = 0; i < PIN_NONE; i++) {
        if ((g_io_high_count[i] > MP_TEST_IO_COUNT) && (g_io_low_count[i] > MP_TEST_IO_COUNT)) {
            tempio |= (0x00000001<<i);
        }
    }
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = tempio & 0x000000ff;
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = (tempio >> PCBA_TEST_SHIFT_8BITS) & 0x000000ff;
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = (tempio >> PCBA_TEST_SHIFT_16BITS) & 0x000000ff;
    g_public_var.ack_pc_paket.pc_data[g_ack_paket_index++] = (tempio >> PCBA_TEST_SHIFT_24BITS) & 0x000000ff;
    g_public_var.ack_pc_paket.paketlen = g_ack_paket_index;

    g_public_var.ack_pc_paket.readyflag = 1;
}

void pcba_test_usb_command_pro(uint8_t *buff)
{
    g_public_var.rx_buffer = buff;
    /* 数据字节1低半字节=频道与RF地址，高半字字=自检项目选择 */
    switch (g_public_var.rx_buffer[COMMAND_PCBA_STUB_INDEX]) {
        case COMMAND_PCBA_SUB_START:
            pcba_test_command_start();
            break;
        case COMMAND_PCBA_SUB_RD_RESULT:
            pcba_test_command_rd_result();
            break;
        case COMMAND_PCBA_SUB_SLEEP:
            g_pcba_test_step = PCBA_TEST_STEP_SLEEP_CUR;
            g_delay_sleep_count = PCBA_TEST_DELAY_SLEEP_COUNT;

            g_public_var.ack_pc_paket.pc_data[0] = 1;   //  ACK_PC_SUCCESS
            g_public_var.ack_pc_paket.paketlen = 1;
            g_public_var.ack_pc_paket.readyflag = 1;
            break;

        default:
            g_public_var.ack_pc_paket.pc_data[0] = 0;   //  ACK_PC_FAIL
            g_public_var.ack_pc_paket.paketlen = 1;
            g_public_var.ack_pc_paket.readyflag = 1;
            break;
    }
}

void app_pcba_test_step_io(void)
{
    uint32_t test_io_level_val = 0;
    for (uint8_t i = 0; i < PIN_NONE; i++) {
        test_io_level_val += uapi_gpio_get_val(i);
    }
    // 不确定的值，根据具体管脚确认
    if (test_io_level_val == 0x3FF) {
        g_io1_valid_count++;
    } else if (test_io_level_val == 0xFFC00) {
        g_io2_valid_count++;
    }

    for (uint8_t i = 0; i < PIN_NONE; i++) {
        if (pcba_get_last_time_pin_level(g_last_io_status, i) != (uapi_gpio_get_val(i) << i)) {
            if (uapi_gpio_get_val(i) == GPIO_LEVEL_LOW) {
                // 是具体管脚个数还是第几个？
                g_io_low_count[i]++;
            } else {
                g_io_high_count[i]++;
            }
        }
    }
}

void app_pcba_test_step_adc(void)
{
    uapi_adc_init(ADC_CLOCK_NONE);
    uint8_t gadc_channel = GADC_CHANNEL_0;
    adc_port_gadc_entirely_open(gadc_channel, true);
    for (int i = 0; i < ADC_AUTO_SAMPLE_TEST_TIMES; i++) {
        g_public_var.bat_volmv =  adc_port_gadc_entirely_sample(gadc_channel);
        g_public_var.adc_times_cnt++;
    }
    adc_port_gadc_entirely_close(gadc_channel);
}

void app_pcba_test_step_sel_fre(void)
{
    uapi_gpio_set_dir(IO_PCBA_PULSE_2HZ, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(IO_PCBA_PULSE_2HZ, GPIO_LEVEL_LOW);

    timer_irq_info_t irq_info;
    irq_info.irq = TIMER_0_IRQN;
    irq_info.priority = PCBA_TEST_TIMER_PRIORITY;
    uapi_timer_start_high_precision(TIMER_INDEX_0, TIMER_MODE_PERIODIC, PCBA_TEST_TIMER0_500MS,
                                    &irq_info, pcba_2hz_irq_handler);
}

void app_pcba_test_step_sel_rf(void)
{
    sle_rf_tx_param_args_t param = { 0 };
    g_pcba_test_ch = test_channel_table[g_pcba_test_index];
    param.tx_freq = g_pcba_test_ch;
    param.tx_power = g_pcba_test_power;
    param.test_data_len = PCBA_TEST_TX_PARAM_DATA_LEN;
    param.pk_payload_type = 0;
    param.tx_phy = 1;
    param.tx_format = PCBA_TEST_TX_FORMAT;
    param.tx_rate = PCBA_TEST_TX_RATE;
    param.tx_pilot_ratio = 0;
    param.tx_polar_r = 0;
    param.tx_interval = PCBA_TEST_TX_INTERVAL;
    bt_at_sle_rf_tx_cmd(&param);

    g_pcba_result = 0;
    g_tx_paket_counter = PCBA_TEST_PARAM_100;
    g_tx_paket_success_counter = 0;
    g_total_tx_rssi = (-PCBA_TEST_PARAM_100);
    g_total_tx_rssi_times = 1;
    g_total_ack_rssi = (-PCBA_TEST_PARAM_100);
    g_total_ack_rssi_times = 1;
    g_time_balse_2ms_Flag = false;
    g_pcba_test_step = PCBA_TEST_STEP_IO_FRE_WRK_CUR_PAKET;
}

void app_pcba_test_init(void)
{
/* 所有管脚初始化输入 */
    for (uint8_t i = 0; i < PIN_NONE; i++) {
        uapi_gpio_set_dir(i, GPIO_DIRECTION_INPUT);
        uapi_pin_set_pull(i, PIN_PULL_NONE);
    }
    if (g_pcba_timer_running) {
        uapi_timer_stop_high_precision(TIMER_INDEX_1);
        g_pcba_timer_running = false;
    }

    if (g_pcba_timer_running == false) {
        timer_irq_info_t irq_info;
        irq_info.irq = TIMER_1_IRQN;
        irq_info.priority = PCBA_TEST_TIMER_PRIORITY;
        uapi_timer_start_high_precision(TIMER_INDEX_1, TIMER_MODE_PERIODIC, PCBA_TEST_TIMER1_2MS, &irq_info,
                                        pcba_test_timer_handler);
        g_pcba_timer_running = true;
    }
    g_public_var.bat_avs_adc 	= 0;
    g_public_var.bat_volmv  	= 0;
    g_public_var.adc_times_cnt = 0;

    g_pcba_test_step = PCBA_TEST_STEP_WAIT_START;
    g_pcba_timeout_timer = PCBA_TEST_TIMER0_30S;
}

void app_pcba_test_mode(void)
{
    /* 初始化 */
    app_pcba_test_init();

    while (g_public_var.mode == MOUSE_MODE_PCBA) {
        uapi_watchdog_kick();

        /* 等待接收USB送过来的频道与地址及测试项目的开始命令 */
        switch (g_pcba_test_step) {
            case PCBA_TEST_STEP_WAIT_START:
                break;

            case PCBA_TEST_STEP_START:
                if (g_pcba_test_selcect & MOUSE_SELF_SEL_RF_IO) {
                    app_pcba_test_step_sel_rf();
                }
                // 需测频偏的话，用Timer0，P0.3输出2Hz信号
                if (g_pcba_test_selcect & MOUSE_SELF_SEL_FRE) {
                    app_pcba_test_step_sel_fre();
                }
                if (g_pcba_test_selcect & MOUSE_SELF_SEL_LOWBAT) {
                    // adc电量测试 待实现
                    app_pcba_test_step_adc();
                }

                g_io1_valid_count = 0;
                g_io2_valid_count = 0;
                for (uint8_t i = 0; i < PIN_NONE; i++) {
                    g_io_low_count[i] = 0;
                    g_io_high_count[i] = 0;
                }
                g_last_io_status = 0;
                break;
            case PCBA_TEST_STEP_IO_FRE_WRK_CUR_PAKET:

                if (g_time_balse_2ms_Flag) {
                    g_time_balse_2ms_Flag = false;
                    if (g_pcba_test_selcect & MOUSE_SELF_SEL_RF_IO) {
                        // 待实现
                    }
                    // IO开短路检测
                    if (g_pcba_test_selcect & MOUSE_SELF_SEL_RF_IO) {
                        app_pcba_test_step_io();
                    }
                }
                break;
            // 进入睡眠电流检测
            case PCBA_TEST_STEP_SLEEP_CUR:
                // 待实现
                if (!g_delay_sleep_count) {
                }
                break;
            default:
                break;
        }
        osal_msleep(1);
    }
}