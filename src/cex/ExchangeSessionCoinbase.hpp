#pragma once

#include "cex/ExchangeConfiguration.hpp"
#include "cex/ExchangeSession.hpp"
#include "cex/MarketUpdate.hpp"
#include "cex/Utils.hpp"

#include <nlohmann/json.hpp>

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

    virtual auto checkMessage(nlohmann::json const& msg) const -> bool override
    {
        if (!msg.contains("channel") || msg["channel"] != "l2_data")
        {
            return false;
        }

        if (!msg.contains("events") && msg["events"].is_array())
        {
            return false;
        }

        return true;
    }

    virtual void processMessage(nlohmann::json const& msg) override
    {
        auto const exchange = Exchange::COINBASE;
        auto const& events = msg.at("events");

        for (auto const& event : events)
        {
            auto const ticker = fromString<Ticker>(event.at("product_id").get<std::string>());
            auto const& updates = event.at("updates");

            for (auto const& update : updates)
            {
                MarketUpdate marketUpdate;
                marketUpdate.ticker = ticker;
                marketUpdate.side = fromString<Side>(update.at("side").get<std::string>());
                marketUpdate.price = std::stod(update.at("price_level").get<std::string>());
                marketUpdate.quantity = std::stod(update.at("new_quantity").get<std::string>());
                marketUpdate.exchange = exchange;
                publish(marketUpdate);
            }
        }
    }
};
