#include <iostream>
#include <chrono>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <unistd.h>
#include <cmath>
#include <algorithm>
#include <limits>
#include <tuple>
#include <thread>
#include <mutex>
#include <assert.h>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
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
	bool debugMode;
	bool timeMode;
	int orderBookDepth;
};


/*
*
* Print method to print information about parsed
* user info
*
*/
void printUserInput(UserInput userInput)
{
	cout << "Arb Path Length: " << userInput.pathLen << endl;
	cout << "Start Crypto: " << userInput.startCoin << endl;
	cout << "Min Trade Amount: " << userInput.tradeAmt << endl;
	cout << "Removed exchanges: " << userInput.exchangeRemove << endl;
	cout << "Lower bound profitability: " << userInput.lowerBound << endl;
	cout << "Coin amount requested: " << userInput.coinReq << endl;
	cout << "Volume requested: " << userInput.volReq << endl;
	cout << "Debug mode: " << userInput.debugMode << endl;
	cout << "Time mode: " << userInput.timeMode << endl;
	cout << "Order book depth: " << userInput.orderBookDepth << endl;
}


/*
*
* Parse user settings from a user_settings.txt file
* and fill a UserInput struct
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
	if (values["debugMode"].at(0) == '1')
		userInput.debugMode = true;
	else
		userInput.debugMode = false;
	if (values["timeMode"].at(0) == '1')
		userInput.timeMode = true;
	else
		userInput.timeMode = false;
	userInput.orderBookDepth = stoi(values["orderBookDepth"]);
}


unordered_set<string> removeExchanges(string removeExchanges)
{
	unordered_set<string> exchangeVec;

	// Use a loop to find each delimiter and extract the substring between the delimiters
	size_t startPos = 0;
	size_t delimPos = removeExchanges.find('/');
	while (delimPos != string::npos) {
		exchangeVec.insert(removeExchanges.substr(startPos, delimPos - startPos));
		startPos = delimPos + 1;
		delimPos = removeExchanges.find('/', startPos);
	}

	// Add the final substring to the vector
	exchangeVec.insert(removeExchanges.substr(startPos));

	// Print the extracted elements
	return exchangeVec;
}


/*
*
* main arbitrage running branch
* - Facilitates all features of the framework
* 	while minimizing the log information 
*
*/
void mainArbOnly(UserInput &userInput, Graph &g, unordered_map<string, vector<string>> &symbolMap, 
				unordered_set<string> &seenSymbols, unordered_map<string, double> &feeMap,
				unordered_set<string> &exchangeRemove)
{
	string startCoin = userInput.startCoin;
	int frameworkIterations=0, positiveArbs=0;
	int currIterations=0, currArbsFound=0;
	int optimalAmount;
	double profitability;
	vector<TrackProfit> arbPath;

	// Set the graph
	pullAllTicker(symbolMap, g, true, seenSymbols, exchangeRemove);

	// call resize the symbolMap to only hold viable trading pairs
	symbolHashMapResize(symbolMap, seenSymbols);
	seenSymbols.clear();

	while (true){
		// update the graph
		pullAllTicker(symbolMap, g, false, seenSymbols, exchangeRemove);

		// detect best arbitrage path in the graph
		arbPath = ArbDetect(g, startCoin, 1.0 + userInput.lowerBound, 1.10, userInput.pathLen);
		frameworkIterations++; currIterations++;
		cout << "Iteration " << frameworkIterations << ": ";
		if (arbPath.size() > 0)
		{
			// determine optimal trade amount through orderbook information
			profitability = amountOptControlMain(g, arbPath, userInput.orderBookDepth, feeMap, userInput.tradeAmt);
			positiveArbs++; currArbsFound++;
		}
		
		LogArbInfo(arbPath, feeMap, userInput.startCoin, profitability);

		CheckPointInfo(frameworkIterations, positiveArbs, currIterations, currArbsFound);
		sleep(1);
	}
	
}


/*
*
* debug arbitrage running branch
* - Facilitates all features of the framework
* 	for exactly one successful arbitrage find 
* - Debug mode will print important arbitrage
*  	log information such as graph size, arbitrage
* 	path information, orderbook parsing, and 
* 	ideal size finding
*/
void mainDebugMode(UserInput &userInput, Graph &g, unordered_map<string, vector<string>> &symbolMap, 
				unordered_set<string> &seenSymbols, unordered_map<string, double> &feeMap,
				unordered_set<string> &exchangeRemove)
{
	string startCoin = userInput.startCoin;
	vector<TrackProfit> arbPath;
	printStars();
	cout << "UserInput:" << endl;
	printUserInput(userInput);
	printStars();
	cout << endl;
	// Set the graph
	pullAllTicker(symbolMap, g, true, seenSymbols, exchangeRemove);

	// call resize the symbolMap to only hold viable trading pairs
	symbolHashMapResize(symbolMap, seenSymbols);
	seenSymbols.clear();
		
	printStars();
	cout << "Graph Stats:" << endl;
	cout << "Number of vertices: " << g.getVertexCount() << endl;
	cout << "Number of edges: " << g.getEdgeCount() << endl;
	printStars();
	cout << endl;

	printStars();
	cout << "Performing Arb Finder from " << startCoin << endl;
	printStars();

	int found = 0, need = 1, iterations = 1;
	while (found < need)
	{
		sleep(2);
		// update the graph
		pullAllTicker(symbolMap, g, false, seenSymbols, exchangeRemove);

		// detect best arbitrage path in the graph
		arbPath = ArbDetect(g, startCoin, 1.0 + userInput.lowerBound, 1.10, userInput.pathLen);
		if (arbPath.size() > 0)
		{
			cout << "Found Arb Path in " << iterations << " iterations" << endl;
			printStars();
			cout << endl;
			cout << "Arbitrage Path" << endl;
			printArbInfo(arbPath, feeMap);
			printArbProfitability(arbPath, feeMap);
			printStars();
			cout << endl;
			
			printStars();
			cout << "Amount Optimization Debug Info" << endl;
			amountOptControlDebug(g, arbPath, userInput.orderBookDepth, feeMap, userInput.tradeAmt);
			printStars();
			
			found++;
		}
		else
		{
			cout << "Iteration " << iterations << " found no arbitrage path" << endl;
			iterations += 1;
		}
	}
}


/*
*
* time arbitrage running branch
* - Facilitates all features of the framework
* 	to benchmark the time dominant operations
* 	of the framework
* - These operations include ticker data pulling,
*	arbitrage finding, orderbook pulling, and
*	ideal amount finding
*/
void mainTimeMode(UserInput &userInput, Graph &g, unordered_map<string, vector<string>> &symbolMap, 
				unordered_set<string> &seenSymbols, unordered_map<string, double> &feeMap,
				unordered_set<string> &exchangeRemove)
{
	string startCoin = userInput.startCoin;
	vector<TrackProfit> arbPath;
	double profitability;

	// Set the graph
	pullAllTicker(symbolMap, g, true, seenSymbols, exchangeRemove);

	// call resize the symbolMap to only hold viable trading pairs
	symbolHashMapResize(symbolMap, seenSymbols);
	seenSymbols.clear();

	int iterations = 1, foundPaths = 0;
	while (true)
	{
		vector<double> times(4);
		// check point print every
		if (iterations % 100 == 0)
		{
			cout << iterations << " Iterations Check Point: ";
			cout << foundPaths << " profitable paths found" << endl;
		}

		// update the graph
		auto start = high_resolution_clock::now();
		pullAllTicker(symbolMap, g, false, seenSymbols, exchangeRemove);
		auto end = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(end - start);
		times[0] = (duration.count());

		// detect best arbitrage path in the graph
		start = high_resolution_clock::now();
		arbPath = ArbDetect(g, startCoin, 1.0 + userInput.lowerBound, 1.10, userInput.pathLen);
		end = high_resolution_clock::now();
		duration = duration_cast<milliseconds>(end - start);
		times[1] = (duration.count());
		
		if (arbPath.size() > 0)
		{
			profitability = amountOptControlTime(g, arbPath, userInput.orderBookDepth, feeMap, userInput.tradeAmt, times);
			foundPaths++;
		}
		
		// print iteration information
		cout << "Iter " << iterations << ": ";
		cout << "Ticker_t=" << times[0] << " ms, ";
		cout << "ArbFind_t=" << times[1] << " ms, ";
		cout << "OrdBook_t=" << times[2] << " ms, ";
		cout << "OptAmt_t=" << times[3] << " ms" << endl;
		if (arbPath.size() > 0){
			cout << "\t-";
			LogArbInfo(arbPath, feeMap, userInput.startCoin, profitability);
		}
		iterations++;
		sleep(1);
	}
}


int main(){
	UserInput userInput;
	parseUserSettings(userInput);

	unordered_set<string> seenSymbols;
	Graph g;
	unordered_map<string, vector<string>> symbolMap = buildSymbolHashMap("../../Symbol_Data_Files/Viable_Trading_Pairs.txt");
	unordered_map<string, double> feeMap = buildFeeMap();
	unordered_set<string> exchangeRemove = removeExchanges(userInput.exchangeRemove);

	if (userInput.debugMode)
		mainDebugMode(userInput, g, symbolMap, seenSymbols, feeMap, exchangeRemove);
	else if (userInput.timeMode)
		mainTimeMode(userInput, g, symbolMap, seenSymbols, feeMap, exchangeRemove);
	else
		mainArbOnly(userInput, g, symbolMap, seenSymbols, feeMap, exchangeRemove);

	return 1;
}