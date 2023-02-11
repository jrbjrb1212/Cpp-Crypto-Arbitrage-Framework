#include <iostream>
#include "Header_Files/arbitrage_finder.hpp"
#include "Header_Files/graph.hpp"
#include "Header_Files/exchange_api_pull.hpp"

using namespace std;

void printUnorderedMapV2(const std::unordered_map<std::string, std::vector<std::string>>& map) {
  for (const auto& kv : map) {
    std::cout << kv.first << ": [";
    for (const auto& v : kv.second) {
      std::cout << v << ", ";
    }
    std::cout << "]" << std::endl;
  }
}

void buildSymbolMap(unordered_map<string, vector<string> > &symbolMap)
{
    // read from Symbol_Space_Split.txt and build a dictionary
    ifstream input_file("../../Symbol_Data_Files/Symbol_Space_Split.txt");
    string line, lineCopy;
    short int slashPos;

    if (input_file.is_open())
    {
        while (getline(input_file, line))
        {
            vector<string> symbols_vec = {line.substr(0, line.find("/")), line.substr(line.find("/") + 1)};
            string symbol_key = line.substr(0, line.find("/")) + line.substr(line.find("/") + 1);
            symbolMap[symbol_key] = symbols_vec;
        }
        input_file.close();
    }
    else
    {
        cout << "Unable to open file" << endl;
    }
}


int main()
{
	Graph g;
	unordered_map<string, vector<string>> symbolMap = buildSymbolHashMap();

	// g.addEdge("USD", "LTC", 1.01, "A");
	// g.addEdge("USD", "LTC", 1.04, "B");
	// g.addEdge("USD", "BTC", 1.28, "A");
	// g.addEdge("USD", "ETH", 1.5, "A");
	// g.addEdge("LTC", "ETH", 1.2, "A");
	// g.addEdge("LTC", "BTC", 1.02, "A");
	// g.addEdge("ETH", "BTC", 1.1, "A");

	// pullBinance(symbolMap, g);
	// pullBitMart(symbolMap, g);
	pullAll(symbolMap, g, true);

	// sequential times:
	// 9.626
	// 9.648
	// 9.7.11
	// 9.377
	// 9.228

	// Parallel times:
	//

	cout << "Number of vertices: " << g.getVertexCount() << endl;
	cout << "Number of edges: " << g.getEdgeCount() << endl;
	// g.printGraph();

	

	// vector<string> sourceCoins {"USDT", "ETH"};

	// for (string coin : sourceCoins)
	// {
	// 	cout << "Performing Arb Finder from " << coin << endl;
	// 	vector<TrackProfit> arbPath = ArbDetect(g, coin, 0.95, 0.8);
	// 	// validate the arbPath using the graph
	// 	if (arbPath.size() != 0) {
	// 		for(int i = 0; i < arbPath.size(); i++){
	// 			cout << "From " << arbPath[i].from << " to " << arbPath[i].to << " trade val: " << WeightConversion(arbPath[i].weight);
	// 			cout << " via " << arbPath[i].exchange << endl;
	// 		}
	// 	}
	// }
	return 0;
}
