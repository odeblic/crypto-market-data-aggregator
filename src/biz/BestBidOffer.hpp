#pragma once

#include "core/Book.hpp"
#include "core/Utils.hpp"

static inline auto computeBestBidOffer(Book const& book) -> Book
{
    auto bestBidOffer = book;

    if (book.ask.size() > 0)
    {
        bestBidOffer.ask.resize(1);
    }

    if (book.bid.size() > 0)
    {
        bestBidOffer.bid.resize(1);
    }

    return bestBidOffer;
}
