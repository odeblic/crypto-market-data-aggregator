#include "cfg/Loader.hpp"
#include "cfg/PriceBandsConfiguration.hpp"
#include "core/Arguments.hpp"
#include "rpc/MarketDataClient.hpp"
#include "rpc/MarketDataHandlerPriceBands.hpp"

#include <nlohmann/json.hpp>

int main(int argc, char ** argv)
{
    auto arguments = Arguments{argc, argv};
    auto path = arguments.getConfigFilePath();
    auto config = loadConfigFromFile<PriceBandsConfiguration>(path);
    auto handler = MarketDataHandlerPriceBands{config.bands};
    auto client = MarketDataClient{config.aggregator.host, config.aggregator.port, handler};
    client.StreamMarketDataSnapshots("BTCUSD");
}
