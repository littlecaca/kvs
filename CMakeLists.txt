cmake_minimum_required(VERSION 3.16)

project(kvstore)

option(KVS_DEBUG "debug or not" ON)
option(KVS_TEST_PRINT "print test info" ON)
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
kvs_client.c
ntyco.c)

add_library(kvstore_base STATIC ${SRCS})

target_include_directories(kvstore_base
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(kvstore kvstore.c)
target_link_libraries(kvstore kvstore_base)

# ====================
#  external
# ====================
include(ExternalProject)

message(STATUS "Add dependent project ntyco")

set(ntyco_path ${CMAKE_SOURCE_DIR}/vendors/NtyCo)
ExternalProject_Add(
    NtyCo
    SOURCE_DIR ${ntyco_path}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS ${ntyco_path}/libntyco.a
    CLEAN_COMMAND make clean
)

add_dependencies(kvstore_base NtyCo)

add_library(ntyco STATIC IMPORTED)
set_target_properties(ntyco PROPERTIES IMPORTED_LOCATION
    ${ntyco_path}/libntyco.a)
target_link_libraries(kvstore_base PRIVATE ntyco dl pthread)
target_include_directories(kvstore_base PRIVATE ${ntyco_path}/core)

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
    target_compile_definitions(kvstore_base PRIVATE KVS_TEST_PRINT)
endif()


# ==================
#  install
# ==================
set(CMAKE_INSTALL_PREFIX ..)

install(TARGETS kvstore
    DESTINATION bin)
install(TARGETS kvstore_base
    DESTINATION libs)
