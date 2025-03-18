/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Provides ble hid Server demo API \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-12-29， Create file. \n
 */

/**
 * @defgroup bluetooth_bts_hid_server HID SERVER API
 * @ingroup  bluetooth
 * @{
 */
#ifndef PM_BLE_HID_SERVER_H
#define PM_BLE_HID_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Human Interface Device UUID */
#define BLE_UUID_HUMAN_INTERFACE_DEVICE 0x1812
/* HID Information UUID */
#define BLE_UUID_HID_INFORMATION 0x2A4A
/* Report Map UUID */
#define BLE_UUID_REPORT_MAP 0x2A4B
/* HID Control Point UUID */
#define BLE_UUID_HID_CONTROL_POINT 0x2A4C
/* Report UUID */
#define BLE_UUID_REPORT 0x2A4D
/* Protocol Mode UUID */
#define BLE_UUID_PROTOCOL_MODE 0x2A4E
/* Client Characteristic Configuration UUID */
#define BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION 0x2902
/* External Report Reference UUID */
#define BLE_UUID_EXTERNAL_REPORT_REFERENCE 0x2907
/* Report Reference UUID */
#define BLE_UUID_REPORT_REFERENCE 0x2908

#ifndef BTH_BLE_HCI_ONETRACK
#pragma pack (1)
#endif
#ifndef BTH_BLE_HCI_ONETRACK
#pragma pack ()
#endif

/**
 * @if Eng
 * @brief  Use this funtion to change name value.
 * @par name, len.
 * @attention  NULL
 * @param  NULL
 * @retval NULL
 * @par Dependency:
 * @li ble_hid_server.h
 * @else
 * @brief  设置注册服务时的name
 * @par NULL
 * @attention  NULL
 * @param  NULL
 * @retval NULL
 * @par 依赖:
 * @li ble_hid_server.h
 * @endif
 */
void pm_ble_hid_set_name_value(const uint8_t *name, const uint8_t len);

/**
 * @if Eng
 * @brief  Use this funtion to change appearance value.
 * @par appearance.
 * @attention  NULL
 * @param  NULL
 * @retval NULL
 * @par Dependency:
 * @li ble_hid_server.h
 * @else
 * @brief  设置注册服务时的appearance
 * @par NULL
 * @attention  NULL
 * @param  NULL
 * @retval NULL
 * @par 依赖:
 * @li ble_hid_server.h
 * @endif
 */
void pm_ble_hid_set_appearance_value(uint16_t appearance);

/**
 * @if Eng
 * @brief  Use this funtion to init HID service.
 * @par Description:add HID service, character and character descriptor, then start service.
 * @attention  NULL
 * @param  NULL
 * @retval NULL
 * @par Dependency:
 * @li ble_hid_server.h
 * @else
 * @brief  初始化HID服务。
 * @par 说明:添加HID服务，特征，特征描述符，然后启动服务。
 * @attention  NULL
 * @param  NULL
 * @retval NULL
 * @par 依赖:
 * @li ble_hid_server.h
 * @endif
 */
void pm_ble_hid_server_init(void);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif
