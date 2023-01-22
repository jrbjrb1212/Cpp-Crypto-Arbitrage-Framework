#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>


struct Edge {
    std::string to;
    double weight;
};

class Graph {   

public:
    std::unordered_map<std::string, std::vector<Edge>> adjacency_list;
    Graph () {}

    // TODO: Change graph to add edges in both directions as the crypto exchange rate is the inverse
    void addEdge(std::string from, std::string to, double weight) {
        adjacency_list[from].push_back({to, weight});
    }

    void printGraph() {
        for (auto it = adjacency_list.begin(); it != adjacency_list.end(); it++) {
            std::string vertex = it->first;
            std::vector<Edge> edges = it->second;
            std::cout << vertex << ": ";
            for (const auto& edge : edges) {
                std::cout << "(" << edge.to << ", " << edge.weight << ") ";
            }
            std::cout << std::endl;
        }
    }
};
