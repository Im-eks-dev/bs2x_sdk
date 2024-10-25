/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: Provide the interface for nfc sle message queue. \n
 * Author: @CompanyNameTag \n
 * Date: 2023-06-28 \n
 */
#ifndef NFC_SLE_QUEUE_H
#define NFC_SLE_QUEUE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MsgNode {
    uint8_t *data;
    uint16_t len;
#ifdef NFC_SLE_MASTER_SAMPLE
    uint8_t slaveNum;
#endif
};

int32_t NFC_SLE_MsgAdd(uint8_t slaveNum, uint8_t *data, uint16_t len);

struct MsgNode* NFC_SLE_MsgGet(void);

void NFC_SLE_MsgQueueInit(void);

uint8_t GetMsgQueueLen(void);

#ifdef __cplusplus
}
#endif

#endif // NFC_SLE_QUEUE_H