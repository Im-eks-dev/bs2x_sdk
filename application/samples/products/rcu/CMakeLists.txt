#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER AND DEFINED CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server/sle_rcu_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server/sle_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_codec/sle_vdt_codec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_pdm/sle_vdt_pdm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_hid_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_rcu_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/rcu.c
)
set(HEADER_LIST
${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_codec
${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_pdm
${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server
${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER AND DEFINED CONFIG_SAMPLE_SUPPORT_IR)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server/sle_rcu_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server/sle_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_codec/sle_vdt_codec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_pdm/sle_vdt_pdm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/ir_nec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/rcu.c
)
set(HEADER_LIST
${CMAKE_CURRENT_SOURCE_DIR}/ir
${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_codec
${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_pdm
${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server/sle_rcu_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server/sle_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_codec/sle_vdt_codec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_pdm/sle_vdt_pdm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/rcu.c
)
set(HEADER_LIST
${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_codec
${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_pdm
${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_hid_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_rcu_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/rcu.c
)
set(HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RCU_DONGLE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/sle_rcu_dongle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/sle_rcu_hid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/sle_rcu_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_codec/sle_vdt_codec.c
)
set(HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_amic/sle_amic_vdt_codec)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_IR)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/ir_demo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/ir_nec.c
)
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)