#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

# 空组件，避免修改 build/config/target_config/ws63/config.py

set(COMPONENT_NAME "xf_app")
build_component()

set(COMPONENT_NAME "tasks_xf_entry")
build_component()

set(COMPONENT_NAME "port_utils")
build_component()

set(COMPONENT_NAME "port_xf_ble")
build_component()

set(COMPONENT_NAME "port_xf_event")
build_component()

set(COMPONENT_NAME "port_xf_gpio")
build_component()

set(COMPONENT_NAME "port_xf_i2c")
build_component()

set(COMPONENT_NAME "port_xf_pwm")
build_component()

set(COMPONENT_NAME "port_xf_ringbuf")
build_component()

set(COMPONENT_NAME "port_xf_sle")
build_component()

set(COMPONENT_NAME "port_xf_spi")
build_component()

set(COMPONENT_NAME "port_xf_systime")
build_component()

set(COMPONENT_NAME "port_xf_uart")
build_component()

set(COMPONENT_NAME "port_xf_test")
build_component()

set(COMPONENT_NAME "port_xf_timer")
build_component()

set(COMPONENT_NAME "port_xf_wifi")
build_component()
