/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE ADV Config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-17, Create file. \n
 */

#ifndef SLE_MEASURE_DIS_SERVER_ALG_H
#define SLE_MEASURE_DIS_SERVER_ALG_H

#include "sle_hadm_manager.h"

#define MEASURE_DIS_INVALID (-1)
#define MEASURE_DIS_RSSI_INVALID (-128)
#define MEASURE_DIS_HIGH_INVALID (-10)
#define MEASURE_DIS_IQ_REPORT_CNT_MAX 4
#define POSALG_DATA_NUM 79
#define IQ_DATA_MAX (MEASURE_DIS_IQ_REPORT_CNT_MAX * SLE_CS_IQ_REPORT_COUNT)
#define POSALG_RSSI_OFFSET 256
#define POSALG_CALIB_VAL 0.8
#define POSALG_IQ_FLIP_BARRIER 1023
#define POSALG_IQ_FLIP_OFFSET 2048
#define MEASURE_DIS_TIMESTAMP_DIFF_MAX 10

typedef struct {
    int key_num;
    int key_num_all;
} measure_dis_keys_info;

typedef struct {
    uint32_t                  sn;                               /*!< 测量SN */
    float                     dist_first;                       /*!< 第一个测距值 */
    float                     dist_second;                      /*!< 第二个测距值 */
    float                     dist_double;                      /*!< 第二个测距值 */
    float                     rssi;                             /*!< 信号强度 */
    float                     high;                             /*!< 置信度相关系数 */
    float                     prob;                             /*!< 置信度 */
    unsigned char             smooth_num;                       /*!<有效平滑次数 */
} measure_dis_profile_msg_dis_t;

typedef struct {
    uint16_t i_data;
    uint16_t q_data;
} measure_dis_stored_qte_trans_t;

typedef struct {
    uint8_t samp_cnt;
    uint8_t rssi;
    uint16_t es_sn;
    uint32_t timestamp_sn;
    measure_dis_stored_qte_trans_t data[IQ_DATA_MAX];
#if (defined(GLE_CS_MODE3_SUPPORT))
    uint32_t tof_result;
#endif
} measure_dis_stored_iq_data_t;

typedef struct {
    uint16_t i_data;
    uint16_t q_data;
} sle_channel_sounding_qte_trans_t;

typedef struct {
    uint8_t samp_cnt;
    uint8_t rssi;
    uint16_t es_sn;
    uint32_t timestamp_sn;
    sle_channel_sounding_qte_trans_t data[IQ_DATA_MAX];
    uint32_t tof_result;
} sle_channel_sounding_iq_trans_t;

errcode_t measure_dis_reg_callbacks(void);
errcode_t measure_dis_match_msg(measure_dis_msg_node_t *msg_node);
errcode_t measure_dis_remote_iq(uint16_t len, uint8_t *value);

#endif