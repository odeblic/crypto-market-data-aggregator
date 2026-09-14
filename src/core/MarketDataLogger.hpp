#pragma once

#include "core/Display.hpp"
#include "core/Formatter.hpp"
#include "core/MarketDataSink.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"

class MarketDataLogger : public MarketDataSink
{
public:
    MarketDataLogger()
    {
    }

    virtual auto write(MarketUpdate const& update) -> bool override
    {
        Display::getInstance().show(toString(update));
        return true;
    }

private:
};
