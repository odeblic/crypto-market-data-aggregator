#include "core/MarketDataLogger.hpp"
#include "core/Utils.hpp"
#include "ws/WebsocketClient.hpp"

#include <iostream>

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        std::cerr << "An exchange must be specified:\n"
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

    auto const exchange = std::string(argv[1]);
    MarketDataLogger logger;
    WebsocketClient client{logger};
    client.connect(fromString<Exchange>(exchange));
    client.run();
    return 0;
}
