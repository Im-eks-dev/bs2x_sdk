#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_pinctrl")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/common/hal_pinctrl.c

)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/common

)

if(DEFINED CONFIG_PINCTRL_USING_V150)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/common/hal_pinctrl_v150.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/common/hal_pinctrl_v150_regs_op.c")
elseif(DEFINED CONFIG_PINCTRL_USING_BS2X)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/bs2x/hal_pinctrl_bs2x.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/bs2x")
else()
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${CHIP}/hal_pinctrl_${CHIP}.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/${CHIP}")
endif()

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_HAL_PINCTRL
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
