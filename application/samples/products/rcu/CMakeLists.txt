#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER OR DEFINED CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER OR DEFINED CONFIG_SAMPLE_SUPPORT_IR)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/rcu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/keyscan/app_keyscan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/common/app_msg_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/common/app_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/app/app_status.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/app/app_ulp.c
)
set(HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/keyscan
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/common
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/app
)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
list(APPEND SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server/sle_rcu_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server/sle_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_codec/sle_vdt_codec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_pdm/sle_vdt_pdm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/common/app_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/voice/app_voice.c
)
list(APPEND HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_codec
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_pdm
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_service
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/common
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/voice
)
if(DEFINED CONFIG_RCU_MASS_PRODUCTION_TEST)
    list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/rcu/mp_test/rcu_mp_test.c)
    list(APPEND HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/rcu/mp_test)
endif()
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
list(APPEND SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_hid_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_rcu_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_rcu_server.c
)
list(APPEND HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server
)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_IR)
list(APPEND SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/ir_demo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/ir_nec.c
)
list(APPEND HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ir
)
endif()

#===============================================================================
# DONGLE
# 
#===============================================================================

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RCU_DONGLE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/sle_rcu_dongle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/sle_rcu_hid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/sle_rcu_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_codec/sle_vdt_codec.c
)
set(HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_codec
)
if(DEFINED CONFIG_RCU_MASS_PRODUCTION_TEST)
    list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/dongle/mp_test/rcu_dongle_mp_test.c)
    list(APPEND HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/dongle/mp_test)
endif()
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)