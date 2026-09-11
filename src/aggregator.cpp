#include "cex/ExchangeSessionFactory.hpp"
#include "core/Aggregator.hpp"
#include "core/MarketUpdate.hpp"
#include "core/MarketDataLogger.hpp"
#include "core/MarketDataPublisher.hpp"
#include "core/MarketDataQueue.hpp"
#include "rpc/MarketDataService.hpp"

#include <boost/asio/ssl.hpp>
#include <grpcpp/grpcpp.h>
#include <nlohmann/json.hpp>

#include "marketdata.grpc.pb.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <utility>

int main(int argc, char ** argv)
{
    using namespace boost::asio;
    //grpc_init();
    io_context ioctx;
    ssl::context sslctx{ssl::context::tls_client};
    sslctx.set_default_verify_paths();
    MarketDataQueue queue;
    MarketDataPublisher publisher{queue};
    MarketDataLogger logger;
    Aggregator aggregator;
    ExchangeSessionFactory factory{ioctx, sslctx, publisher};
    MarketDataService service;

    auto makeSession = [&factory](Exchange exchange)
    {
        if (auto session = factory.make(exchange))
        {
            return session;
        }

        throw std::runtime_error("exchange could not be dealt with");
    };

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

    makeSession(Exchange::BINANCE)->run();
    makeSession(Exchange::BITMEX)->run();
    makeSession(Exchange::BYBIT)->run();
    makeSession(Exchange::COINBASE)->run();
    makeSession(Exchange::HYPERLIQUID)->run();
    //makeSession(Exchange::INTERNAL)->run();
    makeSession(Exchange::KRAKEN)->run();
    makeSession(Exchange::OKX)->run();
    ioctx.run();
    //grpc_shutdown();
    return 0;
}
