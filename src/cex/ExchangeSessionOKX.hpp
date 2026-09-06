#pragma once

#include "ExchangeConfiguration.hpp"
#include "ExchangeSession.hpp"
#include "MarketUpdate.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <string>
#include <utility>

inline auto const configOKX = ExchangeConfiguration
{
    .host = "ws.okx.com",
    .port = 8443,
    .path = "/ws/v5/public",
    .subscription = R"({
    "op": "subscribe",
    "args": [
        {
            "channel": "books",
            "instId": "BTC-USDT"
        }
    ]
})",
};

struct ExchangeSessionOKX : ExchangeSession
{
    template<typename... Args>
    explicit ExchangeSessionOKX(Args&&... args)
    : ExchangeSession(std::forward<Args>(args)...)
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
        auto const ticker = std::string(msg["arg"]["instId"]).substr(0, Ticker().max_size() - 1);

        auto makeMarketUpdate = [&](Side side, double price, double quantity)
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
