
set ( SRC_T
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/application.cpp
    
)

set ( INC_T
    ${CMAKE_SOURCE_DIR}/include/application.h    
)

set( LIBSRC_T
    ${CMAKE_SOURCE_DIR}/src/cry.cpp
)

set( LIBINC_T
    ${CMAKE_SOURCE_DIR}/include/cry/cry.h
    ${CMAKE_SOURCE_DIR}/include/cry/common.h
    ${CMAKE_SOURCE_DIR}/include/cry/config.h
    ${CMAKE_SOURCE_DIR}/include/cry/version.h
)

set( LIBSRC ${LIBSRC_T} ${LIBINC_T})

set ( SRC ${SRC_T} ${INC_T} ${LIBSRC}) 

if(GCOV)
    set(GCOV_SRC
        ${CMAKE_SOURCE_DIR}/src/cry.cpp
    )
endif(GCOV)

