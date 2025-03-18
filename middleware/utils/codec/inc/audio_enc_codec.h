/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Application core os initialize interface header for standard \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-27, Create file. \n
 */
#ifndef AUDIO_ENC_CODEC_H
#define AUDIO_ENC_CODEC_H

#include "audio_common.h"

typedef struct {
    bool msbc;
    int16_t channel_mode;       /* JOINT_STEREO/STEREO/DUAL/MONO */
    int16_t sample_rate;        /* 16, 32, 44.1 or 48 */
    int16_t allocation_method;  /* loudness or SNR */
    int16_t num_of_subbands;    /* 4, 8 */
    int16_t num_of_blocks;      /* 4, 8, 12 or 16 */
    int16_t bit_pool;           /* 16*numOfSb for mono & dual; */
} sbc_enc_config;

typedef struct {
    int param_flag;
    sbc_enc_config sbc_enc_param;
} aenc_param_cfg;

typedef struct {
    bool eos;
    uint32_t frame_pms;
    uint32_t frame_size;

    uint32_t max_bit_out_size;
    uint32_t not_enough_data_cnt;

    ha_codec_id codec_id;
    cps_profile profile;
    audio_pcm_format pcm_attr;

    void *h_enc;
    ha_codec_encode *codec;
    ha_codec_enc_in_packet in_pkt;
    ha_codec_enc_out_packet out_pkt;

    aenc_param_cfg sbc_enc_param_ctrl;
} audio_aenc_inst;

typedef struct {
    ha_codec_id codec_id;
    ha_codec_enc_param open_param; /* Encoder open param */
} aenc_attr;

/* sbc definition param define */
#define GET_DEFAULT 0
#define USE_DEFINITION 1
#ifdef SBC_USE_DEFINITION_PARAM
#define SBC_PARAM_FLAG USE_DEFINITION
#else
#define SBC_PARAM_FLAG GET_DEFAULT
#endif

#define SBC_ALLOC_LOUDNESS 0
#define SBC_ALLOC_SNR 1

#define SUBBANDS_8 8
#define SUBBANDS_4 4

#define SBC_ENC_MONO 0
#define SBC_ENC_DUAL 1
#define SBC_ENC_STEREO 2
#define SBC_ENC_JIN_ST 3

#define SBC_BLOCK_4 4
#define SBC_BLOCK_8 8
#define SBC_BLOCK_12 12
#define SBC_BLOCK_16 16

#define SBC_SF16000 0
#define SBC_SF32000 1
#define SBC_SF44100 2
#define SBC_SF48000 3
#define SBC_CAPS_DEFAULT_BIT_POOL 14
#define MSBC_CAPS_DEFAULT_NUM_OF_BLOCKS 15

int32_t aenc_encode_frame(void);
int32_t aenc_open_codec(int32_t codecid, ha_codec_enc_param *open_param);
void aenc_close_codec(void);
uint32_t audio_get_encoder_consume_data_size(void);
uint32_t audio_get_encoder_product_data_size(void);
void audio_set_encoder_consume_data_size(uint32_t data_size);
void audio_encoder_fifo_info_display(void);
#endif