#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

#include <iostream>
#include <memory>
#include <queue>
#include <string>
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

    inline auto getPath() const -> std::string
    {
        return std::string(req.target());
    }

    inline auto getTicker() const -> std::string
    {
        auto const path = getPath();
        std::string_view prefix{"/marketdata/"};

        if (path.rfind(prefix, 0) == 0 && path.length() > prefix.length())
        {
            return boost::to_upper_copy(path.substr(prefix.length()));
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
        doRead();
    }

    void doRead()
    {
        boost::beast::http::async_read(
            ws.next_layer(),
            buffer,
            req,
            boost::beast::bind_front_handler(&ServerSessionConnected::onRead, shared_from_this()));
    }

    void onRead(boost::beast::error_code ec, std::size_t byteCount)
    {
        boost::ignore_unused(byteCount);

        if (ec)
        {
            LOG_ERROR("HTTP read failed: " + ec.message());
            return;
        }

        LOG_DEBUG("HTTP read succeeded");

        if(boost::beast::websocket::is_upgrade(req))
        {
            auto const path = getPath();

            if (!path.starts_with("/marketdata/"))
            {
                LOG_ERROR("Bad path requested by WS client: " + path);
                namespace http = boost::beast::http;
                http::response<http::string_body> res{http::status::not_found, req.version()};
                res.set(http::field::server, "InternalExchange");
                res.set(http::field::content_type, "text/plain");
                res.keep_alive(false);
                res.body() = "The requested path is not a valid WebSocket endpoint.\n";
                res.prepare_payload();
                http::write(ws.next_layer(), res, ec);

                if (ec)
                {
                    LOG_ERROR("HTTP write failed: " + ec.message());
                }
                else
                {
                    LOG_DEBUG("HTTP write succeeded");
                }

                return;
            }

            LOG_DEBUG("Path requested by WS client: " + path);
            auto const ticker = getTicker();
            LOG_INFO("Subscribed ticker: " + ticker);

            ws.async_accept(
                req,
                boost::beast::bind_front_handler(&ServerSessionConnected::onHandshakeWS, shared_from_this()));
        }
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
        auto addMessage = [this](std::string side, double price, double quantity)
        {
            auto const ticker = getTicker();

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
    boost::beast::http::request<boost::beast::http::string_body> req;
    boost::beast::flat_buffer buffer;
    boost::asio::steady_timer timer;
    std::queue<nlohmann::json> messages;
};
