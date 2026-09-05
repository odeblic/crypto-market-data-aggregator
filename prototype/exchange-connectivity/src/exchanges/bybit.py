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
class Data:
    s: str
    b: list
    a: list
    u: int


@dataclass_json
@dataclass
class Message:
    topic: str
    type: str
    data: Data


class SubscriberBybit(utils.MarketDataSubscriber):
    EXCHANGE_NAME = 'BYBIT'
    # API_URL = 'wss://stream.bybit.com/v5/public/linear'
    HOST = 'stream.bybit.com'
    PORT = 443
    PATH = 'v5/public/linear'
    SUBSCRIPTION = {
        "op": "subscribe",
        "args": ["orderbook.50.BTCUSDT"]
    }

    def __init__(self) -> None:
        super().__init__(self.EXCHANGE_NAME, host=self.HOST, port=self.PORT, path=self.PATH, subscription=self.SUBSCRIPTION)

    def check(self, message: Any) -> bool:
        return 'topic' in message and message.get('topic') == 'orderbook.50.BTCUSDT'

    def process(self, message: Any) -> None:
        message = Message.from_dict(message)
        for update in message.data.a:
            self._sink.on_market_update(message.data.s, Side.ASK.name, float(update[0]), float(update[1]))
        for update in message.data.b:
            self._sink.on_market_update(message.data.s, Side.BID.name, float(update[0]), float(update[1]))


def main() -> None:
    subscriber = SubscriberBybit()
    asyncio.run(subscriber.subscribe())


if __name__ == '__main__':
    main()
