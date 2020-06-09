
cmake_minimum_required( VERSION 3.10 )

project ( cry VERSION 0.1 )

SET ( APP cry)

option(BUILD_ENCRYPT "build encryptor" ON)
option(BUILD_DECRYPT "build decryptor" OFF)

IF(BUILD_ENCRYPT)
    SET ( APP ${APP}e)
    SET ( BUILD_DECRYPT OFF)
    add_definitions(-DCRY_ENCRYPT=1 -DCRY_DECRYPT=0)
else(BUILD_ENCRYPT)
    SET ( APP ${APP}d)
    SET ( BUILD_DECRYPT ON)
    add_definitions(-DCRY_ENCRYPT=0 -DCRY_DECRYPT=1)
endif(BUILD_ENCRYPT)

include( cmake/src.cmake )

include( cmake/req.cmake )

add_executable( ${APP} ${SRC})
target_link_libraries(${APP} ${LIBS})
target_include_directories(${APP} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${APP} PUBLIC ${INCS})
