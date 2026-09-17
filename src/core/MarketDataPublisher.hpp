#pragma once

#include "core/MarketUpdate.hpp"
#include "core/MarketDataQueue.hpp"
#include "core/MarketDataSink.hpp"

#include <functional>

class MarketDataPublisher : public MarketDataSink
{
public:
    MarketDataPublisher(MarketDataQueue& queue)
    : queue(queue)
    {
    }

    virtual auto write(MarketUpdate const& update) -> bool override
    {
        return queue.get().push(update);
    }

private:
    std::reference_wrapper<MarketDataQueue> queue;
};
