cmake_minimum_required(VERSION 3.16)

aux_source_directory(. SRCS)

add_executable(kvstore ${SRCS})

set(CMAKE_INSTALL_PREFIX ..)

install(TARGETS kvstore
    DESTINATION bin)