#pragma once

#include "core/MarketUpdate.hpp"

class MarketDataSink
{
public:
    MarketDataSink()
    {
    }

    virtual ~MarketDataSink() = default;

    virtual auto write(MarketUpdate const& update) -> bool = 0;

private:
};
