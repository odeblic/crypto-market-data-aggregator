#include "cex/ExchangeSessionFactory.hpp"
#include "core/MarketUpdate.hpp"
#include "core/MarketDataLogger.hpp"
#include "core/MarketDataPublisher.hpp"
#include "core/MarketDataQueue.hpp"

#include <boost/asio/ssl.hpp>
#include <grpcpp/grpcpp.h>
#include <nlohmann/json.hpp>

#include "marketdata.grpc.pb.h"

#include <iostream>
#include <thread>

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
    ExchangeSessionFactory factory{ioctx, sslctx, publisher};

    auto makeSession = [&factory](Exchange exchange)
    {
        if (auto session = factory.make(exchange))
        {
            return session;
        }

        throw std::runtime_error("exchange could not be dealt with");
    };

    std::thread consumer([&]()
    {
        while (true)
        {
            MarketUpdate update;

            while (queue.pop(update))
            {
                logger.write(update);
            }
        }
    });

    makeSession(Exchange::BINANCE)->run();
    makeSession(Exchange::BITMEX)->run();
    makeSession(Exchange::BYBIT)->run();
    makeSession(Exchange::COINBASE)->run();
    makeSession(Exchange::KRAKEN)->run();
    makeSession(Exchange::OKX)->run();
    ioctx.run();
    consumer.join();
    //grpc_shutdown();
    return 0;
}
