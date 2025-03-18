/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Mouse wheel  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */
#include <string.h>
#include "osal_interrupt.h"
#include "osal_debug.h"
#include "app_public.h"
#include "mouse_sensor.h"
#include "sfc_flash.h"

#define APP_USBD_VID 0x24ae
#define APP_USBD_PID 0x4510

flash_ble_data flash_ble_info;
flash_sle_data flash_sle_info;
mouse_sensor_lb_t s_mouse_sensor;

// 逻辑地址到实际flash的开始地址的映射表
const uint32_t LogicalToFlashStartAddrMapTable[] = {
    FLASH_START_ADDR_MACRO + (0 * 0x1000),   // 0		FLASH_ADDR_INDEX_MACRO1
    FLASH_START_ADDR_MACRO + (1 * 0x1000),   // 1 	FLASH_ADDR_INDEX_MACRO2
    FLASH_START_ADDR_MACRO + (2 * 0x1000),   // 2		FLASH_ADDR_INDEX_MACRO3
    FLASH_START_ADDR_MACRO + (3 * 0x1000),   // 3		FLASH_ADDR_INDEX_MACRO4
    FLASH_START_ADDR_MACRO + (4 * 0x1000),   // 4		FLASH_ADDR_INDEX_MACRO5
    FLASH_START_ADDR_MACRO + (5 * 0x1000),   // 5		FLASH_ADDR_INDEX_MACRO6
    FLASH_START_ADDR_MACRO + (6 * 0x1000),   // 6		FLASH_ADDR_INDEX_MACRO7
    FLASH_START_ADDR_MACRO + (7 * 0x1000),   // 7		FLASH_ADDR_INDEX_MACRO8
    FLASH_START_ADDR_MACRO + (8 * 0x1000),   // 8		FLASH_ADDR_INDEX_MACRO9
    FLASH_START_ADDR_MACRO + (9 * 0x1000),   // 9		FLASH_ADDR_INDEX_MACRO10
    FLASH_START_ADDR_MACRO + (10 * 0x1000),  // 10	FLASH_ADDR_INDEX_MACRO11
    FLASH_START_ADDR_MACRO + (11 * 0x1000),  // 11	FLASH_ADDR_INDEX_MACRO12

    FLASH_START_ADDR_MACRO + (12 * 0x1000),  // 12	FLASH_ADDR_INDEX_GUN_MACRO1
    FLASH_START_ADDR_MACRO + (13 * 0x1000),  // 13	FLASH_ADDR_INDEX_GUN_MACRO2
    FLASH_START_ADDR_MACRO + (14 * 0x1000),  // 14	FLASH_ADDR_INDEX_GUN_MACRO3
    FLASH_START_ADDR_MACRO + (15 * 0x1000),  // 15	FLASH_ADDR_INDEX_GUN_PIC1

    FLASH_START_ADDR_MAC_ADDR,  // 16	FLASH_START_ADDR_MAC_ADDR

    FLASH_START_ADDR_SLE,  // 17	FLASH_START_ADDR_SLE

    FLASH_START_ADDR_BLE,  // 18	FLASH_START_ADDR_BLE

    FLASH_START_ADDR_FW,  // 19	FLASH_START_ADDR_FW
};
#define APP_FLASH_WORD_ALIGN 4
static inline void *sfc_malloc(uint32_t x)
{
    return osal_kmalloc(x, 0);
}

static inline void sfc_free(uint8_t *x)
{
    osal_kfree(x);
}
static uint32_t flash_read_data_align(const uint32_t *addr, uint32_t *length, uint8_t *first_cover_count,
                                      uint8_t *last_cover_count)
{
    uint32_t dest_addr;
    /* Check if the address is 4 bytes aligned. */
    if (((*addr) & 0x3) != 0) {
        /* The actual send address requires 4 byte alignment. */
        dest_addr = ((*addr) & 0xFFFFFFFC);

        /* Fill the first 4 bytes of data. */
        *first_cover_count = (*addr) & 0x3;
        (*length) += (*first_cover_count);
    } else {
        dest_addr = (*addr);
    }
    /* Check if the last 4 bytes are aligned. */
    if (((*length) & 0x3) != 0) {
        /* Fill the first 4 bytes of data. */
        *last_cover_count = APP_FLASH_WORD_ALIGN - (uint8_t)((*length) & 0x3);
        (*length) += (*last_cover_count);
    }
    return dest_addr;
}
errcode_t app_flash_updata(const uint32_t flash_offset, uint32_t size, const uint8_t *ram_data)
{
    errcode_t ret = ERRCODE_SUCC;

    uint32_t write_size = size;
    uint8_t first_cover_count = 0;
    uint8_t last_cover_count = 0;
    uint32_t dest_addr = flash_read_data_align(&flash_offset, &write_size, &first_cover_count, &last_cover_count);

    uint8_t *align_write_buff = (uint8_t *)sfc_malloc(write_size);
    if (align_write_buff == NULL) {
        return ERRCODE_MALLOC;
    }
    uint32_t mstatus = osal_irq_lock();
    if (uapi_sfc_reg_read(dest_addr, align_write_buff, write_size) != ERRCODE_SUCC) {
        ret = ERRCODE_FAIL;
        goto sfc_flash_err;
    }
    osal_irq_restore(mstatus);
    if (memcpy_s((uint8_t *)(align_write_buff + first_cover_count), write_size, (uint8_t *)ram_data, size) != EOK) {
        ret = ERRCODE_FAIL;
    }
    mstatus = osal_irq_lock();
    uapi_sfc_reg_erase(flash_offset, APP_FLASH_SECTOR_SIZE);
    if (uapi_sfc_reg_write(dest_addr, (uint8_t *)align_write_buff, write_size) != ERRCODE_SUCC) {
        ret = ERRCODE_FAIL;
    }
sfc_flash_err:
    osal_irq_restore(mstatus);
    sfc_free(align_write_buff);
    return ret;
}
uint8_t save_data_to_flash(uint32_t flash_offset, uint8_t *buff, uint32_t len)
{
    uint8_t tmp_buf[APP_FLASH_SECTOR_SIZE];
    if (len > APP_FLASH_SECTOR_SIZE) {
        return SAVE_FAILED;
    }
    uint32_t irq_sts = osal_irq_lock();
    for (uint8_t i = 0; i < 2; i++) {   // 2 loop
        uapi_sfc_reg_write(flash_offset, buff, len);
        uapi_sfc_reg_read(flash_offset, tmp_buf, len);

        if (memcmp(tmp_buf, buff, len) == 0) {
            osal_irq_restore(irq_sts);
            osal_printk("save_data_to_flash_succ\r\n");
            return SAVE_SUCCESS;
        } else {
            uapi_sfc_reg_erase(flash_offset, APP_FLASH_SECTOR_SIZE);
        }
    }
    osal_irq_restore(irq_sts);
    return SAVE_FAILED;
}

void app_flash_updata_flash_buffer(void)
{
    if (app_flash_updata(FLASH_START_ADDR_FW, sizeof(m_PublicVar), (uint8_t *)&m_PublicVar) == ERRCODE_SUCC) {
        return;
    } else {
        osal_printk("save_dev_info_ERR\r\n");
    }
}

void save_sle_pair_info(void)
{
    if (app_flash_updata(FLASH_START_ADDR_SLE, sizeof(flash_sle_info), (uint8_t *)&flash_sle_info) == ERRCODE_SUCC) {
        return;
    } else {
        osal_printk("save_pair_info_ERR\r\n");
    }
}

void save_ble_pair_info(void)
{
    if (app_flash_updata(FLASH_START_ADDR_BLE, sizeof(flash_ble_info), (uint8_t *)&flash_ble_info) == ERRCODE_SUCC) {
        return;
    } else {
        osal_printk("save_pair_info_ERR\r\n");
    }
}

#if (APP_FLASH_DEBUG)
void app_flash_sensor_cpi(void)
{
    // 以50为单位，所以要除以50, 800:dpi_value
    m_PublicVar.UseModeConfig.Config.CPI_X.cpi_val_x.cpi_x_value16[0] = 800;
    m_PublicVar.UseModeConfig.Config.CPI_X.cpi_val_x.cpi_x_value16[1] = 1200;   // x轴1+1档位对应的值1200
    m_PublicVar.UseModeConfig.Config.CPI_X.cpi_val_x.cpi_x_value16[2] = 1600;   // x轴2+1档位对应的值1600
    m_PublicVar.UseModeConfig.Config.CPI_X.cpi_val_x.cpi_x_value16[3] = 2400;   // x轴3+1档位对应的值2400
    m_PublicVar.UseModeConfig.Config.CPI_X.cpi_val_x.cpi_x_value16[4] = 3200;   // x轴4+1档位对应的值3200
    m_PublicVar.UseModeConfig.Config.CPI_X.cpi_val_x.cpi_x_value16[5] = 6400;   // x轴5+1档位对应的值6400
    m_PublicVar.UseModeConfig.Config.CPI_X.cpi_val_x.cpi_x_value16[6] = 26000;  // x轴6+1档位对应的值26000
    m_PublicVar.UseModeConfig.Config.CPI_Y.cpi_val_y.cpi_y_value16[0] = 800;    // y轴1档位对应的值800
    m_PublicVar.UseModeConfig.Config.CPI_Y.cpi_val_y.cpi_y_value16[1] = 1200;   // y轴1+1档位对应的值1200
    m_PublicVar.UseModeConfig.Config.CPI_Y.cpi_val_y.cpi_y_value16[2] = 1600;   // y轴2+1档位对应的值1600
    m_PublicVar.UseModeConfig.Config.CPI_Y.cpi_val_y.cpi_y_value16[3] = 2400;   // y轴3+1档位对应的值2400
    m_PublicVar.UseModeConfig.Config.CPI_Y.cpi_val_y.cpi_y_value16[4] = 3200;   // y轴4+1档位对应的值3200
    m_PublicVar.UseModeConfig.Config.CPI_Y.cpi_val_y.cpi_y_value16[5] = 6400;   // y轴5+1档位对应的值6400
    m_PublicVar.UseModeConfig.Config.CPI_Y.cpi_val_y.cpi_y_value16[6] = 26000;  // y轴6+1档位对应的值26000
}

// 读取按键及模式配置
uint8_t app_flash_read_workmide_info(void)
{
    s_mouse_sensor.Sensor_type = mouse_sensor_seclect_type();

    // 模式配置 flash数据没有初始化进行初始化处理   @todo
    if (m_PublicVar.UseModeConfig.Config.cpi_led.mix_pra.cpi_Level8 > 6) {  // 6 dpi最高level
        m_PublicVar.UseModeConfig.Config.cpi_led.mix_pra.cpi_Level8 = 2;    // 默认cpi_level = 2
    }

    if (m_PublicVar.UseModeConfig.Config.cpi_led.mix_pra.led_mode8 > 4) {   // 4 灯光模式档位
        m_PublicVar.UseModeConfig.Config.cpi_led.mix_pra.led_mode8 = 4; // 4 灯光模式
    }

    if (m_PublicVar.UseModeConfig.Config.CPI_X.cpi_val_x.cpi_total8 > 6) {  // 6 cpi可设置的总档位
        if (s_mouse_sensor.Sensor_type == SENSOR_TYPE_PAW3395) {
            app_flash_sensor_cpi();
        } else if (s_mouse_sensor.Sensor_type == SENSOR_TYPE_PAW3950) {
            app_flash_sensor_cpi();
        }
        m_PublicVar.UseModeConfig.Config.CPI_X.cpi_val_x.cpi_total8 = 6;    // 6 CPI 可设总档位字节
    }

    if (m_PublicVar.UseModeConfig.Config.CPI_X.cpi_val_x.cpi_x_value16[0] == 0xffff) {
        if (s_mouse_sensor.Sensor_type == SENSOR_TYPE_PAW3395) {
            app_flash_sensor_cpi();
        } else if (s_mouse_sensor.Sensor_type == SENSOR_TYPE_PAW3950) {
            app_flash_sensor_cpi();
        }
        m_PublicVar.UseModeConfig.Config.CPI_X.cpi_val_x.cpi_total8 = 6;    // 6 CPI 可设总档位字节
    }

    return true;
}

// 上电后读取flash中的数据
uint8_t app_flash_poweron_get_flash(void)
{
    uint8_t result = true;
    osal_printk("- - - - - - >Power On Get Flash\r\n");
    // 读取配对信息及FW信息
    uapi_sfc_reg_read(FLASH_START_ADDR_FW, (uint8_t *)&m_PublicVar.UsePairFWInfo, sizeof(m_PublicVar.UsePairFWInfo));

    if ((m_PublicVar.UsePairFWInfo.usbVID != APP_USBD_VID) || (m_PublicVar.UsePairFWInfo.usbPID != APP_USBD_PID)) {
        // FLASH没有初始化，第一次使用时进行初始化处理          @todo
        m_PublicVar.UsePairFWInfo.usbVID = APP_USBD_VID;
        m_PublicVar.UsePairFWInfo.usbPID = APP_USBD_PID;
        m_PublicVar.UsePairFWInfo.firmware_version = MOUSE_FW_REVISION;
        m_PublicVar.UsePairFWInfo.usb_mouse_workmode = SENSOR_LOW_POWER_MODE;  // 默认
        m_PublicVar.UsePairFWInfo.sle_mouse_workmode = SENSOR_LOW_POWER_MODE;  // 默认
        m_PublicVar.UsePairFWInfo.ble_mouse_workmode = SENSOR_OFFICE_MODE;     // 默认
        m_PublicVar.UsePairFWInfo.BLEOr24GFlag = FLASH_PROTOCOL_SLE_FLAG;      // 上电默认SLE
    }
    app_flash_updata_flash_buffer();
    return result;
}
#endif
