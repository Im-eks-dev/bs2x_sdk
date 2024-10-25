#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "std_rom_lds_porting")

if(NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    return()
endif()

set(PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR})
set(LINK_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker.prelds" CACHE INTERNAL "" FORCE)
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}" "*.lds")
if(DEFINED CONFIG_BUILD_USING_STATIC_LIBRARIES_SDK)
    if(EXISTS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
        set(SOURCES )
        set(LIBS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    endif()
endif()

build_component()
