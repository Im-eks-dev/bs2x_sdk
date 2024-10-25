#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================

# 当前组件的名字默认为 CMakeLists.txt 所在目录名
get_filename_component(COMPONENT_NAME_TEMP ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(COMPONENT_NAME ${COMPONENT_NAME_TEMP})
message("COMPONENT_NAME:   ${COMPONENT_NAME}")

file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

set(SOURCES
    ${SOURCES}
    ${SRC_LIST}
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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk("${CMAKE_CURRENT_SOURCE_DIR}" "*")

build_component()
