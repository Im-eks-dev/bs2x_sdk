#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "imu_wakeup_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/imu_wakeup_porting.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(WHOLE_LINK
    true
)

if(DEFINED CONFIG_BUILD_USING_STATIC_LIBRARIES_SDK)
    if(EXISTS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
        set(SOURCES )
        set(LIBS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    endif()
endif()

build_component()