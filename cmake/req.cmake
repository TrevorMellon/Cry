
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(gcrypt REQUIRED)

find_package(Boost REQUIRED filesystem system program_options)

SET ( LIBS
    ${GCRYPT_LIBRARIES}
    ${Boost_LIBRARIES}
)

SET (INCS
    ${GCRYPT_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
)