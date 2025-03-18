/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved.
 * Description: BT PUBLIC API module.
 */

#ifndef _FLASH_H_
#define _FLASH_H_
#include "common_def.h"
#include "platform_core.h"
#include "bts_def.h"
#include "memory_config_common.h"
#include "osal_debug.h"

#define DEV_NV_SAVE_ADDR_START   (0x7c000)
#define DEV_NV_SAVE_ADDR_END     (0x7e000)
#define PAGE_SIZE                 4096

#define DEV_NV_SAVE_AREA_1      (0x7c000)
#define DEV_NV_SAVE_AREA_2      (0x7d000)

#define SECTOR_USER_FLAG        0XA55A55AA
#define SECTOR_NONE_USER        0X00000000
#define VENDOR_LOG_SAVE_AREA_1          0XF2000
#define VENDOR_LOG_SAVE_AREA_2          0XF3000

#define CHANNEL_MAX_NUM     4
#define CHANNEL_NUM_IDX0    0
#define CHANNEL_NUM_IDX1    1
#define CHANNEL_NUM_IDX2    2
#define CHANNEL_NUM_IDX3    3
#define LTK_KEY_INFO_LEN    20
#define USER_OTHER_BYTE_SIZE  32
#define USER_REG_READ_LEN 4

#define SLE_PAIR_MAC_IDX 3

enum ble_pair {
    NONE_PAIR = 0,
    ENTRY_PAIR,
    PAIR_SUCCESS,
};

enum sle_mode {
    NONE_MODE = 0,
    MOUSE_BLE,
    MOUSE_SLE,
};

typedef struct {
    uint8_t ble_mac[BD_ADDR_LEN];
    uint8_t sle_mac[BD_ADDR_LEN];
    uint8_t central_mac[BD_ADDR_LEN];
    uint8_t central_type;
    uint8_t connect_flag : 1;
    uint8_t direct_adv_enable : 1;
    uint8_t pair : 2;
    uint8_t mode : 2;
} CHANNEL_INFO_ST;

typedef struct {
    uint32_t idx;
    uint8_t ble_mac[BD_ADDR_LEN];
    uint8_t sle_mac[BD_ADDR_LEN];

    uint8_t sub_model_id;
    uint8_t mouse_sn_len;
    uint8_t mouse_sn[33];
    uint8_t mouse_pid[2];
    uint8_t mouse_vid[2];
    uint8_t mode_id[5];
    uint8_t hw_version[10];
    uint8_t fw_version[10];
    uint8_t name[8];
    uint8_t magic[4];
    uint8_t mouse_cpi;
    uint8_t mouse_channel;
    uint8_t ble_pair_mac;
    uint8_t sle_pair_mac;
    CHANNEL_INFO_ST channel_info[CHANNEL_MAX_NUM];  // add an special chn con2dongle
    uint8_t last_mouse_channel;  // 记录3挡位模式用于3挡和连接dongle模式切换
    uint8_t authCode[16];
    uint8_t authCodeId[16];
    uint8_t linkKey[CHANNEL_MAX_NUM][LTK_KEY_INFO_LEN];        //  ltk buf only dongle use now
    uint8_t freq_offset_write_flag;
    uint8_t freq_offset;
    // 扩展字节 防止出现新旧程序flash空间对不齐的问题
    uint8_t none1;
    uint8_t none2;
    uint8_t none3;
    uint8_t none4;
    uint8_t none5;
    uint8_t none6;
    uint8_t none7;
    uint8_t none8;
} FLASH_DEVICE_ST;
extern FLASH_DEVICE_ST g_flash_device;

typedef struct {
    uint8_t idx;
    uint8_t hilink_authSetup[3];
} FLASH_OTHER_ST;
extern FLASH_OTHER_ST g_flash_other;

void set_mouse_cpi_value(uint8_t cpi);

uint8_t get_mouse_cpi_value(void);

void set_mouse_current_channel_number(uint8_t number);

uint8_t get_mouse_current_channel_number(void);

void set_channel_mode_status(uint8_t number, uint8_t status);

uint8_t get_channel_mode_status(uint8_t number, uint8_t status);

void set_channel_pair_status(uint8_t number, uint8_t status);

uint8_t get_channel_pair_status(uint8_t number, uint8_t status);

void clear_all_channel_data_flash(void);

void save_all_flash_info(void);

void get_all_flash_info(void);

void power_on_get_flash_data(uint8_t app_flag);

void flash_data_restore_factory(uint8_t reset);

void flash_test(void);
#endif
