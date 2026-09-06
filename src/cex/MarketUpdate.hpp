#pragma once

#include <array>

typedef std::array<char, 16> Ticker;

enum class Side : bool
{
    ASK,
    BID,
};

enum class Exchange : char
{
    UNKNOWN,
    BINANCE,
    BITMEX,
    BYBIT,
    COINBASE,
    KRAKEN,
    OKX,
};

struct MarketUpdate
{
    double price{0.0};
    double quantity{0.0};
    Ticker ticker{};
    Side side{};
    Exchange exchange{};
};
