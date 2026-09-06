#pragma once

#include "ExchangeConfiguration.hpp"
#include "ExchangeSession.hpp"
#include "MarketUpdate.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <string>
#include <utility>

inline auto const configBitMEX = ExchangeConfiguration
{
    .host = "ws.bitmex.com",
    .port = 443,
    .path = "/realtime",
    .subscription = R"({
    "op": "subscribe",
    "args": [
        "orderBookL2_25:XBTUSDT"
    ]
})",
};

struct ExchangeSessionBitMEX : ExchangeSession
{
    template<typename... Args>
    explicit ExchangeSessionBitMEX(Args&&... args)
    : ExchangeSession(std::forward<Args>(args)...)
    {
    }
};
