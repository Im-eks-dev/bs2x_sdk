/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: nfc only sample. \n
 * Author: @CompanyNameTag \n
 * Date: 2023-07-6 \n
 */
#include "app_init.h"
#include "oal_plat_op.h"
#include "oal_os.h"
#include "nfc_hal_core.h"
#ifdef HAVE_NFC_LISTEN
#include "example_ce.h"
#endif
#ifdef HAVE_NFC_POLL
#include "example_rw.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_ONLY_SAMPLE_TASK_STACK_SIZE               0x800
#define NFC_ONLY_SAMPLE_TASK_PRIO                     (osPriority_t)(17)

#define NFC_DISCOVERY_FOREVER          0
#define NFC_WAIT_NEXT_BUSINESS_TO      500
#define DATA_BUF_SIZE                  300

typedef enum {
    NFC_RW_MODE,
    NFC_CE_MODE,
} NfcWorkMode;

NfcWorkMode g_nfcWorkMode = NFC_RW_MODE;

static void *NfcOnlySampleProcess(const char *arg)
{
    UNUSED(arg);
    while (true) {
        NfcErrorCode ret = NFC_HAL_Open();
        if (ret != NFC_OK) {
            PS_PRINT_ERR("nfc open fail, exit nfc process!\r\n");
            return NULL;
        }
        do {
            switch (g_nfcWorkMode) {
#ifdef HAVE_NFC_POLL
                case NFC_RW_MODE:
                    ret = NFC_EXAMPLE_StartReaderWriter();
                    break;
#endif
#ifdef HAVE_NFC_LISTEN
                case NFC_CE_MODE:
                    ret = NFC_EXAMPLE_StartCardEmulation();
                    break;
#endif
                default:
                    break;
            }
            oal_msleep(NFC_WAIT_NEXT_BUSINESS_TO);
        } while (ret == NFC_OK);
        PS_PRINT_ERR("nfc business error = 0x%x\r\n", ret);
        ret = NFC_HAL_Close();
        if (ret != NFC_OK) {
            PS_PRINT_ERR("nfc close fail, exit nfc process!\r\n");
            return NULL;
        }
    }
}

static void NfcOnlySampleEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "NfcOnlySampleTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = NFC_ONLY_SAMPLE_TASK_STACK_SIZE;
    attr.priority = NFC_ONLY_SAMPLE_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)NfcOnlySampleProcess, NULL, &attr) == NULL) {
        PS_PRINT_ERR("nfc only sample task create fail!\n");
        return;
    }
}

/* Run the NfcOnlySampleEntry. */
app_run(NfcOnlySampleEntry);

#ifdef __cplusplus
}
#endif
