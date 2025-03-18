/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: BLE Mouse BAS and DIS Service Server SAMPLE. \n
 * Author: @CompanyNameMagicTag \n
 * History: \n
 * 2024-04-20, Create file. \n
 */
#include <stdbool.h>

#include "osal_task.h"

#include "sfc_flash.h"
#include "usb_init_app.h"
#include "mouse_mode_manage.h"
#include "app_public.h"

#define CUSTUMER_PAGE_REPORT_ID 0x8
#define CUSTOM_RW_PAGE_REPORT_ID 0x9
#define RECV_LENGTH 32
#define USB_RECV_FAIL_DELAY 50

m_public_var_t m_PublicVar;
device_flag device_flag_t;

// 上电初始化处理
#if (APP_FLASH_DEBUG)
void app_public_init(void)
{
    app_flash_poweron_get_flash();  // 上电后读取flash数据
}
#endif

// 将设备进行复位处理
void app_public_reset_device(void)
{
    m_PublicVar.switch_task = SWITCH_TASK_RESET_DEIVCE;
    osal_printk("app_public_reset_device.\r\n");
}

int app_public_con_type_entry_pcba(void)
{
    if (m_PublicVar.RxPCPaket.buffer8[1] == CONN_TYPE_ENTRY_PCBA) {
        // 0x86 进入PCBA测试模式
        if (m_PublicVar.RxPCPaket.buffer8[2] == 0x86) {  // 2 index
            osal_printk("enrer mouse mode PCBA test success\r\n");
            m_PublicVar.Mode = MOUSE_MODE_PCBA;
            m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
            m_PublicVar.AckPCPaket.PaketLen = 1;
            m_PublicVar.AckPCPaket.ReadyFlag = 1;
        } else {
            osal_printk("enrer mouse mode PCBA test fail\r\n");
            m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_FAIL;
            m_PublicVar.AckPCPaket.PaketLen = 1;
            m_PublicVar.AckPCPaket.ReadyFlag = 1;
        }
        return 1;
    }
    return 0;
}

void app_public_pc_cmd_add_cpi(void)
{
    if (get_app_mouse_handle().dpi_value < 6) {  // 6: 5是dpi的最高档位，所以小于6
        m_PublicVar.dpi_level = get_app_mouse_handle().dpi_value;
        device_flag_t.WriteFlashModeFlag |= DEV_INFO_UPDATA;
        device_flag_t.event_flag[2].led_event_flag = DPI_CHANGE;        // 2: led flag
        device_flag_t.event_flag[2].led_event_effect = QUICK_LED_FLAG;  // 2: led flag

        m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
        m_PublicVar.AckPCPaket.PaketLen = 1;
        m_PublicVar.AckPCPaket.ReadyFlag = 1;
        osal_printk("SET CPI ADD>>>>>>>>>>>>>>[1]dpi_set:%d [2]flash_dpi_set:%d \r\n",
            get_app_mouse_handle().dpi_value,
            m_PublicVar.dpi_level);
    }
}

void app_public_pc_cmd_sub_cpi(void)
{
    if (get_app_mouse_handle().dpi_value > 0) {
        m_PublicVar.dpi_level = get_app_mouse_handle().dpi_value;
        device_flag_t.WriteFlashModeFlag |= DEV_INFO_UPDATA;
        device_flag_t.event_flag[2].led_event_flag = DPI_CHANGE;        // 2: led flag
        device_flag_t.event_flag[2].led_event_effect = QUICK_LED_FLAG;  // 2: led flag

        m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
        m_PublicVar.AckPCPaket.PaketLen = 1;
        m_PublicVar.AckPCPaket.ReadyFlag = 1;
        osal_printk("SET CPI SUB>>>>>>>>>>>>>>[1]dpi_set:%d [2]flash_dpi_set:%d \r\n",
            get_app_mouse_handle().dpi_value,
            m_PublicVar.dpi_level);
    }
}

void app_public_pc_cmd_init_flash_def_par(void)
{
    // 3 4 index
    if ((m_PublicVar.RxPCPaket.buffer8[3] == 0x52) && (m_PublicVar.RxPCPaket.buffer8[4] == 0x3d)) {
        m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
        m_PublicVar.AckPCPaket.PaketLen = 1;
        m_PublicVar.AckPCPaket.ReadyFlag = 1;
    } else {
        m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_FAIL;
        m_PublicVar.AckPCPaket.PaketLen = 1;
        m_PublicVar.AckPCPaket.ReadyFlag = 1;
    }
}

void app_public_pc_cmd_set_mode(void)
{
    m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
    m_PublicVar.AckPCPaket.PaketLen = 1;
    m_PublicVar.AckPCPaket.ReadyFlag = 1;
    osal_printk("enrer mouse CMD set sensor mode \r\n");
}

void app_public_pc_cmd_get_fw_ver(void)
{
    osal_printk("enter get fw ver\r\n");
    m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
    m_PublicVar.AckPCPaket.PCData[1] = MOUSE_FW_REVISION & 0x000000ff;
    // 8: 用于计算版本号  2：index
    m_PublicVar.AckPCPaket.PCData[2] = (MOUSE_FW_REVISION >> 8) & 0x000000ff;
    // 16: 用于计算版本号 3：index
    m_PublicVar.AckPCPaket.PCData[3] = (MOUSE_FW_REVISION >> 16) & 0x000000ff;
    // 24: 用于计算版本号 4：index
    m_PublicVar.AckPCPaket.PCData[4] = (MOUSE_FW_REVISION >> 24) & 0x000000ff;
    m_PublicVar.AckPCPaket.PaketLen = 5;  // 5: 包长度
    m_PublicVar.AckPCPaket.ReadyFlag = 1;
}

void app_public_pc_cmd_fw_update(void)
{
    // 执行动作前如果还有未更新的数据先更新到flash中
    if (m_PublicVar.WriteFlashModeFlag) {
        m_PublicVar.DelayUpdataTimer = 0;
        m_PublicVar.IdleWriteFlashCounter = 0;
    }

    m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
    m_PublicVar.AckPCPaket.PaketLen = 1;
    m_PublicVar.AckPCPaket.ReadyFlag = 1;
    osal_printk("enter cmd fw updata\r\n");
}

void app_public_pc_cmd_get_cpi(void)
{
    m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
    m_PublicVar.AckPCPaket.PCData[1] = (m_PublicVar.dpi_level & 0x00ff);
    m_PublicVar.AckPCPaket.PaketLen = 3;   // 3 PaketLen
    m_PublicVar.AckPCPaket.ReadyFlag = 1;  // 5 PaketLen
    osal_printk("read current CPI\r\n");
}

void app_cmd_pc_cmd_read_bat_vol(void)
{
    m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
    if (!m_PublicVar.PowerOnCnt) {                // 有效
        m_PublicVar.AckPCPaket.PCData[1] = 0x01;  // 电量有效标志
    } else {                                      // 无效
        m_PublicVar.AckPCPaket.PCData[1] = 0x00;  // 电量有效标志
    }
    m_PublicVar.AckPCPaket.PCData[2] = m_PublicVar.BatPercent;  // 2 index
    m_PublicVar.AckPCPaket.PaketLen = 3;                        // 3 PaketLen
    m_PublicVar.AckPCPaket.ReadyFlag = 1;
    osal_printk("read current bat_vol\r\n");
}

void app_public_pc_cmd_set_power_mode(void)
{
    m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
    m_PublicVar.AckPCPaket.PaketLen = 1;
    m_PublicVar.AckPCPaket.ReadyFlag = 1;
    osal_printk("enter set power mode\r\n");
}

void app_public_pc_cmd_update_keyxymove(void)
{
    m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
    m_PublicVar.AckPCPaket.PaketLen = 1;
    m_PublicVar.AckPCPaket.ReadyFlag = 1;
}

void app_public_pc_cmd_write_sensor_mode(void)
{
#if (APP_COMMAND_DEBUG)
    device_flag_t.WriteFlashModeFlag |= DEV_INFO_UPDATA;
    app_flash_updata_flash_buffer();
#endif
    m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
    m_PublicVar.AckPCPaket.PaketLen = 1;
    m_PublicVar.AckPCPaket.ReadyFlag = 1;
}

void app_public_pc_cmd_read_sensor_mode(void)
{
    m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
    m_PublicVar.AckPCPaket.PaketLen = 2;  // 2: 当前有有效的配置包未处理
    m_PublicVar.AckPCPaket.ReadyFlag = 1;
}

void app_public_pc_cmd_get_mode(void)
{
    osal_printk("enrer mouse CMD get mode \r\n");
    m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
    m_PublicVar.AckPCPaket.PaketLen = 2;  // 2: 包的长度
    m_PublicVar.AckPCPaket.ReadyFlag = 1;
}

void app_public_pc_cmd_reset_device(void)
{
    app_public_reset_device();
    m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_SUCCESS;
    m_PublicVar.AckPCPaket.PaketLen = 1;
    m_PublicVar.AckPCPaket.ReadyFlag = 1;
}

void app_public_default(void)
{
    m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_FAIL;
    m_PublicVar.AckPCPaket.PaketLen = 1;
    m_PublicVar.AckPCPaket.ReadyFlag = 1;
}

void app_public_app_command_debug(void)
{
    if (m_PublicVar.RxPCPaket.buffer8[2] == PC_CMD_READ_SENSOR_MODE) {  // 2: index
        app_public_pc_cmd_read_sensor_mode();
        return;
    }
    if (m_PublicVar.RxPCPaket.buffer8[2] == PC_CMD_WRITE_SENSOR_MODE) { // 2: index
        app_public_pc_cmd_write_sensor_mode();
        return;
    }
    if (m_PublicVar.RxPCPaket.buffer8[2] == PC_CMD_ADD_CPI) {   // 2: index
        app_public_pc_cmd_add_cpi();
        return;
    }
    if (m_PublicVar.RxPCPaket.buffer8[2] == PC_CMD_SUB_CPI) {   // 2: index
        app_public_pc_cmd_sub_cpi();
        return;
    }
}

void app_public_pc_command_pro_switch_part1(void)
{
    // 进入指定鼠标工作模式
    if (m_PublicVar.RxPCPaket.buffer8[2] == PC_CMD_SET_MODE) {  // 2: index
        app_public_pc_cmd_set_mode();
    }
    // 获取鼠标当前工作模式
    if (m_PublicVar.RxPCPaket.buffer8[2] == PC_CMD_GET_MODE) { // 2: index
        app_public_pc_cmd_get_mode();
        return;
    }
    // 获取 firmware 版本号及型号
    if (m_PublicVar.RxPCPaket.buffer8[2] == PC_CMD_GET_FW_VER) {   // 2: index
        app_public_pc_cmd_get_fw_ver();
        return;
    }
    // 读eeprom
    if (m_PublicVar.RxPCPaket.buffer8[2] == PC_CMD_READ_EEPROM) {   // 2: index
        osal_printk("enter cmd read eeprom\r\n");
        return;
    }
}

void app_public_pc_command_pro_switch(void)
{
    switch (m_PublicVar.RxPCPaket.buffer8[2]) {  // 2: index
#if (APP_COMMAND_DEBUG)
        case PC_CMD_READ_SENSOR_MODE:
        case PC_CMD_WRITE_SENSOR_MODE:
        case PC_CMD_ADD_CPI:
        case PC_CMD_SUB_CPI:
            app_public_app_command_debug();
            break;
#endif
        
        case PC_CMD_SET_MODE:
        case PC_CMD_GET_MODE:
        case PC_CMD_GET_FW_VER:
        case PC_CMD_READ_EEPROM:
            app_public_pc_command_pro_switch_part1();
            break;
        // 写eeprom，宏数据及显示数据
        case PC_CMD_WRITER_EEPROM:
            osal_printk("enter cmd write eeprom\r\n");
            break;
        // 驱动加载指定的配置数据到指定的模式的flash中的保存配置的地址中
        case PC_CMD_RELOAD_CONFIG:
            osal_printk("enter cmd reload config\r\n");
            break;
        // 复位设备
        case PC_CMD_RESET_DEVICE:
            app_public_pc_cmd_reset_device();
            break;
        // 进入固件更新
        case PC_CMD_FW_UPDATE:
            app_public_pc_cmd_fw_update();
            break;
        // 读取当前的CPI
        case PC_CMD_GET_CPI:
            app_public_pc_cmd_get_cpi();
            break;
        // 读取当前电量
        case PC_CMD_READ_BAT_VOL:
            app_cmd_pc_cmd_read_bat_vol();
            break;
        // 设置电源模式
        case PC_CMD_SET_POWER_MODE:
            app_public_pc_cmd_set_power_mode();
            break;
        // 更新OLED显示屏上按键统计及移动距离
        case PC_CMD_UPDATE_KEYXYMOVE:
            app_public_pc_cmd_update_keyxymove();
            break;
        // 初始化为flash默认参数
        case PC_CMD_INIT_FLASH_DEF_PAR:
            app_public_pc_cmd_init_flash_def_par();
            break;
        default:
            app_public_default();
            break;
    }
}

void app_public_pc_command_pro(void)
{
    for (;;) {
        int32_t ret = fhid_recv_data(get_hid_mouse_index(), (char *)m_PublicVar.RxPCPaket.buffer8, RECV_LENGTH);
        if (ret <= 0) {
            osal_msleep(USB_RECV_FAIL_DELAY);
            continue;
        }
        if ((ret == RECV_LENGTH) && (m_PublicVar.RxPCPaket.buffer8[0] == 0xBA)) {
            (void)memset_s(m_PublicVar.AckPCPaket.PCData,
                sizeof(m_PublicVar.AckPCPaket.PCData),
                0,
                sizeof(m_PublicVar.AckPCPaket.PCData));  // ack 数据缓存包清0
            for (uint8_t i = 0; i < 8; i++) {            // 前8个字节
                osal_printk("ret = %d ,buffer8[%d] = %x ", ret, i, m_PublicVar.RxPCPaket.buffer8[i]);
            }
            osal_printk("\r\n");
        }
        int pcba_ret = app_public_con_type_entry_pcba();
        if (pcba_ret == 1) {
            return;
        }
#if PCBA_FUC_ENABLE
        if (m_PublicVar.RxPCPaket.buffer8[1] == COMMAND_PCBA) {  // PCBA命令
            if (m_PublicVar.Mode == MOUSE_MODE_PCBA) {
                PCBA_Test_usb_command_pro();
            } else {
                m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_FAIL;
                m_PublicVar.AckPCPaket.PaketLen = 1;
                m_PublicVar.AckPCPaket.ReadyFlag = 1;
            }
            return;
        }
#else
        m_PublicVar.AckPCPaket.PCData[0] = ACK_PC_FAIL;
        m_PublicVar.AckPCPaket.PaketLen = 1;
        m_PublicVar.AckPCPaket.ReadyFlag = 1;
#endif
        app_public_pc_command_pro_switch();
    }
}
