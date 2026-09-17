#pragma once

#include "core/Book.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Side.hpp"
#include "core/Ticker.hpp"
#include "core/Utils.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <memory>

class MarketDataHandler
{
public:
    virtual void onSnapshot(marketdata::Snapshot const& snapshot) const = 0;

    virtual void onUpdate(marketdata::Update const& update) const = 0;

protected:
    static auto makeBook(marketdata::Snapshot const& snapshot) -> Book
    {
        auto book = Book{};

        for (auto const& ask : snapshot.asks())
        {
            book.ask.emplace_back(ask.price(), ask.quantity());
        }

        for (auto const& bid : snapshot.bids())
        {
            book.bid.emplace_back(bid.price(), bid.quantity());
        }

        return book;
    }

    static auto makeMarketUpdate(marketdata::Update const& update) -> MarketUpdate
    {
        return
        {
            .price = update.price(),
            .quantity = update.quantity(),
            .ticker = fromString<Ticker>(update.symbol()),
            .side = marketdata::Side::BID ? Side::BID : Side::ASK,
        };
    }
};
