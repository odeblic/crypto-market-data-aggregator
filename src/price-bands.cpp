#include "cfg/Loader.hpp"
#include "cfg/PriceBandsConfiguration.hpp"
#include "core/Arguments.hpp"
#include "core/Display.hpp"
#include "rpc/MarketDataClient.hpp"
#include "rpc/MarketDataHandlerPriceBands.hpp"

#include <nlohmann/json.hpp>

int main(int argc, char ** argv)
{
    auto arguments = Arguments{argc, argv};
    auto path = arguments.getConfigFilePath();
    auto config = loadConfigFromFile<PriceBandsConfiguration>(path);
    Display::instantiate(config.verbose, true);
    LOG_INFO("starting the client to show the price bands");
    auto handler = MarketDataHandlerPriceBands{config.bands};
    auto client = MarketDataClient{config.aggregator.host, config.aggregator.port, handler};
    client.StreamMarketDataSnapshots("BTCUSD");
}
