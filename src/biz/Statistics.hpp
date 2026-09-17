#pragma once

#include "core/Book.hpp"
#include "core/Stats.hpp"

#include <algorithm>
#include <optional>

class Statistics
{
public:
    auto compute(Book const& book) -> Stats
    {
        stats.askLevelCount = book.ask.size();
        stats.bidLevelCount = book.bid.size();

        if (book.ask.size() > 0)
        {
            stats.currentAskPrice = book.ask[0].price;
        }

        if (book.bid.size() > 0)
        {
            stats.currentBidPrice = book.bid[0].price;
        }

        if (!stats.lowestAskPrice && stats.currentAskPrice)
        {
            stats.lowestAskPrice = stats.currentAskPrice.value();
        }

        if (!stats.highestBidPrice && stats.currentBidPrice)
        {
            stats.highestBidPrice = stats.currentBidPrice.value();
        }

        for (auto const& level : book.ask)
        {
            stats.lowestAskPrice = std::min(stats.lowestAskPrice.value(), level.price);
        }

        for (auto const& level : book.bid)
        {
            stats.highestBidPrice = std::max(stats.highestBidPrice.value(), level.price);
        }

        if (stats.currentAskPrice && stats.currentBidPrice)
        {
            stats.spread = stats.currentAskPrice.value() - stats.currentBidPrice.value();
            stats.crossedBook = (stats.currentAskPrice.value() <= stats.currentBidPrice.value());
        }
        else
        {
            stats.spread = std::nullopt;
            stats.crossedBook = std::nullopt;
        }

        return stats;
    }

private:
    Stats stats;
};
