#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>

// must use VPN with a server outside of the US as binance does not allow US API requests

struct Edge {
    std::string to;
    double weight;
};

class Graph {
    public:
        Graph () {}
        std::unordered_map<std::string, std::vector<Edge>> adjacency_list;

        // TODO: Change graph to add edges in both directions as the crypto exchange rate is the inverse
        void addEdge(std::string from, std::string to, double weight) {
            adjacency_list[from].push_back({to, weight});
            adjacency_list[to].push_back({from, 1/weight});
        }

        void printGraph() {
            for (auto it = adjacency_list.begin(); it != adjacency_list.end(); it++) {
                std::string vertex = it->first;
                std::vector<Edge> edges = it->second;
                std::cout << vertex << ": ";
                for (const auto& edge : edges) {
                    std::cout << "(" << edge.to << ", " << edge.weight << ") ";
                }
                std::cout << std::endl;
            }
        }
};


static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}
static void buildSymbolHashMap(std::unordered_map<std::string, std::vector<std::string>>& symbolMap){
    // read from Symbols_Slash_Split.txt and build a dictionary
    std::ifstream input_file("/Users/johnbillos/Desktop/Projects/Crypto-Arbitrage-/Symbol_Data_Files/Symbols_Slash_Split.txt");
    std::string line, lineCopy;
    short int slashPos;

    if (input_file.is_open()) {
        while (std::getline(input_file, line)) {
            std::vector<std::string> symbols_vec = {line.substr(0, line.find("/")), line.substr(line.find("/") + 1)};
            std::string symbol_key = line.substr(0, line.find("/")) + line.substr(line.find("/") + 1);
            symbolMap[symbol_key] = symbols_vec;
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
    std::unordered_map<std::string, std::vector<std::string>> symbolMap;

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
            std::string tradeSymbol = "\"" + std::string(item["symbol"]) + "\"";
            std::vector<std::string> assets = symbolMap[tradeSymbol];
            if (assets.size() != 2){
                continue;
            }
            std::string fromAsset, toAsset;
            fromAsset = assets[0].substr(1);
            toAsset = assets[1].substr(0, assets[1].length()-1);
            std::string strPirce = item["price"];
            double price = std::stod(strPirce);


            g.addEdge(fromAsset, toAsset, price);
        }
        // g.printGraph();
        std::cout << g.adjacency_list.size() << std::endl;
        


        // std::cout << response << std::endl;
        

        curl_easy_cleanup(curl);
    }

    return 0;
}
