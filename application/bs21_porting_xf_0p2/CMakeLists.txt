#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

if (DEFINED CONFIG_BS21_PORTING_XF_V0_2_X)

    if(NOT DEFINED ENV{XF_PROJECT_PATH})
        message(FATAL_ERROR "XF_PROJECT_PATH environment variable not detected, please run in xf command line!")
    endif()

    message("XF_PROJECT_PATH:   $ENV{XF_PROJECT_PATH}")
    include("$ENV{XF_PROJECT_PATH}/build/build_environ.cmake")
    
    message("XF_SRCS_STR:       ${XF_SRCS_STR}")
    message("XF_INCS_STR:       ${XF_INCS_STR}")
    message("XF_CFLAGS_STR:     ${XF_CFLAGS_STR}")

    set(XF_SOURCES ${XF_SRCS_STR})

    include(platform_def/platform_def.cmake)
    set(XF_HEADERS 
        ${XF_INCS_STR}
        ${XF_PLATF_DEF}
    )
else()
    set(XF_SOURCES)
    set(XF_HEADERS)
endif() # (DEFINED CONFIG_BS21_PORTING_XF_V0_2_X)

add_subdirectory_if_exist(xf_app)
add_subdirectory_if_exist(tasks_xf_entry)
add_subdirectory_if_exist(port_xf)
