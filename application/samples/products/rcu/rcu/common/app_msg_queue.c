/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: MSG QUEUE. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-16, Create file. \n
 */

#include "app_init.h"
#include "osal_debug.h"
#include "osal_addr.h"
#include "osal_msgqueue.h"
#include "app_msg_queue.h"

unsigned long g_app_msg_queue_id;

void app_create_msgqueue(void)
{
    if (osal_msg_queue_create("app msg queue", APP_MAX_MSG_COUNT, (unsigned long *)&g_app_msg_queue_id,
                              0, sizeof(app_msg_data_t)) != 0) {
        osal_printk("app msg queue create fail\n");
        return;
    }
}

void app_delete_msgqueue(void)
{
    osal_msg_queue_delete(g_app_msg_queue_id);
}

void app_write_msgqueue(app_msg_data_t msg)
{
    if ((void *)&msg == NULL) {
        osal_printk("message write fail!\r\n");
        return;
    }
    osal_msg_queue_write_copy(g_app_msg_queue_id, (void *)&msg, (uint32_t)sizeof(app_msg_data_t), 0);
}

uint32_t app_receive_msgqueue(uint8_t* buf, uint32_t* len)
{
    uint32_t ret;
    if (buf == NULL) {
        osal_printk("message receive fail!\r\n");
        return -1;
    }
    ret = osal_msg_queue_read_copy(g_app_msg_queue_id, (void *)buf, len, APP_MSG_WAIT_FOREVER);
    if (ret != 0) {
        osal_printk("osal_msg_queue_read_copy failed ret = 0x%x!\r\n", ret);
    }
    return ret;
}