#include <iostream>
#include <unordered_map>
#include <vector>
#include <limits>

using namespace std;

const double INF = numeric_limits<double>::max();

struct Edge {
    string to;
    double weight;
};

void BellmanFord(unordered_map<string, vector<Edge>>& adjList, string source, unordered_map<string, double>& dist, unordered_map<string, string>& pred) {
    int V = adjList.size();
    for(auto &[key, value]: dist) dist[key] = INF;
    dist[source] = 1; //Initialize distance from source to source as 1
    for(auto &[key, value]: pred) pred[key] = "";
    pred[source] = source;

    for (int i = 0; i < V - 1; i++) {
        for (auto& [u, edges] : adjList) {
            for (auto& edge : edges) {
                string v = edge.to;
                double w = edge.weight;
                if (dist[v] > dist[u] * (1/w)) {
                    dist[v] = dist[u] * (1/w);
                    pred[v] = u;
                }
            }
        }
    }

    // check for negative cycles
    for (auto& [u, edges] : adjList) {
        for (auto& edge : edges) {
            string v = edge.to;
            double w = edge.weight;
            if (dist[v] > dist[u] * (1/w)) {
                cout << "Negative cycle detected!" << endl;
                cout << "Negative cycle path: ";
                string cur = v;
                while (cur != source) {
                    cout << cur << " ";
                    cur = pred[cur];
                }
                cout << source << endl;
                return;
            }
        }
    }

    // print the shortest distances
    cout << "Shortest distances from the source vertex:" << endl;
    for (auto& [i, d] : dist) {
        cout << i << ": " << d << endl;
    }
}


int main() {
    unordered_map<string, vector<Edge>> adjList;
    unordered_map<string, double> dist;
    unordered_map<string, string> pred;

    // Add edges to the adjacency list
    adjList["USD"].push_back({ "EUR", 0.8 });
    adjList["EUR"].push_back({ "GBP", 1.2 });
    adjList["GBP"].push_back({ "USD", 0.6 });
    adjList["USD"].push_back({ "GBP", 0.9 });

    string source = "USD";

    BellmanFord(adjList, source, dist, pred);
}
