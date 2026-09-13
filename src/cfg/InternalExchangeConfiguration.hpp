#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <unordered_map>

struct InternalExchangeConfiguration
{
    struct Feed
    {
        std::string symbol;
        double price{0.0};
        double quantity{0.0};
        double spread{0.0};
        int depth{0};
    };

    struct Service
    {
        std::string host;
        unsigned short port{0};
        std::string path;
        std::string cert;
        std::string pkey;
    };

    Feed feed;
    Service service;
    bool verbose{false};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InternalExchangeConfiguration::Feed, symbol, price, quantity, spread, depth)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InternalExchangeConfiguration::Service, host, port, path, cert, pkey)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InternalExchangeConfiguration, feed, service, verbose)
