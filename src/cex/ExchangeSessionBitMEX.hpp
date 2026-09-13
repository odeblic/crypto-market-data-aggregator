#pragma once

#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"
#include "ws/ClientSession.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <utility>

struct ExchangeSessionBitMEX : ClientSession
{
    template<typename... Args>
    explicit ExchangeSessionBitMEX(Args&&... args)
    : ClientSession(std::forward<Args>(args)...)
    {
    }

    virtual auto checkMessage(nlohmann::json const& msg) const -> bool override
    {
        if (!msg.contains("table") || msg["table"] != "orderBookL2_25")
        {
            return false;
        }

        if (!msg.contains("action"))
        {
            return false;
        }

        if (!msg.contains("data") && msg["data"].is_array())
        {
            return false;
        }

        return true;
    }

    virtual void processMessage(nlohmann::json const& msg) override
    {
        auto const exchange = Exchange::BITMEX;

        auto makeMarketUpdate = [&](Ticker const& ticker, Side side, double price, double quantity)
        {
            auto marketUpdate = MarketUpdate
            {
                .price = price,
                .quantity = quantity,
                .ticker = ticker,
                .side = side,
                .exchange = exchange,
            };
            return marketUpdate;
        };

        for (auto const& item : msg["data"])
        {
            auto const ticker = fromString<Ticker>(item["symbol"].get<std::string>());
            auto const side = fromString<Side>(item["side"].get<std::string>());
            auto const price = item["price"].get<double>();
            auto const quantity = item.value<long>("size", 0);
            auto const marketUpdate = makeMarketUpdate(ticker, side, price, quantity);
            publish(marketUpdate);
        }
    }
};
