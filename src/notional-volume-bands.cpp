#include "cfg/Loader.hpp"
#include "cfg/NotionalVolumeBandsConfiguration.hpp"
#include "core/Arguments.hpp"
#include "rpc/MarketDataClient.hpp"
#include "rpc/MarketDataHandlerNotionalVolumeBands.hpp"

#include <nlohmann/json.hpp>

int main(int argc, char ** argv)
{
    auto arguments = Arguments{argc, argv};
    auto path = arguments.getConfigFilePath();
    auto config = loadConfigFromFile<NotionalVolumeBandsConfiguration>(path);
    auto handler = MarketDataHandlerNotionalVolumeBands{};
    auto client = MarketDataClient{config.aggregator.host, config.aggregator.port, handler};
    client.StreamMarketDataSnapshots("BTCUSD");
}
