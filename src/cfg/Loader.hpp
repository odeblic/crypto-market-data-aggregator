#pragma once

#include "cfg/AggregatorConfiguration.hpp"
#include "cfg/BestBidOfferConfiguration.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <istream>
#include <stdexcept>
#include <string>
#include <utility>

template <typename T>
auto loadConfigFromStream(std::istream& stream) -> T
{
    if (!stream.good())
    {
        throw std::runtime_error("configuration stream is in a bad state");
    }

    nlohmann::json parsed;
    stream >> parsed;

    return parsed.get<T>();
}

template <typename T>
auto loadConfigFromFile(std::string path) -> T
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error("cannot open configuration file: " + path);
    }

    return loadConfigFromStream<T>(file);
}
