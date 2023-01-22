import requests
import concurrent.futures

def get_24_hr_trading_volume(symbol):
    url = f"https://api.binance.com/api/v3/ticker/24hr?symbol={symbol}"
    response = requests.get(url)
    if response.status_code != 200:
        print(f"Error: {response.text}")
        return 0
    json_data = response.json()
    return json_data["quoteVolume"]

if __name__ == "__main__":
    symbols = [""] * 2142
    with open("all_binance_symbols.txt") as read:
        for i, line in enumerate(read):
            symbols[i] = line[1:-2]
        
    with open("test.txt", "w") as out:
        with concurrent.futures.ProcessPoolExecutor() as executor:
            results = [executor.submit(get_24_hr_trading_volume, symbol) for symbol in symbols]
            for f in concurrent.futures.as_completed(results):
                symbol = symbols[results.index(f)]
                try:
                    volume = f.result()
                    if float(volume) > 0:
                        out.write(symbol + "\n")
                        out.flush()
                except Exception as e:
                    print(f"Error: {e}")


# with open("all_binance_symbols.txt") as read:
#         with open("correct_binance_symbols.txt", 'w') as out:
#             for line in read:
#                 symbol = line[1:-2]
#                 volume = get_24_hr_trading_volume(symbol)
#                 print(f"24 hour trading volume for {symbol} is {volume}")
#                 if float(volume[0]) == 0.0 or float(volume[1]) == 0.0:
#                     continue
#                 out.write(line)
#                 out.flush()