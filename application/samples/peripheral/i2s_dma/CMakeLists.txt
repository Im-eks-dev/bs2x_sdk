#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_I2S_DMA_MASTER)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/i2s_dma_master_demo.c" PARENT_SCOPE)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_I2S_DMA_SLAVE)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/i2s_dma_slave_demo.c" PARENT_SCOPE)
endif()