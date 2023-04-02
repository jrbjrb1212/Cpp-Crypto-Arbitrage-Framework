"""
" User input parsing script to allow user customizability with the
" arbitrage finding frame work
" 	Currently Supported Customizations:
" 	 - Path length: 						Determines the length of arbitrage paths that 
" 											the arbitrage finding algorithm searches
"
"	 - Start Currency: 						Determines the start and end currency of each 
"											arbitrage path. Each arbitrage path is a cycle
"
" 	 - Remove Supported Exchanges: 			Remove any exchange from data pulling and parsing
"								   			that user does not want. Can greatly reduce
" 								   			API latency based on user location
"
" 	 - Lower Bound Profitability: 			Place a lower bound for profitability for 
"								  			paths. Removes extra work for paths that are
"								  			profitable at a minute factor (Ex: 0.001%)
"
"	 - Amount of Coins to Consider: 		Determines the number of coins the framework will
"											consider and compute on. The number of coins is 
"											equivalent to the top x coins on coingecko.com
"
"	 - Minimum 24hr Coin Trading Volume:	User select minimum 24hr trading volume
"											for all coins selected above
"
"""


import sys
import os
import requests


"""
"
" Get desired arbitrage path length from user
"
"""
def path_len_selection():
	path_len = -1
	while (path_len == -1):
		try:
			in_val = int(input("Please select a desired arbitrage path length from [2,3,4,5]: "))
			if in_val < 2 or in_val > 5:
				raise Exception
			path_len = in_val
		except ValueError as verr:
			print("Bad input. Try again")
		except Exception as ex:
			print("Bad input. Try again")
	
	print("")
	return path_len


"""
"
" Get arbitrage path start and end coin from user
"
"""
def start_currency_selection():
	start_coin = None
	valid_coins = ["USDT", "BTC", "ETH", "BNB"]
	while (start_coin == None):
		try:
			print("Please select a desired start coin for all arbitrage paths")
			in_coin = input("(USDT, BTC, ETH, BNB) are supported: ").upper()
			if in_coin not in valid_coins:
				raise Exception
			start_coin = in_coin
		except:
			print("Bad input / Non Valid Start Coin")
	
	print("")
	return start_coin


"""
"
" Method to determine minimum trade size for a given coin
" As of April 1st, 2023, all exchanges supported by this framework
" have a minimum trade size that is equal to or less than 10 USDT
"
"""
def trading_amount_selection(start_coin):
	# min trade size for USDT
	min_trade_size = 20

	# determine min trade size if not in USDT
	while (start_coin != "USDT"):
		try:
			print("Requesting Active Trade Prices")
			web_req = requests.get("https://api.coingecko.com/api/v3/coins/markets?vs_currency=usd&order=market_cap_desc&per_page=25&page={iters}&sparkline=false")
			json_req = web_req.json()
			for line in json_req:
				if line["symbol"].upper() == start_coin:
					USDT_coin_ex_rate = line["current_price"]
					min_trade_size = min_trade_size / USDT_coin_ex_rate
					start_coin = "USDT"
		except:
			continue
	
	return min_trade_size


"""
"
" Method to format exchanges the user wishes to remove
" from API data pulling
"
"""
def format_exchanges(to_remove):
	to_remove_str = ""
	for i in range(len(to_remove)-1):
		to_remove_str += to_remove[i] + "/"
	
	to_remove_str += to_remove[len(to_remove)-1]
	return to_remove_str


"""
"
" Get exchanges that the user wants to blacklist
" from active pulling
"
"""
def remove_exchanges():
	# current 6 supported exchanges
	exchanges = ["binance", "kucoin", "bitmart", "bitget", "gateio", "huobi"]
	to_remove = []

	print("Currently supported exchanges: ")
	print("{Binance, KuCoin, BitMart, Bitget, Gateio, Huobi}")
	print("Enter any exchanges you wish to remove (comma separated)")
	ex_to_remove = input("If none, enter NA ").lower()
	ex_to_remove = ex_to_remove.replace(" ", "").split(",")

	for exchange in ex_to_remove:
		if exchange in exchanges:
			to_remove.append(exchange)
	
	if to_remove:
		to_remove = format_exchanges(to_remove)
	else:
		print("")
		return "None"

	print("")
	return to_remove


"""
"
" Get the lower bound for a profitable arbitrage path
" user expresses as a percentage
"
"""
def lower_bound_selection():
	lower_bound = -1.0
	print("Active arbitrage paths can be as little as 0.001% profitable and extremely profitable such as 10%")
	print("Select a percentage such that a path is not deemed as profitable unless it above that lower bound")
	while (lower_bound == -1.0):
		try:
			input_bound = float(input("What is your lower bound %: ")) / 100
			if input_bound < 0.0:
				raise ValueError
			lower_bound = input_bound
		except ValueError as verr:
			print("Bad input. Try again")
		except Exception as ex:
			print("Bad input. Try again")

	print("")
	return lower_bound


"""
"
" Get the minimum cryptocurrencies a user wants to consider
" the cryptocurrencies are retrieved from coingecko.com
"
"""
def min_coins_selection():
	coin_req = -1
	print("Please request a number of coins you wish for this framework to consider")
	print("Your number request is sent to coingecko.com API and top X coins will be assessed")
	while (coin_req == -1):
		try:
			in_coin_req = int(input("Number of coins (minimum is 25): "))
			if in_coin_req < 25:
				raise ValueError
			if in_coin_req > 250:
				print("More than 250 coins is not recommend as it may fill the framework with smaller, harder to trade coins")
				user_res = input("Would you like to continue? (Y/N) ")
				if user_res.lower != ("Y"):
					continue
			coin_req = in_coin_req

		except ValueError as verr:
			print("Bad input. Try again")
		except Exception as ex:
			print("Bad input. Try again")

	print("")
	return coin_req


""""
"
" Get the 24hr minimum volume that each cryptocurrency must
" have to be deemed as viable for trading
"
"""
def min_vol_selection():
	trade_vol = -1.0
	print("Please request a 24hr trade volume for each coin you wish for this framework to consider")
	print("Your number request is sent to coingecko.com API and only coins at or above that volume will be considered")

	while(trade_vol == -1.0):
		try:
			in_vol = float(input("24hr Trading Volume in USDT (minimum is 0.0 USDT): "))
	
			if in_vol > 400000000:
				print("More than 400m in trading volume as it limits your arbitrage opportunities to less than 100 coins")
				user_res = input("Would you like to continue? (Y/N) ")
				if user_res.lower != ("Y"):
					continue
			trade_vol = in_vol

		except ValueError as verr:
			print("Bad input. Try again")
		except Exception as ex:
			print("Bad input. Try again")

	print("")
	return trade_vol


""""
"
" Get desired orderbook depth to search when determining
" realistic path profitability
"
"""
def order_book_depth_selection():
	order_book_depth = 0
	print("This framework retrieves most up to date orderbook information (active market orders)")
	print("before terming a series of trades as a profitable arbitrage")

	while (order_book_depth == 0):
		try:
			in_n_depth = int(input("How many active orders would you like to consider at any given time? (0, 250]: "))
			if in_n_depth < 5:
				print(f'{in_n_depth} may be too small with order books of lower liquidity')
				in_res = input("Are you sure you want to proceed? (Y/N): ").lower()
				if in_res == "y":
					order_book_depth = in_n_depth
				else:
					raise ValueError
			if in_n_depth > 250:
				print(f'{in_n_depth} is too many orders for most trading pairs. Truncating {in_n_depth} down to 250')
				order_book_depth = 250
				continue
			order_book_depth = in_n_depth


		except ValueError as verr:
			continue
		except Exception as ex:
			print("Bad input. Try again")
	
	print("")
	return order_book_depth


""""
"
" Get debug/test mode activation from user 
"
"""
def debug_time_mode():
	print("This framework is presented as easy to use, modify, and adapt")
	print("For this reason, there are two modes outside of normal arbitrage find mode")
	print(" - Debug Mode: Print all important information about a single framework iteration to learn how it works")
	print(" - Time Mode: Used to benchmark speed of the framework, will provide a time breakdown per iteration")

	debug_mode = time_mode = "0"
	end_condition = False
	while (not end_condition):
		try:
			in_include = input("Would you like to enable either of these options (Y/N): ").lower()
			if in_include == "n":
				end_condition = True
			elif in_include == "y":
				in_debug_mode = input("Would you like to enable debug mode (Y/N): ").lower()
				if in_debug_mode == "y":
					end_condition = True
					debug_mode = "1"
					break
				if in_debug_mode != "n":
					raise ValueError

				in_test_mode = input("Would you like to enable time mode (Y/N): ").lower()
				if in_test_mode == "y":
					end_condition = True
					time_mode = "1"
				elif in_test_mode == "n":
					end_condition = True
				else:
					raise ValueError

			else:
				raise ValueError
		except ValueError as verr:
			print("Bad input. Try again")
		except Exception as ex:
			print("Bad input. Try again")
	

	print("")
	return debug_mode, time_mode


""""
"
" Print current user selections to console
" and ask user to okay them before a user_settings.txt
" is created
"
"""
def print_to_console(selected_user_settings):
	for setting in selected_user_settings:
		if "tradeAmt" in setting:
			continue
		if "lowerBound" in setting:
			setting = setting.split("=")
			percentage = float(setting[1]) * 100
			print(f'{setting[0]}={percentage}%')
			continue
		if "Mode" in setting:
			setting = setting.replace("\n", "").split("=")
			t_f_str = "True" if setting[1] == "1" else "False"
			print(f'{setting[0]}={t_f_str}')
			continue
		print(setting, end="")

	green_light = input("Is all of this information correct? (Y/N): ")
	if green_light.lower() == "y":
		return True
	else:
		os.system("clear")
		print("Restarting user questions now")
		return False


""""
"
" Generate the user setting text file
"
"""
def generate_output_file(user_filepath, user_settings):
	with open(user_filepath, 'w') as f_out:
		for line in user_settings:
			f_out.write(line)


""""
"
" Driver code to get user options
"
"""
def main():
	green_light = False
	user_settings = []

	while (not green_light):
		# Invoke all user customization methods
		path_len = path_len_selection()
		start_coin = start_currency_selection()
		min_trading_amount = trading_amount_selection(start_coin)
		exchanges_remove = remove_exchanges()
		profit_lower_bound = lower_bound_selection()
		coin_req = min_coins_selection()
		vol_req = min_vol_selection()
		order_book_depth = order_book_depth_selection()
		debug_mode, time_mode = debug_time_mode()
		
		# Format all user customization choices
		path_len_str = f'pathLen={path_len}\n'
		start_coin_str = f'startCoin={start_coin}\n'
		trading_amount_str = f'tradeAmt={min_trading_amount}\n'
		exchanges_remove_str = f'exchangeRemove={exchanges_remove}\n'
		lower_bound_str = f'lowerBound={profit_lower_bound}\n'
		coin_req_str = f'coinReq={coin_req}\n'
		vol_req_str = f'volReq={vol_req}\n'
		debug_str = f'debugMode={debug_mode}\n'
		time_str = f'timeMode={time_mode}\n'
		order_book_depth_str = f'orderBookDepth={order_book_depth}\n'

		user_settings = [path_len_str, start_coin_str, trading_amount_str,\
						exchanges_remove_str, lower_bound_str,\
						coin_req_str, vol_req_str,\
						debug_str, time_str,\
						order_book_depth_str]
		os.system("clear")
		green_light = print_to_console(user_settings)

	# Once user approves selections, create the user_settings.txt file
	generate_output_file(sys.argv[1], user_settings)



if __name__ == "__main__":
	main()