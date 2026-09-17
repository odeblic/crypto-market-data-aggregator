#pragma once

#include <optional>

struct Stats
{
    std::optional<double> currentAskPrice;
    std::optional<double> currentBidPrice;
    std::optional<double> spread;
    std::optional<bool> crossedBook;
    std::optional<double> lowestAskPrice;
    std::optional<double> highestBidPrice;
    size_t askLevelCount{0};
    size_t bidLevelCount{0};
};
