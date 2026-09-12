#pragma once

#include "core/Book.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <iostream>
#include <iomanip>
#include <memory>

class MarketDataHandler
{
public:

    virtual void onSnapshot(marketdata::Snapshot const& snapshot) const = 0;

    virtual void onUpdate(marketdata::Update const& update) const = 0;

protected:
    static void printSnapshot(marketdata::Snapshot const& snapshot)
    {
        std::cout << "snapshot for symbol: " << snapshot.symbol() << "\n";

        for (const auto& ask : snapshot.asks())
        {
            std::cout << "ASK " << ask.price() << " " << ask.quantity() << "\n";
        }

        for (const auto& bid : snapshot.bids())
        {
            std::cout << "BID " << bid.price() << " " << bid.quantity() << "\n";
        }
    }

    static void printUpdate(marketdata::Update const& update)
    {
        std::cout << "update for symbol: " << update.symbol()
                  << " | " << (update.side() == marketdata::Side::BID ? "BID" : "ASK")
                  << " | price: " << update.price()
                  << " | quantity: " << update.quantity() 
                  << std::endl;
    }

    static void printBook(Book const& book)
    {
        size_t level = 0;
        size_t const depth = 10;

        auto ask = book.ask.begin();
        auto bid = book.bid.begin();

        std::cout << "\n+--------------------+--------------------+\n";

        while ((level < book.ask.size() || level < book.bid.size()) && level < depth)
        {
            std::cout << "| ";

            if (level < book.bid.size())
            {
                auto const& quote = book.bid[level];
                std::cout << "\033[32m"
                        << std::setfill(' ') << std::setw(7) << std::fixed << std::setprecision(2)
                        << quote.quantity
                        << "\033[0m * \033[32m"
                        << std::setfill(' ') << std::setw(7) << std::fixed << std::setprecision(2)
                        << quote.price
                        << "\033[0m";
            }
            else
            {
                std::cout << "                  ";
            }

            std::cout << " | ";

            if (level < book.ask.size())
            {
                auto const& quote = book.ask[level];
                std::cout << "\033[31m"
                        << std::setfill(' ') << std::setw(7) << std::fixed << std::setprecision(2)
                        << quote.price
                        << "\033[0m * \033[31m"
                        << std::setfill(' ') << std::setw(7) << std::fixed << std::setprecision(2)
                        << quote.quantity
                        << "\033[0m";
            }
            else
            {
                std::cout << "                  ";
            }

            std::cout << " |\n";

            level++;
        }

        std::cout << "+--------------------+--------------------+\n";
    }

    static auto makeBook(marketdata::Snapshot const& snapshot) -> std::unique_ptr<Book>
    {
        auto book = std::make_unique<Book>();

        for (const auto& ask : snapshot.asks())
        {
            book->ask.push_back(Book::Quote{ask.price(), ask.quantity()});
        }

        for (const auto& bid : snapshot.bids())
        {
            book->bid.push_back(Book::Quote{bid.price(), bid.quantity()});
        }

        return book;
    }

    static auto makeMarketUpdate(marketdata::Update const& update) -> std::unique_ptr<MarketUpdate>
    {
        auto marketUpdate = std::make_unique<MarketUpdate>();
        marketUpdate->price = update.price();
        marketUpdate->quantity = update.quantity();
        marketUpdate->ticker = fromString<Ticker>(update.symbol());
        marketUpdate->side = marketdata::Side::BID ? Side::BID : Side::ASK;
        marketUpdate->exchange = Exchange::UNKNOWN;
        return marketUpdate;
    }
};
