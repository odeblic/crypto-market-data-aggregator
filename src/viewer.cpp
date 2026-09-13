#include "cfg/Loader.hpp"
#include "cfg/ViewerConfiguration.hpp"
//#include "core/Arguments.hpp"
#include "core/MarketDataLogger.hpp"
#include "core/Utils.hpp"
#include "ws/WebsocketClient.hpp"

#include <iostream>

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        std::cerr << "Please specify an exchange and a configuration file.\n\n";
        std::cerr << "Usage:\n\t"
                  << argv[0] << " CONFIG_FILE EXCHANGE_NAME\n\n";
        std::cerr << "Example:\n\t"
                  << argv[0] << " config/viewer.json bitmex\n\n";
        std::cerr << "List of available exchanges:\n"
                  << "- Binance\n"
                  << "- BitMEX\n"
                  << "- Bybit\n"
                  << "- Coinbase\n"
                  << "- Hyperliquid\n"
                  << "- Internal\n"
                  << "- Kraken\n"
                  << "- OKX\n";
        return 1;
    }

    //auto arguments = Arguments{argc, argv};
    //auto path = arguments.getConfigFilePath();
    auto const path = argv[1];
    auto const exchangeName = argv[2];
    auto const exchange = fromString<Exchange>(exchangeName);
    auto const config = loadConfigFromFile<ViewerConfiguration>(path);
    MarketDataLogger logger;
    WebsocketClient client{logger};
    client.connect(exchange, config.exchanges.at(exchangeName));
    client.run();
}
