/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Microphone USB Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-12, Create file. \n
 */
#include "osal_debug.h"
#include "tcxo.h"
#include "gadget/f_uac.h"
#include "implementation/usb_init.h"
#include "securec.h"
#include "sle_micro_usb.h"

#define UAC_MANUFACTURER        { 'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 }
#define UAC_MANUFACTURER_LEN    20
#define UAC_PRODUCT             { 'H', 0, 'H', 0, '6', 0, '6', 0, '6', 0, '6', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0 }
#define UAC_PRODUCT_LEN         22
#define UAC_SERIAL              { '2', 0, '0', 0, '2', 0, '0', 0, '0', 0, '6', 0, '2', 0, '4', 0 }
#define UAC_SERIAL_LEN          16
#define SLE_VDT_UAC_BUFFER_COUNT            2
#define SLE_VDT_UAC_BUFFER_LEN              768

int32_t sle_micro_usb_uac_init(void)
{
    const char manufacturer[UAC_MANUFACTURER_LEN] = UAC_MANUFACTURER;
    struct device_string str_manufacturer = {
        .str = manufacturer,
        .len = UAC_MANUFACTURER_LEN
    };

    const char product[UAC_PRODUCT_LEN] = UAC_PRODUCT;
    struct device_string str_product = {
        .str = product,
        .len = UAC_PRODUCT_LEN
    };

    const char serial[UAC_SERIAL_LEN] = UAC_SERIAL;
    struct device_string str_serial_number = {
        .str = serial,
        .len = UAC_SERIAL_LEN
    };

    struct device_id dev_uac_id = {
        .vendor_id = 0x1111,
        .product_id = 0x0009,
        .release_num = 0x0318
    };

    uint32_t ret = usbd_set_device_info(DEV_UAC, &str_manufacturer, &str_product, &str_serial_number, dev_uac_id);
    if (ret != 0) {
        osal_printk("%s set device info fail!\n", MICROPHONE_USB_LOG);
        return 1;
    }

    ret = usb_init(DEVICE, DEV_UAC);
    if (ret != 0) {
        osal_printk("%s usb_init failed!\n", MICROPHONE_USB_LOG);
        return 1;
    }

    ret = (uint32_t)uac_wait_host(UAC_WAIT_HOST_FOREVER);
    if (ret == 0) {
        osal_printk("%s host connected, start test\r\n", MICROPHONE_USB_LOG);
    } else {
        osal_printk("%s host can`t connect\r\n", MICROPHONE_USB_LOG);
        return 1;
    }
    if (fuac_reqbuf_init(SLE_VDT_UAC_BUFFER_COUNT, SLE_VDT_UAC_BUFFER_LEN) != UAC_OK) {
        osal_printk("%s fuac eqbuf init fail.\r\n", MICROPHONE_USB_LOG);
        return 1;
    }
    return 0;
}

int32_t sle_micro_usb_uac_send_data(const uint8_t *buf, int len)
{
    uint32_t uac_buf_index;
    uint8_t *uac_buf = fuac_reqbuf_get(&uac_buf_index);
    if (uac_buf == NULL) {
        osal_printk("%s fuac reqbuf get failed.\r\n", MICROPHONE_USB_LOG);
        return UAC_ERROR_FATAL;
    }
    if (memcpy_s(uac_buf, SLE_VDT_UAC_BUFFER_LEN, buf, len) != EOK) {
        osal_printk("%s memcpy uac_buf fail.\r\n", MICROPHONE_USB_LOG);
        return UAC_ERROR_MEMORY;
    }
    return fuac_send_message((void *)(uintptr_t)uac_buf, len, uac_buf_index);
}