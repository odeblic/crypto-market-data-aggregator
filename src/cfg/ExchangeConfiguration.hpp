#pragma once

#include <nlohmann/json.hpp>

#include <string>

struct ExchangeConfiguration
{
    std::string host;
    int port{443};
    std::string path{"/"};
    nlohmann::json subscription;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ExchangeConfiguration, host, port, path, subscription)
