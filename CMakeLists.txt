cmake_minimum_required( VERSION 3.10 )

project ( cry VERSION 0.1 )

SET ( APP cry)

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

SET(SO ${APP}.0.2)
add_library( ${SO} SHARED ${LIBSRC})

SET ( L ${APP}-static)
add_library( ${L} STATIC ${LIBSRC})

include( cmake/link.cmake )

include(cmake/install.cmake)


