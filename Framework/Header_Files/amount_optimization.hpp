#pragma once

#include "graph.hpp"
#include "arbitrage_finder.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>

/*
*
* Algorithm for searching through most recent orderbook information
* and determining profitability from amount of active liquidity with
* consideration for prices beyond bid and ask
*
*/
double orderBookProfit(vector<TrackProfit> &arbPath, vector<vector<vector<double>>> &orderBookData, 
						vector<vector<string>> &orderBookSides, double startCapital, int nDepth, 
						unordered_map<string, double> &feeMap)
{
	double activeTradeAmt = startCapital;
	for (int i = 0; i < arbPath.size(); i++)
	{
		TrackProfit currProfitItem = arbPath[i];
		double conversionAmt = 0;

		// In a must sell situation with the trading pair
		// if we are selling we must look at the bid side of the table
		if (currProfitItem.from == orderBookSides[i][0])
		{
			// looking to sell x units of a coin to a buyer who wants to buy y units of a the same coin
			// the buyer in this situation is willing to give me another coin as payment

			// iterating over buy side book data
			for (int j = 0; j < nDepth; j++)
			{
				double unitsCouldBuy = activeTradeAmt;
				double unitsAvail = orderBookData[i][1][j];
				double tradeSize = unitsCouldBuy - unitsAvail;

				// more units available on current bid order than I could buy
				if (tradeSize <= 0)
				{
					conversionAmt += unitsCouldBuy * exp(orderBookData[i][0][j]);
					activeTradeAmt = 0;
					break;
				}

				// less units available on the current bid order than I could buy
				else
				{
					conversionAmt += unitsAvail * exp(orderBookData[i][0][j]);
					activeTradeAmt -= unitsAvail;
				}

			}
			if (activeTradeAmt != 0)
			{
				return -1.0;
			}
		}

		// In a must buy situation with the trading pair
		// if we are buying we must look at the bid side of the table
		else
		{
			// looking to buy x units of a coin from a buyer who wants to sell y units of a the same coin
			// the seller in this situation is willing to give me there coin through payment of another coin

			// iterating over sell side book data
			for (int j = 0; j < nDepth; j++)
			{

				double unitsCouldBuy = activeTradeAmt * (1/exp(orderBookData[i][2][j]));
				double unitsAvail = orderBookData[i][3][j];
				double tradeSize = unitsCouldBuy - unitsAvail;

				if (tradeSize <= 0.0)
				{
					conversionAmt += unitsCouldBuy;
					activeTradeAmt = 0;
					break;
				}

				else
				{
					conversionAmt += unitsAvail;
					activeTradeAmt -= unitsAvail * exp(orderBookData[i][2][j]);
				}
			}
			if (activeTradeAmt != 0)
			{
				return -1.0;
			}
		}
		activeTradeAmt = (conversionAmt * (1-feeMap[arbPath[i].exchange]));
	}
	return activeTradeAmt;
}


/*
*
* method to check if any of the order books are 
* empty implying a closed order book
* if they are closed, delete that from the graph
*/
void CheckClosedOrderBooks(vector<vector<vector<double>>> &orderBookData, Graph &g, vector<TrackProfit> &arbPath)
{
	int d1 = orderBookData.size();
	for (int i = 0; i < d1; i++)
	{
		if ((orderBookData[i][1][0] == 0.0) || (orderBookData[i][3][0] == 0.0))
		{
			g.deleteEdge(arbPath[i].from, arbPath[i].to, arbPath[i].exchange);
		}
	}
}


int DetermineMaxOrderBookBuckets(vector<vector<vector<double>>> &orderBookData)
{
	int maxBuckets = INT_MAX, orderBookOrders;
	int d1 = orderBookData.size();
	for (int i = 0; i < d1; i++)
	{
		orderBookOrders = orderBookData[i][0].size();
		maxBuckets = min(maxBuckets, orderBookOrders);
	}
	return maxBuckets;
}


/*
*
* Control flow method for the user choice of
* only executing the main arbitrage method.
* - Bare bones option that has lightweight logging 
*
*/
double amountOptControlMain(Graph &g, vector<TrackProfit> &arbPath, int nDepth, unordered_map<string, double> &feeMap, double minAmt)
{
	// Need to obtain orderbook info for each trade in the arbitrage path
	int d1 = arbPath.size();
	// Each orderbook will provide bid prices, bid sizes, ask prices, ask sizes
	int d2 = 4;
	// Amount of orderbook orders to track
	int d3 = nDepth;
	vector<vector<vector<double>>> orderBookData(d1,vector<vector<double>>(d2,vector<double>(d3)));
	vector<vector<string>> orderBookSides(d1, vector<string>(2));

	pullAllOrderBook(arbPath, orderBookData, orderBookSides, nDepth);
	CheckClosedOrderBooks(orderBookData, g, arbPath);

	int maxBuckets = DetermineMaxOrderBookBuckets(orderBookData);
	double returnOnInvestment, capAfterTrades, maxProfitability=0.0;
	
	vector<double> startCaps {minAmt, minAmt * 1.25, minAmt * 1.5, minAmt * 1.75, minAmt * 2.0, minAmt * 2.5, minAmt * 3.0, minAmt * 4, minAmt * 5.0, minAmt * 10};

	// Check possible starting capital options for profitability of different levels of a starting trade
	for (double startCap : startCaps)
	{
		capAfterTrades = orderBookProfit(arbPath, orderBookData, orderBookSides, startCap, maxBuckets, feeMap);
		returnOnInvestment = (capAfterTrades/startCap - 1); 
		if (capAfterTrades == -1)
			break;
		maxProfitability = max(maxProfitability, returnOnInvestment);
	}
	return maxProfitability * 100;
}


/*
*
* Control flow method for the user choice of
* benchmarking code 
* - In addition to performing normal ideal amount,
*   the code records time it takes to do operations
*
*/
double amountOptControlTime(Graph &g, vector<TrackProfit> &arbPath, int nDepth, unordered_map<string, double> &feeMap, double minAmt, vector<double> &times)
{
	/// Need to obtain orderbook info for each trade in the arbitrage path
	int d1 = arbPath.size();
	// Each orderbook will provide bid prices, bid sizes, ask prices, ask sizes
	int d2 = 4;
	// Amount of orderbook orders to track
	int d3 = nDepth;
	vector<vector<vector<double>>> orderBookData(d1,vector<vector<double>>(d2,vector<double>(d3)));
	vector<vector<string>> orderBookSides(d1, vector<string>(2));

	auto start = high_resolution_clock::now();
	pullAllOrderBook(arbPath, orderBookData, orderBookSides, nDepth);
	auto end = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start);
	times[2] = (duration.count());

	start = high_resolution_clock::now();
	
	CheckClosedOrderBooks(orderBookData, g, arbPath);

	int maxBuckets = DetermineMaxOrderBookBuckets(orderBookData);
	double returnOnInvestment, capAfterTrades, maxProfitability=0.0;
	vector<double> startCaps {minAmt, minAmt * 1.25, minAmt * 1.5, minAmt * 1.75, minAmt * 2.0, minAmt * 2.5, minAmt * 3.0, minAmt * 4, minAmt * 5.0, minAmt * 10};

	for (double startCap : startCaps)
	{
		capAfterTrades = orderBookProfit(arbPath, orderBookData, orderBookSides, startCap, maxBuckets, feeMap);
		returnOnInvestment = (capAfterTrades/startCap - 1); 
		if (capAfterTrades == -1)
			break;
		maxProfitability = max(maxProfitability, returnOnInvestment);
	}

	end = high_resolution_clock::now();
	duration = duration_cast<milliseconds>(end - start);
	times[3] = (duration.count() / 1e-3);
	return maxProfitability;
}


/*
*
* Control flow method for the user choice of
* debugging code
* - Has all bare bones features of ideal algo
* - Additionally has tons of print out information
*
*/
void amountOptControlDebug(Graph &g, vector<TrackProfit> &arbPath, int nDepth, unordered_map<string, double> &feeMap, double minAmt)
{
	// Need to obtain orderbook info for each trade in the arbitrage path
	int d1 = arbPath.size();
	// Each orderbook will provide bid prices, bid sizes, ask prices, ask sizes
	int d2 = 4;
	// Amount of orderbook orders to track
	int d3 = nDepth;
	vector<vector<vector<double>>> orderBookData(d1,vector<vector<double>>(d2,vector<double>(d3)));
	vector<vector<string>> orderBookSides(d1, vector<string>(2));

	cout << endl;
	pullAllOrderBook(arbPath, orderBookData, orderBookSides, nDepth);
	for (int i = 0; i < d1; i++)
	{
		g.printEdge(arbPath[i].from, arbPath[i].to, arbPath[i].exchange);
		cout << "Amounts Unit: " <<  orderBookSides[i][0] << " Price Unit: " << orderBookSides[i][1] << endl;
		cout << "Bids: ";
		PrintVector(orderBookData[i][0]);
		cout << "Bids size: ";
		PrintVector(orderBookData[i][1]);
		cout << "Asks: ";
		PrintVector(orderBookData[i][2]);
		cout << "Asks size: ";
		PrintVector(orderBookData[i][3]);
		cout << endl;
	}

	CheckClosedOrderBooks(orderBookData, g, arbPath);

	int maxBuckets = DetermineMaxOrderBookBuckets(orderBookData);
	double returnOnInvestment, capAfterTrades, maxProfitability=0.0;
	vector<int> viableAmts;
	vector<double> startCaps {minAmt, minAmt * 1.25, minAmt * 1.5, minAmt * 1.75, minAmt * 2.0, minAmt * 2.5, minAmt * 3.0, minAmt * 4, minAmt * 5.0, minAmt * 10};

	// Check possible starting capital options for profitability of different levels of a starting trade
	for (double startCap : startCaps)
	{
		capAfterTrades = orderBookProfit(arbPath, orderBookData, orderBookSides, startCap, maxBuckets, feeMap);
		if (capAfterTrades == -1)
			break;
		returnOnInvestment = (capAfterTrades/startCap - 1); 
		if (returnOnInvestment > 0){
			viableAmts.push_back(startCap);
		}
		maxProfitability = max(maxProfitability, returnOnInvestment);
		
		cout << "Start capital: " << startCap << " " << arbPath[0].from;
		cout << ", Profitability: " << capAfterTrades - startCap << " ";
		cout << arbPath[0].from << ", currROI: " << WeightConversion(returnOnInvestment)-1 << "%" << endl;
	}

	// Print the amounts that produce positive profit
	if (viableAmts.size() > 0)
	{
		cout << "Viable Amounts: ";
		for (int i=0; i<viableAmts.size(); i++)
		{
			if (i == viableAmts.size()-1)
				cout << viableAmts[i] << " " << arbPath[0].from << endl;
			else
				cout << viableAmts[i] << ", ";
		}
	}
	else
	{
		cout << "No Viable Trades Detected" << endl;
	}
}
