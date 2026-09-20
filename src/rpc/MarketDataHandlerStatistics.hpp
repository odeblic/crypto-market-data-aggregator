#pragma once

#include "biz/Statistics.hpp"
#include "core/Display.hpp"
#include "core/Formatter.hpp"
#include "core/Utils.hpp"
#include "rpc/MarketDataFormatter.hpp"
#include "rpc/MarketDataHandler.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

class MarketDataHandlerStatistics : public MarketDataHandler
{
public:
    MarketDataHandlerStatistics(bool verbose)
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
        auto values = statistics.compute(originalBook);
        Display::getInstance().show(toString(values));
    }

    virtual void onDiff(marketdata::Diff const& diff) const override
    {
        if (verbose)
        {
            Display::getInstance().show(toString(diff));
        }
    }

private:
    bool verbose{false};
    // TODO: amend the design to avoid the mutable
    mutable Statistics statistics;
};
