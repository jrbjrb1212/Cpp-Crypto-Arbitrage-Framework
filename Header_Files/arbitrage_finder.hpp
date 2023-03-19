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
* Heplful method that prints out contents of a double vector
*
*/
void PrintVector(const vector<double> &vec)
{
	for (const double &doub : vec)
	{
		cout << doub << ", ";
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
* Print method for log message of tracking profitability
*
*/
void printArbInfo(vector<TrackProfit> &arbPath, unordered_map<string, double> &feeMap)
{
	for(int i = 0; i < arbPath.size(); i++){
		cout << "From " << arbPath[i].from << " to " << arbPath[i].to;
		cout << " via " << arbPath[i].exchange << " using a " << arbPath[i].bidOrask;
		cout << " at " << WeightConversion(arbPath[i].orderPrice);
		cout << " with " << feeMap[arbPath[i].exchange] * 100 << "% fee" << endl;
	}
}


/*
*
* Method for determing if current profit is the max profit
*
*/
bool maxProfitCheck(double& maxProfit, double& currProfit, double& lowerThreshold, double& upperThreshold){
	if (currProfit > maxProfit && currProfit > lowerThreshold && currProfit < upperThreshold)
	{
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
// void makeTrackProfitStruct(TrackProfit& tradeStruct, Edge tradeInfo, string tradeType){
// 	if (tradeType == "ask")
// 	{
// 		tradeStruct.orderPrice = tradeInfo.bidPrice;
// 	}
// 	else if (tradeType == "bid")
// 	{
// 		tradeStruct.orderPrice = tradeInfo.askPrice;
// 	}
// }


/*
*
* Method to record information about the most proftiable arbritrage path
* Designed to be used for profit validation and order/trade amount optimization 
*
*/
void updateMaxPath(vector<TrackProfit>& negCyclePath, vector<Edge> trades)
{
	vector<TrackProfit> path (trades.size());
	
	// firstTrade is special case; each edge struct only contains the destination currency
	TrackProfit firstTrade {trades[trades.size()-1].to, trades[0].to, trades[0].exPrice, trades[0].bidOrAsk, trades[0].exchange};
	path[0] = firstTrade;

	// Add each trade edge to the cyclePath for later validation
	for(int i = 0; i < trades.size()-1; i++)
	{
		TrackProfit trade {trades[i].to, trades[i+1].to, trades[i+1].exPrice, trades[i+1].bidOrAsk, trades[i+1].exchange};
		path[i+1] = trade;
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
	unordered_map<string, vector<Edge> > adjacencyList = g.adjacencyList;

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
    

	for (Edge fristTradeEdge : adjacencyList[source]){
		currProfit += fristTradeEdge.weight;
		
		for (Edge secondTradeEdge : adjacencyList[fristTradeEdge.to]){
			if (secondTradeEdge.to == source){
				continue;
			}
			currProfit += secondTradeEdge.weight;

			for (Edge sourceTradeEdge : adjacencyList[secondTradeEdge.to]){
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

/*
*
* Implmenetation of a brute force O(n^3) algorithm 
* currency arrbritage opportunities with considerations for fees and spread
* TODO: Implement parallel version of this code to massively speed up code
* TODO: Implement a linear programming model that can solve faster asympotically
*/

// vector<TrackProfit> ArbDetect(Graph g, string source, double lowerProfitThreshold, double upperProfitThreshold)
// {
// 	unordered_map<string, vector<Edge> > adjacencyList = g.adjacencyList;

//     double upperBound = log(upperProfitThreshold);
//     double lowerBound = log(lowerProfitThreshold);
// 	double maxProfit = 0, currProfit;
// 	int negCycles = 0;
// 	int startCap = 1;
// 	vector<TrackProfit> negCyclePath;
// 	vector<string> tradeTypes { "BLANK", "BLANK", "BLANK"};

//     /*
//     *
//     * Algorithm attempts to fill in this path with the most profitable trade
//     * source --> Coin1 --> Coin2 --> source
//     * 
//     */

//    // Current algorithm relies on the idea that for a given edge there is only travel on that edge
//    // With the idea of bid-ask pricing, I have the choice at each egde to choose between buying the destination currency with the source currency
//    // at the Ask price or selling the source currency for the destination currency at the Bid price

// 	for (Edge firstTradeEdge : adjacencyList[source]){	
// 		// search through buying the destination currency from the current currency	

// 		// I am paying the askPrice price than I utilizing a buy
// 		// If I want to instantly buy a currency, I pay the ask price
// 		// If I want to instantly sell my currency, I pay the buy price
// 		currProfit = (firstTradeEdge.askPrice + firstTradeEdge.fee);
// 		tradeTypes[0] = "bid";
// 		for (Edge secondTradeEdge : adjacencyList[firstTradeEdge.to]){
// 			if (secondTradeEdge.to == source){
// 				continue;
// 			}

// 			// Search through buying the destination currency from the current currency
// 			currProfit += (secondTradeEdge.askPrice + secondTradeEdge.fee);
// 			tradeTypes[1] = "bid";
// 			for (Edge thirdTradeEdge : adjacencyList[secondTradeEdge.to]){
// 				if (thirdTradeEdge.to == source){
// 					// search through buying the destination currency from the current currency
// 					currProfit += (thirdTradeEdge.askPrice + thirdTradeEdge.fee);
// 					tradeTypes[2] = "bid";
// 					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
// 						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
// 					currProfit -= (thirdTradeEdge.askPrice + thirdTradeEdge.fee);

// 		 			// Search through selling the current currency for the destination currency
// 					currProfit += (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
// 					tradeTypes[2] = "ask";
// 					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
// 						continue;
// 						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
// 					currProfit -= (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
//                     break;
// 				}
// 			}
// 			currProfit -= (secondTradeEdge.askPrice + secondTradeEdge.fee);


// 		 	// Search through selling the current currency for the destination currency
// 			currProfit += (secondTradeEdge.bidPrice + secondTradeEdge.fee);
// 			tradeTypes[1] = "ask";
// 			for (Edge thirdTradeEdge : adjacencyList[secondTradeEdge.to]){
// 				if (thirdTradeEdge.to == source){
// 					// search through buying the destination currency from the current currency
// 					currProfit += (thirdTradeEdge.askPrice + thirdTradeEdge.fee);
// 					tradeTypes[2] = "bid";
// 					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
// 						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
// 					currProfit -= (thirdTradeEdge.askPrice + thirdTradeEdge.fee);

// 		 			// Search through selling the current currency for the destination currency
// 					currProfit += (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
// 					tradeTypes[2] = "ask";
// 					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
// 						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
// 					currProfit -= (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
//                     break;
// 				}
// 			}
// 			currProfit -= (secondTradeEdge.bidPrice + secondTradeEdge.fee);
// 		}

// 		// Search through selling the current currency for the destination currency
// 		currProfit = (firstTradeEdge.bidPrice + firstTradeEdge.fee);
// 		tradeTypes[0] = "ask";
// 		for (Edge secondTradeEdge : adjacencyList[firstTradeEdge.to]){
// 			if (secondTradeEdge.to == source){
// 				continue;
// 			}

// 			// Search through buying the destination currency from the current currency
// 			currProfit += (secondTradeEdge.askPrice + secondTradeEdge.fee);
// 			tradeTypes[1] = "bid";
// 			for (Edge thirdTradeEdge : adjacencyList[secondTradeEdge.to]){
// 				if (thirdTradeEdge.to == source){
// 					// search through buying the destination currency from the current currency
// 					currProfit += (thirdTradeEdge.askPrice + thirdTradeEdge.fee);
// 					tradeTypes[2] = "bid";
// 					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
// 						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
// 					currProfit -= (thirdTradeEdge.askPrice + thirdTradeEdge.fee);

// 		 			// Search through selling the current currency for the destination currency
// 					currProfit += (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
// 					tradeTypes[2] = "ask";
// 					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
// 						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
// 					currProfit -= (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
//                     break;
// 				}
// 			}
// 			currProfit -= (secondTradeEdge.askPrice + secondTradeEdge.fee);


// 		 	// Search through selling the current currency for the destination currency
// 			currProfit += (secondTradeEdge.bidPrice + secondTradeEdge.fee);
// 			tradeTypes[1] = "ask";
// 			for (Edge thirdTradeEdge : adjacencyList[secondTradeEdge.to]){
// 				if (thirdTradeEdge.to == source){
// 					// search through buying the destination currency from the current currency
// 					currProfit += (thirdTradeEdge.askPrice + thirdTradeEdge.fee);
// 					tradeTypes[2] = "bid";
// 					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
// 						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
// 					currProfit -= (thirdTradeEdge.askPrice + thirdTradeEdge.fee);

// 		 			// Search through selling the current currency for the destination currency
// 					currProfit += (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
// 					tradeTypes[2] = "ask";
// 					if (maxProfitCheck(maxProfit, currProfit, lowerBound, upperBound))
// 						// updateMaxPath(negCyclePath, tradeTypes, firstTradeEdge, secondTradeEdge, thirdTradeEdge);
// 					currProfit -= (thirdTradeEdge.bidPrice + thirdTradeEdge.fee);
//                     break;
// 				}
// 			}
// 			currProfit -= (secondTradeEdge.bidPrice + secondTradeEdge.fee);

// 		}

// 	}
// 	cout << "MaxProfit (log): " << maxProfit << endl;
//     cout << "MaxProfit: " << (WeightConversion(maxProfit) - 1) * 100 << "%\n" << endl;

// 	return negCyclePath;
// }


/*
*
* Method for determing tradetypes from a given combintion sequence
* Method turns "AAB" to {"ask", "ask", "bid"} 
*
*/
// void combo2TradeType(vector<string>& tradeTypes, string& combo)
// {
// 	for(int i = 0; i < combo.length(); i++)
// 	{
// 		if (combo[i] == 'A')
// 		{
// 			tradeTypes.push_back("ask");
// 		}
// 		else if (combo[i] == 'B')
// 		{
// 			tradeTypes.push_back("bid");
// 		}
// 	}
// }


/*
*
* Method uses information about the tradetype to determine trade cost
* Method determines that a buy trade will cost the askPrice + fees and vice versa 
*
*/
// double tradeCostFromType(Edge& trade, string& tradeType)
// {
// 	if (tradeType == "bid")
// 	{
// 		return trade.askPrice + trade.fee;
// 	}
// 	else if (tradeType == "ask")
// 	{
// 		return trade.bidPrice + trade.fee;
// 	}

// 	return -1.0;
// }




/*
*
* Strucutre method for determining profitability of triangular arbitrages
* Extropliated into a function as at each trade the bid or ask price can be used
*
*/
// void processCombinationLen3(Graph& g, double& maxProfit, vector<TrackProfit>& negCyclePath, processCombinationInput profitVars)
// {
// 	double currProfit;
// 	vector<string> tradeTypes;
// 	combo2TradeType(tradeTypes, profitVars.combo);

	
// 	for (Edge firstTradeEdge : g.adjacencyList[profitVars.source]){	
// 		currProfit = tradeCostFromType(firstTradeEdge, tradeTypes[0]);

// 		for (Edge secondTradeEdge : g.adjacencyList[firstTradeEdge.to]){
// 			if (secondTradeEdge.to == profitVars.source){
// 				continue;
// 			}

// 			currProfit += tradeCostFromType(secondTradeEdge, tradeTypes[1]);

// 			for (Edge thirdTradeEdge : g.adjacencyList[secondTradeEdge.to]){
// 				if (thirdTradeEdge.to == profitVars.source){
// 					currProfit += tradeCostFromType(thirdTradeEdge, tradeTypes[2]);
// 					// Lock maxProfit and negCyclePath
// 					if (maxProfitCheck(maxProfit, currProfit, profitVars.lowerBound, profitVars.upperBound))
// 					{
// 						vector<Edge> path {firstTradeEdge, secondTradeEdge, thirdTradeEdge};
// 						updateMaxPath(negCyclePath, tradeTypes, path);
// 					}
// 					currProfit -= tradeCostFromType(thirdTradeEdge, tradeTypes[2]);
// 					break;
// 				}
// 			}

// 			currProfit -= tradeCostFromType(secondTradeEdge, tradeTypes[1]);
// 		}
// 	}
// }


/*
*
* Structure method for determining profitability of triangular arbitrages 
* opportunties that are within a 4-way arbritrage
*
*/
// void processCombinationBase3(Graph& g, double currProfit, Edge startEdge, double& maxProfit, 
// 						vector<TrackProfit>& negCyclePath, processCombinationInput profitVars, 
// 						vector<string> tradeTypes)
// {
// 	for (Edge firstTradeEdge : g.adjacencyList[startEdge.to])
// 	{	
// 		if (firstTradeEdge.to == profitVars.source)
// 			continue;

// 		currProfit += tradeCostFromType(firstTradeEdge, tradeTypes[1]);

// 		for (Edge secondTradeEdge : g.adjacencyList[firstTradeEdge.to])
// 		{
// 			if (secondTradeEdge.to == profitVars.source)
// 				continue;

// 			currProfit += tradeCostFromType(secondTradeEdge, tradeTypes[2]);

// 			for (Edge thirdTradeEdge : g.adjacencyList[secondTradeEdge.to])
// 			{
// 				if (thirdTradeEdge.to == profitVars.source){
// 					currProfit += tradeCostFromType(thirdTradeEdge, tradeTypes[3]);
// 					// Lock maxProfit and negCyclePath
// 					if (maxProfitCheck(maxProfit, currProfit, profitVars.lowerBound, profitVars.upperBound))
// 					{
// 						vector<Edge> path {startEdge, firstTradeEdge, secondTradeEdge, thirdTradeEdge};
// 						updateMaxPath(negCyclePath, tradeTypes, path);
// 					}
// 					currProfit -= tradeCostFromType(thirdTradeEdge, tradeTypes[3]);
// 					break;
// 				}
// 			}

// 			currProfit -= tradeCostFromType(secondTradeEdge, tradeTypes[2]);
// 		}
// 		currProfit -= tradeCostFromType(firstTradeEdge, tradeTypes[1]);
// 	}
// }


/*
*
* Strucutre method for determining profitability of four-trade arbitrages
* Extropliated into a function as at each trade the bid or ask price can be used
*
*/
// void processCombinationLen4(Graph& g, double& maxProfit, vector<TrackProfit>& negCyclePath, processCombinationInput profitVars)
// {
// 	double currProfit;
// 	vector<string> tradeTypes;
// 	combo2TradeType(tradeTypes, profitVars.combo);
	
// 	for (Edge firstTradeEdge : g.adjacencyList[profitVars.source]){	
// 		currProfit = tradeCostFromType(firstTradeEdge, tradeTypes[0]);
		
// 		// Process all 3-way arbirtrage searchs in another function
// 		processCombinationBase3(g, currProfit, firstTradeEdge, maxProfit, 
// 						negCyclePath, profitVars, tradeTypes);

// 	}
// }


/*
*
* Band-aid struct created to reduce arguments required for arbirtrage finding functions
*
*/
struct processInput{
	double lowerBound;
	double upperBound;
	double &maxProfit;
	int arbLen;
	string source;
};


/*
*
* Algorithm attempts to fill in this path with the most profitable trade
* SourceCoin --> Coin1 --> SourceCoin
* 
*/
void ProcessLen2(Graph &g, vector<TrackProfit> &negCyclePath, processInput inputVars)
{
	double currProfit;
	/*
    *
    * Algorithm attempts to fill in this path with the most profitable trade
    * SourceCoin --> Coin1 --> SourceCoin
    * 
    */
	for (Edge firstTradeEdge : g.adjacencyList[inputVars.source])
	{	
		// first trade cost
		currProfit = (firstTradeEdge.exPrice + firstTradeEdge.fee);

		// second trade
		for (Edge secondTradeEdge : g.adjacencyList[firstTradeEdge.to])
		{
			// all arb paths must start and end at the source currencies
			if (secondTradeEdge.to == inputVars.source)
			{
				currProfit += (secondTradeEdge.exPrice + secondTradeEdge.fee);
									
				// need to do a max profit check
				if (maxProfitCheck(inputVars.maxProfit, currProfit, inputVars.lowerBound, inputVars.upperBound))
				{
					vector<Edge> path {firstTradeEdge, secondTradeEdge};
					updateMaxPath(negCyclePath, path);
				}
				break;
			}
		}
	} 
}


/*
*
* Algorithm attempts to fill in this path with the most profitable trade
* SourceCoin --> Coin1 --> Coin2 --> SourceCoin
* 
*/
void ProcessLen3(Graph &g, vector<TrackProfit> &negCyclePath, processInput inputVars)
{
	double currProfit;

	// brute force combinations to maximize profitability
	for (Edge firstTradeEdge : g.adjacencyList[inputVars.source])
	{	
		if (firstTradeEdge.to == inputVars.source)
			continue;
		currProfit += (firstTradeEdge.exPrice + firstTradeEdge.fee);

		for (Edge secondTradeEdge : g.adjacencyList[firstTradeEdge.to])
		{
			if (secondTradeEdge.to == inputVars.source)
				continue;
			currProfit += (secondTradeEdge.exPrice + secondTradeEdge.fee);

			for (Edge thirdTradeEdge : g.adjacencyList[secondTradeEdge.to])
			{
				// all arb paths must start and end at the source currencies
				if (thirdTradeEdge.to == inputVars.source)
				{
					currProfit += (thirdTradeEdge.exPrice + thirdTradeEdge.fee);
					// need to do a max profit check
					if (maxProfitCheck(inputVars.maxProfit, currProfit, inputVars.lowerBound, inputVars.upperBound))
					{
						vector<Edge> path {firstTradeEdge, secondTradeEdge, thirdTradeEdge};
						updateMaxPath(negCyclePath, path);
					}
					currProfit -= (thirdTradeEdge.exPrice + thirdTradeEdge.fee);
					break;
				}
			}
			currProfit -= (secondTradeEdge.exPrice + secondTradeEdge.fee);
		}
		currProfit -= (firstTradeEdge.exPrice + firstTradeEdge.fee);
	} 
}


/*
*
* Base triangular arbirtarge algorithm from ProcessLen3 extrolated
* for parallel time improvment of 4 path arbirtrages
* 
*/
void ProcessBase3For4(Graph &g, vector<TrackProfit> &negCyclePath, processInput inputVars, 
                      Edge firstTradeEdge, double currProfit, mutex &negCyclePath_mutex)
{
    for (Edge secondTradeEdge : g.adjacencyList[firstTradeEdge.to])
    {   
        if (secondTradeEdge.to == inputVars.source)
            continue;
        currProfit += (secondTradeEdge.exPrice + secondTradeEdge.fee);

        for (Edge thirdTradeEdge : g.adjacencyList[secondTradeEdge.to])
        {
            if (thirdTradeEdge.to == inputVars.source)
                continue;
            currProfit += (thirdTradeEdge.exPrice + thirdTradeEdge.fee);

            for (Edge fourthTradeEdge : g.adjacencyList[thirdTradeEdge.to])
            {
                // all arb paths must start and end at the source currencies
                if (fourthTradeEdge.to == inputVars.source)
                {
                    currProfit += (fourthTradeEdge.exPrice + fourthTradeEdge.fee);
                    // need to do a max profit check
                    if (maxProfitCheck(inputVars.maxProfit, currProfit, inputVars.lowerBound, inputVars.upperBound))
                    {
                        vector<Edge> path {firstTradeEdge, secondTradeEdge, thirdTradeEdge, fourthTradeEdge};
                        negCyclePath_mutex.lock();
                        updateMaxPath(negCyclePath, path);
                        negCyclePath_mutex.unlock();
                    }
                    currProfit -= (fourthTradeEdge.exPrice + fourthTradeEdge.fee);
                    break;
                }
            }
            currProfit -= (thirdTradeEdge.exPrice + thirdTradeEdge.fee);
        }
        currProfit -= (secondTradeEdge.exPrice + secondTradeEdge.fee);
    } 
}


/*
*
* Base triangular arbirtarge algorithm from ProcessLen3 extrolated
* for parallel time improvment of 5 path arbirtrages
* 
*/
void ProcessBase3For5(Graph &g, vector<TrackProfit> &negCyclePath, processInput inputVars, 
					  Edge firstTradeEdge, Edge secondTradeEdge, double currProfit, mutex &negCyclePath_mutex)
{
	for (Edge thirdTradeEdge : g.adjacencyList[secondTradeEdge.to])
	{	
		if (thirdTradeEdge.to == inputVars.source)
			continue;
		currProfit += (thirdTradeEdge.exPrice + thirdTradeEdge.fee);

		for (Edge fourthTradeEdge : g.adjacencyList[thirdTradeEdge.to])
		{
			if (fourthTradeEdge.to == inputVars.source)
				continue;
			currProfit += (fourthTradeEdge.exPrice + fourthTradeEdge.fee);

			for (Edge fifthTradeEdge : g.adjacencyList[fourthTradeEdge.to])
			{
				// all arb paths must start and end at the source currencies
				if (fifthTradeEdge.to == inputVars.source)
				{
					currProfit += (fifthTradeEdge.exPrice + fifthTradeEdge.fee);
					// need to do a max profit check
					if (maxProfitCheck(inputVars.maxProfit, currProfit, inputVars.lowerBound, inputVars.upperBound))
					{
						vector<Edge> path {firstTradeEdge, secondTradeEdge, thirdTradeEdge, fourthTradeEdge, fifthTradeEdge};
						negCyclePath_mutex.lock();
						updateMaxPath(negCyclePath, path);
						negCyclePath_mutex.unlock();
					}
					currProfit -= (fifthTradeEdge.exPrice + fifthTradeEdge.fee);
					break;
				}
			}
			currProfit -= (fourthTradeEdge.exPrice + fourthTradeEdge.fee);
		}
		currProfit -= (thirdTradeEdge.exPrice + thirdTradeEdge.fee);
	} 
}


/*
*
* Algorithm attempts to fill in this path with the most profitable trade
* SourceCoin --> Coin1 --> Coin2 --> Coin3 --> SourceCoin
* 
*/
void ProcessLen4(Graph &g, vector<TrackProfit> &negCyclePath, processInput inputVars)
{
    double currProfit;
    vector<thread> threads;
    mutex negCyclePath_mutex;

    // brute force combinations to maximize profitability
    for (Edge firstTradeEdge : g.adjacencyList[inputVars.source])
    {
        currProfit = (firstTradeEdge.exPrice + firstTradeEdge.fee);
        threads.push_back(thread(ProcessBase3For4, 
                                ref(g), ref(negCyclePath), ref(inputVars), 
                                ref(firstTradeEdge), ref(currProfit),
                                ref(negCyclePath_mutex)));
    }
    for (auto &thread : threads) {
        thread.join();
    }
}


/*
*
* Algorithm attempts to fill in this path with the most profitable trade
* SourceCoin --> Coin1 --> Coin2 --> Coin3 --> Coin4 --> SourceCoin
* 
*/
void ProcessLen5(Graph &g, vector<TrackProfit> &negCyclePath, processInput inputVars)
{
	double currProfit;
	mutex negCyclePath_mutex;
	vector<thread> threads;
	for (Edge firstTradeEdge : g.adjacencyList[inputVars.source])
	{
		currProfit = (firstTradeEdge.exPrice + firstTradeEdge.fee);

		for (Edge secondTradeEdge : g.adjacencyList[firstTradeEdge.to])
		{
			currProfit += (secondTradeEdge.exPrice + secondTradeEdge.fee);

			threads.push_back(thread(ProcessBase3For5, ref(g), ref(negCyclePath), 
									ref(inputVars), ref(firstTradeEdge), ref(secondTradeEdge), 
									ref(currProfit), ref(negCyclePath_mutex)));

			currProfit -= (secondTradeEdge.exPrice + secondTradeEdge.fee);
		}
	}
	for (thread &thread : threads) {
        thread.join();
    }
}



void ArbDetectControl(Graph &g, vector<TrackProfit> &negCyclePath, processInput inputVars)
{
	double currProfit = 0;
	if (inputVars.arbLen > 3)
	{
		if (inputVars.arbLen == 4)
		{
			ProcessLen4(g, negCyclePath, inputVars);
		}
		else if (inputVars.arbLen == 5)
		{
			ProcessLen5(g, negCyclePath, inputVars);
		}
	}

	if (inputVars.arbLen == 3)
	{
		ProcessLen3(g, negCyclePath, inputVars);
	}
	else if (inputVars.arbLen == 2)
	{
		ProcessLen2(g, negCyclePath, inputVars);
	}

}


/*
* O(n^k) brute force algorithm for determing arbritage profitability
* k is the length of the arb path requested by user
* Algorithm attempts to fill in this path with the most profitable trade
* source --> Coin1 --> Coin2 --> ... --> source
*  - Each --> represents a trade and at each the bid or ask price can be used
*
*/
vector<TrackProfit> ArbDetect(Graph& g, string source, double lowerProfitThreshold, double upperProfitThreshold, int arbLen)
{
	double upperBound = log(upperProfitThreshold);
	double lowerBound = log(lowerProfitThreshold);
	double maxProfit = 0;
	vector<TrackProfit> negCyclePath;

	processInput arbFindVars = {lowerBound, upperBound, maxProfit, arbLen, source};
	ArbDetectControl(g, negCyclePath, arbFindVars);

	//TODO: Remove to another function that is used to validate the profitability of a series of trades
	if (maxProfit != 0) 
	{
		cout << "MaxProfit: " << (WeightConversion(maxProfit) - 1) * 100 << "% for " << arbLen << "-length path" << endl;
	}
	
	return negCyclePath;
}