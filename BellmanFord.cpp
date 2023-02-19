#include <iostream>
#include "Header_Files/graph.hpp"
#include "Header_Files/arbitrage_finder.hpp"
#include "Header_Files/exchange_api_pull.hpp"
#include "Header_Files/combinations.hpp"
#include <chrono>

using namespace std;


int main(){
	unordered_map<string, vector<string>> symbolMap = buildSymbolHashMap();
	unordered_map<string, double> feeMap = buildFeeMap();
	
	Graph g;
	// Set the graph
	
	pullAll(symbolMap, g, true);
	
	// update the graph
	pullAll(symbolMap, g, false);
	cout << "Number of vertices: " << g.getVertexCount() << endl;
	cout << "Number of edges: " << g.getEdgeCount() << endl;
	cout << endl;
	// g.printGraph();

	auto start = high_resolution_clock::now();
	string coin = "USDT";
	cout << "Performing Arb Finder from " << coin << endl;
	for(double i = 0.01; i < 0.25; i+=0.5){
		vector<TrackProfit> arbPath = ArbDetectCombo(g, coin, 1.0, 1.0 + i, 3);
		if (arbPath.size() != 0) {
			for(int i = 0; i < arbPath.size(); i++){
				cout << "From " << arbPath[i].from << " to " << arbPath[i].to;
				cout << " via " << arbPath[i].exchange << " using an " << arbPath[i].bidOrask;
				cout << " at " << WeightConversion(arbPath[i].orderPrice) << " with " << feeMap[arbPath[i].exchange] * 100 << "%" << " fee" << endl;
			}
		}
	}


	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "\n\nTime to Arbdetect: " << duration.count() << " microseconds" << endl;
}

// g.addEdge("USD", "LTC", 1.0, 1.0, 0.002, "A");
	// g.addEdge("USD", "LTC", 1.0, 1.0, 0.002, "B");
	// g.addEdge("USD", "BTC", 1.0, 1.0, 0.002, "A");
	// g.addEdge("USD", "ETH", 1.0, 1.0, 0.002, "A");
	// g.addEdge("LTC", "ETH", 1.1, 1.0, 0.002, "A");
	// g.addEdge("LTC", "BTC", 1.0, 1.0, 0.002, "A");
	// g.addEdge("ETH", "BTC", 1.0, 1.0, 0.002, "A");
