#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
#===============================================================================
set(
    tiot_product_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/tiot_slave_board_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/tiot_board_uart_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/tiot_board_pin_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/tiot_board_misc_port.c
)

set(
    tiot_product_header_list
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common"
)

set(TIOT_HEADER_LIST "${TIOT_HEADER_LIST}" "${tiot_product_header_list}" CACHE INTERNAL "tiot header list" FORCE)
set(TIOT_SRC_LIST "${TIOT_SRC_LIST}" "${tiot_product_src_list}" CACHE INTERNAL "tiot src list" FORCE)