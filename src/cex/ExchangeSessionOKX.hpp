#pragma once

#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"
#include "ws/ClientSession.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <utility>

struct ExchangeSessionOKX : ClientSession
{
    template<typename... Args>
    explicit ExchangeSessionOKX(Args&&... args)
    : ClientSession(std::forward<Args>(args)...)
    {
    }

    virtual auto checkMessage(nlohmann::json const& msg) const -> bool override
    {
        if (!msg.contains("arg") || !msg["arg"].contains("channel") || msg["arg"]["channel"] != "books")
        {
            return false;
        }

        if (!msg.contains("data") || !msg["data"].is_array())
        {
            return false;
        }

        return true;
    }

    virtual void processMessage(nlohmann::json const& msg) override
    {
        auto const exchange = Exchange::OKX;
        auto const ticker = fromString<Ticker>(msg["arg"]["instId"].get<std::string>());

        auto makeMarketUpdate = [&](Side side, double price, double quantity)
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
            if (item.contains("asks"))
            {
                for (auto const& ask : item["asks"])
                {
                    if (ask.size() >= 2)
                    {
                        auto const marketUpdate = makeMarketUpdate(
                            Side::ASK,
                            std::stod(ask[0].get<std::string>()),
                            std::stod(ask[1].get<std::string>())
                        );
                        publish(marketUpdate);
                    }
                }
            }

            if (item.contains("bids"))
            {
                for (auto const& bid : item["bids"])
                {
                    if (bid.size() >= 2)
                    {
                        auto const marketUpdate = makeMarketUpdate(
                            Side::BID,
                            std::stod(bid[0].get<std::string>()),
                            std::stod(bid[1].get<std::string>())
                        );
                        publish(marketUpdate);
                    }
                }
            }
        }
    }
};
