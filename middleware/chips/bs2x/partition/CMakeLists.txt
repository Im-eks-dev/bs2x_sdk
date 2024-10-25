#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "partition_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/partition_porting.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ROOT_DIR}/middleware/chips/${CHIP}/partition/include
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
"-Wno-incompatible-pointer-types"
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
