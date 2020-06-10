
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(gcrypt REQUIRED)

if(WIN32)
add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK=1)
endif(WIN32)

find_package(Boost REQUIRED filesystem system program_options date_time)

SET ( LIBS
    ${GCRYPT_LIBRARIES}
    ${Boost_LIBRARIES}
)

SET (INCS
    ${GCRYPT_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
)