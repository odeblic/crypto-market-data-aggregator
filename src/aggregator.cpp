#include "core/Aggregator.hpp"
#include "core/MarketUpdate.hpp"
#include "core/MarketDataLogger.hpp"
#include "core/MarketDataPublisher.hpp"
#include "core/MarketDataQueue.hpp"
#include "rpc/MarketDataService.hpp"
#include "ws/WebsocketClient.hpp"

#include "marketdata.grpc.pb.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <utility>

int main(int argc, char ** argv)
{
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
        std::string const serverAddress{"0.0.0.0:50051"};
        builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        std::cout << "gRPC server listening on " << serverAddress << std::endl;
        server->Wait();
    });

    client.connect(Exchange::BINANCE);
    client.connect(Exchange::BITMEX);
    client.connect(Exchange::BYBIT);
    client.connect(Exchange::COINBASE);
    client.connect(Exchange::HYPERLIQUID);
    //client.connect(Exchange::INTERNAL);
    client.connect(Exchange::KRAKEN);
    client.connect(Exchange::OKX);
    client.run();
    return 0;
}
