#pragma once

#include "cex/ExchangeConfiguration.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"
#include "ws/ClientSession.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <utility>

inline auto const configInternal = ExchangeConfiguration
{
    .host = "127.0.0.1",
    .port = 8080,
    .path = "/marketdata/btcusdt",
    .subscription = "",
};

struct ExchangeSessionInternal : ClientSession
{
    template<typename... Args>
    explicit ExchangeSessionInternal(Args&&... args)
    : ClientSession(std::forward<Args>(args)...)
    {
    }

    virtual auto checkMessage(nlohmann::json const& msg) const -> bool override
    {
        return true;
    }

    virtual void processMessage(nlohmann::json const& msg) override
    {
        auto const exchange = Exchange::INTERNAL;
        auto const ticker = fromString<Ticker>(msg["ticker"].get<std::string>());
        auto const side = fromString<Side>(msg["side"].get<std::string>());
        auto const price = msg["price"].get<double>();
        auto const quantity = msg["quantity"].get<double>();

        auto marketUpdate = MarketUpdate
        {
            .price = price,
            .quantity = quantity,
            .ticker = ticker,
            .side = side,
            .exchange = exchange,
        };

        publish(marketUpdate);
    }
};
