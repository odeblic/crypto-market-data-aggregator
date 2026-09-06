#pragma once

#include "ExchangeConfiguration.hpp"
#include "ExchangeSession.hpp"
#include "MarketUpdate.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <string>
#include <utility>

inline auto const configBinance = ExchangeConfiguration
{
    .host = "stream.binance.com",
    .port = 9443,
    .path = "/ws/btcusdt@depth@100ms",
    .subscription = R"({
    "method": "SUBSCRIBE",
    "params": [
        "btcusdt@depth20@100ms"
    ],
    "id": 1
})",
};

struct ExchangeSessionBinance : ExchangeSession
{
    template<typename... Args>
    explicit ExchangeSessionBinance(Args&&... args)
    : ExchangeSession(std::forward<Args>(args)...)
    {
    }
};
