/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved.
 * Description: nfc_text_record.h
 * Author: @CompanyNameTag
 * Create: 2023-05-06
 */
#ifndef NFC_TEXT_RECORD_H
#define NFC_TEXT_RECORD_H

/*
 * 1 Other Header File Including
 */
#include <stdint.h>
#include "nfc_ndef_msg.h"
/*
 * 2 Macro Definition
 */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 * UTF类型
 */
typedef enum {
    UTF_8 = 0,
    UTF_16 = 1,
} NfcTextRecordUtf;

/*
 * Text Payload生成所需参数
 */
typedef struct {
    uint8_t utf;
    uint8_t *languageCode;
    uint8_t languageCodeLen;
    uint8_t *textData;
    uint32_t textDataLen;
} NfcTextRecordParam;

/**
 * @brief  Text Record添加函数
 *
 * @param  [in]  param       Text Payload生成所需参数
 * @return uint32_t          成功返回NFC_SUCC，失败返回错误码
 */
uint32_t NFC_AddTextRecord(NfcTextRecordParam *param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
