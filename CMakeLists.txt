cmake_minimum_required(VERSION 3.10)
project(minrss C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

set(SOURCE_FILES src)
set(INCLUDE_FILES include)

message(STATUS ${SYSTEM_INCLUDE_PATH})

include_directories(${GTK3_INCLUDE_DIRS} ${INCLUDE_FILES})

add_executable(minrss ${SOURCE_FILES}/main.c ${SOURCE_FILES}/events.c)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(minrss PUBLIC -g -fsanitize=address -fsanitize=undefined)
    target_link_options(minrss PUBLIC -fsanitize=address -fsanitize=undefined)
endif()

find_library(MRSS_LIBRARY
    NAMES mrss)

message(STATUS ${MRSS_LIBRARY})

find_library(NXML_LIBRARY
    NAMES nxml)

message(STATUS ${NXML_LIBRARY})

target_link_libraries(minrss ${GTK3_LIBRARIES} ${MRSS_LIBRARY} ${NXML_LIBRARY})