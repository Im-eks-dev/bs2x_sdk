/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: Implementation of nfc sle message queue. \n
 * Author: @CompanyNameTag \n
 * Date: 2023-06-28 \n
 */
#include "nfc_sle_queue.h"
#include "oal_os.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_SLE_MSG_POOL_LEN      20

typedef struct {
    struct MsgNode nfcSleMsgPool[NFC_SLE_MSG_POOL_LEN];
    uint8_t front;
    uint8_t rear;
    uint8_t num;
} MsgQueueInfo;

oal_mutex_t g_queueMutex;
MsgQueueInfo g_nfcSleMsgQueue;

static void QueueInit(void)
{
    g_nfcSleMsgQueue.num  = 0;
    g_nfcSleMsgQueue.front = 0;
    g_nfcSleMsgQueue.rear = 0;
}

void NFC_SLE_MsgQueueInit(void)
{
    QueueInit();
    oal_mutex_init(&g_queueMutex);
}

uint8_t GetMsgQueueLen(void)
{
    return g_nfcSleMsgQueue.num;
}

static void AddToMsgQueue(uint8_t slaveNum, uint8_t *data, uint16_t len)
{
#ifdef NFC_SLE_SLAVE_SAMPLE
    UNUSED(slaveNum);
#endif

    oal_mutex_lock(&g_queueMutex);
    g_nfcSleMsgQueue.nfcSleMsgPool[g_nfcSleMsgQueue.rear].data = data;
    g_nfcSleMsgQueue.nfcSleMsgPool[g_nfcSleMsgQueue.rear].len = len;
#ifdef NFC_SLE_MASTER_SAMPLE
    g_nfcSleMsgQueue.nfcSleMsgPool[g_nfcSleMsgQueue.rear].slaveNum = slaveNum;
#endif
    g_nfcSleMsgQueue.rear = (g_nfcSleMsgQueue.rear + 1) % NFC_SLE_MSG_POOL_LEN;
    g_nfcSleMsgQueue.num++;
    oal_mutex_unlock(&g_queueMutex);
}

int32_t NFC_SLE_MsgAdd(uint8_t slaveNum, uint8_t *data, uint16_t len)
{
    if (GetMsgQueueLen() >= NFC_SLE_MSG_POOL_LEN) {
        return -1;
    }

    uint8_t *msg = oal_malloc(len);
    if (msg == NULL) {
        return -1;
    }

    if (memcpy_s(msg, len, data, len) != EOK) {
        oal_free(msg);
        return -1;
    }
    AddToMsgQueue(slaveNum, msg, len);

    return 0;
}

struct MsgNode* NFC_SLE_MsgGet()
{
    struct MsgNode *node;

    oal_mutex_lock(&g_queueMutex);
    if (GetMsgQueueLen() == 0) {
        node = NULL;
        oal_mutex_unlock(&g_queueMutex);
    } else {
        node = &g_nfcSleMsgQueue.nfcSleMsgPool[g_nfcSleMsgQueue.front];
        g_nfcSleMsgQueue.front = (g_nfcSleMsgQueue.front + 1) % NFC_SLE_MSG_POOL_LEN;
        g_nfcSleMsgQueue.num--;
    }
    oal_mutex_unlock(&g_queueMutex);

    return node;
}

#ifdef __cplusplus
}
#endif
