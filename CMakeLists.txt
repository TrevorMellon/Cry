
cmake_minimum_required( VERSION 3.10 )

project ( cry VERSION 0.1 )

SET ( APP cry)

SET ( SRC
    src/main.cpp
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(gcrypt)

add_executable( ${APP} ${SRC})
target_link_libraries(${APP} ${GCRYPT_LIBRARIES})
target_include_directories(${APP} PUBLIC ${GCRYPT_INCLUDE_DIR})

