import asyncio
from dataclasses import dataclass
from dataclasses_json import dataclass_json
from typing import Any
from enum import Enum
import utils


class Side(Enum):
    ASK = "offer"
    BID = "bid"


@dataclass_json
@dataclass
class Level:
    px: float
    sz: float
    n: int


@dataclass_json
@dataclass
class Data:
    coin: str
    levels: list[list[Level]]


@dataclass_json
@dataclass
class Message:
    channel: str
    data: Data


class SubscriberHyperliquid(utils.MarketDataSubscriber):
    EXCHANGE_NAME = 'HYPERLIQUID'
    HOST = 'api.hyperliquid.xyz'
    PORT = 443
    PATH = 'ws'
    SUBSCRIPTION = {
        "method": "subscribe",
        "subscription": {
            "type": "l2Book",
            "coin": "BTC"
        }
    }

    def __init__(self) -> None:
        super().__init__(self.EXCHANGE_NAME, host=self.HOST, port=self.PORT, path=self.PATH, subscription=self.SUBSCRIPTION)

    def check(self, message: Any) -> bool:
        return 'channel' in message and message.get('channel') == 'l2Book'

    def process(self, message: Any) -> None:
        message = Message.from_dict(message)
        for level in message.data.levels[0]:
            self._sink.on_market_update(message.data.coin, Side.BID.name, level.px, level.sz)
        for level in message.data.levels[1]:
            self._sink.on_market_update(message.data.coin, Side.ASK.name, level.px, level.sz)


def main() -> None:
    subscriber = SubscriberHyperliquid()
    asyncio.run(subscriber.subscribe())


if __name__ == '__main__':
    main()
