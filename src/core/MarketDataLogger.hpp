#pragma once

#include "core/MarketDataSink.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"

#include <iomanip>
#include <iostream>

class MarketDataLogger : public MarketDataSink
{
public:
    MarketDataLogger()
    {
    }

    virtual auto write(MarketUpdate const& update) -> bool override
    {
        std::cout << "ticker: \033[35m" << toString(update.ticker) << "\033[0m "
                  << "side: " << (update.side == Side::ASK ? "\033[31mASK\033[0m " : "\033[32mBID\033[0m ")
                  << "price: \033[33m" << std::fixed << std::setprecision(2) << update.price << "\033[0m "
                  << "quantity: \033[33m" << std::defaultfloat << update.quantity << "\033[0m "
                  << "source: \033[34m" << toString(update.exchange) << "\033[0m"
                  << std::endl;

        return true;
    }

private:
};
