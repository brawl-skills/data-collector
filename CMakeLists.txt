cmake_minimum_required(VERSION 3.19)
project(data_collector)

set(CMAKE_CXX_STANDARD 20)

find_package (CURL)

pkg_check_modules(JSONCPP jsoncpp)
#include_directories(${JSON_INC_PATH})

find_package (Boost)
include_directories(${Boost_INCLUDE_DIR})
find_library(PQXX_LIB pqxx REQUIRED)
find_library(PQ_LIB pq REQUIRED)


add_executable(data_collector client.cpp structures.h)
include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(data_collector ${CURL_LIBRARIES} ${PQXX_LIB} ${PQ_LIB} ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES})

