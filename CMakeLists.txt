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

SET(GPG_ALGO  GCRY_CIPHER_SERPENT256)
SET(CRY_EXT  \".cry\")
SET(CRY_DEFAULT_PASSWORD  \"thisISaTEST\")
SET(CRY_DEFAULT_IV \"cRy-b@by-369\")

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

configure_file(templates/config.h.in ${CMAKE_SOURCE_DIR}/include/config.h @ONLY)

add_executable( ${APP} ${SRC})
target_link_libraries(${APP} ${LIBS})
target_include_directories(${APP} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${APP} PUBLIC ${INCS})
