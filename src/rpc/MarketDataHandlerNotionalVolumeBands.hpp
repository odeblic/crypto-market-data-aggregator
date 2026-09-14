#pragma once

#include "core/Book.hpp"
#include "core/Display.hpp"
#include "core/Formatter.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"
#include "rpc/MarketDataFormatter.hpp"
#include "rpc/MarketDataHandler.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <memory>
#include <optional>
#include <span>
#include <vector>

class MarketDataHandlerNotionalVolumeBands : public MarketDataHandler
{
public:
    MarketDataHandlerNotionalVolumeBands(std::span<double> bandValues, bool verbose)
    : bandValues(bandValues.begin(), bandValues.end()), verbose(verbose)
    {
    }

    virtual void onSnapshot(marketdata::Snapshot const& snapshot) const override
    {
        if (verbose)
        {
            Display::getInstance().show(toString(snapshot));
        }

        auto originalBook = makeBook(snapshot);
        auto syntheticBook = compute(originalBook);
        Display::getInstance().show(toString(syntheticBook));
    }

    virtual void onUpdate(marketdata::Update const& update) const override
    {
        if (verbose)
        {
            Display::getInstance().show(toString(update));
        }
    }

private:
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
    bool verbose{false};
};
