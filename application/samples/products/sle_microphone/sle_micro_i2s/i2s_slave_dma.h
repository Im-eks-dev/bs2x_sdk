/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE MICRO sample of I2s. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-12, Create file. \n
 */
#ifndef I2S_SLAVE_DMA_H
#define I2S_SLAVE_DMA_H

#include "soc_osal.h"
#include "common_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif /* __cplusplus */
#endif /* __cplusplus */

#define CONFIG_I2S_SLAVE_DMA_TRANSFER_EVENT 1
#define I2S_SLAVE_RECV_QUEUE_COUNT 4

#ifdef CONFIG_SLE_CODEC_DATA_TRANSMISSION
#define I2S_TRANSFER_LEN_OF_DMA 128
#else
#define I2S_TRANSFER_LEN_OF_DMA 56
#endif

#pragma pack(1)
    typedef struct {
        uint16_t u16_len;
        uint16_t au16_pcm_data[I2S_TRANSFER_LEN_OF_DMA];
    } st_i2s_pack;

    typedef struct {
        /* data */
        uint16_t u16_send_counts;
        uint8_t u8_tail;
        uint8_t u8_head;
        st_i2s_pack ast_pack[I2S_SLAVE_RECV_QUEUE_COUNT];
    } st_i2s_recv;
#pragma pack()

void i2s_slave_dma_init(void);

osal_event *get_i2s_slave_osal_event_point(void);

st_i2s_pack *scan_i2s_recv_data(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif