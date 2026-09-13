#include "cfg/BestBidOfferConfiguration.hpp"
#include "cfg/Loader.hpp"
#include "core/Arguments.hpp"
#include "rpc/MarketDataClient.hpp"
#include "rpc/MarketDataHandlerBestBidOffer.hpp"

#include <nlohmann/json.hpp>

int main(int argc, char ** argv)
{
    auto arguments = Arguments{argc, argv};
    auto path = arguments.getConfigFilePath();
    auto config = loadConfigFromFile<BestBidOfferConfiguration>(path);
    auto handler = MarketDataHandlerBestBidOffer{};
    auto client = MarketDataClient{config.aggregator.host, config.aggregator.port, handler};
    client.StreamMarketDataSnapshots("BTCUSD");
}
