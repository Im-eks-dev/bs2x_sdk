/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: nfc rw example, ExampleReaderWriter should enable build macro HAVE_NFC_POLL \n
 * Author: @CompanyNameTag \n
 * Date: 2023-07-19 \n
 */
#ifndef EXAMPLE_RW_H
#define EXAMPLE_RW_H

#include "nfc_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t mLe;
    uint8_t mLc;
    uint16_t fileId;
    uint16_t fileSize;
} T4tCc;

NfcErrorCode NFC_EXAMPLE_StartReaderWriter(void);

NfcErrorCode NFC_EXAMPLE_RwInit(void);

NfcErrorCode NFC_EXAMPLE_DecodeT4TCc(uint8_t *data, uint16_t len, T4tCc *ctx);

#ifdef __cplusplus
}
#endif

#endif // EXAMPLE_RW_H