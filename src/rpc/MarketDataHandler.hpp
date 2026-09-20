#pragma once

#include "core/Book.hpp"
#include "core/Diff.hpp"
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

    virtual void onDiff(marketdata::Diff const& diff) const = 0;

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

    static auto makeDiff(marketdata::Diff const& diff) -> Diff
    {
        return
        {
            .price = diff.price(),
            .quantity = diff.quantity(),
            .ticker = fromString<Ticker>(diff.symbol()),
            .side = marketdata::Side::BID ? Side::BID : Side::ASK,
        };
    }
};
