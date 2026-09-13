#pragma once

#include "ws/ServerSessionListening.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>

#include <string_view>

class WebsocketServer
{
public:
    WebsocketServer(
        std::string_view host,
        unsigned short port,
        std::string_view certificateFile,
        std::string_view keyFile
    )
    : address(boost::asio::ip::make_address(host.data())),
      port(port),
      ioctx(),
      sslctx(boost::asio::ssl::context::tls_server)
    {
        sslctx.use_certificate_chain_file(certificateFile.data());
        sslctx.use_private_key_file(keyFile.data(), boost::asio::ssl::context::pem);
    }

    void run()
    {
        auto endpoint = boost::asio::ip::tcp::endpoint{address, port};
        std::make_shared<ServerSessionListening>(ioctx, sslctx, endpoint)->run();
        ioctx.run();
    }

private:
    boost::asio::ip::address address;
    boost::asio::ip::port_type port{0};
    boost::asio::io_context ioctx;
    boost::asio::ssl::context sslctx;
};
