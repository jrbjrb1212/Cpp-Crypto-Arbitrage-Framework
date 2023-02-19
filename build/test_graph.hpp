#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <cmath>

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

    void addEdge(string from, string to, double fee, string exchange)
    {
        adjacency_list[from].push_back({to, 0.0, 0.0, log(1-fee), exchange});
        adjacency_list[to].push_back({from, 0.0, 0.0, log(1-fee), exchange});
        m_graphEdges +=2;
    }

    void updateEdge(string from, string to, long double bidPrice, long double askprice, string exchange)
    {
        for (Edge& edge : adjacency_list[from])
        {
            if ((edge.to == to) && (edge.exchange == exchange))
            {
                edge.bidPrice = log(bidPrice);
                edge.askPrice = log(askprice);
                break;
            }
        }
        for (Edge& edge : adjacency_list[to])
        {
            if ((edge.to == from) && (edge.exchange == exchange))
            {
                edge.bidPrice = log(1/askprice);
                edge.askPrice = log(1/bidPrice);
                break;
            }
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
