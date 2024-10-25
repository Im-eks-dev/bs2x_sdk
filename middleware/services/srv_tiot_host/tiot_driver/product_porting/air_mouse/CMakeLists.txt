#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(
    tiot_product_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/w33_board_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/tiot_board_uart_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/tiot_board_pin_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/tiot_board_misc_port.c
)

set(
    tiot_product_header_list
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common"
)

if(DEFINED CONFIG_FILE_BY_ARRAY)
list(
    APPEND tiot_product_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/firmware/w33_fw_file_array.c
)

list(
    APPEND tiot_product_header_list
    "${CMAKE_CURRENT_SOURCE_DIR}/firmware"
)
endif()

set(TIOT_HEADER_LIST "${TIOT_HEADER_LIST}" "${tiot_product_header_list}" CACHE INTERNAL "tiot header list" FORCE)
set(TIOT_SRC_LIST "${TIOT_SRC_LIST}" "${tiot_product_src_list}" CACHE INTERNAL "tiot src list" FORCE)