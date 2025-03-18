/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE measure_dis server Config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-17, Create file. \n
 */

#ifndef SLE_MEASURE_DIS_SERVER_H
#define SLE_MEASURE_DIS_SERVER_H

#include <stdint.h>
#include <securec.h>
#include "errcode.h"
#include "std_def.h"
#include "soc_osal.h"
#include "common_def.h"
#include "sle_ssap_server.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MEASURE_DIS_NUM_CARRY_10_7     10000000
#define MEASURE_DIS_NUM_CARRY_1000     1000
#define MEASURE_DIS_NUM_CARRY_100      100
#define MEASURE_DIS_NUM_CARRY_10       10
#define MEASURE_DIS_NUM_CARRY_2        2

#define MEASURE_DIS_MSG_EVENT 1
#define MEASURE_DIS_MSG_QUEUE_SIZE 10

extern osal_event measure_dis_evt;
extern uint8_t g_measure_dis_conn_id;

typedef enum {
    SLEM_PROFILE_MSG_IQ = 0xFFFFFFEA,
} slem_profile_msg_type_t;

typedef enum {
    SLEM_MSG_LOCAL_IQ,
    SLEM_MSG_REMOTE_IQ,
} slem_msg_node_type_t;

typedef struct {
    uint32_t        type;                             /*!< 消息类型 */
    uint32_t        len;                              /*!< 消息类型 */
    uint8_t         data[0];
} measure_ids_msg_t;

typedef struct server_data {
    uint8_t property_uuid[SLE_UUID_LEN];
    uint8_t server_uuid[SLE_UUID_LEN];
    uint8_t service_uuid[SLE_UUID_LEN];
    uint16_t begin_hdl;
    uint16_t end_hdl;
    uint16_t property_handle;
} measure_dis_server_data_t;

typedef struct {
    uint16_t            conn_id;                          /*!< 连接句柄 */
    uint16_t            type;                             /*!< 消息类型 */
    uint16_t            result;                           /*!< 执行结果 */
    void                *data;
} measure_dis_msg_node_t;

static inline void check_rc_return_rc(uint32_t rc, char *err)
{
    do {
        if ((rc) != ERRCODE_SUCC) {
            osal_printk("CARKEY ERROR: %s fail!: call %s return 0x%x!\n", err, __FUNCTION__, rc);
        }
    } while (0);
}

int measure_dis_server_write_client(uint32_t type, uint8_t *data, uint32_t data_len);
int measure_dis_server_init(void);
errcode_t sle_measure_dis_msg_add(measure_dis_msg_node_t *msg);
void sle_measure_dis_msg_proc(void);
int sle_measure_recv_dis(uint8_t *value, uint16_t len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif