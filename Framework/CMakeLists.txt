cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

project(Crypto)

find_package(nlohmann_json REQUIRED)
include_directories(${nlohmann_json_INCLUDE_DIRS})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}  main.cpp)
target_link_libraries(${PROJECT_NAME}  PRIVATE nlohmann_json curl)