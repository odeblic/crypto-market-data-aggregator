#pragma once

#include "biz/BestBidOffer.hpp"
#include "core/Display.hpp"
#include "core/Formatter.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"
#include "rpc/MarketDataFormatter.hpp"
#include "rpc/MarketDataHandler.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

class MarketDataHandlerBestBidOffer : public MarketDataHandler
{
public:
    MarketDataHandlerBestBidOffer(bool verbose)
    : verbose(verbose)
    {
    }

    virtual void onSnapshot(marketdata::Snapshot const& snapshot) const override
    {
        if (verbose)
        {
            Display::getInstance().show(toString(snapshot));
        }

        auto originalBook = makeBook(snapshot);
        auto syntheticBook = computeBestBidOffer(originalBook);
        Display::getInstance().show(toString(syntheticBook));
    }

    virtual void onUpdate(marketdata::Update const& update) const override
    {
        if (verbose)
        {
            Display::getInstance().show(toString(update));
        }
    }

private:
    bool verbose{false};
};
