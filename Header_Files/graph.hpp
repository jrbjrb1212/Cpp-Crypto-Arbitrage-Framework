#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

struct Edge
{
    string to;
    double weight;
    string exchange;
};

class Graph
{
private:
    int m_graphEdges = 0;

public:
    Graph() {}
    unordered_map<string, vector<Edge> > adjacency_list;

    // TODO: Change graph to add edges in both directions as the crypto exchange rate is the inverse
    void addEdge(string from, string to, double weight, string exchange)
    {
        adjacency_list[from].push_back({to, -log(weight), exchange});
        adjacency_list[to].push_back({from, -log(1 / weight), exchange});
        m_graphEdges +=2;
    }

    int getVertexCount()
    {
        return adjacency_list.size();
    }

    int getEdgeCount()
    {
        return m_graphEdges;
    }

    void printGraph()
    {
        cout << "\nGraph Print in form of Adjaceny List: " << endl;
        for (auto it = adjacency_list.begin(); it != adjacency_list.end(); it++)
        {
            string vertex = it->first;
            vector<Edge> edges = it->second;
            cout << vertex << ": ";
            for (Edge edge : edges)
            {
                cout << "(" << edge.to << ", " << edge.weight << ", " << edge.exchange << ") ";
            }
            cout << endl;
        }
    }
};
