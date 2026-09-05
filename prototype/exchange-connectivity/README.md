# CEX Subscriber

## Objective

This is a prototype to discover several crypto exchanges
and get familiar with their connectivity.

It also allows to assess the data quality before
performing the aggregation.

## Description

It is written in Python and uses [uv](https://docs.astral.sh/uv/).

The program connects to the following exchanges:

+ binance
+ bitmex
+ bybit
+ coinbase
+ kraken
+ okx

Once the connection established, it will simply log market updates
for the **BTC/USDT** pair on the console and in a file.

## Usage

Run it for a specific exchange:

```sh
uv run src/main.py --exchange EXCHANGE_NAME
```

Run it for a specific exchange, showing raw JSON messages:

```sh
uv run src/main.py --exchange EXCHANGE_NAME --debug
```

Collect 10 seconds of market data for each exchange
and concatenete all results in a single CSV file:

```sh
make all.csv
```

Same but with a custom duration:

```sh
make all.csv TIMEOUT=2
```
