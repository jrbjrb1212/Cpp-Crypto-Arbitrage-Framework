#include <iostream>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <unistd.h>
#include "Header_Files/graph.hpp"
#include "Header_Files/arbitrage_finder.hpp"
#include "Header_Files/exchange_api_pull.hpp"
#include "Header_Files/combinations.hpp"
#include "Header_Files/amount_optimization.hpp"

using namespace std;

/*
*
* Struct for holding all custom user settings
* that are generated from shell_driver.sh
*
*/
struct UserInput
{
	int pathLen;
	string startCoin;
	double tradeAmt;
	string exchangeRemove;
	double lowerBound;
	int coinReq;
	double volReq;
};


/*
*
* 
*
*/
void parseUserSettings(UserInput &userInput)
{
	ifstream file("../../user_settings.txt");
    unordered_map<string, string> values;
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string key, value;
        getline(iss, key, '=');
        getline(iss, value);
        values[key] = value;
    }
    userInput.pathLen = stoi(values["pathLen"]);
    userInput.startCoin = values["startCoin"];
    userInput.tradeAmt = stod(values["tradeAmt"]);
    userInput.exchangeRemove = values["exchangeRemove"];
    userInput.lowerBound = stod(values["lowerBound"]);
    userInput.coinReq = stoi(values["coinReq"]);
    userInput.volReq = stod(values["volReq"]);
}



int main(){
	bool test = true;
	UserInput userInput;
	if (test)
	{
		parseUserSettings(userInput);
		cout << userInput.pathLen << endl;
	}
	else{

	unordered_map<string, vector<string>> symbolMap = buildSymbolHashMap("../../Symbol_Data_Files/Viable_Trading_Pairs.txt");
	unordered_set<string> seenSymbols;
	unordered_map<string, double> feeMap = buildFeeMap();
	
	Graph g;
	int nDepth = 100;

	// Set the graph
	pullAllTicker(symbolMap, g, true, seenSymbols);
	// call the buildSymbolHashMap resize method
	symbolHashMapResize(symbolMap, seenSymbols);
	seenSymbols.clear();
	
	cout << "Number of vertices: " << g.getVertexCount() << endl;
	cout << "Number of edges: " << g.getEdgeCount() << endl;
	cout << endl;

	string coin = "USDT";
	vector<TrackProfit> arbPath;
	cout << "Performing Arb Finder from " << coin << endl;

	int found = 0, need = 1;
	while (found < need)
	{
		// update the graph
		auto start = high_resolution_clock::now();
		pullAllTicker(symbolMap, g, false, seenSymbols);
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		cout << "Time to Pull Update Ticker: " << duration.count() << " microseconds" << endl;


		// detect best arbitrage path in the graph
		arbPath = ArbDetect(g, coin, 1.0, 1.25, 3);
		if (arbPath.size() > 0)
		{
			found++;

			// (FOR TESTING) print arbitrage information in full detail
			printArbInfo(arbPath, feeMap);
			
			// determine optimal trade amount through orderbook information
			amountOptControl(g, arbPath, nDepth, feeMap);
		}
	}
	}
}