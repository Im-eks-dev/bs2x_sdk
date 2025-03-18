/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved.
 * Description: BT PUBLIC API module.
 */
#include "osal_debug.h"
#include "debug_print.h"
#include "soc_osal.h"
#include "common_def.h"
#include "gpio.h"
#include "pinctrl.h"
#include "platform_core.h"
#include "pinctrl_porting.h"
#include "sfc.h"

#include "nv_key.h"
#include "string.h"
#include "nv_porting.h"
#include "uapi_crc.h"
#include "nv_nvregion.h"
#include "flash.h"

FLASH_DEVICE_ST g_flash_device;
FLASH_OTHER_ST g_flash_other;

void set_mouse_cpi_value(uint8_t cpi)
{
    g_flash_device.mouse_cpi = cpi;
    save_all_flash_info();
}

uint8_t get_mouse_cpi_value(void)
{
    return g_flash_device.mouse_cpi;
}

void set_mouse_current_channel_number(uint8_t number)
{
    g_flash_device.mouse_channel = number;
    save_all_flash_info();
}

uint8_t get_mouse_current_channel_number(void)
{
    return g_flash_device.mouse_channel;
}

void set_channel_mode_status(uint8_t number, uint8_t status)
{
    g_flash_device.channel_info[number].mode = status;
    save_all_flash_info();
    }

uint8_t get_channel_mode_status(uint8_t number, uint8_t status)
{
    return g_flash_device.channel_info[number].mode = status;
}

void set_channel_pair_status(uint8_t number, uint8_t status)
{
    g_flash_device.channel_info[number].pair = status;
    save_all_flash_info();
}

uint8_t get_channel_pair_status(uint8_t number, uint8_t status)
{
    return g_flash_device.channel_info[number].pair = status;
}

void clear_all_channel_data_flash(void)
{
    uint8_t i = 0;
    for (i = 0; i < CHANNEL_MAX_NUM; i++) {
        memset_s(g_flash_device.channel_info[i].ble_mac, BD_ADDR_LEN, 0, BD_ADDR_LEN);
        memset_s(g_flash_device.channel_info[i].sle_mac, BD_ADDR_LEN, 0, BD_ADDR_LEN);
        memset_s(g_flash_device.channel_info[i].central_mac, BD_ADDR_LEN, 0, BD_ADDR_LEN);
        g_flash_device.channel_info[i].central_type = 0;
        g_flash_device.channel_info[i].connect_flag = 0;
        g_flash_device.channel_info[i].direct_adv_enable = 0;
        g_flash_device.channel_info[i].pair = NONE_PAIR;
        g_flash_device.channel_info[i].mode = NONE_MODE;
    }
    save_all_flash_info();
}


void save_all_flash_info(void)
{
    uint32_t sector_user_flag;
    uint32_t user_data_read_adr;
    uint32_t user_data_write_adr;
    uint32_t user_data_write_adr_back;

    uapi_sfc_reg_read(DEV_NV_SAVE_AREA_1, (uint8_t *)&sector_user_flag, USER_REG_READ_LEN);
    if (sector_user_flag == SECTOR_USER_FLAG) {
        user_data_read_adr = DEV_NV_SAVE_AREA_1;
        user_data_write_adr = DEV_NV_SAVE_AREA_2;
        user_data_write_adr_back = DEV_NV_SAVE_AREA_2;
    } else {
        user_data_read_adr = DEV_NV_SAVE_AREA_2;
        user_data_write_adr = DEV_NV_SAVE_AREA_1;
        user_data_write_adr_back = DEV_NV_SAVE_AREA_1;
    }

    uapi_sfc_reg_erase(user_data_write_adr, PAGE_SIZE);

    user_data_write_adr += USER_REG_READ_LEN;
    uapi_sfc_reg_write((const uint32_t)user_data_write_adr, (uint8_t *)(&g_flash_device.idx), sizeof(FLASH_DEVICE_ST));

    user_data_write_adr += sizeof(FLASH_DEVICE_ST);
    uapi_sfc_reg_write((const uint32_t)user_data_write_adr, (uint8_t *)(&g_flash_other.idx), sizeof(FLASH_OTHER_ST));

    sector_user_flag = 0;
    uapi_sfc_reg_write(user_data_read_adr, (uint8_t *)&sector_user_flag, USER_REG_READ_LEN);
    sector_user_flag = SECTOR_USER_FLAG;
    uapi_sfc_reg_write((uint32_t)user_data_write_adr_back, (uint8_t *)&sector_user_flag, USER_REG_READ_LEN);
}


void get_all_flash_info(void)
{
    uint32_t sector_user_flag;
    uint32_t user_data_read_adr;

    uapi_sfc_reg_read(DEV_NV_SAVE_AREA_1, (uint8_t *)&sector_user_flag, USER_REG_READ_LEN);
    if (sector_user_flag == SECTOR_USER_FLAG) {
        user_data_read_adr = DEV_NV_SAVE_AREA_1;
    } else {
        user_data_read_adr = DEV_NV_SAVE_AREA_2;
    }

    user_data_read_adr += USER_REG_READ_LEN;
    uapi_sfc_reg_read((const uint32_t)user_data_read_adr, (uint8_t *)(&g_flash_device.idx), sizeof(FLASH_DEVICE_ST));

    user_data_read_adr += sizeof(FLASH_DEVICE_ST);
    uapi_sfc_reg_read((const uint32_t)user_data_read_adr, (uint8_t *)(&g_flash_other.idx), sizeof(FLASH_OTHER_ST));
}

void clear_pair_info(void)
{
    g_flash_device.sle_pair_mac = g_flash_device.sle_mac[SLE_PAIR_MAC_IDX];
    g_flash_device.mouse_channel = CHANNEL_MAX_NUM - 1;
    g_flash_device.last_mouse_channel = g_flash_device.mouse_channel;
    for (uint8_t i = 0; i < CHANNEL_MAX_NUM; i++) {
        memset_s(g_flash_device.channel_info[i].ble_mac, BD_ADDR_LEN, 0, BD_ADDR_LEN);
        memset_s(g_flash_device.channel_info[i].sle_mac, BD_ADDR_LEN, 0, BD_ADDR_LEN);
        memset_s(g_flash_device.channel_info[i].central_mac, BD_ADDR_LEN, 0, BD_ADDR_LEN);
        g_flash_device.channel_info[i].central_type = 0;
        g_flash_device.channel_info[i].connect_flag = 0;
        g_flash_device.channel_info[i].direct_adv_enable = 0;
        g_flash_device.channel_info[i].pair = NONE_PAIR;
        g_flash_device.channel_info[i].mode = NONE_MODE;
        g_flash_device.linkKey[i][LTK_KEY_INFO_LEN - 1] = 0;    //  ltk 去掉
    }
}

void power_on_get_flash_data(uint8_t app_flag)
{
    osal_printk("power_on_get_flash_data enter, app_flag:%d.\n", app_flag);
    uint32_t nv_user_flag_1, nv_user_flag_2;
    uapi_sfc_reg_read(DEV_NV_SAVE_AREA_1, (uint8_t *)&nv_user_flag_1, USER_REG_READ_LEN);
    uapi_sfc_reg_read(DEV_NV_SAVE_AREA_2, (uint8_t *)&nv_user_flag_2, USER_REG_READ_LEN);

    if ((nv_user_flag_1 != SECTOR_USER_FLAG) && (nv_user_flag_2 != SECTOR_USER_FLAG)) {
        osal_printk("first power on ! init data ...\n");
        uint32_t sector_user_flag;
        uint32_t user_data_read_adr;
        uint32_t user_data_write_adr;
        uint32_t user_data_write_adr_back;

        uint8_t temp_data[16] = {0};
        uapi_sfc_reg_read(DEV_NV_SAVE_AREA_1, temp_data, sizeof(temp_data));

        g_flash_device.idx = SECTOR_USER_FLAG;
        g_flash_device.mouse_cpi = 1;
        g_flash_device.mouse_channel = 0;
        g_flash_device.last_mouse_channel = 0;
        g_flash_device.channel_info[CHANNEL_NUM_IDX0].mode = NONE_MODE;
        g_flash_device.channel_info[CHANNEL_NUM_IDX1].mode = NONE_MODE;
        g_flash_device.channel_info[CHANNEL_NUM_IDX2].mode = NONE_MODE;
        g_flash_device.channel_info[CHANNEL_NUM_IDX0].pair = NONE_PAIR;
        g_flash_device.channel_info[CHANNEL_NUM_IDX1].pair = NONE_PAIR;
        g_flash_device.channel_info[CHANNEL_NUM_IDX2].pair = NONE_PAIR;

        g_flash_device.mouse_vid[0] = 0x12;
        g_flash_device.mouse_vid[1] = 0xD1;
        g_flash_device.mouse_pid[0] = 0x10;
        g_flash_device.mouse_pid[1] = 0xD4;
        g_flash_device.mouse_sn_len = 0x00;
        g_flash_device.ble_pair_mac = 0;
        g_flash_device.sle_pair_mac = 0;
        memcpy_s(g_flash_device.ble_mac, BD_ADDR_LEN, &temp_data[1], BD_ADDR_LEN);
        memcpy_s(g_flash_device.sle_mac, BD_ADDR_LEN, &temp_data[BD_ADDR_LEN + 1], BD_ADDR_LEN);

        memset_s(g_flash_device.name, sizeof(g_flash_device.name), '\0', sizeof(g_flash_device.name));
        memset_s(g_flash_device.mouse_sn, sizeof(g_flash_device.mouse_sn), '\0', sizeof(g_flash_device.mouse_sn));
        memset_s(g_flash_device.mode_id, sizeof(g_flash_device.mode_id), '\0', sizeof(g_flash_device.mode_id));
        memset_s(g_flash_device.hw_version, sizeof(g_flash_device.hw_version), '\0', sizeof(g_flash_device.hw_version));
        memset_s(g_flash_device.fw_version, sizeof(g_flash_device.fw_version), '\0', sizeof(g_flash_device.fw_version));

        user_data_read_adr = DEV_NV_SAVE_AREA_1;
        user_data_write_adr = DEV_NV_SAVE_AREA_2;
        user_data_write_adr_back = DEV_NV_SAVE_AREA_2;

        uapi_sfc_reg_erase(user_data_write_adr, PAGE_SIZE);

        user_data_write_adr += USER_REG_READ_LEN;
        uapi_sfc_reg_write((const uint32_t)user_data_write_adr,
            (uint8_t *)(&g_flash_device.idx), sizeof(FLASH_DEVICE_ST));

        user_data_write_adr += sizeof(FLASH_DEVICE_ST);
        uapi_sfc_reg_write((const uint32_t)user_data_write_adr,
            (uint8_t *)(&g_flash_other.idx), sizeof(FLASH_OTHER_ST));
        
        sector_user_flag = SECTOR_NONE_USER;
        uapi_sfc_reg_write(user_data_read_adr, (uint8_t *)&sector_user_flag, USER_REG_READ_LEN);
        sector_user_flag = SECTOR_USER_FLAG;
        uapi_sfc_reg_write((uint32_t)user_data_write_adr_back, (uint8_t *)&sector_user_flag, USER_REG_READ_LEN);
    } else {
        osal_printk("get_all_flash_info ...\n");
        get_all_flash_info();
        if (strncmp((char *)g_flash_device.name, "HuaWei", strlen("HuaWei")) != 0) {  // 如果不等于华为字符串，则进入
            if ((g_flash_device.sub_model_id == 0xff) || (g_flash_device.sub_model_id == 0x00)) {  // 等于0xff 则表示没校正过频偏
                osal_printk("===none freq_offset %02x ===\n", g_flash_device.freq_offset);
            } else {
                g_flash_device.freq_offset = g_flash_device.sub_model_id; // 产测写入频偏的位置
                g_flash_device.freq_offset_write_flag = 0xA5; // 写入标志
                osal_printk("===freq_offset %02x ===\n", g_flash_device.freq_offset);
            }

            g_flash_device.mouse_cpi = 1;
            g_flash_device.mouse_channel = 0;
            g_flash_device.last_mouse_channel = 0;
            g_flash_device.channel_info[CHANNEL_NUM_IDX0].mode = NONE_MODE;
            g_flash_device.channel_info[CHANNEL_NUM_IDX1].mode = NONE_MODE;
            g_flash_device.channel_info[CHANNEL_NUM_IDX2].mode = NONE_MODE;
            g_flash_device.channel_info[CHANNEL_NUM_IDX0].pair = NONE_PAIR;
            g_flash_device.channel_info[CHANNEL_NUM_IDX1].pair = NONE_PAIR;
            g_flash_device.channel_info[CHANNEL_NUM_IDX2].pair = NONE_PAIR;

            g_flash_device.mouse_vid[0] =  0x12;
            g_flash_device.mouse_vid[1] =  0xD1;
            g_flash_device.mouse_pid[0] =  0x10;

            memcpy_s(g_flash_device.channel_info[CHANNEL_NUM_IDX3].ble_mac, BD_ADDR_LEN,
                g_flash_device.ble_mac, BD_ADDR_LEN);
            memcpy_s(g_flash_device.channel_info[CHANNEL_NUM_IDX3].sle_mac, BD_ADDR_LEN,
                g_flash_device.sle_mac, BD_ADDR_LEN);
            g_flash_device.channel_info[CHANNEL_NUM_IDX3].sle_mac[SLE_PAIR_MAC_IDX] &= 0x3f;

            g_flash_device.mouse_sn_len =  0x00;
            g_flash_device.ble_pair_mac = 0;
            g_flash_device.sle_pair_mac = 0;
            save_all_flash_info();
        }
        osal_printk("sub_model_id 0x%x \r\n", g_flash_device.sub_model_id);
        g_flash_device.mouse_vid[0] = 0x12;
        g_flash_device.mouse_vid[1] = 0xD1;
        g_flash_device.mouse_pid[0] = 0x10;
        g_flash_device.mouse_pid[1] = 0xD4;

        memset_s(g_flash_device.name, sizeof(g_flash_device.name), '\0', sizeof(g_flash_device.name));
        memset_s(g_flash_device.mouse_sn, sizeof(g_flash_device.mouse_sn), '\0', sizeof(g_flash_device.mouse_sn));
        memset_s(g_flash_device.mode_id, sizeof(g_flash_device.mode_id), '\0', sizeof(g_flash_device.mode_id));
        memset_s(g_flash_device.hw_version, sizeof(g_flash_device.hw_version), '\0', sizeof(g_flash_device.hw_version));
        memset_s(g_flash_device.fw_version, sizeof(g_flash_device.fw_version), '\0', sizeof(g_flash_device.fw_version));
    }

    osal_printk("g_flash_device.mouse_channel 0x%x\n", g_flash_device.mouse_channel);
    osal_printk("g_flash_device.last_mouse_channel 0x%x\n", g_flash_device.last_mouse_channel);

    osal_printk("sub_model_id:[%d]\n", g_flash_device.sub_model_id);
    osal_printk("sn:[%s] name:[%s] hw:[%s] sw:[%s] %d %d mode_id[%s] \n", g_flash_device.mouse_sn, g_flash_device.name,
        g_flash_device.hw_version, g_flash_device.fw_version, g_flash_device.ble_pair_mac, g_flash_device.sle_pair_mac,
        g_flash_device.mode_id);
}

void flash_data_restore_factory(uint8_t reset)
{
    osal_printk("flash_data_restore_factory ...\n");

    get_all_flash_info();

    g_flash_device.mouse_cpi = 1;
    g_flash_device.mouse_channel = 0;
    g_flash_device.last_mouse_channel = g_flash_device.mouse_channel;
    if (reset) {
        g_flash_device.ble_pair_mac = 0;
        g_flash_device.sle_pair_mac = 0;
    } else {
        g_flash_device.ble_pair_mac = (g_flash_device.ble_pair_mac + 1) & 0x3f;
        g_flash_device.sle_pair_mac = (g_flash_device.sle_pair_mac + 1) & 0x3f;
    }

    for (int i = 0; i < CHANNEL_MAX_NUM - 1; i++) {   //  暂时不恢复dongle
        memset_s(&g_flash_device.channel_info[i], sizeof(CHANNEL_INFO_ST), 0, sizeof(CHANNEL_INFO_ST));
        g_flash_device.linkKey[i][LTK_KEY_INFO_LEN - 1] = 0;    //  ltk 去掉
    }

    save_all_flash_info();
}


void flash_test(void)
{
    printf("----------------------------start----------------------------\n");
    
    memset_s(&g_flash_device.idx, sizeof(FLASH_DEVICE_ST), 0, sizeof(FLASH_DEVICE_ST));
    printf("--clear RAM---%02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x-----------\n",\
        g_flash_device.idx,\
        g_flash_device.mouse_cpi,\
        g_flash_device.mouse_channel,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX0].mode,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX1].mode,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX2].mode,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX0].pair,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX1].pair,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX2].pair);
    
    g_flash_device.idx = SECTOR_USER_FLAG;
    g_flash_device.mouse_cpi = 1;
    g_flash_device.mouse_channel = 0;
    g_flash_device.channel_info[CHANNEL_NUM_IDX0].mode = 3;  // mode:3
    g_flash_device.channel_info[CHANNEL_NUM_IDX1].mode = 2;  // mode:2
    g_flash_device.channel_info[CHANNEL_NUM_IDX2].mode = 1;
    g_flash_device.channel_info[CHANNEL_NUM_IDX0].pair = 1;
    g_flash_device.channel_info[CHANNEL_NUM_IDX1].pair = 2;  // pair:2
    g_flash_device.channel_info[CHANNEL_NUM_IDX2].pair = 3;  // pair:3

    printf("--set   RAM---%02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x-----------\n",\
        g_flash_device.idx,\
        g_flash_device.mouse_cpi,\
        g_flash_device.mouse_channel,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX0].mode,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX1].mode,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX2].mode,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX0].pair,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX1].pair,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX2].pair);
    
    printf("--save  RAM--------------------------------------------------\n");
    save_all_flash_info();
    
    memset_s(&g_flash_device.idx, sizeof(FLASH_DEVICE_ST), 0, sizeof(FLASH_DEVICE_ST));
    printf("--clear RAM---%02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x-----------\n",\
        g_flash_device.idx,\
        g_flash_device.mouse_cpi,\
        g_flash_device.mouse_channel,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX0].mode,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX1].mode,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX2].mode,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX0].pair,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX1].pair,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX2].pair);
    
    get_all_flash_info();
    printf("--get   RAM---%02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x-----------\n",\
        g_flash_device.idx,\
        g_flash_device.mouse_cpi,\
        g_flash_device.mouse_channel,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX0].mode,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX1].mode,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX2].mode,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX0].pair,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX1].pair,\
        g_flash_device.channel_info[CHANNEL_NUM_IDX2].pair);
    printf("-----------------------------end-----------------------------\n");
}
