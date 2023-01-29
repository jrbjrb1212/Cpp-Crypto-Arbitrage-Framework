//TODO: Fix snake case typing of adjaceny list

#include <iostream>
#include "Header_Files/bellmon_ford.hpp"
#include "Header_Files/graph.hpp"

using namespace std;

int main()
{
	Graph g;
	g.addEdge("USD", "LTC", 1.01);
	g.addEdge("USD", "BTC", 1.28);
	g.addEdge("USD", "ETH", 1.5);
	g.addEdge("LTC", "ETH", 1.2);
	g.addEdge("LTC", "BTC", 1.02);
	g.addEdge("ETH", "BTC", 1.1);
	

	// g.addEdge("USD", "LTC", -log(1.1));
	// g.addEdge("LTC", "ETH", -log(1.12));
	// g.addEdge("ETH", "BTC", -log(0.95));
	// g.addEdge("BTC", "USD", -log(1.28));
	// g.printGraph();
	// cout << endl;
	unordered_map<string, vector<Edge> > adjacency_list = g.adjacency_list;

	vector<string> arbPath = BellmonFord(g, "USD", 0.05);
	// validate the arbPath using the graph
	for(int i = 1; i < arbPath.size(); i++){
		string from = arbPath[i-1];
		string to = arbPath[i];
		for (Edge edge : adjacency_list[from]){
			if (edge.to != to)
			{
				continue;
			}
			cout << "From " << from << " to " << to << " trade val: " << weightConversion(edge.weight) << endl;
		}
	}

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
