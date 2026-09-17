#pragma once

#include "cfg/ServerConfiguration.hpp"

#include <nlohmann/json.hpp>

struct StatisticsConfiguration
{
    ServerConfiguration aggregator;
    bool verbose{false};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StatisticsConfiguration, aggregator, verbose)
