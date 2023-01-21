#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

// must use VPN with a server outside of the US as binance does not allow US API requests

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

int main()
{
    CURL *curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl)
    {
        // can query in parrelel later with https://api.binance.com/api/v3/ticker/price/?symbol=btcusdt
        // where we iterate through a presaved file of all symbols
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.binance.com/api/v3/ticker/price");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            // parse the list of json strings returned form the binance API call
            nlohmann::json json_data = nlohmann::json::parse(response);
            // print out every price data point from binance
            for (const auto& item : json_data) {
                std::cout << item["symbol"] << " " << item["price"] << " " << std::endl;
            }
            // std::cout << response << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    return 0;
}
