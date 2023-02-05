#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include "Header_Files/graph.hpp"
#include "Header_Files/bellmon_ford.hpp"

using namespace std;

// must use VPN with a server outside of the US as binance does not allow US API requests

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}
static void buildSymbolHashMap(unordered_map<string, vector<string> > &symbolMap)
{
    // read from Symbol_Space_Split.txt and build a dictionary
    ifstream input_file("../../Symbol_Data_Files/Symbol_Space_Split.txt");
    string line, lineCopy;
    short int slashPos;

    if (input_file.is_open())
    {
        while (getline(input_file, line))
        {
            short strLen = line.length();
            short forSlashPos = line.find("/");
            string firstCoin = line.substr(1, forSlashPos-1);
            string secondCoin = line.substr(forSlashPos + 1, strLen - forSlashPos - 2);

            vector<string> symbolsVec = {firstCoin, secondCoin};
            string symbol_key = firstCoin + secondCoin;
            symbolMap[symbol_key] = symbolsVec;
        }
        input_file.close();
    }
    else
    {
        cout << "Unable to open file" << endl;
    }
}

int main()
{
    Graph g;
    CURL *curl;
    CURLcode res;
    string response;
    unordered_map<string, vector<string> > symbolMap;

    buildSymbolHashMap(symbolMap);

    curl = curl_easy_init();
    if (curl)
    {
        const char* exchangeURL = "https://api.binance.com/api/v3/ticker/price";
        curl_easy_setopt(curl, CURLOPT_URL, exchangeURL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }

        // TODO: Test to make sure something correct is returned from API call aka on the right VPN
        nlohmann::json json_data = nlohmann::json::parse(response);

        for (const auto &item : json_data)
        {
            string tradeSymbol = string(item["symbol"]);
            vector<string> assets = symbolMap[tradeSymbol];
            if (assets.size() != 2)
            {
                continue;
            }
            string fromAsset, toAsset;
            fromAsset = assets[0];
            toAsset = assets[1];
            string strPrice = item["price"];
            double price = stod(strPrice);

            g.addEdge(fromAsset, toAsset, price);
        }

        vector<string> sourceCoins {"USDT", "BTC", "ETH", "LTC"};

        g.printGraph();

        // TODO: remove later; for testing
        cout << "Number of vertices: " << g.getVertexCount() << endl;
        cout << "Number of edges: " << g.getEdgeCount() << endl;

        curl_easy_cleanup(curl);
    }
    return 0;
}
