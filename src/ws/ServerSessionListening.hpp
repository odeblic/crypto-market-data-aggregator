#pragma once

#include "ws/ServerSessionConnected.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>

#include <iostream>
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
            std::cerr << "Socket creation failed: " << ec.message() << "\n";
            return;
        }

        acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);

        if (ec)
        {
            std::cerr << "Set option failed: " << ec.message() << "\n";
            return;
        }

        acceptor.bind(endpoint, ec);

        if (ec)
        {
            std::cerr << "Bind failed: " << ec.message() << "\n";
            return;
        }

        acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);

        if (ec)
        {
            std::cerr << "Listen failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "Listen OK\n";
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

        std::cout << "Accepting...\n";
        doAccept();
    }

    boost::asio::io_context& ioctx;
    boost::asio::ssl::context& sslctx;
    boost::asio::ip::tcp::acceptor acceptor;
};
