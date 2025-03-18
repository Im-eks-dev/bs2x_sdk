/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: CAN Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-02-04, Create file. \n
 */
#include "boards.h"
#include "pinctrl.h"
#include "can.h"
#include "soc_osal.h"
#include "app_init.h"

#define CAN_TASK_PRIO                     24
#define CAN_TASK_STACK_SIZE               0x400
#define CAN_SEND_DELAY                    100
#define CAN_SEND_FAIL_DELAY               1000
#define CAN_MESSAGE_MAX_LENGTH            64

uint8_t g_data[CAN_MESSAGE_MAX_LENGTH] = {0};

static void test_stub_can_fd_callback(can_int_sts_t status, can_msg_attr_t *received_msg,
                                      uint8_t *data, uint8_t length)
{
    osal_printk("CAN sts : %x\n", status);
    if (status == CAN_FRAME_RECEIVED_INT) {
#if defined(CONFIG_CAN_SAMPLE_ENABLE_BUS_LOAD_TEST)
        unused(data);
        unused(received_msg);
        unused(length);
        can_msg_attr_t can_msg0 = build_can_fd_format_standard_data_frame(0x600, 1);
        errcode_t ret = uapi_can_send_msg(CAN_BUS_0, &can_msg0, g_data, CAN_MESSAGE_MAX_LENGTH);
        if (ret != ERRCODE_SUCC) {
            osal_printk("send_failed %x\r\n", ret);
        }
#else /* CONFIG_CAN_SAMPLE_ENABLE_BUS_LOAD_TEST */
        osal_printk("id = %x, Byte0 = %x, length = %d\n", received_msg->std_id, data[0], length);
#endif /* !CONFIG_CAN_SAMPLE_ENABLE_BUS_LOAD_TEST */
    } else {
        uint8_t trans_err_count = uapi_can_get_transmit_error_count(CAN_BUS_0);
        uint8_t recv_err_count = uapi_can_get_receive_error_count(CAN_BUS_0);
        osal_printk("tx err cnt = %x, rx err cnt = %x", trans_err_count, recv_err_count);
    }
}

static void *can_task(const char *arg)
{
    unused(arg);
    for (int i = 0; i < CAN_MESSAGE_MAX_LENGTH; i++) {
        g_data[i] = i;
    }
#ifndef CONFIG_CAN_SAMPLE_ENABLE_BUS_LOAD_TEST
    can_msg_attr_t can_msg0 = build_can_fd_format_standard_data_frame(0x100, 1);
    can_msg_attr_t can_msg1 = build_can_fd_format_standard_data_frame(0x101, 1);
    while (1) {
        g_data[0] = 0x1; /* 无意义数，仅做报文区分用。 */
        errcode_t ret = uapi_can_send_msg(CAN_BUS_0, &can_msg0, g_data, CAN_MESSAGE_MAX_LENGTH);
        osal_msleep(CAN_SEND_DELAY);
        g_data[0] = 0x2; /* 无意义数，仅做报文区分用。 */
        ret |= uapi_can_send_msg(CAN_BUS_0, &can_msg1, g_data, CAN_MESSAGE_MAX_LENGTH);
        osal_msleep(CAN_SEND_DELAY);
        if (ret != ERRCODE_SUCC) {
            osal_printk("send_failed %x\r\n", ret);
            osal_msleep(CAN_SEND_FAIL_DELAY);
        }
    }
#endif
    return NULL;
}

static void can_entry(void)
{
    osal_printk("start CAN sample test\r\n");
    can_global_config_t global_config = {
        .std_baud_rate = CAN_BPS_500K,
        .fd_baud_rate = CAN_FD_BPS_2M,
        .point = CAN_SAMPLE_PERCENT_80,
        .can_mode = CONFIG_CAN_SAMPLE_ENABLE_LOOPBACK,
    };

    can_filter_t std_filter;

    std_filter.fid1 = 0x101;
    std_filter.fid2 = 0x201;
    std_filter.filter_type = RANGE_FILTER;
    std_filter.fifo_id = 0;

    can_filter_list_t std_list = {
        .filters = &std_filter,
        .filter_num = 1
    };
    errcode_t ret = uapi_can_init(CAN_BUS_0, &global_config, &std_list, NULL);
    if (ret != ERRCODE_SUCC) {
        osal_printk("CAN init failed. Retval:%x\n", ret);
        return;
    }

    ret = uapi_can_callback_register(CAN_BUS_0, test_stub_can_fd_callback);
    if (ret != ERRCODE_SUCC) {
        osal_printk("CAN register failed. Retval:%x\n", ret);
        return;
    }

    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)can_task, 0, "CANTask", CAN_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, CAN_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the button_entry. */
app_run(can_entry);