/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE MICRO sample of i2s. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-12, Create file. \n
 */

#include "i2s_slave_dma.h"
#include "sle_connection_manager.h"
#include "soc_osal.h"
#include "../sle_micro_server/sle_micro_server.h"
#include "gpio.h"
#include "pinctrl.h"
#include "i2s.h"
#include "i2c.h"
#include "tcxo.h"
#include "watchdog.h"
#include "hal_dma.h"
#include "hal_sio.h"

// pin define
#define CONFIG_I2S_SLAVE_DMA_SCLK_PIN S_MGPIO13
#define CONFIG_I2S_SLAVE_DMA_SCLK_PIN_MODE HAL_PIO_I2S_SCLK

#define CONFIG_I2S_SLAVE_DMA_WS_PIN S_MGPIO14
#define CONFIG_I2S_SLAVE_DMA_WS_PIN_MODE HAL_PIO_I2S_WS

#define CONFIG_I2S_SLAVE_DMA_DO_PIN S_MGPIO15
#define CONFIG_I2S_SLAVE_DMA_DO_PIN_MODE HAL_PIO_I2S_DOUT

#define CONFIG_I2S_SLAVE_DMA_DI_PIN S_MGPIO16
#define CONFIG_I2S_SLAVE_DMA_DI_PIN_MODE HAL_PIO_I2S_DIN

// para define
#define CONFIG_I2S_SLAVE_DMA_DRIVER_MODE SLAVE
#define CONFIG_I2S_SLAVE_DMA_TRANSFER_MODE STD_MODE
#define CONFIG_I2S_SLAVE_DMA_DATA_WIDTH SIXTEEN_BIT
#define CONFIG_I2S_SLAVE_DMA_CHN_NUM TWO_CH
#define CONFIG_I2S_SLAVE_DMA_DIV_NUMBER 32
#define CONFIG_I2S_SLAVE_DMA_NUM_OF_CHN 2

#define CONFIG_I2S_SLAVE_DMA_TX_INT_THRESHOLD 7
#define CONFIG_I2S_SLAVE_DMA_RX_INT_THRESHOLD 1

#define CONFIG_I2S_SLAVE_DMA_SRC_WIDTH HAL_DMA_TRANSFER_WIDTH_16
#define CONFIG_I2S_SLAVE_DMA_DEST_WIDTH HAL_DMA_TRANSFER_WIDTH_16

#define CONFIG_I2S_SLAVE_DMA_SRC_INCREMENT HAL_DMA_ADDRESS_INC_NO_CHANGE
#define CONFIG_I2S_SLAVE_DMA_DEST_INCREMENT HAL_DMA_ADDRESS_INC_INCREMENT

#define CONFIG_I2S_TRANS_SRC_ADDR 0x52030058

static st_i2s_recv g_st_recv = {0};
osal_event g_i2s_slave_dma_id;

void i2s_slave_dma_pinmux(void)
{
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_I2S_SLAVE_DMA_SCLK_PIN, PIN_IE_1);
    uapi_pin_set_ie(CONFIG_I2S_SLAVE_DMA_WS_PIN, PIN_IE_1);
    uapi_pin_set_ie(CONFIG_I2S_SLAVE_DMA_DI_PIN, PIN_IE_1);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
    uapi_pin_set_mode(CONFIG_I2S_SLAVE_DMA_SCLK_PIN, CONFIG_I2S_SLAVE_DMA_SCLK_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2S_SLAVE_DMA_WS_PIN, CONFIG_I2S_SLAVE_DMA_WS_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2S_SLAVE_DMA_DO_PIN, CONFIG_I2S_SLAVE_DMA_DO_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2S_SLAVE_DMA_DI_PIN, CONFIG_I2S_SLAVE_DMA_DI_PIN_MODE);
}
static int32_t i2s_slave_dma_add_lli_node(uint8_t index, dma_channel_t dma_channel, dma_transfer_cb_t trans_done)
{
    dma_ch_user_peripheral_config_t transfer_config;

    transfer_config.src = CONFIG_I2S_TRANS_SRC_ADDR;
    transfer_config.dest = (uint32_t)(uintptr_t)(&g_st_recv.ast_pack[index].au16_pcm_data[0]);
    transfer_config.transfer_num = (uint16_t)I2S_TRANSFER_LEN_OF_DMA;
    transfer_config.src_handshaking = HAL_DMA_HANDSHAKING_I2S_RX;
    transfer_config.dest_handshaking = 0;
    transfer_config.trans_type = HAL_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA;
    transfer_config.trans_dir = HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM;
    transfer_config.priority = 0;
    transfer_config.src_width = CONFIG_I2S_SLAVE_DMA_SRC_WIDTH;
    transfer_config.dest_width = CONFIG_I2S_SLAVE_DMA_DEST_WIDTH;
    transfer_config.burst_length = 0;
    transfer_config.src_increment = CONFIG_I2S_SLAVE_DMA_SRC_INCREMENT;
    transfer_config.dest_increment = CONFIG_I2S_SLAVE_DMA_DEST_INCREMENT;
    transfer_config.protection = HAL_DMA_PROTECTION_CONTROL_BUFFERABLE;

    errcode_t ret = uapi_dma_configure_peripheral_transfer_lli(dma_channel, &transfer_config, trans_done);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Configure the DMA fail. %x\r\n", "i2s dma lli", ret);
        return 1;
    }
    return 0;
}

static void i2s_slave_dma_trans_done_callback(uint8_t intr, uint8_t channel, uintptr_t arg)
{
    unused(channel);
    unused(arg);
    switch (intr) {
        case HAL_DMA_INTERRUPT_TFR:
            g_st_recv.u8_tail = (g_st_recv.u8_tail + 1) % I2S_SLAVE_RECV_QUEUE_COUNT;
            g_st_recv.ast_pack[g_st_recv.u8_tail].u16_len = I2S_TRANSFER_LEN_OF_DMA;
            if (osal_event_write(&g_i2s_slave_dma_id, CONFIG_I2S_SLAVE_DMA_TRANSFER_EVENT) != OSAL_SUCCESS) {
                osal_printk("osal_event_write fail!\r\n");
                return;
            }
            break;
        case HAL_DMA_INTERRUPT_ERR:
            osal_printk("i2s DMA transfer error.\r\n");
            break;
        default:
            break;
    }
}

void i2s_slave_dma_init(void)
{
    // slave dma事件初始化
    if (osal_event_init(&g_i2s_slave_dma_id) != OSAL_SUCCESS) {
        return;
    }
    // 数据接收初始化
    memset_s(&g_st_recv, sizeof(st_i2s_recv), 0x00, sizeof(st_i2s_recv));
    g_st_recv.u8_head = 0;
    g_st_recv.u8_tail = 0;
    // I2S引脚初始化
    i2s_slave_dma_pinmux();
    // I2S外设初始化
    uapi_i2s_init(0, NULL);
    // I2S的配置参数
    i2s_config_t st_config = {
        .drive_mode = CONFIG_I2S_SLAVE_DMA_DRIVER_MODE,
        .transfer_mode = CONFIG_I2S_SLAVE_DMA_TRANSFER_MODE,
        .data_width = CONFIG_I2S_SLAVE_DMA_DATA_WIDTH,
        .channels_num = CONFIG_I2S_SLAVE_DMA_CHN_NUM,
        .timing = NONE_MODE,
        .clk_edge = RISING_EDGE,
        .div_number = CONFIG_I2S_SLAVE_DMA_DIV_NUMBER,
        .number_of_channels = CONFIG_I2S_SLAVE_DMA_NUM_OF_CHN,
    };
    // i2s dma配置参数
    i2s_dma_attr_t st_attr = {
        .tx_dma_enable = 0,
        .tx_int_threshold = CONFIG_I2S_SLAVE_DMA_TX_INT_THRESHOLD,
        .rx_dma_enable = 1,
        .rx_int_threshold = CONFIG_I2S_SLAVE_DMA_RX_INT_THRESHOLD,
    };
    // 设置i2s的参数
    uapi_i2s_set_config(SIO_BUS_0, &st_config);
    // 设置dma的参数
    uapi_i2s_dma_config(SIO_BUS_0, &st_attr);

    /* DMA init. */
    uapi_dma_init();
    // 开启dma
    uapi_dma_open();
    g_st_recv.u8_tail = 0;
    g_st_recv.ast_pack[g_st_recv.u8_tail].u16_len = I2S_TRANSFER_LEN_OF_DMA;

    // 配置链式dma
    dma_channel_t dma_channel = uapi_dma_get_lli_channel(0, HAL_DMA_HANDSHAKING_MAX_NUM);
    for (uint8_t i = 0; i < I2S_SLAVE_RECV_QUEUE_COUNT; i++) {
        if (i2s_slave_dma_add_lli_node(i, dma_channel, i2s_slave_dma_trans_done_callback) != 0) {
            osal_printk("i2s_slave_dma_add_lli_node fail!\r\n");
            return;
        }
    }
    // 启动链式dma
    if (uapi_dma_enable_lli(dma_channel, i2s_slave_dma_trans_done_callback, (uintptr_t)NULL) == ERRCODE_SUCC) {
        osal_printk("dma enable lli memory transfer succ!\r\n");
    }
    // 使能sio的接收
    hal_sio_set_rx_enable(SIO_BUS_0, 1);

    osal_printk("sle_micro_i2s init OK\r\n");
    return;
}

osal_event *get_i2s_slave_osal_event_point(void)
{
    return &g_i2s_slave_dma_id;
}

st_i2s_pack *scan_i2s_recv_data(void)
{
    if (g_st_recv.u8_head == g_st_recv.u8_tail) {
        return NULL;
    }
    st_i2s_pack *pst_pack = &g_st_recv.ast_pack[g_st_recv.u8_head];
    g_st_recv.u8_head = (g_st_recv.u8_head + 1) % I2S_SLAVE_RECV_QUEUE_COUNT;
    return pst_pack;
}
