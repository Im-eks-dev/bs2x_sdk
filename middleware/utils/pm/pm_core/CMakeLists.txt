#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "pm_core")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_core.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_core_ballot/pm_core_ballot.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_core_fsm/pm_core_fsm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_core_int/pm_core_int.c
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_core_ballot
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_core_fsm
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_core_int
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
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
