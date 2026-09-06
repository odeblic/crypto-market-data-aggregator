#pragma once

#include "ExchangeConfiguration.hpp"
#include "ExchangeSession.hpp"
#include "MarketUpdate.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <string>
#include <utility>

inline auto const configOKX = ExchangeConfiguration
{
    .host = "ws.okx.com",
    .port = 8443,
    .path = "/ws/v5/public",
    .subscription = R"({
    "op": "subscribe",
    "args": [
        {
            "channel": "books",
            "instId": "BTC-USDT"
        }
    ]
})",
};

struct ExchangeSessionOKX : ExchangeSession
{
    template<typename... Args>
    explicit ExchangeSessionOKX(Args&&... args)
    : ExchangeSession(std::forward<Args>(args)...)
    {
    }
};
