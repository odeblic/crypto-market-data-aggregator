#pragma once

#include <array>

using Ticker = std::array<char, 16>;

enum class Side : bool
{
    ASK,
    BID,
};

enum class Exchange : char
{
    BINANCE,
    BITMEX,
    BYBIT,
    COINBASE,
    HYPERLIQUID,
    INTERNAL,
    KRAKEN,
    OKX,
    MAX, // keep MAX at the end
};

struct MarketUpdate
{
    double price{0.0};
    double quantity{0.0};
    Ticker ticker{};
    Side side{};
    Exchange exchange{};
};
