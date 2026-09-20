#pragma once

#include "core/Side.hpp"

struct Diff
{
    std::string symbol;
    double price{0.0};
    double quantity{0.0};
    Side side{};
};
