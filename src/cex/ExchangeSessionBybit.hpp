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

    virtual auto checkMessage(nlohmann::json const& msg) const -> bool override
    {
        if (!msg.contains("topic") || msg["topic"] != "orderbook.50.BTCUSDT")
        {
            return false;
        }

        if (!msg.contains("data") || !msg["data"].is_object())
        {
            return false;
        }

        return true;
    }

    virtual void processMessage(nlohmann::json const& msg) override
    {
        auto const exchange = Exchange::BYBIT;

        Ticker ticker{};

        if (msg.contains("data") && msg["data"].contains("s"))
        {
            std::string const symbol = msg["data"]["s"].get<std::string>();
            std::size_t const copyLen = std::min(symbol.size(), ticker.size());
            std::copy_n(symbol.begin(), copyLen, ticker.begin());
        }

        auto const processSide = [this, &ticker](nlohmann::json const& entries, Side const side)
        {
            for (auto const& entry : entries)
            {
                MarketUpdate update;
                update.price = std::stod(entry[0].get<std::string>());
                update.quantity = std::stod(entry[1].get<std::string>());
                update.ticker = ticker;
                update.side = side;
                update.exchange = exchange;
                publish(update);
            }
        };

        if (msg.contains("data"))
        {
            nlohmann::json const& data = msg["data"];
            if (data.contains("a"))
            {
                processSide(data["a"], Side::ASK);
            }
            if (data.contains("b"))
            {
                processSide(data["b"], Side::BID);
            }
        }
    }
};
