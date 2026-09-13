#pragma once

#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"
#include "ws/ClientSession.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <utility>

struct ExchangeSessionHyperliquid : ClientSession
{
    template<typename... Args>
    explicit ExchangeSessionHyperliquid(Args&&... args)
    : ClientSession(std::forward<Args>(args)...)
    {
    }

    virtual auto checkMessage(nlohmann::json const& msg) const -> bool override
    {
        if (!msg.contains("channel") || msg["channel"] != "l2Book")
        {
            return false;
        }

        if (!msg.contains("data"))
        {
            return false;
        }

        return true;
    }

    virtual void processMessage(nlohmann::json const& msg) override
    {
        auto const exchange = Exchange::HYPERLIQUID;

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

        auto const ticker = fromString<Ticker>(msg["data"]["coin"].get<std::string>());

        for (auto const& item : msg["data"]["levels"][0])
        {
            auto const price = std::stod(item["px"].get<std::string>());
            auto const quantity = std::stod(item["sz"].get<std::string>());
            auto const marketUpdate = makeMarketUpdate(ticker, Side::BID, price, quantity);
            publish(marketUpdate);
        }

        for (auto const& item : msg["data"]["levels"][1])
        {
            auto const price = std::stod(item["px"].get<std::string>());
            auto const quantity = std::stod(item["sz"].get<std::string>());
            auto const marketUpdate = makeMarketUpdate(ticker, Side::ASK, price, quantity);
            publish(marketUpdate);
        }
    }
};
