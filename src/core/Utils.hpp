#pragma once

#include "core/MarketUpdate.hpp"

#include <algorithm>
#include <cctype>
#include <type_traits>
#include <stdexcept>
#include <string>

template <typename T>
static inline auto fromString(std::string str) -> T
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::toupper(c);
    });

    if constexpr (std::is_same_v<T, Side>)
    {
        if (str == "ASK" || str == "SELL" || str == "OFFER")
        {
            return Side::ASK;
        }
        else if (str == "BID" || str == "BUY")
        {
            return Side::BID;
        }
        else
        {
            throw std::runtime_error("invalid side: " + str);
        }
    }
    else if constexpr (std::is_same_v<T, Exchange>)
    {
        if (str == "UNKNOWN")
        {
            return Exchange::UNKNOWN;
        }
        else if (str == "BINANCE")
        {
            return Exchange::BINANCE;
        }
        else if (str == "BITMEX")
        {
            return Exchange::BITMEX;
        }
        else if (str == "BYBIT")
        {
            return Exchange::BYBIT;
        }
        else if (str == "COINBASE")
        {
            return Exchange::COINBASE;
        }
        else if (str == "KRAKEN")
        {
            return Exchange::KRAKEN;
        }
        else if (str == "OKX")
        {
            return Exchange::OKX;
        }
        else
        {
            throw std::runtime_error("invalid exchange: " + str);
        }
    }
    else if constexpr (std::is_same_v<T, Ticker>)
    {
        if (str.size() > Ticker().max_size() - 1)
        {
            str.resize(Ticker().max_size() - 1);
        }

        Ticker ticker{};
        std::copy(str.cbegin(), str.cend(), ticker.begin());
        return ticker;
    }
    else
    {
        return T{};
    }

    static_assert(std::is_same_v<T, Side> || std::is_same_v<T, Ticker> || std::is_same_v<T, Exchange>, "not implemented for this type");
}

static inline auto toString(Side side) -> std::string
{
    switch (side)
    {
    case Side::ASK:
        return "ASK";
    case Side::BID:
        return "BID";
    default:
        return "?";
    }
}

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
        return "?";
    }
}

static inline auto toString(Ticker const& ticker) -> std::string
{
    return std::string(ticker.begin(), ticker.end());
}
