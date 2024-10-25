/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: TIoT firmware load module xci cfg handle. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-12-05, Create file. \n
 */
#ifndef TIOT_CFG_HANDLE_XCI_H
#define TIOT_CFG_HANDLE_XCI_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_common_firmware_cfg_handle_priv CFG HANDLE XCI
 * @ingroup  tiot_common_firmware
 * @{
 */

/**
 * @brief  TIoT firmware xci extend command handle.
 * @param  [in]  fw TIoT firmware object.
 * @param  [in]  ext_cmd Extend command.
 * @param  [in]  cmd Firmware Command.
 * @param  [in]  len Firmware Command length.
 * @return if OK, return 0, if need retry ,return 1, else return -1.
 */
int32_t tiot_fw_ext_cmd_handle_xci(tiot_fw *fw, uint16_t ext_cmd, const uint8_t *cmd, uint16_t len);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif