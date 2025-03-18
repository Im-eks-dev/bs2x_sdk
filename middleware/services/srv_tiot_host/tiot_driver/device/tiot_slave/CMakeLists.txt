#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_DEV_TIOT_SLAVE)
set(
    device_include_list
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(
    device_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/tiot_slave_device_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tiot_slave_service.c
)

set(TIOT_HEADER_LIST "${TIOT_HEADER_LIST}" "${device_include_list}" CACHE INTERNAL "tiot header list" FORCE)
set(TIOT_SRC_LIST "${TIOT_SRC_LIST}" "${device_src_list}" CACHE INTERNAL "tiot src list" FORCE)
endif()
