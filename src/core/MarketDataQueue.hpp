#pragma once

#include "core/MarketUpdate.hpp"

#include <boost/lockfree/queue.hpp>

typedef boost::lockfree::queue<MarketUpdate, boost::lockfree::capacity<1024>> MarketDataQueue;
