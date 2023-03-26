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


int main(){
	unordered_map<string, vector<string>> symbolMap = buildSymbolHashMap("../../Symbol_Data_Files/Viable_Trading_Pairs.txt");
	unordered_set<string> seenSymbols;
	unordered_map<string, double> feeMap = buildFeeMap();
	
	Graph g;
	int nDepth = 20;

	// Set the graph
	pullAllTicker(symbolMap, g, true, seenSymbols);
	symbolHashMapResize(symbolMap, seenSymbols);
	seenSymbols.clear();
	// call the buildSymbolHashMap resize method
	
	cout << "Number of vertices: " << g.getVertexCount() << endl;
	cout << "Number of edges: " << g.getEdgeCount() << endl;
	cout << endl;

	string coin = "USDT";
	vector<TrackProfit> arbPath;
	cout << "Performing Arb Finder from " << coin << endl;
	double start = 0.1, end = 0.25, increment;
	increment = end;
	for(double i = start; i < end; i+=increment){
		
		// update the graph
		pullAllTicker(symbolMap, g, false, seenSymbols);
		
		auto start = high_resolution_clock::now();
		arbPath = ArbDetect(g, coin, 1.0, 1.0 + i, 2);
		arbPath = ArbDetect(g, coin, 1.0, 1.0 + i, 3);
		arbPath = ArbDetect(g, coin, 1.0, 1.0 + i, 4);
		arbPath = ArbDetect(g, coin, 1.0, 1.0 + i, 5);
		printArbEdgeInfo(g, arbPath);
		// printArbInfo(arbPath, feeMap);
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		cout << "Time to Pull Ticker, ArbDetect and Pull Orderbook: " << duration.count() << " microseconds" << endl;
		cout << endl;
				
		// amountOptControl(g, arbPath, nDepth, feeMap);
		
	}
}