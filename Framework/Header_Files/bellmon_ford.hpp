// CURRENTLY DEPRECATED AND NOT IN USE

#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <tuple>
#include "graph.hpp"
#include <assert.h>

using namespace std;
vector<string> maxPath;
// double maxProfit = 0;


double weightConversion(double edgeWeight){
	return exp(-1 * (edgeWeight / log(exp(1))));
}

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
	cout << "Arbritrage Detected!" << endl;
	for (int i = 0; i < cycleVec.size() - 1; i++)
	{
		cout << cycleVec[i] << " --> ";
	}

	cout << cycleVec[cycleVec.size() - 1] << endl;
}


/*
*
* Method to calculate the profitablity of a arbritage opportunity
* prints to console the profitability percentage of the trade
# See TODO:
*/
double ProfitCalc(vector<string> cycleVec, unordered_map<string, vector<Edge> > adjacency_list)
{
	// TODO: this is an inefficent search for the trades needed to calcualte arbritage profit
	// may change letter if my adjaceny list strucutre goes from hashmap[string, vectors] to hashmap[string, hashmap]

	double sourceVal = 1;
	string fromSymbol = cycleVec[cycleVec.size() - 1];
	for (int i = cycleVec.size() - 2; i >= 0; i--)
	{
		string toSymbol = cycleVec[i];
		// cout << "Working with " << fromSymbol << " and " << toSymbol << endl;
		for (Edge edge : adjacency_list[fromSymbol])
		{
			if (edge.to == toSymbol)
			{
				// cout << "Trade from: " << fromSymbol << " to: " << toSymbol << endl;
				// cout << "-ln(symbolTradeVal) aka edge.weight: " << edge.weight << endl;
				// cout << "-1*(edge.weight/exp(1) aka e^{...}: " << (-1 * (edge.weight / log(exp(1)))) << endl;
				// cout << "Decrypted oringinal trade val: " << exp(-1 * (edge.weight / log(exp(1)))) << endl;
				sourceVal = sourceVal * exp(-1 * (edge.weight / log(exp(1))));
				// cout << "SourceVal: " << sourceVal << endl;
			}
		}
		fromSymbol = toSymbol;
	}
	// Reverse iterate through cycleVec
	// Get the cycleVec.size() - 1 trades from my natural log to decmial formula
	sourceVal = (sourceVal - 1) * 100;
	cout << sourceVal << "% profitability" << endl;
	cout << endl;
	return sourceVal;
}


void printVector(const vector<string> &vec)
{
	for (const string &str : vec)
	{
		cout << str << ", ";
	}
	cout << endl;
}


void printUnorderedSet(const unordered_set<string> &set)
{
	for (const string &str : set)
	{
		cout << str << ", ";
	}
	cout << endl;
}


void printUnorderedMap(const unordered_map<string, string> &map)
{
	for (auto access : map)
	{
		cout << "From: " << access.first << ", To: " << access.second << endl;
	}
	cout << endl;
}

// variable types
// void dfsMaxProfit(unordered_map<string, vector<Edge> > adjaceny_list, string source, double currProfit,
// 				  string currVertex, unordered_set<string> seenVertexSet, vector<string> path)
// {
// 	// base case for getting back to the source start
// 	if ((currVertex == source))
// 	{
// 		// printVector(path);
// 		if (currProfit < maxProfit)
// 		{
// 			maxProfit = currProfit;
// 			maxPath = path;
// 		}
// 		else if (currProfit == maxProfit)
// 		{
// 			maxPath = (path.size() < maxPath.size()) ? path : maxPath;
// 		}
// 		return;
// 		// return currProfit;
// 	}
// 	if (seenVertexSet.size() >= 3)
// 	{
// 		return;
// 		// return -1;
// 	}

// 	for (Edge edge : adjaceny_list[currVertex])
// 	{
// 		cout << "Curr Max Profit: " << maxProfit << endl;
// 		cout << "Curr Profit: " << currProfit << endl;
// 		cout << "Curr Vertex from: " << currVertex << endl;
// 		cout << "Curr Edge To: " << edge.to << endl;
// 		cout << "Curr Path Vector: ";
// 		printVector(path);
// 		cout << "Seen set: ";
// 		printUnorderedSet(seenVertexSet);
// 		cout << endl;
// 		// if the desintation of the edge has not be visited yet
// 		if (seenVertexSet.count(edge.to) == 0)
// 		{
// 			path.push_back(edge.to);
// 			seenVertexSet.insert(edge.to);
// 			// exp(-1 * (edge.weight / log(exp(1))));
// 			currProfit += edge.weight;
// 			dfsMaxProfit(adjaceny_list, source, currProfit, edge.to, seenVertexSet, path);
// 			// double dfsProfit = dfsMaxProfit(adjaceny_list, source, currProfit, edge.to, seenVertexSet, path);
// 			seenVertexSet.erase(edge.to);
// 			// currProfit = (dfsProfit < currProfit) ? dfsProfit : currProfit;
// 			path.pop_back();
// 		}
// 	}

// 	return;
// 	// return currProfit;
// }

/*
*
* Implmenetation of Bellmon Ford Algorithm to detect Currency Arbritage opportunities
* TODO: Implement more efficent or parallel versions of Bellmon Ford
*/
vector<string> BellmonFord(Graph g, string source, double profitThreshold)
{
	// const float INF = numeric_limits<float>::max();
	// const int V = g.getVertexCount();
	unordered_map<string, vector<Edge> > adjacency_list = g.adjacency_list;
	// unordered_map<string, string> prevVert;

	// // Initialize distance array
	// unordered_map<string, double> dists;
	// for (auto access : adjacency_list)
	// {
	// 	dists[access.first] = INF;
	// }
	// dists[source] = 1;

	// // Do V-1 iterations of Bellmon Ford
	// for (int i = 0; i < V - 1; i++)
	// {
	// 	vector<string> vertexStack{};
	// 	unordered_set<string> vertexSeenSet{};
	// 	vertexStack.push_back(source);

	// 	while (!vertexStack.empty())
	// 	{
	// 		string currVertex = vertexStack.back();
	// 		vertexStack.pop_back();
	// 		vertexSeenSet.insert(currVertex);

	// 		// traverse all the edges of currVertex
	// 		for (Edge edge : adjacency_list[currVertex])
	// 		{
	// 			if (vertexSeenSet.count(edge.to) == 0)
	// 			{
	// 				vertexStack.push_back(edge.to);
	// 			}

	// 			double newCost = dists[currVertex] + edge.weight;
	// 			if (newCost < dists[edge.to])
	// 			{
	// 				dists[edge.to] = newCost;
	// 				prevVert[edge.to] = currVertex;
	// 			}
	// 		}
	// 	}
	// }

	// I only want to detect a negative cycle in which the source coin is invloved
	/*
    string currVertex = source;
    vector<string> vertexStack;
    vertexStack.push_back(source);
    unordered_set<string> vertexSeenSet;
    double max2Profit = 1;
    // call recursion here
    for (Edge edge : adjacency_list[source])
    {
        vector<string> path{source, edge.to};
        vertexSeenSet.insert(edge.to);
        double currProfit = edge.weight;
        // double dfsProfit = dfsMaxProfit(adjacency_list, source, currProfit, edge.to, vertexSeenSet, path);
        dfsMaxProfit(adjacency_list, source, currProfit, edge.to, vertexSeenSet, path);
        vertexSeenSet.erase(edge.to);
        // if (dfsProfit < max2Profit){
        //     max2Profit = dfsProfit;
        // }
    }
    cout << "maxProfit: " << maxProfit << endl;
    // check if any trade is profitable
    double maxProfitConversion = exp(-1 * (maxProfit / log(exp(1))));
    cout << "MaxProfit Conversion: " << maxProfitConversion << endl;
    if (maxProfit >= (1 + profitThreshold))
    {
        PrintCycle(maxPath);
        cout << (maxProfit - 1) * 100 << "% Profitability" << endl;
    }
    */

	/*
    * Detect a negative cycle anywhere in the graph 
    * V-th relax loop to check for negative cycle
    */

	// TODO: modify such that I am only looking for negative cycles that include my selected source Coin

	// TODO: Try to implement this article https://www.geeksforgeeks.org/print-negative-weight-cycle-in-a-directed-graph/
	// or this one https://cp-algorithms.com/graph/finding-negative-cycle-in-graph.html#implementation
	// I need to have a way to discover the path that has a negative cycle and calculate if it is profitable
	double maxProfit = 0;
	double currProfit = 0;
	int negCycles = 0;
	vector<string> negCyclePath;

	string currVertex = source;
	for (Edge fristTradeEdge : adjacency_list[currVertex]){
		currProfit += fristTradeEdge.weight;
		
		for (Edge secondTradeEdge : adjacency_list[fristTradeEdge.to]){
			if (secondTradeEdge.to == source){
				continue;
			}
			currProfit += secondTradeEdge.weight;

			for (Edge sourceTradeEdge : adjacency_list[secondTradeEdge.to]){
				if (sourceTradeEdge.to != source){
					continue;
				}
				currProfit += sourceTradeEdge.weight;
				if (currProfit < maxProfit){
					maxProfit = currProfit;
					negCyclePath = {source, fristTradeEdge.to, secondTradeEdge.to, source};
				}
				currProfit -= sourceTradeEdge.weight;
				break;
			}

			currProfit -= secondTradeEdge.weight;
		}

		currProfit -= fristTradeEdge.weight;
	}

	if (maxProfit < 0){
		cout << endl;
		cout << "Arbritarge Opportunity Detected!!" << endl;
		double maxProfitConversion = weightConversion(maxProfit);
		cout << "MaxProfit in -log(x): " << maxProfit << endl;
		cout << "MaxProfit in x: " << maxProfitConversion << endl;
		cout << ((maxProfitConversion - 1) * 100) << "% profitability" << endl;
		PrintCycle(negCyclePath);
		cout << endl;
	}

	return negCyclePath;

	// For loop to go through all the edges of USD
	// For loop to go through all the edges of edges of USD
	// get back to USD most likely by iterating through the edges of the previous



	// while (!vertexStack.empty()){
	// 	string currVertex = vertexStack.back();
	// 	vertexStack.pop_back();
	// 	vertexSeenSet.insert(currVertex);
	// 	for (Edge edge : adjacency_list[currVertex])
	// 	{	
	// 		if (vertexSeenSet.count(edge.to) == 0)
	// 		{
	// 			vertexStack.push_back(edge.to);
	// 		}
	// 		double newCost = dists[currVertex] + edge.weight;
	// 		if (newCost < dists[edge.to])
	// 		{
	// 			cout << "Negative Cycle Detected!!!" << endl;
	// 			cout << "Arbritage Opportunity Detected" << endl;
	// 		}
	// 	}
	// }

	// if (C != -1) {
    //     for (int i = 0; i < V; i++)
    //         C = parent[C];
    //     // To store the cycle vertex
    //     vector<int> cycle;
    //     for (int v = C;; v = parent[v]) {
    //         cycle.push_back(v);
    //         if (v == C
    //             && cycle.size() > 1)
    //             break;
    //     }

    //     // Reverse cycle[]
    //     reverse(cycle.begin(), cycle.end());

    //     // Printing the negative cycle
    //     for (int v : cycle)
    //         cout << v << ' ';
    //     cout << endl;
	// 	return;
    // }

	// while (!vertexStack.empty() && vertexStack[0] == source)
	// {
	// 	string currVertex = vertexStack.back();
	// 	vertexStack.pop_back();
	// 	vertexSeenSet.insert(currVertex);
	// 	for (Edge edge : adjacency_list[currVertex])
	// 	{
	// 		if (vertexSeenSet.count(edge.to) == 0)
	// 		{
	// 			vertexStack.push_back(edge.to);
	// 		}

	// 		double newCost = dists[currVertex] + edge.weight;
	// 		if (newCost < dists[edge.to])
	// 		{
	// 			short i = 0;
	// 			cout << "\nNegative Cycle Detected!!!!" << endl;
	// 			// Go through prevVert to print out the path needed to execute the trade
	// 			vector<string> cycleVec = {edge.to, currVertex};
	// 			unordered_set<string> vertexSeenSet = {edge.to, currVertex};
	// 			while (vertexSeenSet.count(prevVert[currVertex]) == 0)
	// 			{
	// 				if (i >= 5)
	// 				{
	// 					break;
	// 				}
	// 				cycleVec.push_back(prevVert[currVertex]);
	// 				currVertex = prevVert[currVertex];
	// 				i++;
	// 			}
	// 			if (i >= 5)
	// 			{
	// 				break;
	// 			}
	// 			cycleVec.push_back(prevVert[currVertex]);
	// 			printVector(cycleVec);
	// 			// cycleVec = CorrectArbCycle(cycleVec, 1);
	// 			// double profitability = ProfitCalc(cycleVec, adjacency_list);
	// 			return;
	// 		}
	// 	}
	// }

	// printBellmonFord(dists, source);
	// return dists;
}