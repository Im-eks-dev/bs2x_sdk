#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_DONGLE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_micro_with_dongle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_micro_usb/sle_micro_usb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_micro_client/sle_micro_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_micro_codec/sle_micro_codec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_micro_low_lantency.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_SERVER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_micro_with_dongle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_micro_server/sle_micro_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_micro_server/sle_micro_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_micro_codec/sle_micro_codec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_micro_low_lantency.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_micro_i2s/i2s_slave_dma.c
)
endif()
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)