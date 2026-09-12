#pragma once

#include "core/Book.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"
#include "rpc/MarketDataHandler.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <memory>

class MarketDataHandlerPriceBands : public MarketDataHandler
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
        auto priceBands = std::make_unique<Book>();

        auto calculateAndAdd = [&](Side side, int bps)
        {
            auto& quotes = (side == Side::ASK ? book.ask : book.bid);
            auto& bands = (side == Side::ASK ? priceBands->ask : priceBands->bid);

            if (auto band = calculateBand(quotes, side, bps); band)
            {
                bands.push_back(*band);
            }
        };

        for (auto bps : {50, 100, 200, 500, 1'000})
        {
            calculateAndAdd(Side::ASK, bps);
            calculateAndAdd(Side::BID, bps);
        }

        return priceBands;
    }

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
            if (side == Side::ASK && quote.price < shiftedPrice ||
                side == Side::BID && quote.price > shiftedPrice)
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
};
