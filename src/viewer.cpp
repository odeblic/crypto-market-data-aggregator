#include "cex/ExchangeSessionFactory.hpp"

#include <boost/asio/ssl.hpp>

#include <iostream>
#include <stdexcept>

namespace net = boost::asio;

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        std::cerr << "An exchange must be specified:" << std::endl;
        std::cerr << "- Binance" << std::endl;
        std::cerr << "- BitMEX" << std::endl;
        std::cerr << "- Bybit" << std::endl;
        std::cerr << "- Coinbase" << std::endl;
        std::cerr << "- Kraken" << std::endl;
        std::cerr << "- OKX" << std::endl;
        return 1;
    }

    std::string const exchange(argv[1]);

    try
    {
        net::io_context ioctx;
        net::ssl::context sslctx{net::ssl::context::tlsv12_client};
        sslctx.set_default_verify_paths();
        ExchangeSessionFactory factory{ioctx, sslctx};
        auto session = factory.make(exchange);

        if (!session)
        {
            std::cerr << "This exchange could not be dealt with or is not supported." << std::endl;
            return 1;
        }

        session->run();
        ioctx.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
