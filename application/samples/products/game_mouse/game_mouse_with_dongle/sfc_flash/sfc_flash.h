/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Mouse wheel  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#ifndef SFC_FLASH_H
#define SFC_FLASH_H

#include <stdint.h>
#include "common_def.h"
#include "sfc.h"
#include "securec.h"
#include "osal_addr.h"
#include "bts_le_gap.h"
#include "sle_common.h"
#include "app_public.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define SAVE_SUCCESS 1
#define SAVE_FAILED 0

// 写操作的最小单位为页
#define APP_FLASH_PAGE_SIZE 256  // 4096
// 扇区大小
#define APP_FLASH_SECTOR_SIZE 4096

// 一条宏占用一个flash PAGE
#define FLASH_MACRO_PAGE_SIZE FLASH_SECTOR_SIZE

// 显示图片占用一个flash page
#define FLASH_PIC_PAGE_SIZE FLASH_SECTOR_SIZE

// =============================flash空间分配定义============================================
// Macro
#define FLASH_START_ADDR_MACRO 0x87000
#define FLASH_END_ADDR_MACRO 0x95FFF
// MAC ADDR数据区u32
#define FLASH_START_ADDR_MAC_ADDR 0x96000
#define FLASH_END_ADDR_MAC_ADDR 0x96FFF
// SLE 数据区
#define FLASH_START_ADDR_SLE 0x97000
#define FLASH_END_ADDR_SLE 0x97FFF
// BLE 数据区
#define FLASH_START_ADDR_BLE 0x98000
#define FLASH_END_ADDR_BLE 0x98FFF
// 设备数据模式相关及FW Info
#define FLASH_START_ADDR_FW 0x99000
#define FLASH_END_ADDR_FW 0x9CFFF

typedef struct flash_mast_data {
    uint8_t ble_status_flag;
    uint8_t slave_mac_addr[4];
    bd_addr_t peer_addr;
} flash_mast_data;

typedef struct flash_ble_data {
    uint8_t mast_id;
    flash_mast_data flash_mast_d[3];
} flash_ble_data;

typedef struct flash_sle_data {
    uint8_t sle_pair_flag;
    sle_addr_t sle_peer_addr;
} flash_sle_data;

typedef struct {
    uint8_t Sensor_type;  // 传感器类型
    uint8_t LastLiftCutoff;
    uint8_t LastWorkMode;
    int16_t sensor_x;
    int16_t sensor_y;
#if SENSOR_SYNC_ENABLE
    int16_t accum_x;
    int16_t accum_y;
    int16_t scaled_1ms_delta_x;
    int16_t scaled_1ms_delta_y;
    uint8_t flag_1msReport;
    uint16_t count_1ms_valuse;
    uint32_t LastRTCCounter;
#endif
} mouse_sensor_lb_t;

#define BOOT_FLASH_SAVE_EN 0
extern flash_ble_data flash_ble_info;
extern flash_sle_data flash_sle_info;

uint8_t save_data_to_flash(uint32_t flash_offset, uint8_t *buff, uint32_t len);
void app_flash_updata_flash_buffer(void);
void save_ble_pair_info(void);
void save_sle_pair_info(void);
errcode_t sfc_flash_write(const uint32_t flash_offset, uint32_t size, const uint8_t *ram_data);

// 上电后读取flash中的数据
#if APP_FLASH_DEBUG
uint8_t app_flash_read_workmide_info(void);
uint8_t app_flash_poweron_get_flash(void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif