#pragma once

#include "core/MarketUpdate.hpp"

#include <boost/lockfree/queue.hpp>

using MarketDataQueue = boost::lockfree::queue<MarketUpdate, boost::lockfree::capacity<1024>>;
