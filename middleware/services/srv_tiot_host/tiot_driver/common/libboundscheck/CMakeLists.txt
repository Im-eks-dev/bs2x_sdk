#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(TIOT_HEADER_LIST "${TIOT_HEADER_LIST}" "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE INTERNAL "tiot header list" FORCE)
if(DEFINED CONFIG_LIBBOUNDSCHECK_IMPLEMENTED)
else()
set(libboundscheck_src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memcpy_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memmove_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memset_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/securecutil.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/secureprintoutput_a.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/snprintf_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sprintf_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/strcat_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/strcpy_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/strncat_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/strncpy_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vsnprintf_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vsprintf_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/strtok_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sscanf_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vsscanf_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/secureinput_a.c
)
set(TIOT_SRC_LIST "${TIOT_SRC_LIST}" "${libboundscheck_src}" CACHE INTERNAL "tiot src list" FORCE)
endif()
