// compile and run with g++ main.cpp -o main -lcurl && ./main

#include <iostream>
#include <curl/curl.h>

size_t
writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

int main()
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.binance.com/api/v3/ticker/price?symbol=BTCUSDT");

        // Set headers
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "X-MBX-APIKEY: L9eVxtAHmxCf0OZrRVDWXPAc9otIdFN5NoOIMivGgJRwI8UWaeodMNSu7xRieIEU");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        //set callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

        //print the response
        std::cout << readBuffer << std::endl;

        // Cleanup
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}

// find_package(nlohmann_json REQUIRED)
// include_directories(${nlohmann_json_INCLUDE_DIRS})

// add_executable(your_program_name main.cpp)
// target_link_libraries(your_program_name PRIVATE nlohmann_json)