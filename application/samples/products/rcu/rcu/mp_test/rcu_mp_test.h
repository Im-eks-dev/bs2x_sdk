/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: RCU MP Test header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-25, Create file. \n
 */
#ifndef RCU_MP_TEST_H
#define RCU_MP_TEST_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup APP_SAMPLES_RCU
 * @ingroup  APP_SAMPLES
 * @{
 */
#define TLV_TYPE_AND_LEN                     3
#define PRIVATE_FRAME_TLV_DATA_BYTE         12
#define PRIVATE_FRAME_INFO_LEN              64
#define PRIVATE_FRAME_FLAG                  0x1F
#define PRIVATE_FRAME_CMD_GET_MAC_ADDR      0x1
#define PRIVATE_FRAME_CMD_GET_VERSION       0x2
#define PRIVATE_FRAME_CMD_GET_KEY_VALUE     0x6
#define MP_TEST_MAX_STATION_NUM              6
#define TLV_LEN_BYTE1                        10
#define BODYLEN_BYTE                         7
#define VERSION_LEN                          4
typedef enum private_frame_service_id {
    PRIVATE_FRAME_SERVICE_DEVICE_DISCOVERY = 0x1,
    PRIVATE_FRAME_SERVICE_CONN_MANAGER,
    PRIVATE_FRAME_SERVICE_SSAP_CLIENT,
    PRIVATE_FRAME_SERVICE_SSAP_SERVER,
    PRIVATE_FRAME_SERVICE_FACTORY_TEST
} private_frame_service_id_t;

typedef struct private_frame {
    uint8_t flag;
    uint8_t version;
    uint8_t total_frame;
    uint8_t frame_seq;
    uint8_t rsv[1];
    uint8_t service_id;
    uint8_t cmd_id;
    uint16_t body_len;
} private_frame_t;

void rcu_mp_test_handler(private_frame_t *private_frame, uint8_t *cmd, uint16_t cmd_len);
void rcu_mp_test_rcu_to_dongle_key_value(uint8_t key_nums, uint8_t *key_values);
void rcu_mp_test_set_work_station(uint8_t combine_key_flag);
int rcu_mp_test_set_default_announce_param(sle_addr_t *addr);
void rcu_out_of_production_testing(void);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif