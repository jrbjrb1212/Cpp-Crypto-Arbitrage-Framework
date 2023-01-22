#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>
#include "Header_Files/Graph.h"

// must use VPN with a server outside of the US as binance does not allow US API requests

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}
static void buildSymbolHashMap(std::unordered_map<std::string, std::string>& symbolMap){
    // read from Symbols_Slash_Split.txt and build a dictionary
    std::ifstream input_file("/Users/johnbillos/Desktop/Projects/Crypto-Arbitrage-/Symbol_Data_Files/Symbols_Slash_Split.txt");
    std::string line, lineCopy;
    short int slashPos;

    if (input_file.is_open()) {
        while (std::getline(input_file, line)) {
            line = line.substr(1, line.length()-2);
            lineCopy = line;
            slashPos = line.find("/"); // returns position of the slash
            line.replace(slashPos, 1, "");

            // add to symbolMap
            symbolMap[line] = lineCopy;
        }
        input_file.close();
    }
    else {
        std::cout << "Unable to open file" << std::endl;
    }
}


int main()
{
    CURL *curl;
    CURLcode res;
    std::string response;
    std::unordered_map<std::string, std::string> symbolMap;

    buildSymbolHashMap(symbolMap);

    curl = curl_easy_init();
    if (curl)
    {
        // TODO:
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
        Graph g;
        // parse the list of json strings returned form the binance API call
        nlohmann::json json_data = nlohmann::json::parse(response);
        // print out every price data point from binance
        for (const auto& item : json_data) {
            std::cout << item["symbol"] << std::endl;
            // g.addEdge("A", "B", stod(item["price"]));
        }
        // g.printGraph()
        


        // std::cout << response << std::endl;
        

        curl_easy_cleanup(curl);
    }

    return 0;
}
