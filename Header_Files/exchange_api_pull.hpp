#pragma once

#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;
using namespace std::chrono;


/*
*
* Determines datasize for buffer needed for data received from API responses
*
*/
static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}


/*
*
* Develops a HashMap of predetermined viable trading pairs to add to
* an active graph of trading pair exchanges
* Map Strucuture: Coin1Coin2 -> [Coin1, Coin2] 
*
*/
unordered_map<string, vector<string>> buildSymbolHashMap()
{
    // read from Symbol_Space_Split.txt and build a dictionary
    ifstream input_file("../../Symbol_Data_Files/Symbol_Space_Split.txt");
    string line, lineCopy;
    short int slashPos;
    unordered_map<string, vector<string>> symbolMap;


    if (input_file.is_open())
    {
        while (getline(input_file, line))
        {
            short strLen = line.length(), forSlashPos = line.find("/");
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
    return symbolMap;
}


/*
*
* Develops a HashMap of exchanges and their spot trading fees
* Map Strucuture: Binance -> [0.002] 
* TODO: implement API request to get most up to date trading fees
* not public endpoint so authoritzation via API key is required
*
*/
unordered_map<string, double> buildFeeMap(vector<string> exchanges){
    unordered_map<string, double> feeMap;
    feeMap['binance'] = 0.002;
    feeMap['bitget'] = 0.002;
    feeMap['bitmart'] = 0.005;
    feeMap['gateio'] = 0.003;
    feeMap['huobi'] = 0.002;
    feeMap['kucoin'] = 0.002;
    return feeMap
}



/*
*
* Pull spot ticker data from Binance Exchange via API
* and add data to Graph
*
*/
void pullBinance(unordered_map<string, vector<string> > &symbolMap, Graph &g, mutex &symbolMapMutex)
{
    CURL *curl;
    CURLcode res;
    string response;
    string exchange= "binance";

    curl = curl_easy_init();
    if (curl)
    {
        cout << "Start pull from " << exchange << "...";
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

        nlohmann::json json_data = nlohmann::json::parse(response);

        // Lock the mutex
        symbolMapMutex.lock();
        for (const auto &item : json_data)
        {
            string tradeSymbol = string(item["symbol"]);
            vector<string> assets = symbolMap[tradeSymbol];

            // Trading pair is not viable by user settings
            if (assets.size() != 2)
                continue;

            string fromAsset = assets[0];
            string toAsset = assets[1];
            string strPrice = item["price"];
            double price = stod(strPrice);
            g.addEdge(fromAsset, toAsset, price, exchange);
        }
        // Unlock the mutex
        symbolMapMutex.unlock();
        
        curl_easy_cleanup(curl);
        cout << "Finished pull from " << exchange << "\n" << endl;
    }
}


/*
*
* Pull spot ticker data from bitget Exchange via API
* and add data to Graph
*
*/
void pullBitget(unordered_map<string, vector<string> > &symbolMap, Graph &g, mutex &symbolMapMutex)
{
    CURL *curl;
    CURLcode res;
    string response;
    string exchange = "bitget";

    curl = curl_easy_init();
    if (curl)
    {
        cout << "Start pull from " << exchange << "...";
        const char* exchangeURL = "https://api.bitget.com/api/spot/v1/market/tickers";
        curl_easy_setopt(curl, CURLOPT_URL, exchangeURL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);


        // Check for errors
        if (res != CURLE_OK)
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }

        
        nlohmann::json json_data = nlohmann::json::parse(response);

        // Lock the mutex
        symbolMapMutex.lock();
        for (auto& item : json_data["data"]) {
            // symbol comes in uppercase with the coins seperated by a hyphen
            string tradeSymbol = item["symbol"];
            vector<string> assets = symbolMap[tradeSymbol];
            if (assets.size() != 2)
            {
                continue;
            }
            string fromAsset = assets[0], toAsset = assets[1], stringPrice;
            stringPrice = item["buyOne"];
            double price = stod(stringPrice);
            g.addEdge(fromAsset, toAsset, price, exchange);
        }
        // Unlock the mutex
        symbolMapMutex.unlock();

        cout << "Finished pull from " << exchange << "\n" << endl;
        curl_easy_cleanup(curl);
    }
}


/*
*
* Pull spot ticker data from BitMart Exchange via API
* and add data to Graph
*
*/
void pullBitMart(unordered_map<string, vector<string> > &symbolMap, Graph &g, mutex &symbolMapMutex)
{
    CURL *curl;
    CURLcode res;
    string response;
    string exchange = "bitmart";

    curl = curl_easy_init();
    if (curl)
    {
        cout << "Start pull from " << exchange << "...";
        const char* exchangeURL = "https://api-cloud.bitmart.com/spot/v2/ticker";
        curl_easy_setopt(curl, CURLOPT_URL, exchangeURL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }

        
        nlohmann::json json_data = nlohmann::json::parse(response);
        
        // Lock the mutex
        symbolMapMutex.lock();
        for (auto& item : json_data["data"]["tickers"]) {
            // symbol comes in uppercase with the coins seperated by a hyphen
            string tradeSymbol = item["symbol"];
            int delimeterPos = tradeSymbol.find("_");
            tradeSymbol = tradeSymbol.substr(0, delimeterPos) + tradeSymbol.substr(delimeterPos+1);
            vector<string> assets = symbolMap[tradeSymbol];
            if (assets.size() != 2)
            {
                continue;
            }
            string fromAsset, toAsset, stringPrice;
            fromAsset = assets[0];
            toAsset = assets[1];
            stringPrice = item["best_ask"];
            double price = stod(stringPrice);
            g.addEdge(fromAsset, toAsset, price, exchange);
        }
        // Unlock the mutex
        symbolMapMutex.unlock();

        cout << "Finished pull from " << exchange << "\n" <<endl;
        curl_easy_cleanup(curl);
    }
}

/*
*
* Pull spot ticker data from Gate.io Exchange via API
* and add data to Graph
*
*/
void pullGateio(unordered_map<string, vector<string> > &symbolMap, Graph &g, mutex &symbolMapMutex)
{
    CURL *curl;
    CURLcode res;
    string response;
    string exchange = "gateio";

    curl = curl_easy_init();
    if (curl)
    {
        cout << "Start pull from " << exchange << "...";
        const char* exchangeURL = "https://api.gateio.ws/api/v4/spot/tickers";
        curl_easy_setopt(curl, CURLOPT_URL, exchangeURL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }

        
        nlohmann::json json_data = nlohmann::json::parse(response);

        // Lock the mutex
        symbolMapMutex.lock();
        for (auto& item : json_data) {
            // symbol comes in uppercase with the coins seperated by a hyphen
            string tradeSymbol = item["currency_pair"];
            int delimeterPos = tradeSymbol.find("_");
            tradeSymbol = tradeSymbol.substr(0, delimeterPos) + tradeSymbol.substr(delimeterPos+1);
            vector<string> assets = symbolMap[tradeSymbol];
            if (assets.size() != 2)
            {
                continue;
            }
            string fromAsset, toAsset, stringPrice;
            fromAsset = assets[0];
            toAsset = assets[1];
            stringPrice = item["lowest_ask"];
            double price = stod(stringPrice);
            g.addEdge(fromAsset, toAsset, price, exchange);
        }
        // Unlock the mutex
        symbolMapMutex.unlock();

        cout << "Finished pull from " << exchange << "\n" << endl;
        curl_easy_cleanup(curl);
    }
}


/*
*
* Pull spot ticker data from Huobi Exchange via API
* and add data to Graph
*
*/
void pullHuobi(unordered_map<string, vector<string> > &symbolMap, Graph &g, mutex &symbolMapMutex)
{
    CURL *curl;
    CURLcode res;
    string response;
    string exchange = "huboi";

    curl = curl_easy_init();
    if (curl)
    {
        cout << "Start pull from " << exchange << "...";
        const char* exchangeURL = "https://api.huobi.pro/market/tickers";
        curl_easy_setopt(curl, CURLOPT_URL, exchangeURL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }

        nlohmann::json json_data = nlohmann::json::parse(response);
        
        // Lock the mutex
        symbolMapMutex.lock();
        for (auto& item : json_data["data"]) {
            // Huobi returns symbol name in all lower case where my hashmap is all uppercase
            string tradeSymbol = item["symbol"];
            string tradeSymbolUpper;
            for (char c : tradeSymbol){
                tradeSymbolUpper += toupper(c);
            }
            tradeSymbolUpper = tradeSymbolUpper;
            vector<string> assets = symbolMap[tradeSymbolUpper];
            if (assets.size() != 2)
                continue;

            string fromAsset = assets[0], toAsset = assets[1];

            // Huobi returns "ask" value as a double instead of a string
            double price = item["ask"];
            g.addEdge(fromAsset, toAsset, price, exchange);
        }
        // Unlock the mutex
        symbolMapMutex.unlock();

        cout << "Finished pull from " << exchange << "\n" << endl;
        curl_easy_cleanup(curl);
    }
}

/*
*
* Pull spot ticker data from Kucoin Exchange via API
* and add data to Graph
*
*/
void pullKucoin(unordered_map<string, vector<string> > &symbolMap, Graph &g, mutex &symbolMapMutex)
{
    CURL *curl;
    CURLcode res;
    string response;
    string exchange = "kucoin";

    curl = curl_easy_init();
    if (curl)
    {
        cout << "Start pull from " << exchange << "...";
        const char* exchangeURL = "https://api.kucoin.com/api/v1/market/allTickers";
        curl_easy_setopt(curl, CURLOPT_URL, exchangeURL );
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

        // Lock the mutex
        symbolMapMutex.lock();
        for (auto& item : json_data["data"]["ticker"]) {
            // symbol comes in uppercase with the coins seperated by a hyphen
            string tradeSymbol = item["symbol"];
            int delimeterPos = tradeSymbol.find("-");
            tradeSymbol = tradeSymbol.substr(0, delimeterPos) + tradeSymbol.substr(delimeterPos+1);
            vector<string> assets = symbolMap[tradeSymbol];
            if (assets.size() != 2)
                continue;
            
            string fromAsset, toAsset, stringPrice;
            fromAsset = assets[0];
            toAsset = assets[1];
            stringPrice = item["buy"];
            double price = stod(stringPrice);
            g.addEdge(fromAsset, toAsset, price, exchange);
        }
        // Unlock the mutex
        symbolMapMutex.unlock();

        cout << "Finished pull from " << exchange << "\n" << endl;
        curl_easy_cleanup(curl);
    }
}


/*
*
* Sequentially pull spot ticker data for all coins on all exchanges currently
* supported in this framework via API
*
*/
void pullAll(unordered_map<string, vector<string> > &symbolMap, Graph &g, bool parallelOpt) {
    // if (!parallelOpt) {
    //     pullBinance(symbolMap, g);
    //     pullBitget(symbolMap, g);
    //     pullBitMart(symbolMap, g);
    //     pullGateio(symbolMap, g);
    //     pullKucoin(symbolMap, g);
    //     pullHuobi(symbolMap, g);
    // } else {
        mutex symbolMapMutex;
        vector<thread> threads;
        threads.push_back(thread(pullBinance, ref(symbolMap), ref(g), ref(symbolMapMutex)));
        threads.push_back(thread(pullBitget, ref(symbolMap), ref(g), ref(symbolMapMutex)));
        threads.push_back(thread(pullBitMart, ref(symbolMap), ref(g), ref(symbolMapMutex)));
        threads.push_back(thread(pullGateio, ref(symbolMap), ref(g), ref(symbolMapMutex)));
        threads.push_back(thread(pullKucoin, ref(symbolMap), ref(g), ref(symbolMapMutex)));
        threads.push_back(thread(pullHuobi, ref(symbolMap), ref(g), ref(symbolMapMutex)));
        for (auto &thread : threads) {
            thread.join();
        }
    // }
}