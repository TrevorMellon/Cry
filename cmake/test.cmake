
include(GoogleTest)

SET ( TEST_APP ${PROJECT_NAME}_test1)

enable_testing()

include ( cmake/test_src.cmake )

add_executable( ${TEST_APP} ${TEST_SRC} ${LIBSRC} )

target_include_directories( ${TEST_APP} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_include_directories( ${TEST_APP} PRIVATE
    ${INCS}
)

target_link_libraries( ${TEST_APP} ${LIBS} )

#target_link_libraries(test1 GTest::gtest GTest::gtest_main)
# Google Test
target_link_libraries( ${TEST_APP}
    ${GTEST_BOTH_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}    
)

target_compile_definitions( ${TEST_APP} PRIVATE
    -DTEST_FILE="${PROJECT_SOURCE_DIR}/test/test.txt"
)

gtest_discover_tests(
    ${TEST_APP}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test
)

