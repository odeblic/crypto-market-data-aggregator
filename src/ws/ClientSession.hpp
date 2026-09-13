#pragma once

#include "cfg/ExchangeConfiguration.hpp"
#include "core/MarketDataSink.hpp"
#include "core/MarketUpdate.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <nlohmann/json.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <utility>

class ClientSession : public std::enable_shared_from_this<ClientSession>
{
public:
    explicit ClientSession(
        boost::asio::io_context& ioctx,
        boost::asio::ssl::context& sslctx,
        MarketDataSink& sink,
        ExchangeConfiguration config
    )
    : resolver(boost::asio::make_strand(ioctx)),
      ws(boost::asio::make_strand(ioctx), sslctx),
      sink(sink),
      config(std::move(config))
    {
    }

    virtual ~ClientSession() = default;

    void run()
    {
        if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), config.host.c_str()))
        {
            boost::beast::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
            std::cerr << "SNI Error: " << ec.message() << "\n";
            return;
        }

        resolver.async_resolve(
            config.host, std::to_string(config.port),
            boost::beast::bind_front_handler(&ClientSession::onResolve, shared_from_this())
        );
    }

protected:
    void publish(MarketUpdate const& update)
    {
        sink.get().write(update);
    }

private:
    void onResolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results)
    {
        if (ec)
        {
            std::cerr << "Resolve failed: " << ec.message() << "\n";
            return;
        }

        boost::beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));
        boost::beast::get_lowest_layer(ws).async_connect(
            results,
            boost::beast::bind_front_handler(&ClientSession::onConnect, shared_from_this())
        );
    }

    void onConnect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::endpoint_type ep)
    {
        if (ec)
        {
            std::cerr << "Connect failed: " << ec.message() << "\n";
            return;
        }

        boost::beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));

        ws.next_layer().async_handshake(
            boost::asio::ssl::stream_base::client,
            boost::beast::bind_front_handler(&ClientSession::onHandshakeSSL, shared_from_this())
        );
    }

    void onHandshakeSSL(boost::beast::error_code ec)
    {
        if (ec)
        {
            std::cerr << "SSL Handshake failed: " << ec.message() << "\n";
            return;
        }

        boost::beast::get_lowest_layer(ws).expires_never();
        ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::client));

        std::cout << "Host: " << config.host << "\n";
        std::cout << "Port: " << config.port << "\n";
        std::cout << "Path: " << config.path << "\n";
        std::cout << "Subs: " << config.subscription << "\n";

        ws.async_handshake(
            config.host, config.path,
            boost::beast::bind_front_handler(&ClientSession::onHandshake, shared_from_this()
        ));
    }

    void onHandshake(boost::beast::error_code ec)
    {
        if (ec)
        {
            std::cerr << "WS Handshake failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "Connected to live market data feed!\n";

        if (!config.subscription.empty())
        {
            std::cout << "Subscription is required.\n";
            doSubscribe();
        }
        else
        {
            doRead();
        }
    }

    void doSubscribe()
    {
        ws.async_write(
            boost::asio::buffer(config.subscription.dump()),
            boost::beast::bind_front_handler(&ClientSession::onSubscribe, shared_from_this()
        ));
    }

    void onSubscribe(boost::beast::error_code ec, std::size_t byteCount)
    {
        if (ec)
        {
            std::cerr << "Subscription failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "Subscription succeeded (" << byteCount << " bytes transferred)\n";
        doRead();
    }

    void doRead()
    {
        ws.async_read(
            buffer,
            boost::beast::bind_front_handler(&ClientSession::onRead, shared_from_this())
        );
    }

    void onRead(boost::beast::error_code ec, std::size_t byteCount)
    {
        if (ec)
        {
            std::cerr << "Read failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "Read succeeded (" << byteCount << " bytes transferred)\n";
        onMessage(boost::beast::buffers_to_string(buffer.data()));
        buffer.clear();
        doRead();
    }

    void onMessage(std::string const& str)
    {
        auto const msg = nlohmann::json::parse(str);

        displayMessage(msg);

        if (checkMessage(msg))
        {
            processMessage(msg);
        }
    }

    void displayMessage(nlohmann::json const& msg) const
    {
        std::cout << "\033[30;1m" << msg.dump(2) << "\033[0m\n\n";
    }

    virtual auto checkMessage(nlohmann::json const& msg) const -> bool
    {
        return false;
    }

    virtual void processMessage(nlohmann::json const& msg)
    {
    }

    boost::asio::ip::tcp::resolver resolver;
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>> ws;
    std::reference_wrapper<MarketDataSink> sink;
    ExchangeConfiguration config;
    boost::beast::flat_buffer buffer;
};
