#pragma once

#include "core/Display.hpp"
#include "ws/ServerSessionConnected.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>

#include <memory>
#include <utility>

class ServerSessionListening : public std::enable_shared_from_this<ServerSessionListening>
{
public:
    ServerSessionListening(
        boost::asio::io_context& ioctx,
        boost::asio::ssl::context& sslctx,
        boost::asio::ip::tcp::endpoint const endpoint
    )
    : ioctx{ioctx},
      sslctx{sslctx},
      acceptor{boost::asio::make_strand(ioctx)}
    {
        boost::beast::error_code ec;
        acceptor.open(endpoint.protocol(), ec);

        if (ec)
        {
            LOG_ERROR("Socket creation failed: " + ec.message());
            return;
        }

        LOG_DEBUG("Socket creation succeeded");
        acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);

        if (ec)
        {
            LOG_ERROR("Set option failed: " + ec.message());
            return;
        }

        LOG_DEBUG("Set option succeeded");
        acceptor.bind(endpoint, ec);

        if (ec)
        {
            LOG_ERROR("Bind failed: " + ec.message());
            return;
        }

        LOG_DEBUG("Bind succeeded");
        acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);

        if (ec)
        {
            LOG_ERROR("Listen failed: " + ec.message());
            return;
        }

        LOG_DEBUG("Listen succeeded");
    }

    void run()
    {
        doAccept();
    }

private:
    void doAccept()
    {
        acceptor.async_accept(
            boost::asio::make_strand(ioctx),
            boost::beast::bind_front_handler(&ServerSessionListening::onAccept, shared_from_this())
        );
    }

    void onAccept(boost::beast::error_code const ec, boost::asio::ip::tcp::socket socket)
    {
        if (!ec)
        {
            std::make_shared<ServerSessionConnected>(std::move(socket), sslctx)->run();
        }

        doAccept();
    }

    boost::asio::io_context& ioctx;
    boost::asio::ssl::context& sslctx;
    boost::asio::ip::tcp::acceptor acceptor;
};
