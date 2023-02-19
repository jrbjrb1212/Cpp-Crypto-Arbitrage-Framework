#pragma once

// TODO: Remove imports that never get used
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <tuple>
#include <thread>
#include <mutex>
#include <assert.h>
#include "graph.hpp"
#include "combinations.hpp"


using namespace std;

//TODO: Fix snake case typing of adjaceny list


/*
*
* Struct designed for log data storage and arbirtage validation outside of initali discovery
*
*/
struct TrackProfit
{
    string from;
    string to;
    long double orderPrice;
	string bidOrask;
    string exchange;
};


/*
*
* Computes from log representation of edge weight to
* double decimal representation of edge weight
*
*/
long double WeightConversion(long double conversionMetric){
	return exp((conversionMetric / log(exp(1))));
}


/*
*
* Heplful method that prints out the trades of an arbritage opporutunity
*
*/
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
* Method for determing if current profit is the max profit
* Also, makes applicable path tracking updates
*
*/
bool maxProfitCheck(double& maxProfit, double& currProfit, double& lowerThreshold, double& upperThreshold){
	if (currProfit > maxProfit && currProfit > lowerThreshold && currProfit < upperThreshold) {
		maxProfit = currProfit;
		return true;
	}
	return false;
}


/*
*
* Method to determine cost a specific trade
* When buying a currency, the ask price must be paid for likely instant order fullifillment 
* When selling a currency, the bid price must be paid for likely instant order fullifillment 
*
*/
void makeTrackProfitStruct(TrackProfit& tradeStruct, Edge tradeInfo, string tradeType){
	if (tradeType == "ask")
	{
		tradeStruct.orderPrice = tradeInfo.bidPrice;
	}
	else if (tradeType == "bid")
	{
		tradeStruct.orderPrice = tradeInfo.askPrice;
	}
}


/*
*
* Method to record information about the most proftiable arbritrage path
* Designed to be used for profit validation and order/trade amount optimization 
*
*/
void updateMaxPath(vector<TrackProfit>& negCyclePath, vector<string>& tradeTypes, vector<Edge> trades){
	vector<TrackProfit> path;
	
	// need to model start currency to end currency via a singular trade
	// firstTrade is special case because each edge struct only contains the destination currency
	TrackProfit firstTrade {trades[trades.size()-1].to, trades[0].to, 0.0, tradeTypes[0], trades[0].exchange};
	makeTrackProfitStruct(firstTrade, trades[0], tradeTypes[0]);
	path.push_back(firstTrade);

	// Add each trade edge to the cyclePath for later validation
	for(int i = 0; i < trades.size()-1; i++)
	{
		TrackProfit trade {trades[i].to, trades[i+1].to, 0.0, tradeTypes[i+1], trades[i+1].exchange};
		makeTrackProfitStruct(trade, trades[i+1], tradeTypes[i+1]);
		path.push_back(trade);
	}
	
	negCyclePath = path;
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
	double maxProfit = 0, currProfit;
	int negCycles = 0;
	int startCap = 1;
	vector<TrackProfit> negCyclePath;
	vector<string> tradeTypes { "BLANK", "BLANK", "BLANK"};

    /*
    *
    * Algorithm attempts to fill in this path with the most profitable trade
    * source --> Coin1 --> Coin2 --> source
    * 
    */

   // Current algorithm relies on the idea that for a given edge there is only travel on that edge
   // With the idea of bid-ask pricing, I have the choice at each egde to choose between buying the destination currency with the source currency
   // at the Ask price or selling the source currency for the destination currency at the Bid price

	for (Edge firstTradeEdge : adjacency_list[source]){	
		// search through buying the destination currency from the current currency	

		// I am paying the askPrice price than I utilizing a buy
		// If I want to instantly buy a currency, I pay the ask price
		// If I want to instantly sell my currency, I pay the buy price
		currProfit = (firstTradeEdge.askPrice + firstTradeEdge.fee);
		tradeTypes[0] = "bid";
		for (Edge secondTradeEdge : adjacency_list[firstTradeEdge.to]){
			if (secondTradeEdge.to == source){
				continue;
			}

			// Search through buying the destination currency from the current currency
			currProfit += (secondTradeEdge.askPrice + secondTradeEdge.fee);
			tradeTypes[1] = "bid";
			for (Edge thirdTradeEdge : adjacency_list[secondTradeEdge.to]){
				if (thirdTradeEdge.to == source){
					// search through buying the destination currency from the current currency
					currProfit += (thirdTradeEdge.askPrice + thirdTradeEdge.fee);
					tradeTypes[2] = "bid";
					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
					currProfit -= (thirdTradeEdge.askPrice + thirdTradeEdge.fee);

		 			// Search through selling the current currency for the destination currency
					currProfit += (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
					tradeTypes[2] = "ask";
					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
						continue;
						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
					currProfit -= (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
                    break;
				}
			}
			currProfit -= (secondTradeEdge.askPrice + secondTradeEdge.fee);


		 	// Search through selling the current currency for the destination currency
			currProfit += (secondTradeEdge.bidPrice + secondTradeEdge.fee);
			tradeTypes[1] = "ask";
			for (Edge thirdTradeEdge : adjacency_list[secondTradeEdge.to]){
				if (thirdTradeEdge.to == source){
					// search through buying the destination currency from the current currency
					currProfit += (thirdTradeEdge.askPrice + thirdTradeEdge.fee);
					tradeTypes[2] = "bid";
					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
					currProfit -= (thirdTradeEdge.askPrice + thirdTradeEdge.fee);

		 			// Search through selling the current currency for the destination currency
					currProfit += (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
					tradeTypes[2] = "ask";
					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
					currProfit -= (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
                    break;
				}
			}
			currProfit -= (secondTradeEdge.bidPrice + secondTradeEdge.fee);
		}

		// Search through selling the current currency for the destination currency
		currProfit = (firstTradeEdge.bidPrice + firstTradeEdge.fee);
		tradeTypes[0] = "ask";
		for (Edge secondTradeEdge : adjacency_list[firstTradeEdge.to]){
			if (secondTradeEdge.to == source){
				continue;
			}

			// Search through buying the destination currency from the current currency
			currProfit += (secondTradeEdge.askPrice + secondTradeEdge.fee);
			tradeTypes[1] = "bid";
			for (Edge thirdTradeEdge : adjacency_list[secondTradeEdge.to]){
				if (thirdTradeEdge.to == source){
					// search through buying the destination currency from the current currency
					currProfit += (thirdTradeEdge.askPrice + thirdTradeEdge.fee);
					tradeTypes[2] = "bid";
					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
					currProfit -= (thirdTradeEdge.askPrice + thirdTradeEdge.fee);

		 			// Search through selling the current currency for the destination currency
					currProfit += (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
					tradeTypes[2] = "ask";
					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
					currProfit -= (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
                    break;
				}
			}
			currProfit -= (secondTradeEdge.askPrice + secondTradeEdge.fee);


		 	// Search through selling the current currency for the destination currency
			currProfit += (secondTradeEdge.bidPrice + secondTradeEdge.fee);
			tradeTypes[1] = "ask";
			for (Edge thirdTradeEdge : adjacency_list[secondTradeEdge.to]){
				if (thirdTradeEdge.to == source){
					// search through buying the destination currency from the current currency
					currProfit += (thirdTradeEdge.askPrice + thirdTradeEdge.fee);
					tradeTypes[2] = "bid";
					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
					currProfit -= (thirdTradeEdge.askPrice + thirdTradeEdge.fee);

		 			// Search through selling the current currency for the destination currency
					currProfit += (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
					tradeTypes[2] = "ask";
					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
					currProfit -= (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
                    break;
				}
			}
			currProfit -= (secondTradeEdge.bidPrice + secondTradeEdge.fee);

		}

	}
	cout << "MaxProfit (log): " << maxProfit << endl;
    cout << "MaxProfit: " << (WeightConversion(maxProfit) - 1) * 100 << "%\n" << endl;

	return negCyclePath;
}


/*
*
* Method for determing tradetypes from a given combintion sequence
* Method turns "AAB" to {"ask", "ask", "bid"} 
*
*/
void combo2TradeType(vector<string>& tradeTypes, string& combo)
{
	for(int i = 0; i < combo.length(); i++)
	{
		if (combo[i] == 'A')
		{
			tradeTypes.push_back("ask");
		}
		else if (combo[i] == 'B')
		{
			tradeTypes.push_back("bid");
		}
	}
}


/*
*
* Method uses information about the tradetype to determine trade cost
* Method determines that a buy trade will cost the askPrice + fees and vice versa 
*
*/
double tradeCostFromType(Edge& trade, string& tradeType)
{
	if (tradeType == "bid")
	{
		return trade.askPrice + trade.fee;
	}
	else if (tradeType == "ask")
	{
		return trade.bidPrice + trade.fee;
	}

	return -1.0;
}


/*
*
* Band-aid struct created to reduce arguments required for processCombination functions
*
*/
struct processCombinationInput{
	double lowerBound;
	double upperBound;
	string source;
	string combo;
};


/*
*
* Strucutre method for determining profitability of triangular arbitrages
* Extropliated into a function as at each trade the bid or ask price can be used
*
*/
void processCombinationLen3(Graph& g, double& maxProfit, vector<TrackProfit>& negCyclePath, processCombinationInput profitVars)
{
	double currProfit;
	vector<string> tradeTypes;
	combo2TradeType(tradeTypes, profitVars.combo);

	
	for (Edge firstTradeEdge : g.adjacency_list[profitVars.source]){	
		currProfit = tradeCostFromType(firstTradeEdge, tradeTypes[0]);

		for (Edge secondTradeEdge : g.adjacency_list[firstTradeEdge.to]){
			if (secondTradeEdge.to == profitVars.source){
				continue;
			}

			currProfit += tradeCostFromType(secondTradeEdge, tradeTypes[1]);

			for (Edge thirdTradeEdge : g.adjacency_list[secondTradeEdge.to]){
				if (thirdTradeEdge.to == profitVars.source){
					currProfit += tradeCostFromType(thirdTradeEdge, tradeTypes[2]);
					// Lock maxProfit and negCyclePath
					if (maxProfitCheck(maxProfit, currProfit, profitVars.lowerBound, profitVars.upperBound))
					{
						vector<Edge> path {firstTradeEdge, secondTradeEdge, thirdTradeEdge};
						updateMaxPath(negCyclePath, tradeTypes, path);
					}
					currProfit -= tradeCostFromType(thirdTradeEdge, tradeTypes[2]);
					break;
				}
			}

			currProfit -= tradeCostFromType(secondTradeEdge, tradeTypes[1]);
		}
	}
}


/*
*
* Structure method for determining profitability of triangular arbitrages 
* opportunties that are within a 4-way arbritrage
*
*/
void processCombinationBase3(Graph& g, double currProfit, Edge startEdge, double& maxProfit, 
						vector<TrackProfit>& negCyclePath, processCombinationInput profitVars, 
						vector<string> tradeTypes)
{
	for (Edge firstTradeEdge : g.adjacency_list[startEdge.to])
	{	
		if (firstTradeEdge.to == profitVars.source)
			continue;

		currProfit += tradeCostFromType(firstTradeEdge, tradeTypes[1]);

		for (Edge secondTradeEdge : g.adjacency_list[firstTradeEdge.to])
		{
			if (secondTradeEdge.to == profitVars.source)
				continue;

			currProfit += tradeCostFromType(secondTradeEdge, tradeTypes[2]);

			for (Edge thirdTradeEdge : g.adjacency_list[secondTradeEdge.to])
			{
				if (thirdTradeEdge.to == profitVars.source){
					currProfit += tradeCostFromType(thirdTradeEdge, tradeTypes[3]);
					// Lock maxProfit and negCyclePath
					if (maxProfitCheck(maxProfit, currProfit, profitVars.lowerBound, profitVars.upperBound))
					{
						vector<Edge> path {startEdge, firstTradeEdge, secondTradeEdge, thirdTradeEdge};
						updateMaxPath(negCyclePath, tradeTypes, path);
					}
					currProfit -= tradeCostFromType(thirdTradeEdge, tradeTypes[3]);
					break;
				}
			}

			currProfit -= tradeCostFromType(secondTradeEdge, tradeTypes[2]);
		}
		currProfit -= tradeCostFromType(firstTradeEdge, tradeTypes[1]);
	}
}


/*
*
* Strucutre method for determining profitability of four-trade arbitrages
* Extropliated into a function as at each trade the bid or ask price can be used
*
*/
void processCombinationLen4(Graph& g, double& maxProfit, vector<TrackProfit>& negCyclePath, processCombinationInput profitVars)
{
	double currProfit;
	vector<string> tradeTypes;
	combo2TradeType(tradeTypes, profitVars.combo);
	
	for (Edge firstTradeEdge : g.adjacency_list[profitVars.source]){	
		currProfit = tradeCostFromType(firstTradeEdge, tradeTypes[0]);
		
		// Process all 3-way arbirtrage searchs in another function
		processCombinationBase3(g, currProfit, firstTradeEdge, maxProfit, 
						negCyclePath, profitVars, tradeTypes);

	}
}




/*
* O(n^3) brute force algorithm for determing arbritage profitability
* Algorithm attempts to fill in this path with the most profitable trade
* source --> Coin1 --> Coin2 --> source
*  - Each --> represents a trade and at each the bid or ask price can be used
*
*/
vector<TrackProfit> ArbDetectCombo(Graph& g, string source, double lowerProfitThreshold, double upperProfitThreshold, int comboLen)
{
	double upperBound = log(upperProfitThreshold);
	double lowerBound = log(lowerProfitThreshold);
	double maxProfit = 0;
	vector<TrackProfit> negCyclePath;
	vector<string> tradeTypes { "BLANK", "BLANK", "BLANK"};


	// Current algorithm relies on the idea that for a given edge there is only travel on that edge
	// With the idea of bid-ask pricing, I have the choice at each egde to choose between buying the destination currency with the source currency
	// at the Ask price or selling the source currency for the destination currency at the Bid price

	// generate all bid-ask trade combinations of certain length that is parameter passed
	vector<string> combos;
	vector<char> bidOrAsk {'A', 'B'};
	generateCombinations(comboLen, "", combos, bidOrAsk);
	printVector(combos);

	/*
	*
	* Test every bid-ask trade combination generated above
	* Algorithm maximizes for profitability between upper and lower thresholds
	*
	*/
    vector<thread> threads;
	for (int i = 0; i < combos.size(); i++)
   	{
		processCombinationInput profitVars = {lowerBound, upperBound, source, combos[i]};
		if (comboLen == 3)
		{
			threads.push_back(thread(processCombinationLen3, ref(g), ref(maxProfit), ref(negCyclePath), ref(profitVars)));
		}
		else if (comboLen == 4)
		{
			threads.push_back(thread(processCombinationLen4, ref(g), ref(maxProfit), ref(negCyclePath), ref(profitVars)));
		}
  	}
    for (auto &thread : threads) {
        thread.join();
    }

	//TODO: Remove to another function that is used to validate the profitability of a series of trades
	if (maxProfit != 0) 
	{
		cout << "MaxProfit (log): " << maxProfit << endl;
		cout << "MaxProfit: " << (WeightConversion(maxProfit) - 1) * 100 << "%\n" << endl;
	}

	return negCyclePath;
}