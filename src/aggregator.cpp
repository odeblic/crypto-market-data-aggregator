#include "biz/Aggregator.hpp"
#include "cfg/AggregatorConfiguration.hpp"
#include "cfg/Loader.hpp"
#include "core/ArgumentParser.hpp"
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
    auto parser = ArgumentParser{argc, argv};
    parser.assertExchangeCount(0, 7);
    auto arguments = parser.getArguments();
    auto const config = loadConfigFromFile<AggregatorConfiguration>(arguments.configPath);
    auto const verbose = config.verbose || arguments.verbose;
    Display::instantiate(verbose, true);
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
        auto const serverAddress = std::string{config.service.host + ":" + std::to_string(config.service.port)};
        auto builder = grpc::ServerBuilder{};
        builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);

        if (auto server = builder.BuildAndStart())
        {
            LOG_DEBUG("gRPC server listening on " + serverAddress);
            server->Wait();
        }
        else
        {
            LOG_ERROR("gRPC server could not start");
        }
    });

    for (auto const& [exchangeId, exchangeName] : arguments.exchanges)
    {
        client.connect(exchangeId, config.exchanges.at(exchangeName));
    }

    client.run();
}
