
SET ( TEST_SRC_T
    ${PROJECT_SOURCE_DIR}/test/test.cpp
    ${PROJECT_SOURCE_DIR}/test/app.cpp
    ${PROJECT_SOURCE_DIR}/test/passwd.cpp
) 

SET( TEST_INC_T

) 

SET ( TEST_SRC
    ${TEST_SRC_T}
    ${TEST_INC_T}
    ${LIBSRC_T}
    ${LIBINC_T}
    ${PROJECT_SOURCE_DIR}/src/application.cpp
)
