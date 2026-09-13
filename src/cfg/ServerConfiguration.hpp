#pragma once

#include <nlohmann/json.hpp>

#include <string>

struct ServerConfiguration
{
    std::string host;
    int port{0};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ServerConfiguration, host, port)
