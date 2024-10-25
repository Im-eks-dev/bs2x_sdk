/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: Provide the entry for nfc sle slave sample thread. \n
 * Author: @CompanyNameTag \n
 * Date: 2023-06-28 \n
 */
#include <stdio.h>
#include "app_init.h"
#include "sle_hadm_api.h"
#include "oal_os.h"
#include "oal_plat_op.h"

#include "nfc_sle_slave.h"
#include "nfc_sle_common.h"
#include "nfc_sle_queue.h"
#include "nfc_hal_core.h"
#include "nfc_hal_t4t.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_SLE_SLAVE_TASK_STACK_SIZE               0x800
#define NFC_SLE_SLAVE_TASK_PRIO                     (osPriority_t)(17)

#define NFC_SLE_SLAVE_ACT_NUM (sizeof(g_nfcSleSlaveProcMsgActTbl) / sizeof(NfcSleAct))

osThreadId_t g_nfcSleSlaveTaskId = NULL;

NfcSleAct g_nfcSleSlaveProcMsgActTbl[] = {
    {NFC_SLE_TRIGGER_DISCOVERY, NFC_SLE_ProcTriggerDiscoveryCmd},
    {NFC_SLE_T4T_SELECT_AID_CMD, NFC_SLE_ProcT4TSelectAidCmd},
    {NFC_SLE_T4T_SELECT_FILE_CMD, NFC_SLE_ProcT4TSelectFileCmd},
    {NFC_SLE_T4T_READ_CMD, NFC_SLE_ProcT4TReadCmd},
    {NFC_SLE_T4T_WRITE_CMD, NFC_SLE_ProcT4TWriteCmd},
};

NfcSleAct* NFC_SLE_SlaveSearchActByOid(uint8_t oid)
{
    for (uint8_t i = 0; i < NFC_SLE_SLAVE_ACT_NUM; ++i) {
        if (g_nfcSleSlaveProcMsgActTbl[i].oidType == oid) {
            return &g_nfcSleSlaveProcMsgActTbl[i];
        }
    }
    return NULL;
}

void NFC_SLE_SlaveProcMsg(struct MsgNode *node)
{
    if (node->len < NFC_SLE_OID_LEN && node->data == NULL) {
        return;
    }

    uint8_t oidType = *node->data;
    NfcSleAct *act = NFC_SLE_SlaveSearchActByOid(oidType);
    if (act == NULL) {
        return;
    }

    uint8_t *msg = &node->data[NFC_SLE_OID_LEN];
    uint16_t msgLen = node->len - NFC_SLE_OID_LEN;
    return act->actionFunc(msg, msgLen);
}

void SlaveMsgLoop(void)
{
    struct MsgNode *msg = NFC_SLE_MsgGet();
    if (msg == NULL) {
        return;
    }

    NFC_SLE_SlaveProcMsg(msg);
    oal_free(msg->data);
}

void NFC_SLE_SlaveMsgCbk(uint8_t *addr, uint8_t *data, uint16_t dataLen)
{
    UNUSED(addr);
    int32_t ret = NFC_SLE_MsgAdd(0, data, dataLen);
    if (ret != 0) {
        PS_PRINT_ERR("add sle nfc data queue fail!\n");
        return;
    }

    osThreadResume(g_nfcSleSlaveTaskId);
    return;
}

void NFC_SLE_SlaveCbkFuncRegister(void)
{
    sle_hadm_cbks_t funcs = {
        .connect_cbk = NULL,
        .disconnect_cbk = NULL,
        .server_recive_cbk = NULL,
        .client_recive_cbk = NFC_SLE_SlaveMsgCbk
    };

    sle_hadm_reg_cbks(&funcs);
}

static void *NFC_SLE_SlaveProcess(const char *arg)
{
    UNUSED(arg);
    NfcErrorCode ret = NFC_HAL_Open();
    if (ret != NFC_OK) {
        PS_PRINT_ERR("nfc open fail, exit nfc sle process!\r\n");
        return NULL;
    }

    NFC_SLE_SlaveCbkFuncRegister();
    NFC_SLE_MsgQueueInit();
    while (true) {
        SlaveMsgLoop();
        if (GetMsgQueueLen() == 0) {
            osThreadSuspend(g_nfcSleSlaveTaskId);
        }
    }
    return NULL;
}

static void NFC_SLE_SlaveEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "NfcSleSlaveTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = NFC_SLE_SLAVE_TASK_STACK_SIZE;
    attr.priority = NFC_SLE_SLAVE_TASK_PRIO;

    g_nfcSleSlaveTaskId = osThreadNew((osThreadFunc_t)NFC_SLE_SlaveProcess, NULL, &attr);
    if (g_nfcSleSlaveTaskId == NULL) {
        PS_PRINT_ERR("nfc sle slave task create fail!\n");
        return;
    }
}

/* Run the NFC_SLE_SlaveEntry. */
app_run(NFC_SLE_SlaveEntry);

#ifdef __cplusplus
}
#endif
