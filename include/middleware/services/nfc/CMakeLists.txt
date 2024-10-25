#===============================================================================
# @brief    nfc cmake file
# Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "nfc_header")

set(SOURCES
)

if(DEFINED CONFIG_ENABLE_NFC_CONTROLLER_SAMPLE)
    add_subdirectory_if_exist(controller)
endif()

if(DEFINED CONFIG_ENABLE_NFC_TAG_SAMPLE)
	set(PUBLIC_HEADER "${PUBLIC_HEADER}"
		${CMAKE_CURRENT_SOURCE_DIR}/tag
	)
endif()

set(PUBLIC_DEFINES
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
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

if(DEFINED CONFIG_BUILD_USING_STATIC_LIBRARIES_SDK)
    if(EXISTS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
        set(SOURCES )
        set(LIBS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    endif()
endif()

build_component()