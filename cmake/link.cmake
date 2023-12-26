
target_link_libraries(${APP} ${LIBS})
target_include_directories(${APP} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${APP} PUBLIC ${INCS})


if(BUILD_SHARED_LIB)
    target_link_libraries( ${SO} ${LIBS})
    target_include_directories( ${SO} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_include_directories( ${SO} PUBLIC ${INCS})
endif(BUILD_SHARED_LIB)


if(BUILD_STATIC_LIB)
    target_include_directories( ${L} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_include_directories( ${L} PUBLIC ${INCS})
endif(BUILD_STATIC_LIB)

