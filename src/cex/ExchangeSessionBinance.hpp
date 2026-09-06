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

    virtual auto checkMessage(nlohmann::json const& msg) const -> bool override
    {
        if (!msg.contains("asks"))
        {
            return false;
        }

        if (!msg.contains("bids"))
        {
            return false;
        }

        return true;
    }

    virtual void processMessage(nlohmann::json const& msg) override
    {
        auto const exchange = Exchange::BINANCE;

        auto const parseAndPublish = [this](nlohmann::json const & entries, Side const side)
        {
            if (entries.is_array() == false)
            {
                return;
            }

            for (auto const & entry : entries)
            {
                if (entry.is_array() && entry.size() >= 2)
                {
                    MarketUpdate marketUpdate{};
                    marketUpdate.price = std::stod(entry[0].get<std::string>());
                    marketUpdate.quantity = std::stod(entry[1].get<std::string>());
                    marketUpdate.side = side;
                    marketUpdate.exchange = exchange;

                    publish(marketUpdate);
                }
            }
        };

        if (msg.contains("asks"))
        {
            parseAndPublish(msg["asks"], Side::ASK);
        }

        if (msg.contains("bids"))
        {
            parseAndPublish(msg["bids"], Side::BID);
        }
    }
};
