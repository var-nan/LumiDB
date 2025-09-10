cmake_minimum_required(VERSION 3.28)
project(lsm_db LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_STANDARD_REQUIRED ON)

set(SOURCES in_memory_store_wal.cpp logger.cpp)

find_package(ZLIB REQUIRED)

add_executable(lsm_db main.cpp ${SOURCES})
target_link_libraries(lsm_db PRIVATE ZLIB::ZLIB)


# enable testing
enable_testing()
add_subdirectory(tests)