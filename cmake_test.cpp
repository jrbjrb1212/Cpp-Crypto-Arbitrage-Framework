#include <iostream>
#include <curl/curl.h>


using namespace std;


int main() {
    
    cout << "The test worked!" << endl;

    return 0;
}

/*
cmake_minimum_required(VERSION 3.10)

project(crypto)

# find_package(nlohmann_json REQUIRED)
# include_directories(${nlohmann_json_INCLUDE_DIRS})

# find_package(CURL REQUIRED)
# include_directories(${CURL_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}  cmake_test.cpp)
# target_link_libraries(${PROJECT_NAME}  PRIVATE nlohmann_json CURL)
 */