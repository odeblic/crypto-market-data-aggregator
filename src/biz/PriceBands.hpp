#pragma once

#include "core/Book.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"

#include <optional>
#include <span>
#include <vector>

class PriceBands
{
public:
    PriceBands(std::span<int> bandValues)
    : bandValues(bandValues.begin(), bandValues.end())
    {
    }

    auto compute(Book const& book) const -> Book
    {
        auto priceBands = Book{};

        auto calculateAndAdd = [&](Side side, int bps)
        {
            auto& quotes = (side == Side::ASK ? book.ask : book.bid);
            auto& bands = (side == Side::ASK ? priceBands.ask : priceBands.bid);

            if (auto band = calculateBand(quotes, side, bps); band)
            {
                bands.push_back(*band);
            }
        };

        for (auto bps : bandValues)
        {
            calculateAndAdd(Side::ASK, bps);
            calculateAndAdd(Side::BID, bps);
        }

        return priceBands;
    }

private:
    static auto calculateBand(std::vector<Book::Quote> const& quotes, Side side, int bps) -> std::optional<Book::Quote>
    {
        if (quotes.empty())
        {
            return std::nullopt;
        }

        double const bestPrice = quotes[0].price;
        double const shiftedPrice = bestPrice * (1 + (side == Side::ASK ? bps / 10'000. : -bps / 10'000.));
        double cumulatedQuantity = 0.0;

        for (auto const& quote : quotes)
        {
            if ((side == Side::ASK && quote.price < shiftedPrice) ||
                (side == Side::BID && quote.price > shiftedPrice))
            {
                cumulatedQuantity += quote.quantity;
            }
            else
            {
                break;
            }
        }

        return Book::Quote{shiftedPrice, cumulatedQuantity};
    }

    std::vector<int> bandValues;
};
