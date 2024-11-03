cmake_minimum_required(VERSION 3.16)

project(kvstore)

option(KVS_DEBUG "debug or not" ON)
option(KVS_TEST_PRINT "print test info" OFF)
option(KVS_TEST "enable test or not" ON)

# =======================
#  srcs, headers, or libs
# ========================
set(SRCS
kvs_malloc.c
kvs_protocol.c
kvs_rbtree.c
logger.c
reactor.c
server.c
kvs_socket.c
kvs_client.c)

add_library(kvstore_base STATIC ${SRCS})

target_include_directories(kvstore_base
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(kvstore kvstore.c)
target_link_libraries(kvstore kvstore_base)

# ====================
#  debug
# ====================
if(KVS_DEBUG)
add_compile_options(-g -O0)
else()
add_compile_definitions(NDEBUG)
endif()

# ==================
#  test
# ==================
if(KVS_TEST)
    message(STATUS "open TEST")
    enable_testing()
    add_subdirectory(test)
    add_subdirectory(integrated_test)
endif()

if(KVS_TEST_PRINT)
    message(STATUS "set KVS_TEST")
    message(STATUS ${TEST_PRINT})
    target_compile_definitions(kvstore_base PRIVATE KVS_TEST)
endif()



# ==================
#  install
# ==================

set(CMAKE_INSTALL_PREFIX ..)

install(TARGETS kvstore
    DESTINATION bin)
install(TARGETS kvstore_base
    DESTINATION libs)