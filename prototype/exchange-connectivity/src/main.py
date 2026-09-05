import argparse
import asyncio
import sys
import importlib
import utils


def main():
    parser = argparse.ArgumentParser(description="Subscribe to a crypto exchange and receive market updates.")
    parser.add_argument('--debug', action='store_true', help='Display raw JSON messages')
    parser.add_argument("--exchange", choices=["binance", "bybit", "kraken", "okx", "coinbase", "bitmex"], required=True, help="Name of the exchange to subscribe to")
    args = parser.parse_args()
    utils.DISPLAY_RAW_MESSAGES = args.debug
    try:
        selected_exchange = importlib.import_module(f'exchanges.{args.exchange}')
    except ImportError:
        print(f"Error: Exchange '{args.exchange}' not found.")
        sys.exit(1)
    try:
        asyncio.run(selected_exchange.main())
    except KeyboardInterrupt:
        print("\nStream stopped by user.")


if __name__ == "__main__":
    main()
