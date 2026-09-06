#pragma once

#include "ExchangeConfiguration.hpp"
#include "ExchangeSession.hpp"
#include "MarketUpdate.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <string>
#include <utility>

inline auto const configBybit = ExchangeConfiguration
{
    .host = "stream.bybit.com",
    .port = 443,
    .path = "/v5/public/linear",
    .subscription = R"({
    "op": "subscribe",
    "args": [
        "orderbook.50.BTCUSDT"
    ]
})",
};

struct ExchangeSessionBybit : ExchangeSession
{
    template<typename... Args>
    explicit ExchangeSessionBybit(Args&&... args)
    : ExchangeSession(std::forward<Args>(args)...)
    {
    }
};
