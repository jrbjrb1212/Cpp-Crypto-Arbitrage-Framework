#pragma once

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

/*
*
* Struct designed for log data storage and arbitrage validation outside of inital discovery
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
	return exp(conversionMetric);
}


/*
*
* Helpful method that prints out the trades of an arbitrage opportunity
*
*/
void PrintCycle(vector<TrackProfit> cycleVec)
{
	cout << "Arbitrage Detected!" << endl;
	for (int i = 0; i < cycleVec.size() - 1; i++)
	{
		cout << cycleVec[i].from << " --> ";
	}

	cout << cycleVec[cycleVec.size() - 1].from << " --> " << cycleVec[cycleVec.size() - 1].to << endl;
}


/*
*
* Helpful method that prints out contents of a string vector
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
* Helpful method that prints out contents of a double vector
*
*/
void PrintVector(vector<double> &vec)
{
	for (double &num : vec)
	{
		cout << num << ", ";
	}
	cout << endl;
}


/*
*
* Helpful method that prints out contents of a hash set vector
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
* Helpful method that prints out contents of a hash map vector
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
* Print method for a checkpoint update on the arbitrage 
* find rate overall and since the last checkpoint
*
*/
void CheckPointInfo(int frameworkIterations, int positiveArbs, int &currIterations,  int &currArbsFound)
{
	if (frameworkIterations % 25 == 0)
	{
		cout << endl;
		cout << "Current iteration: " << frameworkIterations << endl;;
		cout << "\t-over last 25 iterations, arbitrage find rate:\t" << (double)currArbsFound/currIterations << "%" << endl;
		cout << "\t-overall arbitrage find rate:\t" << (double)positiveArbs/frameworkIterations<< "%" << endl;  
		cout << endl;
		currIterations=0;
		currArbsFound=0;
	}
}


/*
*
* Print method for determining the profitability 
* of a given arbitrage cycle detected
*
*/
void printArbProfitability(vector<TrackProfit> &arbPath, unordered_map<string, double> &feeMap)
{
	double currProfit = 0;
	int arbLen = arbPath.size();
	for (int i=0; i<arbPath.size(); i++){
		currProfit += arbPath[i].orderPrice + feeMap[arbPath[i].exchange];
	}
	cout << "MaxProfit: " << (WeightConversion(currProfit) - 1) * 100 << "% for " << arbLen << "-length path" << endl;
}



/*
*
* Determine the profit from 
* a given arbitrage cycle
*
*/
double arbPathMaxProfit(vector<TrackProfit> &arbPath, unordered_map<string, double> &feeMap)
{
	double profit = 0;
	for (int i = 0; i < arbPath.size(); i++)
	{
		profit += arbPath[i].orderPrice + log(1-feeMap[arbPath[i].exchange]);
	}
	profit = (WeightConversion(profit) - 1) * 100;
	return profit;
}


/*
*
* Print method for logging Arbitrage path vertex and edges
*
*/
void printArbEdgeInfo(Graph &g, vector<TrackProfit> &arbPath)
{
	for (int i = 0; i < arbPath.size(); i++)
	{
		g.printEdge(arbPath[i].to, arbPath[i].from, arbPath[i].exchange);
		g.printEdge(arbPath[i].from, arbPath[i].to, arbPath[i].exchange);
		cout << endl;
	}
}


/*
*
* Debugging print method for printing some stars
*
*/
void printStars()
{
	cout << "*****************" << endl;
}

void LogArbInfo(vector<TrackProfit> &arbPath, unordered_map<string, double> &feeMap, string startCoin, double idealAmountProfit)
{	
	if (arbPath.size() > 0)
	{
		cout << startCoin << "->";
		for (int i = 0; i < arbPath.size()-1; i++)
			cout << arbPath[i].to << "->";
		cout << startCoin;
		cout << ", tick_p=" << arbPathMaxProfit(arbPath, feeMap) << "%";
		cout << ", orbo_p=" << idealAmountProfit << "%" << endl;
	}
	else
	{
		cout << "no profitable path detected" << endl;
	}
}


/*
*
* Method for determining if current profit is the max profit
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
* Method to record information about the most profitable arbitrage path
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
* Band-aid struct created to reduce arguments required for arbitrage finding functions
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
* Base triangular arbitrage algorithm from ProcessLen3 extrapolated
* for parallel time improvement of 4 path arbitrages
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
* Base triangular arbitrage algorithm from ProcessLen3 extrapolated
* for parallel time improvement of 5 path arbitrages
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
* O(n^2/p * n^3) brute force algorithm for determining arbitrage profitability
* p is the number of available processors 
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
	
	return negCyclePath;
}