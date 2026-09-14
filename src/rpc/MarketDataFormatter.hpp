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
        buffer << "ASK  price:" << toString(ask.price(), false) << "  quantity:" << toString(ask.quantity(), true) << '\n';
    }

    for (auto const& bid : snapshot.bids())
    {
        buffer << "BID  price:" << toString(bid.price(), false) << "  quantity:" << toString(bid.quantity(), true) << '\n';
    }

    buffer << "end of snapshot for symbol: " << snapshot.symbol() << '\n';
    return buffer.str();
}

static inline auto toString(marketdata::Update const& update) -> std::string
{
    std::stringstream buffer;
    buffer << "update for symbol: " << update.symbol()
           << '\n'
           << (update.side() == marketdata::Side::BID ? "BID" : "ASK")
           << "  price:" << toString(update.price(), false)
           << "  quantity:" << toString(update.quantity(), true)
           << '\n';
    return buffer.str();
}
