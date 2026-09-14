#pragma once

#include "core/Book.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"

#include <iomanip>
#include <sstream>
#include <string>

static inline auto toString(MarketUpdate const& update) -> std::string
{
    std::stringstream buffer;
    buffer << "ticker: \033[35m" << toString(update.ticker) << "\033[0m "
            << "side: " << (update.side == Side::ASK ? "\033[31mASK\033[0m " : "\033[32mBID\033[0m ")
            << "price: \033[33m" << std::fixed << std::setprecision(2) << update.price << "\033[0m "
            << "quantity: \033[33m" << std::defaultfloat << update.quantity << "\033[0m "
            << "source: \033[34m" << toString(update.exchange) << "\033[0m\n";
    return buffer.str();
}

static inline auto toString(Book const& book, size_t const depth = 10) -> std::string
{
    std::stringstream buffer;
    size_t level = 0;
    buffer << "\n+--------------------+--------------------+\n";

    while ((level < book.ask.size() || level < book.bid.size()) && level < depth)
    {
        buffer << "| ";

        if (level < book.bid.size())
        {
            auto const& quote = book.bid[level];
            buffer << "\033[32m"
                    << std::setfill(' ') << std::setw(7) << std::fixed << std::setprecision(2)
                    << quote.quantity
                    << "\033[0m * \033[32m"
                    << std::setfill(' ') << std::setw(7) << std::fixed << std::setprecision(2)
                    << quote.price
                    << "\033[0m";
        }
        else
        {
            buffer << "                  ";
        }

        buffer << " | ";

        if (level < book.ask.size())
        {
            auto const& quote = book.ask[level];
            buffer << "\033[31m"
                    << std::setfill(' ') << std::setw(7) << std::fixed << std::setprecision(2)
                    << quote.price
                    << "\033[0m * \033[31m"
                    << std::setfill(' ') << std::setw(7) << std::fixed << std::setprecision(2)
                    << quote.quantity
                    << "\033[0m";
        }
        else
        {
            buffer << "                  ";
        }

        buffer << " |\n";
        level++;
    }

    buffer << "+--------------------+--------------------+\n";
    return buffer.str();
}
