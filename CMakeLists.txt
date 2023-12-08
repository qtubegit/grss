cmake_minimum_required(VERSION 3.10)
project(minrss CXX)
set (CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

set(SOURCE_FILES src)
set(INCLUDE_FILES include)

include_directories(${GTK3_INCLUDE_DIRS} ${INCLUDE_FILES})

link_directories(${GTK3_LIBRARY_DIRS})

message(STATUS ${GTK3_LIBRARY_DIRS})

add_executable(minrss ${SOURCE_FILES}/main.cc
    ${SOURCE_FILES}/window_ui.cc
    ${SOURCE_FILES}/events.cc
    ${SOURCE_FILES}/feed.cc
    ${SOURCE_FILES}/helpers.cc)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(minrss PUBLIC -g -fsanitize=address -fsanitize=undefined)
    target_link_options(minrss PUBLIC -fsanitize=address -fsanitize=undefined)
endif()

find_library(MRSS_LIBRARY
    NAMES mrss)

find_library(CURL_LIBRARY
    NAMES curl)

find_library(NXML_LIBRARY
    NAMES nxml)

message(STATUS ${MRSS_LIBRARY})
message(STATUS ${NXML_LIBRARY})
message(STATUS ${GTK3_LIBRARIES})
message(STATUS ${CURL_LIBRARY})

target_link_libraries(minrss
    ${GTK3_LIBRARIES}
    ${MRSS_LIBRARY}
    ${NXML_LIBRARY}
    ${CURL_LIBRARY})