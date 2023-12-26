cmake_minimum_required( VERSION 3.10 )

project ( cry VERSION 0.1 )

SET ( APP cry)

SET ( CMAKE_CXX_STANDARD 14 )

include ( cmake/options.cmake )

include( cmake/src.cmake )

include( cmake/req.cmake )

include(cmake/git.cmake)
#include(cmake/versioner.cmake)
include(cmake/version.cmake)

if(BUILD_TESTS)
    include ( cmake/test.cmake )
endif(BUILD_TESTS)

if(GCOV)
    include (cmake/gcov.cmake )
endif(GCOV)

include(cmake/conf.cmake)

add_executable( ${APP} ${SRC})

if(BUILD_SHARED_LIB)
    SET(SO ${APP}.0.2)
    add_library( ${SO} SHARED ${LIBSRC})
endif(BUILD_SHARED_LIB)

if(BUILD_STATIC_LIB)
    SET ( L ${APP}-static)
    add_library( ${L} STATIC ${LIBSRC})
endif(BUILD_STATIC_LIB)

include( cmake/link.cmake )

include(cmake/install.cmake)

include (cmake/gccreport.cmake)
include (cmake/cppcheck.cmake)


