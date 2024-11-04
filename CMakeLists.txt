cmake_minimum_required(VERSION 3.16)

project(kvstore)

option(KVS_DEBUG "debug or not" ON)
option(KVS_TEST_PRINT "print test info" OFF)
option(KVS_TEST "enable test or not" ON)
# use the reactor as the network engine defaultly
option(KVS_USE_NTYCO "use the ntyco newwork engine" OFF)

# =======================
#  srcs, headers, or libs
# ========================
set(BASE_SRCS
kvs_malloc.c
kvs_protocol.c
kvs_rbtree.c
logger.c
reactor.c
server.c
kvs_socket.c
ntyco.c)

set(CLIENT_SRCS
kvs_client.c
kvs_socket.c
logger.c
)

add_library(kvstore_base STATIC ${BASE_SRCS})
add_library(kvstore_client STATIC ${CLIENT_SRCS})

target_include_directories(kvstore_base
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(kvstore_client
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(kvstore kvstore.c)
target_link_libraries(kvstore kvstore_base)

# ====================
#  external
# ====================
if(KVS_USE_NTYCO)
    include(ExternalProject)
    target_compile_definitions(kvstore_base PRIVATE NETWORK_SELECT=NETWORK_NTYCO)

    message(STATUS "Use ntyco as the network engine")

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
else()
    message(STATUS "Use reactor as the network engine")
    target_compile_definitions(kvstore_base PUBLIC NETWORK_SELECT=NETWORK_REACTOR)

endif()

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
install(TARGETS kvstore_client
    DESTINATION libs)
