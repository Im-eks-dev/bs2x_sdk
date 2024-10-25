/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: SLE OTA sample of client. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-02-01, Create file. \n
 */
#ifndef SLE_OTA_CMD_HANDLER_H
#define SLE_OTA_CMD_HANDLER_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    SERVICE_ID_SERVICE_DISCOVER = 1,
    SERVICE_ID_SERVICE_CONNECT,
    SERVICE_ID_CLIENT_MANAGE,
    SERVICE_ID_SERVICE_MANAGE,
    SERVICE_ID_FACTORY_TEST_SERVICE,
    SERVICE_ID_LOW_LATENCY_SERVICE_MANAGE,
} sle_ota_service_type_t;

typedef enum {
    COMMAND_ID_ENABLE_SLE = 1,
    COMMAND_ID_DISABLE,
    COMMAND_ID_SET_DEVICE_ADDR,
    COMMAND_ID_GET_DEVICE_ADDR,
    COMMAND_ID_SET_DEVICE_NAME,
    COMMAND_ID_GET_DEVICE_NAME,
    COMMAND_ID_SET_ANNOUNCE_DATA,
    COMMAND_ID_SET_ANNOUNCE_PARAM,
    COMMAND_ID_SET_START_ANNOUNCE,
    COMMAND_ID_SET_END_ANNOUNCE,
    COMMAND_ID_SET_SCAN_PARAM,
    COMMAND_ID_UPLOAD_SCAN_RESULT,
    COMMAND_ID_START_SCAN,
    COMMAND_ID_STOP_SCAN,
} sle_ota_service_discovery_command_t;

typedef enum {
    COMMAND_ID_SEND_LINK_REQ = 1,
    COMMAND_ID_BREAK_LINK_REQ,
    COMMAND_ID_UPG_LINK_PARAM,
    COMMAND_ID_SEND_BOUND_REQ,
    COMMAND_ID_DELETE_BOUND,
    COMMAND_ID_DELETE_ALL_BOUND,
    COMMAND_ID_GET_BOUND_DEVICE_NUM,
    COMMAND_ID_GET_BOUND_DEVICE,
    COMMAND_ID_GET_BOUND_STATUS,
    COMMAND_ID_GET_DEVICE_RSSI,
    COMMAND_ID_GET_ACB_PARAM,
    COMMAND_ID_SET_PHY_PARAM,
    COMMAND_ID_SET_POWRER_MAXIMUM,
    COMMAND_ID_SEND_LINK_MANAGEMEND_CHECK,
} sle_ota_service_connect_command_t;

typedef enum {
    COMMAND_ID_REGISTER_CLIENT = 1,
    COMMAND_ID_UNREGISTER_CLIENT,
    COMMAND_ID_SEARCH_SERVER_DESCRIPTION,
    COMMAND_ID_UUID_READ_REQ,
    COMMAND_ID_HANDLE_READ_REQ,
    COMMAND_ID_WRITE_REQ,
    COMMAND_ID_WRITE_CMD,
    COMMAND_ID_EXCHANGE_INFO_REQ,
} sle_ota_client_service_command_t;

typedef struct {
    uint8_t        flag;
    uint8_t        version;
    uint8_t        total_frame;
    uint8_t        frame_seq;
    uint8_t        rsv;
    uint8_t        service_id;
    uint8_t        command_id;
    uint8_t        body_len[2];
    uint8_t        tlv[0];
} sle_ota_frame_header_t;

typedef struct {
    uint8_t        flag;
    uint8_t        version;
    uint8_t        total_frame;
    uint8_t        frame_seq;
    uint8_t        rsv;
    uint8_t        service_id;
    uint8_t        command_id;
    uint8_t        body_len[2];
    uint8_t        type;
    uint8_t        len[2];
    uint8_t        errorcode;
    uint8_t        mic[2];
} sle_ota_response_frame_t;

typedef struct {
    uint8_t        flag;
    uint8_t        version;
    uint8_t        total_frame;
    uint8_t        frame_seq;
    uint8_t        rsv;
    uint8_t        service_id;
    uint8_t        command_id;
    uint8_t        body_len[2];
    uint8_t        data[0];
} ota_transmit_frame_header_t;

typedef struct {
    uint8_t        type;
    uint8_t        type_len;
    uint8_t        type_value;
    uint8_t        frame_seq;
    uint8_t        rsv;
    uint8_t        service_id;
    uint8_t        command_id;
    uint8_t        body_len[2];
    uint8_t        data[0];
} ota_local_device_t;

typedef struct {
    uint8_t type;
    uint16_t len;
    uint8_t data[0];
} ota_tlv_t;

void sle_ota_set_hid_index(uint32_t index);
uint32_t sle_ota_get_hid_index(void);
errcode_t sle_ota_discover_device_service(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length);
errcode_t sle_ota_manage_connection_service(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length);
errcode_t sle_ota_manage_ssap_client(uint8_t service_id, uint8_t command_id, uint8_t *buffer, uint16_t length);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif