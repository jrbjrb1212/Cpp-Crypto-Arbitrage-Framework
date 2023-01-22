#include <iostream>
#include <unordered_map>
#include <vector>


struct Edge {
    std::string to;
    double weight;
};


class Graph {
public:
    Graph() {}

    // TODO: Change graph to add edges in both directions as the crypto exchange is the inverse
    void addEdge(std::string from, std::string to, double weight) {
        adjacency_list[from].push_back({to, weight});
    }

    void printGraph() {
        for (const auto& [vertex, edges] : adjacency_list) {
            std::cout << vertex << ": ";
            for (const auto& edge : edges) {
                std::cout << "(" << edge.to << ", " << edge.weight << ") ";
            }
            std::cout << std::endl;
        }
    }

private:
    std::unordered_map<std::string, std::vector<Edge>> adjacency_list;
};
