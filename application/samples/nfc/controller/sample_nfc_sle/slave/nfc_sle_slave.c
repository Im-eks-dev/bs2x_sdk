/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: The process of nfc sle message in slave. \n
 * Author: @CompanyNameTag \n
 * Date: 2023-06-28 \n
 */
#include "nfc_sle_slave.h"
#include <stdbool.h>
#include "securec.h"
#include "sle_hadm_api.h"
#include "oal_plat_op.h"

#include "example_rw.h"
#include "nfc_hal_core.h"
#include "nfc_hal_t4t.h"
#include "nfc_utils.h"
#include "nfc_sle_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DATA_BUF_SIZE                      300
#define NFC_DISCOVERY_FOREVER              0
#define NFC_SLE_DATA_TYPE                  0
#define NFC_SLE_SLAVE_ROLE                 1
#define NFC_SLE_RSP_DATA_MAX_LEN           248
#define NFC_SLE_RSP_OID_AND_STATUS_LEN     2
#define NFC_SLE_STATUS_FAIL                1

#define T4T_FILE_ID_LEN                    2
#define T4T_READ_OFFSET_LEN                2
#define T4T_READ_CMD_LEN                   3

int SleSlaveSendMsgToMaster(uint8_t *sendBuff, uint16_t sendLen)
{
    return sle_hadm_client_write(NFC_SLE_DATA_TYPE, sendLen, sendBuff, NFC_SLE_SLAVE_ROLE);
}

void NfcSendActivatedNtf(uint8_t status, uint8_t protocol)
{
    uint8_t buff[] = {NFC_SLE_ACTIVATED_NTF, status, protocol};
    SleSlaveSendMsgToMaster(buff, sizeof(buff));
}

void NfcSendSelectAidRsp(uint8_t status)
{
    uint8_t buff[] = {NFC_SLE_T4T_SELECT_AID_RSP, status};
    SleSlaveSendMsgToMaster(buff, NFC_SLE_RSP_OID_AND_STATUS_LEN);
}

void NfcSendSelectFileRsp(uint8_t status)
{
    uint8_t buff[] = {NFC_SLE_T4T_SELECT_FILE_RSP, status};
    SleSlaveSendMsgToMaster(buff, NFC_SLE_RSP_OID_AND_STATUS_LEN);
}

void NfcSendReadRsp(uint8_t status, uint8_t dataLen, uint8_t *data)
{
    uint8_t buff[NFC_SLE_RSP_DATA_MAX_LEN] = {NFC_SLE_T4T_READ_RSP, status};
    uint8_t idx = NFC_SLE_RSP_OID_AND_STATUS_LEN;
    if (status == NFC_SLE_STATUS_OK) {
        buff[idx++] = dataLen;
        if (memcpy_s(&buff[idx], NFC_SLE_RSP_DATA_MAX_LEN - idx, data, dataLen) != EOK) {
            // 状态改为fail，fail时只发送oid、status、fid
            buff[NFC_SLE_OID_LEN] = NFC_SLE_STATUS_FAIL;
            idx--;
        } else {
            idx += dataLen;
        }
    }

    SleSlaveSendMsgToMaster(buff, idx);
}

void NfcSendWriteRsp(uint8_t status)
{
    uint8_t buff[] = {NFC_SLE_T4T_WRITE_RSP, status};
    SleSlaveSendMsgToMaster(buff, NFC_SLE_RSP_OID_AND_STATUS_LEN);
}

NfcErrorCode NfcStartDiscovery(NfcIntfActInfo *intfActInfo, uint8_t techNum, uint8_t *tech)
{
    uint8_t recvBuffer[DATA_BUF_SIZE];

    NfcErrorCode ret = NFC_EXAMPLE_RwInit();
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_StartDiscovery(tech, techNum);
    if (ret != NFC_OK) {
        return ret;
    }

    // Discovery time = 0 is wait-forever
    ret = NFC_HAL_PollRecvIntfActNtf(PRIO_FIRST, recvBuffer, sizeof(recvBuffer), NFC_DISCOVERY_FOREVER);
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_PollParseIntfActNtf(intfActInfo, recvBuffer, sizeof(recvBuffer));
    if (ret != NFC_OK) {
        return ret;
    }

    return NFC_OK;
}

void NFC_SLE_ProcT4TSelectAidCmd(uint8_t *msg, uint16_t msgLen)
{
    UNUSED(msg);
    if (msgLen != 0) {
        return;
    }

    uint8_t rApduData[T4T_DATA_MAX_LEN] = {0};
    NfcHalT4tRApdu rApduInfo = {
        .data = rApduData,
        .dataBuffSize = T4T_DATA_MAX_LEN,
        .dataLen = 0,
        .sw1 = 0x00,
        .sw2 = 0x00
    };
    NfcErrorCode ret = NFC_HAL_T4TPollerSelectAid(&rApduInfo);
    if (ret != NFC_OK || rApduInfo.sw1 != CMD_COMPLETE_SW1 || rApduInfo.sw2 != CMD_COMPLETE_SW2) {
        NfcSendSelectAidRsp(NFC_SLE_STATUS_FAIL);
    } else {
        NfcSendSelectAidRsp(NFC_SLE_STATUS_OK);
    }
}

void NFC_SLE_ProcT4TSelectFileCmd(uint8_t *msg, uint16_t msgLen)
{
    if (msg == NULL || msgLen < T4T_FILE_ID_LEN) {
        return;
    }

    uint16_t fid = UTIL_Combine8bitTo16bit(msg[0], msg[1]);
    uint8_t rApduData[T4T_DATA_MAX_LEN] = {0};
    NfcHalT4tRApdu rApduInfo = {
        .data = rApduData,
        .dataBuffSize = T4T_DATA_MAX_LEN,
        .dataLen = 0,
        .sw1 = 0x00,
        .sw2 = 0x00
    };
    NfcErrorCode ret = NFC_HAL_T4TPollerSelectFile(fid, &rApduInfo);
    if (ret != NFC_OK || rApduInfo.sw1 != CMD_COMPLETE_SW1 || rApduInfo.sw2 != CMD_COMPLETE_SW2) {
        NfcSendSelectFileRsp(NFC_SLE_STATUS_FAIL);
    } else {
        NfcSendSelectFileRsp(NFC_SLE_STATUS_OK);
    }
}

void NFC_SLE_ProcT4TReadCmd(uint8_t *msg, uint16_t msgLen)
{
    if (msg == NULL || msgLen < T4T_READ_CMD_LEN) {
        return;
    }

    uint16_t idx = 0;
    uint16_t offset = UTIL_Combine8bitTo16bit(msg[idx], msg[idx + 1]);
    idx += T4T_READ_OFFSET_LEN;
    uint8_t expLen = msg[idx];
    uint8_t rApduData[T4T_DATA_MAX_LEN] = {0};
    NfcHalT4tRApdu rApduInfo = {
        .data = rApduData,
        .dataBuffSize = T4T_DATA_MAX_LEN,
        .dataLen = 0,
        .sw1 = 0x00,
        .sw2 = 0x00
    };
    NfcErrorCode ret = NFC_HAL_T4TPollerReadData(offset, expLen, &rApduInfo);
    if (ret != NFC_OK || rApduInfo.sw1 != CMD_COMPLETE_SW1 || rApduInfo.sw2 != CMD_COMPLETE_SW2) {
        NfcSendReadRsp(NFC_SLE_STATUS_FAIL, (uint8_t)rApduInfo.dataLen, rApduInfo.data);
    } else {
        NfcSendReadRsp(NFC_SLE_STATUS_OK, (uint8_t)rApduInfo.dataLen, rApduInfo.data);
    }
}

void NFC_SLE_ProcT4TWriteCmd(uint8_t *msg, uint16_t msgLen)
{
    if (msg == NULL || msgLen <= 0) {
        return;
    }
    return;
}

void NFC_SLE_ProcTriggerDiscoveryCmd(uint8_t *msg, uint16_t msgLen)
{
    if (msgLen <= 0) {
        return;
    }

    NfcErrorCode ret = NFC_HAL_Deactivate();
    if (ret != NFC_OK) {
        return NfcSendActivatedNtf(NFC_SLE_STATUS_FAIL, 0x00);
    }

    uint8_t techNum = *msg++;
    uint8_t *techList = msg;
    NfcIntfActInfo intfActInfo = {0};
    ret = NfcStartDiscovery(&intfActInfo, techNum, techList);
    if (ret != NFC_OK) {
        ret = NFC_HAL_Open();
        if (ret != NFC_OK) {
            PS_PRINT_ERR("nfc open fail!\r\n");
            return;
        } else {
            return NfcSendActivatedNtf(NFC_SLE_STATUS_FAIL, 0x00);
        }
    } else {
        return NfcSendActivatedNtf(NFC_SLE_STATUS_OK, intfActInfo.protocol);
    }
}

#ifdef __cplusplus
}
#endif
