/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: Provide the entry for nfc sle master sample thread. \n
 * Author: @CompanyNameTag \n
 * Date: 2023-06-28 \n
 */
#include <stdint.h>
#include "app_init.h"
#include "sle_hadm_api.h"
#include "sle_hadm_common.h"
#include "oal_os.h"
#include "oal_plat_op.h"

#include "nfc_sle_master.h"
#include "nfc_sle_common.h"
#include "nfc_sle_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_SLE_MASTER_TASK_STACK_SIZE               0x800
#define NFC_SLE_MASTER_TASK_PRIO                     (osPriority_t)(17)

#define NFC_SLE_MASTER_ACT_NUM (sizeof(g_nfcSleMasterProcMsgActTbl) / sizeof(NfcSleAct))

osThreadId_t g_nfcSleMasterTaskId = NULL;

NfcSleAct g_nfcSleMasterProcMsgActTbl[] = {
    {NFC_SLE_ACTIVATED_NTF, NFC_SLE_ProcActivatedNtf},
    {NFC_SLE_T4T_SELECT_AID_RSP, NFC_SLE_ProcT4TSelectAidRsp},
    {NFC_SLE_T4T_SELECT_FILE_RSP, NFC_SLE_ProcT4TSelectFileRsp},
    {NFC_SLE_T4T_READ_RSP, NFC_SLE_ProcT4TReadRsp},
    {NFC_SLE_T4T_WRITE_RSP, NFC_SLE_ProcT4TWriteRsp},
};

NfcSleAct* NFC_SLE_MasterSearchActByOid(uint8_t oid)
{
    for (uint8_t i = 0; i < NFC_SLE_MASTER_ACT_NUM; ++i) {
        if (g_nfcSleMasterProcMsgActTbl[i].oidType == oid) {
            return &g_nfcSleMasterProcMsgActTbl[i];
        }
    }
    return NULL;
}

void NFC_SLE_MasterProcMsg(struct MsgNode *node)
{
    if (node->len < NFC_SLE_OID_LEN && node->data == NULL) {
        return;
    }

    uint8_t oidType = *node->data;
    NfcSleAct *act = NFC_SLE_MasterSearchActByOid(oidType);
    if (act == NULL) {
        return;
    }

    uint8_t *msg = &node->data[NFC_SLE_OID_LEN];
    uint16_t msgLen = node->len - NFC_SLE_OID_LEN - NFC_SLE_STATUS_LEN;
    uint8_t status = *msg++;
    if (status != NFC_SLE_STATUS_OK) {
        return NFC_SLE_SendTriggerDiscoveryCmd(node->slaveNum);
    }
    return act->actionFunc(node->slaveNum, msg, msgLen);
}


void MasterMsgLoop(void)
{
    struct MsgNode *msg = NFC_SLE_MsgGet();
    if (msg == NULL) {
        return;
    }

    NFC_SLE_MasterProcMsg(msg);
    oal_free(msg->data);
}

void NFC_SLE_MasterConnectedCbk(uint8_t *addr, uint16_t lcid, uint8_t connectState)
{
    UNUSED(lcid);
    UNUSED(connectState);
    uint8_t slaveNum = 0;
    sle_hadm_get_num_by_addr(addr, &slaveNum);
    NFC_SLE_SendTriggerDiscoveryCmd(slaveNum);
}

void NFC_SLE_MasterMsgCbk(uint8_t num, uint8_t *addr, uint8_t *data, uint16_t dataLen)
{
    UNUSED(addr);
    int32_t ret = NFC_SLE_MsgAdd(num, data, dataLen);
    if (ret != 0) {
        PS_PRINT_ERR("add sle nfc data queue fail!\n");
        return;
    }

    osThreadResume(g_nfcSleMasterTaskId);
    return;
}

void NFC_SLE_MasterCbkFuncRegister(void)
{
    sle_hadm_cbks_t funcs = {
        .connect_cbk = NFC_SLE_MasterConnectedCbk,
        .disconnect_cbk = NULL,
        .server_recive_cbk = NFC_SLE_MasterMsgCbk,
        .client_recive_cbk = NULL
    };

    sle_hadm_reg_cbks(&funcs);
}

static void *NFC_SLE_MasterProcess(const char *arg)
{
    UNUSED(arg);
    NFC_SLE_MasterCbkFuncRegister();
    NFC_SLE_MsgQueueInit();
    while (true) {
        MasterMsgLoop();
        if (GetMsgQueueLen() == 0) {
            osThreadSuspend(g_nfcSleMasterTaskId);
        }
    }
    return NULL;
}

static void NFC_SLE_MasterEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "NfcSleMasterTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = NFC_SLE_MASTER_TASK_STACK_SIZE;
    attr.priority = NFC_SLE_MASTER_TASK_PRIO;

    g_nfcSleMasterTaskId = osThreadNew((osThreadFunc_t)NFC_SLE_MasterProcess, NULL, &attr);
    if (g_nfcSleMasterTaskId == NULL) {
        PS_PRINT_ERR("nfc sle master task create fail!\n");
        return;
    }
}

/* Run the NFC_SLE_MasterEntry. */
app_run(NFC_SLE_MasterEntry);

#ifdef __cplusplus
}
#endif
