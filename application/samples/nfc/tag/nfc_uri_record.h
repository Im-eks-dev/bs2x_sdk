/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved.
 * Description: nfc_uri_record.h
 * Author: @CompanyNameTag
 * Create: 2023-05-06
 */
#ifndef NFC_URI_RECORD_H
#define NFC_URI_RECORD_H

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
 * URI Index
 */
typedef enum {
    NFC_URI_NONE          = 0x00,
    NFC_URI_HTTP_WWW      = 0x01,
    NFC_URI_HTTPS_WWW     = 0x02,
    NFC_URI_HTTP          = 0x03,
    NFC_URI_HTTPS         = 0x04,
    NFC_URI_TEL           = 0x05,
    NFC_URI_MAILTO        = 0x06,
    NFC_URI_FTP_ANONYMOUS = 0x07,
    NFC_URI_FTP_FTP       = 0x08,
    NFC_URI_FTPS          = 0x09,
    NFC_URI_SFTP          = 0x0A,
    NFC_URI_SMB           = 0x0B,
    NFC_URI_NFS           = 0x0C,
    NFC_URI_FTP           = 0x0D,
    NFC_URI_DAV           = 0x0E,
    NFC_URI_NEWS          = 0x0F,
    NFC_URI_TELNET        = 0x10,
    NFC_URI_IMAP          = 0x11,
    NFC_URI_RTSP          = 0x12,
    NFC_URI_URN           = 0x13,
    NFC_URI_POP           = 0x14,
    NFC_URI_SIP           = 0x15,
    NFC_URI_SIPS          = 0x16,
    NFC_URI_TFTP          = 0x17,
    NFC_URI_BTSPP         = 0x18,
    NFC_URI_BTL2CAP       = 0x19,
    NFC_URI_BTGOEP        = 0x1A,
    NFC_URI_TCPOBEX       = 0x1B,
    NFC_URI_IRDAOBEX      = 0x1C,
    NFC_URI_FILE          = 0x1D,
    NFC_URI_URN_EPC_ID    = 0x1E,
    NFC_URI_URN_EPC_TAG   = 0x1F,
    NFC_URI_URN_EPC_PAT   = 0x20,
    NFC_URI_URN_EPC_RAW   = 0x21,
    NFC_URI_URN_EPC       = 0x22,
    NFC_URI_URN_NFC       = 0x23,
    NFC_URI_RFU           = 0xFF,
} NfcUriId;

/*
 * Uri Payload生成所需参数
 */
typedef struct {
    uint8_t uriId;
    uint8_t *uriData;
    uint8_t uriDataLen;
} NfcUriRecordParam;

/**
 * @brief  Uri Record添加函数
 *
 * @param  [in]  param       Uri Payload生成所需参数
 * @return uint32_t          成功返回NFC_SUCC，失败返回错误码
 */
uint32_t NFC_AddUriRecord(NfcUriRecordParam *param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
