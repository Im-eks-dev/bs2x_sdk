/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Sample usb dfu source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-11-29, Create file. \n
 */
#include <stdint.h>
#include "gadget/f_dfu.h"
#include "soc_osal.h"
#include "securec.h"
#include "partition.h"
#ifdef SUPPORT_CFBB_UPG
#include "upg.h"
#endif
#include "watchdog.h"
#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_EMBED_FLASH
#include "eflash.h"
#endif
#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_SFC
#include "sfc.h"
#endif
#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_EXTERN_FLASH
#include "flash.h"
#endif
#include "implementation/usb_init.h"
#include "memory_config_common.h"
#include "uapi_crc.h"
#include "cpu_utils.h"

#define DFU_MANUFACTURER  { 'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 }
#define DFU_MANUFACTURER_LEN   20
#define DFU_PRODUCT  { 'H', 0, 'H', 0, '6', 0, '6', 0, '6', 0, '6', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0 }
#define DFU_PRODUCT_LEN        22
#define DFU_SERIAL   { '2', 0, '0', 0, '2', 0, '0', 0, '0', 0, '6', 0, '2', 0, '4', 0 }
#define DFU_SERIAL_LEN         16
#define DFU_DELAY_MS           (200)
#define DFU_TIMEOUT_MS         (1000 * 80)
#define DFU_RESET_WAIT_MS      (20)

#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_EMBED_FLASH
#define DFU_DOWNLOAD_FLASH_START   EMBED_FLASH_START
#endif
#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_SFC
#define DFU_DOWNLOAD_FLASH_START   SFC_FLASH_START
#endif
#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_EXTERN_FLASH
#define DFU_DOWNLOAD_FLASH_START   FLASH_START
#define DFU_DOWNLOAD_PARTITION_OFFSET   EXTERN_FLASH_PARTITION_OFFSET
#endif
static uintptr_t g_dfu_addr = (uintptr_t)0;
static uint32_t g_dfu_write_len = 0;

static uint32_t g_dfu_start_addr = 0;
static uint32_t g_dfu_size = 0;

typedef struct {
    uint32_t start_flag;
    uint16_t packet_size;
    uint8_t frame_type;
    uint8_t frame_type_reverse;
    uint32_t file_addr;
    uint32_t file_len;
    uint32_t erase_size;
    uint8_t formal;
    uint8_t formal_reverse;
    uint16_t checksum;
} dfu_packge_header_t;

int sample_usb_dfu_init(void)
{
    int ret;
    const char manufacturer[DFU_MANUFACTURER_LEN] = DFU_MANUFACTURER;
    struct device_string str_manufacturer = {
        .str = manufacturer,
        .len = DFU_MANUFACTURER_LEN
    };

    const char product[DFU_PRODUCT_LEN] = DFU_PRODUCT;
    struct device_string str_product = {
        .str = product,
        .len = DFU_PRODUCT_LEN
    };

    const char serial[DFU_SERIAL_LEN] = DFU_SERIAL;
    struct device_string str_serial_number = {
        .str = serial,
        .len = DFU_SERIAL_LEN
    };

    struct device_id dev_id = {
        .vendor_id = 0x1111,
        .product_id = 0xff03,
        .release_num = 0x0119
    };
#if defined(SSB_FOTA_OFFSET) && SSB_FOTA_OFFSET != 0 && !defined(SUPPORT_CFBB_UPG)
    g_dfu_start_addr = SSB_FOTA_OFFSET;
    g_dfu_size = SSB_FOTA_LENGTH;
#else
    partition_information_t info;
    ret = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret != ERRCODE_SUCC || info.part_info.addr_info.addr == 0 || info.part_info.addr_info.size == 0) {
        return -1;
    }
    g_dfu_start_addr = info.part_info.addr_info.addr;
    g_dfu_size = info.part_info.addr_info.size;
#endif
#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_EXTERN_FLASH
    g_dfu_addr = (uintptr_t)(g_dfu_start_addr - DFU_DOWNLOAD_PARTITION_OFFSET);
#else
    g_dfu_addr = (uintptr_t)g_dfu_start_addr;
#endif

    ret = usbd_set_device_info(DEV_DFU, &str_manufacturer, &str_product, &str_serial_number, dev_id);
    if (ret != 0) {
        return -1;
    }

    ret = usb_init(DEVICE, DEV_DFU);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

static void usb_dfu_download_fail(void)
{
    osal_msleep(DFU_RESET_WAIT_MS);
#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_EMBED_FLASH
    uapi_eflash_erase(g_dfu_start_addr, g_dfu_write_len);
#endif
#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_SFC
    uapi_sfc_reg_erase(g_dfu_start_addr, g_dfu_write_len);
#endif
#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_EXTERN_FLASH
    uapi_flash_block_erase(0, g_dfu_start_addr, g_dfu_write_len, true);
#endif
    cpu_utils_reset_chip_with_log((cores_t)APPS, REBOOT_CAUSE_DFU_UPG_FAIL);
}

void sample_usb_dfu_wait_ugrade_done_and_reset(void)
{
    uint32_t count = 0;
    while (1) {
#ifdef CONFIG_DRIVERS_USB_DFU_GADGET
        if (usb_dfu_update_status() == 1) {
#ifdef SUPPORT_CFBB_UPG
            uapi_upg_request_upgrade(true);
#else
            cpu_utils_reset_chip_with_log((cores_t)APPS, REBOOT_CAUSE_APPLICATION_SYSRESETREQ);
#endif
        }
#endif
        if (count > (DFU_TIMEOUT_MS / DFU_DELAY_MS)) {
            osal_printk("dfu overtime\r\n");
            usb_dfu_download_fail();
        }
        osal_msleep(DFU_DELAY_MS);
        count++;
    }
}

void usb_dfu_download_callback(const uint8_t *buf, uint32_t len)
{
    osal_printk("dfu start addr:%x, cur addr:%x, size:%x\r\n", g_dfu_start_addr, g_dfu_addr, g_dfu_size);
    static bool is_pack_header = true;
    if (buf == NULL || len == 0) {
        return;
    }

    if (is_pack_header) {
        is_pack_header = false;
#ifdef SUPPORT_CFBB_UPG
        dfu_packge_header_t *header = (dfu_packge_header_t *)buf;
        upg_prepare_info_t info = {0};
        info.package_len = header->file_len;
        uapi_upg_prepare(&info);
#endif
        return;
    }
    if (g_dfu_addr + len > g_dfu_start_addr + g_dfu_size) {
        osal_printk("dfu overflow\r\n");
        usb_dfu_download_fail();
    }
    for (int i = 0; i < DFU_SERIAL_LEN; i++) {
        osal_printk("%x ", buf[i]);
    }
    osal_printk("\r\n");
    int ret = osal_irq_lock();
#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_EMBED_FLASH
    uapi_eflash_erase((uint32_t)g_dfu_addr, len);
    uapi_eflash_write((uint32_t)g_dfu_addr, (uint32_t *)(uintptr_t)buf, len);
#endif
#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_SFC
    uapi_sfc_reg_erase((uint32_t)g_dfu_addr, len);
    uapi_sfc_reg_write((uint32_t)g_dfu_addr, (uint8_t *)buf, len);
#endif
#ifdef CONFIG_SAMPLE_USB_MOUSE_DFU_USE_EXTERN_FLASH
    uapi_flash_block_erase(0, g_dfu_addr, len, true);
    uapi_flash_write_data(0, g_dfu_addr, buf, len);
#endif
    osal_irq_restore(ret);
    g_dfu_write_len += len;
#if !defined(CONFIG_SAMPLE_USB_MOUSE_DFU_USE_EXTERN_FLASH)
    if (memcmp(buf, (uint8_t *)(g_dfu_addr + DFU_DOWNLOAD_FLASH_START), len) != 0) {
        osal_printk("flash err\r\n");
        usb_dfu_download_fail();
    }
#endif
    uapi_watchdog_kick();
    g_dfu_addr += len;
}
