# Cpp Crypto Arbitrage Framework
A C++ dominant framework for pulling real-time data via public endpoints from six crypto exchanges to compute a crypto currency arbitrage that is both mathematically profitable and a proper amount to trade to get realistic profitability  

COOL PICTURE COMING SOON

# Speed through C++ and parallelization
Pulling active ticker data via public API endpoints from all trading pairs across 6 crypto exchanges each iteration is about 2.5 MB of JSON string data. This data is used by downloading ticker data, parsing ticker data, performing arbitrage finding algorithm, formatting the arbitrage path, downloading relevant orderbook information, and determining realistic profitability. Most of these are expensive, time consuming operations that in the end will net a path that is deemed profitable except if the path takes too long to compute the profitability/trading opportunity may have disappeared.

For the reason that speed is royalty in this situation, C++ is used on all time dominant operations and all operations that could have benefited from embarrassingly or data parallelism were accommodated. On my own hardware (has not been tested on outside hardware), the time dominant factor of my framework is the time spent downloading from the public endpoint. Over 99% of time is spent simply downloading ticker and orderbook data, which is both good and bad (See Time BreakDown soon). It showcases that the rest of the code is inefficient, but that the code suffers from outside factors that can not be avoided without additional hardware costs. 

# Outline
# Arbitrage Finding Algorithm
- Talk about implementation of brute force algorithm and source coin
- Talk about implementation of data as a data structure
- Talk about each vertex is a coin and there may be multiple edges from a given coin to another coin as there are multiple exchange being considered 
- Talk about the use of natural logs for numerical stability
- Possibly put in the mathematical formula that I am maximizing

# Trading Amount Optimization
- Talk about dipping into multiple buckets
- Talk about the customizable choice of how much buckets/orders to dip into

# Flexibility
- Talk about path length
- Customizability with profit thresholds
- Trading Amount OPtimization options
- Closed Exchange Option

# Usability
- Entire program is driven from a shell script that will ask user for user options and then begin performing the arbitrage finding


# Exchange Data Pulling
PICTURE OF DATA PARALLELISM


# Speed Benchmarks


# Simulated Results



# Possible Next Stops or Different Directions
- Format the data into, a matrix and using k matrix multiply to maximize the with the entry in the result or intermediate multiply matrix being the argMax of a row from matrixA and column from matrixB
    - Reduces arbitrage algorithm to O((k+1)*n^(2.81)), where k is the number of desired trades, for larger arbitrage paths and is quite amendable to parallelization
    - Easy to calculate but would be harder to trace the path from the result
- Along with (k+1) matrix multiplies is the idea of tropical algebra for currency arbitrage outlined in this [paper](https://commons.lib.jmu.edu/cgi/viewcontent.cgi?article=1303&context=honors201019) that uses more sophisticated mathematical formuli, such as the kronocker and Karp's Algorithm

- Another option is [Johnson's algorithm](https://en.wikipedia.org/wiki/Johnson%27s_algorithm) or [Floyd-Warshall](https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm) to compute an all pairs 
    - Both will yield the greatest possible profitability for an arbitrage is fed data correctly
    - Both suffer from tracing from profitability to the actual path as multiple intersecting arbitrage paths in the graph will produce incorrect tracing

- A final option is linear programming with a linear solver for both improving the computational speed of the arbitrage finder and amount of optimizer
    - There are multiple providers of free/payed linear solvers such as Gurobi, MATLAB, HiGHS, or any [others]()
    - Constraints could be freely added by used such as certain coins to favor or avoid, path length, minimal or maximal exchanges to make trades on, min or max of trading a coin or set of coins, bias towards coins that have or have not been traded before
    - Objective function would be the maximization profit from available trades at any given time 


# Disclaimer
- This crypto arbitrage framework was done for personal interest. It is possible that this framework will output a false positive path that is produce financial loss because of outdated orderbook data so any outside use of this code is at the users discretion and risk 
- Very few exchanges offer cross exchange trades, so keeping balances on all exchanges in use is recommended for the best market order speed efficiency else one would have transfer funds from exchanges and pay additional fees