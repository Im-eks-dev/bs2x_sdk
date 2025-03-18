/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE MEASURE_DIS sample of client. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-03, Create file. \n
 */
#ifndef SLE_MEASURE_DIS_CLIENT_SLEM_H
#define SLE_MEASURE_DIS_CLIENT_SLEM_H

#include "sle_measure_dis_client_slem.h"
#include "sle_hadm_manager.h"

#define MEASURE_DIS_IQ_REPORT_CNT_MAX 4
#define POSALG_DATA_NUM 79
#define IQ_DATA_MAX (MEASURE_DIS_IQ_REPORT_CNT_MAX * SLE_CS_IQ_REPORT_COUNT)

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

#endif