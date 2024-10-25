/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: The process of nfc sle message in master. \n
 * Author: @CompanyNameTag \n
 * Date: 2023-06-28 \n
 */
#ifndef NFC_SLE_MASTER_H
#define NFC_SLE_MASTER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void NFC_SLE_ProcActivatedNtf(uint8_t slaveNum, uint8_t *msg, uint16_t msgLen);
void NFC_SLE_ProcT4TSelectAidRsp(uint8_t slaveNum, uint8_t *msg, uint16_t msgLen);
void NFC_SLE_ProcT4TSelectFileRsp(uint8_t slaveNum, uint8_t *msg, uint16_t msgLen);
void NFC_SLE_ProcT4TReadRsp(uint8_t slaveNum, uint8_t *msg, uint16_t msgLen);
void NFC_SLE_ProcT4TWriteRsp(uint8_t slaveNum, uint8_t *msg, uint16_t msgLen);

void NFC_SLE_SendTriggerDiscoveryCmd(uint8_t slaveNum);

#ifdef __cplusplus
}
#endif

#endif // NFC_SLE_MASTER_H