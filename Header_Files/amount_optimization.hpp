#pragma once


double actualProfitOrderBook(Graph &g, vector<TrackProfit> &arbPath, vector<vector<vector<double>>> orderBookData, double startCapital, int nDepth, unordered_map<string, double> &feeMap)
{
    double activeTradeAmt = startCapital;

    // iterate over each trade in arbPath
    for (int i = 0; i < arbPath.size(); i++)
    {
        // at each trade in arbPath
        // want to dive into the connceted orderbook 
        // and continously try to get the best price at either the 
        double conversionCapAmt = 0;

        // if the trade is a ask, I want to make the price at the active bidPrice for instant order completion
        if (arbPath[i].bidOrask == "ask")
        {
            cout << "Trade: " << arbPath[i].from << arbPath[i].to;
            cout << " Start Capital: " << activeTradeAmt << endl;
            g.printEdge(arbPath[i].from, arbPath[i].to, arbPath[i].exchange);
            
            for (int j = 0; j < nDepth; j++)
            {
                double unitsCouldBuy = activeTradeAmt * (1/exp(orderBookData[i][0][j]));
                double unitsAval = orderBookData[i][1][j];
                double tradeSize = unitsCouldBuy - unitsAval;
                // can buy all requested units with current orderBookPrice
                if (tradeSize <= 0)
                {
                    // edit some info about activteTradeAmt
                    conversionCapAmt += unitsCouldBuy;
                    activeTradeAmt = 0;
                    break;
                }

                // cannot buy all requested units with current orderBookPrice
                // reduce activeTradeAmt by the cost of purchasing all units
                else
                {
                    conversionCapAmt += orderBookData[i][1][j];
                    activeTradeAmt = tradeSize / (1/exp(orderBookData[i][0][j]));
                }
                cout << "activeTradeAmt: " << activeTradeAmt << " cost: " << exp(orderBookData[i][0][j]) << " lot size: " << orderBookData[i][1][j] << endl; 
            }
            // if (activeTradeAmt != 0)
            //     cout << "Order size was not fillable by orderbook; Illiquid market detected" << endl;
            cout << "End Capital: " << conversionCapAmt << endl;
        }
        // if the trade is a bid, I want to make the price at the active askPrice for instant order completion
        else if (arbPath[i].bidOrask == "bid")
        {
            cout << "Trade: " << arbPath[i].from << arbPath[i].to;
            cout << " Start Capital: " << activeTradeAmt << endl;
            g.printEdge(arbPath[i].from, arbPath[i].to, arbPath[i].exchange);
            for (int j = 0; j < nDepth; j++)
            {
                double unitsCouldBuy = activeTradeAmt * exp(orderBookData[i][2][j]);
                double unitsAval = orderBookData[i][3][j];
                double tradeSize = unitsCouldBuy - unitsAval;
                // can buy all requested units with current orderBookPrice
                if (tradeSize <= 0)
                {
                    // edit some info about activteTradeAmt
                    conversionCapAmt += unitsCouldBuy;
                    activeTradeAmt = 0;
                    break;
                }

                // cannot buy all requested units with current orderBookPrice
                // reduce activeTradeAmt by the cost of purchasing all units
                else
                {
                    conversionCapAmt += orderBookData[i][3][j];
                    activeTradeAmt = tradeSize / exp(orderBookData[i][2][j]);
                } 
            }
            // if (activeTradeAmt != 0)
            //     cout << "Order size was not fillable by orderbook; Illiquid market detected" << endl;
            cout << "End Capital: " << conversionCapAmt << endl;
        }
        activeTradeAmt = (conversionCapAmt * (1-feeMap[arbPath[i].exchange]));
    }

    return activeTradeAmt;
}


void amountOptControl(Graph &g, vector<TrackProfit> &arbPath, int nDepth, unordered_map<string, double> &feeMap)
{
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

   
    // TODO: update late with user selected minimal trade amount
    double startTradeAmt = 1000.0, activeTradeAmt;
    // double startTradeAmtLog = log(startTradeAmt);

    // TODO: Add illquid prevention (dipping into multiple orderbook prices is okay but slippage should be managed)
    // TODO: Add a slippage calculation tolerance (will face higher slippage when actually deployed)
    activeTradeAmt = actualProfitOrderBook(g, arbPath, orderBookData, startTradeAmt, nDepth, feeMap);

    cout << "Predicted Profitability from scanning Order Books: " << endl;
    cout << "startAmt: " << startTradeAmt << " endAmt: " << activeTradeAmt << endl;
}