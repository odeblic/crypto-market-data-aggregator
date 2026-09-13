#pragma once

#include "core/MarketDataSink.hpp"
#include "ws/ClientSession.hpp"
#include "ws/SessionFactory.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

class WebsocketClient
{
public:
    WebsocketClient(MarketDataSink& sink)
    : ioctx(),
      sslctx(boost::asio::ssl::context::tls_client),
      factory{ioctx, sslctx, sink}
    {
        sslctx.set_default_verify_paths();
    }

    void connect(Exchange exchange)
    {
        auto session = factory.make(exchange);
        session->run();
    }

    void run()
    {
        ioctx.run();
    }

private:
    boost::asio::io_context ioctx;
    boost::asio::ssl::context sslctx;
    SessionFactory factory;
};
