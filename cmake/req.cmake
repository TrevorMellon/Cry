
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(gcrypt REQUIRED)

if(WIN32)
#add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK=1)
add_definitions(-DBOOST_ALL_NO_LIB)
endif(WIN32)

if(BOOST_STATIC)
set ( Boost_USE_STATIC_LIBS ON ) 
endif(BOOST_STATIC)

find_package(Boost REQUIRED 
    filesystem 
    system 
    program_options 
    date_time
    random
    )

SET ( LIBS
    ${GCRYPT_LIBRARIES}
    #${Boost_LIBRARIES}
    Boost::filesystem
    Boost::system
    Boost::program_options
    Boost::date_time
    Boost::random
)

SET (INCS
    ${GCRYPT_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
)

if(BUILD_TESTS)
    #find_package(GTest REQUIRED)
    find_package(Threads REQUIRED)
endif(BUILD_TESTS)
