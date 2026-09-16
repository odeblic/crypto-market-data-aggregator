#pragma once

#include "core/MarketUpdate.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>

class MarketDataQueue
{
public:
    auto push(MarketUpdate const& update) -> bool
    {
        {
            std::lock_guard lock(mutex);

            if (shutdown)
            {
                return false;
            }

            queue.push(update);
        }

        condition.notify_one();
        return true;
    }

    auto pop(MarketUpdate& update) -> bool
    {
        std::lock_guard lock(mutex);

        if (queue.empty())
        {
            return false;
        }

        update = std::move(queue.front());
        queue.pop();
        return true;
    }

    auto waitPop(MarketUpdate& update) -> bool
    {
        std::unique_lock lock(mutex);
        condition.wait(lock, [this]() { return shutdown || !queue.empty(); });

        if (shutdown && queue.empty())
        {
            return false;
        }

        update = std::move(queue.front());
        queue.pop();
        return true;
    }

    void shutdownQueue()
    {
        {
            std::lock_guard lock(mutex);
            shutdown = true;
        }

        condition.notify_all();
    }

private:
    std::mutex mutex;
    std::condition_variable condition;
    std::queue<MarketUpdate> queue;
    bool shutdown{false};
};
