/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: Provide nfc sle sample common information. \n
 * Author: @CompanyNameTag \n
 * Date: 2023-06-28 \n
 */
#ifndef NFC_SLE_COMMON_H
#define NFC_SLE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_SLE_STATUS_OK                           0
#define NFC_SLE_STATUS_FAIL                         1
#define NFC_SLE_OID_LEN                             1
#define NFC_SLE_DATA_BYTE_LEN                       1
#define NFC_SLE_STATUS_LEN                          1

typedef void (*NFC_SLE_MASTER_ACTION_FUN)(uint8_t num, uint8_t *msg, uint16_t msgLen);
typedef void (*NFC_SLE_SLAVE_ACTION_FUN)(uint8_t *msg, uint16_t msgLen);

typedef struct {
    uint8_t oidType;
#ifdef NFC_SLE_MASTER_SAMPLE
    NFC_SLE_MASTER_ACTION_FUN actionFunc;
#else
    NFC_SLE_SLAVE_ACTION_FUN actionFunc;
#endif
} NfcSleAct;

enum NfcSleOid {
    // DL msg
    // common oid
    NFC_SLE_TRIGGER_DISCOVERY = 0x00,

    // t4t oid
    NFC_SLE_T4T_SELECT_AID_CMD = 0x40,
    NFC_SLE_T4T_SELECT_FILE_CMD,
    NFC_SLE_T4T_READ_CMD,
    NFC_SLE_T4T_WRITE_CMD,

    // UL msg
    // common oid
    NFC_SLE_ACTIVATED_NTF = 0x80,

    // t4t oid
    NFC_SLE_T4T_SELECT_AID_RSP = 0xC0,
    NFC_SLE_T4T_SELECT_FILE_RSP,
    NFC_SLE_T4T_READ_RSP,
    NFC_SLE_T4T_WRITE_RSP,
};

#ifdef __cplusplus
}
#endif

#endif // NFC_SLE_COMMON_H