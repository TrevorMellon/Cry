
include(GoogleTest)

include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP True
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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
    #${GTEST_BOTH_LIBRARIES}
    GTest::gtest
    GTest::gtest_main
    ${CMAKE_THREAD_LIBS_INIT}    
)

target_compile_definitions( ${TEST_APP} PRIVATE
    -DTEST_FILE="${PROJECT_SOURCE_DIR}/test/test.txt"
)

gtest_discover_tests(
    ${TEST_APP}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test
)

