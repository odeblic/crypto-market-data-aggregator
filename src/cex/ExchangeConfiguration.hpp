#pragma once

#include <string>

struct ExchangeConfiguration
{
    std::string host;
    int port;
    std::string path;
    std::string subscription;
};
