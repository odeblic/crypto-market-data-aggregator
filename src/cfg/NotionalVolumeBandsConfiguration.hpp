#pragma once

#include "cfg/ServerConfiguration.hpp"

#include <nlohmann/json.hpp>

struct NotionalVolumeBandsConfiguration
{
    ServerConfiguration aggregator;
    bool verbose{false};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NotionalVolumeBandsConfiguration, aggregator, verbose)
