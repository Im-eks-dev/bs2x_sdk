#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "pm_clock")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_clock.c
)

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
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
