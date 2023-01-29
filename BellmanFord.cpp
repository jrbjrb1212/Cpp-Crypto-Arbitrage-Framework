//TODO: Fix snake case typing of adjaceny list

#include <iostream>
#include "Header_Files/bellmon_ford.hpp"
#include "Header_Files/graph.hpp"

using namespace std;

int main()
{
	Graph g;
	g.addEdge("USD", "LTC", 1.0);
	g.addEdge("USD", "BTC", 1.0);
	g.addEdge("USD", "ETH", 1.0);
	g.addEdge("LTC", "ETH", 1.0);
	g.addEdge("LTC", "BTC", 1.0);
	g.addEdge("ETH", "BTC", 1.1);
	

	// g.addEdge("USD", "LTC", -log(1.1));
	// g.addEdge("LTC", "ETH", -log(1.12));
	// g.addEdge("ETH", "BTC", -log(0.95));
	// g.addEdge("BTC", "USD", -log(1.28));
	// g.printGraph();
	// cout << endl;

	BellmonFord(g, "USD", 0.05);

	// Test case that works with basic implementation of Bellmon Ford
	// g.addEdge("A", "B", 10);
	// g.addEdge("B", "D", 10);
	// g.addEdge("D", "E", 7);
	// g.addEdge("E", "F", 15);
	// g.addEdge("E", "C", 5);
	// g.addEdge("C", "B", 5);

	// g.printGraph();
	// BellmonFord(g, "A");
	return 0;
}
