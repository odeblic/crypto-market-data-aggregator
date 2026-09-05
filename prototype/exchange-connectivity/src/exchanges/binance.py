import asyncio
from dataclasses import dataclass
from dataclasses_json import dataclass_json
from typing import Any
from enum import Enum
import utils


class Side(Enum):
    ASK = "ask"
    BID = "bid"


@dataclass_json
@dataclass
class Message:
    e: str
    s: str
    b: list[list[float]]
    a: list[list[float]]


class SubscriberBinance(utils.MarketDataSubscriber):
    EXCHANGE_NAME = 'BINANCE'
    # API_URL = 'wss://ws-api.binance.com:9443/ws-api/v3'
    # API_URL = 'wss://stream.binance.com:9443'
    # API_URL = 'wss://stream.binance.com:9443/ws/btcusdt@depth@100ms'
    HOST = 'stream.binance.com'
    PORT = 9443
    PATH = 'ws/btcusdt@depth@100ms'
    SUBSCRIPTION = {
        "method": "SUBSCRIBE",
        "params": [
            "btcusdt@depth20@100ms",
            # "btcusdt@aggTrade",
            # "btcusdt@depth@100ms",
        ],
        "id": 1,
    }

    """
    SUBSCRIPTION = {
        "method": "SUBSCRIBE",
        "params": {
                "symbol": "BTC/USDT",
        },
        "id": 1,
    }
    """

    def __init__(self) -> None:
        super().__init__(self.EXCHANGE_NAME, host=self.HOST, port=self.PORT, path=self.PATH, subscription=self.SUBSCRIPTION)

    def check(self, message: Any) -> bool:
        return 'e' in message and message.get('e') == 'depthUpdate'

    def process(self, message: Any) -> None:
        message = Message.from_dict(message)
        for update in message.a:
            self._sink.on_market_update(message.s, Side.ASK.name, float(update[0]), float(update[1]))
        for update in message.b:
            self._sink.on_market_update(message.s, Side.BID.name, float(update[0]), float(update[1]))


def main() -> None:
    subscriber = SubscriberBinance()
    asyncio.run(subscriber.subscribe())


if __name__ == '__main__':
    main()
