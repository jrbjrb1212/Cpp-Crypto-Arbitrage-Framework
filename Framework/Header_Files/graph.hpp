#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <cmath>

using namespace std;


/*
*
* Defines an edge in the graph
*
*/
struct Edge
{
    string to;
    double exPrice;
    double fee;
    string exchange;
    string bidOrAsk;
};


/*
*
* Adjacency list graph custom data
* structure that defines trade relations
* across all exchanges that the arbitrage bot 
* pulls active ticker data from.
*
*/
class Graph
{
private:
    int m_graphEdges = 0;

public:
    Graph() {}
    unordered_map<string, vector<Edge> > adjacencyList;

    // Expand the graph by adding an edge
    // - Usually only done during the 1st set of GET requests
    void addEdge(string from, string to, double fee, string exchange)
    {
        adjacencyList[from].push_back({to, 0.0, log(1-fee), exchange, ""});
        adjacencyList[to].push_back({from, 0.0, log(1-fee), exchange, ""});
        m_graphEdges += 2;
    }

    // Expand the graph by adding an edge
    // - Done when an orderbook is searched but it comes up empty
    //   thus the edge is illiquid and should not be searched in the
    void deleteEdge(string from, string to, string exchange)
    {
        vector<Edge> &edges = adjacencyList[from];
        for (int i = 0; i < edges.size(); i++)
        {
            if ((edges[i].to == to) && (edges[i].exchange == exchange))
            {
                edges.erase(edges.begin() + i);
                if (edges.size() == 0)
                    adjacencyList.erase(from);
                break;
            }
        }
        edges = adjacencyList[to];
        for (int i = 0; i < edges.size(); i++)
        {
            if ((edges[i].to == from) && (edges[i].exchange == exchange))
            {
                edges.erase(edges.begin() + i);
                if (edges.size() == 0)
                    adjacencyList.erase(to);
                break;
            }
        }
    }


    // Update an edge with new ticker prices
    // - Completed each time new ticker data is obtained
    void updateEdge(string from, string to, double bidPrice, double askPrice, string exchange)
    {
        for (Edge& edge : adjacencyList[from])
        {
            if ((edge.to == to) && (edge.exchange == exchange))
            {
                edge.exPrice = log(bidPrice);
                edge.bidOrAsk = "bid";
                break;
            }
        }
        for (Edge& edge : adjacencyList[to])
        {
            if ((edge.to == from) && (edge.exchange == exchange))
            {
                edge.exPrice = log(1) - log(askPrice);
                edge.bidOrAsk = "ask";
                break;
            }
        }
    }

    int getVertexCount()
    {
        return adjacencyList.size();
    }

    int getEdgeCount()
    {
        return m_graphEdges;
    }


    // Prints the all data contained within the graph
    void printGraph()
    {
        cout << "\nGraph Print in form of Adjaceny List: " << endl;
        for (auto it = adjacencyList.begin(); it != adjacencyList.end(); it++)
        {
            string vertex = it->first;
            vector<Edge> edges = it->second;
            cout << vertex << ": ";
            for (Edge edge : edges)
            {
                cout << "(" << edge.to << ", " << edge.exPrice << ", " << edge.fee << ", " << edge.exchange << ") ";
            }
            cout << endl;
        }
    }


    // Prints all data associated with a particular edge in the graph
    void printEdge(string from, string to, string exchange) {
        vector<Edge> edges = adjacencyList[from];
        
        // Loop through the edges to find the edge that ends at the specified vertex
        for (Edge edge : edges) {
            if (edge.to == to && exchange == edge.exchange) {
                // Print the edge information
                cout << "Edge from " << from << " to " << to << " with exchange " << edge.exchange << ": ";
                cout << "ExPrice = " << edge.exPrice << ", Fee = " << edge.fee << endl;
                return;
            }
        }
        
        // If the loop finishes without finding the edge, print an error message
        cout << "Error: no edge found from " << from << " to " << to << endl;
    } 
};
