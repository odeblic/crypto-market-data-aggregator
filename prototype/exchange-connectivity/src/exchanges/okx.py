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
class Arg:
    channel: str
    instId: str


@dataclass_json
@dataclass
class DataItem:
    asks: list[list[Any]]
    bids: list[list[Any]]


@dataclass_json
@dataclass
class Message:
    arg: Arg
    action: str
    data: list[DataItem]


class SubscriberOKX(utils.MarketDataSubscriber):
    EXCHANGE_NAME = 'OKX'
    # API_URL = 'wss://ws.okx.com:8443/ws/v5/public'
    HOST = 'ws.okx.com'
    PORT = 8443
    PATH = 'ws/v5/public'
    SUBSCRIPTION = {
        "op": "subscribe",
        "args": [
            {
                "channel": "books",
                "instId": "BTC-USDT"
            }
        ]
    }

    def __init__(self) -> None:
        super().__init__(self.EXCHANGE_NAME, host=self.HOST, port=self.PORT, path=self.PATH, subscription=self.SUBSCRIPTION)

    def check(self, message: Any) -> bool:
        return 'action' in message and message.get('action') == 'update'

    def process(self, message: Any) -> None:
        message = Message.from_dict(message)
        for data_item in message.data:
            for update in data_item.asks:
                self._sink.on_market_update(message.arg.instId, Side.ASK.name, float(update[0]), float(update[1]))
            for update in data_item.bids:
                self._sink.on_market_update(message.arg.instId, Side.BID.name, float(update[0]), float(update[1]))


def main() -> None:
    subscriber = SubscriberOKX()
    asyncio.run(subscriber.subscribe())


if __name__ == '__main__':
    main()
