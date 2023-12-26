
add_custom_command(TARGET vera_trgt
    COMMAND echo "=================== VERA++ ===================="
    COMMAND mkdir -p ${OBJECT_DIR}/gcov
    #COMMAND cp -R ${OBJECT_DIR}/${APP}.dir/src/* ${OBJECT_DIR}/gcov
    #COMMAND cp -R ${OBJECT_DIR}/${TEST_APP}.dir/src/* ${OBJECT_DIR}/gcov
    #COMMAND cp -R ${OBJECT_DIR}/${TEST_APP}.dir/test/* ${OBJECT_DIR}/gcov
    #COMMAND gcov -b -o ${OBJECT_DIR}/gcov ${TEST_SRC}   
    COMMAND vera 
    COMMAND echo "-- Coverage files have been output to ${PROJECT_SOURCE_DIR}/.gcov/coverage"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.gcov/coverage
    )
    