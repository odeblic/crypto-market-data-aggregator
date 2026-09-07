#pragma once

#include "cex/ExchangeConfiguration.hpp"
#include "cex/ExchangeSession.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <utility>

inline auto const configKraken = ExchangeConfiguration
{
    .host = "ws.kraken.com",
    .port = 443,
    .path = "/",
    .subscription = R"({
    "event": "subscribe",
    "subscription": {
        "name": "book"
    },
    "pair": [
        "BTC/USDT"
    ]
})",
};

struct ExchangeSessionKraken : ExchangeSession
{
    template<typename... Args>
    explicit ExchangeSessionKraken(Args&&... args)
    : ExchangeSession(std::forward<Args>(args)...)
    {
    }

    virtual auto checkMessage(nlohmann::json const& msg) const -> bool override
    {
        if (!msg.is_array())
        {
            return false;
        }

        if (msg.size() != 4)
        {
            return false;
        }

        if (msg[2] != "book-10")
        {
            return false;
        }

        return true;
    }

    virtual void processMessage(nlohmann::json const& msg) override
    {
        auto const exchange = Exchange::KRAKEN;
        auto const ticker = fromString<Ticker>(msg[3].get<std::string>());

        nlohmann::json const& data{msg[1]};

        auto const parseUpdates = [&](nlohmann::json const & updates, Side const side)
        {
            for (auto const & entry : updates)
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

        if (data.contains("b"))
        {
            parseUpdates(data["b"], Side::BID);
        }

        if (data.contains("a"))
        {
            parseUpdates(data["a"], Side::ASK);
        }
    }
};
