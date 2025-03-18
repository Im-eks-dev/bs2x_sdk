/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024 All rights reserved. \n
 *
 * Description: usb serial source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-03-18, Create file. \n
 */

#include "console.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "securec.h"
#include "tcxo.h"
#include "implementation/usb_init.h"
#include "cpu_utils.h"
#include "gadget/usbd_acm.h"
#include "slp_factory.h"
#include "sle_connection_manager.h"
#include "../sle_air_mouse_client/sle_air_mouse_client.h"

#define SERIAL_MANUFACTURER      { 'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 }
#define SERIAL_MANUFACTURER_LEN  20
#define SERIAL_PRODUCT           { 'S', 0, 'L', 0, 'P', 0, ' ', 0, 'T', 0, 'V', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0 }
#define SERIAL_PRODUCT_LEN       22
#define SERIAL_SERIAL            { '2', 0, '0', 0, '2', 0, '4', 0, '0', 0, '3', 0, '1', 0, '8', 0 }
#define SERIAL_SERIAL_LEN        16
#define SERIAL_RET_ERR           (-1)
#define SERIAL_DELAY_MS          (1000)
#define SERIAL_TASK_STACK_SIZE   (0x400)
#define FRAME_DOWNLOAD_OFFSET    0x101000
#define CRC_VERIFY_BUFFER_LEN    25
#define SERIAL_RECV_DATA_MAX_LEN 64
#define USB_RECV_DATA_FAIL_DELAY 50
#define USB_TOGGLE_TASK_SCHEDULE 100
#define USB_SERIAL_INIT_DELAY    2000
#define TEST_BREAK_LEN           (5)
#define SLP_SERIAL_TASK_PRIO     (26)

static char g_usb_serial_recv_data[SERIAL_RECV_DATA_MAX_LEN];

typedef enum {
    SERIAL_CMD_WRITE_CALIPARA_2_ONB = 0,  // 向对端写入校准值
    SERIAL_CMD_READ_CALIPARA_FROM_ONB,    // 从对端读取校准值
    SERIAL_CMD_WRITE_TRX_DELAY_FROM_ONB,  // 向对端写入板级和天线的时延校准值
    SERIAL_CMD_READ_TRX_DELAY_FROM_ONB,   // 从对端读取板级和天线的时延校准值
    SERIAL_CMD_RESTART,                   // 重启测距（通过SLE断连重连）
} serial_cmd_type;

#pragma pack(1)
typedef struct {
    uint8_t cmd_type;
    SlpPhaseCaliPara cali_para;
    uint32_t trxDelay;
} serial_cmd_msg_t;
#pragma pack()

void serial_cmd_proc(serial_cmd_msg_t *serial_cmd_msg)
{
    sle_addr_t *sle_addr = get_sle_air_mouse_server_addr();
    switch (serial_cmd_msg->cmd_type) {
        case SERIAL_CMD_WRITE_CALIPARA_2_ONB:
            SlpWritePhaseCaliPara2Onb(&serial_cmd_msg->cali_para);
            break;
        case SERIAL_CMD_READ_CALIPARA_FROM_ONB:
            SlpReadPhaseCaliParaFromOnb();
            break;
        case SERIAL_CMD_WRITE_TRX_DELAY_FROM_ONB:
            SlpWriteTrxDelay2Onb(serial_cmd_msg->trxDelay);
            break;
        case SERIAL_CMD_READ_TRX_DELAY_FROM_ONB:
            SlpReadTrxDelayFromOnb();
            break;
        case SERIAL_CMD_RESTART:
            sle_disconnect_remote_device(sle_addr);  // 断连
            break;
        default:
            osal_printk("undefined cmd: %u\r\n", serial_cmd_msg->cmd_type);
            break;
    }
}

static int usb_serial_recv_data(void *data)
{
    UNUSED(data);
    usb_serial_ioctl(0, CONSOLE_CMD_RD_BLOCK_SERIAL, 1);
    for (;;) {
        ssize_t recv_len = usb_serial_read(0, g_usb_serial_recv_data, SERIAL_RECV_DATA_MAX_LEN);
        if (recv_len <= 0) {
            osal_msleep(USB_RECV_DATA_FAIL_DELAY);
            osal_printk("serial recv fail\r\n");
            continue;
        }

        if (recv_len == sizeof(serial_cmd_msg_t)) {  // 校准命令
            serial_cmd_msg_t serial_cmd_msg;
            (void)memcpy_s(&serial_cmd_msg, sizeof(serial_cmd_msg_t), &g_usb_serial_recv_data, recv_len);
            serial_cmd_proc(&serial_cmd_msg);
            osal_printk("recv_cmd, len, %u, cmd, %u\r\n", recv_len, serial_cmd_msg.cmd_type);
        } else {
            osal_printk("recv_len: %u\r\n", recv_len);
        }

        usb_serial_write(0, g_usb_serial_recv_data, recv_len);

        if (memcmp(g_usb_serial_recv_data, "break", TEST_BREAK_LEN) == 0) {
            break;
        }
    }
    return -1;
}

int dongle_usb_serial_init(void)
{
    int ret;
    const char manufacturer[SERIAL_MANUFACTURER_LEN] = SERIAL_MANUFACTURER;
    struct device_string str_manufacturer = {.str = manufacturer, .len = SERIAL_MANUFACTURER_LEN};

    const char product[SERIAL_PRODUCT_LEN] = SERIAL_PRODUCT;
    struct device_string str_product = {.str = product, .len = SERIAL_PRODUCT_LEN};

    const char serial[SERIAL_SERIAL_LEN] = SERIAL_SERIAL;
    struct device_string str_serial_number = {.str = serial, .len = SERIAL_SERIAL_LEN};

    struct device_id dev_id = {.vendor_id = 0x1111, .product_id = 0xd, .release_num = 0x0119};

    ret = usbd_set_device_info(DEV_SERIAL, &str_manufacturer, &str_product, &str_serial_number, dev_id);
    if (ret != 0) {
        osal_printk("set device info fail!\n");
        return -1;
    }

    if (usb_device_set_speed(USB_FULL_SPEED) != 0) {
        osal_printk("usb set speed failed!\n");
        return -1;
    }

    ret = usb_init(DEVICE, DEV_SERIAL);
    if (ret != 0) {
        osal_printk("usb_init failed!\n");
        return -1;
    }
    osal_msleep(USB_SERIAL_INIT_DELAY);

    osal_task *task_handle = NULL;
    osal_kthread_lock();  // 创建任务前锁中断
    task_handle = osal_kthread_create(
        (osal_kthread_handler)usb_serial_recv_data, 0, "SLPFactoryTestSerialTask", SERIAL_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLP_SERIAL_TASK_PRIO);  // 设置任务优先级
    }
    osal_kthread_unlock();

    return 0;
}
