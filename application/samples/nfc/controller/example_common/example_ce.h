/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: nfc ce example, ExampleCardEmulation should enable build macro HAVE_NFC_LISTEN \n
 * Author: @CompanyNameTag \n
 * Date: 2023-07-19 \n
 */
#ifndef EXAMPLE_CE_H
#define EXAMPLE_CE_H

#include "nfc_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

NfcErrorCode NFC_EXAMPLE_StartCardEmulation(void);

#ifdef __cplusplus
}
#endif

#endif // EXAMPLE_CE_H