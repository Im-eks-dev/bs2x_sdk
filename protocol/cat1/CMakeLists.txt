set(PROTOCOL_ROOT
    ${CMAKE_CURRENT_SOURCE_DIR})

include("${CMAKE_CURRENT_LIST_DIR}/securec.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/liteos.cmake")

add_subdirectory_if_exist(ims)
add_subdirectory_if_exist(lte)