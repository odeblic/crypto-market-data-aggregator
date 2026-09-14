#include "Book.hpp"
#include "MarketUpdate.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <set>

class Aggregator
{
public:
    using Price = double;

    class Quantities
    {
    public:
        void set(Exchange exchange, double quantity)
        {
            auto const index = static_cast<size_t>(exchange);
            quantities[index] = quantity;
        }

        void reset(Exchange exchange)
        {
            set(exchange, 0.0);
        }

        auto empty() const -> bool
        {
            return static_cast<size_t>(std::count(quantities.begin(), quantities.end(), 0.0)) == quantities.size();
        }

        auto total() const -> double
        {
            return std::accumulate(quantities.begin(), quantities.end(), 0.0);
        }

    private:
        std::array<double, static_cast<size_t>(Exchange::MAX)> quantities{};
    };

    struct PriceLevels
    {
        std::map<Price, Quantities, std::less<Price>> ask;
        std::map<Price, Quantities, std::greater<Price>> bid;
    };

    void onUpdate(MarketUpdate const& update)
    {
        std::scoped_lock<std::mutex> lock(mutex);

        if (update.side == Side::ASK)
        {
            auto& quantities = priceLevels.ask[update.price];
            quantities.set(update.exchange, update.quantity);

            if (quantities.total() == 0.0)
            {
                priceLevels.ask.erase(update.price);
            }
        }
        else
        {
            auto& quantities = priceLevels.bid[update.price];
            quantities.set(update.exchange, update.quantity);

            if (quantities.total() == 0.0)
            {
                priceLevels.bid.erase(update.price);
            }
        }
    }

    auto generateBook() const -> Book
    {
        auto book = Book{};
        std::scoped_lock<std::mutex> lock(mutex);

        for (auto const& [price, quantities] : priceLevels.ask)
        {
            book.ask.push_back(Book::Quote{price, quantities.total()});
        }

        for (auto const& [price, quantities] : priceLevels.bid)
        {
            book.bid.push_back(Book::Quote{price, quantities.total()});
        }

        return book;
    }

private:
    PriceLevels priceLevels;
    std::mutex mutable mutex;
};
