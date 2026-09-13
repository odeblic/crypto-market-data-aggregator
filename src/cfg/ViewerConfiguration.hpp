#pragma once

#include "cfg/ExchangeConfiguration.hpp"

#include <string>
#include <unordered_map>

struct ViewerConfiguration
{
    std::unordered_map<std::string, ExchangeConfiguration> exchanges;
    bool verbose{false};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ViewerConfiguration, exchanges, verbose)
