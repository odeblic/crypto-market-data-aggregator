#pragma once

#include "MarketUpdate.hpp"

#include <string>

static inline auto toString(Exchange exchange) -> std::string
{
    switch (exchange)
    {
    case Exchange::BINANCE:
        return "BINANCE";
    case Exchange::BITMEX:
        return "BITMEX";
    case Exchange::BYBIT:
        return "BYBIT";
    case Exchange::COINBASE:
        return "COINBASE";
    case Exchange::KRAKEN:
        return "KRAKEN";
    case Exchange::OKX:
        return "OKX";
    case Exchange::UNKNOWN:
        return "UNKNOWN";
    default:
        return "";
    }
}

static inline auto toString(Ticker const& ticker) -> std::string
{
    return std::string(ticker.begin(), ticker.end());
}
