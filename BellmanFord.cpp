#include <iostream>
#include "Header_Files/arbitrage_finder.hpp"
#include "Header_Files/graph.hpp"
// #include "Header_Files/exchange_api_pull.hpp"
#include <chrono>

using namespace std;

int main(){
	Graph g;
	// unordered_map<string, vector<string>> symbolMap = buildSymbolHashMap();
	//    void addEdge(string from, string to, double bidPrice, double askPrice, double fee, string exchange)
	// for fee do 1-fee
	g.addEdge("USD", "LTC", 1.0, 1.0, 0, "A");
	g.addEdge("USD", "LTC", 1.0, 1.0, 0, "B");
	g.addEdge("USD", "BTC", 1.0, 1.0, 0, "A");
	g.addEdge("USD", "ETH", 1.0, 1.0, 0, "A");
	g.addEdge("LTC", "ETH", 2.0, 1.0, 0, "A");
	g.addEdge("LTC", "BTC", 1.0, 1.0, 0, "A");
	g.addEdge("ETH", "BTC", 1.0, 1.0, 0, "A");
	g.printGraph();

	string coin = "USD";
	cout << "Performing Arb Finder from " << coin << endl;
	vector<TrackProfit> arbPath = ArbDetect(g, coin, 0, 0);
	if (arbPath.size() != 0) {
		for(int i = 0; i < arbPath.size(); i++){
			cout << "From " << arbPath[i].from << " to " << arbPath[i].to;
			cout << " via " << arbPath[i].exchange << endl;
		}
	}

}

/*
int main()
{
	Graph g;
	unordered_map<string, vector<string>> symbolMap = buildSymbolHashMap();

	// g.addEdge("USD", "LTC", 1.01, "A");
	// g.addEdge("USD", "LTC", 1.04, "B");
	// g.addEdge("USD", "BTC", 1.28, "A");
	// g.addEdge("USD", "ETH", 1.5, "A");
	// g.addEdge("LTC", "ETH", 1.2, "A");
	// g.addEdge("LTC", "BTC", 1.02, "A");
	// g.addEdge("ETH", "BTC", 1.1, "A");

	auto start = high_resolution_clock::now();
	pullAll(symbolMap, g, true);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "\n\nTime to pull and parse data: " << duration.count() << endl;

	cout << "Number of vertices: " << g.getVertexCount() << endl;
	cout << "Number of edges: " << g.getEdgeCount() << endl;
	cout << endl;
	// g.printGraph();

	
    auto start2 = high_resolution_clock::now();
	vector<string> sourceCoins {"USDT", "ETH"};

	for (string coin : sourceCoins)
	{
		cout << "Performing Arb Finder from " << coin << endl;
		vector<TrackProfit> arbPath = ArbDetect(g, coin, 0.95, 0.8);
		// validate the arbPath using the graph
		if (arbPath.size() != 0) {
			for(int i = 0; i < arbPath.size(); i++){
				cout << "From " << arbPath[i].from << " to " << arbPath[i].to << " trade val: " << WeightConversion(arbPath[i].weight);
				cout << " via " << arbPath[i].exchange << endl;
			}
		}
	}
	auto stop2 = high_resolution_clock::now();
	auto duration2 = duration_cast<microseconds>(stop2 - start2);
	cout << "\n\nTime to ARbdetect: " << duration2.count() << endl;
	return 0;
}
*/