/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE MICRO Codec Header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-23, Create file. \n
 */
#ifndef SLE_MICRO_CODEC_H
#define SLE_MICRO_CODEC_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

// 16k采样率：各类型编码前输入数据长度与解码后输出数据长度
#define ENC_FREAM_16K_SBC_SIZE 256
#define ENC_FREAM_16K_MSBC_SIZE 240
#define ENC_FREAM_16K_OPUS_SIZE 320
#define ENC_FREAM_16K_L2HC_SIZE 320

// 48k采样率：各类型编码前输入数据长度与解码后输出数据长度
#define ENC_FREAM_48K_SBC_SIZE 256
#define ENC_FREAM_48K_MSBC_SIZE 240
#define ENC_FREAM_48K_OPUS_SIZE 960
#define ENC_FREAM_48K_L2HC_SIZE 960

// 16k采样率：各类型编码后输出数据长度与解码前输入数据长度
#define DEC_FREAM_16K_SBC_SIZE 68
#define DEC_FREAM_16K_MSBC_SIZE 60
#define DEC_FREAM_16K_OPUS_SIZE 100
#define DEC_FREAM_16K_L2HC_SIZE 84

// 48k采样率：各类型编码后输出数据长度与解码前输入数据长度
#define DEC_FREAM_48K_SBC_SIZE 68
#define DEC_FREAM_48K_MSBC_SIZE 60
#define DEC_FREAM_48K_OPUS_SIZE 120
#define DEC_FREAM_48K_L2HC_SIZE 84

void sle_micro_codec_init(void);

void sle_micro_codec_deinit(void);

uint32_t sle_micro_codec_encode(uint8_t *pcm_data, uint8_t **enc_data);

uint32_t sle_micro_codec_decode(uint8_t *enc_data, uint8_t **pcm_data);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif