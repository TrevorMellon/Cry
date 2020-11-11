
SET ( SRC_T
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/application.cpp
    
)

SET( LIBSRC_T
    ${CMAKE_SOURCE_DIR}/src/cry.cpp
)

SET ( INC_T
    ${CMAKE_SOURCE_DIR}/include/application.h    
)

SET( LIBINC_T
    ${CMAKE_SOURCE_DIR}/include/cry/cry.h
    ${CMAKE_SOURCE_DIR}/include/cry/common.h
    ${CMAKE_SOURCE_DIR}/include/cry/config.h
    ${CMAKE_SOURCE_DIR}/include/cry/version.h
)

SET( LIBSRC ${LIBSRC_T} ${LIBINC_T})

SET ( SRC ${SRC_T} ${INC_T} ${LIBSRC}) 

