#include "cfg/AggregatorConfiguration.hpp"
#include "cfg/Loader.hpp"
#include "core/Arguments.hpp"
#include "core/Aggregator.hpp"
#include "core/Display.hpp"
#include "core/MarketUpdate.hpp"
#include "core/MarketDataLogger.hpp"
#include "core/MarketDataPublisher.hpp"
#include "core/MarketDataQueue.hpp"
#include "rpc/MarketDataService.hpp"
#include "ws/WebsocketClient.hpp"

#include "marketdata.grpc.pb.h"

#include <chrono>
#include <thread>
#include <utility>

int main(int argc, char ** argv)
{
    auto arguments = Arguments{argc, argv};
    auto path = arguments.getConfigFilePath();
    auto const config = loadConfigFromFile<AggregatorConfiguration>(path);
    Display::instantiate(config.verbose, true);
    LOG_INFO("starting the aggregator to consolidate market data from exchanges into a single book");
    MarketDataQueue queue;
    MarketDataPublisher publisher{queue};
    MarketDataLogger logger;
    Aggregator aggregator;
    WebsocketClient client{publisher};
    MarketDataService service;

    std::jthread thread_print([&]()
    {
        while (true)
        {
            MarketUpdate update;

            while (queue.pop(update))
            {
                logger.write(update);
                aggregator.onUpdate(update);
            }
        }
    });

    std::jthread thread_agg([&]()
    {
        while (true)
        {
            auto book = aggregator.generateBook();
            service.updateBook(std::move(book));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::jthread thread_gRPC([&]()
    {
        grpc::ServerBuilder builder;
        std::string const serverAddress{config.service.host + ":" + std::to_string(config.service.port)};
        builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        LOG_DEBUG("gRPC server listening on " + serverAddress);
        server->Wait();
    });

    client.connect(Exchange::BINANCE, config.exchanges.at("binance"));
    client.connect(Exchange::BITMEX, config.exchanges.at("bitmex"));
    client.connect(Exchange::BYBIT, config.exchanges.at("bybit"));
    client.connect(Exchange::COINBASE, config.exchanges.at("coinbase"));
    client.connect(Exchange::HYPERLIQUID, config.exchanges.at("hyperliquid"));
    //client.connect(Exchange::INTERNAL, config.exchanges.at("internal"));
    client.connect(Exchange::KRAKEN, config.exchanges.at("kraken"));
    client.connect(Exchange::OKX, config.exchanges.at("okx"));
    client.run();
}
