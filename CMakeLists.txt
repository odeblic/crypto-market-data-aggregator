cmake_minimum_required(VERSION 3.20)
project(crypto-market-data-aggregator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(OpenSSL REQUIRED)

set(FETCHCONTENT_QUIET OFF)

include(FetchContent)
include(boost)
include(catch2)
include(grpc)
include(json)

enable_testing()

add_subdirectory(proto)
add_subdirectory(src)
add_subdirectory(test)
