#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_MEASURE_DIS_CLIENT)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_measure_dis_client/sle_measure_dis_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_measure_dis_client/sle_measure_dis_client_slem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_measure_dis.c
)
set(HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/sle_measure_dis_client)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_MEASURE_DIS_SERVER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_measure_dis_server/sle_measure_dis_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_measure_dis_server/sle_measure_dis_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_measure_dis_server/sle_measure_dis_server_alg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_measure_dis.c
)
set(HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/sle_measure_dis_server)
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)