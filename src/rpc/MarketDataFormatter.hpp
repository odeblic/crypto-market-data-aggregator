#pragma once

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <iomanip>
#include <sstream>
#include <string>

static inline auto toString(marketdata::Snapshot const& snapshot) -> std::string
{
    std::stringstream buffer;
    buffer << "snapshot for symbol: " << snapshot.symbol() << '\n';

    for (auto const& ask : snapshot.asks())
    {
        buffer << "ASK " << ask.price() << " " << ask.quantity() << '\n';
    }

    for (auto const& bid : snapshot.bids())
    {
        buffer << "BID " << bid.price() << " " << bid.quantity() << '\n';
    }

    return buffer.str();
}

static inline auto toString(marketdata::Update const& update) -> std::string
{
    std::stringstream buffer;
    buffer << "update for symbol: " << update.symbol()
            << " | " << (update.side() == marketdata::Side::BID ? "BID" : "ASK")
            << " | price: " << update.price()
            << " | quantity: " << update.quantity() 
            << '\n';
    return buffer.str();
}
