#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_OF_GAME_MOUSE_WITH_DONGLE)
    add_subdirectory_if_exist(game_mouse_with_dongle)
endif()

if(DEFINED CONFIG_SAMPLE_OF_GAME_MOUSE_WITH_TEST)
    add_subdirectory_if_exist(game_mouse_with_test)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)