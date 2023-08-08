import requests
import sys
# TODO: consider changing order of the code for readability


"""
" 
" Method parses coin information from json endpoint data
" if the trading volume is higher than the user defined minimum
" mark the coin as viable for trading
"
"""
def parse_coin_from_json(json_data, viable_coins, min_vol):
	# each entry in the json data is a hashmap of information
	# about a coin
	for coin_data in json_data:
		try:
			vol = float(coin_data["total_volume"])
			if vol < min_vol:
				continue
			
			# append the coin to the viable_coins list if the min 
			# volume condition is satisfied
			viable_coins.append(coin_data["symbol"].upper())
		except:
			continue

	return viable_coins


"""
" 
" Use requests to get crypto data from
" the coin gecko endpoint and parse it with
" the json library
"
"""
def extract_coin_info(page_URL):
	request_res = requests.get(page_URL)
	if request_res.status_code == 200:
		# Convert the response to JSON format
		json_coins_data = request_res.json()
		return json_coins_data


"""
" 
" Method to retrieve general crypto data
" via the coingecko free public API
" data include symbol, volume, price change, etc.
"
"""
def scrape_pages(viable_coin_count, min_vol):
	viable_coins = []
	iters, last_count = 1, -1

	# continue to parse more coins while they are more coins to request via api
	# and the last iteration of the loop added coins
	while (len(viable_coins) < viable_coin_count and last_count != len(viable_coins)):
		last_count = len(viable_coins)
		page_URL = f"https://api.coingecko.com/api/v3/coins/markets?vs_currency=usd&order=market_cap_desc&per_page=250&page={iters}&sparkline=false"
		json_coins_data = extract_coin_info(page_URL)
		parse_coin_from_json(json_coins_data, viable_coins, min_vol)
		# print(len(viable_coins))
		
		iters += 1	

	# truncate out extra coins if they were parsed and added
	return viable_coins[:viable_coin_count-1]

"""
" Method to print all viable coins to a txt file
" a set can be created from file to parse out
" nonviable coins from API Ticker Data
"""
def create_symbol_output_file(viable_coins):
	with open("Viable_Trading_Pairs.txt", "w") as f_out:
		for i, from_symbol in enumerate(viable_coins):
			if from_symbol == "" or "." in from_symbol:
				continue
			for j, to_symbol in enumerate(viable_coins[i+1:]):
				if to_symbol == "" or "." in to_symbol:
					continue
				str_forward_print = f'\"{from_symbol}/{to_symbol}\"\n'
				f_out.write(str_forward_print)
				str_backward_print = f'\"{to_symbol}/{from_symbol}\"\n'
				f_out.write(str_backward_print)


"""
"
" Method for parsing coin volume cli
" argument and user input where needed
"
"""
def cli_vol_parse():
	min_coin_vol = float(sys.argv[2])
	if min_coin_vol <= 0:
		print("Please enter a number a real number for minimum 24hr trading volume")
		print("CLI Format: python3 scrape_coinmarketcap.py <Number of coins> <minimum 24hr trade volume (USD)>")
		return True
	
	if min_coin_vol > 400000000:
		print("More than 400m in trading volume as it limits your arbitrage opportunities to less than 100 coins")
		user_res = input("Would you like to continue? (Y/N) ")
		if user_res == "N" or user_res == "n":
			return True


"""
"
" Method for parsing amount of coins cli
" argument and user input where needed
"
"""
def cli_coins_parse():
	number_coins_req = int(sys.argv[1])
	if number_coins_req <= 0:
		print("Please enter a number a real number for amount of coins")
		print("CLI Format: python3 scrape_coinmarketcap.py <Number of coins> <minimum 24hr trade volume (USD)>")
		return True
	
	if number_coins_req > 250:
		print("More than 250 coins is not recommend as it may fill the graph with smaller, harder to trade coins")
		user_res = input("Would you like to continue? (Y/N) ")
		if user_res == "N" or user_res == "n":
			return True


"""
"
" Method for parsing CLI and directing
" user input where needed
"
"""
def cli_parse():
	if (len(sys.argv) != 3):
		print("CLI Format: python3 scrape_coinmarketcap.py <Number of coins> <minimum 24hr trade volume (USD)>")
		return True

	# parse the first CLI argument which is # number of coins
	if cli_coins_parse():
		return True
	
	# parse the second CLI argument which is minimum 24hr trading volume
	if cli_vol_parse():
		return True


"""
"
" Drive method for parsing CLI 
" and calling escarping and formatting
"
"""
def main():
	number_coins_req = int(sys.argv[1])
	min_coin_vol = float(sys.argv[2])
	
	# scrape all pages
	viable_coins = scrape_pages(number_coins_req, min_coin_vol)
	percent_requested = len(viable_coins) / number_coins_req
	if percent_requested < 0.50:
		print(f"Only {len(viable_coins)} coins were returned, only {percent_requested * 100}% of requested")
		user_res = input("Would you like to continue? (Y/N) ")
		if user_res == "N" or user_res == "n":
			return
	
	create_symbol_output_file(viable_coins)
	return
	
if __name__ == "__main__":
	main()