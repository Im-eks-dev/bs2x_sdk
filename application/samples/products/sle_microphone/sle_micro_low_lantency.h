/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: SLE MICRO sample of low latency. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-8, Create file. \n
 */
#ifndef SLE_MICRO_LOW_LANTENCY_H
#define SLE_MICRO_LOW_LANTENCY_H

// low lantency 发送队列条目数
#define SLE_LL_SEND_QUEUE_COUNT 6
#define SLE_UAC_QUEUE_COUNT 6

#ifdef CONFIG_SLE_CODEC_DATA_TRANSMISSION
#define SLE_LL_SEND_DATA_NUMBER 68
#define SLE_UAC_SEND_DATA_NUMBER 68
#else
#define SLE_LL_SEND_DATA_NUMBER 112
#define SLE_UAC_SEND_DATA_NUMBER 112
#endif

#pragma pack(1)
typedef struct {
    // 麦克风数据区分标识
    uint8_t u8_mic;
    // 数据检验标识位（丢包、重复包)
    uint16_t u16_pk_cnt;
    // 数据位
    uint8_t auc_pcm_data[SLE_LL_SEND_DATA_NUMBER];
} st_ll_data;

typedef struct {
    uint16_t u16_len;
    st_ll_data st_data;
} st_ll_send_pack;

/**
* @Description: UAC传递数据结构
* @Date:2024/05/07 19:45:27
*/
typedef struct {
    uint16_t u16_len;
    uint8_t auc_pcm_data[SLE_UAC_SEND_DATA_NUMBER];
} st_uac_pack;

typedef struct {
    uint8_t u8_tail;
    uint8_t u8_head;
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_SERVER)
    uint16_t u16_pk_cnt;
    uint16_t u16_last_pk_cnt;
    st_ll_send_pack ast_send[SLE_LL_SEND_QUEUE_COUNT];
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_DONGLE)
    uint16_t u16_mic1_last_cnt;
    uint16_t u16_mic2_last_cnt;
    st_uac_pack ast_pack[SLE_UAC_QUEUE_COUNT];
#endif
} st_ll_send_queue;
#pragma pack()

void sle_micro_low_latency_tx_init(low_latency_general_tx_callback low_latency_general_tx_cb);

void sle_micro_low_latency_rx_init(low_latency_general_rx_callback low_latency_general_rx_cb);

#endif