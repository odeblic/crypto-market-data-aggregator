#pragma once

#include "core/Book.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"

#include <iomanip>
#include <sstream>
#include <string>

static inline auto toString(double value, bool precise) -> std::string
{
    size_t const precision = (precise ? 6 : 2);
    size_t const length = 7 + precision;
    std::ostringstream buffer;
    buffer << std::setw(length) << std::fixed << std::setprecision(precision) << value;
    return buffer.str();
}

static inline auto toString(MarketUpdate const& update) -> std::string
{
    std::stringstream buffer;
    buffer << "ticker: \033[35m" << toString(update.ticker) << "\033[0m "
           << "side: " << (update.side == Side::ASK ? "\033[31mASK\033[0m " : "\033[32mBID\033[0m ")
           << "price: \033[33m" << toString(update.price, false) << "\033[0m "
           << "quantity: \033[33m" << toString(update.quantity, true) << "\033[0m "
           << "source: \033[34m" << toString(update.exchange) << "\033[0m\n";
    return buffer.str();
}

static inline auto toString(Book const& book, size_t const depth = 10) -> std::string
{
    std::stringstream buffer;
    size_t level = 0;
    buffer << "\n+--------------------------+--------------------------+\n";

    while ((level < book.ask.size() || level < book.bid.size()) && level < depth)
    {
        buffer << "| ";

        if (level < book.bid.size())
        {
            auto const& quote = book.bid[level];
            buffer << "\033[32m"
                    << toString(quote.quantity, true)
                    << "\033[0m  \033[32m"
                    << toString(quote.price, false)
                    << "\033[0m";
        }
        else
        {
            buffer << "                        ";
        }

        buffer << " | ";

        if (level < book.ask.size())
        {
            auto const& quote = book.ask[level];
            buffer << "\033[31m"
                    << toString(quote.price, false)
                    << "\033[0m  \033[31m"
                    << toString(quote.quantity, true)
                    << "\033[0m";
        }
        else
        {
            buffer << "                        ";
        }

        buffer << " |\n";
        level++;
    }

    buffer << "+--------------------------+--------------------------+\n";
    return buffer.str();
}
