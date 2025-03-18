/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 *
 * Description: BLE Mouse BAS and DIS Service Server. \n
 *
 * History: \n
 * 2024-04-21, Create file. \n
 */
#ifndef BLE_BAS_AND_DIS_SERVER_H
#define BLE_BAS_AND_DIS_SERVER_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

errcode_t ble_bas_server_battery_level_notify(uint8_t percentage);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif

#endif /* SRC_BLE_BAS_SERV_H_ */