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

//TODO: Fix snake case typing of adjaceny list
//TODO: Remove any size() for loop as size is a member variable of atleast the vector container

struct TrackProfit
{
    string from;
    string to;
    double weight;
    string exchange;
};


/*
*
* Computes from log representation of edge weight to
* double representation of edge weight
# 
*/
double WeightConversion(double edgeWeight){
	return exp(-1 * (edgeWeight / log(exp(1))));
}


void PrintCycle(vector<TrackProfit> cycleVec)
{
	cout << "Arbritrage Detected!" << endl;
	for (int i = 0; i < cycleVec.size() - 1; i++)
	{
		cout << cycleVec[i].from << " --> ";
	}

	cout << cycleVec[cycleVec.size() - 1].from << " --> " << cycleVec[cycleVec.size() - 1].to << endl;
}

/*
*
* Method to calculate the profitablity of a arbritage opportunity
* prints to console the profitability percentage of the trade
# See TODO:
*/
/*
double ProfitCalc(vector<string> cycleVec, unordered_map<string, vector<Edge> > adjacency_list)
{
	// TODO: this is an inefficent search for the trades needed to calcualte arbritage profit
	// may change letter if my adjaceny list strucutre goes from hashmap[string, vectors] to hashmap[string, hashmap]

	double sourceVal = 1;
	string fromSymbol = cycleVec[cycleVec.size() - 1];
	for (int i = cycleVec.size() - 2; i >= 0; i--)
	{
		string toSymbol = cycleVec[i];
		for (Edge edge : adjacency_list[fromSymbol])
		{
			if (edge.to == toSymbol)
			{
				sourceVal = sourceVal * exp(-1 * (edge.weight / log(exp(1))));
			}
		}
		fromSymbol = toSymbol;
	}
	sourceVal = (sourceVal - 1) * 100;
	cout << sourceVal << "% profitability" << endl;
	cout << endl;
	return sourceVal;
}
*/

/*
*
* Heplful method that prints out contents of a string vector
*
*/
void printVector(const vector<string> &vec)
{
	for (const string &str : vec)
	{
		cout << str << ", ";
	}
	cout << endl;
}


/*
*
* Heplful method that prints out contents of a hash set vector
*
*/
void printUnorderedSet(const unordered_set<string> &set)
{
	for (const string &str : set)
	{
		cout << str << ", ";
	}
	cout << endl;
}


/*
*
* Heplful method that prints out contents of a hash map vector
*
*/
void printUnorderedMap(const unordered_map<string, string> &map)
{
	for (auto access : map)
	{
		cout << "From: " << access.first << ", To: " << access.second << endl;
	}
	cout << endl;
}


/*
*
* Implmenetation of a brute force O(n^3) algorithm to detect
* currency arrbritage opportunities
* TODO: Implement parallel version of this code to massively speed up code
* TODO: Implement a linear programming model that can solve faster asympotically
*/
/*
vector<TrackProfit> ArbDetect(Graph g, string source, double lowerProfitThreshold, double upperProfitThreshold)
{
	unordered_map<string, vector<Edge> > adjacency_list = g.adjacency_list;

    double upperBound = log(upperProfitThreshold);
    double lowerBound = log(lowerProfitThreshold);
	double maxProfit = 0, currProfit = 0;
	int negCycles = 0;
	vector<TrackProfit> negCyclePath;

    /*
    *
    * Algorithm attempts to fill in this path with the most profitable trade
    * source --> Coin1 --> Coin2 --> source
    * 
    

	for (Edge fristTradeEdge : adjacency_list[source]){
		currProfit += fristTradeEdge.weight;
		
		for (Edge secondTradeEdge : adjacency_list[fristTradeEdge.to]){
			if (secondTradeEdge.to == source){
				continue;
			}
			currProfit += secondTradeEdge.weight;

			for (Edge sourceTradeEdge : adjacency_list[secondTradeEdge.to]){
				if (sourceTradeEdge.to == source){
                    currProfit += sourceTradeEdge.weight;
                    
                    if (lowerBound > currProfit && currProfit > upperBound){
                        cout << "Curr Profit: " << ((WeightConversion(currProfit) - 1) * 100) << "% profitability :";
                        cout << " Log representation: " << currProfit << " Upper Bound: " << upperBound <<  endl;
                        if (currProfit < maxProfit){
                        maxProfit = currProfit;
                        TrackProfit trade1 {source, fristTradeEdge.to, fristTradeEdge.weight, fristTradeEdge.exchange};
                        TrackProfit trade2 {fristTradeEdge.to, secondTradeEdge.to, secondTradeEdge.weight, secondTradeEdge.exchange};
                        TrackProfit trade3 {secondTradeEdge.to, sourceTradeEdge.to, sourceTradeEdge.weight, sourceTradeEdge.exchange};
                        negCyclePath = {trade1, trade2, trade3};
                        }
                    }
                    currProfit -= sourceTradeEdge.weight;
                    break;
				}
			}
			currProfit -= secondTradeEdge.weight;
		}

		currProfit -= fristTradeEdge.weight;
	}
    cout << maxProfit << endl;

	if (maxProfit < log(lowerProfitThreshold) && maxProfit > upperBound){
        double maxProfitConversion = WeightConversion(maxProfit);
		cout << endl;
		cout << "Arbritarge Opportunity Detected!!" << endl;
		cout << "MaxProfit in -log(x): " << maxProfit << endl;
		cout << "MaxProfit in x: " << maxProfitConversion << endl;
		cout << ((maxProfitConversion - 1) * 100) << "% profitability" << endl;
		PrintCycle(negCyclePath);
		cout << endl;
	}

	return negCyclePath;
}
*/


/*
*
* Implmenetation of a brute force O(n^3) algorithm 
* currency arrbritage opportunities with considerations for fees and spread
* TODO: Implement parallel version of this code to massively speed up code
* TODO: Implement a linear programming model that can solve faster asympotically
*/
vector<TrackProfit> ArbDetect(Graph g, string source, double lowerProfitThreshold, double upperProfitThreshold)
{
	unordered_map<string, vector<Edge> > adjacency_list = g.adjacency_list;

    double upperBound = log(upperProfitThreshold);
    double lowerBound = log(lowerProfitThreshold);
	double maxProfit = 0, currProfit = 0;
	int negCycles = 0;
	int startAmt = 1000;
	vector<TrackProfit> negCyclePath;

    /*
    *
    * Algorithm attempts to fill in this path with the most profitable trade
    * source --> Coin1 --> Coin2 --> source
    * 
    */

	for (Edge fristTradeEdge : adjacency_list[source]){		
		currProfit = startAmt * (fristTradeEdge.bidPrice/fristTradeEdge.askPrice) * (1-fristTradeEdge.fee);
		for (Edge secondTradeEdge : adjacency_list[fristTradeEdge.to]){
			if (secondTradeEdge.to == source){
				continue;
			}
			for (Edge sourceTradeEdge : adjacency_list[secondTradeEdge.to]){
				if (sourceTradeEdge.to == source){
                    double profit = startAmt * (fristTradeEdge.bidPrice/fristTradeEdge.askPrice) * (1-fristTradeEdge.fee);
					profit *= (sourceTradeEdge.bidPrice/secondTradeEdge.askPrice) * (1-secondTradeEdge.fee);
					profit *= (sourceTradeEdge.askPrice);
					profit -= (fristTradeEdge.askPrice * secondTradeEdge.askPrice) * (1-sourceTradeEdge.fee);
					if (profit > startAmt){
						cout << "Profit: " << profit << endl;	
						vector<string> path {source, fristTradeEdge.to, secondTradeEdge.to, sourceTradeEdge.to};
						printVector(path);
					}
                    
                    break;
				}
			}
		}
	}
    // cout << maxProfit << endl;

	// if (maxProfit < log(lowerProfitThreshold) && maxProfit > upperBound){
    //     double maxProfitConversion = WeightConversion(maxProfit);
	// 	cout << endl;
	// 	cout << "Arbritarge Opportunity Detected!!" << endl;
	// 	cout << "MaxProfit in -log(x): " << maxProfit << endl;
	// 	cout << "MaxProfit in x: " << maxProfitConversion << endl;
	// 	cout << ((maxProfitConversion - 1) * 100) << "% profitability" << endl;
	// 	PrintCycle(negCyclePath);
	// 	cout << endl;
	// }

	return negCyclePath;
}