/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved. \n
 * Description: nfc ce example, ExampleCardEmulation should enable build macro HAVE_NFC_LISTEN \n
 * Author: @CompanyNameTag \n
 * Date: 2022-08-05 \n
 */
#include "example_ce.h"
#include "nfc_hal_core.h"
#include "oal_plat_op.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_DISCOVERY_TIME                          0
#define LISTEN_ROUTE_DEFAULT_POWER_MODE             0x3b
#define LISTEN_ROUTE_TECH_BASED_QUALIF_TYPE         0x00
#define LISTEN_ROUTE_TECH_BASED_VAL_LEN             0x03
#define LISTEN_ROUTE_LAST_CFG_MSG                   0
#define HCP_HEADER_LEN                              2

NfcErrorCode NFC_EXAMPLE_ProcessHciFromNfcee(uint8_t *buff, uint16_t len)
{
    if (len < NCI_HEAD_LEN + HCP_HEADER_LEN) {
        return NFC_OK;
    }

    uint8_t rsp[] = {0x01, 0x00, 0x02, 0x81, 0x80};
    if (buff[NCI_HEAD_LEN] == 0x81 && buff[NCI_HEAD_LEN + 1] == 0x15) {
        // ADM_NOTIFY_ALL_PIPE_CLEARED_CMD
        return NFC_HAL_SendData(rsp, sizeof(rsp), NCI_STATIC_HCI_CONN);
    } else if (buff[NCI_HEAD_LEN] == 0x81 && buff[NCI_HEAD_LEN + 1] == 0x12) {
        // ADM_NOTIFY_PIPE_CREATED_CMD
        return NFC_HAL_SendData(rsp, sizeof(rsp), NCI_STATIC_HCI_CONN);
    } else if (buff[NCI_HEAD_LEN + 1] == 0x03) {
        // ANY_OPEN_PIPE_CMD
        uint8_t openPipeRsp[] = {0x01, 0x00, 0x03, 0x92, 0x80, 0x00};
        openPipeRsp[NCI_HEAD_LEN] = buff[NCI_HEAD_LEN];
        return NFC_HAL_SendData(openPipeRsp, sizeof(openPipeRsp), NCI_STATIC_HCI_CONN);
    }
    return NFC_OK;
}

NfcErrorCode NFC_EXAMPLE_SetListenRouteTbl(uint8_t nfceeId, uint8_t *techModeList, uint8_t num)
{
    uint8_t routeCmd[HAL_MAX_BUF_LEN] = {0x21, 0x01, 0x00, 0x00, 0x00};
    uint16_t idx = NCI_HEAD_LEN;
    routeCmd[idx++] = LISTEN_ROUTE_LAST_CFG_MSG;
    routeCmd[idx++] = num;
    for (uint8_t i = 0; i < num; ++i) {
        routeCmd[idx++] = LISTEN_ROUTE_TECH_BASED_QUALIF_TYPE;
        routeCmd[idx++] = LISTEN_ROUTE_TECH_BASED_VAL_LEN;
        routeCmd[idx++] = nfceeId;
        routeCmd[idx++] = LISTEN_ROUTE_DEFAULT_POWER_MODE;
        routeCmd[idx++] = (techModeList[i] - 0x80); // tech-mode to tech for listen
    }
    routeCmd[NCI_LEN_POS] = idx - NCI_HEAD_LEN;
    return NFC_HAL_SetListenRouteTbl(routeCmd, idx);
}

void NFC_EXAMPLE_ProcessCardEmuOnce(uint32_t timeout)
{
    // Wait for first business NTF with indicated timeout
    uint8_t recvBuff[HAL_SHORT_BUFF_LEN] = {0};
    uint16_t len = HAL_SHORT_BUFF_LEN;
    NfcErrorCode ret = NFC_HAL_ReceiveNciPkt(recvBuff, &len, timeout);
    if (ret != NFC_OK || len < NCI_HEAD_LEN) {
        return;
    }

    do {
        // CardEmu from Sim/SE only send NTF to DH, can just receive and print/ignore them.
        // Following NTFs inside one time business should come within NCI_READ_TO timeout.
        len = HAL_SHORT_BUFF_LEN;
        ret = NFC_HAL_ReceiveNciPkt(recvBuff, &len, NCI_READ_TO);
    } while (ret == NFC_OK && len > NCI_HEAD_LEN);

    // No more NTF, this example treats it as business end when RW leave.
    return;
}

NfcErrorCode NFC_EXAMPLE_CeInit(FuncProcessHciFromNfcee NFC_ProcessHciFromNfcee)
{
    NfcHalCoreParams params = {NCI_READ_TO, NCI_READ_DATA_TO, NFC_ProcessHciFromNfcee};
    NFC_HAL_SetCoreParams(&params);

    NfcErrorCode ret = NFC_HAL_NciReset();
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_NciInit();
    if (ret != NFC_OK) {
        return ret;
    }

    // 配置参数1：0xA300， nfcee enable bit，默认使能eSE，产品根据芯片类型和使用场景来配置
    uint8_t setConfigCmd[] = {0x20, 0x02, 0x05, 0x01, 0xA3, 0x00, 0x01, 0x04};
    ret = NFC_HAL_NciSetConfig(setConfigCmd, sizeof(setConfigCmd));
    if (ret != NFC_OK) {
        return ret;
    }

#ifdef FORCE_DEFAULT_CONFIG
    // 开启device低功耗模式（已经是默认配置项）
    ret = NFC_HAL_CfgLowPwr(true);
    if (ret != NFC_OK) {
        return ret;
    }

    // RF接口映射：Poll模式下的IsoDep协议映射到IsoDep接口（已经是默认映射项之一）
    uint8_t rfIntfMapCmd[] = {0x21, 0x00, 0x04, 0x01, 0x04, 0x02, 0x02};
    ret = NFC_HAL_NciDiscovMap(rfIntfMapCmd, sizeof(rfIntfMapCmd));
    if (ret != NFC_OK) {
        return ret;
    }
#endif

    return NFC_OK;
}

NfcErrorCode NFC_EXAMPLE_StartCardEmulation(void)
{
    NfcErrorCode ret = NFC_EXAMPLE_CeInit(NFC_EXAMPLE_ProcessHciFromNfcee);
    if (ret != NFC_OK) {
        return ret;
    }

    // HCI Retention RAM info restore for host reboot case. Below is just an example without init.
    // For normal usage, hciRamBuff and hciRamInfoLen should be initialized by reading HCI retention ram info
    // from flash/file for each nfcee. Please note: hci ram info should be set from addr = hciRamBuff + 3,
    // the header 3 bytes are reserved for NCI header setting.
    uint8_t hciRamBuff[HAL_MAX_BUF_LEN];
    uint16_t hciRamInfoLen = NCI_MAX_PAYLOAD_LEN;
    ret = NFC_HAL_RestoreHciRam(hciRamBuff, hciRamInfoLen);
    if (ret != NFC_OK) {
        // Restore is just for avoiding session init when reboot for known simCard/SE.
        // Restore failure doesn't mean following business failure.
    }

    // NFCEE discovery, get available nfcee list
    uint8_t nfceeList[HAL_SHORT_BUFF_LEN] = {0};
    uint8_t num = 0;
    ret = NFC_HAL_NfceeDiscovery(nfceeList, HAL_SHORT_BUFF_LEN, &num);
    if (ret != NFC_OK) {
        return ret;
    }

    // NFCEE enable, example card emulation only enable first nfcee
    uint8_t techModeList[HAL_SHORT_BUFF_LEN] = {0};
    num = 0;
    // Handle RF discov request, HCI pipe cmd, and nfcee enable ntf
    ret = NFC_HAL_NfceeEnable(nfceeList[0], techModeList, HAL_SHORT_BUFF_LEN, &num);
    if (ret != NFC_OK) {
        return ret;
    }

    // Set listen route table, for nfcee only add tech based route
    ret = NFC_EXAMPLE_SetListenRouteTbl(nfceeList[0], techModeList, num);
    if (ret != NFC_OK) {
        return ret;
    }

    // Example card emulation start listening
    ret = NFC_HAL_StartDiscovery(techModeList, num);
    if (ret != NFC_OK) {
        return ret;
    }

    // Listen time = 0 is wait-forever for first business NTF
    // This example only handle CE business one time, e.g., move RW close to CE device and move it away once.
    NFC_EXAMPLE_ProcessCardEmuOnce(NFC_DISCOVERY_TIME);

    // Exit NFC cardEmu mode
    ret = NFC_HAL_Deactivate();
    if (ret != NFC_OK) {
        return ret;
    }

    // HCI Retention RAM info save. This example just get info for the first found nfcee.
    ret = NFC_HAL_GetHciRam(nfceeList[0], hciRamBuff, &hciRamInfoLen);
    if (ret == NFC_OK) {
        // User should save info from addr = hciRamBuff + 4 with length = hciRamInfoLen - 4,
        // into flash/file after successfully calling NFC_HAL_StorePropHciRam.
    }

    // May choose to enter idle sleep for lowest power mode as "NFC close".
    return NFC_HAL_EnterIdleSleep();
}

#ifdef __cplusplus
}
#endif