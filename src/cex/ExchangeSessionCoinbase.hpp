#pragma once

#include "ExchangeConfiguration.hpp"
#include "ExchangeSession.hpp"
#include "MarketUpdate.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <string>
#include <utility>

inline auto const configCoinbase = ExchangeConfiguration
{
    .host = "advanced-trade-ws.coinbase.com",
    .port = 443,
    .path = "/",
    .subscription = R"({
    "type": "subscribe",
    "channel": "level2",
    "product_ids": [
        "BTC-USDT"
    ]
})",
};

struct ExchangeSessionCoinbase : ExchangeSession
{
    template<typename... Args>
    explicit ExchangeSessionCoinbase(Args&&... args)
    : ExchangeSession(std::forward<Args>(args)...)
    {
    }
};
