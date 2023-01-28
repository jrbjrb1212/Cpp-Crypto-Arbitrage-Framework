/*
Psudeo Code for Bellmon Ford
BEGIN
// Source distance set to 0
d(v[1]) ← 0

// Initalize members of distance array
FOR j = 2,..,n DO
    d(v[j]) ← ∞

// Main Loop of iterations for Bellman
FOR i = 1,..,(|V|-1) DO
    FOR ALL (u,v) in E DO
        d(v) ← min(d(v), d(u)+l(u,v))

// V-th iteration check for negative cycles
FOR ALL (u,v) in E DO
    IF d(v) > d(u) + l(u,v) DO
        Message: "Negative Circle"
END
*/
//TODO: Fix snake case typing of adjaceny list

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;

struct Edge
{
	string to;
	double weight;
};

class Graph
{
private:
	int m_graphEdges = 0;

public:
	Graph() {}
	unordered_map<string, vector<Edge> > adjacency_list;

	// TODO: Change graph to add edges in both directions as the crypto exchange rate is the inverse
	void addEdge(string from, string to, double weight)
	{
		adjacency_list[from].push_back({to, weight});
		// make sure each vertex is a key in the adjaceny_list
		if (adjacency_list.find(to) == adjacency_list.end())
		{
			adjacency_list[to];
		}
		m_graphEdges++;

		//TODO: Change back later
		// adjacency_list[to].push_back({from, 1/weight});
		// m_graphEdges+=2;
	}

	void printGraph()
	{
		cout << "\nGraph Print in form of Adjaceny List: " << endl;
		for (auto it = adjacency_list.begin(); it != adjacency_list.end(); it++)
		{
			string vertex = it->first;
			vector<Edge> edges = it->second;
			cout << vertex << ": ";
			for (auto edge : edges)
			{
				cout << "(" << edge.to << ", " << edge.weight << ") ";
			}
			cout << endl;
		}
	}
	int getVertexCount()
	{
		return adjacency_list.size();
	}
	int getEdgeCount()
	{
		return m_graphEdges;
	}
};

void printBellmonFord(unordered_map<string, double> dists, string source)
{
	cout << "\nPrint out of distance array of Bellmon Ford Algo: " << endl;
	for (auto access : dists)
	{
		string vertex = access.first;
		double shortestPath = access.second;
		cout << "Vertex: " << vertex << " shortest path from " << source << ": " << shortestPath << endl;
	}
}

void PrintCycle(vector<string> cycleVec)
{
	cout << "Arbitrage Opportunity: " << endl;
	for (int i = cycleVec.size() - 1; i > 0; i--)
	{
		cout << cycleVec[i] << " --> ";
	}

	cout << cycleVec[cycleVec.size() - 1] << endl;
}

vector<string> CorrectArbCycle(vector<string> cycleVec, int sourcePos)
{
	vector<string> vecPopped;
	// Clean up cyclevec so that our source, most likely 'USD', is first element
	for (int i = 0; i < sourcePos; i++)
	{
		// TODO: replace later; useless O(n) operation
		vecPopped.push_back(cycleVec[i]);
		cycleVec.erase(cycleVec.begin());
	}
	cycleVec.push_back(cycleVec[0]);
	PrintCycle(cycleVec);
	return cycleVec;
}

void ProfitCalc(vector<string> cycleVec, unordered_map<string, vector<Edge> > adjacency_list)
{
	// TODO: You are not updating the from symbol
	cout << endl;
	int sourceVal = 1;
	string fromSymbol = cycleVec[cycleVec.size() - 1];
	for (int i = cycleVec.size() - 2; i >= 0; i--)
	{
		string toSymbol = cycleVec[i];
		cout << "Working with " << fromSymbol << " and " << toSymbol << endl;
		cycleVec[i] string of a symbol for (Edge edge : adjacency_list[fromSymbol])
		{
			if (edge.to == toSymbol)
			{
				cout << "Trade from: " << fromSymbol << " to: " << toSymbol << endl;
				cout << "-ln(symbolTradeVal) aka edge.weight: " << edge.weight << endl;
				cout << "-1*(edge.weight/exp(1) aka e^{...}: " << (-1 * (edge.weight / log(exp(1)))) << endl;
				cout << "Decrypted oringinal trade val: " << exp(-1 * (edge.weight / log(exp(1)))) << endl;
				sourceVal *= exp(-1 * (edge.weight / log(exp(1))));
			}
		}
	}
	// Reverse iterate through cycleVec
	// Get the cycleVec.size() - 1 trades from my natural log to decmial formula

	cout << "Profitability %: " << sourceVal << endl;
}

unordered_map<string, double> BellmonFord(Graph g, string source)
{
	const float INF = numeric_limits<float>::max();
	const int V = g.getVertexCount();
	unordered_map<string, vector<Edge> > adjacency_list = g.adjacency_list;
	unordered_map<string, string> prevVert;

	// Initialize distance array
	unordered_map<string, double> dists;
	for (auto access : adjacency_list)
	{
		dists[access.first] = INF;
	}
	dists[source] = 1;

	// Do V-1 iterations of Bellmon Ford
	for (int i = 0; i < V - 1; i++)
	{
		vector<string> vertexStack{};
		unordered_set<string> vertexSeenSet{};
		vertexStack.push_back(source);

		while (!vertexStack.empty())
		{
			string currVertex = vertexStack.back();
			vertexStack.pop_back();
			vertexSeenSet.insert(currVertex);

			// traverse all the edges of currVertex
			for (Edge edge : adjacency_list[currVertex])
			{
				if (vertexSeenSet.count(edge.to) == 0)
				{
					vertexStack.push_back(edge.to);
				}

				double newCost = dists[currVertex] + edge.weight;
				if (newCost < dists[edge.to])
				{
					dists[edge.to] = newCost;
					prevVert[edge.to] = currVertex;
				}
			}
		}
		// Print bellmon Ford distance array after each iteration
		// printBellmonFord(dists, source);
	}
	for (auto unload : prevVert)
	{
		cout << "From: " << unload.second << " Second: " << unload.first << endl;
	}

	// V-th relax loop to check for negative cycle
	vector<string> vertexStack{};
	unordered_set<string> vertexSeenSet{};
	vertexStack.push_back(source);

	while (!vertexStack.empty())
	{
		string currVertex = vertexStack.back();
		vertexStack.pop_back();
		vertexSeenSet.insert(currVertex);
		for (Edge edge : adjacency_list[currVertex])
		{
			if (vertexSeenSet.count(edge.to) == 0)
			{
				vertexStack.push_back(edge.to);
			}

			double newCost = dists[currVertex] + edge.weight;
			if (newCost < dists[edge.to])
			{
				cout << "\nNegative Cycle Detected!!!!" << endl;
				printBellmonFord(dists, source);
				// Go through prevVert to print out the path needed to execute the trade
				vector<string> cycleVec = {edge.to, currVertex};
				unordered_set<string> vertexSeenSet = {edge.to, currVertex};
				while (vertexSeenSet.count(prevVert[currVertex]) == 0)
				{
					cycleVec.push_back(prevVert[currVertex]);
					currVertex = prevVert[currVertex];
				}
				cycleVec.push_back(prevVert[currVertex]);
				cycleVec = CorrectArbCycle(cycleVec, 1);
				ProfitCalc(cycleVec, adjacency_list);
				return dists;
			}
		}
	}

	printBellmonFord(dists, source);
	return dists;
}

int main()
{
	Graph g;
	g.addEdge("USD", "LTC", -log(1.1));
	g.addEdge("LTC", "ETH", -log(1.12));
	g.addEdge("ETH", "BTC", -log(0.95));
	g.addEdge("BTC", "USD", -log(1.28));
	g.printGraph();

	BellmonFord(g, "USD");

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
