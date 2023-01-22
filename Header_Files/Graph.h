#include <unordered_map>
#include <vector>
#include <string>

struct Edge {
    std::string to;
    double weight;
};

class Graph {
private:
    std::unordered_map<std::string, std::vector<Edge>> adjacency_list;

public:
    Graph () {}

    // TODO: Change graph to add edges in both directions as the crypto exchange rate is the inverse
    void addEdge(std::string from, std::string to, double weight);

    void printGraph();
};
