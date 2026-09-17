#pragma once

#include "core/Exchange.hpp"
#include "core/Side.hpp"
#include "core/Ticker.hpp"

struct MarketUpdate
{
    double price{0.0};
    double quantity{0.0};
    Ticker ticker{};
    Side side{};
    Exchange exchange{};
};
