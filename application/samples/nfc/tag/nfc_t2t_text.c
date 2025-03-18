/*
 * Copyright (c) @CompanyNameMagicTag 2024. All rights reserved.
 * Description: Provide the entry for nfc sample
 * Author: @CompanyNameTag
 * Date: 2024-01-03
 */

#include <stddef.h>
#include "nfc_tag_t2t.h"
#include "nfc_ndef_msg.h"
#include "nfc_text_record.h"
#include "osal_debug.h"
#include "app_init.h"
#include "common_def.h"

uint8_t g_ndefBuff[NFC_T2T_MAX_NDEF_SIZE];

static void NFC_T2T_Callback(uint32_t event, const uint8_t *data, uint16_t len)
{
    unused(data);
    unused(len);

    switch (event) {
        case NFC_T2T_EVENT_FIELD_ON:
            osal_printk("nfc field on!\r\n");
            break;
        case NFC_T2T_EVENT_FIELD_OFF:
            osal_printk("nfc field off!\r\n");
            break;
        default:
            break;
    }
    return;
}

NfcTagError NFC_T2T_TextNdefMsgEncode(uint8_t *buff, uint32_t *bufflen)
{
    uint8_t enCode[] = {'e', 'n'};
    uint8_t enData[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
    NfcTextRecordParam param = {.utf = UTF_8,
                                .languageCode = enCode,
                                .languageCodeLen = sizeof(enCode),
                                .textData = enData,
                                .textDataLen = sizeof(enData),
    };

    NfcTagError ret = NFC_AddTextRecord(&param);
    if (ret != NFC_OK) {
        return ret;
    }

    return NFC_NdefMsgEncode(buff, bufflen);
}

void NFC_T2T_SystemInit(void)
{
    NfcTagError ret;

    NFC_T2T_Init(NFC_T2T_Callback);

    uint8_t nfcId[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    ret = NFC_T2T_SetConfig(NFC_T2T_PARAM_NFCID, nfcId, sizeof(nfcId));
    if (ret != NFC_OK) {
        return;
    }

    uint32_t buffLen = (uint32_t)sizeof(g_ndefBuff);
    ret = NFC_T2T_TextNdefMsgEncode(g_ndefBuff, &buffLen);
    if (ret != NFC_OK) {
        return;
    }

    ret = NFC_T2T_SetNdef(g_ndefBuff, buffLen);
    if (ret != NFC_OK) {
        return;
    }

    NFC_T2T_StartEmulation();
    return;
}

app_run(NFC_T2T_SystemInit);