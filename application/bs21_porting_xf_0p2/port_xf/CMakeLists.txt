#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================

# 以文件名排序添加子目录，方便比对
set(PUBLIC_HEADER
    ${PUBLIC_HEADER}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory_if_exist(port_xf_ringbuf)
add_subdirectory_if_exist(port_utils)
add_subdirectory_if_exist(port_xf_ble)
# add_subdirectory_if_exist(port_xf_event)
add_subdirectory_if_exist(port_xf_gpio)
add_subdirectory_if_exist(port_xf_i2c)
add_subdirectory_if_exist(port_xf_pwm)
add_subdirectory_if_exist(port_xf_sle)
add_subdirectory_if_exist(port_xf_spi)
# add_subdirectory_if_exist(port_xf_systime)
# add_subdirectory_if_exist(port_xf_test) # 仅供参考
# add_subdirectory_if_exist(port_xf_timer)
add_subdirectory_if_exist(port_xf_uart)
# add_subdirectory_if_exist(port_xf_wifi)

set(COMPONENT_NAME "port_xf_event")
build_component()

set(COMPONENT_NAME "port_xf_systime")
build_component()

set(COMPONENT_NAME "port_xf_test")
build_component()

set(COMPONENT_NAME "port_xf_timer")
build_component()

set(COMPONENT_NAME "port_xf_wifi")
build_component()
