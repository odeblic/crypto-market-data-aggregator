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
        LOG_DEBUG("Host: " + config.host);
        LOG_DEBUG("Port: " + std::to_string(config.port));
        LOG_DEBUG("Path: " + config.path);
        LOG_DEBUG("Subs: " + config.subscription.dump());

        if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), config.host.c_str()))
        {
            boost::beast::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
            LOG_ERROR("SNI Error: " + ec.message());
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
            LOG_ERROR("Resolve failed: " + ec.message());
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
            LOG_ERROR("Connect failed: " + ec.message());
            return;
        }

        LOG_DEBUG("Connect succeeded (" + ep.address().to_string() + ":" + std::to_string(ep.port()) + ")");
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
            LOG_ERROR("SSL Handshake failed: " + ec.message());
            return;
        }

        LOG_DEBUG("SSL Handshake succeeded");
        boost::beast::get_lowest_layer(ws).expires_never();
        ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::client));
        ws.async_handshake(
            config.host, config.path,
            boost::beast::bind_front_handler(&ClientSession::onHandshake, shared_from_this()
        ));
    }

    void onHandshake(boost::beast::error_code ec)
    {
        if (ec)
        {
            LOG_ERROR("WS Handshake failed: " + ec.message());
            return;
        }

        LOG_DEBUG("WS Handshake succeeded");

        if (!config.subscription.empty())
        {
            LOG_DEBUG("Subscription is required");
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
            LOG_ERROR("Subscription failed: " + ec.message());
            return;
        }

        LOG_DEBUG("Subscription succeeded (" + std::to_string(byteCount) + " bytes transferred)");
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
            LOG_ERROR("Read failed: " + ec.message());
            return;
        }

        LOG_DEBUG("Read succeeded (" + std::to_string(byteCount) + " bytes transferred)");
        onMessage(boost::beast::buffers_to_string(buffer.data()));
        buffer.clear();
        doRead();
    }

    void onMessage(std::string const& str)
    {
        auto const msg = nlohmann::json::parse(str);
        LOG_DEBUG("message received: " + msg.dump(2));

        if (checkMessage(msg))
        {
            processMessage(msg);
        }
    }

    virtual auto checkMessage(nlohmann::json const& msg) const -> bool = 0;

    virtual void processMessage(nlohmann::json const& msg) = 0;

    boost::asio::ip::tcp::resolver resolver;
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>> ws;
    std::reference_wrapper<MarketDataSink> sink;
    ExchangeConfiguration config;
    boost::beast::flat_buffer buffer;
};
