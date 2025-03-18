/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: DONGLE MP Test header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-25, Create file. \n
 */
#ifndef SLE_RCU_DONGLE_MP_TEST_H
#define SLE_RCU_DONGLE_MP_TEST_H
#include <stdint.h>
#include <stdbool.h>
#include "osal_task.h"
#include "sle_ssap_client.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup APP_SAMPLES_RCU_DONGLE
 * @ingroup  APP_SAMPLES
 * @{
 */
#define TLV_TYPE_TYPE                       1
#define TLV_TYPE_LEN                        2
#define BODY_LEN                            2
#define TLV_TYPE_VALUE                      4
#define COMMAND_ID_BYTE                     6
#define MP_TEST_INFO_LEN                    64
#define PRIVATE_SLE_FRAME_FLAG              0x1F
#define MP_TEST_CMD_BIT                     6
#define MP_TEST_TOTAL_LEN_BIT               7
#define MP_TEST_TLV_LEN_BIT                 14
#define MP_TEST_TLV_DATA_BIT                13
#define MP_TEST_TLV_MAX_LEN                 46
#define MP_TEST_TLV_AVG_LEN                 36
#define MP_TEST_MAX_STATION_NUM             6
#define MP_TEST_GET_MAC_CMD                 0x1
#define MP_TEST_GET_VER_CMD                 0x2
#define MP_TEST_SET_RSSI_CMD                0x3
#define MP_TEST_CLR_CONN_CMD                0x4
#define MP_TEST_SET_TEST_STATION_CMD        0x5
#define MP_TEST_GET_KEY_CMD                 0x6
#define MP_TEST_GET_VOICE_CMD               0x7
#define MP_TEST_STOP_VOICE_CMD              0x8
#define MP_TEST_DELAY_US                    (500UL)
#define MP_TEST_SEND_ENDPOINT               1
#define MP_TEST_SUCC_RET                    0x66
#define MP_TEST_FAIL_RET                    0xFF

bool get_send_voice(void);
void *rcu_dongle_mp_test_task(const char *arg);
void rcu_dongle_mp_test_submit(ssapc_handle_value_t *data);
void rcu_dongle_mp_test_set_test_station(uint8_t *tlv);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif