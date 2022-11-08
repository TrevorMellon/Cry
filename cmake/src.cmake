
SET ( SRC_T
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/application.cpp
    
)

SET ( INC_T
    ${CMAKE_SOURCE_DIR}/include/application.h    
)

SET( LIBSRC_T
    ${CMAKE_SOURCE_DIR}/src/cry.cpp
)

SET( LIBINC_T
    ${CMAKE_SOURCE_DIR}/include/cry/cry.h
    ${CMAKE_SOURCE_DIR}/include/cry/common.h
    ${CMAKE_SOURCE_DIR}/include/cry/config.h
    ${CMAKE_SOURCE_DIR}/include/cry/version.h
)

SET( LIBSRC ${LIBSRC_T} ${LIBINC_T})

SET ( SRC ${SRC_T} ${INC_T} ${LIBSRC}) 

if(GCOV)
    SET(GCOV_SRC
        ${CMAKE_SOURCE_DIR}/src/cry.cpp
    )
endif(GCOV)

