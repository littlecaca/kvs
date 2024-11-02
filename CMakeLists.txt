cmake_minimum_required(VERSION 3.16)

project(kvstore)

option(DEBUG "debug or not" ON)
option(TEST "enable test or not" ON)

# ====================
#  debug
# ====================
if(DEBUG)
add_compile_options(-g -O0)
else()
add_compile_definitions(NDEBUG)
endif()

# =======================
#  srcs, headers, or libs
# ========================
aux_source_directory(. SRCS)

add_library(kvstore STATIC ${SRCS})

target_include_directories(kvstore 
                        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# ==================
#  test
# ==================
if(TEST)
enable_testing()
add_subdirectory(test)
endif()

# ==================
#  install
# ==================

set(CMAKE_INSTALL_PREFIX ..)

install(TARGETS kvstore
    DESTINATION libs)