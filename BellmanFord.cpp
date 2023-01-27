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

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;


struct Edge;
class Graph;
void printBellmonFord(unordered_map<string, int> dists);
unordered_map<string, int> BellmonFord(Graph g, string source);



struct Edge {
    string to;
    double weight;
};

class Graph {
    private:
        int m_graphEdges = 0;

    public:
        Graph () {}
        unordered_map<string, vector<Edge>> adjacency_list;

        // TODO: Change graph to add edges in both directions as the crypto exchange rate is the inverse
        void addEdge(string from, string to, double weight) {
            adjacency_list[from].push_back({to, weight});
			// make sure each vertex is a key in the adjaceny_list
			if (adjacency_list.find(to) == adjacency_list.end()){
				adjacency_list[to];
			}
			m_graphEdges++;

			//TODO: Change back later
            // adjacency_list[to].push_back({from, 1/weight});
            // m_graphEdges+=2;
        }

        void printGraph() {
			cout << "\nGraph Print in form of Adjaceny List: " << endl;
            for (auto it = adjacency_list.begin(); it != adjacency_list.end(); it++) {
                string vertex = it->first;
                vector<Edge> edges = it->second;
                cout << vertex << ": ";
                for (const auto& edge : edges) {
                    cout << "(" << edge.to << ", " << edge.weight << ") ";
                }
                cout << endl;
            }
        }
        int getVertexCount() {
            return adjacency_list.size();
        }
        int getEdgeCount() {
            return m_graphEdges;
        }
};

void printBellmonFord(unordered_map<string, int> dists, string source){
	cout << "\nPrint out of distance array of Bellmon Ford Algo: " << endl;
	for (auto access : dists) {
		string vertex = access.first;
		int shortestPath = access.second;
		cout << "Vertex: " << vertex << " shortest path from " << source <<  ": "  << shortestPath << endl;
	}
}

unordered_map<string, int> BellmonFord(Graph g, string source){
	const int INF = INT_MAX;
	const int V = g.getVertexCount();
    unordered_map<string, vector<Edge>> adjacency_list = g.adjacency_list;

	// Initialize distance array
	unordered_map<string, int> dists;
	for (auto access : adjacency_list){
		dists[access.first] = INF;
	}
	dists[source] = 0;

	// Do V-1 iterations of Bellmon Ford
	for (int i = 0; i < V-1; i++){
		vector<string> vertexStack {};
		unordered_set<string> vertexSeenSet {}; 
		vertexStack.push_back(source);

		while(!vertexStack.empty()){
			string currVertex = vertexStack.back();
			vertexStack.pop_back();
			vertexSeenSet.insert(currVertex);

			// traverse all the edges of currVertex
			for (Edge edge : adjacency_list[currVertex]){
				if (vertexSeenSet.count(edge.to) == 0) {
					vertexStack.push_back(edge.to);
				}

				int newCost = dists[currVertex] + edge.weight;
				if (newCost < dists[edge.to]){
					dists[edge.to] = newCost;
				}
			}
		}
	}
	
	// V-th relax loop to check for negative cycle
	vector<string> vertexStack {};
	unordered_set<string> vertexSeenSet {}; 
	vertexStack.push_back(source);

	while(!vertexStack.empty()){
		string currVertex = vertexStack.back();
		vertexStack.pop_back();
		vertexSeenSet.insert(currVertex);
		for (Edge edge : adjacency_list[currVertex]){
			if (vertexSeenSet.count(edge.to) == 0) {
				vertexStack.push_back(edge.to);
			}

			int newCost = dists[currVertex] + edge.weight;
			if (newCost < dists[edge.to]){
				cout << "Negative Cycle Detected!!!!" << endl;
				printBellmonFord(dists, source);
				return dists;
			}
		}
	}

	printBellmonFord(dists, source);
    return dists;
}

int main(){
    Graph g;
    g.addEdge("A", "B", 10);
    g.addEdge("B", "D", 10);
    g.addEdge("D", "E", 7);
    g.addEdge("E", "F", 15);
    g.addEdge("E", "C", 5);
    g.addEdge("C", "B", 5);

	g.printGraph();
    BellmonFord(g, "A");
    return 0;
}
