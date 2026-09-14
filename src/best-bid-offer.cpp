#include "cfg/BestBidOfferConfiguration.hpp"
#include "cfg/Loader.hpp"
#include "core/Arguments.hpp"
#include "core/Display.hpp"
#include "rpc/MarketDataClient.hpp"
#include "rpc/MarketDataHandlerBestBidOffer.hpp"

#include <nlohmann/json.hpp>

int main(int argc, char ** argv)
{
    auto arguments = Arguments{argc, argv};
    auto path = arguments.getConfigFilePath();
    auto config = loadConfigFromFile<BestBidOfferConfiguration>(path);
    Display::instantiate(config.verbose, true);
    LOG_INFO("starting the client to show the best bid offer");
    auto handler = MarketDataHandlerBestBidOffer{config.verbose};
    auto client = MarketDataClient{config.aggregator.host, config.aggregator.port, handler};
    client.StreamMarketDataSnapshots("BTCUSD");
}
