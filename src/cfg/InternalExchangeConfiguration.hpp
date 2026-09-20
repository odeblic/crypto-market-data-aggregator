#pragma once

#include <nlohmann/json.hpp>

#include <string>

struct InternalExchangeConfiguration
{
    struct Service
    {
        std::string host;
        unsigned short port{0};
        std::string path;
        std::string cert;
        std::string pkey;
    };

    Service service;
    bool verbose{false};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InternalExchangeConfiguration::Service, host, port, path, cert, pkey)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InternalExchangeConfiguration, service, verbose)
