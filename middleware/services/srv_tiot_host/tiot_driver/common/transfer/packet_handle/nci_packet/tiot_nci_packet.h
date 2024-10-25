/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: TIOT nci packet header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-01-02, Create file. \n
 */
#ifndef TIOT_NCI_PACKET_H
#define TIOT_NCI_PACKET_H

#include "tiot_xfer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_common_transfer_packet_handle_tiot_nci_packet Packet Handle
 * @ingroup  tiot_common_transfer_packet_handle
 * @{
 */

/**
 * @brief  rx data read out, copy to user buffer[data, len].
 * @return int32_t if OK, return readout length;if wait timeout, return 0; if err, return -1.
 */
int32_t tiot_nci_rx_data_out(tiot_xfer_manager *xfer, uint8_t *data, uint32_t len, uint32_t timeout_ms);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* TIOT_NCI_PACKET_H */