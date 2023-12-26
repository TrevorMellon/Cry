
if(GCC_REPORT)

add_custom_target(gccreport
    COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/.gccreport
    #COMMAND ${CMAKE_MAKE_PROGRAM} test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

set(GCCR_LIBS
    -lgcrypt -lboost_filesystem -lboost_system -lboost_program_options -lboost_date_time
)

add_custom_command(TARGET gccreport
    COMMAND echo "=================== GCC REPORT ===================="
    COMMAND g++ -fdiagnostics-show-option ${SRC_T} ${LIBSRC_T} -I${PROJECT_SOURCE_DIR}/include -Wall ${GCCR_LIBS} > ${CMAKE_BINARY_DIR}/gccreport.log 2>&1

    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.gccreport
    )

endif(GCC_REPORT)
