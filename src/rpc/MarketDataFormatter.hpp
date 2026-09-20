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

static inline auto toString(marketdata::Diff const& diff) -> std::string
{
    std::stringstream buffer;
    buffer << "diff for symbol: " << diff.symbol()
           << '\n'
           << (diff.side() == marketdata::Side::BID ? "BID" : "ASK")
           << "  price:" << toString(diff.price(), false)
           << "  quantity:" << toString(diff.quantity(), true)
           << '\n';
    return buffer.str();
}
