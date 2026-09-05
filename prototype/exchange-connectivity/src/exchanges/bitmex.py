import asyncio
from dataclasses import dataclass
from dataclasses_json import dataclass_json
from typing import Any, Optional
from enum import Enum
import utils


class Action(Enum):
    PARTIAL = "partial"
    INSERT = "insert"
    UPDATE = "update"
    DELETE = "delete"


class Side(Enum):
    ASK = "Sell"
    BID = "Buy"


@dataclass_json
@dataclass
class Update:
    symbol: str
    side: Side
    price: float
    size: Optional[int] = None


@dataclass_json
@dataclass
class Message:
    action: Action
    data: list[Update]


class SubscriberBitMEX(utils.MarketDataSubscriber):
    EXCHANGE_NAME = 'BITMEX'
    # API_URL = 'wss://bitmex.com'
    HOST = 'ws.bitmex.com'
    PORT = 443
    PATH = 'realtime'
    SUBSCRIPTION = {
        "op": "subscribe",
        "args": ["orderBookL2_25:XBTUSDT"],
    }

    def __init__(self) -> None:
        super().__init__(self.EXCHANGE_NAME, host=self.HOST, port=self.PORT, path=self.PATH, subscription=self.SUBSCRIPTION)

    def check(self, message: Any) -> bool:
        return 'table' in message and message.get('table') == 'orderBookL2_25' and \
               'action' in message and message.get('action') in Action

    def process(self, message: Any) -> None:
        message = Message.from_dict(message)
        # TODO: rely on the specified action
        if True or message.action == Action.PARTIAL:
            for update in message.data:
                if update.size is None:
                    self._sink.on_market_update(update.symbol, update.side.name, update.price, 0.0)
                else:
                    self._sink.on_market_update(update.symbol, update.side.name, update.price, update.size)


def main() -> None:
    subscriber = SubscriberBitMEX()
    asyncio.run(subscriber.subscribe())


if __name__ == '__main__':
    main()
