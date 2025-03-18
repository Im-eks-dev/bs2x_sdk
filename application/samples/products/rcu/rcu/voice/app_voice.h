/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: APP Voice Header File. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-28, Create file. \n
 */
#ifndef APP_VOICE_H
#define APP_VOICE_H
 
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */
 
#define RING_BUFFER_NUMBER                 4
 
extern uint8_t g_write_buffer_node;
extern uint8_t g_read_buffer_node;
extern uint32_t *g_pdm_dma_data[RING_BUFFER_NUMBER];
 
void rcu_amic_init(uint16_t g_conn_id);
void rcu_amic_deinit(uint16_t g_conn_id);
 
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
 
#endif