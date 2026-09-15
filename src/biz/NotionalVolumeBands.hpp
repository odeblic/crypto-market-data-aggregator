#pragma once

#include "core/Book.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"

#include <optional>
#include <span>
#include <vector>

class NotionalVolumeBands
{
public:
    NotionalVolumeBands(std::span<double> bandValues)
    : bandValues(bandValues.begin(), bandValues.end())
    {
    }

    auto compute(Book const& book) const -> Book
    {
        auto notionalVolumeBands = Book{};

        auto calculateAndAdd = [&](Side side, double desiredNotional)
        {
            auto& quotes = (side == Side::ASK ? book.ask : book.bid);
            auto& bands = (side == Side::ASK ? notionalVolumeBands.ask : notionalVolumeBands.bid);

            if (auto band = calculateBand(quotes, desiredNotional))
            {
                bands.push_back(*band);
            }
        };

        for (auto notional : bandValues)
        {
            calculateAndAdd(Side::ASK, notional);
            calculateAndAdd(Side::BID, notional);
        }

        return notionalVolumeBands;
    }

private:
    static auto calculateBand(std::vector<Book::Quote> const& quotes, double desiredNotional) -> std::optional<Book::Quote>
    {
        double cumulatedNotional = 0.0;
        double cumulatedQuantity = 0.0;

        for (auto const& quote : quotes)
        {
            auto const quoteNotional = quote.price * quote.quantity;

            if (cumulatedNotional + quoteNotional <= desiredNotional)
            {
                cumulatedNotional += quoteNotional;
                cumulatedQuantity += quote.quantity;
            }
            else
            {
                auto const missingNotional = desiredNotional - cumulatedNotional;
                auto const missingQuantity = missingNotional / quote.price;
                cumulatedNotional += missingNotional;
                cumulatedQuantity += missingQuantity;
                break;
            }
        }

        if (cumulatedNotional >= desiredNotional)
        {
            auto const averagedPrice = cumulatedNotional / cumulatedQuantity;
            return Book::Quote{averagedPrice, cumulatedQuantity};
        }
        else
        {
            return std::nullopt;
        }
    }

    std::vector<double> bandValues;
};
