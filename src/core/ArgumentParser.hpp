#pragma once

#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"

#include <boost/algorithm/string.hpp>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class ArgumentParser
{
public:
    struct ExchangeReference
    {
        Exchange id{};
        std::string name;
    };

    struct Arguments
    {
        std::string configPath;
        bool verbose{false};
        std::vector<ExchangeReference> exchanges;
    };

    ArgumentParser(int const argc, char ** argv)
    : argc(argc), argv(argv)
    {
        for (int i = 1; i < argc; ++i)
        {
            std::string const arg{argv[i]};

            if (arg == "--config")
            {
                if (i + 1 >= argc)
                {
                    printUsageAndExit("--config requires a file path argument");
                }

                arguments.configPath = argv[++i];
            }
            else if (arg == "--verbose")
            {
                arguments.verbose = true;
            }
            else if (arg == "--help")
            {
                printUsageAndExit();
            }
            else if (arg.starts_with("--") || arg.starts_with("-"))
            {
                printUsageAndExit("unknown option: " + arg);
            }
            else
            {
                try
                {
                    auto const exchangeId = fromString<Exchange>(arg);
                    auto const exchangeName = boost::algorithm::to_lower_copy(arg);
                    arguments.exchanges.emplace_back(exchangeId, exchangeName);
                }
                catch(std::runtime_error const& e)
                {
                    printUsageAndExit(e.what());
                }
            }
        }

        if (arguments.configPath.empty())
        {
            printUsageAndExit("no configuration file was provided");
        }
    }

    auto getArguments() const -> Arguments
    {
        return arguments;
    }

    void assertExchangeCount(size_t min, size_t max = 0) const
    {
        if (max == 0)
        {
            max = min;
        }

        if (arguments.exchanges.size() < min || arguments.exchanges.size() > max)
        {
            if (min == 0 && max == 0)
            {
                printUsageAndExit("No exchange needs to be specified.");
            }
            else if (min == max)
            {
                printUsageAndExit("Exactly " + std::to_string(min) + " exchange(s) must be specified.");
            }
            else
            {
                printUsageAndExit("Between " + std::to_string(min) + " and " + std::to_string(max) + " exchange(s) must be specified.");
            }
        }
    }

    void printUsageAndExit(std::string error = "") const
    {
        if (!error.empty())
        {
            std::cerr << "Error:\n\t" << error << "\n\n";
        }

        std::cerr << "Usage:\n\t"
                  << argv[0] << " [EXCHANGE_1 EXCHANGE_2 EXCHANGE_3 ...] --config CONFIG_FILE [--verbose] [--help]\n\n";
        std::cerr << "Examples:\n\t"
                  << argv[0] << " --config /path/to/file.json bitmex --verbose\n\t"
                  << argv[0] << " hyperliquid okx kraken --config /path/to/file.json\n\t"
                  << argv[0] << " --help\n\n";
        std::cerr << "List of available exchanges:\n"
                  << "- Binance\n"
                  << "- BitMEX\n"
                  << "- Bybit\n"
                  << "- Coinbase\n"
                  << "- Hyperliquid\n"
                  << "- Internal\n"
                  << "- Kraken\n"
                  << "- OKX\n";

        if (error.empty())
        {
            std::exit(EXIT_SUCCESS);
        }
        else
        {
            std::exit(EXIT_FAILURE);
        }
    }

private:
    int argc{0};
    char ** argv{nullptr};
    Arguments arguments;
};
