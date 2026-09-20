#pragma once

#include "core/Book.hpp"
#include "core/Captions.hpp"
#include "core/Stats.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Side.hpp"
#include "core/Utils.hpp"

#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

static inline auto humanizeNotional(double value) -> std::string
{
    std::stringstream buffer;

    if (value >= 1'000'000.0)
    {
        buffer << std::fixed << std::setprecision(1) << value / 1'000'000.0 << "M";
    }
    else if (value >= 1'000.0)
    {
        buffer << std::fixed << std::setprecision(1) << value / 1'000.0 << "K";
    }
    else
    {
        buffer << std::fixed << std::setprecision(1) << value;
    }

    return buffer.str();
}

static inline auto humanizeBps(int bps, bool minus) -> std::string
{
    if (minus)
    {
        return "-" + std::to_string(bps) + " bps";
    }
    else
    {
        return "+" + std::to_string(bps) + " bps";
    }
}

static inline auto capString(std::string_view str, size_t max) -> std::string
{
    if (max < 3)
    {
        return std::string(str.substr(0, max));
    }
    else if (str.size() <= max)
    {
        return std::string(str);
    }
    else
    {
        return std::string(str.substr(0, max - 3)) + "...";
    }
}

static inline auto centerString(std::string_view str, size_t size) -> std::string
{
    if (str.length() >= size)
    {
        return std::string(str.substr(0, size));
    }

    size_t const totalPadding = size - str.length();
    size_t const leftPadding = totalPadding / 2;
    size_t const rightPadding = totalPadding - leftPadding;
    std::string centered;
    centered.reserve(size);
    centered.append(leftPadding, ' ');
    centered.append(str);
    centered.append(rightPadding, ' ');
    return centered;
}

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

static inline auto toString(Book const& book, Captions const& captions = {}, size_t const depth = 0) -> std::string
{
    std::stringstream buffer;
    size_t level = 0;
    bool const hasLabels = captions.askLabels.size() > 0 || captions.bidLabels.size() > 0;
    auto const margin = std::string((hasLabels ? captions.maxLabelSize + 2 : 0), ' ');

    if (!captions.symbol.empty())
    {
        buffer << "\n\n"
               << margin
               << "                       <\033[35m"
               << std::setw(8)
               << captions.symbol
               << "\033[0m>\n"
               << margin
               << "                           |\n"
               << margin
               << "+--------------------------+--------------------------+\n";
    }
    else
    {
        buffer << "\n\n"
               << margin
               << "+--------------------------+--------------------------+\n";
    }

    if (!captions.title.empty())
    {
        buffer << margin
               << "| \033[34m"
               << centerString(captions.title, 51)
               << "\033[0m |\n"
               << margin
               << "+--------------------------+--------------------------+\n";
    }

    while ((level < book.ask.size() || level < book.bid.size()) && (depth == 0 || level < depth))
    {
        if (hasLabels)
        {
            if (level < captions.bidLabels.size())
            {
                buffer << "\033[34m"
                    << std::setw(captions.maxLabelSize)
                    << captions.bidLabels[level]
                    << "  \033[0m";
            }
            else
            {
                buffer << margin;
            }
        }

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

        buffer << " |";

        if (hasLabels)
        {
            if (level < captions.askLabels.size())
            {
                buffer << "  \033[34m"
                       << std::setw(captions.maxLabelSize)
                       << captions.askLabels[level]
                       << "\033[0m";
            }
        }

        buffer << "\n";
        level++;
    }

    buffer << margin
           << "+--------------------------+--------------------------+\n";
    return buffer.str();
}

static inline auto toString(Stats const& stats) -> std::string
{
    std::stringstream buffer;
    auto const NA = "\033[30;1m      n/a\033[0m\n";
    buffer << "current ask price: ";

    if (stats.currentAskPrice)
    {
        buffer << "\033[31m" << toString(stats.currentAskPrice.value(), false) << "\033[0m\n";
    }
    else
    {
        buffer << NA;
    }

    buffer << "current bid price: ";

    if (stats.currentBidPrice)
    {
        buffer << "\033[32m" << toString(stats.currentBidPrice.value(), false) << "\033[0m\n";
    }
    else
    {
        buffer << NA;
    }

    buffer << "spread:            ";

    if (stats.spread)
    {
        buffer << "\033[33m" << toString(stats.spread.value(), false) << "\033[0m\n";
    }
    else
    {
        buffer << NA;
    }

    buffer << "crossed book:      ";

    if (stats.crossedBook)
    {
        if (stats.crossedBook.value())
        {
            buffer << "\033[33m      yes\033[0m\n";
        }
        else
        {
            buffer << "\033[33m       no\033[0m\n";
        }
    }
    else
    {
        buffer << NA;
    }

    buffer << "lowest ask price:  ";

    if (stats.lowestAskPrice)
    {
        buffer << "\033[31m" << toString(stats.lowestAskPrice.value(), false) << "\033[0m\n";
    }
    else
    {
        buffer << NA;
    }

    buffer << "highest bid price: ";

    if (stats.highestBidPrice)
    {
        buffer << "\033[32m" << toString(stats.highestBidPrice.value(), false) << "\033[0m\n";
    }
    else
    {
        buffer << NA;
    }

    buffer << "ask level count:   \033[35m" << std::setw(9) << stats.askLevelCount << "\033[0m\n";
    buffer << "bid level count:   \033[35m" << std::setw(9) << stats.bidLevelCount << "\033[0m\n\n";
    return buffer.str();
}
