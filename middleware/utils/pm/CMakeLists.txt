#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(pm_dev)
add_subdirectory_if_exist(pm_fsm)
add_subdirectory_if_exist(pm_veto)
add_subdirectory_if_exist(pm_sleep)
add_subdirectory_if_exist(pm_dfx)
add_subdirectory_if_exist(pm_core)

set(COMPONENT_NAME "pm")

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(DEFINED CONFIG_BUILD_USING_STATIC_LIBRARIES_SDK)
    if(EXISTS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
        set(SOURCES )
        set(LIBS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    endif()
endif()

build_component()