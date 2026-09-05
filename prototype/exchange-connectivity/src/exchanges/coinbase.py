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
class Update:
    side: Side
    price_level: float
    new_quantity: float


@dataclass_json
@dataclass
class Event:
    product_id: str
    updates: list[Update]


@dataclass_json
@dataclass
class Message:
    channel: str
    events: list[Event]


class SubscriberCoinbase(utils.MarketDataSubscriber):
    EXCHANGE_NAME = 'COINBASE'
    # API_URL = 'wss://advanced-trade-ws.coinbase.com'
    HOST = 'advanced-trade-ws.coinbase.com'
    PORT = 443
    PATH = ''
    SUBSCRIPTION = {
        "type": "subscribe",
        "channel": "level2",
        "product_ids": ["BTC-USDT"]
    }

    def __init__(self) -> None:
        super().__init__(self.EXCHANGE_NAME, host=self.HOST, port=self.PORT, path=self.PATH, subscription=self.SUBSCRIPTION)

    def check(self, message: Any) -> bool:
        return 'channel' in message and message.get('channel') == 'l2_data'

    def process(self, message: Any) -> None:
        message = Message.from_dict(message)
        for event in message.events:
            for update in event.updates:
                self._sink.on_market_update(event.product_id, update.side.name, update.price_level, update.new_quantity)


def main() -> None:
    subscriber = SubscriberCoinbase()
    asyncio.run(subscriber.subscribe())


if __name__ == '__main__':
    main()
