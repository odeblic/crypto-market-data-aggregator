#pragma once

#include <vector>

struct Book
{
    struct Quote
    {
        double price{0.0};
        double quantity{0.0};
    };

    std::vector<Quote> ask;
    std::vector<Quote> bid;
};
