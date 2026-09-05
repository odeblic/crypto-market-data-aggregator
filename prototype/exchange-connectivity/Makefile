.PHONY: clean

TIMEOUT := 10

all.csv: binance.csv bitmex.csv bybit.csv coinbase.csv kraken.csv okx.csv
	head -n 1 binance.csv > all.csv
	#head -n 1 $(ls *.csv | head -n 1) > all.csv
	tail -n +2 -q binance.csv >> all.csv
	tail -n +2 -q bitmex.csv >> all.csv
	tail -n +2 -q bybit.csv >> all.csv
	tail -n +2 -q coinbase.csv >> all.csv
	tail -n +2 -q kraken.csv >> all.csv
	tail -n +2 -q okx.csv >> all.csv

%.csv:
	-timeout $(TIMEOUT) uv run src/main.py --exchange $*

clean:
	rm -f binance.csv
	rm -f bitmex.csv
	rm -f bybit.csv
	rm -f coinbase.csv
	rm -f kraken.csv
	rm -f okx.csv
	rm -f all.csv
