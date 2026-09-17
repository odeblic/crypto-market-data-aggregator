#pragma once

#include "biz/NotionalVolumeBands.hpp"
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
    }

    virtual void onSnapshot(marketdata::Snapshot const& snapshot) const override
    {
        if (verbose)
        {
            Display::getInstance().show(toString(snapshot));
        }

        auto originalBook = makeBook(snapshot);
        auto syntheticBook = bands.compute(originalBook);
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
    NotionalVolumeBands bands;
    bool verbose{false};
};
