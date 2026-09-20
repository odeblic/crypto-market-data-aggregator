#pragma once

#include "biz/NotionalVolumeBands.hpp"
#include "core/Captions.hpp"
#include "core/Display.hpp"
#include "core/Formatter.hpp"
#include "core/Utils.hpp"
#include "rpc/MarketDataFormatter.hpp"
#include "rpc/MarketDataHandler.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <span>

class MarketDataHandlerNotionalVolumeBands : public MarketDataHandler
{
public:
    MarketDataHandlerNotionalVolumeBands(std::span<double const> bandValues, bool verbose)
    : bands(bandValues), verbose(verbose)
    {
        captions.title = "Notional Volume Bands";
        captions.maxLabelSize = 6;

        for (auto value : bandValues)
        {
            auto const str = humanizeNotional(value);
            captions.askLabels.push_back(str);
            captions.bidLabels.push_back(str);
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
    NotionalVolumeBands bands;
    Captions captions;
    bool verbose{false};
};
