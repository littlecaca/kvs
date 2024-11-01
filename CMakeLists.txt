cmake_minimum_required(VERSION 3.16)

project(kvstore)

aux_source_directory(. SRCS)

option(DEBUG "debug or not" ON)

if(DEBUG)
    add_compile_options(-g -O0)
else()
    add_compile_definitions(NODEBUG)
endif()

add_executable(kvstore ${SRCS})

set(CMAKE_INSTALL_PREFIX ..)

install(TARGETS kvstore
    DESTINATION bin)