#pragma once

#include "core/Book.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"
#include "rpc/MarketDataHandler.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <optional>
#include <memory>

class MarketDataHandlerNotionalVolumeBands : public MarketDataHandler
{
public:
    virtual void onSnapshot(marketdata::Snapshot const& snapshot) const override
    {
        //printSnapshot(snapshot);
        auto book = makeBook(snapshot);
        auto syntheticBook = compute(*book);
        printBook(*syntheticBook);
    }

    virtual void onUpdate(marketdata::Update const& update) const override
    {
        //printUpdate(update);
    }

private:
    static auto compute(Book const& book) -> std::unique_ptr<Book>
    {
        auto notionalVolumeBands = std::make_unique<Book>();

        auto calculateAndAdd = [&](Side side, double desiredNotional)
        {
            auto& quotes = (side == Side::ASK ? book.ask : book.bid);
            auto& bands = (side == Side::ASK ? notionalVolumeBands->ask : notionalVolumeBands->bid);

            if (auto band = calculateBand(quotes, desiredNotional); band)
            {
                bands.push_back(*band);
            }
        };

        for (auto notional : {25'000, 100'000, 250'000, 1'000'000, 5'000'000, 10'000'000, 25'000'000, 50'000'000})
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
};
