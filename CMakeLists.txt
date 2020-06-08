
cmake_minimum_required( VERSION 3.10 )

project ( cry VERSION 0.1 )

SET ( APP cry)

include( cmake/src.cmake )

include( cmake/req.cmake )

add_executable( ${APP} ${SRC})
target_link_libraries(${APP} ${LIBS})
target_include_directories(${APP} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${APP} PUBLIC ${INCS})
