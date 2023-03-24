#include <iostream>
#include <unistd.h>
#include "Header_Files/graph.hpp"
#include "Header_Files/arbitrage_finder.hpp"
#include "Header_Files/exchange_api_pull.hpp"
#include "Header_Files/combinations.hpp"
#include "Header_Files/amount_optimization.hpp"
#include <chrono>

using namespace std;


int main(){
	unordered_map<string, vector<string>> symbolMap = buildSymbolHashMap();
	unordered_map<string, double> feeMap = buildFeeMap();
	
	Graph g;
	int nDepth = 20;

	// Set the graph
	pullAllTicker(symbolMap, g, true);
	
	cout << "Number of vertices: " << g.getVertexCount() << endl;
	cout << "Number of edges: " << g.getEdgeCount() << endl;
	cout << endl;

	string coin = "USDT";
	vector<TrackProfit> arbPath;
	cout << "Performing Arb Finder from " << coin << endl;
	for(double i = 0.5; i < 25; i+=0.6){
		auto start = high_resolution_clock::now();
		
		// update the graph
		pullAllTicker(symbolMap, g, false);
		
		arbPath = ArbDetect(g, coin, 1.1, 1.0 + i, 4);
		printArbInfo(arbPath, feeMap);
		break;
				
		cout << endl;
		amountOptControl(g, arbPath, nDepth, feeMap);
		
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		cout << "Time to Pull Ticker, ArbDetect and Pull Orderbook: " << duration.count() << " microseconds" << endl;
		cout << endl;
	}
}