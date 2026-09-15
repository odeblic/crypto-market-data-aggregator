#include "cfg/BestBidOfferConfiguration.hpp"
#include "cfg/Loader.hpp"
#include "core/ArgumentParser.hpp"
#include "core/Display.hpp"
#include "rpc/MarketDataClient.hpp"
#include "rpc/MarketDataHandlerBestBidOffer.hpp"

#include <nlohmann/json.hpp>

int main(int argc, char ** argv)
{
    auto parser = ArgumentParser{argc, argv};
    parser.assertExchangeCount(0);
    auto const arguments = parser.getArguments();
    auto const config = loadConfigFromFile<BestBidOfferConfiguration>(arguments.configPath);
    auto const verbose = config.verbose || arguments.verbose;
    Display::instantiate(verbose, true);
    LOG_INFO("starting the client to show the best bid offer");
    auto handler = MarketDataHandlerBestBidOffer{verbose};
    auto client = MarketDataClient{config.aggregator.host, config.aggregator.port, handler};
    client.StreamMarketDataSnapshots("BTCUSD");
}
