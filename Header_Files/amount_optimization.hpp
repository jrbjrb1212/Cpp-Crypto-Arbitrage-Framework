#pragma once

#include <vector>
#include <unordered_map>
#include "graph.hpp"
#include <algorithm>

/*
*
* Algorithm for searching through most recent orderbook information
* and determining profitability from amount of active liquidity with
* consideration for prices beyond bid and ask
*
*/
//TODO: currently an issue with how I am considered orderbook amounts
double actualProfitOrderBook(Graph &g, vector<TrackProfit> &arbPath, vector<vector<vector<double>>> orderBookData, double startCapital, int nDepth, unordered_map<string, double> &feeMap)
{
    double activeTradeAmt = startCapital;

    // iterate over each trade in arbPath
    for (int i = 0; i < arbPath.size(); i++)
    {
        // at each trade in arbPath
        // want to dive into the connected orderbook 
        // and continuously try to get the best price at either the 
        double conversionCapAmt = 0;
        
        // If a trade is an ask, it is really a bid
        // Need to pay askPrice so look through askPrices and askAmounts
        if (arbPath[i].bidOrask == "ask")
        {
            cout << "Trade: " << arbPath[i].from << arbPath[i].to;
            cout << " Start Capital: " << activeTradeAmt << endl;
            g.printEdge(arbPath[i].from, arbPath[i].to, arbPath[i].exchange);
            
            for (int j = 0; j < nDepth; j++)
            {
                double unitsCouldBuy = activeTradeAmt / (exp(orderBookData[i][2][j]));
                double unitsAvail = orderBookData[i][3][j];
                double tradeSize = unitsCouldBuy - unitsAvail;
                // can buy all requested units with current orderBookPrice
                if (tradeSize <= 0)
                {
                    // edit some info about activeTradeAmt
                    conversionCapAmt += unitsCouldBuy;
                    activeTradeAmt = 0;
                    cout << "activeTradeAmt: " << activeTradeAmt << " cost: " << exp(orderBookData[i][2][j]) << " lot size: " << orderBookData[i][3][j] << endl;
                    break;
                }

                // cannot buy all requested units with current orderBookPrice
                // reduce activeTradeAmt by the cost of purchasing all units
                else
                {
                    conversionCapAmt += orderBookData[i][3][j];
                    activeTradeAmt = tradeSize * (exp(orderBookData[i][2][j]));
                    cout << "activeTradeAmt: " << activeTradeAmt << " cost: " << exp(orderBookData[i][2][j]) << " lot size: " << orderBookData[i][3][j] << endl; 
                }
            }
            if (activeTradeAmt != 0)
            {
                cout << "Order size was not fillable by orderbook; Illiquid market detected" << endl;
                return -1.0;
            }
            cout << "End Capital: " << conversionCapAmt << endl;
        }
        
        // If a trade is an bid, it is really a ask
        // Need to pay bidPrice so look through bidPrices and bidAmounts
        else if (arbPath[i].bidOrask == "bid")
        {
            cout << "Trade: " << arbPath[i].from << arbPath[i].to;
            cout << " Start Capital: " << activeTradeAmt << endl;
            g.printEdge(arbPath[i].from, arbPath[i].to, arbPath[i].exchange);
            for (int j = 0; j < nDepth; j++)
            {
                double unitsCouldBuy = activeTradeAmt * exp(orderBookData[i][0][j]);
                double unitsAvail = orderBookData[i][1][j];
                double tradeSize = unitsCouldBuy - unitsAvail;
                // can buy all requested units with current orderBookPrice
                if (tradeSize <= 0)
                {
                    // edit some info about activteTradeAmt
                    conversionCapAmt += unitsCouldBuy;
                    activeTradeAmt = 0;
                    cout << "activeTradeAmt: " << activeTradeAmt << " cost: " << exp(orderBookData[i][0][j]) << " lot size: " << orderBookData[i][1][j] << endl;
                    break;
                }

                // cannot buy all requested units with current orderBookPrice
                // reduce activeTradeAmt by the cost of purchasing all units
                else
                {
                    conversionCapAmt += orderBookData[i][1][j];
                    activeTradeAmt = tradeSize / exp(orderBookData[i][0][j]);
                    cout << "activeTradeAmt: " << activeTradeAmt << " cost: " << exp(orderBookData[i][0][j]) << " lot size: " << orderBookData[i][1][j] << endl;
                } 
            }
            if (activeTradeAmt != 0)
            {
                cout << "Order size was not fillable by orderbook; Illiquid market detected" << endl;
                return -1.0;
            }
            cout << "End Capital: " << conversionCapAmt << endl;
        }
        activeTradeAmt = (conversionCapAmt * (1-feeMap[arbPath[i].exchange]));
    }

    return activeTradeAmt;
}


/*
*
* Driving method for pulling active orderbook information for all trades
* within the determined arbitrage path and then control program path flow
* in calculating ideal trade amount 
*
*/
void amountOptControl(Graph &g, vector<TrackProfit> &arbPath, int nDepth, unordered_map<string, double> &feeMap)
{
    //TODO: Explain the dimensions
    int d1 = arbPath.size(), d2 = 4, d3 = nDepth;
    vector<vector<vector<double>>> orderBookData(d1,vector<vector<double>>(d2,vector<double>(d3)));
    pullAllOrderBook(arbPath, orderBookData, nDepth);
    for (int i = 0; i < d1; i++)
    {
        g.printEdge(arbPath[i].from, arbPath[i].to, arbPath[i].exchange);
        PrintVector(orderBookData[i][0]);
        PrintVector(orderBookData[i][1]);
        PrintVector(orderBookData[i][2]);
        PrintVector(orderBookData[i][3]);
        cout << endl;
    }


    cout << endl;
    int maxBuckets = 10, orderBookOrders;
    double returnOnInvestment;
    for (int i = 0; i < d1; i++)
    {
        orderBookOrders = orderBookData[i][0].size();
        maxBuckets = min(maxBuckets, orderBookOrders);
    }
    vector<int> startCaps;
    startCaps.push_back(20);
    startCaps.push_back(40);
    


    for (int startCap : startCaps)
    {
        // TODO: If I ever get a -1, I can exit the loop as I know no further (higher) startCapital will ]
        // have enough liquidity
        returnOnInvestment = actualProfitOrderBook(g, arbPath, orderBookData, startCap, maxBuckets, feeMap);
        if (returnOnInvestment == -1)
            break;
        cout << "returnOnInvestment: " << returnOnInvestment << " realistic profitability: " << (returnOnInvestment/startCap - 1) * 100 << "%" << endl;
        cout << endl;
    }

}