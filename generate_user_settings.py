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


def trading_amount_selection(start_coin):
	min_trade_size = 20
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
	
	print("")
	return min_trade_size


def format_exchanges(to_remove):
	to_remove_str = ""
	for i in range(len(to_remove)-1):
		to_remove_str += to_remove[i] + "/"
	
	to_remove_str += to_remove[len(to_remove)-1]
	return to_remove_str

def remove_exchanges():
	exchanges = ["binance", "kucoin", "bitmart", "bitget", "gate.io", "huobi"]
	to_remove = []

	print("Currently supported exchanges: ")
	print("{Binance, KuCoin, BitMart, Bitget, Gate.io, Huobi}")
	print("Enter any exchanges you wish to remove (comma separated)")
	ex_to_remove = input("If none, enter NA ").lower()
	ex_to_remove = ex_to_remove.replace(" ", "").split(",")

	for exchange in ex_to_remove:
		if exchange in exchanges:
			to_remove.append(exchange)
	
	if to_remove:
		to_remove = format_exchanges(to_remove)
	else:
		return ""

	print("")
	return to_remove

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

def min_vol_selection():
	trade_vol = -1.0
	print("Please request a 24hr trade volume for each coin you wish for this framework to consider")
	print("Your number request is sent to coingecko.com API and only coins at or above that volume will be considered")

	while(trade_vol == -1.0):
		try:
			in_vol = float(input("24hr Trading Volume (minimum is 0.0): "))
	
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


def print_to_console(selected_user_settings):
	for setting in selected_user_settings:
		if "tradeAmt" in setting:
			continue
		if "lowerBound" in setting:
			setting = setting.split("=")
			percentage = float(setting[1]) * 100
			print(f'{setting[0]}={percentage}%')
			continue
		print(setting, end="")

	green_light = input("Is all of this information correct? (Y/N): ")
	if green_light.lower() == "y":
		return True
	else:
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


def main():
	print(f'Input file is {sys.argv[1]}')
	green_light = False
	user_settings = []
	while (not green_light):
		path_len = path_len_selection()
		start_coin = start_currency_selection()
		min_trading_amount = trading_amount_selection(start_coin)
		exchanges_remove = remove_exchanges()
		profit_lower_bound = lower_bound_selection()
		coin_req = min_coins_selection()
		vol_req = min_vol_selection()
		
		path_len_str = f'pathLen={path_len}\n'
		start_coin_str = f'startCoin={start_coin}\n'
		trading_amount_str = f'tradeAmt={min_trading_amount}\n'
		exchanges_remove_str = f'exchangeRemove={exchanges_remove}\n'
		lower_bound_str = f'lowerBound={profit_lower_bound}\n'
		coin_req_str = f'coinReq={coin_req}\n'
		vol_req_str = f'volReq={vol_req}\n'

		user_settings = [path_len_str, start_coin_str, trading_amount_str,\
						exchanges_remove_str, lower_bound_str,\
						coin_req_str, vol_req_str]
		os.system("clear")
		green_light = print_to_console(user_settings)

	
	generate_output_file(sys.argv[1], user_settings)



if __name__ == "__main__":
	main()