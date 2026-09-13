#pragma once

#include <stdexcept>
#include <string>

class Arguments
{
public:
    Arguments(int argc, char ** argv)
    {
        if (argc != 2)
        {
            throw std::runtime_error("no configuration file was provided");
        }

        configFilePath = std::string{argv[1]};
    }

    auto getConfigFilePath() const -> std::string
    {
        return configFilePath;
    }

private:
    std::string configFilePath;
};
