#pragma once

#include "cfg/ServerConfiguration.hpp"

#include <nlohmann/json.hpp>

#include <vector>

struct PriceBandsConfiguration
{
    ServerConfiguration aggregator;
    std::vector<int> bands;
    bool verbose{false};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PriceBandsConfiguration, aggregator, bands, verbose)
