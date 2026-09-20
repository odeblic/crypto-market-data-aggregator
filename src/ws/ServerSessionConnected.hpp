#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

#include <iostream>
#include <memory>
#include <queue>
#include <utility>

class ServerSessionConnected : public std::enable_shared_from_this<ServerSessionConnected>
{
public:
    explicit ServerSessionConnected(
        boost::asio::ip::tcp::socket&& socket,
        boost::asio::ssl::context& ctx
    )
    : ws{std::move(socket), ctx},
      timer{ws.get_executor()}
    {
    }

    void run()
    {
        boost::asio::dispatch(
            ws.get_executor(),
            boost::beast::bind_front_handler(&ServerSessionConnected::onRun, shared_from_this())
        );
    }

    static inline auto extractTicker(std::string const target) -> std::string
    {
        std::string const prefix{"/marketdata/"};

        if (target.rfind(prefix, 0) == 0 && target.length() > prefix.length())
        {
            return target.substr(prefix.length());
        }

        return "";
    }

private:
    void onRun()
    {
        boost::beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));
        ws.next_layer().async_handshake(
            boost::asio::ssl::stream_base::server,
            boost::beast::bind_front_handler(&ServerSessionConnected::onHandshakeSSL, shared_from_this())
        );
    }

    void onHandshakeSSL(boost::beast::error_code const ec)
    {
        if (ec)
        {
            LOG_ERROR("SSL handshake failed: " + ec.message());
            return;
        }

        LOG_DEBUG("SSL handshake succeeded");
        boost::beast::get_lowest_layer(ws).expires_never();
        ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));
        ws.set_option(boost::beast::websocket::stream_base::decorator(
            [](boost::beast::websocket::response_type& res)
            {
                res.set(boost::beast::http::field::server, "MarketDataServer");
            })
        );
        ws.async_accept(boost::beast::bind_front_handler(&ServerSessionConnected::onHandshakeWS, shared_from_this()));
    }

    void onHandshakeWS(boost::beast::error_code const ec)
    {
        if (ec)
        {
            LOG_ERROR("WS Handshake failed: " + ec.message());
            return;
        }

        LOG_DEBUG("WS Handshake succeeded");
        schedulePublishing();
    }

    void generateMessages()
    {
        auto const ticker = "BTCUSDT";

        auto addMessage = [&](std::string side, double price, double quantity)
        {
            nlohmann::json const message = {
                {"ticker", ticker},
                {"side", side},
                {"price", price},
                {"quantity", quantity}
            };

            messages.push(message);
        };

        addMessage("ask", 78001.00, 4.0000);
        addMessage("ask", 78000.75, 3.0000);
        addMessage("ask", 78000.60, 2.0000);
        addMessage("ask", 78000.45, 1.0000);
        addMessage("bid", 78000.30, 1.0000);
        addMessage("bid", 78000.15, 2.0000);
        addMessage("bid", 78000.00, 3.0000);
    }

    void doPublish()
    {
        if (messages.empty())
        {
            LOG_DEBUG("No more messages in the queue");
            schedulePublishing();
            return;
        }

        auto const message = messages.front();
        ws.text(true);
        ws.async_write(
            boost::asio::buffer(message.dump()),
            boost::beast::bind_front_handler(&ServerSessionConnected::onPublish, shared_from_this())
        );
        messages.pop();
    }

    void onPublish(boost::beast::error_code ec, std::size_t byteCount)
    {
        if (ec)
        {
            LOG_ERROR("Publish failed: " + ec.message());
            return;
        }

        LOG_DEBUG("Publish succeeded (" + std::to_string(byteCount) + " bytes transferred)");
        doPublish();
    }

    void schedulePublishing()
    {
        timer.expires_after(std::chrono::milliseconds(1000));
        timer.async_wait(
            boost::beast::bind_front_handler(&ServerSessionConnected::onScheduleEvent, shared_from_this()));
    }

    void cancelPublishing()
    {
        boost::beast::error_code ec;
        timer.cancel(ec);

        if (ec)
        {
            LOG_ERROR("Stop timer failed: " + ec.message());
            return;
        }

        LOG_DEBUG("Stop timer succeeded");
    }

    void onScheduleEvent(boost::beast::error_code const ec)
    {
        if (ec)
        {
            LOG_ERROR("Schedule event failed: " + ec.message());
            return;
        }

        LOG_DEBUG("Schedule event succeeded");
        generateMessages();
        doPublish();
    }

    void doClose()
    {
        LOG_DEBUG("Closing...");
        ws.async_close(
            boost::beast::websocket::close_code::normal,
            boost::beast::bind_front_handler(&ServerSessionConnected::onClose, shared_from_this())
        );
    }

    void onClose(boost::beast::error_code const ec)
    {
        cancelPublishing();

        if (ec)
        {
            LOG_ERROR("Close failed: " + ec.message());
            return;
        }

        LOG_DEBUG("Close succeeded");
    }

    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>> ws;
    boost::beast::flat_buffer buffer;
    boost::asio::steady_timer timer;
    std::queue<nlohmann::json> messages;
};
