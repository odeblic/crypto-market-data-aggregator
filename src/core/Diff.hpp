#pragma once

#include "core/Side.hpp"
#include "core/Ticker.hpp"

struct Diff
{
    double price{0.0};
    double quantity{0.0};
    Ticker ticker{};
    Side side{};
};
