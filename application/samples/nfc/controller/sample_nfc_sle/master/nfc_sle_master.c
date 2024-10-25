/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: The process of nfc sle message in master. \n
 * Author: @CompanyNameTag \n
 * Date: 2023-06-28 \n
 */
#include "nfc_sle_master.h"
#include <stdbool.h>
#include "sle_hadm_api.h"
#include "oal_plat_op.h"

#include "nfc_sle_common.h"
#include "nfc_hal_core.h"
#include "example_rw.h"
#include "nfc_utils.h"
#include "nfc_hal_t4t.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_SLE_DATA_TYPE               0
#define NFC_TRIGGER_DISV_CMD_MAX_LEN    16
#define T4T_FILE_ID_LEN                 2
#define T4T_NDEF_LEN_BYTE_LEN           2

typedef enum {
    NFC_DISCOVERY,
    NFC_T4T_SELECT_AID,
    NFC_T4T_CC_FILE,
    NFC_T4T_NDEF_FILE
} NfcMasterStatus;

NfcMasterStatus g_nfcMasterStatus = NFC_DISCOVERY;
uint8_t g_nfcDisvTechModeList[] = {NFC_NCI_A_PASSIVE_POLL_MODE};

int SleMasterSendMsgToSlave(uint8_t slaveNum, uint8_t *sendBuff, uint16_t sendLen)
{
    return sle_hadm_server_ntf_by_addr(slaveNum, NFC_SLE_DATA_TYPE, sendLen, sendBuff);
}

void NfcSendSelectAidCmd(uint8_t slaveNum)
{
    g_nfcMasterStatus = NFC_T4T_SELECT_AID;

    uint8_t buff[] = {NFC_SLE_T4T_SELECT_AID_CMD};
    SleMasterSendMsgToSlave(slaveNum, buff, sizeof(buff));
}

void NfcSendSelectFileCmd(uint8_t slaveNum, uint16_t fid)
{
    if (fid == T4T_CC_FILE_ID) {
        g_nfcMasterStatus = NFC_T4T_CC_FILE;
    } else {
        g_nfcMasterStatus = NFC_T4T_NDEF_FILE;
    }

    uint8_t buff[] = {NFC_SLE_T4T_SELECT_FILE_CMD, UTIL_Byte1(fid), UTIL_Byte0(fid)};
    SleMasterSendMsgToSlave(slaveNum, buff, sizeof(buff));
}

void NfcSendReadCmd(uint8_t slaveNum, uint16_t offset, uint8_t rspMaxLen)
{
    uint8_t buff[] = {NFC_SLE_T4T_READ_CMD, UTIL_Byte1(offset), UTIL_Byte0(offset), rspMaxLen};
    SleMasterSendMsgToSlave(slaveNum, buff, sizeof(buff));
}

void NFC_SLE_SendTriggerDiscoveryCmd(uint8_t slaveNum)
{
    uint8_t buff[NFC_TRIGGER_DISV_CMD_MAX_LEN] = {NFC_SLE_TRIGGER_DISCOVERY};
    uint8_t idx = NFC_SLE_OID_LEN;
    uint8_t techNum = sizeof(g_nfcDisvTechModeList);
    if (techNum >= NFC_TRIGGER_DISV_CMD_MAX_LEN - NFC_SLE_OID_LEN) {
        PS_PRINT_ERR("discovery technology list overflow\n");
        return;
    }

    buff[idx++] = techNum;
    for (uint8_t tech = 0; tech < techNum; ++tech) {
        buff[idx + tech] = g_nfcDisvTechModeList[tech];
    }
    idx += techNum;

    g_nfcMasterStatus = NFC_DISCOVERY;
    SleMasterSendMsgToSlave(slaveNum, buff, idx);
}

void NfcParseNdefFile(uint8_t *ndef, uint8_t dataLen)
{
    UNUSED(ndef);
    UNUSED(dataLen);
    PS_PRINT_INFO("read ndef file success!!!\n");
    return;
}

void NFC_SLE_ProcActivatedNtf(uint8_t slaveNum, uint8_t *msg, uint16_t msgLen)
{
    if (msg == NULL || msgLen <= 0) {
        return;
    }
    uint8_t protocol = *msg;
    if (protocol == NFC_NCI_RF_PROTOCOL_ISO_DEP) {
        return NfcSendSelectAidCmd(slaveNum);
    } else {
        return NFC_SLE_SendTriggerDiscoveryCmd(slaveNum);
    }
}

void NFC_SLE_ProcT4TSelectAidRsp(uint8_t slaveNum, uint8_t *msg, uint16_t msgLen)
{
    UNUSED(msg);
    UNUSED(msgLen);
    NfcSendSelectFileCmd(slaveNum, T4T_CC_FILE_ID);
}

void NFC_SLE_ProcT4TSelectFileRsp(uint8_t slaveNum, uint8_t *msg, uint16_t msgLen)
{
    if (msg == NULL || msgLen < T4T_FILE_ID_LEN) {
        return;
    }

    if (g_nfcMasterStatus == NFC_T4T_CC_FILE) {     // select cc file rsp, then read cc file
        NfcSendReadCmd(slaveNum, 0x00, T4T_DATA_MAX_LEN);
    } else if (g_nfcMasterStatus == NFC_T4T_NDEF_FILE) {
        // select ndef file rsp, then read ndef file, ndef file len should be read the first time.
        NfcSendReadCmd(slaveNum, 0x00, T4T_NDEF_LEN_BYTE_LEN);
    } else {
        NFC_SLE_SendTriggerDiscoveryCmd(slaveNum);
    }
}

void NFC_SLE_ProcT4TReadRsp(uint8_t slaveNum, uint8_t *msg, uint16_t msgLen)
{
    if (msg == NULL || msgLen < T4T_FILE_ID_LEN + NFC_SLE_DATA_BYTE_LEN) {
        return;
    }

    uint8_t idx = 0;
    uint8_t dataLen = msg[idx++];
    if (g_nfcMasterStatus == NFC_T4T_CC_FILE) {      // read cc file rsp
        T4tCc ccInfo;
        NfcErrorCode ret = NFC_EXAMPLE_DecodeT4TCc(&msg[idx], dataLen, &ccInfo);
        if (ret != NFC_OK) {
            NFC_SLE_SendTriggerDiscoveryCmd(slaveNum);
        } else {
            NfcSendSelectFileCmd(slaveNum, ccInfo.fileId);
        }
    } else if (g_nfcMasterStatus == NFC_T4T_NDEF_FILE) {        // read ndef file rsp
        if (dataLen == T4T_NDEF_LEN_BYTE_LEN) {   // read ndef file len rsp
            uint16_t ndefLen = UTIL_Combine8bitTo16bit(msg[idx], msg[idx + 1]);
            return NfcSendReadCmd(slaveNum, T4T_NDEF_LEN_BYTE_LEN, ndefLen);
        } else {                                  // read ndef record file rsp
            NfcParseNdefFile(&msg[idx], dataLen);
            NFC_SLE_SendTriggerDiscoveryCmd(slaveNum);
        }
    }
}

void NFC_SLE_ProcT4TWriteRsp(uint8_t slaveNum, uint8_t *msg, uint16_t msgLen)
{
    UNUSED(slaveNum);
    if (msg == NULL || msgLen <= 0) {
        return;
    }
    return;
}

#ifdef __cplusplus
}
#endif
