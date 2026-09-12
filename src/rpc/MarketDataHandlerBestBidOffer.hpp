#pragma once

#include "core/Book.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"
#include "rpc/MarketDataHandler.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <memory>

class MarketDataHandlerBestBidOffer : public MarketDataHandler
{
public:
    virtual void onSnapshot(marketdata::Snapshot const& snapshot) const override
    {
        //printSnapshot(snapshot);
        auto book = makeBook(snapshot);
        auto syntheticBook = compute(*book);
        printBook(*syntheticBook);
    }

    virtual void onUpdate(marketdata::Update const& update) const override
    {
        //printUpdate(update);
    }

private:
    static auto compute(Book const& book) -> std::unique_ptr<Book>
    {
        auto bestBidOffer = book;
        bestBidOffer.ask.resize(1);
        bestBidOffer.bid.resize(1);
        return std::make_unique<Book>(bestBidOffer);
    }
};
