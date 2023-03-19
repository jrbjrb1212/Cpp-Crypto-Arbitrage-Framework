#pragma once


void amountOpt(vector<vector<double>> &orderbookSingData, double activeTradeAmt)
{
    // only considering at the bid or ask price
    

}


void amountOpt (vector<TrackProfit> &arbPath, int nDepth, unordered_map<string, double> &feeMap)
{
    int d1 = arbPath.size(), d2 = 4, d3 = nDepth;
    vector<vector<vector<double>>> orderBookData(d1,vector<vector<double>>(d2,vector<double>(d3)));
    pullAllOrderBook(arbPath, orderBookData, nDepth);

   
    // TODO: update late with user selected minimal trade amount
    double minimalTradeAmt = 10.0;
    double minimalTradeAmtLog = log(minimalTradeAmt);
    
    // at first trade I have startCapLog with the orderPrice of the first trade to consider
    // at the second trade I have (startCapLog + firstTrade.price + firstTradeFee) to consider
    // third trade is so on

    for(int i = 0; i < arbPath.size(); i++)
    {
        if (arbPath[i].bidOrask == "bid") 
        {
            // pull out first element of ask pricing and amount from orderBookData

        } 
        else if (arbPath[i].bidOrask == "ask")
        {
            // pull out first element of bid pricing and amount from orderBookData

        }   
    }

    // cout << "log rep: " << minimalTradeAmtLog << ", decimal rep: " << WeightConversion(minimalTradeAmtLog) << endl;
    // cout << "profitability: " << (WeightConversion(minimalTradeAmtLog)/minimalTradeAmt - 1) * 100 << "%" << endl;
}