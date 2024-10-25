/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: The process of nfc sle message in slave. \n
 * Author: @CompanyNameTag \n
 * Date: 2023-06-28 \n
 */
#ifndef NFC_SLE_SLAVE_H
#define NFC_SLE_SLAVE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void NFC_SLE_ProcT4TSelectAidCmd(uint8_t *msg, uint16_t msgLen);
void NFC_SLE_ProcT4TSelectFileCmd(uint8_t *msg, uint16_t msgLen);
void NFC_SLE_ProcT4TReadCmd(uint8_t *msg, uint16_t msgLen);
void NFC_SLE_ProcT4TWriteCmd(uint8_t *msg, uint16_t msgLen);
void NFC_SLE_ProcTriggerDiscoveryCmd(uint8_t *msg, uint16_t msgLen);

#ifdef __cplusplus
}
#endif

#endif // NFC_SLE_SLAVE_H