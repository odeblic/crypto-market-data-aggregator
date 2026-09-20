#pragma once

#include "biz/PriceBands.hpp"
#include "core/Captions.hpp"
#include "core/Display.hpp"
#include "core/Formatter.hpp"
#include "core/Utils.hpp"
#include "rpc/MarketDataFormatter.hpp"
#include "rpc/MarketDataHandler.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <span>

class MarketDataHandlerPriceBands : public MarketDataHandler
{
public:
    MarketDataHandlerPriceBands(std::span<int const> bandValues, bool verbose)
    : bands(bandValues), verbose(verbose)
    {
        captions.title = "Price Bands";
        captions.maxLabelSize = 10;

        for (auto value : bandValues)
        {
            captions.askLabels.push_back(humanizeBps(value, false));
            captions.bidLabels.push_back(humanizeBps(value, true));
        }
    }

    virtual void onSnapshot(marketdata::Snapshot const& snapshot) override
    {
        if (verbose)
        {
            Display::getInstance().show(toString(snapshot));
        }

        auto [symbol, originalBook] = makeSnapshot(snapshot);
        auto syntheticBook = bands.compute(originalBook);
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
    PriceBands bands;
    Captions captions;
    bool verbose{false};
};
