import asyncio
from dataclasses import dataclass
from dataclasses_json import dataclass_json
from typing import Any
from enum import Enum
import utils


class Side(Enum):
    ASK = "offer"
    BID = "bid"


class SubscriberKraken(utils.MarketDataSubscriber):
    EXCHANGE_NAME = 'KRAKEN'
    # API_URL = 'wss://ws.kraken.com'
    HOST = 'ws.kraken.com'
    PORT = 443
    PATH = ''
    SUBSCRIPTION = {
        "event":"subscribe",
        "subscription": {
            "name": "book"
        },
        "pair": [
            "BTC/USDT"
        ]
    }

    def __init__(self) -> None:
        super().__init__(self.EXCHANGE_NAME, host=self.HOST, port=self.PORT, path=self.PATH, subscription=self.SUBSCRIPTION)

    def check(self, message: Any) -> bool:
        return 'event' not in message

    def process(self, message: Any) -> None:
        symbol = message[3]
        updates = message[1]
        for update in updates.get('a', []):
            self._sink.on_market_update(symbol, Side.ASK.name, float(update[0]), float(update[1]))
        for update in updates.get('as', []):
            self._sink.on_market_update(symbol, Side.ASK.name, float(update[0]), float(update[1]))
        for update in updates.get('b', []):
            self._sink.on_market_update(symbol, Side.BID.name, float(update[0]), float(update[1]))
        for update in updates.get('bs', []):
            self._sink.on_market_update(symbol, Side.BID.name, float(update[0]), float(update[1]))


def main() -> None:
    subscriber = SubscriberKraken()
    asyncio.run(subscriber.subscribe())


if __name__ == '__main__':
    main()
