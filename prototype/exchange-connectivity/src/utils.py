import json
import websockets
from enum import StrEnum
from pathlib import Path
from typing import Any
from abc import ABC, abstractmethod

DISPLAY_RAW_MESSAGES = False


class Side(StrEnum):
    ASK = "ask"
    BID = "bid"


class MarketDataSink:
    __source_name: str
    __file_path: Path

    def __init__(self, name: str) -> None:
        self.__source_name = name
        self.__file_path = Path(f'{name.lower()}.csv')
        self.__create_file()

    def __create_file(self) -> None:
        with open(self.__file_path, 'w') as f:
            f.write(f'TICKER,SIDE,PRICE,QUANTITY,SOURCE\n')

    def __write_file(self, ticker: str, side: Side, price: float, quantity: float) -> None:
        with open(self.__file_path, 'a') as f:
            f.write(f'{ticker},{side.value},{price},{quantity},{self.__source_name.lower()}\n')

    def __display(self, ticker: str, side: Side, price: float, quantity: float) -> None:
        print(f'Ticker: \033[35m{ticker:.^12}\033[0m, ', end='')
        if side == Side.ASK:
            print(f'Side: \033[31m{side.name}\033[0m, ', end='')
        else:
            print(f'Side: \033[32m{side.name}\033[0m, ', end='')
        print(f'Price: \033[33m{price:,.4f}\033[0m, ', end='')
        print(f'Quantity: \033[33m{quantity:,.12f}\033[0m, ', end='')
        print(f'Source: \033[34m{self.__source_name.upper()}\033[0m', end='')
        print()

    def on_market_update(self, ticker: str, side: Side | str, price: float, quantity: float) -> None:
        if isinstance(side, str):
            side = Side(side.lower())
        self.__display(ticker, side, price, quantity)
        self.__write_file(ticker, side, price, quantity)


class MarketDataSubscriber(ABC):
    __name: str
    __host: str
    __port: int
    __path: str
    __subscription: Any
    _sink: MarketDataSink

    def __init__(self, name: str, *, host: str, port: int | None = None, path: str | None = None, subscription: Any = None) -> None:
        self.__name = name.lower()
        self.__host = host
        self.__port = port
        self.__path = path
        self.__subscription = subscription
        self._sink = MarketDataSink(name)

    @property
    def _url(self) -> str:
        url = f'wss://{self.__host}'
        if self.__port is not None:
            url += f':{self.__port}'
        if self.__path is not None:
            url += f'/{self.__path}'
        return url

    async def subscribe(self):
        print(f'Connecting to {self._url}...')
        async with websockets.connect(self._url) as websocket:
            print('Connected!\n')
            if self.subscribe is not None:
                print('Sending subscription request...\n')
                await websocket.send(json.dumps(self.__subscription))
                print('Subscription request sent.\n')
            else:
                print('No subscription request needed.\n')
            print('Awaiting data...\n')
            try:
                async for message in websocket:
                    self.on_message(json.loads(message))
            except websockets.ConnectionClosed as e:
                print(f"Connection closed: {e}")
            # except Exception as e:
            #     print(f"An error occurred: {e}")

    def on_message(self, message: Any) -> None:
        if DISPLAY_RAW_MESSAGES:
            display_raw(message)
        if self.check(message):
            self.process(message)

    @abstractmethod
    def check(self, message: Any) -> bool:
        raise NotImplemented

    @abstractmethod
    def process(self, message: Any) -> None:
        raise NotImplemented


def display_raw(message: Any) -> None:
    msg = json.dumps(message, indent=2)
    print(f'\033[30;1m{msg}\033[0m')
    print("-" * 60)
