#pragma once

#include "core/Exchange.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Side.hpp"
#include "core/Ticker.hpp"
#include "core/Utils.hpp"
#include "ws/ClientSession.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <utility>

struct ExchangeSessionBinance : ClientSession
{
    template<typename... Args>
    explicit ExchangeSessionBinance(Args&&... args)
    : ClientSession(std::forward<Args>(args)...)
    {
    }

    virtual auto checkMessage(nlohmann::json const& msg) const -> bool override
    {
        if (!msg.contains("e") || msg["e"] != "depthUpdate")
        {
            return false;
        }

        if (!msg.contains("a"))
        {
            return false;
        }

        if (!msg.contains("b"))
        {
            return false;
        }

        return true;
    }

    virtual void processMessage(nlohmann::json const& msg) override
    {
        auto const exchange = Exchange::BINANCE;
        auto const ticker = fromString<Ticker>(msg["s"].get<std::string>());

        auto const parseAndPublish = [this, &ticker](nlohmann::json const& entries, Side const side)
        {
            if (entries.is_array() == false)
            {
                return;
            }

            for (auto const& entry : entries)
            {
                if (entry.is_array() && entry.size() >= 2)
                {
                    MarketUpdate marketUpdate{};
                    marketUpdate.price = std::stod(entry[0].get<std::string>());
                    marketUpdate.quantity = std::stod(entry[1].get<std::string>());
                    marketUpdate.ticker = ticker;
                    marketUpdate.side = side;
                    marketUpdate.exchange = exchange;
                    publish(marketUpdate);
                }
            }
        };

        if (msg.contains("a"))
        {
            parseAndPublish(msg["a"], Side::ASK);
        }

        if (msg.contains("b"))
        {
            parseAndPublish(msg["b"], Side::BID);
        }
    }
};
