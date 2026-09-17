#pragma once

#include "core/Errors.hpp"
#include "core/Exchange.hpp"
#include "core/Side.hpp"
#include "core/Ticker.hpp"

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <type_traits>
#include <string>
#include <string_view>

template <typename T>
static inline auto fromString(std::string_view str) -> T
{
    if constexpr (std::is_same_v<T, Side>)
    {
        auto const side = boost::to_upper_copy(std::string{str});

        if (side == "ASK" || side == "SELL" || side == "OFFER")
        {
            return Side::ASK;
        }
        else if (side == "BID" || side == "BUY")
        {
            return Side::BID;
        }
        else
        {
            throw InvalidSide(str);
        }
    }
    else if constexpr (std::is_same_v<T, Exchange>)
    {
        auto const exchange = boost::to_upper_copy(std::string{str});

        if (exchange == "BINANCE")
        {
            return Exchange::BINANCE;
        }
        else if (exchange == "BITMEX")
        {
            return Exchange::BITMEX;
        }
        else if (exchange == "BYBIT")
        {
            return Exchange::BYBIT;
        }
        else if (exchange == "COINBASE")
        {
            return Exchange::COINBASE;
        }
        else if (exchange == "HYPERLIQUID")
        {
            return Exchange::HYPERLIQUID;
        }
        else if (exchange == "INTERNAL")
        {
            return Exchange::INTERNAL;
        }
        else if (exchange == "KRAKEN")
        {
            return Exchange::KRAKEN;
        }
        else if (exchange == "OKX")
        {
            return Exchange::OKX;
        }
        else
        {
            throw InvalidExchange(str);
        }
    }
    else if constexpr (std::is_same_v<T, Ticker>)
    {
        auto ticker = Ticker{};
        auto length = std::min(Ticker().max_size() - 1, str.size());
        std::copy_n(str.cbegin(), length, ticker.begin());
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
        throw InvalidSide(static_cast<int>(side));
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
    case Exchange::HYPERLIQUID:
        return "HYPERLIQUID";
    case Exchange::INTERNAL:
        return "INTERNAL";
    case Exchange::KRAKEN:
        return "KRAKEN";
    case Exchange::OKX:
        return "OKX";
    default:
        throw InvalidExchange(static_cast<int>(exchange));
    }
}

static inline auto toString(Ticker const& ticker) -> std::string
{
    return std::string(ticker.begin(), ticker.end());
}
