# CEX Subscriber

## Objective

This prototype is used to discover several crypto exchanges and become
familiar with their connectivity.

It also allows us to assess data quality before performing aggregation.

## Description

It is written in Python and uses [uv](https://docs.astral.sh/uv/).

The program connects to the following exchanges:

+ binance
+ bitmex
+ bybit
+ coinbase
+ kraken
+ okx

Once the connection is established, it simply logs market updates
for the **BTC/USDT** pair to the console and to a file.

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
and concatenate all results into a single CSV file:

```sh
make all.csv
```

Same but with a custom duration:

```sh
make all.csv TIMEOUT=2
```
