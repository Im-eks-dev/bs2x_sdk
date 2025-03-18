#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "samples")

set(SOURCES
)

set(PUBLIC_HEADER
)

if(DEFINED CONFIG_ENABLE_BT_SAMPLE)
    add_subdirectory_if_exist(bt)
endif()
if(DEFINED CONFIG_ENABLE_PERIPHERAL_SAMPLE)
    add_subdirectory_if_exist(peripheral)
endif()
if(DEFINED CONFIG_ENABLE_WIFI_SAMPLE)
    add_subdirectory_if_exist(wifi)
endif()
if(DEFINED CONFIG_ENABLE_PRODUCTS_SAMPLE)
    add_subdirectory_if_exist(products)
endif()
if(DEFINED CONFIG_ENABLE_RADAR_SAMPLE)
    add_subdirectory_if_exist(radar)
endif()
if(DEFINED CONFIG_ENABLE_NFC_SAMPLE)
    add_subdirectory_if_exist(nfc)
endif()

add_subdirectory_if_exist(custom)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
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

if(DEFINED CONFIG_ENABLE_XF_SUPPORT)
    if(NOT DEFINED ENV{XF_PROJECT_PATH})
    message(FATAL_ERROR "XF_PROJECT_PATH environment variable not detected, please run in xf command line!")
    endif()

    message("XF_PROJECT_PATH:   $ENV{XF_PROJECT_PATH}")
    include("$ENV{XF_PROJECT_PATH}/build/build_environ.cmake")

    message("XF_SRCS_STR:       ${XF_SRCS_STR}")
    message("XF_INCS_STR:       ${XF_INCS_STR}")
    message("XF_CFLAGS_STR:     ${XF_CFLAGS_STR}")

    set(SOURCES ${SOURCES} ${XF_SRCS_STR})
    set(PUBLIC_HEADER ${PUBLIC_HEADER} ${XF_INCS_STR})
    set(COMPONENT_CCFLAGS ${COMPONENT_CCFLAGS} "-Wno-error" ${XF_CFLAGS_STR})
endif()

install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/products" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/peripheral" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/nfc" "*")
build_component()
