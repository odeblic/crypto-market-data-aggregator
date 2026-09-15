#include "cfg/Loader.hpp"
#include "cfg/ViewerConfiguration.hpp"
#include "core/ArgumentParser.hpp"
#include "core/Display.hpp"
#include "core/MarketDataLogger.hpp"
#include "core/Utils.hpp"
#include "ws/WebsocketClient.hpp"

int main(int argc, char * argv[])
{
    auto parser = ArgumentParser{argc, argv};
    parser.assertExchangeCount(1);
    auto arguments = parser.getArguments();
    auto const exchangeReference = arguments.exchanges[0];
    auto const config = loadConfigFromFile<ViewerConfiguration>(arguments.configPath);
    auto const verbose = config.verbose || arguments.verbose;
    Display::instantiate(verbose, true);
    LOG_INFO("starting the viewer to see the market data from a single exchange");
    MarketDataLogger logger;
    WebsocketClient client{logger};
    client.connect(exchangeReference.id, config.exchanges.at(exchangeReference.name));
    client.run();
}
