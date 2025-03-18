/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: IR NEC Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-10, Create file. \n
 */
#include "gpio.h"
#include "tcxo.h"
#include "ir_tx.h"
#include "osal_addr.h"
#if defined(CONFIG_SAMPLE_SUPPORT_IR_STUDY)
#include "ir_study.h"
#endif /* CONFIG_SAMPLE_SUPPORT_IR_STUDY */
#include "pwm.h"
#include "ir_nec.h"

#define PATTERN_LEN          68
#define IR_RECEIVE_PIN       CONFIG_SAMPLE_IR_STUDY_PIN
#define NEC_FREQ             38000
#define NEC_START_HIGH       9000
#define NEC_START_LOW        (-4500)
#define NEC_END_HIGH         560
#define NEC_END_LOW          (-20000)
#define NEC_ONE_H            560
#define NEC_ONE_L            (-1690)
#define NEC_ZERO_H           560
#define NEC_ZERO_L           (-560)
#define PARAM_2              2
#define PARAM_18             18
#define PARAM_34             34
#define PARAM_50             50
#define PARAM_66             66
#define PARAM_67             67
#define BIT_LEN_8            8
#define BIT_LEN_32           32
#define DATA_FILL_POSITION   0x80000000
#define LEVEL_ERROR          200

static int16_t g_pattern[PATTERN_LEN] = { 0 };

static void ir_ie_one_data(uint8_t ie_one_data, int start_bit)
{
    int start = start_bit;
    uint8_t data = ie_one_data;
    for (int i = 0; i < BIT_LEN_8; i++) {
        if (data & 0x01) {
            /* send one */
            g_pattern[start + i] = NEC_ONE_H;
            g_pattern[start + 1 + i] = NEC_ONE_L;
        } else {
            /* send zero */
            g_pattern[start + i] = NEC_ZERO_H;
            g_pattern[start + 1 + i] = NEC_ZERO_L;
        }
        start++;
        data >>= 1;
    }
}

void ir_transmit_nec(uint8_t user_code_h, uint8_t data_code)
{
    /* start */
    g_pattern[0] = NEC_START_HIGH;
    g_pattern[1] = NEC_START_LOW;
    /* user_code_h */
    ir_ie_one_data(user_code_h, PARAM_2);
    /* user_code_l */
    ir_ie_one_data(~user_code_h, PARAM_18);
    /* data_code */
    ir_ie_one_data(data_code, PARAM_34);
    /* ~data_code */
    ir_ie_one_data(~data_code, PARAM_50);
    g_pattern[PARAM_66] = NEC_END_HIGH;
    g_pattern[PARAM_67] = NEC_END_LOW;
    uapi_pwm_deinit();
    uapi_ir_init(CONFIG_SAMPLE_IR_SEND_PIN);
    uapi_ir_transmit(NEC_FREQ, g_pattern, PATTERN_LEN);
}

void ir_nec_send(uint8_t user_code_h, uint8_t key)
{
#if defined(CONFIG_SAMPLE_SUPPORT_IR_STUDY)
    uint32_t len = 0;
    uint32_t freq = uapi_ir_study_check(key, &len);
    if ((len != 0xFFFFFFFF) && (len != 0x0)) {
        uapi_ir_study_send(key, len, freq);
    } else {
        ir_transmit_nec(user_code_h, key);
    }
#else
    ir_transmit_nec(user_code_h, key);
#endif /* CONFIG_SAMPLE_SUPPORT_IR_STUDY */
}