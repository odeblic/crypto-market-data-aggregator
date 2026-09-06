#pragma once

#include "ExchangeConfiguration.hpp"
#include "MarketUpdate.hpp"
#include "Utils.hpp"

#include <boost/asio/strand.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <nlohmann/json.hpp>

#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

class ExchangeSession : public std::enable_shared_from_this<ExchangeSession>
{
public:
    explicit ExchangeSession(net::io_context& ioctx, ssl::context& sslctx, ExchangeConfiguration const& config)
    : resolver(net::make_strand(ioctx)), ws(net::make_strand(ioctx), sslctx), config(config)
    {
    }

    virtual ~ExchangeSession() = default;

    void run()
    {
        if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), config.host.c_str()))
        {
            beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
            std::cerr << "SNI Error: " << ec.message() << "\n";
            return;
        }

        resolver.async_resolve(
            config.host, std::to_string(config.port),
            beast::bind_front_handler(&ExchangeSession::onResolve, shared_from_this())
        );
    }

protected:
    void publish(MarketUpdate const& update)
    {
        std::cout << "ticker: \033[35m" << toString(update.ticker) << "\033[0m "
                  << "side: " << (update.side == Side::ASK ? "\033[31mASK\033[0m " : "\033[32mBID\033[0m ")
                  << "price: \033[33m" << std::fixed << std::setprecision(2) << update.price << "\033[0m "
                  << "quantity: \033[33m" << std::defaultfloat << update.quantity << "\033[0m "
                  << "source: \033[34m" << toString(update.exchange) << "\033[0m"
                  << std::endl;
    }

private:
    void onResolve(beast::error_code ec, tcp::resolver::results_type results)
    {
        if (ec)
        {
            std::cerr << "Resolve failed: " << ec.message() << "\n";
            return;
        }

        beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));
        beast::get_lowest_layer(ws).async_connect(
            results,
            beast::bind_front_handler(&ExchangeSession::onConnect, shared_from_this())
        );
    }

    void onConnect(beast::error_code ec, tcp::resolver::endpoint_type ep)
    {
        if (ec)
        {
            std::cerr << "Connect failed: " << ec.message() << "\n";
            return;
        }

        beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));

        ws.next_layer().async_handshake(
            ssl::stream_base::client,
            beast::bind_front_handler(&ExchangeSession::onHandshakeSSL, shared_from_this())
        );
    }

    void onHandshakeSSL(beast::error_code ec)
    {
        if (ec)
        {
            std::cerr << "SSL Handshake failed: " << ec.message() << "\n";
            return;
        }

        beast::get_lowest_layer(ws).expires_never();
        ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

        std::cout << "Host: " << config.host << "\n";
        std::cout << "Port: " << config.port << "\n";
        std::cout << "Path: " << config.path << "\n";
        std::cout << "Subs: " << config.subscription << "\n";

        ws.async_handshake(
            config.host, config.path,
            beast::bind_front_handler(&ExchangeSession::onHandshake, shared_from_this()
        ));
    }

    void onHandshake(beast::error_code ec)
    {
        if (ec)
        {
            std::cerr << "WS Handshake failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "Connected to live market data feed!\n";

        if (config.subscription.length() > 0)
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
            net::buffer(config.subscription),
            beast::bind_front_handler(&ExchangeSession::onSubscribe, shared_from_this()
        ));
    }

    void onSubscribe(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
        {
            std::cerr << "Write failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "Subscription message sent successfully. Waiting for incoming stream...\n";
        doRead();
    }

    void doRead()
    {
        ws.async_read(
            buffer,
            beast::bind_front_handler(&ExchangeSession::onRead, shared_from_this())
        );
    }

    void onRead(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
        {
            std::cerr << "Read error: " << ec.message() << "\n";
            return;
        }

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

    tcp::resolver resolver;
    websocket::stream<ssl::stream<beast::tcp_stream>> ws;
    ExchangeConfiguration config;
    beast::flat_buffer buffer;
};
