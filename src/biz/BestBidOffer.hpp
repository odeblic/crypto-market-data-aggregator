#pragma once

#include "core/Book.hpp"
#include "core/Utils.hpp"

static inline auto computeBestBidOffer(Book const& book) -> Book
{
    auto bestBidOffer = book;
    bestBidOffer.ask.resize(1);
    bestBidOffer.bid.resize(1);
    return bestBidOffer;
}
