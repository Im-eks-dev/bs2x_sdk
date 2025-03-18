/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse Client Manager  Inc. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#ifndef SLE_MOUSE_CLIENT_H
#define SLE_MOUSE_CLIENT_H

#include "sle_ssap_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @addtogroup connectivity_main
 * @{
 */
typedef enum {
    SLE_DONGLE_INIT_PROTOCOL_STACK,
    SLE_DONGLE_SCAN_DEVICES,
    SLE_DONGLE_CONNECTED,
    SLE_DONGLE_PAIR_DEVICES,
    SLE_DONGLE_DISCONNECTED,
    SLE_DONGLE_DISCOVERY_SERVICES,
    SLE_DONGLE_WAKE_UP_HOST,
    SLE_DONGLE_CHANGE_MOUSE_REPORT_RATE,
    SLE_DONGLE_ENABLE_LOW_LATENCY,
    SLE_DONGLE_DISABLE_LOW_LATENCY,
    SLE_DONGLE_HID_SEND_DATA,
    SLE_DONGLE_SET_DPI,
    SLE_DONGLE_INVAILD = 0xFF,
} app_dongle_event_t;

#define DONGLE_PACKAGE_HEAD 0x05
#define DONGLE_PACKAGE_TYPE 0x5B
#define DONGLE_PACKANG_LENGTH 5

typedef struct {
    uint8_t head;
    uint8_t type;
    uint16_t length;
    uint8_t cmd;
    uint8_t data[4];
} app_dongle_package_t;

typedef enum {
    COMMAND_SEND_DPI = 0x00,
    COMMAND_DPI_CHECK = 0x01,
    COMMAND_SEND_REPORT = 0x02,
    COMMAND_REPORT_CHECK = 0x03,
    COMMAND_BATTERY_LAVEL_CHECK = 0x04,
} app_dongle_send_cmd_t;

#define HID_DATA_DPI 0
#define HID_DATA_REPORT 1

void sle_mouse_client_init(void);
void sle_mouse_client_start_scan(void);
void sle_mouse_client_discover_service(void);
void sle_mouse_client_exchange_mtu(uint32_t length);
void sle_mouse_client_send_customer_data(uint8_t *data, uint16_t len);
void sle_mouse_client_pair_device(void);
void sle_mouse_client_disconnect_process(void);
uint8_t sle_mouse_client_get_connect_state(void);
uint16_t sle_mouse_client_get_connect_id(void);
int sle_mouse_client_parser_customer_data(uint8_t *buf, uint16_t len);
void sle_mouse_client_ssapc_write_req(uint8_t cmd, uint8_t *buf, uint16_t length);
void sle_mouse_client_set_is_pair_complete(bool flag);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
