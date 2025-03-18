/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved.
 * Description: nfc_uri_record.c
 * Author: @CompanyNameTag
 * Create: 2023-05-06
 */
/*
 * 1 Header File Including
 */
#include "nfc_uri_record.h"
#include "nfc_tag_error.h"
#include "securec.h"

#ifdef FILE_ID_MANUAL
#define THIS_FILE_ID OAM_FILE_ID_NFC_URI_RECORD_C
#endif
/*
 * 2 Global Variable Definition
 */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static const uint8_t g_uriType = 'U';

uint32_t NFC_UriPayloadEncode(NfcUriRecordParam *recordParam, uint8_t *buff, uint32_t *buffLen)
{
    if ((uint32_t)(recordParam->uriDataLen + 1) > *buffLen) {
        return NFC_ERR_INVALID_LENGTH;
    }
    /***************************************
    URI Payload帧结构
    ------------------
    Byte 0 |Byte 1 - end
    uriId  |uriData
    ------------------
    ***************************************/
    uint32_t idx = 0;
    buff[idx++] = recordParam->uriId;
    if (memcpy_s(buff + idx, *buffLen - idx, recordParam->uriData, recordParam->uriDataLen) != EOK) {
        return NFC_ERR_MEMCPY_FAIL;
    }
    idx += recordParam->uriDataLen;
    *buffLen = idx;

    return NFC_OK;
}

uint32_t NFC_AddUriRecord(NfcUriRecordParam *param)
{
    if (param == NULL) {
        return NFC_ERR_PTR_NULL;
    }
    NfcNdefRecord record;
    record.tnf = TNF_WELL_KNOWN;
    record.idLen = 0;
    record.id = NULL;
    record.typeLen = 0x1;
    record.type = &g_uriType;
    record.encodeFunc = (NfcNdefRecordEncodeFunc)NFC_UriPayloadEncode;
    record.recordParam = param;

    return NFC_AddRecord(record);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
