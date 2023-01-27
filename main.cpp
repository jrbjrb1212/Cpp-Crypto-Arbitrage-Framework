#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>

using namespace std;

// must use VPN with a server outside of the US as binance does not allow US API requests

struct Edge {
    string to;
    double weight;
};

class Graph {
    private:
        int m_graphEdges = 0;

    public:
        Graph () {}
        unordered_map<string, vector<Edge>> adjacency_list;

        // TODO: Change graph to add edges in both directions as the crypto exchange rate is the inverse
        void addEdge(string from, string to, double weight) {
            adjacency_list[from].push_back({to, weight});
            adjacency_list[to].push_back({from, 1/weight});
            m_graphEdges+=2;
        }

        void printGraph() {
            for (auto it = adjacency_list.begin(); it != adjacency_list.end(); it++) {
                string vertex = it->first;
                vector<Edge> edges = it->second;
                cout << vertex << ": ";
                for (const auto& edge : edges) {
                    cout << "(" << edge.to << ", " << edge.weight << ") ";
                }
                cout << endl;
            }
        }
        int getVertexCount() {
            return adjacency_list.size();
        }
        int getEdgeCount() {
            return m_graphEdges;
        }
};


static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}
static void buildSymbolHashMap(unordered_map<string, vector<string>>& symbolMap){
    // read from Symbols_Slash_Split.txt and build a dictionary
    ifstream input_file("/Users/johnbillos/Desktop/Projects/Crypto-Arbitrage-/Symbol_Data_Files/Symbols_Slash_Split.txt");
    string line, lineCopy;
    short int slashPos;

    if (input_file.is_open()) {
        while (getline(input_file, line)) {
            vector<string> symbols_vec = {line.substr(0, line.find("/")), line.substr(line.find("/") + 1)};
            string symbol_key = line.substr(0, line.find("/")) + line.substr(line.find("/") + 1);
            symbolMap[symbol_key] = symbols_vec;
        }
        input_file.close();
    }
    else {
        cout << "Unable to open file" << endl;
    }
}


int main()
{
    CURL *curl;
    CURLcode res;
    string response;
    unordered_map<string, vector<string>> symbolMap;

    buildSymbolHashMap(symbolMap);
    

    curl = curl_easy_init();
    if (curl)
    {
        // TODO:
        // can query in parrelel later with https://api.binance.com/api/v3/ticker/price/?symbol=btcusdt
        // but not sure it would becuase one call will give all data in one JSON string

        // where we iterate through a presaved file of all symbols
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.binance.com/api/v3/ticker/price");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
        Graph g;
        // parse the list of json strings returned form the binance API call
        // TODO: Test to make sure something correct is returned from API call aka on the right VPN
        nlohmann::json json_data = nlohmann::json::parse(response);

        // parse every price data point from binance and add it to my graph
        for (const auto& item : json_data) {
            string tradeSymbol = "\"" + string(item["symbol"]) + "\"";
            vector<string> assets = symbolMap[tradeSymbol];
            // check for symbol that is not in my map
            if (assets.size() != 2){
                continue;
            }
            string fromAsset, toAsset;
            fromAsset = assets[0].substr(1);
            toAsset = assets[1].substr(0, assets[1].length()-1);
            string strPirce = item["price"];
            double price = stod(strPirce);


            g.addEdge(fromAsset, toAsset, price);
        }

        // run BellmanFord from USD
        // BellmanFord(g, "USDT");
        g.printGraph();

        // TODO: remove later; for testing
        cout << "Number of vertices: " << g.getVertexCount() << endl;
        cout << "Number of edges: " << g.getEdgeCount() << endl;
                

        curl_easy_cleanup(curl);
    }

    return 0;
}
