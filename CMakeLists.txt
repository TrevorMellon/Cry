cmake_minimum_required( VERSION 3.10 )

project ( cry VERSION 0.1 )

SET ( APP cry)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message(STATUS "Enabling Debug Mode")
    add_definitions(-DDBG=1)
    set(CRY_BUFFER_MULT 16)
else()
    set(CRY_BUFFER_MULT 1024)
endif()

option(BUILD_ENCRYPT "build encryptor" OFF)
option(BUILD_DECRYPT "build decryptor" OFF)
option(BUILD_ENCDEC "build encryptor and decryptor" OFF)

if(NOT CRY_DEFAULT_PASSWORD)
    message(STATUS "Using generic default password")
    SET(CRY_DEFAULT_PASSWORD "DEFAULTpassword123CHANGEME")
endif()

if(NOT CRY_DEFAULT_IV)
    message(STATUS "Using default IV")
    SET(CRY_DEFAULT_IV "cRy-b@by-369")
endif()

SET(GPG_ALGO  GCRY_CIPHER_SERPENT256)
SET(CRY_EXT  \".cry\")

if(BUILD_ENCDEC)
    SET (BUILD_ENCRYPT OFF)
    SET (BUILD_DECRYPT OFF)
    SET (APP ${APP})
    add_definitions(-DCRY_ENCRYPT=1 -DCRY_DECRYPT=1)
elseif(BUILD_ENCRYPT)
    SET ( APP ${APP}e)
    add_definitions(-DCRY_ENCRYPT=1 -DCRY_DECRYPT=0)
elseif(BUILD_DECRYPT)
    SET ( APP ${APP}d)
    add_definitions(-DCRY_ENCRYPT=0 -DCRY_DECRYPT=1)
endif(BUILD_ENCDEC)

include( cmake/src.cmake )

include( cmake/req.cmake )

include(cmake/git.cmake)
include(cmake/versioner.cmake)
include(cmake/version.cmake)

configure_file(templates/config.h.in ${CMAKE_SOURCE_DIR}/include/cry/config.h @ONLY)

add_executable( ${APP} ${SRC})
target_link_libraries(${APP} ${LIBS})
target_include_directories(${APP} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${APP} PUBLIC ${INCS})

SET(SO ${APP}.0.02)

add_library( ${SO} SHARED ${LIBSRC})
target_link_libraries( ${SO} ${LIBS})
target_include_directories( ${SO} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories( ${SO} PUBLIC ${INCS})

SET ( L ${APP}-static)

add_library( ${L} STATIC ${LIBSRC})
target_include_directories( ${L} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories( ${L} PUBLIC ${INCS})

include(cmake/install.cmake)


