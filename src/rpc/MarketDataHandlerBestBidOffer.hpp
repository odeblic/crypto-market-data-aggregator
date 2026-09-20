#pragma once

#include "biz/BestBidOffer.hpp"
#include "core/Captions.hpp"
#include "core/Display.hpp"
#include "core/Formatter.hpp"
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
        captions.title = "Best Bid Offer";
    }

    virtual void onSnapshot(marketdata::Snapshot const& snapshot) override
    {
        if (verbose)
        {
            Display::getInstance().show(toString(snapshot));
        }

        auto [symbol, originalBook] = makeSnapshot(snapshot);
        auto syntheticBook = computeBestBidOffer(originalBook);
        captions.symbol = symbol;
        Display::getInstance().show(toString(syntheticBook, captions));
    }

    virtual void onDiff(marketdata::Diff const& diff) override
    {
        if (verbose)
        {
            Display::getInstance().show(toString(diff));
        }
    }

private:
    bool verbose{false};
    Captions captions;
};
