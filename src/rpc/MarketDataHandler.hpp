#pragma once

#include "core/Book.hpp"
#include "core/Diff.hpp"
#include "core/Side.hpp"
#include "core/Snapshot.hpp"
#include "core/Ticker.hpp"
#include "core/Utils.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

class MarketDataHandler
{
public:
    virtual void onSnapshot(marketdata::Snapshot const& snapshot) = 0;

    virtual void onDiff(marketdata::Diff const& diff) = 0;

protected:
    static auto makeSnapshot(marketdata::Snapshot const& snapshot) -> Snapshot
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

        return
        {
            .symbol = snapshot.symbol(),
            .book = book,
        };
    }

    static auto makeDiff(marketdata::Diff const& diff) -> Diff
    {
        return
        {
            .symbol = diff.symbol(),
            .price = diff.price(),
            .quantity = diff.quantity(),
            .side = marketdata::Side::BID ? Side::BID : Side::ASK,
        };
    }
};
