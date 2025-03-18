#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_MULTI_CONN_SERVER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/server/sle_multi_conn_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/server/sle_multi_conn_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_multi_conn.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_MULTI_CONN_CLIENT)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/client/sle_multi_conn_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_multi_conn.c
)

endif()
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
