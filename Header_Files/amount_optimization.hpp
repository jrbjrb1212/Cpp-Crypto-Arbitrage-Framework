#pragma once

#include <vector>
#include <unordered_map>
#include "graph.hpp"
#include <algorithm>
#include <chrono>

// TODO: Comment the functions here and possibly extrapolate some methods

/*
*
* Algorithm for searching through most recent orderbook information
* and determining profitability from amount of active liquidity with
* consideration for prices beyond bid and ask
*
*/
double orderBookProfit(Graph &g, vector<TrackProfit> &arbPath, vector<vector<vector<double>>> orderBookData, vector< vector<string>> &orderBookSides, double startCapital, int nDepth, unordered_map<string, double> &feeMap)
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
			
			cout << "Trade: " << arbPath[i].from << arbPath[i].to;
			cout << " Start Capital: " << activeTradeAmt << endl;
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
					cout << "activeTradeAmt: " << activeTradeAmt << " cost: " << exp(orderBookData[i][0][j]) << " lot size: " << orderBookData[i][3][j] << endl;
					break;
				}

				// less units available on the current bid order than I could buy
				else
				{
					conversionAmt += unitsAvail * exp(orderBookData[i][0][j]);
					activeTradeAmt -= unitsAvail;
					cout << "activeTradeAmt: " << activeTradeAmt << " cost: " << exp(orderBookData[i][0][j]) << " lot size: " << orderBookData[i][3][j] << endl; 
				}

			}
			if (activeTradeAmt != 0)
			{
				cout << "Order size was not fillable by orderbook; Illiquid market detected" << endl;
				return -1.0;
			}
			cout << "End Capital: " << conversionAmt << " " << arbPath[i].to << endl;
		}

		// In a must buy situation with the trading pair
		// if we are buying we must look at the bid side of the table
		else
		{
			cout << "Trade: " << arbPath[i].from << arbPath[i].to;
			cout << " Start Capital: " << activeTradeAmt << endl;
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
					cout << "activeTradeAmt: " << activeTradeAmt << " cost: " << (1/exp(orderBookData[i][2][j])) << " lot size: " << orderBookData[i][1][j] << endl;
					break;
				}

				else
				{
					conversionAmt += unitsAvail;
					activeTradeAmt -= unitsAvail * exp(orderBookData[i][2][j]);
					cout << "activeTradeAmt: " << activeTradeAmt << " cost: " << exp(orderBookData[i][2][j]) << " lot size: " << orderBookData[i][1][j] << endl;
				}
			}
			if (activeTradeAmt != 0)
			{
				cout << "Order size was not fillable by orderbook; Illiquid market detected" << endl;
				return -1.0;
			}
			cout << "End Capital: " << conversionAmt << " " << arbPath[i].to << endl;
		}

		activeTradeAmt = (conversionAmt * (1-feeMap[arbPath[i].exchange]));
	}


	return activeTradeAmt;
}


/*
*
* Algorithm for searching through most recent orderbook information
* and determining profitability from amount of active liquidity with
* consideration for prices beyond bid and ask
*
*/
double orderBookProfit(vector<TrackProfit> &arbPath, vector<vector<vector<double>>> orderBookData, 
						vector< vector<string>> &orderBookSides, double startCapital, int nDepth, 
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


void amountOptControl(Graph &g, vector<TrackProfit> &arbPath, int nDepth, unordered_map<string, double> &feeMap, double minAmt, vector<double> &times)
{
	//TODO: Explain the dimensions
	int d1 = arbPath.size(), d2 = 4, d3 = nDepth;
	vector<vector<vector<double>>> orderBookData(d1,vector<vector<double>>(d2,vector<double>(d3)));
	vector<vector<string>> orderBookSides(d1, vector<string>(2));

	auto start = high_resolution_clock::now();
	pullAllOrderBook(arbPath, orderBookData, orderBookSides, nDepth);
	auto end = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(end - start);
	times[2] = (duration / 1e-3s);

	start = high_resolution_clock::now();
	// check if any of the order books are empty
	// implying a closed order book
	// if they are closed, delete that edge from graph
	for (int i = 0; i < d1; i++)
	{
		if ((orderBookData[i][1][0] == 0.0) || (orderBookData[i][3][0] == 0.0))
		{
			g.deleteEdge(arbPath[i].from, arbPath[i].to, arbPath[i].exchange);
		}
	}


	int maxBuckets = nDepth, orderBookOrders;
	double returnOnInvestment, profitability=0.0, bestStart=-1.0;
	for (int i = 0; i < d1; i++)
	{
		orderBookOrders = orderBookData[i][0].size();
		maxBuckets = min(maxBuckets, orderBookOrders);
	}
	vector<double> startCaps {minAmt, minAmt * 1.25, minAmt * 1.5, minAmt * 1.75, minAmt * 2.0, minAmt * 2.5, minAmt * 3.0, minAmt * 4, minAmt * 5.0, minAmt * 10};


	for (double startCap : startCaps)
	{
		// TODO: If I ever get a -1, I can exit the loop as I know no further (higher) startCapital will ]
		// have enough liquidity
		returnOnInvestment = orderBookProfit(arbPath, orderBookData, orderBookSides, startCap, maxBuckets, feeMap);
		if (returnOnInvestment == -1)
			break;
		returnOnInvestment = (returnOnInvestment/startCap - 1); 

		if (profitability == 0.0)
		{
			profitability = returnOnInvestment;
			bestStart = startCap;
		}
		else if (profitability - returnOnInvestment < 0.001)
		{
			profitability = returnOnInvestment;
			bestStart = startCap;
		}
	}

	end = high_resolution_clock::now();
	duration = duration_cast<microseconds>(end - start);
	times[3] = (duration / 1e-3s);
}


/*
*
* Driving method for pulling active orderbook information for all trades
* within the determined arbitrage path and then control program path flow
* in calculating ideal trade amount 
*
*/
void amountOptControl(Graph &g, vector<TrackProfit> &arbPath, int nDepth, unordered_map<string, double> &feeMap, double minAmt)
{
	//TODO: Explain the dimensions
	int d1 = arbPath.size(), d2 = 4, d3 = nDepth;
	vector<vector<vector<double>>> orderBookData(d1,vector<vector<double>>(d2,vector<double>(d3)));
	vector<vector<string>> orderBookSides(d1, vector<string>(2));

	cout << endl;
	pullAllOrderBook(arbPath, orderBookData, orderBookSides, nDepth);
	for (int i = 0; i < d1; i++)
	{
		g.printEdge(arbPath[i].from, arbPath[i].to, arbPath[i].exchange);
		cout << "Amounts Unit: " <<  orderBookSides[i][0] << " Price Unit: " << orderBookSides[i][1] << endl;
		PrintVector(orderBookData[i][0]);
		PrintVector(orderBookData[i][1]);
		PrintVector(orderBookData[i][2]);
		PrintVector(orderBookData[i][3]);
		cout << endl;
	}

	// check if any of the order books are empty
	// implying a closed order book
	// if they are closed, delete that 
	for (int i = 0; i < d1; i++)
	{
		if ((orderBookData[i][1][0] == 0.0) || (orderBookData[i][3][0] == 0.0))
		{
			g.deleteEdge(arbPath[i].from, arbPath[i].to, arbPath[i].exchange);
		}
	}


	int maxBuckets = nDepth, orderBookOrders;
	double returnOnInvestment, profitability=0.0, bestStart=-1.0;
	for (int i = 0; i < d1; i++)
	{
		orderBookOrders = orderBookData[i][0].size();
		maxBuckets = min(maxBuckets, orderBookOrders);
	}
	vector<double> startCaps {minAmt, minAmt * 1.25, minAmt * 1.5, minAmt * 1.75, minAmt * 2.0, minAmt * 2.5, minAmt * 3.0, minAmt * 4, minAmt * 5.0, minAmt * 10};


	for (double startCap : startCaps)
	{
		// TODO: If I ever get a -1, I can exit the loop as I know no further (higher) startCapital will ]
		// have enough liquidity
		returnOnInvestment = orderBookProfit(arbPath, orderBookData, orderBookSides, startCap, maxBuckets, feeMap);
		if (returnOnInvestment == -1)
			break;
		returnOnInvestment = (returnOnInvestment/startCap - 1); 

		if (profitability == 0.0)
		{
			profitability = returnOnInvestment;
			bestStart = startCap;
		}
		else if (profitability - returnOnInvestment < 0.001)
		{
			profitability = returnOnInvestment;
			bestStart = startCap;
		}
		cout << "Profitability: " << profitability << " currROI: " << returnOnInvestment << " bestStartAmt: " << bestStart; 
	}

}
