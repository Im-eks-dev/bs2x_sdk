#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "build_version")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/private/build_version.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/public
)

set(PRIVATE_HEADER
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

if(DEFINED CONFIG_BUILD_USING_STATIC_LIBRARIES_SDK)
    if(EXISTS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
        set(SOURCES )
        set(LIBS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    endif()
endif()

build_component()
