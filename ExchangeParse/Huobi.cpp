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

// must use VPN with a server outside of the US as Huobi does not allow US API requests

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
            vector<string> symbols_vec = {line.substr(0, line.find("/")), line.substr(line.find("/") + 1)};
            string symbol_key = line.substr(0, line.find("/")) + line.substr(line.find("/") + 1);
            symbolMap[symbol_key] = symbols_vec;
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
        const char* exchangeURL = "https://api.huobi.pro/market/tickers";
        curl_easy_setopt(curl, CURLOPT_URL, exchangeURL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        // cout << response << endl;

        // Check for errors
        if (res != CURLE_OK)
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }

        nlohmann::json json_data = nlohmann::json::parse(response);
        for (auto& item : json_data["data"]) {
            // Huobi returns symbol name in all lower case where my hashmap is all uppercase
            string tradeSymbol = item["symbol"];
            string tradeSymbolUpper;
            for (char c : tradeSymbol){
                tradeSymbolUpper += toupper(c);
            }
            tradeSymbolUpper = "\"" + tradeSymbolUpper + "\"";
            vector<string> assets = symbolMap[tradeSymbolUpper];
            if (assets.size() != 2)
            {
                continue;
            }
            string fromAsset, toAsset;
            fromAsset = assets[0];
            toAsset = assets[1];

            // Huobi returns "ask" value as a double instead of a string
            double price = item["ask"];
            g.addEdge(fromAsset, toAsset, price);
        }

        vector<string> sourceCoins {"USDT", "BTC", "ETH", "LTC"};

        g.printGraph();

        // TODO: remove later; for testing
        // cout << "Number of vertices: " << g.getVertexCount() << endl;
        // cout << "Number of edges: " << g.getEdgeCount() << endl;

        curl_easy_cleanup(curl);
    }
    return 0;
}
