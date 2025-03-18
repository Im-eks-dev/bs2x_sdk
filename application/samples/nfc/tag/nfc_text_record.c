/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved.
 * Description: nfc_text_record.c
 * Author: @CompanyNameTag
 * Create: 2023-05-06
 */
/*
 * 1 Header File Including
 */
#include "nfc_text_record.h"
#include "nfc_tag_error.h"
#include "securec.h"

#ifdef FILE_ID_MANUAL
#define THIS_FILE_ID OAM_FILE_ID_NFC_TEXT_RECORD_C
#endif
/*
 * 2 Global Variable Definition
 */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define TEXT_REC_STATUS_LEN       1
#define TEXT_REC_STATUS_POS       7
#define TEXT_REC_MAX_ISO_CODE_LEN 63

static const uint8_t g_textType = 'T';

uint32_t TextParamJudgement(const NfcTextRecordParam *recordParam)
{
    if (recordParam->languageCodeLen == 0 || recordParam->languageCodeLen > TEXT_REC_MAX_ISO_CODE_LEN ||
        recordParam->languageCode == NULL || recordParam->textDataLen == 0 || recordParam->textData == NULL) {
        return NFC_ERR_INVALID_LENGTH;
    }
    return NFC_OK;
}
uint32_t NFC_TextPayloadEncode(NfcTextRecordParam *recordParam, uint8_t *buff, uint32_t *buffLen)
{
    uint32_t ret = TextParamJudgement(recordParam);
    if (ret != NFC_OK) {
        return ret;
    }

    uint32_t payloadLen = TEXT_REC_STATUS_LEN + recordParam->languageCodeLen + recordParam->textDataLen;
    if (payloadLen > *buffLen) {
        return NFC_ERR_INVALID_LENGTH;
    }
    /***************************************
    TEXT Payload帧结构
    ------------------
    Byte 0 |Byte 1 - isoCodeLen |Byte isoCodeLen+1 - end
    Status |isoCode             |textData
    ------------------
    ***************************************/

    /***************************************
     Status字段结构
     ----------------------------------------
     b7  b6  b5  b4  b3  b2  b1  b0  Meaning
     x                               UTF
         0                           RFU
             x   x   x   x   x   x   isoCodeLen
     ----------------------------------------
    ***************************************/
    uint32_t idx = 0;
    buff[idx++] = (recordParam->utf << TEXT_REC_STATUS_POS) + recordParam->languageCodeLen;
    if (memcpy_s(buff + idx, *buffLen - idx, recordParam->languageCode, recordParam->languageCodeLen) != EOK) {
        return NFC_ERR_MEMCPY_FAIL;
    }
    idx += recordParam->languageCodeLen;

    if (memcpy_s(buff + idx, *buffLen - idx, recordParam->textData, recordParam->textDataLen) != EOK) {
        return NFC_ERR_MEMCPY_FAIL;
    }
    idx += recordParam->textDataLen;
    *buffLen = idx;

    return NFC_OK;
}

uint32_t NFC_AddTextRecord(NfcTextRecordParam *param)
{
    if (param == NULL) {
        return NFC_ERR_PTR_NULL;
    }
    NfcNdefRecord record;
    record.tnf = TNF_WELL_KNOWN;
    record.idLen = 0;
    record.id = NULL;
    record.typeLen = 0x1;
    record.type = &g_textType;
    record.encodeFunc = (NfcNdefRecordEncodeFunc)NFC_TextPayloadEncode;
    record.recordParam = param;

    return NFC_AddRecord(record);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
