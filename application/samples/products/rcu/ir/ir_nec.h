/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: IR NEC header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-10, Create file. \n
 */
#ifndef IR_NEC_H
#define IR_NEC_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief  Transmit NEC IR code.
 * @param  [in] user_code_h The IR user code.
 * @param  [in] data_code The IR data code.
 * @else
 * @brief  发送NEC协议格式的红外码。
 * @param  [in] user_code_h 红外用户码。
 * @param  [in] data_code 红外数据码。
 * @endif
 */
void ir_transmit_nec(uint8_t user_code_h, uint8_t data_code);

/**
 * @if Eng
 * @brief  Transmit learned NEC IR code.
 * @param  [in] user_code_h The IR user code.
 * @param  [in] key Learned Key.
 * @else
 * @brief  发送经过学习后的NEC格式的红外码。
 * @param  [in] user_code_h 红外用户码。
 * @param  [in] key 学习过的按键。
 * @endif
 */
void ir_nec_send(uint8_t user_code_h, uint8_t key);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


#endif