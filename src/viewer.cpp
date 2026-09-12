#include "cex/ExchangeSessionFactory.hpp"
#include "core/MarketDataLogger.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl.hpp>

#include <iostream>

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        std::cerr << "An exchange must be specified:\n"
                  << "- Binance\n"
                  << "- BitMEX\n"
                  << "- Bybit\n"
                  << "- Coinbase\n"
                  << "- Hyperliquid\n"
                  << "- Internal\n"
                  << "- Kraken\n"
                  << "- OKX\n";
        return 1;
    }

    using namespace boost::asio;
    auto const exchange = argv[1];
    io_context ioctx;
    ssl::context sslctx{ssl::context::tls_client};
    sslctx.set_default_verify_paths();
    MarketDataLogger logger;
    ExchangeSessionFactory factory{ioctx, sslctx, logger};
    auto session = factory.make(exchange);

    if (!session)
    {
        std::cerr << "This exchange could not be dealt with or is not supported.\n";
        return 1;
    }

    session->run();
    ioctx.run();
    return 0;
}
