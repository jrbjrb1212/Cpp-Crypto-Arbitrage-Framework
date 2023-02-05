
#include <iostream>
#include "Header_Files/arbitrage_finder.hpp"
#include "Header_Files/graph.hpp"
#include "Header_Files/exchange_api_pull.hpp"

using namespace std;

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

	// pullBinance(symbolMap, g);
	// pullBitMart(symbolMap, g);
	pullAll(symbolMap, g);

	cout << "Number of vertices: " << g.getVertexCount() << endl;
	cout << "Number of edges: " << g.getEdgeCount() << endl;
	// g.printGraph();

	

	vector<string> sourceCoins {"USDT"};

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
	return 0;
}
