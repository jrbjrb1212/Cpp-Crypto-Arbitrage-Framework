#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

struct Edge
{
    string to;
    long double bidPrice;
    long double askPrice;
    double fee;
    string exchange;
};

class Graph
{
private:
    int m_graphEdges = 0;

public:
    Graph() {}
    unordered_map<string, vector<Edge> > adjacency_list;

    void addEdge(string from, string to, long double bidPrice, long double askPrice, double fee, string exchange)
    {
        // adjacency_list[from].push_back({to, bidPrice, askPrice, (1-fee), exchange});
        // adjacency_list[to].push_back({from, (1 / askPrice), (1 / bidPrice), (1-fee), exchange});
        adjacency_list[from].push_back({to, log(bidPrice), log(askPrice), log(1-fee), exchange});
        adjacency_list[to].push_back({from, log(1 / askPrice), log(1/bidPrice), log(1-fee), exchange});
        m_graphEdges +=2;
    }

    //TODO: add a method to update an edge's weight as I think it will be faster than destroying the graph and rebuilding from scratch

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
                cout << "(" << edge.to << ", " << edge.bidPrice << ", " << edge.askPrice << ", " << edge.fee << ", " << edge.exchange << ") ";
            }
            cout << endl;
        }
    }
};
