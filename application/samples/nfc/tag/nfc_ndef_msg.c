/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved.
 * Description: nfc_ndef_msg.c
 * Author: @CompanyNameTag
 * Create: 2023-05-06
 */
/*
 * 1 Header File Including
 */
#include "nfc_ndef_msg.h"
#include "nfc_tag_error.h"
#include "securec.h"

#ifdef FILE_ID_MANUAL
#define THIS_FILE_ID OAM_FILE_ID_NFC_NDEF_MSG_C
#endif
/*
 * 2 Global Variable Definition
 */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define NDEF_RECORD_HEAD_BASE_LEN          6 // 包含第一字节、TypeLength的一个字节和payload Length的四个字节
#define NDEF_RECORD_FIRST_BYTE             0
#define NDEF_RECORD_PAYLOAD_LEN_FIELD_LEN  4
#define NDEF_RECORD_PAYLOAD_LEN_FIRST_BYTE 2
#define NDEF_RECORD_IL_BIT                 3
#define NDEF_RECORD_HEAD_MB_FIELD          7
#define NDEF_RECORD_HEAD_ME_FIELD          6

/*
 * Record管理结构体
 */
typedef struct {
    NfcNdefRecord recordList[MAX_RECORD_NUM];   // Record管理队列
    uint8_t curNum;                             // 当前队列内Record数量
} NfcNdefRecordManagement;

NfcNdefRecordManagement g_management = {
    .curNum = 0
};

void NFC_RecordManagementClear(void)
{
    memset_s(&g_management, sizeof(g_management), 0, sizeof(g_management));
}

uint32_t NFC_AddRecord(NfcNdefRecord record)
{
    if (g_management.curNum == MAX_RECORD_NUM) {
        return NFC_ERR_INVALID_LENGTH;
    }
    g_management.recordList[g_management.curNum++] = record;
    return NFC_OK;
}

uint8_t NFC_GetRecordPosition(uint8_t index, uint8_t recordCurNum)
{
    uint8_t recordPos = 0;
    if (index == 0) {
        UTIL_SetBitEn(&recordPos, NDEF_RECORD_HEAD_MB_FIELD);
    }
    if (recordCurNum == index + 1) {
        UTIL_SetBitEn(&recordPos, NDEF_RECORD_HEAD_ME_FIELD);
    }

    return recordPos;
}

uint32_t NFC_NdefUndefineEncodeFuncHandle(NfcNdefRecordPayload *param, uint8_t *buff, uint32_t *buffLen)
{
    if (memcpy_s(buff, *buffLen, param->payload, param->payloadLen) != EOK) {
        return NFC_ERR_MEMCPY_FAIL;
    }
    *buffLen = param->payloadLen;

    return NFC_OK;
}

uint32_t NFC_NdefEncodeRecord(NfcNdefRecord *record, uint8_t recordPos, uint8_t *buff, uint32_t *buffLen)
{
    if (record == NULL || record->recordParam == NULL) {
        return NFC_ERR_PTR_NULL;
    }
    if ((record->idLen > 0 && (*buffLen <= (uint32_t)(NDEF_RECORD_HEAD_BASE_LEN + record->idLen + 1))) ||
        (record->idLen == 0 && *buffLen <= NDEF_RECORD_HEAD_BASE_LEN)) {
        return NFC_ERR_INVALID_LENGTH;
    }

    /***************************************
    NDEF RECORD帧结构
    ------------------
    Byte 0     |Byte 1 |Byte2      |Byte3      |Byte4      |Byte5      |Byte6       |Byte7 |Byte8     |Byte9 - end
    FIRST_BYTE |TYPE_L |PAYLOAD_L 3|PAYLOAD_L 2|PAYLOAD_L 1|PAYLOAD_L 0|ID_L(OPTION)|TYPE  |ID(OPTION)|PAYLOAD
    ------------------
    ***************************************/

    /***************************************
     FIRST_BYTE字段结构
     ----------------------------------------
     b8  b7  b6  b5  b4  b3  b2  b1  Meaning
     x                               MB
         x                           ME
             x                       CF
                 x                   SR
                     x               IL
                         x   x   x   TNF
     ----------------------------------------
    ***************************************/
    /* Record Head */
    uint32_t idx = 0;
    buff[idx++] = recordPos | record->tnf;
    buff[idx++] = record->typeLen;

    idx += NDEF_RECORD_PAYLOAD_LEN_FIELD_LEN;   // 先跳过payload length组帧，后续全部组帧完毕后填写

    if (record->idLen > 0) {
        buff[idx++] = record->idLen;
        UTIL_SetBitEn(&buff[NDEF_RECORD_FIRST_BYTE], NDEF_RECORD_IL_BIT);
    }
    if (record->typeLen > 0) {
        if (memcpy_s(buff + idx, *buffLen - idx, record->type, record->typeLen) != EOK) {
            return NFC_ERR_MEMCPY_FAIL;
        }
        idx += record->typeLen;
    }
    if (record->idLen > 0) {
        if (memcpy_s(buff + idx, *buffLen - idx, record->id, record->idLen) != EOK) {
            return NFC_ERR_MEMCPY_FAIL;
        }
        idx += record->idLen;
    }

    /* Record Payload */
    uint32_t payloadLen = *buffLen - idx;
    uint32_t ret = NFC_OK;
    if (record->encodeFunc != NULL) {
        ret = record->encodeFunc(record->recordParam, buff + idx, &payloadLen);
    } else {
        ret = NFC_NdefUndefineEncodeFuncHandle(record->recordParam, buff + idx, &payloadLen);
    }
    if (ret != NFC_OK) {
        return ret;
    }

    /* Record Head Payload Length */
    uint8_t i = NDEF_RECORD_PAYLOAD_LEN_FIRST_BYTE;
    buff[i++] = UTIL_Byte3(payloadLen);
    buff[i++] = UTIL_Byte2(payloadLen);
    buff[i++] = UTIL_Byte1(payloadLen);
    buff[i++] = UTIL_Byte0(payloadLen);

    *buffLen = payloadLen + idx;

    return NFC_OK;
}

uint32_t NFC_NdefMsgEncode(uint8_t *buff, uint32_t *buffLen)
{
    if (buff == NULL || buffLen == NULL) {
        NFC_RecordManagementClear();
        return NFC_ERR_PTR_NULL;
    }

    NfcNdefRecord *recordList = g_management.recordList;
    uint32_t totalLen = 0;
    for (uint8_t i = 0; i < g_management.curNum; i++) {
        uint8_t recordPos = NFC_GetRecordPosition(i, g_management.curNum);
        uint32_t len = *buffLen - totalLen;
        uint32_t ret = NFC_NdefEncodeRecord(recordList + i, recordPos, buff + totalLen, &len);
        if (ret != NFC_OK) {
            NFC_RecordManagementClear();
            return ret;
        }

        totalLen += len;
    }
    *buffLen = totalLen;
    NFC_RecordManagementClear();
    return NFC_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
