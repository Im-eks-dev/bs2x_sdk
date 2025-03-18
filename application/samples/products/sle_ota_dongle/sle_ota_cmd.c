/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: SLE OTA Recv Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-02-01, Create file. \n
 */
#include "securec.h"
#include "common_def.h"
#include "bts_le_gap.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ota_client.h"
#include "gadget/f_hid.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "sle_ota_cmd_handler.h"
#include "sle_ota_cmd.h"

#define WRITE_REQ_BUFFER_LEN        256
#define FRAME_8_BIT_SHIFT           8

static uint16_t g_reassembly_len = 0;
static uint16_t g_reassembly_offset = 0;
static uint8_t  g_reassembly_expect_seq = 0;
static uint8_t g_reassembly_buff[WRITE_REQ_BUFFER_LEN] = { 0 };


typedef errcode_t (*sle_ota_dongle_pkt_recv_hook)(uint8_t service_id, uint8_t command_id,
                                                  uint8_t *buffer, uint16_t length);
typedef struct {
    uint8_t service_id;
    sle_ota_dongle_pkt_recv_hook handler;
} sle_ota_dongle_service_handler_t;

static sle_ota_dongle_service_handler_t g_sle_ota_dongle_cmd_id_tbl[] = {
    { SERVICE_ID_SERVICE_DISCOVER,                  sle_ota_discover_device_service },
    { SERVICE_ID_SERVICE_CONNECT,                   sle_ota_manage_connection_service },
    { SERVICE_ID_CLIENT_MANAGE,                     sle_ota_manage_ssap_client },
    { SERVICE_ID_SERVICE_MANAGE,                    NULL },
    { SERVICE_ID_FACTORY_TEST_SERVICE,              NULL },
    { SERVICE_ID_LOW_LATENCY_SERVICE_MANAGE,        NULL },
};

static errcode_t sle_ota_dongle_cmd_receiver(uint8_t service_id, uint8_t command_id, uint8_t *data, uint16_t len)
{
    uint32_t i;
    for (i = 0; i < sizeof(g_sle_ota_dongle_cmd_id_tbl) / sizeof(g_sle_ota_dongle_cmd_id_tbl[0]); i++) {
        sle_ota_dongle_service_handler_t *item = &g_sle_ota_dongle_cmd_id_tbl[i];
        if (item->service_id == service_id && item->handler != NULL) {
            item->handler(service_id, command_id, data, len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

errcode_t sle_ota_recv_handler(uint8_t *data, uint16_t length)
{
    sle_ota_frame_header_t *head = (sle_ota_frame_header_t *)((uint8_t *)data);
    uint16_t len = 0;
    if (head->flag != SLE_LINK_FRAME_HEAD_FLAG) {
        return ERRCODE_NOT_SUPPORT;
    }
    if (head->total_frame <= 1) {
        /* 不需要组包 */
        (void)memset_s(g_reassembly_buff, WRITE_REQ_BUFFER_LEN, 0, WRITE_REQ_BUFFER_LEN);
        sle_ota_frame_header_t *req = (sle_ota_frame_header_t *)((uint8_t *)data);
        return sle_ota_dongle_cmd_receiver(req->service_id, req->command_id, data, length);
    }

    /* 组包 */
    if ((head->frame_seq == 0) && (g_reassembly_expect_seq == head->frame_seq)) {
        (void)memset_s(g_reassembly_buff, WRITE_REQ_BUFFER_LEN, 0, WRITE_REQ_BUFFER_LEN);
        g_reassembly_offset = 0;
        /* Bodylen长度 */
        uint16_t body_len = data[SLE_LINK_FRAME_BODY_LEN_INDEX] +
                            (data[SLE_LINK_FRAME_BODY_LEN_INDEX + 1] << FRAME_8_BIT_SHIFT);
        g_reassembly_len = SLE_LINK_FRAME_HEAD_LEN + SLE_LINK_FRAME_PAYLOAD_HEAD_LEN + body_len +
                           SLE_LINK_FRAME_MIC_LEN;
        len = length - SLE_LINK_FRAME_MIC_LEN;
        if (memcpy_s(g_reassembly_buff, len, data, len) != EOK) {
            return ERRCODE_FAIL;
        }
        g_reassembly_offset = len;
        g_reassembly_expect_seq += 1;
    } else if ((head->frame_seq == (head->total_frame - 1)) && (g_reassembly_expect_seq == head->frame_seq)) {
        /* 最后一帧 */
        /* 还未接收body长度 */
        len = g_reassembly_len - g_reassembly_offset;
        if (memcpy_s(g_reassembly_buff + g_reassembly_offset, len, data + SLE_LINK_FRAME_HEAD_LEN, len) != EOK) {
            return ERRCODE_FAIL;
        }
        g_reassembly_expect_seq = 0;
        sle_ota_frame_header_t *req = (sle_ota_frame_header_t *)((uint8_t *)g_reassembly_buff);
        return sle_ota_dongle_cmd_receiver(req->service_id, req->command_id, g_reassembly_buff, g_reassembly_len);
    } else if (g_reassembly_expect_seq == head->frame_seq) {
        len = length - SLE_LINK_FRAME_HEAD_LEN - SLE_LINK_FRAME_MIC_LEN;
        if (memcpy_s(g_reassembly_buff + g_reassembly_offset, len, data + SLE_LINK_FRAME_HEAD_LEN, len) != EOK) {
            return ERRCODE_FAIL;
        }
        g_reassembly_offset += len;
        g_reassembly_expect_seq += 1;
    } else {
        g_reassembly_expect_seq = 0;
        g_reassembly_offset = 0;
        g_reassembly_len = 0;
    }
    return ERRCODE_SUCC;
}