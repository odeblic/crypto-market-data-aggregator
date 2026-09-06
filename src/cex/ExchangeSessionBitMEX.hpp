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

        auto makeMarketUpdate = [&](std::string ticker, Side side, double price, double quantity)
        {
            auto marketUpdate = MarketUpdate
            {
                .price = price,
                .quantity = quantity,
                .side = side,
                .exchange = exchange,
            };

            std::copy(ticker.cbegin(), ticker.cend(), marketUpdate.ticker.begin());
            marketUpdate.ticker.back() = '\0';
            return marketUpdate;
        };

        auto parseSide = [&](std::string side) -> Side
        {
            if (side == "Sell")
            {
                return Side::ASK;
            }
            else if (side == "Buy")
            {
                return Side::BID;
            }
            else
            {
                throw std::runtime_error("bad side");
            }
        };

        for (auto const& item : msg["data"])
        {
            auto const ticker = std::string(item["symbol"]).substr(0, Ticker().max_size() - 1);
            auto const side = parseSide(item["side"].get<std::string>());
            auto const price = item["price"].get<double>();
            auto const quantity = item.value<long>("size", 0);
            auto const marketUpdate = makeMarketUpdate(ticker, side, price, quantity);
            publish(marketUpdate);
        }
    }
};
