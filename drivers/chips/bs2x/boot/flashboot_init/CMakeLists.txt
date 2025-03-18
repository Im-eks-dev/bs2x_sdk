#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "flashboot_init_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/flashboot_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/upgrade_version_check/upgrade_version_check.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/upgrade_version_check
)

set(PRIVATE_HEADER
)

if(DEFINED CONFIG_DRIVERS_USB_DFU_GADGET OR DEFINED CONFIG_DRIVERS_USB_HID_GADGET)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/usb_download/usb_download.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/usb_download/usb_boot_info.c)
    list(APPEND PRIVATE_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/usb_download)
endif()

set(PRIVATE_DEFINES
)

if(DEFINED CONFIG_DRIVERS_USB_DFU_GADGET OR DEFINED CONFIG_DRIVERS_USB_HID_GADGET)
set(PUBLIC_DEFINES
    FLASHBOOT_LOAD_BY_USB
)
endif()

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

build_component()