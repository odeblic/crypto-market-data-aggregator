#pragma once

#include "ExchangeConfiguration.hpp"
#include "ExchangeSession.hpp"
#include "MarketUpdate.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <string>
#include <utility>

inline auto const configKraken = ExchangeConfiguration
{
    .host = "ws.kraken.com",
    .port = 443,
    .path = "/",
    .subscription = R"({
    "event": "subscribe",
    "subscription": {
        "name": "book"
    },
    "pair": [
        "BTC/USDT"
    ]
})",
};

struct ExchangeSessionKraken : ExchangeSession
{
    template<typename... Args>
    explicit ExchangeSessionKraken(Args&&... args)
    : ExchangeSession(std::forward<Args>(args)...)
    {
    }
};
