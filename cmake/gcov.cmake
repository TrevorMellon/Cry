# Original Source: https://jhbell.com/using-cmake-and-gcov
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)
#add_executable(gcov_test ${LIBSRC} ${PROJECT_SOURCE_DIR}/test/test.cpp )

set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

##target_include_directories(gcov_test PUBLIC include)
#target_compile_definitions(gcov_test PUBLIC -DTEST_FILE="${PROJECT_SOURCE_DIR}/test/test.txt")

##target_link_libraries(gcov_test PUBLIC ${LIBS})
#target_link_libraries(gcov_test PUBLIC -lgcov --coverage)
#target_include_directories(gcov_test PUBLIC ${INCS})

set(OBJECT_DIR ${CMAKE_BINARY_DIR}/CMakeFiles)


# Create the gcov target. Run coverage tests with 'make gcov'
add_custom_target(gcov_trgt
    COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/.gcov/coverage
    COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/.gcov/obj
    COMMAND ${CMAKE_MAKE_PROGRAM} test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
add_custom_command(TARGET gcov_trgt
    COMMAND echo "=================== GCOV ===================="
    COMMAND mkdir -p ${OBJECT_DIR}/gcov
    #COMMAND cp -R ${OBJECT_DIR}/${APP}.dir/src/* ${OBJECT_DIR}/gcov
    #COMMAND cp -R ${OBJECT_DIR}/${TEST_APP}.dir/src/* ${OBJECT_DIR}/gcov
    #COMMAND cp -R ${OBJECT_DIR}/${TEST_APP}.dir/test/* ${OBJECT_DIR}/gcov
    #COMMAND gcov -b -o ${OBJECT_DIR}/gcov ${TEST_SRC}    
    COMMAND echo "-- Coverage files have been output to ${PROJECT_SOURCE_DIR}/.gcov/coverage"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.gcov/coverage
    )
add_dependencies(gcov_trgt ${TEST_APP})
# Make sure to clean up the coverage folder
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES coverage)

# Create the gcov-clean target. This cleans the build as well as generated 
# .gcda and .gcno files.
add_custom_target(scrub
COMMAND ${CMAKE_MAKE_PROGRAM} clean
COMMAND rm -f ${OBJECT_DIR}/*.gcno
COMMAND rm -f ${OBJECT_DIR}/*.gcda
WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
