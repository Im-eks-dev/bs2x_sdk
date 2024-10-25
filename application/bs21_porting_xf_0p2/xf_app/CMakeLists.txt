#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================

# 当前组件的名字默认为 CMakeLists.txt 所在目录名
get_filename_component(COMPONENT_NAME_TEMP ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(COMPONENT_NAME ${COMPONENT_NAME_TEMP})

if (NOT DEFINED CONFIG_BS21_PORTING_XF_V0_2_X)
    # 空组件，避免修改 ws63_1.10.t7/drivers/boards/ws63/evb/linker/ws63_liteos_app_linker/function.h
    build_component()
    return()
endif() # (DEFINED CONFIG_XF_APP_ENABLE)

if (DEFINED CONFIG_XF_APP_BUILD_FROM_SOURCE)

    set(SOURCES
        # ${CMAKE_CURRENT_SOURCE_DIR}/xxx.c
        ${XF_SOURCES}
    )

    set(PUBLIC_HEADER
        # ${CMAKE_CURRENT_SOURCE_DIR}
        ${XF_HEADERS}
    )

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
        "-Wno-error"
        ${XF_CFLAGS_STR}
    )

else()

    set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib${COMPONENT_NAME}.a)

endif() # CONFIG_XF_APP_BUILD_FROM_SOURCE

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

# 应该不需要
# message("!!!!!!LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND}")
# set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})

install_sdk("${CMAKE_CURRENT_SOURCE_DIR}" "*")

build_component()
