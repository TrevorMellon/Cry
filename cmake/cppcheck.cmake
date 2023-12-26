


if (CMAKE_CXX_CPPCHECK)
    message(STATUS "Enabling extra rules for cppcheck")
    set(CPPCHECK_XML_OUTPUT "${CMAKE_BINARY_DIR}/cppcheck.xml")
    set(CPPCHECK_CHECKS_ARGS 
        "--enable=all;--inconclusive;--force;--inline-suppr;-v;--suppress=missingInclude"   
    )

    #list(
    #    APPEND CMAKE_CHECK_ARGS
    #    "--enable=all"
    #    "--inconclusive"
    #    "--force"
    #    "--inline-suppr"
    #    "-v"
    #
    #)

    find_package(CppCheck)
    #list(
    #    APPEND CMAKE_CXX_CPPCHECK 
    #        "--enable=all"
    #        "--inconclusive"
    #        "--force" 
    #        "--inline-suppr"
    #        "--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt"
    #        "--xml"
    #        "-v"
    #        "2>${CMAKE_BINARY_DIR}/cppcheck.xml"
    #)
    #set (CPPCHECK_COMMAND "${CMAKE_CXX_CPPCHECK} --enable=all --inclusive --force --xml --output-file=${CMAKE_BINARY_DIR}/test.xml")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/analysis/cppcheck)

    add_custom_target(cppcheck-analysis
        COMMAND ${CPPCHECK_COMMAND})
    message (STATUS "${CPPCHECK_COMMAND}")
endif(CMAKE_CXX_CPPCHECK)
