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
            std::string product_id = event.at("product_id").get<std::string>();
            Ticker ticker{};
            std::copy_n(product_id.begin(), std::min(product_id.size(), ticker.size()), ticker.begin());

            auto const& updates = event.at("updates");

            for (auto const& update : updates)
            {
                MarketUpdate marketUpdate;
                marketUpdate.ticker = ticker;

                std::string sideStr = update.at("side").get<std::string>();
                marketUpdate.side = (sideStr == "bid") ? Side::BID : Side::ASK;

                marketUpdate.price = std::stod(update.at("price_level").get<std::string>());
                marketUpdate.quantity = std::stod(update.at("new_quantity").get<std::string>());
                marketUpdate.exchange = exchange;

                publish(marketUpdate);
            }
        }
    }
};
