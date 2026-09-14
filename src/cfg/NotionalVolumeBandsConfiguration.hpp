#pragma once

#include "cfg/ServerConfiguration.hpp"

#include <nlohmann/json.hpp>

#include <vector>

struct NotionalVolumeBandsConfiguration
{
    ServerConfiguration aggregator;
    std::vector<double> bands;
    bool verbose{false};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NotionalVolumeBandsConfiguration, aggregator, bands, verbose)
