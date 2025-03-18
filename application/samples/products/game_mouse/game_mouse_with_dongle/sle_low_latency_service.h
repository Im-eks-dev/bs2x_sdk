/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Mouse Low Latency Inc. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#ifndef SLE_LOW_LAYENCY_SERVICE_H
#define SLE_LOW_LAYENCY_SERVICE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */
typedef enum {
    SLE_MOUSE_NONE,
    SLE_MOUSE_3Hz    = 3,
    SLE_MOUSE_10Hz   = 10,
    SLE_MOUSE_125Hz  = 125,
    SLE_MOUSE_250Hz  = 250,
    SLE_MOUSE_500Hz  = 500,
    SLE_MOUSE_1000Hz = 1000,
    SLE_MOUSE_2000Hz = 2000,
    SLE_MOUSE_4000Hz = 4000,
    SLE_MOUSE_8000Hz = 8000,
} app_sle_report_rate_t;

typedef enum {
    RATE_LEVEL_0,
    RATE_LEVEL_1,
    RATE_LEVEL_2,
    RATE_LEVEL_3,
    RATE_LEVEL_4,
    RATE_LEVEL_5,
    RATE_LEVEL_6,
} app_sle_rate_level_t;

void sle_low_latency_dongle_init(int usb_hid_index);
bool usb_dongle_is_recv_vaild_data(void);
void set_sle_dongle_report_rate(uint16_t report_rate);
uint16_t get_sle_dongle_report_rate(void);
void app_dongle_send_msg(uint8_t msg_event, uint8_t msg_sub_event);
void set_sle_dongle_dpi_data(uint32_t dpi);
void usb_sle_high_mouse_report(uint16_t lenth, uint8_t *data);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
