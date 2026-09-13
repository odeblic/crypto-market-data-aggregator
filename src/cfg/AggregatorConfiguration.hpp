#pragma once

#include "cfg/ExchangeConfiguration.hpp"
#include "cfg/ServerConfiguration.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <unordered_map>

struct AggregatorConfiguration
{
    std::unordered_map<std::string, ExchangeConfiguration> exchanges;
    ServerConfiguration service;
    bool verbose{false};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AggregatorConfiguration, exchanges, service, verbose)
