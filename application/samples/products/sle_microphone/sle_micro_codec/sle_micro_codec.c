/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE MICRO Codec Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-23, Create file. \n
 */
#include "common_def.h"
#include "osal_debug.h"
#include "audio_enc_codec.h"
#include "audio_dec_codec.h"
#include "audio_profile_calc.h"
#include "sle_micro_codec.h"

typedef struct sle_micro_codec_inst {
    uint32_t codec_id;
    uint32_t sample_rate;
    uint32_t bit_depth;
    uint32_t channels;
} sle_micro_codec_inst_t;

static sle_micro_codec_inst_t g_sle_micro_codec_inst = {
    .codec_id = HA_CODEC_ID_SBC,
    .sample_rate = HA_CODEC_SAMPLE_RATE_48K,
    .bit_depth = HA_CODEC_BIT_DEPTH_16,
    .channels = HA_CODEC_CH_NUM_1
};

void sle_micro_codec_set_enc_size(void)
{
    uint32_t data_size;
    data_size = ENC_FREAM_48K_SBC_SIZE * g_sle_micro_codec_inst.channels;
    audio_set_encoder_consume_data_size(data_size);
}

void sle_micro_codec_set_dec_size(void)
{
    uint32_t data_size;
    data_size = DEC_FREAM_48K_SBC_SIZE * g_sle_micro_codec_inst.channels;
    audio_set_decoder_consume_data_size(data_size);
}

void sle_micro_codec_init(void)
{
    int32_t ret;
    sle_micro_codec_set_enc_size();
    sle_micro_codec_set_dec_size();
    ha_codec_enc_param enc_open_param;
    enc_open_param.channels = g_sle_micro_codec_inst.channels;
    enc_open_param.bit_depth = g_sle_micro_codec_inst.bit_depth;
    enc_open_param.sample_rate = g_sle_micro_codec_inst.sample_rate;
    enc_open_param.private_data = NULL;
    enc_open_param.private_data_size = 0;
    ret = aenc_open_codec(g_sle_micro_codec_inst.codec_id, &enc_open_param);
    if (ret != AUDIO_SUCCESS) {
        osal_printk("open encoder codec fail %#x\n", ret);
        aenc_close_codec();
        return;
    }

    ha_codec_dec_param dec_open_param;
    dec_open_param.channels = g_sle_micro_codec_inst.channels;
    dec_open_param.bit_depth = g_sle_micro_codec_inst.bit_depth;
    dec_open_param.sample_rate = g_sle_micro_codec_inst.sample_rate;
    dec_open_param.private_data = NULL;
    dec_open_param.private_data_size = 0;
    ret = adec_open_codec(g_sle_micro_codec_inst.codec_id, &dec_open_param);
    if (ret != AUDIO_SUCCESS) {
        osal_printk("open decoder codec fail %#x\n", ret);
        aenc_close_codec();
        adec_close_codec();
        return;
    }
}

void sle_micro_codec_deinit(void)
{
    aenc_close_codec();
    adec_close_codec();
}


uint32_t sle_micro_codec_encode(uint8_t *pcm_data, uint8_t **enc_data)
{
    int32_t ret;
    ring_buffer_t *enc_in_fifo = audio_get_in_fifo(HA_CODEC_ENDODER);
    ring_buffer_t *enc_out_fifo = audio_get_out_fifo(HA_CODEC_ENDODER);

    /* audio codec begin */
    ret = write_data_into_fifo(pcm_data, enc_in_fifo, audio_get_encoder_consume_data_size());
    if (ret != AUDIO_SUCCESS) {
        osal_printk("write data err %d\n", ret);
        return 0;
    }

    ret = aenc_encode_frame();
    if (ret != 0) {
        osal_printk("aenc_encode_frame %d\n", ret);
        return 0;
    }

    uint32_t enc_product_data_size = audio_get_encoder_product_data_size();
    ret = read_data_from_fifo(enc_data, enc_out_fifo, enc_product_data_size);
    if (ret != AUDIO_SUCCESS) {
        return 0;
    }

    read_data_from_fifo_finish(enc_out_fifo, enc_product_data_size);

    return enc_product_data_size;
}

uint32_t sle_micro_codec_decode(uint8_t *enc_data, uint8_t **pcm_data)
{
    int32_t ret;
    ring_buffer_t *dec_in_fifo = audio_get_in_fifo(HA_CODEC_DECODER);
    ring_buffer_t *dec_out_fifo = audio_get_out_fifo(HA_CODEC_DECODER);
    write_data_into_fifo(enc_data, dec_in_fifo, audio_get_decoder_consume_data_size());
    ret = adec_decode_frame(0);
    if (ret != 0) {
        osal_printk("adec_decode_frame %d\n", ret);
        return 0;
    }
    
    uint32_t product_data_size = audio_get_decoder_product_data_size();
    ret = read_data_from_fifo(pcm_data, dec_out_fifo, product_data_size);
    if (ret != AUDIO_SUCCESS) {
        return 0;
    }
    read_data_from_fifo_finish(dec_out_fifo, product_data_size);
    return product_data_size;
}