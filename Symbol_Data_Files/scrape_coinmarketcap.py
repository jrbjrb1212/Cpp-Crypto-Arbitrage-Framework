import requests
import concurrent.futures
from bs4 import BeautifulSoup
import sys
# TODO: need to add comments
# TODO: need to look into what kind of information I am actually pulling from coinmarket urls
# TODO: need to add viable symbols one per line into viable symbols output file
# TODO: need to change the symbol_map in exchange_api_pull
#		- change from "BTCUSDT" -> {BTC, USD}

def main():
	if (len(sys.argv) != 3):
		print("CLI Format: python3 scrape_coinmarketcap.py <Number of coins> <minimum 24hr trade volume (USD)>")
		return

	number_coins_req = int(sys.argv[1])
	if number_coins_req <= 0:
		print("Please enter a number a real number for amount of coins")
		print("CLI Format: python3 scrape_coinmarketcap.py <Number of coins> <minimum 24hr trade volume (USD)>")
		return
	if number_coins_req > 250:
		print("More than 250 coins is not recommend as it may fill the graph with smaller, harder to trade coins")
		user_res = input("Would you like to continue? (Y/N) ")
		if user_res == "N" or user_res == "n":
			return
	
	min_coin_vol = float(sys.argv[2])
	if min_coin_vol <= 0:
		print("Please enter a number a real number for minimum 24hr trading volume")
		print("CLI Format: python3 scrape_coinmarketcap.py <Number of coins> <minimum 24hr trade volume (USD)>")
		return
	if min_coin_vol > 400000000:
		print("More than 400m in trading volume as it limits your arbritrage opportunties to less than 100 coins")
		user_res = input("Would you like to continue? (Y/N) ")
		if user_res == "N" or user_res == "n":
			return


	pages_to_parse = number_coins_req // 100
	last_page_coins = number_coins_req % 100

	coinmarketcap_baseURL = "https://coinmarketcap.com/?page="
	pages_to_scrape = []
	if pages_to_parse == 0:
		pages_to_scrape.append(coinmarketcap_baseURL + "1")
	else:
		for i in range(1, pages_to_parse+2):
			pages_to_scrape.append(coinmarketcap_baseURL + str(i))

	viable_coins = scrape_pages(pages_to_scrape, last_page_coins, min_coin_vol)
	if len(viable_coins) < 50:
		print("Only", len(viable_coins), "coins were returned")
		user_res = input("Would you like to continue? (Y/N) ")
		if user_res == "N" or user_res == "n":
			return
	
	create_symbol_output_file(viable_coins)


def scrape_page(coin_page_URL):
	# Scrape coin symbols and volumes from a single page
	scraped_coins, scraped_vols = [], []
	request_res = requests.get(coin_page_URL)
	html_rep = BeautifulSoup(request_res.text, "html.parser")
	for coin in html_rep.find_all("p", {"class": "coin-item-symbol"}):
		scraped_coins.append(coin.renderContents().decode('UTF-8'))
	for vol in html_rep.find_all("span", {"class": "sc-edc9a476-1 gqomIJ"}):
		scraped_vols.append(float(vol.renderContents().decode('UTF-8')[1:].replace(",", "")))
	return scraped_coins, scraped_vols


def scrape_pages(pages_to_scrape, last_page_coins, min_vol):
	# Scrape coin symbols and volumes from multiple pages
	scraped_coins, scraped_vols = [], []
	viable_coins = []
	with concurrent.futures.ThreadPoolExecutor() as executor:
		# Use ThreadPoolExecutor to run scrape_page on multiple pages in parallel
		futures = []
		for coin_page_URL in pages_to_scrape:
			futures.append(executor.submit(scrape_page, coin_page_URL))
		# Wait for all the futures to complete and collect the results
		for future in concurrent.futures.as_completed(futures):
			coins, vols = future.result()
			scraped_coins.extend(coins)
			scraped_vols.extend(vols)
	
	scraped_coins = scraped_coins[:-(10-last_page_coins)]
	scraped_vols = scraped_vols[:-(10-last_page_coins)]

	for coin, vol in zip(scraped_coins, scraped_vols):
		if vol < min_vol:
			continue
		viable_coins.append(coin)
	
	return viable_coins


def create_symbol_output_file(viable_coins):
	with open("Viable_Symbols.txt", 'w') as f_write:
		for from_coin in viable_coins:
			for to_coin in viable_coins:
				if from_coin == to_coin:
					continue
				f_write.write("\"" + from_coin + "/" + to_coin + "\"" + "\n")
			f_write.flush()
	




if __name__ == "__main__":
	main()