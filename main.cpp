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
    for (int i = 0; i < 10; i++){
    Graph g;
    CURL *curl;
    CURLcode res;
    string response;
    unordered_map<string, vector<string> > symbolMap;

    buildSymbolHashMap(symbolMap);

    curl = curl_easy_init();
    if (curl)
    {
        // TODO:
        // can query in parrelel later with https://api.binance.com/api/v3/ticker/price/?symbol=btcusdt
        // but not sure it would becuase one call will give all data in one JSON string

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.binance.com/api/v3/ticker/price");
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

        // parse every price data point from binance and add it to my graph
        // Can defenstively paralleize parsing the json data
        for (const auto &item : json_data)
        {
            string tradeSymbol = "\"" + string(item["symbol"]) + "\"";
            vector<string> assets = symbolMap[tradeSymbol];
            // check for symbol that is not in my map
            if (assets.size() != 2)
            {
                continue;
            }
            string fromAsset, toAsset;
            fromAsset = assets[0].substr(1);
            toAsset = assets[1].substr(0, assets[1].length() - 1);
            string strPirce = item["price"];
            double price = stod(strPirce);

            g.addEdge(fromAsset, toAsset, price);
        }

        vector<string> sourceCoins {"USDT", "BTC", "ETH", "LTC"};

        // run BellmanFord from USD
        for (string coin : sourceCoins)
        {
            cout << "Performing Bellmon Ford from " << coin << endl;
            vector<string> arbPath = BellmonFord(g, coin, 0.05);
            // validate the arbPath using the graph
            if (arbPath.size() != 0) {
                for(int i = 1; i < arbPath.size(); i++){
                    string from = arbPath[i-1];
                    string to = arbPath[i];
                    for (Edge edge : g.adjacency_list[from]){
                        if (edge.to != to)
                        {
                            continue;
                        }
                        cout << "From " << from << " to " << to << " trade val: " << weightConversion(edge.weight) << endl;
                    }
                }
            }
        }
        // g.printGraph();

        // TODO: remove later; for testing
        // cout << "Number of vertices: " << g.getVertexCount() << endl;
        // cout << "Number of edges: " << g.getEdgeCount() << endl;

        curl_easy_cleanup(curl);
    }
    }

    return 0;
}
