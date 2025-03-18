/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved. \n
 * Description: nfc rw example, ExampleReaderWriter should enable build macro HAVE_NFC_POLL \n
 * Author: @CompanyNameTag \n
 * Date: 2022-08-05 \n
 */
#include "example_rw.h"
#include "nfc_hal_core.h"
#include "oal_plat_op.h"
#include "securec.h"
#include "nfc_hal_t1t.h"
#include "nfc_hal_t2t.h"
#include "nfc_hal_t3t.h"
#include "nfc_hal_t4t.h"
#include "nfc_hal_t5t.h"
#include "nfc_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DATA_BUF_SIZE                               300
#define NFC_DISCOVERY_TIME                          0
#define PRESENCE_CHECK_TIMEOUT                      300

typedef struct {
    bool isExtended;
    bool isMultiReadEn;
    bool isLockEn;
    bool isSpecialEn;
    uint32_t t5tAreaSize;
} T5tCc;

void NFC_PrintArray(const uint8_t *buff, uint16_t len, const char *str)
{
    (void)buff;
    (void)str;
    PS_PRINT_INFO("%s %d < ", str, len);
    for (uint16_t i = 0; i < len; ++i) {
        PS_PRINT_INFO("%02x,", buff[i]);
    }
    PS_PRINT_INFO("\n");
}

NfcErrorCode NFC_EXAMPLE_ProcessT1T(void)
{
    uint8_t rxBuff[T1T_RALL_RES_LEN] = {0};
    NfcHalT1TRidRes ridRes;
    // T1T Rid
    NfcErrorCode ret = NFC_HAL_T1TPollerProcRid(&ridRes);
    if (ret != NFC_OK) {
        return ret;
    }
    // T1T Rall
    ret = NFC_HAL_T1TPollerProcRall(ridRes.uid, rxBuff, sizeof(rxBuff));
    if (ret != NFC_OK) {
        return ret;
    }
    NFC_PrintArray(rxBuff, T1T_RALL_RES_LEN, "[T1T] RALL");
    // T1T WriteE, for example, write block 2 byte 3 with data = 0xAA.
    uint8_t data = 0xAA;
    ret = NFC_HAL_T1TPollerProcWriteE(ridRes.uid, MAKE_ADD(0x02, 0x03), data);
    if (ret != NFC_OK) {
        return ret;
    }
    return NFC_OK;
}

NfcErrorCode NFC_EXAMPLE_ProcessT2T(void)
{
    uint8_t rxBuff[T2T_READ_DATA_LEN] = {0};
    uint8_t txBuff[T2T_WRITE_DATA_LEN] = {0x11, 0x22, 0x33, 0x44};
    uint16_t rxLen = sizeof(rxBuff);
    // T2T read blockNo = 0
    NfcErrorCode ret = NFC_HAL_T2TPollerProcRead(0x00, rxBuff, rxLen);
    if (ret != NFC_OK) {
        return ret;
    }
    NFC_PrintArray(rxBuff, T2T_READ_DATA_LEN, "[T2T] read");
    // T2T read blockNo = A
    ret = NFC_HAL_T2TPollerProcRead(0x0A, rxBuff, rxLen);
    if (ret != NFC_OK) {
        return ret;
    }
    NFC_PrintArray(rxBuff, T2T_READ_DATA_LEN, "[T2T] read");
    // T2T write blockNo = A
    ret = NFC_HAL_T2TPollerProcWrite(0x0A, txBuff, sizeof(txBuff));
    if (ret != NFC_OK) {
        return ret;
    }
    // If T2T tag support sector select, we can switch to sectorNo = 1
    ret = NFC_HAL_T2TPollerProcSelectSector(0x01);
    if (ret != NFC_OK) {
        return ret;
    }
    return NFC_OK;
}

NfcErrorCode NFC_EXAMPLE_ProcessT3T(NfcTechFParams *techF)
{
    NfcHalT3tCmdParams t3t = {
        .nfcid2 = {0},
        .numberOfServices = 1,
        .serviceCodeList = {0x0B, 0x00}, // service 0x000B
        .numberOfBlock = 1,
        .blockList = {0x80, 0x00}, // block 0
        .blockListLen = 0x02
    };
    if (memcpy_s(t3t.nfcid2, NFCID2_LEN, techF->sensfRes, NFCID2_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    // T3T check service 0x000B, blockNo = 0
    NfcHalT3tCheckRspParams checkRsp;
    (void)memset_s(&checkRsp, sizeof(checkRsp), 0x00, sizeof(checkRsp));
    NfcErrorCode ret = NFC_HAL_T3TPollCheckCmd(&t3t, &checkRsp);
    if (ret != NFC_OK) {
        return ret;
    }
    // T3T check service 0x000B, blockNo = 3
    t3t.blockList[1] = 0x03;
    (void)memset_s(&checkRsp, sizeof(checkRsp), 0x00, sizeof(checkRsp));
    ret = NFC_HAL_T3TPollCheckCmd(&t3t, &checkRsp);
    if (ret != NFC_OK) {
        return ret;
    }
    NFC_PrintArray(checkRsp.blockData, T3T_BLOCK_SIZE * checkRsp.numberOfBlock, "[T3T] read");
    // T3T update service 0x0009, blockNo = 3
    t3t.serviceCodeList[0] = 0x09;
    t3t.blockList[1] = 0x03;
    uint8_t writeData[T3T_BLOCK_SIZE] = {0x11, 0x22, 0x33, 0x44};
    NfcHalT3tUpdateRspParams updateRsp;
    (void)memset_s(&updateRsp, sizeof(updateRsp), 0x00, sizeof(updateRsp));
    ret = NFC_HAL_T3TPollUpdateCmd(&t3t, &updateRsp, writeData, sizeof(writeData));
    if (ret != NFC_OK) {
        return ret;
    }
    // T3T polling cmd
    NfcHalT3tPollCmdParams pollParams = {.sc = 0xFFFF, .rc = 0x01, .tsn = 0x03, .timeout = 0x400};
    NfcHalT3tSensfRes ntfParams;
    (void)memset_s(&ntfParams, sizeof(ntfParams), 0x00, sizeof(ntfParams));
    ret = NFC_HAL_NciT3TPolling(&pollParams, &ntfParams);
    for (uint8_t i = 0; i < ntfParams.resNum; ++i) {
        NFC_PrintArray(ntfParams.resParams[i].nfcid2, 0x8, "T3T polling ntf nfcid");
    }
    return ret;
}

NfcErrorCode NFC_EXAMPLE_DecodeT4TCc(uint8_t *data, uint16_t len, T4tCc *ctx)
{
    if (len < T4T_MIN_CC_LEN) {
        return NFC_ERR_CODE_NDEF_PROC_DETECT_WRONG_TAG;
    }
    uint16_t ccLen = UTIL_Combine8bitTo16bit(data[0], data[1]);
    uint16_t idx = sizeof(uint16_t);
    uint8_t t4tVer = data[idx++];
    ctx->mLe = UTIL_Combine8bitTo16bit(data[idx], data[idx + 1]);
    idx += sizeof(uint16_t);
    ctx->mLc = UTIL_Combine8bitTo16bit(data[idx], data[idx + 1]);
    idx += sizeof(uint16_t);
    PS_PRINT_INFO("[T4T] ccLen = %d, t4tVer = 0x%x, mLe = %d, mLc = %d\n", ccLen, t4tVer, ctx->mLe, ctx->mLc);
    if (t4tVer != T4T_MAP_VER || ccLen < T4T_MIN_CC_LEN) {
        return NFC_ERR_CODE_NDEF_PROC_DETECT_WRONG_TAG;
    }
    uint8_t ctrlType = data[idx++];
    uint8_t ctrlLen = data[idx++];
    PS_PRINT_INFO("[T4T] ctrlType = 0x%x, ctrlLen = %d\n", ctrlType, ctrlLen);
    if (ctrlType != T4T_NDEF_FILE_CTRL_TLV || ctrlLen != 0x06) {
        return NFC_ERR_CODE_NDEF_PROC_DETECT_WRONG_TAG;
    }
    ctx->fileId = UTIL_Combine8bitTo16bit(data[idx], data[idx + 1]);
    idx += sizeof(uint16_t);
    ctx->fileSize = UTIL_Combine8bitTo16bit(data[idx], data[idx + 1]);
    PS_PRINT_INFO("[T4T] fileId = 0x%x, fileSize = %d\n", ctx->fileId, ctx->fileSize);

    return NFC_OK;
}

NfcErrorCode NFC_EXAMPLE_ProcessT4T(void)
{
    uint8_t rApduData[T4T_DATA_MAX_LEN] = {0};
    NfcHalT4tRApdu rApduInfo = { 0x00 };
    rApduInfo.data = rApduData;
    rApduInfo.dataBuffSize = T4T_DATA_MAX_LEN;

    // Select AID
    NfcErrorCode ret = NFC_HAL_T4TPollerSelectAid(&rApduInfo);
    if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
        return ret;
    }
    // Select File
    ret = NFC_HAL_T4TPollerSelectFile(T4T_CC_FILE_ID, &rApduInfo);
    if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
        return ret;
    }
    // Read CC
    ret = NFC_HAL_T4TPollerReadData(0x00, T4T_DATA_MAX_LEN, &rApduInfo);
    if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
        return ret;
    }
    NFC_PrintArray(rApduInfo.data, rApduInfo.dataLen, "[T4T] CC file");
    // Decode T4T CC info
    T4tCc t4tCc;
    ret = NFC_EXAMPLE_DecodeT4TCc(rApduInfo.data, rApduInfo.dataLen, &t4tCc);
    if (ret != NFC_OK) {
        return ret;
    }
    // After reading & decoding CC file info, user should know what T4T files on tag (for example fileId = 0xE104).
    ret = NFC_HAL_T4TPollerSelectFile(t4tCc.fileId, &rApduInfo);
    if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
        return ret;
    }
    // Read T4T NDEF message length
    ret = NFC_HAL_T4TPollerReadData(0x00, 0x02, &rApduInfo);
    if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
        return ret;
    }
    uint16_t ndefMsgLen = UTIL_Combine8bitTo16bit(rApduInfo.data[0], rApduInfo.data[1]);
    PS_PRINT_INFO("[T4T] NDEF msg length = %d\n", ndefMsgLen);
    // Read T4T NDEF message
    uint16_t lenToRead = ndefMsgLen;
    uint16_t readBlkLen = t4tCc.mLe > T4T_DATA_MAX_LEN ? T4T_DATA_MAX_LEN : t4tCc.mLe;
    do {
        uint16_t readLen = lenToRead > readBlkLen ? readBlkLen : lenToRead;
        ret = NFC_HAL_T4TPollerReadData(0x02, readLen, &rApduInfo);
        if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
            return ret;
        }
        NFC_PrintArray(rApduInfo.data, readLen, "[T4T] NDEF");
        lenToRead -= readLen;
    } while (lenToRead > 0);
    // Write Empty Ndef to tag
    uint8_t emptyNdef[] = {0x00, 0x03, 0xD0, 0x00, 0x00};
    return NFC_HAL_T4TPollerWriteData(0x00, emptyNdef, sizeof(emptyNdef), &rApduInfo);
}

NfcErrorCode NFC_DecodeT5TCc(NfcHalT5TReadRes *readRes, NfcHalT5TRWInfo *t5t, T5tCc *t5tCc)
{
    // When first time read CC, should use NFC_HAL_T5TReadSingleBlock with t5t->blockLen = 0, t5t->blockNo = 0.
    // t5t->blockLen value will be set correctly during first read.
    NfcErrorCode ret = NFC_HAL_T5TReadSingleBlock(t5t, readRes);
    if (ret != NFC_OK) {
        return ret;
    }
    PS_PRINT_INFO("[T5T] blocklen = %d\n", t5t->blockLen);
    NFC_PrintArray(readRes->data, t5t->blockLen, "[T5T] read CC part1");
    uint8_t idx = 0;
    t5tCc->isExtended = (readRes->data[idx++] == T5T_2BYTE_ADDR_MODE_SUPPORT);
    ++idx; // jump over version/access byte
    t5tCc->t5tAreaSize = readRes->data[idx++];
    uint8_t feature = readRes->data[idx++];
    t5tCc->isMultiReadEn = (feature & 0x01) != 0x0;
    t5tCc->isLockEn = (feature & 0x08) != 0x0;
    t5tCc->isSpecialEn = (feature & 0x10) != 0x0;
    PS_PRINT_INFO("[T5T] isExtended = %d, isMultiReadEn = %d, isLockEn = %d, isSpecialEn = %d\n",
                  t5tCc->isExtended, t5tCc->isMultiReadEn, t5tCc->isLockEn, t5tCc->isSpecialEn);
    if (t5tCc->t5tAreaSize == 0) {
        if (t5t->blockLen == 0x4) {
            // Need read another four bytes in CC
            t5t->blockNo = 0x1;
            ret = NFC_HAL_T5TReadSingleBlock(t5t, readRes);
            if (ret != NFC_OK) {
                return ret;
            }
            idx = 0;
        }
        idx += sizeof(uint16_t);
        t5tCc->t5tAreaSize = UTIL_Combine8bitTo16bit(readRes->data[idx], readRes->data[idx + 1]);
        NFC_PrintArray(readRes->data, t5t->blockLen, "[T5T] read CC part2");
    }
    t5tCc->t5tAreaSize *= 8; // T5T_Area size measured in bytes is equal to 8 * MLEN.

    return NFC_OK;
}

NfcErrorCode NFC_EXAMPLE_ProcessT5T(NfcTechVParams *techV)
{
    NfcHalT5TRWInfo t5t;
    memset_s(&t5t, sizeof(t5t), 0x00, sizeof(t5t));
    t5t.reqFlag.isAms = true;

    if (memcpy_s(t5t.uid, UID_LEN, techV->uid, UID_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }

    uint8_t rxBuff[T5T_MAX_READ_LEN] = {0};
    NfcHalT5TReadRes readRes = {rxBuff, T5T_MAX_READ_LEN, 0, 0, 0, 0};
    T5tCc t5tCc;
    NfcErrorCode ret = NFC_DecodeT5TCc(&readRes, &t5t, &t5tCc);
    if (ret != NFC_OK) {
        return ret;
    }
    // Try read multiple blocks from blockNo = 0.
    if (t5tCc.isMultiReadEn) {
        t5t.blockNo = 0x0;
        t5t.reqFlag.isOptionFlag = false;
        uint16_t readBlkNum = 2; // only read 3 blocks to test READ_MULTIPLE_BLOCK
        ret = NFC_HAL_T5TReadMultBlock(&t5t, &readRes, readBlkNum);
        if (ret != NFC_OK) {
            return ret;
        }
        NFC_PrintArray(rxBuff, t5t.blockLen * (readBlkNum + 1), "[T5T] multi-read");
    }
    // User should fill writeData, and then write a block. For example, try blockNo = 6.
    uint8_t writeData[T5T_MAX_BLOCK_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint16_t writeLen = t5t.blockLen < T5T_MAX_BLOCK_LEN ? t5t.blockLen : T5T_MAX_BLOCK_LEN;
    t5t.blockNo = 0x6;
    ret = NFC_HAL_T5TWriteSingleBlock(&t5t, writeData, writeLen);
    if (ret != NFC_OK) {
        return ret;
    }
    // Send SlpV cmd to remote to go to Quite state.
    ret = NFC_HAL_T5TSlpV(&t5t);
    if (ret != NFC_OK) {
        return ret;
    }
    // Select remote to go to Selected state.
    ret = NFC_HAL_T5TSelect(&t5t);
    if (ret != NFC_OK) {
        return ret;
    }
    t5t.reqFlag.isSms = true;
    t5t.reqFlag.isAms = false;
    // Lock block
    if (t5tCc.isLockEn) {
        ret = NFC_HAL_T5TLockSingleBlock(&t5t);
        if (ret != NFC_OK) {
            return ret;
        }
    }
    return NFC_OK;
}

NfcErrorCode NFC_EXAMPLE_TagOperation(NfcIntfActInfo *intfActInfo)
{
    if (intfActInfo->protocol == NFC_NCI_RF_PROTOCOL_T1T) {
        return NFC_EXAMPLE_ProcessT1T();
    } else if (intfActInfo->protocol == NFC_NCI_RF_PROTOCOL_T2T) {
        return NFC_EXAMPLE_ProcessT2T();
    } else if (intfActInfo->protocol == NFC_NCI_RF_PROTOCOL_T3T) {
        return NFC_EXAMPLE_ProcessT3T(&intfActInfo->techParams.f);
    } else if (intfActInfo->protocol == NFC_NCI_RF_PROTOCOL_ISO_DEP) {
        return NFC_EXAMPLE_ProcessT4T();
    } else if (intfActInfo->protocol == NFC_NCI_RF_PROTOCOL_T5T) {
        return NFC_EXAMPLE_ProcessT5T(&intfActInfo->techParams.v);
    }

    PS_PRINT_ERR("Tag Operation unknown protocol!\n");
    return NFC_OK;
}

bool NFC_EXAMPLE_PresenceCheck(NfcIntfActInfo *intfActInfo)
{
    NfcErrorCode ret;

    if (intfActInfo->protocol == NFC_NCI_RF_PROTOCOL_T1T) {
        NfcHalT1TRidRes res;
        ret = NFC_HAL_T1TPollerProcRid(&res);
        if (ret != NFC_OK || memcmp(intfActInfo->techParams.a.nfcid1, res.uid, T1T_UID_ECHO_LEN) != 0) {
            return false;
        }
        return true;
    } else if (intfActInfo->protocol == NFC_NCI_RF_PROTOCOL_T2T) {
        uint8_t t2tData[T2T_READ_DATA_LEN] = {0};
        ret = NFC_HAL_T2TPollerProcRead(0x0, t2tData, sizeof(t2tData));
        if (ret != NFC_OK) {
            return false;
        }
        return true;
    } else if (intfActInfo->protocol == NFC_NCI_RF_PROTOCOL_T3T) {
        NfcHalT3tPollCmdParams cmdParams = {.sc = 0xFFFF, .rc = 0x01, .tsn = 0x03, .timeout = PRESENCE_CHECK_TIMEOUT};
        NfcHalT3tSensfRes ntfParams;
        ntfParams.resNum = 0;
        ret = NFC_HAL_NciT3TPolling(&cmdParams, &ntfParams);
        if (ret != NFC_OK || ntfParams.resNum == 0) {
            return false;
        }
        return true;
    } else if (intfActInfo->protocol == NFC_NCI_RF_PROTOCOL_ISO_DEP) {
        bool isPresent = false;
        ret = NFC_HAL_NciIsoDepNakPresenceCheck(&isPresent, PRESENCE_CHECK_TIMEOUT);
        if (ret != NFC_OK) {
            return false;
        }
        return isPresent;
    } else if (intfActInfo->protocol == NFC_NCI_RF_PROTOCOL_T5T) {
        NfcHalT5TRWInfo t5t = {
            .reqFlag = {.isOptionFlag = false, .isAms = false, .isSms = true},
            .uid = {0},
            .blockLen = 0,
            .blockNo = 0
        };
        uint8_t t5tData[NCI_MAX_PAYLOAD_LEN] = {0};
        NfcHalT5TReadRes readRes = {t5tData, NCI_MAX_PAYLOAD_LEN, 0, 0, 0, 0};
        ret = NFC_HAL_T5TReadSingleBlock(&t5t, &readRes);
        if (ret != NFC_OK) {
            return false;
        }
        return true;
    }

    PS_PRINT_ERR("[PresenceCheck] invalid protocol!\n");
    return false;
}

NfcErrorCode NFC_EXAMPLE_RwSetRfParam(void)
{
    uint8_t recvRsp[HAL_MAX_BUF_LEN] = {0};
    uint16_t rspLen = HAL_MAX_BUF_LEN;

    // 参数1：SET_CC_GAIN_MARGIN
    // 参数2：SET_VPA
    // 参数3：SET_PEAK_MAX_N
    // 参数4：SET_SOF_THRES_A_106
    // 参数5：SET_AGC_POLL_MARGIN
    // 参数6：CFG_CORR_SHIFT
    uint8_t setRegCmd[] = {0x2F, 0x12, 0x1F, 0x06,
                           0x04, 0x21, 0x00, 0x00, 0x07,
                           0x04, 0x22, 0x00, 0x00, 0x07,
                           0x04, 0x01, 0x4a, 0x00, 0x0a,
                           0x04, 0x01, 0x02, 0x00, 0x04,
                           0x04, 0x00, 0x03, 0x00, 0x03,
                           0x04, 0x01, 0x10, 0x00, 0x06};

    return NFC_HAL_SendNciCmdAndRecvRsp(setRegCmd, sizeof(setRegCmd) / sizeof(setRegCmd[0]),
                                        recvRsp, &rspLen);
}

NfcErrorCode NFC_EXAMPLE_RwInit(void)
{
    NfcErrorCode ret = NFC_HAL_NciReset();
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_NciInit();
    if (ret != NFC_OK) {
        return ret;
    }

    // 配置参数1:0xA003，关闭状态保护定时器
    // 配置参数2:轮询周期内listen时间 0x1F4 = 500ms
    // 配置参数3:NFCC_CONFIG_CONTROL，允许NFCC整合RF参数
    // 配置参数4:CON_DISCOVERY_PARAM，允许Poll模式
    uint8_t setConfigCmd[] = {0x20, 0x02, 0x0F, 0x04,
                              0xA0, 0x03, 0x01, 0x00,
                              0x00, 0x02, 0xF4, 0x01,
                              0x85, 0x01, 0x01,
                              0x02, 0x01, 0x01};
    ret = NFC_HAL_NciSetConfig(setConfigCmd, sizeof(setConfigCmd));
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_EXAMPLE_RwSetRfParam();
    if (ret != NFC_OK) {
        return ret;
    }

#ifdef FORCE_DEFAULT_CONFIG
    // 开启device低功耗模式（已经是默认配置项）
    ret = NFC_HAL_CfgLowPwr(true);
    if (ret != NFC_OK) {
        return ret;
    }

    // RF接口映射：Poll模式下的IsoDep协议映射到IsoDep接口（已经是默认配置项之一）
    uint8_t rfIntfMapCmd[] = {0x21, 0x00, 0x04, 0x01, 0x04, 0x01, 0x02};
    ret = NFC_HAL_NciDiscovMap(rfIntfMapCmd, sizeof(rfIntfMapCmd));
    if (ret != NFC_OK) {
        return ret;
    }
#endif

    return NFC_OK;
}

NfcErrorCode NFC_EXAMPLE_StartPolling(void)
{
    // 默认开启ABFV四种技术的轮询
    uint8_t list[] = {NFC_NCI_A_PASSIVE_POLL_MODE, NFC_NCI_B_PASSIVE_POLL_MODE,
                      NFC_NCI_F_PASSIVE_POLL_MODE, NFC_NCI_V_PASSIVE_POLL_MODE};
    return NFC_HAL_StartDiscovery(list, sizeof(list));
}

NfcErrorCode NFC_EXAMPLE_StartReaderWriter(void)
{
    uint8_t recvBuffer[DATA_BUF_SIZE];
    uint8_t sendBuffer[DATA_BUF_SIZE];
    uint16_t readLen = DATA_BUF_SIZE;
    NfcIntfActInfo intfActInfo = {0};

    NfcErrorCode ret = NFC_EXAMPLE_RwInit();
    if (ret != NFC_OK) {
        return ret;
    }

    // Example reader start discovery with tech A & B & F & V.
    ret = NFC_EXAMPLE_StartPolling();
    if (ret != NFC_OK) {
        return ret;
    }

    // Discovery time = 0 is wait-forever
    ret = NFC_HAL_PollRecvIntfActNtf(PRIO_FIRST, recvBuffer, sizeof(recvBuffer), NFC_DISCOVERY_TIME);
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_PollParseIntfActNtf(&intfActInfo, &recvBuffer[NCI_HEAD_LEN], recvBuffer[NCI_LEN_POS]);
    if (ret != NFC_OK) {
        return ret;
    }

    // Data exchange option 1: Tag operation API
    // According to intfActInfo.protocol, user can use Tag operation APIs to exchange info with remote.
    ret = NFC_EXAMPLE_TagOperation(&intfActInfo);
    if (ret != NFC_OK) {
        return ret;
    }

    // Data exchange option 2: Raw data exchange
    // sendBuffer should be set by User, and recvBuffer should be checked by User.
    // During data exchange period NFC_HAL_SendDataAndRecvData may be executed several times. */
    ret = NFC_HAL_TranceiveUnchainedData(sendBuffer, 0x10, NCI_STATIC_RF_CONN, recvBuffer, &readLen);
    if (ret != NFC_OK) {
        return ret;
    }

    // Tag presence check
    if (NFC_EXAMPLE_PresenceCheck(&intfActInfo)) {
        PS_PRINT_INFO("[PresenceCheck] card presence check result is true\n");
    } else {
        PS_PRINT_INFO("[PresenceCheck] card presence check result is false\n");
    }

    // Exit NFC reader mode
    ret = NFC_HAL_Deactivate();
    if (ret != NFC_OK) {
        return ret;
    }

    // May choose to enter idle sleep for lowest power mode as "NFC close".
    return NFC_HAL_EnterIdleSleep();
}

#ifdef __cplusplus
}
#endif