# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.

set(COMPONENT_NAME "cmn_header")

add_subdirectory_if_exist(driver)
add_subdirectory_if_exist(middleware)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/middleware/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/middleware/services/wifi
    ${CMAKE_CURRENT_SOURCE_DIR}/middleware/services/radar
)

if(DEFINED CONFIG_BUILD_USING_STATIC_LIBRARIES_SDK)
    if(EXISTS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
        set(SOURCES )
        set(LIBS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    endif()
endif()

build_component()
