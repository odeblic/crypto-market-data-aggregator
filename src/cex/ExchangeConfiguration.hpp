#pragma once

#include <string>

struct ExchangeConfiguration
{
    std::string host;
    int port{443};
    std::string path{"/"};
    std::string subscription;
    bool debug{false};
};
