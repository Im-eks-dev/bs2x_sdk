#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SPI_TRX_MASTER)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/spi_trx_master_demo.c" PARENT_SCOPE)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SPI_TRX_SLAVE)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/spi_trx_slave_demo.c" PARENT_SCOPE)
endif()